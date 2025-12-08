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
#include "process.h"

namespace myProc {
    class ActiveProcesses {
    private:
        std::vector<Process> processesVector;

    public:
        ActiveProcesses();

        void readProcDir();
        void printProcessContainer() const;

        static bool isKernelProcess(const std::string& pid);
    };
}

#endif //MYPROCESSMANAGER_ACTIVEPROCESSES_H