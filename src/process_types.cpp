//
// Created by german on 30/11/25.
//

#include "process_types.h"
#include "common.h"
#include "process.h"
#include "systemMonitor.h"
#include "cpuSnapShot.h"

using namespace std;

namespace myProc::types {
    const LineFieldData LINE_FIELDS[] = {
        {1, "pid", [](Process &p, const string &val) { p.setPid(val);}},
        {2, "comm", [](Process &p, const string &val) { p.setName(val);}},
        {3, "state", [](Process &p, const string &val) { p.setState(commonLib::getStateString(val[0]));}},
        {14, "utime", [](Process &p, const string &val) {p.setUtime(stoull(val));}},
        {15, "stime", [](Process &p, const string &val) {p.setStime(stoull(val));}},
        {22, "starttime",  [](Process &p, const string &val){p.setStartTime(stoull(val));}},
        //add more fields here
    };

    const ColFieldData<Process> COLUM_FIELDS[] = {
        {"VmRSS", [](Process &p, const string &val) {p.setVmRSS(stoul(val));}},
        {"VmSize", [](Process &p, const string &val) {p.setVmSize(stoul(val));}}
    };

    const ColFieldData<SystemMonitor> SYSTEM_FIELDS[] = {
        {"MemTotal", [](SystemMonitor &m, const string &val) {m.set_total_ram(stoul(val));}},
        {"MemAvailable", [](SystemMonitor &m, const string &val) {m.set_available_ram(stoul(val));}},
        {"totalTime", [](SystemMonitor &m, const string &val) {m.set_uptime(stoull(val));}}
    };

    const LineCpuFieldData<CpuSnapShot> CPU_FIELDS[] = {
        {1, "user", [](CpuSnapShot &m, const string &val) {m.set_user(stoul(val));}},
        {2, "nice", [](CpuSnapShot &m, const string &val) {m.set_nice(stoul(val));}},
        {3, "system", [](CpuSnapShot &m, const string &val) {m.set_system(stoul(val));}},
        {4, "idle", [](CpuSnapShot &m, const string &val) {m.set_idle(stoul(val));}},
        {5, "iowait", [](CpuSnapShot &m, const string &val) {m.set_iowait(stoul(val));}},
        {6, "irq", [](CpuSnapShot &m, const string &val) {m.set_irq(stoul(val));}},
        {7, "softirq", [](CpuSnapShot &m, const string &val) {m.set_softirq(stoul(val));}},
        {8, "steal", [](CpuSnapShot &m, const string &val) {m.set_steal(stoul(val));}},
        {9, "guest", [](CpuSnapShot &m, const string &val) {m.set_guest(stoul(val));}},
        {10, "guest_nice", [](CpuSnapShot &m, const string &val) {m.set_guest_nice(stoul(val));}}
    };

    const size_t LINE_FIELD_COUNT = sizeof(LINE_FIELDS) / sizeof(LineFieldData);
    const size_t COLUM_FIELD_COUNT = sizeof(COLUM_FIELDS) / sizeof(ColFieldData<Process>);
    const size_t SYSTEM_FIELD_COUNT = sizeof(SYSTEM_FIELDS) / sizeof(ColFieldData<SystemMonitor>);
    const size_t CPU_FIELD_COUNT = sizeof(CPU_FIELDS) / sizeof(LineCpuFieldData<CpuSnapShot>);

}