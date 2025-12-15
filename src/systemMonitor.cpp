//
// Created by german on 8/12/25.
//
#include <string>
#include <spdlog/spdlog.h>
#include <fstream>

#include "systemMonitor.h"
#include "process_types.h"
#include "common.h"
#include "errors.h"

using namespace std;

namespace myProc {
    SystemMonitor::SystemMonitor () {
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

            //Call the setters here
            //TODO:

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
            parseStatFile(statFile);

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
                string val = to_string(it->second);//we need to cast this value
                meta.setter(*this, val); // Apply the setter lambda
            }
        }
    }

    //Save logic as status file
    std::unordered_map<string, string> SystemMonitor::parseMemInfo(ifstream &memFile) {
        spdlog::info("Parsing meminfo file");
        unordered_map<string, string> memDataMap;
        unordered_map<string, string> allValues;
        //loop all lines of the file
        for (string line; getline(memFile, line);) {
            //Split by '\t'
            vector<string> lineValues = commonLib::splitStringByChar(line, ':');
            //remove empty whitespaces
            string &lineValue = lineValues.at(1);
            lineValue.erase(ranges::remove_if(lineValue, [](const unsigned char c){ return std::isspace(c); }).begin(), lineValue.end());
            allValues[lineValues.at(0)] = lineValues.at(1);
        }
        //now loop the static list searching for the values in the map
        for (size_t i = 0; i < types::SYSTEM_FIELD_COUNT; ++i) {
            const auto &fielData = types::SYSTEM_FIELDS[i];
            //Check for empty value
            if (allValues[fielData.name].empty()) {
                spdlog::warn("Field system property: {} not found in meminfo file", fielData.name);
                continue;
            }
            memDataMap[fielData.name] = allValues[fielData.name];
        }

        return memDataMap;
    }

    //TODO:
    std::unordered_map<string, string> SystemMonitor::parseStatFile(ifstream &statFile) {
        spdlog::info("Parsing stat file");
        unordered_map<string, string> statDataMap;
        return  statDataMap;
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