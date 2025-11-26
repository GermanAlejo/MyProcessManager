//
// Created by german on 24/11/25.
//
#pragma once

#ifndef MYPROCESSMANAGER_PROCESS_H
#define MYPROCESSMANAGER_PROCESS_H

#include <common.h>

using namespace std;

class Process {
private:
    int pid{};
    string fullProcessPath;
    string name;

    void readStatFile(const std::string& processName);

public:
    Process(const string& processName);

    void refresh(string &pidFileName);
    void print() const;
    int getPid() const;
    string getName();
    string getFullProcessPath();
    void setPid(const int &pid);
    void setName(const string &name);
    void setFullProcessPath(const string &path);
};

#endif //MYPROCESSMANAGER_PROCESS_H