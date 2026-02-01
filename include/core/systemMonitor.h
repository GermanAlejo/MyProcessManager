//
// Created by german on 8/12/25.
//

#pragma once

#ifndef MYPROCESSMANAGER_SYSTEMMONITOR_H
#define MYPROCESSMANAGER_SYSTEMMONITOR_H

#include <unordered_map>
#include <cstdint>
#include <string>
#include <optional>

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
        unsigned long usedRam{};
        double totalRamGiB;
        double usedRamGiB;
        double availableRamGiB;
        double usedRamPercentage;
        uint64_t uptime{}; //use methods from common
        //Values to store cpu info
        std::optional<CpuSnapShot> lastCpuRead;
        double totalCPU{}; //cpu usage
        //TODO: Make function to return FreeMemory(available) in GiB

        /**
         * function to read the proc/meminfo file and save the memory info from the file
         */
        void readMemInfo();

        /**
         * Maps the file containing the memory information into a map
         * @param memFile
         * @return std::unordered_map<std::string, std::string>
         */
        static std::unordered_map<std::string, std::string> parseMemInfo(std::ifstream &memFile);

        /**
         * function to read the proc/stat file and save the cpu info from the file
         */
        void readStatFile();

        /**
         * Creates a cpu snapshot
         * @param statFile
         * @return CpuSnapShot
         */
        static CpuSnapShot parseStatFile(std::ifstream &statFile);

        /**
         * function to read the proc/uptime and save the time usage info from the file
         */
        void readUpTime();

        /**
         * Function to calculate cpu values to represent usage
         * we will calculate here totalCPU
         */
        void calculateTotalCPU(const CpuSnapShot &newSnapShot);

        /**
         * Calculate ram usage percentage using total_ram and used_ram
         */
        void calculateRamFinalValues();

    public:
        SystemMonitor();

        /**
         *
         */
        void refresh();

        [[nodiscard]] int total_processes() const;

        [[nodiscard]] unsigned long total_ram() const;

        [[nodiscard]] unsigned long used_ram() const;

        [[nodiscard]] unsigned long available_ram() const;

        [[nodiscard]] double used_ram_gib() const;

        [[nodiscard]] double used_ram_percentage() const;

        [[nodiscard]] double total_ram_gib() const;

        [[nodiscard]] double available_ram_gib() const;

        [[nodiscard]] double total_cpu() const;

        [[nodiscard]] uint64_t get_uptime() const;

        void set_total_processes(int total_processes);

        void set_total_ram(unsigned long total_ram);

        void set_used_ram(unsigned long used_ram);

        void set_available_ram(unsigned long available_ram);

        void set_used_ram_gib(double used_ram_gib);

        void set_used_ram_percentage(double used_ram_percentage);

        void set_total_ram_gib(double total_ram_gib);

        void set_available_ram_gib(double available_ram_gib);

        void set_total_cpu(double total_cpu);

        void set_uptime(uint64_t uptime);
    };
} // myProc

#endif //MYPROCESSMANAGER_SYSTEMMONITOR_H
