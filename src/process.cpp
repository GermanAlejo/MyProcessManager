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

            //TODO: Change this to dynamic
            //Extract values from map
            setVmRSS(statusMap.at("VmRSS"));
            setVmSize(statusMap.at("VmSize"));

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
        //TODO: Change this to a more dynamic way
        unordered_map<string, string> statusData;
        //TODO: this changes as we need to parse several lines
        for (string line; getline(file, line);) {
            if (statusData.size() >= 2) {
                break;
            }
            vector<string> values = commonLib::splitStringByChar(line, '\t');
            if (line.starts_with("VmRSS")) {
                statusData.insert({"VmRSS", line});
            } else if (line.starts_with("VmSize")) {
                statusData.insert({"VmSize", line});
            }
        }
        return statusData;
    }


    /**
     * This method reads a line from a file
     * we only need one line as the status files are only parsed in one line
     * @param filePath
     * @return
     */
    string readProcessFile(const string &filePath) {
        //check errors
        //TODO: Maybe more checks?
        if (filePath.empty() || !filePath.starts_with('/')) {
            perror("File path is not correct");
            throw ProcessFileError("File path not correct");
        }
        ifstream file(filePath);
        string line;
        if (!file.is_open()) {
            perror("Error reading file");
            throw ProcessFileError("Error opening file");
        }
        //get line and loop with spaces
        getline(file, line);
        return line;
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
                VmRSS << "\n" <<
                VmSize << "\n";
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
