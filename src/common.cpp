//
// Created by german on 30/11/25.
//

#include <fstream>
#include <spdlog/spdlog.h>

#include "common.h"
#include "errors.h"

using namespace std;

namespace  myProc::commonLib {
    unordered_map<string_view, uint64_t> getUptimeData() {
        spdlog::info("Reading uptime file");
        const string path = getUptimePath();
        //check errors
        if (path.empty() || !path.starts_with('/')) {
            spdlog::error("File path not found!");
            throw ProcessFileError("File path not correct");
        }

        ifstream uptimeFile(path);
        string line;
        if (!uptimeFile.is_open()) {
            spdlog::error("File found - but could not be open");
            throw ProcessFileError("Error opening file");
        }

        //get line and loop with spaces
        getline(uptimeFile, line);
        return parseUptimeFile(line);
    }

    unordered_map<string_view, uint64_t> parseUptimeFile(const string &line) {
        spdlog::info("Parsing uptime file");
        if (line.empty()) {
            spdlog::error("Empty line provided");
            throw ProcessReadError("Empty line");
        }

        //get stream from line and parse it
        stringstream ss(line);
        unordered_map<string_view, uint64_t> uptimeMap;
        //extract direcly as is always 2 values
        //make a for to extract all values
        ss >> uptimeMap[TOTAL_TIME_KEY] >> uptimeMap[TOTAL_IDLE_KEY];
        return uptimeMap;
    }

    string getStateString(const char &stateChar) {
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
    }

    string getStatPath(const string &pid) {
        if (pid.empty()) {
            string base = procBase;
            base.pop_back();
            return base + string(statPath);
        }
        return procBase + pid + statPath;
    }

    std::string getMemInfoPath() {
        return string(procBase) + string(memInfoPath);
    }

    string getStatusPath(const string &pid) {
        return string(procBase) + pid +string(statusPath);
    }

    string getUptimePath() {
        return string(procBase) + string(uptimePath);
    }

    std::string getExecutablePath(const std::string &pid) {
        return string(procBase) + pid + string(executablePath);
    }


    vector<string> splitStringByChar(const string &lineString, const char &delimiter) {
        stringstream ss(lineString);
        string segment;
        vector<string> result;

        while(getline(ss, segment, delimiter))
        {
            result.push_back(segment);
        }
        return result;
    }

    bool isNumber(const std::string &str) {
        for (const char &c : str) {
            if (isdigit(c) == 0) return false;
        }
        return true;
    }

}