#include <iostream>
#include "../include/process.h"

int main() {
    //Testing
    Process newProcessTest ("/proc/1/stat");
    Process newProcessTest2 ("/proc/5007/stat");

    newProcessTest.print();
    newProcessTest2.print();

    return 0;
}
