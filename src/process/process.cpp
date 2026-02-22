//
// Created by german on 24/11/25.
//
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>

#include "../../include/common/common.h"
#include "../../include/process/process.h"
#include "../../include/common/errors.h"
#include "../../include/process/process_types.h"

using namespace std;

namespace myProc {
    //Constructors
    Process::Process(const string &processName, const uint64_t systemUpTime, const std::shared_ptr<spdlog::logger>& logger) {
        this->console = logger;
        console->info("Creating new process with pid: {}", processName);
        readStatFile(processName);
        readStatusFile(processName);
        calculateMemory();
        calculateCPU(systemUpTime);
    }

    //Private methods
    void Process::readStatFile(const string &processNumber) {
        try {
            console->info("Reading stat file");
            //Check error
            string statFile = commonLib::getStatPath(processNumber);
            //check errors
            if (statFile.empty() || !statFile.starts_with('/')) {
                console->error("File path not found!");
                throw ProcessFileError("File path not correct");
            }

            ifstream pidFile(statFile);
            string line;
            if (!pidFile.is_open()) {
                console->error("File found - but could not be open");
                throw ProcessFileError("Error opening file");
            }
            //get line and loop with spaces
            getline(pidFile, line);
            unordered_map<string, string> processMap = parseStatFile(line);

            //we iterate over the fields
            for (size_t i = 0; i < types::LINE_FIELD_COUNT; ++i) {
                //fieldData with function meta
                const auto &meta = types::LINE_FIELDS[i];
                // Look up the extracted value with iterator
                if (auto it = processMap.find(meta.name); it != processMap.end()) {
                    meta.setter(*this, it->second); // Apply the setter lambda
                }
            }
            pidFile.close();
        } catch (ProcessError &e) {
            console->error("Process error: {}", e.what());
            throw ProcessError("Process error"); //TODO: maybe this should be change to capture all possiblea exceptions
        }
    }

    void Process::readStatusFile(const string &processNumber) {
        try {
            console->info("Reading status file");
            string statusFile = commonLib::getStatusPath(processNumber);
            //check errors
            if (statusFile.empty() || !statusFile.starts_with('/')) {
                console->error("File path not found!");
                throw ProcessFileError("File path not correct");
            }
            ifstream pidStatusFile(statusFile);
            if (!pidStatusFile.is_open()) {
                console->error("File found - but could not be open");
                throw ProcessFileError("Error opening file");
            }
            unordered_map<string, string> statusMap = parseStatusFile(pidStatusFile);

            //Extract values from map
            //we iterate over the fields
            for (size_t i = 0; i < types::COLUM_FIELD_COUNT; ++i) {
                //fieldData with function meta
                const auto &meta = types::COLUM_FIELDS[i];
                // Look up the extracted value with iterator
                if (auto it = statusMap.find(meta.name); it != statusMap.end()) {
                    meta.setter(*this, it->second); // Apply the setter lambda
                }
            }

            pidStatusFile.close();
        } catch (ProcessError &e) {
            console->error("Process error: {}", e.what());
            throw ProcessError("Process error"); //TODO: maybe this should be change to capture all possiblea exceptions
        }
    }

    unordered_map<string, string> Process::parseStatFile(const string &fileLine) {
        console->info("Parsing stat file");
        if (fileLine.empty()) {
            console->error("Empty line provided");
            throw ProcessReadError("Empty line");
        }

        //get stream from line and parse it
        stringstream ss(fileLine);
        unordered_map<int, string> allValues;
        unordered_map<string, string> processesMap;
        //make a for to extract all values
        for (int pos = 1; ss >> allValues[pos]; ++pos);

        //save only values we want
        for (size_t i = 0; i < types::LINE_FIELD_COUNT; ++i) {
            const auto &fieldData = types::LINE_FIELDS[i];
            processesMap[fieldData.name] = allValues[fieldData.pos];
        }
        return processesMap;
    }

    unordered_map<string, string> Process::parseStatusFile(ifstream &file) {
        console->info("Parsing status file");
        unordered_map<string, string> allValues;
        unordered_map<string, string> statusData;
        //loop all lines of file
        for (string line; getline(file, line);) {
            //for each line split by \t
            vector<string> lineValues = commonLib::splitStringByChar(line, '\t');
            //some values from status file are in several columns, ignore those for the moment
            if (lineValues.size() != 2) {
                console->debug("Skipping line - multiple values!");
                continue;
            }
            //remove ':' from name values
            lineValues.at(0).pop_back();
            allValues[lineValues.at(0)] = lineValues.at(1); //save values from lines into map
        }
        //now we loop the static list searching for out values
        for (size_t i = 0; i < types::COLUM_FIELD_COUNT; ++i) {
            const auto &fieldData = types::COLUM_FIELDS[i];
            //if empty we didn't find the parameters we were looking for
            if (allValues[fieldData.name].empty()) {
                console->warn("Field status property: {} not found in status field", fieldData.name);
                continue;
            }
            statusData[fieldData.name] = allValues[fieldData.name];
        }
        return statusData;
    }

    //public methods
    void Process::refresh(const uint64_t &systemUpTime) {
        try {
            console->info("REFRESHING - {}", pid);
            readStatFile(pid);
            readStatusFile(pid);
            calculateCPU(systemUpTime);
            calculateMemory();
        } catch (ProcessError &err) {
            console->error("Error refreshing");
            throw ProcessResourcesError(err.what());
        }
    }

    void Process::calculateCPU(const uint64_t &systemUpTime) {
        //TODO: Implement error catching here
        console->info("Calculating CPU usage for: {}", getPid());
        const long ticks = sysconf(_SC_CLK_TCK); //Clock ticks per second (usually 100 on Linux)
        const long totalTime = getUtime() + getsTime(); //total process time
        //Convert process total time to seconds
        const double seconds = totalTime / ticks; //this should stay as double
        //how long the process has been running in sec
        const double processUpTime = systemUpTime - (getStartTime() / ticks);

        const int numCores = sysconf(_SC_NPROCESSORS_ONLN);
        double cpuUsage = (seconds / processUpTime) / numCores * 100;
        if (cpuUsage < 0) {
            console->warn("CPU usage is 0 - Error calculating usage");
            cpuUsage = 0.;
        }
        set_cpu_usage(cpuUsage);
    }

    void Process::calculateMemory() {
        console->info("Calculating MiB memory usage");
        set_vm_rss_mib(static_cast<double>(getVmRSS()) / 1024);
        set_vm_size_mib(static_cast<double>(getVmSize()) / 1024);
    }

    long Process::getElapsedSeconds(const uint64_t systemUpTime) const {
        const long ticks = sysconf(_SC_CLK_TCK);
        return (systemUpTime - (getStartTime() / ticks));
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

    string Process::getName() const {
        return this->name;
    }

    void Process::setName(const string &name) {
        this->name = name;
    }

    string Process::getState() const {
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

    double Process::vm_rss_mib() const {
        return VmRSSGiB;
    }

    void Process::set_vm_rss_mib(const double &vm_rss_gib) {
        VmRSSGiB = vm_rss_gib;
    }

    double Process::vm_size_mib() const {
        return VmSizeGiB;
    }

    void Process::set_vm_size_mib(const double &vm_size_gib) {
        VmSizeGiB = vm_size_gib;
    }

    double Process::get_cpu_usage() const {
        return cpu_usage;
    }

    void Process::set_cpu_usage(const double &cpu_usage) {
        this->cpu_usage = cpu_usage;
    }

}
