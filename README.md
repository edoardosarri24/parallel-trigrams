# parallel-trigrams

### Requirements
- CMake
- The [GPerfTools](https://developer.ridgerun.com/wiki/index.php/Profiling_with_GPerfTools).
- The modern Go version of [pprof](https://github.com/google/pprof)
- OpenMP
- Go language.

### Installation
- Set your pprof path in [profiling.sh](exec/profiling.sh) executable.

### Execution
Use the scripts in the `exec/` directory to build and run the project. All scripts require a mandatory argument: `seq` or `par`.
- Run (Release): `./exec/release_execution.sh [seq|par]` - Builds in **Release** mode (optimized with `-O3 -march=native`) and runs the application. Use this for performance measurements.
- Run (Debug): `./exec/debug_execution.sh [seq|par]` - Builds in default/Debug mode. Use this for development and debugging.
- Profile: `./exec/profiling.sh [seq|par]` - Builds in **Release** mode with profiling enabled, runs, and generates reports in `[sequential|parallel]/result_profiling/`.
- Sanitizers:
  - `./exec/AUBsanitizer.sh [seq|par]` - Runs with Address and Undefined Behavior Sanitizers (Debug mode).
  - `./exec/Msanitizer.sh [seq|par]` - Runs with Memory Sanitizer (Debug mode).