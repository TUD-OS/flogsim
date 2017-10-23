# Compilation

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

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

  - [ ] Add sequence ids to messages
  - [ ] Replace priority queue with vector
  - [ ] Add checked correction
  - [ ] If root node is dead, it still sends messages.
  - [ ] Fix visualization bug with arrows
  - [ ] Add number of messages send