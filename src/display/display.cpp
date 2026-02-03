//
// Created by german on 25/1/26.
//

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>
#include <csignal>

#include "display/display.h"

#include <iostream>

#include "common/common.h"

using namespace std;

namespace myProc {
    Display::Display(SystemMonitor &system_monitor, ActiveProcesses &active_processes, const bool &active_logs) : current_active_processes(
            active_processes),
        current_system_monitor(system_monitor) {
        //Set up console output
        console = commonLib::initializeLogger(active_logs);
    }

    void Display::render() {
        //testing
        init();
        displayMetrics(2.4, 1.2);
        cleanup();
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

    void Display::renderProcessesList() const {
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

    void Display::cleanup() {
        std::cout << "\033[?25h";  // Show cursor
    }

    void Display::init() {
        // Clear screen, hide cursor
        std::cout << "\033[2J\033[?25l";
        std::cout.flush();
    }

    void Display::displayMetrics(float cpu, float mem) {
        // Move to top, preserve logs below
        std::cout << "\033[H";  // Home
        std::cout << "CPU: " << cpu << "%\n";
        std::cout << "Memory: " << mem << "%\n";
        std::cout.flush();

        // Logs appear below your metrics
        console->info("Event occurred");
        console->info("TEST");
    }
}
