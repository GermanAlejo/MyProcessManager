# **SystemMonitor**

Purpose
- **Description:** Monitors Linux system-level metrics used by MyProcessManager: RAM usage, CPU usage and uptime. Reads from the usual procfs files (e.g. `/proc/meminfo`, `/proc/stat`, `/proc/uptime`) and exposes simple getters for tests or UI consumption.

Fields (important ones)
- **`totalProcesses`**: int — total number of processes observed.
- **`totalRam`**: unsigned long — total RAM read from `/proc/meminfo` (stored in kB).
- **`availableRam`**: unsigned long — available RAM (kB).
- **`usedRam`**: unsigned long — used RAM (kB).
- **`usedRamGiB`**: double — used RAM converted to GiB (see conversion below).
- **`usedRamPercentage`**: double — percentage of RAM used (0..100).
- **`totalCPU`**: double — last computed total CPU usage percentage (0..100).
- **`uptime`**: uint64_t — system uptime in seconds.

Key Methods
- **`SystemMonitor()`**: constructor — reads meminfo, uptime and stat once to initialise values.
- **`refresh()`**: (TODO in-code) intended to refresh readings.
- **`readMemInfo()` / `parseMemInfo()`**: read and parse `/proc/meminfo` into a map and populate fields.
- **`readUpTime()`**: read `/proc/uptime` via helper and sets `uptime`.
- **`readStatFile()` / `parseStatFile()`**: read `/proc/stat` and build a `CpuSnapShot` representing CPU counters.
- **`calculateRamFinalValues()`**: derives used RAM, percentages and GiB values from `totalRam` and `availableRam`.
- **`calculateTotalCPU(const CpuSnapShot &newSnapShot)`**: compares the previously stored `CpuSnapShot` with a newly read snapshot to compute total CPU usage.

Notes on memory conversions
- The code stores memory values from `/proc/meminfo` in kB. To convert to GiB the code divides by `1024 * 1024` (i.e. kB -> GiB via: kB / 1024 / 1024). The method `calculateRamFinalValues()` also computes the used percentage and sets these fields.

CPU usage formula (used in `calculateTotalCPU`)

The implementation computes CPU usage by comparing two snapshots of the cumulative CPU counters and applying the standard idle/non-idle delta method. Using the naming conventions from the code:

$$\text{PrevIdle} = \text{prevIdle} + \text{prevIowait}$$
$$\text{Idle} = \text{idle} + \text{iowait}$$

$$\text{PrevNonIdle} = \text{prevUser} + \text{prevNice} + \text{prevSystem} + \text{prevIrq} + \text{prevSoftirq} + \text{prevSteal}$$
$$\text{NonIdle} = \text{user} + \text{nice} + \text{system} + \text{irq} + \text{softirq} + \text{steal}$$

$$\text{PrevTotal} = \text{PrevIdle} + \text{PrevNonIdle}$$
$$\text{Total} = \text{Idle} + \text{NonIdle}$$

Differentiate (current minus previous):
$$\text{totald} = \text{Total} - \text{PrevTotal}$$
$$\text{idled} = \text{Idle} - \text{PrevIdle}$$

CPU usage fraction (0..1):
$$\text{CPU\_fraction} = \frac{\text{totald} - \text{idled}}{\text{totald}}$$

The code multiplies this by 100 to produce a percentage:
$$\text{CPU\_percentage} = \text{CPU\_fraction} \times 100$$

Practical notes
- The implementation reads `/proc/stat` twice with a one-second wait between reads to compute the delta. It stores the first snapshot in `lastCpuRead` and reads a second snapshot into a local `CpuSnapShot` to compute the delta.
- Error handling: file access/parsing errors throw `ProcessFileError` / `ProcessReadError` (see project's `errors.h`).
- The CPU calculation protects against negative results by clamping negative computed percentages to zero.

