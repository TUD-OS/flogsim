# Compilation

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

To generate visualization from plots you need R. With all the packages
this will require about 100 Mb of space on your disk.

In Debian you would need to install following packages:

  - r-base
  - r-cran-data.table
  - r-cran-ggplot2
  - r-cran-tidyr
  - r-cran-reshape2
  - r-cran-optparse

# Usage

Use "--help"

Possible parameters for --faults are "none" and "uniform"

Possible parameters for --collective are "binary_bcast" and
"correctedtree_bcast"

Example usage

```bash
./flogsim --P 128 --o 1 --L 4 --faults uniform --F 4 --coll correctedtree_bcast --help
```

# Visualisation

You can use `../script/plot_trace.R` script to visualize generated
traces. Run the script with `--help` to see all available options.

The typical usage will be:

```bash
../script/plot_trace.R -m ./log.model.csv ./log.trace.csv
```

Will by default create file `plot.pdf` out of model `log.model.csv`
and trace `log.trace.csv`.

If you are overwelmed with number of messages, you can either disable
them using `-c` flag, or only show specific nodes using `--messages`
flag.

# TODO

  - [ ] Add checked correction
  - [ ] If root node is dead, it still sends messages.
  - [ ] Implement nodeset
  - [ ] Write tests
  - [ ] Add list of failed nodes to the command line interface
  - [ ] Add various finishing events: All done, All received, etc.
