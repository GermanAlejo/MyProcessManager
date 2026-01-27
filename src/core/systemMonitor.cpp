//
// Created by german on 8/12/25.
//
#include <string>
#include <spdlog/spdlog.h>
#include <fstream>

#include "../../include/core/systemMonitor.h"
#include "../../include/process/process_types.h"
#include "../../include/core/cpuSnapShot.h"
#include "../../include/common/common.h"
#include "../../include/common/errors.h"

using namespace std;

namespace myProc {
    SystemMonitor::SystemMonitor() {
        spdlog::info("Creating system monitor");
        readMemInfo();
        readUpTime();
        readStatFile();
    }

    void SystemMonitor::readMemInfo() {
        try {
            spdlog::info("Reading meminfo file");
            string memInfoFilePath = commonLib::getMemInfoPath();
            if (memInfoFilePath.empty() || !memInfoFilePath.starts_with('/')) {
                spdlog::error("File path not valid");
                throw ProcessFileError("File path mem not valid");
            }
            ifstream memFile(memInfoFilePath);
            if (!memFile.is_open()) {
                spdlog::error("File could not be opened");
                throw ProcessFileError("Mem file cannot be opened");
            }
            unordered_map<string, string> memFileData = parseMemInfo(memFile);

            //get attribute values
            for (size_t i = 0; i < types::SYSTEM_FIELD_COUNT; ++i) {
                //fieldData with function meta
                const auto &meta = types::SYSTEM_FIELDS[i];
                // Look up the extracted value with iterator
                if (auto it = memFileData.find(meta.name); it != memFileData.end()) {
                    meta.setter(*this, it->second); // Apply the setter lambda
                }
            }

            //calculate used memory
            calculateRamFinalValues();

            memFile.close();
        } catch (ProcessError &err) {
            spdlog::error("Process error: {}", err.what());
            throw ProcessFileError("Error reading file in System monitor");
        }
    }

    void SystemMonitor::readStatFile() {
        try {
            spdlog::info("Reading stat file");
            string statFilePath = commonLib::getStatPath();
            if (statFilePath.empty() || !statFilePath.starts_with('/')) {
                spdlog::error("File path not valid");
                throw ProcessFileError("Stat file path not valid");
            }
            ifstream statFile(statFilePath);
            if (!statFile.is_open()) {
                spdlog::error("File could not be opened");
                throw ProcessFileError("Stat file cannot be opened");
            }
            //Parse file here
            if (!this->lastCpuRead.has_value()) {
                this->lastCpuRead = parseStatFile(statFile);
            }
            //Wait 1 sec
            commonLib::waitSomeSeconds(1);

            //Read file second time
            CpuSnapShot new_snap_shot = parseStatFile(statFile);

            //Compare snapshots and calculate cpu% usage
            calculateTotalCPU(new_snap_shot);

            statFile.close();
        } catch (ProcessError &err) {
            spdlog::error("Process error: {}", err.what());
            throw ProcessFileError("Error reading file in System monitor");
        }
    }

    void SystemMonitor::readUpTime() {
        spdlog::info("Reading uptime file");
        unordered_map<string_view, uint64_t> uptimeDataMap = commonLib::getUptimeData();
        for (size_t i = 0; i < types::SYSTEM_FIELD_COUNT; ++i) {
            //fieldData with function meta
            const auto &meta = types::SYSTEM_FIELDS[i];
            // Look up the extracted value with iterator
            if (auto it = uptimeDataMap.find(meta.name); it != uptimeDataMap.end()) {
                string val = to_string(it->second); //we need to cast this value
                meta.setter(*this, val); // Apply the setter lambda
            }
        }
    }

    //Save logic as status file
    unordered_map<string, string> SystemMonitor::parseMemInfo(ifstream &memFile) {
        spdlog::info("Parsing meminfo file");
        unordered_map<string, string> memDataMap;
        unordered_map<string, string> allValues;
        //loop all lines of the file
        for (string line; getline(memFile, line);) {
            //Split by '\t'
            vector<string> lineValues = commonLib::splitStringByChar(line, ':');
            //remove empty whitespaces
            string &lineValue = lineValues.at(1);
            lineValue.erase(ranges::remove_if(
                                lineValue, [](const unsigned char c) {
                                    return std::isspace(c);
                                }).begin(), lineValue.end());
            allValues[lineValues.at(0)] = lineValues.at(1);
        }
        //now loop the static list searching for the values in the map
        for (size_t i = 0; i < types::SYSTEM_FIELD_COUNT; ++i) {
            const auto &fieldData = types::SYSTEM_FIELDS[i];
            //Check for empty value
            if (allValues[fieldData.name].empty()) {
                spdlog::warn("Field system property: {} not found in meminfo file", fieldData.name);
                continue;
            }
            memDataMap[fieldData.name] = allValues[fieldData.name];
        }

        return memDataMap;
    }

    CpuSnapShot SystemMonitor::parseStatFile(ifstream &statFile) {
        spdlog::info("Parsing stat file");
        //clear EOF error flags
        statFile.clear();
        //move read pointer to start of file
        statFile.seekg(0);
        string line;
        getline(statFile, line);
        if (line.empty()) {
            spdlog::error("Empty line provided");
            throw ProcessReadError("Empty line read in file");
        }

        stringstream ss(line);
        unordered_map<int, string> allValues;
        unordered_map<string, string> cpuMap;
        //make a for to extract all values
        for (int pos = 0; ss >> allValues[pos]; ++pos); //we should have 11 values

        //save only values we want
        for (size_t i = 0; i < types::CPU_FIELD_COUNT; ++i) {
            const auto &fieldData = types::CPU_FIELDS[i];
            //check for cpu value and skip
            if (fieldData.name == "cpu") {
                spdlog::warn("Skipping cpu string value");
                continue;
            }
            cpuMap[fieldData.name] = allValues[fieldData.pos];
        }
        //Create snapshot
        CpuSnapShot snapshot(cpuMap);

        return snapshot;
    }

    void SystemMonitor::calculateRamFinalValues() {
        spdlog::info("Calculate RAM usage percentage");
        const unsigned long ramUsage = total_ram() - available_ram();
        const double finalRam = static_cast<double>(ramUsage) / static_cast<double>(total_ram());
        //Used ram does not work (expects a long not a double)
        set_used_ram(ramUsage);
        set_used_ram_percentage(finalRam * 100);
        set_used_ram_gib(ramUsage / (1024 * 1024));
        //Set total ram as gib
        set_total_ram_gib(total_ram() / (1024 * 1024));
    }

    void SystemMonitor::calculateTotalCPU(const CpuSnapShot &newSnapShot) {
        spdlog::info("Calculating CPU usage");
        //Subtract previous from current ones
        const unsigned long prevIdle = lastCpuRead->idle + lastCpuRead->iowait;
        const unsigned long idle = newSnapShot.idle + newSnapShot.iowait;

        const unsigned long prevNonIdle = lastCpuRead->user + lastCpuRead->nice + lastCpuRead->system + lastCpuRead->softirq +
            lastCpuRead->irq + lastCpuRead->steal;
        const unsigned long nonIdle = newSnapShot.user + newSnapShot.nice + newSnapShot.system + newSnapShot.irq +
            newSnapShot.softirq + newSnapShot.steal;

        const unsigned long prevTotal = prevIdle + prevNonIdle;
        const unsigned long total = idle + nonIdle;

        // Actual value minus previous one
        const double totald = static_cast<double>(total) - static_cast<double>(prevTotal);
        const double idled = static_cast<double>(idle) - static_cast<double>(prevIdle);

        // Calculate percentage
        double cpu_percentage = (totald - idled) / totald;
        if (cpu_percentage < 0) {
            spdlog::warn("CPU usage is 0 - Error calculating usage");
            cpu_percentage = 0.;
        }
        cpu_percentage *= 100;
        set_total_cpu(round(cpu_percentage * 100.) / 100.); //Round with 2 digits
    }


    void SystemMonitor::refresh() {
        spdlog::info("Refreshing System Monitor");
        readMemInfo();
        readUpTime();
        readStatFile();
    }

    [[nodiscard]] int SystemMonitor::total_processes() const {
        return totalProcesses;
    }

    void SystemMonitor::set_total_processes(const int total_processes) {
        totalProcesses = total_processes;
    }

    [[nodiscard]] unsigned long SystemMonitor::total_ram() const {
        return totalRam;
    }

    void SystemMonitor::set_total_ram(const unsigned long total_ram) {
        totalRam = total_ram;
    }

    [[nodiscard]] unsigned long SystemMonitor::used_ram() const {
        return usedRam;
    }

    void SystemMonitor::set_used_ram(const unsigned long used_ram) {
        usedRam = used_ram;
    }

    unsigned long SystemMonitor::available_ram() const {
        return availableRam;
    }

    void SystemMonitor::set_available_ram(const unsigned long available_ram) {
        availableRam = available_ram;
    }

    [[nodiscard]] double SystemMonitor::used_ram_gib() const {
        return usedRamGiB;
    }

    void SystemMonitor::set_used_ram_gib(double used_ram_gib) {
        usedRamGiB = used_ram_gib;
    }

    [[nodiscard]] double SystemMonitor::used_ram_percentage() const {
        return usedRamPercentage;
    }

    void SystemMonitor::set_used_ram_percentage(double used_ram_percentage) {
        usedRamPercentage = used_ram_percentage;
    }

    [[nodiscard]] double SystemMonitor::total_ram_gib() const {
        return totalRamGiB;
    }

    void SystemMonitor::set_total_ram_gib(double total_ram_gi_b) {
        totalRamGiB = total_ram_gi_b;
    }

    [[nodiscard]] double SystemMonitor::total_cpu() const {
        return totalCPU;
    }

    void SystemMonitor::set_total_cpu(double total_cpu) {
        totalCPU = total_cpu;
    }

    [[nodiscard]] uint64_t SystemMonitor::get_uptime() const {
        return uptime;
    }

    void SystemMonitor::set_uptime(const uint64_t uptime) {
        this->uptime = uptime;
    }
} // myProc
