//
// Created by german on 24/11/25.
//
#include <map>
#include "../include/common.h"
#include "../include/process.h"

using namespace std;

namespace myProc {
    //Constructors
    Process::Process(const string &processName) {
        readStatFile(processName);
    }

    //Private methods
    void Process::readStatFile(const string &processName) {
        try {
            //Check error
            if (!commonLib::replace(this->fullProcessPath, processName)) {
                perror("Error searching for file");
                throw ProcessReadError("Failed reading /proc/ for pid: " + processName);
            }

            ifstream pidFile(fullProcessPath);
            string line;
            vector<string> elemVector;
            if (!pidFile.is_open()) {
                perror("Error reading file");
                throw ProcessFileError("Error opening file");
            }
            //get line and loop with spaces
            getline(pidFile, line);
            map<string, string> processMap = parseStatFile(line);

            //TODO: Change this to const and automate it
            this->name = processMap.at("Name");
            //string to int
            this->pid = stoi(processMap.at("Pos-1"));
            //TODO: Change how to cast string maybe to explicit cast
            this->state = commonLib::getStateString(processMap.at("Pos-2")[0]);

            pidFile.close();
        } catch (ProcessError &e) {
            std::cerr << "Process error: " << e.what() << "\n";
            throw ProcessError("Process error");//TODO: maybe this should be change to capture all possiblea exceptions
        }
    }

    map<string, string> Process::parseStatFile(const string &fileLine) {
        if (fileLine.empty()) {
            perror("Empty line provided");
            throw ProcessReadError("Empty line");
        }

        map<string, string> processesMap;
        //get stream from line and parse it
        stringstream ss(fileLine);
        string auxStr;
        int startNamePos = fileLine.find('(');
        int endNamePos = fileLine.find(')');
        string processName = fileLine.substr(startNamePos, endNamePos - 1);
        int cont = 0;
        while (ss >> auxStr) {
            if (auxStr.starts_with('(') || auxStr.ends_with(')')) {
                processesMap.insert({"Name", processName});
            } else {
                processesMap.insert({"Pos-" + to_string(++cont), auxStr});
            }
        }

        return processesMap;
    }

    //public methods
    void Process::refresh(string &pidFileName) {
        readStatFile(pidFileName);
    }

    void Process::print() const {
        cout << "PID: " << pid << " | NAME: " << name << " | STATE: " << state <<"\n";
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

    string Process::getState() {
        return this->state;
    }

    void Process::setState(const string& state) {
        this->state = state;
    }
}
