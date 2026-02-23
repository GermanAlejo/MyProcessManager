//
// Created by german on 25/1/26.
//

#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>
#include <csignal>
#include <fcntl.h>
#include <sys/ioctl.h>

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
        displayGlobalMetrics();
        displayProcessMetricsHeader();
        displayProcessMetrics();
        renderFooter();
    }

    void Display::renderHeader() const {
        auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
        const string today_str = format("{:%d-%m-%Y}", today);
        string msg;
        string header;
        string title = " My Process Manager - " + today_str + " ";
        const int toFill = (terminalCoordinates.second - title.length()) / 2;
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
        writeToTerminal(
            string(ANSI::Positioning::CLEAR_SCROLL_BACK_BUFF) + string(ANSI::Positioning::HOME) + string(
                ANSI::Positioning::CLEAR));
    }

    void Display::displayGlobalMetrics() const {
        //get seconds from monitor
        const long seconds = current_system_monitor.get_uptime();
        //Build string ouput
        ostringstream oss;
        oss << left
                << setw(MAX_CELL_SIZE) << format(" CPU: {:.2f}%", current_system_monitor.total_cpu())
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << format("Total Memory: {:.2f}GiB", current_system_monitor.total_ram_gib())
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << format("Memory Used: {:.2f}GiB", current_system_monitor.used_ram_gib())
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << format("Free Memory {:.2f}GiB", current_system_monitor.available_ram_gib())
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << format("Uptime: {}h {}m {}s",
                                                 seconds / 3600,
                                                 (seconds % 3600) / 60,
                                                 seconds % 60)
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << format("Number of Processes: {}", current_system_monitor.total_processes())
                << setw(MAX_CELL_SIZE / 2) << "|";
        //extract string
        const string header = oss.str();
        //do again to adjust entire row
        ostringstream final;
        final << left << setw(terminalCoordinates.second) << header;
        const string msg = ANSI::buildAnsiCodes(ANSI::Color::WHITE, ANSI::BackgroundColor::BLUE, ANSI::Style::BOLD)
                           +
                           final.str() + string(ANSI::Positioning::RESET);
        writeToTerminal("\n");
        writeToTerminal(msg);
    }

    void Display::displayProcessMetricsHeader() const {
        //Use ostringstream to build an adjuster header string
        ostringstream oss;
        oss << left
                << setw(MAX_CELL_SIZE) << " PID"
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << "NAME"
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << "State"
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << "CPU"
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << "Resident Memory"
                << setw(MAX_CELL_SIZE / 2) << "|"
                << setw(MAX_CELL_SIZE) << "Time Running"
                << setw(MAX_CELL_SIZE / 2) << "|";
        const string headerStr = oss.str();

        //Now adjust the entire header
        ostringstream final;
        final << left << setw(terminalCoordinates.second) << headerStr;
        const string msg = ANSI::buildAnsiCodes(ANSI::Color::BLACK, ANSI::BackgroundColor::WHITE, ANSI::Style::BOLD) +
                           final.str() + string(ANSI::Positioning::RESET);
        writeToTerminal("\n");
        writeToTerminal(msg);
    }

    //TODO: IMplement max amount of processes displayed
    //TODO: Implement a way to distinct which process to paint
    void Display::displayProcessMetrics() const {
        const int maxProcessPerScreen = terminalCoordinates.first;
        int currentCount = 0;
        for (const Process &process: current_active_processes.get_processes_vector()) {
            if (currentCount == maxProcessPerScreen) break;
            const long seconds = process.getElapsedSeconds(current_system_monitor.get_uptime());
            //Use ostringstream to build an adjuster header string
            ostringstream oss;
            oss << left
                    << setw(MAX_CELL_SIZE) << process.getPid()
                    << setw(MAX_CELL_SIZE / 2) << " "
                    << setw(MAX_CELL_SIZE) << process.getName()
                    << setw(MAX_CELL_SIZE / 2) << " "
                    << setw(MAX_CELL_SIZE) << process.getState()
                    << setw(MAX_CELL_SIZE / 2) << " "
                    << setw(MAX_CELL_SIZE) << format("{:.2f}", process.get_cpu_usage())
                    << setw(MAX_CELL_SIZE / 2) << " "
                    << setw(MAX_CELL_SIZE) << commonLib::displayMemory(process.vm_rss_mib())
                    << setw(MAX_CELL_SIZE / 2) << " "
                    << setw(MAX_CELL_SIZE) << format("{}h {}m {}s",
                                                     seconds / 3600,
                                                     (seconds % 3600) / 60,
                                                     seconds % 60)
                    << setw(MAX_CELL_SIZE / 2) << " ";
            const string processStr = oss.str();
            ostringstream final;
            final << left << setw(terminalCoordinates.second) << processStr;
            string msg = ANSI::buildAnsiCodes(ANSI::Color::WHITE, nullopt, nullopt) +
                         final.str() + string(ANSI::Positioning::RESET);
            writeToTerminal("\n");
            writeToTerminal(msg);
            ++currentCount;
        }
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

    std::pair<int, int> Display::getEndTerminal() const {
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
