//
// Created by german on 30/11/25.
//

#include "process_types.h"
#include "common.h"

using namespace std;

namespace myProc::types {
    const FieldData FIELDS[] = {
        {1, "pid", [](Process &p, const string &val) { p.setPid(val);}},
        {2, "comm", [](Process &p, const string &val) { p.setName(val);}},
        {3, "state", [](Process &p, const string &val) { p.setState(commonLib::getStateString(val[0]));}},
        {14, "utime", [](Process &p, const string &val) {p.setUtime(stoull(val));}},
        {15, "stime", [](Process &p, const string &val) {p.setStime(stoull(val));}},
        {22, "starttime",  [](Process &p, const string &val){p.setStartTime(stoull(val));}},
        //add more fields here
    };

    const StatusFieldData STATUS_FIELDS[] = {
        {"VmRSS", [](Process &p, const string &val) {p.setVmRSS(stoul(val));}},
        {"VmSize", [](Process &p, const string &val) {p.setVmSize(stoul(val));}},
    };

    const size_t FIELD_COUNT = sizeof(FIELDS) / sizeof(FieldData);
    const size_t STATUS_FIELD_COUNT = sizeof(STATUS_FIELDS) / sizeof(StatusFieldData);

}