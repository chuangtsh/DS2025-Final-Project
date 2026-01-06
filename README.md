# Project setup and quick start

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

After building, you can run the small demos:

```bash
./datastructure/datastructure.exe
./baseline/baseline.exe
```

To run the benchmark:

```bash
./benchmark/benchmark.exe
```

Visualize the data structure implementation
---------------------
- `./datastructure/datastructure.exe`
  - This creates `.dot` files in `graph_dots/`
- Go to VSCode extensions, install `Graphviz Interactive Preview`
- Open those `.dot` files, and click an icon of the extension on the right side at the top of the page
  - Some icons alongside are `open changes`, `split editor right` ...
