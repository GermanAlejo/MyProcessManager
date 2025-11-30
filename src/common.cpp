//
// Created by german on 30/11/25.
//

#include "common.h"

using namespace std;

namespace  myProc::commonLib {
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
        return procBase + pid + statPath;
    }

    string getStatusPath(const string &pid) {
        return procBase + pid + statusPath;
    }

    vector<string> splitStringByChar(const string &lineString, const char &delimiter) {
        stringstream ss(lineString);
        string segment;
        vector<std::string> result;

        while(getline(ss, segment, delimiter))
        {
            result.push_back(segment);
        }
        return result;
    }
}