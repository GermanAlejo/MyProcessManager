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
         * Constructor
         * @param active_processes
         * @param system_monitor
         */
        Display();

        /**
         * Main render method to call in main
         */
        void render();

        void testRender();

    private:

        //Logger
        std::shared_ptr<spdlog::logger> console;
        //Processes & Monitor
        ActiveProcesses current_active_processes;
        SystemMonitor current_system_monitor;

        //TODO: render System metrics (CPU, Memory, etc...)
        void renderSystemMetrics() const;

        //TODO: render Process List
        void renderProcessesList();

        //TODO: Render header
        void renderHeader() const;

        //TODO: Render footer
        void renderFooter() const;
    };
}

#endif //MYPROCESSMANAGER_DISPLAY_H