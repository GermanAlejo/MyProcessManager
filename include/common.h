//
// Created by german on 25/11/25.
//

#pragma once

#ifndef MYPROCESSMANAGER_COMMON_H
#define MYPROCESSMANAGER_COMMON_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "errors.h"

namespace myProc::commonLib {
    inline constexpr char processPath[] = "/proc/*/stat";
    inline constexpr std::string processFinderChar = "*";

    enum state {
        R = 'R',//Running
        S = 'S',//Sleeping in an interruptible wait
        D = 'D',//Waiting in uninterruptible disk sleep
        Z = 'Z',//Zombie
        T = 'T',//Stopped (on a signal) or (before Linux 2.6.33) trace stopped
        t = 't',//Tracing stop (Linux 2.6.33 onward)
        W = 'W',//Paging (only before Linux 2.6.0)
        X = 'X',//Dead (from Linux 2.6.0 onward)
        x = 'x',//Dead (Linux 2.6.33 to 3.13 only)
        K = 'k',//Wakekill (Linux 2.6.33 to 3.13 only)
        //w = 'W',//Waking (Linux 2.6.33 to 3.13 only)
        P = 'P',//Parked (Linux 3.9 to 3.13 only)
        I = 'I',//Idle (Linux 4.14 onward)
    };

    inline std::string getStateString(const char& stateChar) {
        switch (stateChar) {
            case state::R:
                return "Running";
            case state::S:
                return "Sleeping";
            case state::D:
                return "Waiting";
            case state::Z:
                return "Zombie";
            case state::T:
                return "Stopped";
            case state::t:
                return "Tracing stop";
            case state::W:
                return "Dead";
            case state::X:
                return "Dead";
            case state::K:
                return "Wakekill";
                //case state::w:
                //    return "Waking";
            case state::P:
                return "Parked";
            case state::I:
                return "Idle";
            default: return "Unknown";
        }
        throw std::runtime_error("Type not supported/valid");
    }

    inline bool replace(std::string &fullProcessPath, const std::string &pid) {
        std::string res = processPath;
        fullProcessPath += res;
        int start = res.find(processFinderChar);
        std::string end = pid + res.substr(start + 1, res.length());
        if (start == std::string::npos) {
            return false;
        }
        fullProcessPath.replace(start, res.length(), end);
        return true;
    }
}

#endif //MYPROCESSMANAGER_COMMON_H