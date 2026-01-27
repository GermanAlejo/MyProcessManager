
#include <iostream>
#include <memory>

#include "core/activeProcesses.h"
#include "core/systemMonitor.h"
#include "display/display.h"

using namespace myProc;

int main(int argc, char* argv[]) {
    const int REFRESH_INTERVALS_MS = 1000; //1s
    // Initialize pointers
    std::unique_ptr<SystemMonitor> monitor = std::make_unique<SystemMonitor>();
    std::unique_ptr<ActiveProcesses> active_processes = std::make_unique<ActiveProcesses>();
    Display display(*monitor, *active_processes);

    bool running = true;

    //TODO: Investigate graceful shutdown

    while (running) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(REFRESH_INTERVALS_MS)
        );
        try {
            monitor->refresh();
            active_processes->refresh();
            display.render();
        } catch (const std::exception& e) {
            std::cerr << "Error during refresh: " << e.what() << "\n";
        }
    }

    return 0;
}
