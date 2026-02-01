//
// Created by german on 2/12/25.
//

#pragma once

#ifndef MYPROCESSMANAGER_ACTIVEPROCESSES_H
#define MYPROCESSMANAGER_ACTIVEPROCESSES_H

#include <string>
#include <vector>
#include "../process/process.h"

namespace myProc {
    class ActiveProcesses {
    private:
        std::vector<Process> processesVector;
        uint64_t systemUpTime;

        //TODO: Ensure the vector is ordered
        //TODO: Make function to order vector by Memory usage

        static bool isKernelProcess(const std::string& pid);
        void readProcDir();
    public:
        explicit ActiveProcesses(const uint64_t &totalUpTime);

        void refresh();
        void printProcessContainer() const;
        std::vector<Process> get_processes_vector();
        void set_processes_vector(const std::vector<Process> &processes_vector);
    };
}

#endif //MYPROCESSMANAGER_ACTIVEPROCESSES_H