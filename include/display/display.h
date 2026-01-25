//
// Created by german on 25/1/26.
//

#ifndef MYPROCESSMANAGER_DISPLAY_H
#define MYPROCESSMANAGER_DISPLAY_H

#include "core/activeProcesses.h"
#include "core/systemMonitor.h"

namespace myProc {
    class Display {
    public:
        /**
         * Constructor
         */
        Display();

        /**
         * Main render method to call in main
         * @param monitor SystemMonitor
         * @param processes ActiveProcesses
         */
        void render(const SystemMonitor &monitor, const ActiveProcesses &processes);

    private:
        //TODO: render System metrics (CPU, Memory, etc...)

        //TODO: render Process List

        //TODO: Render header

        //TODO: Render footer
    };
}

#endif //MYPROCESSMANAGER_DISPLAY_H