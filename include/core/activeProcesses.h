//
// Created by german on 2/12/25.
//

#pragma once

#ifndef MYPROCESSMANAGER_ACTIVEPROCESSES_H
#define MYPROCESSMANAGER_ACTIVEPROCESSES_H

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include "../process/process.h"

namespace myProc {
    class ActiveProcesses {
    private:
        std::vector<Process> processesVector;

        //TODO: Ensure the vector is ordered
        //TODO: Make function to order vector by Memory usage

        static bool isKernelProcess(const std::string& pid);
        void readProcDir();
    public:
        ActiveProcesses();

        void refresh() const;
        void printProcessContainer() const;
        std::vector<Process> get_processes_vector();
        void set_processes_vector(const std::vector<Process> &processes_vector);
    };
}

#endif //MYPROCESSMANAGER_ACTIVEPROCESSES_H