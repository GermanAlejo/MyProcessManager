//
// Created by german on 24/11/25.
//
#include <map>
#include <ranges>
#include <fstream>
#include <iostream>
#include "../include/common.h"
#include "../include/process.h"
#include "../include/errors.h"
#include "../include/process_types.h"

using namespace std;

namespace myProc {
    //Constructors
    Process::Process(const string &processName) {
        readStatFile(processName);
        readStatusFile(processName);
    }

    //Private methods
    void Process::readStatFile(const string &processNumber) {
        try {
            //Check error
            string statFile = commonLib::getStatPath(processNumber);
            //check errors
            if (statFile.empty() || !statFile.starts_with('/')) {
                perror("File path is not correct");
                throw ProcessFileError("File path not correct");
            }

            ifstream pidFile(statFile);
            string line;
            if (!pidFile.is_open()) {
                perror("Error reading file");
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
            std::cerr << "Process error: " << e.what() << "\n";
            throw ProcessError("Process error"); //TODO: maybe this should be change to capture all possiblea exceptions
        }
    }

    void Process::readStatusFile(const string &processNumber) {
        try {
            string statusFile = commonLib::getStatusPath(processNumber);
            //check errors
            if (statusFile.empty() || !statusFile.starts_with('/')) {
                perror("File path is not correct");
                throw ProcessFileError("File path not correct");
            }
            ifstream pidStatusFile(statusFile);
            if (!pidStatusFile.is_open()) {
                perror("Error reading file");
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
            std::cerr << "Process error: " << e.what() << "\n";
            throw ProcessError("Process error"); //TODO: maybe this should be change to capture all possiblea exceptions
        }
    }

    unordered_map<string, string> Process::parseStatFile(const string &fileLine) {
        if (fileLine.empty()) {
            perror("Empty line provided");
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
        unordered_map<string, string> allValues;
        unordered_map<string, string> statusData;
        //loop all lines of file
        for (string line; getline(file, line);) {
            //for each line split by \t
            vector<string> lineValues = commonLib::splitStringByChar(line, '\t');
            //some values from status file are in several columns, ignore those for the moment
            if (lineValues.size() != 2) {
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
        readStatFile(pidFileName);
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

    string Process::getsTime() {
        return this->stime;
    }

    void Process::setStime(const string &stime) {
        this->stime = stime;
    }

    string Process::getUtime() {
        return this->utime;
    }

    void Process::setUtime(const string &utime) {
        this->utime = utime;
    }

    string Process::getStartTime() {
        return this->startTime;
    }

    void Process::setStartTime(const string &startTime) {
        this->startTime = startTime;
    }

    string Process::getVmRSS() {
        return this->VmRSS;
    }

    void Process::setVmRSS(const string &vmRss) {
        this->VmRSS = vmRss;
    }

    string Process::getVmSize() {
        return this->getVmSize();
    }

    void Process::setVmSize(const string &VmSize) {
        this->VmSize = VmSize;
    }
}
