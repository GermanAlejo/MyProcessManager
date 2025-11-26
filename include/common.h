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

inline constexpr char processPath[] = "/proc/*/stat";
inline constexpr std::string processFinderChar = "*";

inline bool replace(std::string& fullProcessPath, const std::string& pid) {
    std::string res = processPath;
    fullProcessPath += res;
    int start = res.find(processFinderChar);
    std::string end = pid + res.substr(start + 1, res.length());
    if(start == std::string::npos) {
        return false;
    }
    fullProcessPath.replace(start, res.length(), end);
    return true;
}

#endif //MYPROCESSMANAGER_COMMON_H