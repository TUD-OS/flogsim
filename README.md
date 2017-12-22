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

## Enabling tests

By default the simulator compiles without tests. If you want these,
add `-DUNIT_TESTS=ON` to cmake configuration.

# Usage

Use "--help"

Possible parameters for --faults are "none" and "uniform". If you want
explicitly specify list of failed nodes pass the node ids in comma
separated list, like `--faults 1,4,7`.

Possible parameters for --collective are "binary_bcast" and
"correctedtree_bcast"

Example usage

```bash
./flogsim --P 128 --o 1 --L 4 --faults uniform --F 4 --coll correctedtree_bcast
```

## Result printing

Please note that output formats print slightly different
information. The reason for that is purely practical.

Format 'csv' prints the results in a form of csv table. List of failed
nodes is not printed in this format.

Format 'csv-id' allows to save on experiment description. Instead the
user is expected to provide additional token, which identifies the
experiment as parameter '--id'.

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

If you run the simulator in repeat mode (makes only sense with failure
injections), trace log get additional prefix to mark run number.

# Fault injection experiments

Simulating fault injection experiments (FIE) requires significant
computation resources. That is why the way to do them is to run
experiments on taurus.

FIE requires a long running MySQL/MariaDB server running. I do not
describe the way to install the MySQL database. I just assume that you
have one installed.

The whole fault injection can be done by running script
`fault_tests.sh`.

Here is what it does.

  1. Sets BASE directory
  2. Starts the server (`faults_server.sh`)
  4. Queries the server name
  3. Optionally resets the database (`faults_create_plan.sh`)
  5. Start the tests (`faults_run.sh`)

Look onto exported variables in `fault_tests.sh` to configure FIE
parameters.

Be aware that the scripts were written with SLURM in mind.

# TODO

  - [ ] Write tests
  - [ ] Add list of failed nodes to the command line interface
  - [ ] Add various finishing events: All done, All received, etc.
  - [ ] Add ceil(L/g) network capacity limit

# Dropscon

  - [ ] Exponential backoff

# Code style

## Constructors

Write constructors with initialization lists as:

```C++
class A
{
  int c, d;
public:
  A()
    : c(0),
      d(0)
  {}
};
```

Mind the colon!

## Whitespace

 - Put space after coma
