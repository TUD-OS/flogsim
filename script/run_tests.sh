#!/bin/bash

PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"
FLOGSIM=$PROJECT_DIR/build/flogsim

./flogsim --coll phased_checked_corrected_lame_bcast  -L 2 -o 1 -g 1 -P $((16)) --faults none --log log  --parallelism 1  -k 2 --results-format csv -r 1 | head -n 1

ALL_P="{64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072}"
ALL_COLL="{checked_corrected_binomial_bcast,phased_checked_corrected_binomial_bcast,checked_corrected_optimal_bcast,phased_checked_corrected_optimal_bcast,checked_corrected_lame_bcast,phased_checked_corrected_lame_bcast}"
ALL_L="{1,2}"
ALL_PAR="{1,2}"
ALL_K="2"

COMBINATIONS=$(eval echo "$ALL_COLL+$ALL_L+$ALL_P+$ALL_PAR+$ALL_K")

ALL_COLL="{phased_checked_corrected_kary_bcast,checked_corrected_kary_bcast}"
ALL_K="4"
COMBINATIONS="$COMBINATIONS "$(eval echo "$ALL_COLL+$ALL_L+$ALL_P+$ALL_PAR+$ALL_K")

for EXPERIMENT in $COMBINATIONS
do
    read COLL L P PAR k <<<$(IFS="+"; echo $EXPERIMENT)
    ./flogsim --parallelism $PAR -k $k -P $P -o 1 -g 1 -L $L --faults none --coll $COLL --results-format csv -r 1 | tail -n 1
done
