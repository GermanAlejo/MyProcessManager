# Process Manager

## Entities

### Process

attributes:
- int pid: code of the process
- string name name of the process

construyctors:
- constructortaking a pid
- taking a pid and a name

methods
- readStatFile: opens stat file, reads like and parses fiels, then in constructors sets attributes with those fields
- refresh: re read file /proc/pid
- getters
- setters
- print()

FOR LATER:
double getCpuUsage()

size_t getMemoryUsage()

std::string getState()

exception-based error handling

operator<< overload for pretty-printing# MyProcessManager
