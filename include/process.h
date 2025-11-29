//
// Created by german on 24/11/25.
//
#pragma once

#ifndef MYPROCESSMANAGER_PROCESS_H
#define MYPROCESSMANAGER_PROCESS_H

#include <map>
#include <common.h>

using namespace std;

namespace myProc {
    class Process {
    private:
        int pid{};
        string fullProcessPath;
        string name;
        string state;
        string utime;//time scheduled in user mode
        string stime;//time scheduled in kernel mode
        string startTime;//time process started after boot

        void readStatFile(const string& processName);
        static map<string, string> parseStatFile(const string& fileLine);

    public:
        Process(const string& processName);
        void refresh(string &pidFileName);
        void print() const;
        //getters & setters
        int getPid() const;
        string getName();
        string getFullProcessPath();
        string getState();
        string getUtime();
        string getsTime();
        string getStartTime();
        void setPid(const int &pid);
        void setName(const string &name);
        void setFullProcessPath(const string &path);
        void setState(const string& state);
        void setUtime(const string& utime);
        void setStime(const string& stime);
        void setStartTime(const string& startTime);
    };
}

#endif //MYPROCESSMANAGER_PROCESS_H