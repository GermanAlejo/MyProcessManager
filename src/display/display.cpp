//
// Created by german on 25/1/26.
//

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>

#include "display/display.h"

using namespace std;

namespace myProc {

    Display::Display() {
        //Set up private variables
        current_active_processes = ActiveProcesses();
        current_system_monitor = SystemMonitor();
        //Set up console output
        console = spdlog::get("console");
        if (!console) {
            console = spdlog::stdout_color_mt("console");
            std::shared_ptr<spdlog::sinks::stderr_color_sink_mt> sink = std::static_pointer_cast<spdlog::sinks::stderr_color_sink_mt>(console->sinks()[0]);
            sink->set_color(spdlog::level::info, sink->green);
            console->set_pattern("%^%v%$");
        }
    }

    void Display::render() {
        renderHeader();
        renderSystemMetrics();
        renderProcessesList();
        renderFooter();
    }

    void Display::renderSystemMetrics() const {
        console->info("CPU: {} | Total Memory: {} | Memory Used: {} | Free Memory:  {} | Uptime: {}",
            current_system_monitor.total_cpu(),
            current_system_monitor.total_ram_gib(),
            current_system_monitor.used_ram_gib(),
            current_system_monitor.available_ram(),
            current_system_monitor.get_uptime());
    }

    void Display::renderProcessesList() {
        for (Process process : current_active_processes.get_processes_vector()) {
            console->info("PID: {} | {} | State: {} | Virtual Mem: {} | Resident Memory: {} | Time Running: {}",
                process.getPid(),
                process.getName(),
                process.getState(),
                process.getVmSize(),
                process.getVmRSS(),
                process.getUtime());
        }
    }

    void Display::renderHeader() const {
        auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
        console->info("====== My Process Manager - {} ======", format("{}", today));
    }

    void Display::renderFooter() const {
        console->info("=================================");
    }



}
