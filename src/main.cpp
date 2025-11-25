#include <iostream>
#include <fstream>

int main() {
    //Testing
    std::ifstream pidFile("/proc/1/stat");
    std::string line;
    if (pidFile.is_open()) {
        while (getline(pidFile, line)) {
            std::cout << line << "\n";
        }
    } else std::cout << "Unable to open file";
    pidFile.close();

    return 0;
}
