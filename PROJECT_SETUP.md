# Project setup and quick start

This repository contains a small skeleton for the DS2025 final project. It provides three example components you can expand:

- `datastructure/datastructure.cpp` — a demo program that will become your data structure demo (prints a hello message for now).
- `baseline/baseline.cpp` — a baseline implementation placeholder.
- `benchmark/benchmark.cpp` — a benchmark runner that reads `benchmark/data/sample_input.txt` and will be used to compare implementations.

Build
-----

Run `make` at the repository root to build all example executables:

```bash
make
```

This will create the following executables:

- `datastructure/datastructure.exe`
- `benchmark/benchmark.exe`
- `baseline/baseline.exe`

Run
---

After building you can run the small demos:

```bash
./datastructure/datastructure.exe
./baseline/baseline.exe
./benchmark/benchmark.exe
```

Extending the project
---------------------

- Replace the placeholder `main` functions with real demos and measurement harnesses.
- Put benchmark inputs under `benchmark/data/` and make the benchmark runner call the datastructure and baseline executables (or link them) to measure runtime.
- Update the `Makefile` as needed to add more targets or libraries.
