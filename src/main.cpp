#include <iostream>
#include <ranges>

#include "activeProcesses.h"
#include "common.h"
#include "../include/process.h"

using namespace myProc;

int main() {
    //Testing
    //Process newProcessTest ("1");
//
    //newProcessTest.print();
    //newProcessTest.refresh(newProcessTest.getPid());

    ActiveProcesses allProcesses;
    allProcesses.printProcessContainer();

    return 0;
}
