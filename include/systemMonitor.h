//
// Created by german on 8/12/25.
//

#pragma once

#ifndef MYPROCESSMANAGER_SYSTEMMONITOR_H
#define MYPROCESSMANAGER_SYSTEMMONITOR_H

#include <unordered_map>
#include <cstdint>
#include <string>

#include "cpuSnapShot.h"

namespace myProc {

    class SystemMonitor {
        /**
         *  Calculate values in numeric and later in printing get string in %
         *
         */
    private:
        int totalProcesses{};

        //values that can be read
        unsigned long totalRam{}; //in kB
        unsigned long availableRam{}; //in kB
        uint64_t uptime{}; //use methods from common
        //Values to store cpu info
        CpuSnapShot lastCpuRead;
        //values to compute
        unsigned long usedRam{};
        double totalCPU{}; //cpu usage

        void refresh();

        /**
         * function to read the proc/meminfo file and save the memory info from the file
         */
        void readMemInfo();

        static std::unordered_map<std::string, std::string> parseMemInfo(std::ifstream &memFile);

        /**
         * function to read the proc/stat file and save the cpu info from the file
         */
        void readStatFile();

        static CpuSnapShot parseStatFile(std::ifstream &statFile);

        /**
         * function to read the proc/uptime and save the time usage info from the file
         */
        void readUpTime();

    public:
        SystemMonitor();

        [[nodiscard]] int total_processes() const;

        [[nodiscard]] unsigned long total_ram() const;

        [[nodiscard]] unsigned long used_ram() const;

        [[nodiscard]] unsigned long available_ram() const;

        [[nodiscard]] double total_cpu() const;

        [[nodiscard]] uint64_t get_uptime() const;

        void set_total_processes(int total_processes);

        void set_total_ram(unsigned long total_ram);

        void set_used_ram(unsigned long used_ram);

        void set_available_ram(unsigned long available_ram);

        void set_total_cpu(double total_cpu);

        void set_uptime(uint64_t uptime);
    };
} // myProc

#endif //MYPROCESSMANAGER_SYSTEMMONITOR_H
