//
// Created by german on 24/11/25.
//
#include "../include/common.h"
#include "../include/process.h"

using namespace std;

//Constructors
Process::Process(const std::string &filePath) {
    readStatFile(filePath);
}

Process::Process(const int &pid, const std::string &name) : pid(pid), name(name) {
}

//Private methods
void Process::readStatFile(const std::string &pidFileName) {
    ifstream pidFile(pidFileName);
    string line;
    vector<string> elemVector;
    if (!pidFile.is_open()) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    //get line and loop with spaces
    getline(pidFile, line);
    stringstream ss(line);
    string auxStr;
    while (ss >> auxStr) {
        elemVector.push_back(auxStr);
    }

    this->pid = stoi(elemVector.at(0));
    this->name = elemVector.at(1);

    pidFile.close();
}

//public methods
void Process::refresh() {
}

void Process::print() {
    cout << "PID: " << pid << "-NAME: " << name << "\n";
}
