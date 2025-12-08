//
// Created by german on 25/11/25.
//

#pragma once

#ifndef MYPROCESSMANAGER_COMMON_H
#define MYPROCESSMANAGER_COMMON_H

#include <string>
#include <unordered_map>
#include <vector>

namespace myProc::commonLib {
    inline constexpr char processPath[] = "/proc/*/stat";
    inline constexpr char procBase[] = "/proc/";
    inline constexpr char statPath[] = "/stat";
    inline constexpr char executablePath[] = "/exe";
    inline constexpr char statusPath[] = "/status";
    inline constexpr char uptimePath[] = "uptime"; //does not depent of a process so remove /
    inline constexpr char processPathStatus[] = "/proc/*/status";
    inline constexpr std::string_view processFinderChar = "/*";
    inline constexpr std::string_view TOTAL_TIME_KEY = "totalTime"; //return as uint64_t
    inline constexpr std::string_view TOTAL_IDLE_KEY = "idleTime"; //return as uint64_t

    std::unordered_map<std::string_view, uint64_t> getUptimeData();

    std::unordered_map<std::string_view, uint64_t> parseUptimeFile(const std::string &line);

    enum state {
        R = 'R', //Running
        S = 'S', //Sleeping in an interruptible wait
        D = 'D', //Waiting in uninterruptible disk sleep
        Z = 'Z', //Zombie
        T = 'T', //Stopped (on a signal) or (before Linux 2.6.33) trace stopped
        t = 't', //Tracing stop (Linux 2.6.33 onward)
        W = 'W', //Paging (only before Linux 2.6.0)
        X = 'X', //Dead (from Linux 2.6.0 onward)
        x = 'x', //Dead (Linux 2.6.33 to 3.13 only)
        K = 'k', //Wakekill (Linux 2.6.33 to 3.13 only)
        //w = 'W',//Waking (Linux 2.6.33 to 3.13 only)
        P = 'P', //Parked (Linux 3.9 to 3.13 only)
        I = 'I', //Idle (Linux 4.14 onward)
    };

    std::string getUptimePath();

    std::string getStateString(const char &stateChar);

    std::string getStatPath(const std::string &pid);

    std::string getStatusPath(const std::string &pid);

    std::string getExecutablePath(const std::string &pid);

    std::vector<std::string> splitStringByChar(const std::string &lineString, const char &delimiter);

    bool isNumber(const std::string &str);
}

#endif //MYPROCESSMANAGER_COMMON_H
