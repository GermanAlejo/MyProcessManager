//
// Created by german on 2/12/25.
//

#include <filesystem>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include "activeProcesses.h"
#include "errors.h"
#include "common.h"
#include "process.h"

using namespace std;

namespace myProc {
    ActiveProcesses::ActiveProcesses() {
        readProcDir();
    }

    void ActiveProcesses::readProcDir() {
        try {
            spdlog::info("Creating container");
            filesystem::path rootPath = commonLib::procBase;
            if (!filesystem::exists(rootPath)) {
                spdlog::error("Proc base path does not exist or is corrupted");
                throw ProcessReadError("Proc base path not found!");
            }
            spdlog::info("Reading all processes");
            for (const auto& entry : filesystem::directory_iterator(rootPath)) {
                string pid = entry.path().filename().string(); //get folder name to use as key
                if (!commonLib::isNumber(pid)) {
                    spdlog::warn("Tried to read a folder which is not a process - Skipping: {}", pid);
                    continue;
                }
                string processName = entry.path().filename().string();
                if (isKernelProcess(processName)) {
                    spdlog::warn("Skipping kernel process with name: {}", processName);
                    continue;
                }
                Process newProcess(processName);
                processesVector.push_back(newProcess);
            }
        } catch (ProcessError &err) {
            spdlog::error(err.what());
            throw ProcessError("Error in process container");
        }
    }

    void ActiveProcesses::printProcessContainer() const {
        try {
            spdlog::info("Printing all processes for this container: ");
            for (Process p : processesVector) {
                spdlog::info(" ------------------- Process: {} ------------------- ", p.getName());
                p.print();
            }
            spdlog::info("--------- Done Printing ---------");
        } catch (ProcessError &err) {
            spdlog::error(err.what());
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


}
