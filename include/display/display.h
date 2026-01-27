//
// Created by german on 25/1/26.
//

#ifndef MYPROCESSMANAGER_DISPLAY_H
#define MYPROCESSMANAGER_DISPLAY_H

#include <memory>
#include <spdlog/spdlog.h>

#include "core/activeProcesses.h"
#include "core/systemMonitor.h"

namespace myProc {
    class Display {
    public:
        /**
         * Constructor, initializes the Monitor anc process container
         */
        Display(SystemMonitor& system_monitor, ActiveProcesses& active_processes);

        /**
         * Main render method to call in main
         */
        void render();

    private:
        //Logger
        std::shared_ptr<spdlog::logger> console;
        //Processes & Monitor
        ActiveProcesses& current_active_processes;
        SystemMonitor& current_system_monitor;

        /**
         * Outputs the Global system metrics
         */
        void renderSystemMetrics() const;

        //TODO: render Process List
        void renderProcessesList();

        /**
         * Outputs the Header of the manager
         */
        void renderHeader() const;

        /**
         * Outputs the footer of the Manager
         */
        void renderFooter() const;
    };
}

#endif //MYPROCESSMANAGER_DISPLAY_H