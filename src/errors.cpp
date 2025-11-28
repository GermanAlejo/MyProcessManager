//
// Created by german on 28/11/25.
//

#include "errors.h"

using namespace std;

ProcessError::ProcessError(const std::string& msg)
    : runtime_error(msg) {}

ProcessFileError::ProcessFileError(const string& msg)
: ProcessError(msg) {}


ProcessNotFound::ProcessNotFound(const string& msg)
    : ProcessError(msg) {}


ProcessAccessDenied::ProcessAccessDenied(const string& msg)
    : ProcessError(msg) {}

ProcessReadError::ProcessReadError(const string& msg)
    : ProcessError(msg) {}