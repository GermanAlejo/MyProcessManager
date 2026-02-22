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
        std::string utime; //time scheduled in user mode (CPU ticks)
        std::string stime; //time scheduled in kernel mode (CPU ticks)
        std::string startTime; //time process started after boot
        //values from status
        std::string VmRSS; //resident memory
        double VmRSSGiB;
        std::string VmSize; //virtual memory
        double VmSizeGiB;
        double cpu_usage;
        //Logger
        std::shared_ptr<spdlog::logger> console;

        void readStatFile(const std::string &processNumber);

        std::unordered_map<std::string, std::string> parseStatFile(const std::string &fileLine);

        void readStatusFile(const std::string &processNumber);

        std::unordered_map<std::string, std::string> parseStatusFile(std::ifstream &file);

    public:
        Process(const std::string &processName, const uint64_t systemUpTime, const std::shared_ptr<spdlog::logger>& logger);

        //call this function each x seconds to refresh process data
        void refresh(const uint64_t &systemUpTime);

        //function to calculate cpu usage
        void calculateCPU(const uint64_t &systemUpTime);

        //function to calculate memory inMB
        void calculateMemory();

        void print() const;

        //getters & setters
        [[nodiscard]] std::string getPid() const;

        [[nodiscard]] std::string getName() const;

        [[nodiscard]] std::string getState() const;

        [[nodiscard]] unsigned long long getUtime() const;

        [[nodiscard]] unsigned long long getsTime() const;

        [[nodiscard]] uint64_t getStartTime() const;

        [[nodiscard]] unsigned long getVmRSS() const;

        [[nodiscard]] unsigned long getVmSize() const;

        void setPid(const std::string &pid);

        void setName(const std::string &name);

        void setState(const std::string &state);

        void setUtime(const unsigned long long &utime);

        void setStime(const unsigned long long &stime);

        void setStartTime(const uint64_t &startTime);

        void setVmRSS(const unsigned long &vmRss);

        void setVmSize(const unsigned long &VmSize);

        [[nodiscard]] double vm_rss_mib() const;

        void set_vm_rss_mib(const double &vm_rss_gib);

        [[nodiscard]] double vm_size_mib() const;

        void set_vm_size_mib(const double &vm_size_gib);

        [[nodiscard]] double get_cpu_usage() const;

        void set_cpu_usage(const double &cpu_usage);

        long getElapsedSeconds(const uint64_t systemUpTime) const;
    };
}

#endif //MYPROCESSMANAGER_PROCESS_H
