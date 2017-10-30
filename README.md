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

Additionally you need to install `purrr`, which is not currently a
debian package. Thus you will need to install it using R package
system. Start R console and type `install.packages('purrr')`. Choose
where to install (for example into your home dir) and from which
server you would like to fetch packages. In principle you can replace
purrr's map with apply. Patches are welcome.

From a directory with the log run `Rscript ../script/plot_trace.R` it
will generate graph plot.pdf.

# Usage

Use "--help"

Possible parameters for --faults are "none" and "uniform"

Possible parameters for --collective are "binary_bcast" and
"correctedtree_bcast"

Example usage

```bash
./flogsim --P 128 --o 1 --L 4 --faults uniform --F 4 --coll correctedtree_bcast --help
```

# TODO

  - [ ] Add checked correction
  - [ ] If root node is dead, it still sends messages.
  - [ ] Implement nodeset
  - [ ] Compute time for phased bcast properly
  - [ ] Write tests
  - [ ] Add Idle task
