//
// Created by german on 28/11/25.
//


#pragma once
#include <stdexcept>

#ifndef MYPROCESSMANAGER_ERRORS_H
#define MYPROCESSMANAGER_ERRORS_H

class ProcessError : public std::runtime_error {
public:
    explicit ProcessError(const std::string& msg);
};

class ProcessFileError : public ProcessError {
public:
    explicit ProcessFileError(const std::string& msg);
};

class ProcessNotFound : public ProcessError {
public:
    explicit ProcessNotFound(const std::string& msg);
};

class ProcessAccessDenied : public ProcessError {
public:
    explicit ProcessAccessDenied(const std::string& msg);
};

class ProcessReadError : public ProcessError {
public:
    explicit ProcessReadError(const std::string& msg);
};

#endif //MYPROCESSMANAGER_ERRORS_H