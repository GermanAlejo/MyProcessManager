#include <iostream>

#include "common.h"
#include "../include/process.h"

using namespace myProc;

int main() {
    //Testing
    Process newProcessTest ("1");

    newProcessTest.print();
    newProcessTest.refresh(newProcessTest.getPid());

    return 0;
}
