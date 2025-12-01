//
// Created by german on 24/11/25.
//
#pragma once

#ifndef MYPROCESSMANAGER_PROCESS_H
#define MYPROCESSMANAGER_PROCESS_H

#include <unordered_map>

namespace myProc {
    class Process {
    private:
        std::string pid {};
        //values from stat
        std::string name;
        std::string state;
        std::string utime;//time scheduled in user mode
        std::string stime;//time scheduled in kernel mode
        std::string startTime;//time process started after boot
        //values from status
        std::string VmRSS; //resident memory
        std::string VmSize; //virtual memory

        void readStatFile(const std::string& processNumber);
        std::unordered_map<std::string, std::string> parseStatFile(const std::string& fileLine);
        void readStatusFile(const std::string& processNumber);
        static std::unordered_map<std::string, std::string> parseStatusFile(std::ifstream& file);

    public:
        Process(const std::string& processName);
        void refresh(const std::string &pidFileName);
        void print() const;
        //getters & setters
        std::string getPid() const;
        std::string getName();
        std::string getState();
        std::string getUtime();
        std::string getsTime();
        std::string getStartTime();
        std::string getVmRSS();
        std::string getVmSize();
        void setPid(const std::string &pid);
        void setName(const std::string &name);
        void setState(const std::string& state);
        void setUtime(const std::string& utime);
        void setStime(const std::string& stime);
        void setStartTime(const std::string& startTime);
        void setVmRSS(const std::string& vmRss);
        void setVmSize(const std::string& VmSize);
    };
}

#endif //MYPROCESSMANAGER_PROCESS_H