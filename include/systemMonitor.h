//
// Created by german on 8/12/25.
//

#pragma once

#ifndef MYPROCESSMANAGER_SYSTEMMONITOR_H
#define MYPROCESSMANAGER_SYSTEMMONITOR_H

#include <unordered_map>
#include <cstdint>
#include <string>

namespace myProc {
    class SystemMonitor {
    private:
        int totalProcesses{};
        unsigned long totalRam{};
        unsigned long usedRam{};
        unsigned long availableRam{};
        double totalCPU{};
        uint64_t uptime{}; //use methods from common

        void refresh();
        void readMemInfo();
        std::unordered_map<std::string, std::string> parseMemInfo(std::ifstream &memFile);
        void readStatFile();
        std::unordered_map<std::string, std::string> parseStatFile(std::ifstream &statFile);
        //use this to call commonlib functions
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