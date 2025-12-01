//
// Created by german on 30/11/25.
//

#ifndef MYPROCESSMANAGER_PROCESS_TYPES_H
#define MYPROCESSMANAGER_PROCESS_TYPES_H

#include <string>
#include <functional>
#include "process.h"
#include "common.h"

namespace myProc::types {
    /**
     * Struct to describe what field to extract with what name
     */
    struct FieldData {
        int pos; //col number from /stat
        std::string name; //key we will store the value with in a map
        std::function<void(Process &, const std::string &)> setter;
    };

    struct StatusFieldData {
        std::string name;
        std::function<void(Process &, const std::string &)> setter;
    };

    extern const FieldData FIELDS[];
    extern const StatusFieldData STATUS_FIELDS[];

    extern const size_t FIELD_COUNT;
    extern const size_t STATUS_FIELD_COUNT;

}
#endif //MYPROCESSMANAGER_PROCESS_TYPES_H
