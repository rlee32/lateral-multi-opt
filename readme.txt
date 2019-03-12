This implementation utilizes multiple heuristics to find local optima and low-cost perturbations.

Currently, 2-opt and v-opt are implemented.

Use plot.py to visualize tsp instances and tours.

Compilation:
1. Make sure "CXX" in "makefile" is set to the desired compiler.
2. Run "make".

Running:
1. Run "./2-opt.out" for usage details.

Style notes:
1. Namespaces follow directory structure. If an entire namespace is in a single header file, the header file name will be the namespace name.
2. Headers are grouped from most to least specific to this repo (e.g. repo header files will come before standard library headers).
3. Put one line break in between function definitions for convenient vim navigation via ctrl + { and ctrl + }.

TODO:
1. Check if output directory exists before writing out paths to file (currently silently fails).



