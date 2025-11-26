//
// Created by german on 24/11/25.
//
#include "../include/common.h"
#include "../include/process.h"

using namespace std;

//Constructors
Process::Process(const string &processName) {
    readStatFile(processName);
}

//Private methods
void Process::readStatFile(const string &processName) {
    //Check error
    if (!replace(this->fullProcessPath ,processName)) {
        perror("Error searching for file");
        exit(EXIT_FAILURE);
    }
    ifstream pidFile(fullProcessPath);
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
void Process::refresh(string &pidFileName) {
    readStatFile(pidFileName);
}

void Process::print() const {
    cout << "PID: " << pid << "-NAME: " << name << "\n";
}

//getters & setters
int Process::getPid() const {
    return this->pid;
}

void Process::setPid(const int &pid) {
    this->pid = pid;
}

string Process::getName() {
    return this->name;
}

void Process::setName(const string &name) {
    this->name = name;
}


