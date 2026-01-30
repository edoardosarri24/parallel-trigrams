# parallel-trigrams

This project counts n-grams (sequences of words, defaulting to trigrams) in large text files. It includes both a sequential and a parallel implementation to compare performance. The system normalizes text and uses a custom Hash Table to efficiently calculate and report statistics, such as the most frequent n-grams.

All is documented in [report](https://github.com/edoardosarri24/parallel-n_grams/blob/master/report.pdf) file.

### Requirements

To build and run this project, you need the following software:
- **CMake** (3.21 or newer).
- A **C Compiler** with **C23** and **OpenMP** support (e.g., GCC, Clang).
- **POSIX-compliant OS** (Linux, macOS) due to `mman.h` usage.
- **Python** (3.12 or newer) for downloading input files.
- **[uv](https://docs.astral.sh/uv/)** for Python dependency management.
- Optional
  - **[Gperftools](https://github.com/gperftools/gperftools)** (specifically `libprofiler`).
  - **Go** and **[pprof](https://github.com/google/pprof)** for generating performance reports.

### Installation
- Edit `exec/profiling.sh` and set the `PPROF_PATH` variable to your pprof executable location.
  To find it, you can run: `which pprof` (after installing Go/pprof).

### Execution
The project execution is a two-step process:

##### Data Download
  Before running the application, download the input dataset with the following command:
```bash
./exec/download_input.sh [size_in_mb]
```
- size_in_mb: Optional. Target size in Megabytes (default: 1024).
- *Note:* The script checks if `data/input.txt` already exists to avoid redundant downloads.

##### Build & Run
Use the scripts in the `exec/` directory to build and run the project automatically.
- Performance (Release):
    ```bash
    ./exec/release_execution.sh [seq|par]
    ```
    Builds with `-O3 -march=native` for maximum performance. Use this for benchmarking.
- Development (Debug):
    ```bash
    ./exec/debug_execution.sh [seq|par]
    ```
    Builds with debug symbols and no optimizations. Best for debugging logic.
- Profiling:
    ```bash
    ./exec/profiling.sh [seq|par]
    ```
    Builds in Release mode with profiling enabled. Generates CPU profile reports (PDF/Text) in `[sequential|parallel]/result_profiling/`.
- Sanitizers (Debug):
    *   `./exec/AUBsanitizer.sh [seq|par]` - Runs with Address and Undefined Behavior Sanitizers.
    *   `./exec/Msanitizer.sh [seq|par]` - Runs with Memory Sanitizer (Linux only, requires Clang).

### Structure
```text
.
├── CMakeLists.txt      # Main project build configuration.
├── README.md           # Project README.
├── agent.md            # Agent role definition and instructions.
├── report.pdf          # Report with all details and ideas.
├── exec/               # Scripts for building, running, and profiling.
├── parallel/           # Source code for the parallel implementation.
├── report/             # LaTeX source for the project report.
└── sequential/         # Source code for the sequential implementation.
```