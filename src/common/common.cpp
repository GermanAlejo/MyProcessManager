//
// Created by german on 30/11/25.
//

#include <fstream>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "../../include/common/common.h"
#include "../../include/common/errors.h"

using namespace std;

namespace myProc::commonLib {
    void waitSomeSeconds(const int seconds) {
        spdlog::info("Waiting for {} second...", seconds);
        constexpr unsigned int microsecond = 1000000;
        usleep(seconds * microsecond); //sleeps for x seconds
    }


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
        //extract as double first (since /proc/uptime has decimals), then convert to uint64_t
        double totalTime, totalIdle;
        ss >> totalTime >> totalIdle;
        uptimeMap[TOTAL_TIME_KEY] = static_cast<uint64_t>(totalTime);
        uptimeMap[TOTAL_IDLE_KEY] = static_cast<uint64_t>(totalIdle);
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

    string getMemInfoPath() {
        return string(procBase) + string(memInfoPath);
    }

    string getStatusPath(const string &pid) {
        return string(procBase) + pid + string(statusPath);
    }

    string getUptimePath() {
        return string(procBase) + string(uptimePath);
    }

    string getExecutablePath(const string &pid) {
        return string(procBase) + pid + string(executablePath);
    }


    vector<string> splitStringByChar(const string &lineString, const char &delimiter) {
        stringstream ss(lineString);
        string segment;
        vector<string> result;

        while (getline(ss, segment, delimiter)) {
            result.push_back(segment);
        }
        return result;
    }

    bool isNumber(const string &str) {
        for (const char &c: str) {
            if (isdigit(c) == 0) return false;
        }
        return true;
    }

    string displayMemory(const double memoryValue) {
        //check if we surpass the MiB value
        if (memoryValue >= 1024) {
            //Truncate the decimals to 2 and divide 1024 so we get GiB properly
            const double resVal = trunc((memoryValue / 1024) * 100.) / 100.;
            //Clean all extra 0s
            return cleanZerosFromString(resVal) + " GiB";
        }
        const double resVal = trunc(memoryValue * 100.) / 100.;
        //Clean all extra 0s
        return cleanZerosFromString(resVal) + " MiB";
    }

    std::string cleanZerosFromString(const double value) {
        ostringstream oss;
        oss << value;
        return oss.str();
    }

    std::shared_ptr<spdlog::logger> initializeLogger(const bool &activeLogger) {
        //Set up console output
        console = spdlog::get("console");
        if (!console) {
            console = spdlog::stdout_color_mt("console");
            const std::shared_ptr<spdlog::sinks::stderr_color_sink_mt> sink = std::static_pointer_cast<
                spdlog::sinks::stderr_color_sink_mt>(console->sinks()[0]);
            sink->set_color(spdlog::level::info, sink->green);
            sink->set_color(spdlog::level::err, sink->red);
            sink->set_color(spdlog::level::warn, sink->yellow);
            sink->set_color(spdlog::level::debug, sink->cyan);
        }
        if (!activeLogger) {
            console->set_level(spdlog::level::off);
        }
        return console;
    }
}
