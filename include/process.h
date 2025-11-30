//
// Created by german on 24/11/25.
//
#pragma once

#ifndef MYPROCESSMANAGER_PROCESS_H
#define MYPROCESSMANAGER_PROCESS_H

#include <unordered_map>

using namespace std;

namespace myProc {
    class Process {
    private:
        string pid {};
        //values from stat
        string name;
        string state;
        string utime;//time scheduled in user mode
        string stime;//time scheduled in kernel mode
        string startTime;//time process started after boot
        //values from status
        string VmRSS; //resident memory
        string VmSize; //virtual memory

        void readStatFile(const string& processNumber);
        unordered_map<string, string> parseStatFile(const string& fileLine);
        void readStatusFile(const string& processNumber);
        static unordered_map<string, string> parseStatusFile(ifstream& file);

    public:
        Process(const string& processName);
        void refresh(const string &pidFileName);
        void print() const;
        //getters & setters
        string getPid() const;
        string getName();
        string getState();
        string getUtime();
        string getsTime();
        string getStartTime();
        string getVmRSS();
        string getVmSize();
        void setPid(const string &pid);
        void setName(const string &name);
        void setState(const string& state);
        void setUtime(const string& utime);
        void setStime(const string& stime);
        void setStartTime(const string& startTime);
        void setVmRSS(const string& vmRss);
        void setVmSize(const string& VmSize);
    };
}

#endif //MYPROCESSMANAGER_PROCESS_H