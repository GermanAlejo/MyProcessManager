//
// Created by german on 28/11/25.
//


#pragma once
#include <stdexcept>

#ifndef MYPROCESSMANAGER_ERRORS_H
#define MYPROCESSMANAGER_ERRORS_H

using namespace std;

class ProcessError : public runtime_error {
public:
    explicit ProcessError(const string& msg);
};

class ProcessFileError : public ProcessError {
public:
    explicit ProcessFileError(const string& msg);
};

class ProcessNotFound : public ProcessError {
public:
    explicit ProcessNotFound(const string& msg);
};

class ProcessAccessDenied : public ProcessError {
public:
    explicit ProcessAccessDenied(const string& msg);
};

class ProcessReadError : public ProcessError {
public:
    explicit ProcessReadError(const string& msg);
};

#endif //MYPROCESSMANAGER_ERRORS_H