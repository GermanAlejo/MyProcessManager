
#include "activeProcesses.h"
#include "systemMonitor.h"

using namespace myProc;

int main() {
    //Testing
    //Process newProcessTest ("1");
//
    //newProcessTest.print();
    //newProcessTest.refresh(newProcessTest.getPid());
    SystemMonitor m;

    ActiveProcesses allProcesses;
    allProcesses.printProcessContainer();

    return 0;
}
