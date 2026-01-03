//
// Created by german on 3/1/26.
//


#include <string>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "cpuSnapShot.h"
#include "process_types.h"

using namespace std;

namespace myProc {
    CpuSnapShot::CpuSnapShot(const unordered_map<string, string> &allData) {
        for (size_t i = 0; i < types::CPU_FIELD_COUNT; ++i) {
            const auto &fieldData = types::CPU_FIELDS[i];
            //Check for empty value
            auto it = allData.find(fieldData.name);
            if (it == allData.end() || it->second.empty()) {
                spdlog::warn("Field system property: {} not found in stat file", fieldData.name);
                continue;
            }
            fieldData.setter(*this, it->second); // Apply the setter lambda
        }
    }
}
