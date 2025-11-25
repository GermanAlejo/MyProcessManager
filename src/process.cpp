//
// Created by german on 24/11/25.
//

#include <iostream>
#include <string>;
#include <fstream>;
#include "../include/process.h";

//Constructors
Process::Process(const std::string& filePath) {
    readStatFile(filePath);
}

Process::Process(const int &pid, const std::string &name): pid(pid), name(name) {}

//Private methods
void Process::readStatFile(const std::string& pidFileName) {
    std::ifstream pidFile(pidFileName);
    std::string line;
    if (pidFile.is_open()) {
        while (getline(pidFile, line)) {
            std::cout << line << "\n";
        }
    }
    pidFile.close();
}

//public methods
void Process::refresh() {

}

void Process::print() {
    std::cout << "PID: " << pid << "NAME: " << name << "\n";
}




