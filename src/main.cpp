#include <csignal>
#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>

#include "common/common.h"
#include "core/activeProcesses.h"
#include "core/systemMonitor.h"
#include "display/display.h"

using namespace myProc;

std::atomic_bool stop = false;
//initialize main logger (always display this logs)
const std::shared_ptr<spdlog::logger> console = commonLib::initializeLogger(true);

/**
* Method to handle graceful shutdown & program interruption
*/
void signal_handler(const int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        console->info("Received signal - Terminating program...");
        stop.store(true);
    }
}

/**
 * Method to read input args for the program
 * @param argc number of params
 * @param argv array with params
 * @param value var to store waiting time between ticks
 * @param active flag to control logs outputs
 */
void read_args(int &argc, char *argv[], int &value, bool &active) {
    for (int i = 1; i < argc; ++i) {
        if (std::string arg = argv[i]; arg == "--interval") {
            const auto input = argv[++i];
            if (input == nullptr) {
                console->error("Invalid Input");
                throw std::invalid_argument("Missing value for argument: " + arg);
            }
            value = std::stoi(input);
        } else if (arg == "--verbose") {
            active = true;
        }
    }
}

int main(int argc, char *argv[]) {
    //Register termination signals
    std::signal(SIGTERM, signal_handler); //ctr + c
    std::signal(SIGINT, signal_handler); //termination request

    constexpr int MAX_INTERVAL = 60000;
    constexpr int MIN_INTERVAL = 1000;

    console->info("Setting default intervals");
    int value = 1000; //1s
    bool active = false; //set to false by default

    if (argc > 1) {
        try {
            read_args(argc, argv, value, active);
            console->info("Intervals provided: {}", value);
            if (value > MAX_INTERVAL || value < MIN_INTERVAL)
                throw std::out_of_range(
                    "Error - number out of range max interval allowed: 60000 (1 minute) min interval allowed: 1000 (1 second)");
        } catch (std::invalid_argument const &ex) {
            console->error(ex.what());
            console->error("Error - Invalid input provided");
            console->info("Usage: MyProcessManager --interval [INTERVAL_IN_SECONDS] --verbose");
            return EXIT_FAILURE;
        } catch (std::out_of_range const &ex) {
            console->error(ex.what());
            return EXIT_FAILURE;
        } catch (std::logic_error const &ex) {
            console->error(ex.what());
            console->info("Usage: MyProcessManager --interval [INTERVAL_IN_SECONDS] --verbose");
            return EXIT_FAILURE;
        }
    }

    const int REFRESH_INTERVALS_MS = value;
    const bool isLoggerActive = active;

    // Initialize pointers
    const std::unique_ptr<SystemMonitor> monitor = std::make_unique<SystemMonitor>(isLoggerActive);
    const std::unique_ptr<ActiveProcesses> active_processes = std::make_unique<ActiveProcesses>(
        monitor->get_uptime(), isLoggerActive);
    Display display(*monitor, *active_processes, isLoggerActive);

    while (!stop.load()) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(REFRESH_INTERVALS_MS)
        );
        try {
            monitor->refresh();
            active_processes->refresh();
            display.render();
            //sleep for a while
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } catch (const std::exception &e) {
            console->error("Error during refresh: {}", e.what());
        }
    }

    //Clear the display/terminal
    display.cleanup();

    return EXIT_SUCCESS;
}
