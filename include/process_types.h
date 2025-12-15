//
// Created by german on 30/11/25.
//

#pragma once
#ifndef MYPROCESSMANAGER_PROCESS_TYPES_H
#define MYPROCESSMANAGER_PROCESS_TYPES_H

#include <string>
#include <functional>
#include <variant>

#include "process.h"
#include "systemMonitor.h"

namespace myProc::types {
    /**
     * Struct to describe what field to extract with what name
     */
    struct LineFieldData {
        int pos; //col number from /stat
        std::string name; //key we will store the value with in a map
        std::function<void(Process &, const std::string &)> setter;
    };

    template<typename T>
    struct ColFieldData {
        std::string name;
        std::function<void(T&, const std::string &)> setter;
    };

    extern const LineFieldData LINE_FIELDS[];
    extern const ColFieldData<Process> COLUM_FIELDS[];
    extern const ColFieldData<SystemMonitor> SYSTEM_FIELDS[];

    extern const size_t LINE_FIELD_COUNT;
    extern const size_t COLUM_FIELD_COUNT;
    extern const size_t SYSTEM_FIELD_COUNT;

}
#endif //MYPROCESSMANAGER_PROCESS_TYPES_H
