//
// Created by german on 25/1/26.
//

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>
#include <csignal>
#include <iostream>

#include "display/display.h"
#include "common/common.h"

using namespace std;

namespace myProc {
    Display::Display(SystemMonitor &system_monitor, ActiveProcesses &active_processes,
                     const bool &active_logs) : current_active_processes(active_processes),
                                                current_system_monitor(system_monitor) {
        //Set up console output
        console = commonLib::initializeLogger(active_logs);
        //open file descriptor to write into terminal
        tty = openTerminal();
        terminalCoordinates = getEndTerminal();
    }

    void Display::render() {
        init();
        renderHeader();
        displayMetrics();
        renderFooter();
    }

    void Display::renderHeader() const {
        auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
        const string today_str = format("{:%d-%m-%Y}", today);
        string msg;
        string header;
        string title = " My Process Manager - " + today_str + " ";
        const int toFill = (terminalCoordinates.second -  title.length()) / 2;
        for (int i = 0; i < toFill; ++i) {
            msg += "=";
        }
        header = msg + title + msg;
        writeToTerminal(string(ANSI::Positioning::HOME));
        writeToTerminal(header);
    }

    void Display::renderFooter() const {
        string footer;
        for (int i = 0; i < terminalCoordinates.second; ++i) {
            footer += "=";
        }
        writeToTerminal("\n" + to_string(terminalCoordinates.first) + " " +  to_string(terminalCoordinates.second));
        writeToTerminal(
            string(
                ANSI::Positioning::moveCursorToRow(
                    terminalCoordinates.first,
                    terminalCoordinates.second
                    )) +
            footer +
            string(ANSI::Positioning::RESET)
        );
    }

    void Display::cleanup() const {
        //writeToTerminal(ANSI::Positioning::HIDE_CURSOR);
        //make sure terminal is closed
        close(this->tty);
    }

    void Display::init() const {
        // Clear screen, hide cursor
        writeToTerminal(string(ANSI::Positioning::CLEAR_SCROLL_BACK_BUFF) + string(ANSI::Positioning::HOME) + string(ANSI::Positioning::CLEAR));
    }

    /*
     *
    === MyProcessManager ===
        CPU: 23.5% | Memory Total: 16.0 GiB | Memory Used: 8.2 GiB | Memory Free: 7.8 GiB | Uptime: 45d 12h 34m
        Processes: total | Total User: 127 | Total Kernel: 89
        <spaace>
        PID | Name | State | Virtual Memory | Resident Memory | Time Running

     *
     */

    void Display::displayMetrics() {
        // Move to top, preserve logs below
        string s = string(ANSI::Positioning::moveCursorToRow(5, 0)) + ANSI::buildAnsiCodes(
                       ANSI::Color::RED, ANSI::BackgroundColor::BLUE, ANSI::Style::BOLD)
                   + "TEST" + string(ANSI::Positioning::RESET);
        writeToTerminal(s);
    }

    int Display::openTerminal() const {
        int tty = open(commonLib::terminalAliasPath, O_RDWR);
        if (tty == -1) {
            console->error("Failed to open terminal");
            throw DisplayOutputError("Could not open file descriptor");
        }
        return tty;
    }

    void Display::writeToTerminal(const std::string_view msg) const {
        write(this->tty, msg.data(), msg.size());
        fsync(this->tty); //force flush
    }

    std::pair<int,int> Display::getEndTerminal() const {
        if (tty < 0 || !isatty(tty)) {
            throw DisplayOutputError("TTY invalid");
        }

        winsize w{};
        if (ioctl(tty, TIOCGWINSZ, &w) == -1) {
            throw DisplayOutputError("Cannot get terminal size");
        }
        //minus one so we can adjust properly the footer and header
        return {w.ws_row - 1, w.ws_col};
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
}
