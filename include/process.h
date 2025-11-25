//
// Created by german on 24/11/25.
//
#pragma once

#ifndef MYPROCESSMANAGER_PROCESS_H
#define MYPROCESSMANAGER_PROCESS_H

#include <string>;
#include <fstream>;

class Process {
private:
    pid_t pid;
    std::string name;

    void readStatFile(const std::string& pidFileName);

public:
    Process(const std::string& filePath);
    Process(const int &pid, const std::string &name);

    void refresh();
    void print();
};

#endif //MYPROCESSMANAGER_PROCESS_H