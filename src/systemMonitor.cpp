//
// Created by german on 8/12/25.
//
#include <string>
#include <spdlog/spdlog.h>
#include <fstream>

#include "systemMonitor.h"
#include "process_types.h"
#include "cpuSnapShot.h"
#include "common.h"
#include "errors.h"

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

            //Now we calculate values
            //calculate used memory
            set_used_ram(total_ram() - available_ram());

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
            //TODO: Implement file read into snap shot
            parseStatFile(statFile);

            //Wait 1 sec

            //Check for previous value, if not empty replace
            //Read file second time

            //Compare snapshots and calculate cpu% usage

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

    //TODO: implement read of stat, this function sould return a snapshot of the file with relevant data
    CpuSnapShot SystemMonitor::parseStatFile(ifstream &statFile) {
        spdlog::info("Parsing stat file");
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

    //TODO:
    void SystemMonitor::refresh() {
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
