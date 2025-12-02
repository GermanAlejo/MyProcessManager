//
// Created by german on 24/11/25.
//
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>

#include "../include/common.h"
#include "../include/process.h"
#include "../include/errors.h"
#include "../include/process_types.h"

using namespace std;

namespace myProc {
    //Constructors
    Process::Process(const string &processName) {
        spdlog::info("Creating new process with pid: {}", processName);
        readStatFile(processName);
        readStatusFile(processName);
    }

    //Private methods
    void Process::readStatFile(const string &processNumber) {
        try {
            spdlog::info("Reading stat file");
            //Check error
            string statFile = commonLib::getStatPath(processNumber);
            //check errors
            if (statFile.empty() || !statFile.starts_with('/')) {
                spdlog::error("File path not found!");
                throw ProcessFileError("File path not correct");
            }

            ifstream pidFile(statFile);
            string line;
            if (!pidFile.is_open()) {
                spdlog::error("File found - but could not be open");
                throw ProcessFileError("Error opening file");
            }
            //get line and loop with spaces
            getline(pidFile, line);
            unordered_map<string, string> processMap = parseStatFile(line);

            //we iterate over the fields
            for (size_t i = 0; i < types::FIELD_COUNT; ++i) {
                //fieldData with function meta
                const auto &meta = types::FIELDS[i];
                // Look up the extracted value with iterator
                if (auto it = processMap.find(meta.name); it != processMap.end()) {
                    meta.setter(*this, it->second); // Apply the setter lambda
                }
            }

            pidFile.close();
        } catch (ProcessError &e) {
            spdlog::error("Process error: {}", e.what());
            throw ProcessError("Process error"); //TODO: maybe this should be change to capture all possiblea exceptions
        }
    }

    void Process::readStatusFile(const string &processNumber) {
        try {
            spdlog::info("Reading status file");
            string statusFile = commonLib::getStatusPath(processNumber);
            //check errors
            if (statusFile.empty() || !statusFile.starts_with('/')) {
                spdlog::error("File path not found!");
                throw ProcessFileError("File path not correct");
            }
            ifstream pidStatusFile(statusFile);
            if (!pidStatusFile.is_open()) {
                spdlog::error("File found - but could not be open");
                throw ProcessFileError("Error opening file");
            }
            unordered_map<string, string> statusMap = parseStatusFile(pidStatusFile);

            //Extract values from map
            //we iterate over the fields
            for (size_t i = 0; i < types::STATUS_FIELD_COUNT; ++i) {
                //fieldData with function meta
                const auto &meta = types::STATUS_FIELDS[i];
                // Look up the extracted value with iterator
                if (auto it = statusMap.find(meta.name); it != statusMap.end()) {
                    meta.setter(*this, it->second); // Apply the setter lambda
                }
            }

            pidStatusFile.close();
        } catch (ProcessError &e) {
            spdlog::error("Process error: {}", e.what());
            throw ProcessError("Process error"); //TODO: maybe this should be change to capture all possiblea exceptions
        }
    }

    unordered_map<string, string> Process::parseStatFile(const string &fileLine) {
        spdlog::info("Parsing stat file");
        if (fileLine.empty()) {
            spdlog::error("Empty line provided");
            throw ProcessReadError("Empty line");
        }

        //get stream from line and parse it
        stringstream ss(fileLine);
        unordered_map<int, string> allValues;
        unordered_map<string, string> processesMap;
        //make a for to extract all values
        for (int pos = 1; ss >> allValues[pos]; ++pos);

        //save only values we want
        for (size_t i = 0; i < types::FIELD_COUNT; ++i) {
            const auto &fieldData = types::FIELDS[i];
            processesMap[fieldData.name] = allValues[fieldData.pos];
        }

        return processesMap;
    }

    unordered_map<string, string> Process::parseStatusFile(ifstream &file) {
        spdlog::info("Parsing status file");
        unordered_map<string, string> allValues;
        unordered_map<string, string> statusData;
        //loop all lines of file
        for (string line; getline(file, line);) {
            //for each line split by \t
            vector<string> lineValues = commonLib::splitStringByChar(line, '\t');
            //some values from status file are in several columns, ignore those for the moment
            if (lineValues.size() != 2) {
                spdlog::debug("Skipping line - multiple values!");
                continue;
            }
            //remove ':' from name values
            lineValues.at(0).pop_back();
            allValues[lineValues.at(0)] = lineValues.at(1); //save values from lines into map
        }
        //now we loop the static list searching for out values
        for (size_t i = 0; i < types::STATUS_FIELD_COUNT; ++i) {
            const auto &fieldData = types::STATUS_FIELDS[i];
            statusData[fieldData.name] = allValues[fieldData.name];
        }
        return statusData;
    }

    //public methods
    void Process::refresh(const string &pidFileName) {
        spdlog::info("REFRESHING - {}", pid);
        readStatFile(pidFileName);
        double cpu = calculateCPU();
        double ram = calculateMemory();
        //TODO: replace this
        spdlog::info("CPU: {} | Memory: {} MB", cpu, ram);
    }

    double Process::calculateCPU() const {
        //TODO: Implement error catching here
        spdlog::info("Calculating CPU usage for: {}", getPid());
        unordered_map<string_view, uint64_t> timeMap = commonLib::getUptimeData();
        const uint64_t systemUpTime = timeMap[commonLib::TOTAL_TIME_KEY]; //total up time in seconds
        long ticks = sysconf(_SC_CLK_TCK); //Clock ticks per second (usually 100 on Linux)
        long totalTime = getUtime() + getsTime(); //total process time
        //Convert process total time to seconds
        double seconds = totalTime / ticks; //this should stay as double
        //how long the process has been running
        double processUpTime = systemUpTime - (getStartTime() / ticks);
        double cpuUsage = seconds / processUpTime;
        return cpuUsage;
    }

    double Process::calculateMemory() const {
        return getVmRSS() / 1024.0;
    }

    void Process::print() const {
        cout << "PID: \t\t\t" << pid << "\n" <<
                "NAME: \t\t\t" << name << "\n" <<
                "STATE: \t\t\t" << state << "\n" <<
                "uTime: \t\t\t" << utime << "\n" <<
                "sTime: \t\t\t" << stime << "\n" <<
                "start time: \t\t\t" << startTime << "\n" <<
                "VmRSS: \t\t" << VmRSS << "\n" <<
                "VmSize: \t\t" << VmSize << "\n";
    }

    //getters & setters
    string Process::getPid() const {
        return this->pid;
    }

    void Process::setPid(const string &pid) {
        this->pid = pid;
    }

    string Process::getName() {
        return this->name;
    }

    void Process::setName(const string &name) {
        this->name = name;
    }

    string Process::getState() {
        return this->state;
    }

    void Process::setState(const string &state) {
        this->state = state;
    }

    unsigned long long Process::getsTime() const {
        return stoll(this->stime);
    }

    void Process::setStime(const unsigned long long &stime) {
        this->stime = to_string(stime);
    }

    unsigned long long Process::getUtime() const {
        return stoll(this->utime);
    }

    void Process::setUtime(const unsigned long long &utime) {
        this->utime = to_string(utime);
    }

    uint64_t Process::getStartTime() const {
        return stoull(this->startTime);
    }

    void Process::setStartTime(const uint64_t &startTime) {
        this->startTime = to_string(startTime);
    }

    unsigned long Process::getVmRSS() const {
        return stoul(this->VmRSS);
    }

    void Process::setVmRSS(const unsigned long &vmRss) {
        this->VmRSS = to_string(vmRss);
    }

    unsigned long Process::getVmSize() const {
        return stoul(this->VmSize);
    }

    void Process::setVmSize(const unsigned long &VmSize) {
        this->VmSize = to_string(VmSize);
    }
}
