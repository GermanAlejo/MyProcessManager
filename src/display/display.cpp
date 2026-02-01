//
// Created by german on 25/1/26.
//

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>

#include "display/display.h"
#include "common/common.h"

using namespace std;

namespace myProc {
    Display::Display(SystemMonitor &system_monitor, ActiveProcesses &active_processes) : current_active_processes(
            active_processes),
        current_system_monitor(system_monitor) {
        //Set up console output
        console = spdlog::get("console");
        if (!console) {
            console = spdlog::stdout_color_mt("console");
            std::shared_ptr<spdlog::sinks::stderr_color_sink_mt> sink = std::static_pointer_cast<
                spdlog::sinks::stderr_color_sink_mt>(console->sinks()[0]);
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
        const long seconds = current_system_monitor.get_uptime();
        console->info(
            "CPU: {}% | Total Memory: {}GiB | Memory Used: {}GiB | Free Memory:  {}GiB | Uptime: {}h {}m {}s | Number Processes: {}",
            current_system_monitor.total_cpu(),
            current_system_monitor.total_ram_gib(),
            current_system_monitor.used_ram_gib(),
            current_system_monitor.available_ram_gib(),
            seconds / 3600,
            (seconds % 3600) / 60,
            seconds % 60,
            current_system_monitor.total_processes());
    }

    void Display::renderProcessesList() {
        for (const Process &process: current_active_processes.get_processes_vector()) {
            long seconds = process.getElapsedSeconds(current_system_monitor.get_uptime());
            console->info(
                "PID: {} | {} | State: {} | CPU: {:.2f}% | Resident Memory: {} | Time Running: {}h {}m {}s",
                process.getPid(),
                process.getName(),
                process.getState(),
                process.get_cpu_usage(),
                commonLib::displayMemory(process.vm_rss_mib()),
                seconds / 3600,
                (seconds % 3600) / 60,
                seconds % 60);
        }
    }

    void Display::renderHeader() const {
        auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
        console->info("\n\n====== My Process Manager - {} ======", format("{}", today));
    }

    void Display::renderFooter() const {
        console->info("=================================\n\n");
    }
}
