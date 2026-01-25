# MyProcessManager

**Project overview**
- MyProcessManager is a small Linux system/process monitoring utility that reads procfs files (primarily `/proc/meminfo`, `/proc/stat` and `/proc/uptime`) to compute and expose system metrics such as RAM usage, CPU usage and uptime. The codebase is organized as a C++ project built with CMake and aims to provide programmatic access to per-system metrics and a process model used by higher-level tooling or UI.

**Architecture (high level)**
- `src/` and `include/`: organized hierarchically into modules for better scalability and clarity.
- **Core Module** (`include/core/`, `src/core/`):
  - `SystemMonitor` (`include/core/systemMonitor.h`, `src/core/systemMonitor.cpp`): reads procfs, parses `/proc/meminfo`, `/proc/stat`, `/proc/uptime`, computes derived values (used RAM, percentage, GiB, CPU percentage) and exposes getters.
  - `CpuSnapShot` (`include/core/cpuSnapShot.h`, `src/core/cpuSnapShot.cpp`): small value object that holds cumulative CPU counters parsed from `/proc/stat`.
  - `activeProcesses` (`include/core/activeProcesses.h`, `src/core/activeProcesses.cpp`): manages active process tracking.
- **Common Module** (`include/common/`, `src/common/`):
  - `common` (`include/common/common.h`, `src/common/common.cpp`): helper functions (string utilities, path getters, wait functions, parsing helpers for uptime, etc.).
  - `errors` (`include/common/errors.h`, `src/common/errors.cpp`): exception types used across the project (`ProcessFileError`, `ProcessReadError`, `ProcessError`, ...).
- **Process Module** (`include/process/`, `src/process/`):
  - `process` (`include/process/process.h`, `src/process/process.cpp`): domain model for processes.
  - `process_types` (`include/process/process_types.h`, `src/process/process_types.cpp`): metadata and types for system fields.
- `main.cpp` (`src/main.cpp`): entrypoint / example runner that wires components together.

**Data flow**
- `SystemMonitor` reads files from procfs using `common` helpers and `parse*` functions, constructs `CpuSnapShot` instances to represent counters, and computes deltas between snapshots to derive CPU usage. Memory fields are parsed, sanitized, and converted from kB to GiB/percentages.

CPU calculation, see SystemMonitor.md

**Build & run (CMake-focused)**
- Prerequisites:
  - A C++23-capable compiler (GCC >= 13, Clang >= 16, or equivalent).
  - CMake (>= 3.12 required).
  - Build system: `ninja` (required for CLion compatibility).

- **Generator choice**: We use **Ninja** as the CMake generator for consistency with CLion IDE. This ensures that command-line builds and CLion builds use the same generator, avoiding compatibility issues.

- Typical build steps (from project root):

**Option 1: Using Ninja (Recommended for CLion compatibility)**

```bash
# Create build directory
mkdir -p build && cd build

# Configure CMake with Ninja generator (Debug build)
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..

# Or for Release build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..

# Build the project
cmake --build . -- -j$(nproc)
```

**Option 2: Using Unix Makefiles (Alternative)**

```bash
# Create build directory
mkdir -p build && cd build

# Configure CMake with Unix Makefiles (Debug build)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Or for Release build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project
cmake --build . -- -j$(nproc)
```

- To run the executable:

```bash
./MyProcessManager
```

- To clean and rebuild:

```bash
rm -rf build
mkdir build && cd build

# Using Ninja
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(nproc)

# Or using Unix Makefiles
# cmake -DCMAKE_BUILD_TYPE=Release ..
# cmake --build . -- -j$(nproc)
```

- Installation requirements:
  - For Ninja: `sudo apt-get install ninja-build` (Ubuntu/Debian)
  - For Unix Makefiles: `sudo apt-get install build-essential` (Ubuntu/Debian)

Dependencies
- Runtime / compile-time dependencies inferred from the codebase:
  - C++ standard library (C++20)
  - `spdlog` (logging) — used in `systemMonitor.cpp` and other files; ensure `spdlog` is available to CMake (either system installed, submodule or fetched by CMake script).
  - No external package manifests are included in the repository root; CMake is expected to locate and link required libs.

**Project layout**

```
MyProcessManager/
├── CMakeLists.txt                 # Top-level build configuration
├── README.md                      # Project readme
├── docs/                          # Documentation
│   ├── Project.md                 # This file
│   ├── Architecture.md            # Architecture diagram
│   └── SystemMonitor.md           # CPU calculation details
├── include/                       # Public headers (organized by module)
│   ├── core/
│   │   ├── systemMonitor.h
│   │   ├── cpuSnapShot.h
│   │   └── activeProcesses.h
│   ├── common/
│   │   ├── common.h
│   │   └── errors.h
│   └── process/
│       ├── process.h
│       └── process_types.h
├── src/                           # Implementation files (organized by module)
│   ├── main.cpp                   # Entry point
│   ├── core/
│   │   ├── CMakeLists.txt         # Core module build config
│   │   ├── systemMonitor.cpp
│   │   ├── cpuSnapShot.cpp
│   │   └── activeProcesses.cpp
│   ├── common/
│   │   ├── CMakeLists.txt         # Common module build config
│   │   ├── common.cpp
│   │   └── errors.cpp
│   └── process/
│       ├── CMakeLists.txt         # Process module build config
│       ├── process.cpp
│       └── process_types.cpp
└── build/                         # Build directory (generated by CMake)
```

Each module has its own `CMakeLists.txt` for better organization and maintainability.

