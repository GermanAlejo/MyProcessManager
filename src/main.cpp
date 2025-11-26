#include <iostream>
#include "../include/process.h"

int main() {
    //Testing
    Process newProcessTest ("1");
    Process newProcessTest2 ("5007");

    newProcessTest.print();
    newProcessTest2.print();

    return 0;
}
