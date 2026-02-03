//
// Created by german on 2/12/25.
//

#include <filesystem>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include "../../include/core/activeProcesses.h"
#include "../../include/common/errors.h"
#include "../../include/common/common.h"
#include "../../include/process/process.h"

using namespace std;

namespace myProc {
    ActiveProcesses::ActiveProcesses(const uint64_t &totalUpTime, const bool &isLoggerActive) {
        //Create logger
        console = commonLib::initializeLogger(isLoggerActive);
        console->info("Creating process container");
        systemUpTime = totalUpTime;
        readProcDir();
    }

    void ActiveProcesses::refresh() {
        try {
            console->info("Refreshing process container");
            readProcDir();
            for (Process p : processesVector) {
                p.refresh(systemUpTime);
            }
        } catch (ProcessError &err) {
            console->error("Error refreshing - {}", err.what());
            throw ProcessReadError("Could not refresh process");
        }
    }


    void ActiveProcesses::readProcDir() {
        try {
            console->info("Creating container");
            const filesystem::path rootPath = commonLib::procBase;
            if (!filesystem::exists(rootPath)) {
                console->error("Proc base path does not exist or is corrupted");
                throw ProcessReadError("Proc base path not found!");
            }
            console->info("Reading all processes");
            for (const auto& entry : filesystem::directory_iterator(rootPath)) {
                string pid = entry.path().filename().string(); //get folder name to use as key
                if (!commonLib::isNumber(pid)) {
                    console->warn("Tried to read a folder which is not a process - Skipping: {}", pid);
                    continue;
                }
                string processName = entry.path().filename().string();
                if (isKernelProcess(processName)) {
                    console->warn("Skipping kernel process with name: {}", processName);
                    continue;
                }
                Process newProcess(processName, systemUpTime);
                processesVector.push_back(newProcess);
            }
        } catch (ProcessError &err) {
            console->error(err.what());
            throw ProcessError("Error in process container");
        }
    }

    void ActiveProcesses::printProcessContainer() const {
        try {
            console->info("Printing all processes for this container: ");
            for (Process p : processesVector) {
                console->info(" ------------------- Process: {} ------------------- ", p.getName());
                p.print();
            }
            console->info("--------- Done Printing ---------");
        } catch (ProcessError &err) {
            console->error(err.what());
            throw ProcessReadError("Error printing some process");
        }
    }

    bool ActiveProcesses::isKernelProcess(const std::string& pid) {
        string exeFullPath = commonLib::getExecutablePath(pid);
        char buff[PATH_MAX];
        if (readlink(exeFullPath.c_str(), buff, size(buff) - 1) == -1) {
            return true;
        }
        return false;
    }

    std::vector<Process> ActiveProcesses::get_processes_vector() {
        return processesVector;
    }

    void ActiveProcesses::set_processes_vector(const std::vector<Process> &processes_vector) {
        processesVector = processes_vector;
    }


}
