//
// Created by german on 24/11/25.
//
#pragma once

#ifndef MYPROCESSMANAGER_PROCESS_H
#define MYPROCESSMANAGER_PROCESS_H

#include <unordered_map>
#include <cstdint>

namespace myProc {
    class Process {
    private:
        std::string pid{};
        //values from stat
        std::string name;
        std::string state;
        std::string utime; //time scheduled in user mode
        std::string stime; //time scheduled in kernel mode
        std::string startTime; //time process started after boot
        //values from status
        std::string VmRSS; //resident memory
        std::string VmSize; //virtual memory

        void readStatFile(const std::string &processNumber);

        std::unordered_map<std::string, std::string> parseStatFile(const std::string &fileLine);

        void readStatusFile(const std::string &processNumber);

        static std::unordered_map<std::string, std::string> parseStatusFile(std::ifstream &file);

    public:
        Process(const std::string &processName);

        //call this function each x seconds to refresh process data
        void refresh(const std::string &pidFileName);

        //function to calculate cpu usage
        double calculateCPU() const;

        //function to calculate memory inMB
        double calculateMemory() const;

        void print() const;

        //getters & setters
        std::string getPid() const;

        std::string getName();

        std::string getState();

        unsigned long long getUtime() const;

        unsigned long long getsTime() const;

        uint64_t getStartTime() const;

        unsigned long getVmRSS() const;

        unsigned long getVmSize() const;

        void setPid(const std::string &pid);

        void setName(const std::string &name);

        void setState(const std::string &state);

        void setUtime(const unsigned long long &utime);

        void setStime(const unsigned long long &stime);

        void setStartTime(const uint64_t &startTime);

        void setVmRSS(const unsigned long &vmRss);

        void setVmSize(const unsigned long &VmSize);
    };
}

#endif //MYPROCESSMANAGER_PROCESS_H
