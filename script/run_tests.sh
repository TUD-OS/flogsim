#!/bin/bash

PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"
FLOGSIM=$PROJECT_DIR/build/flogsim

echo "P,Collective,L,TotalRuntime,FailedNodes,FinishedNodes,UnreachedNodes,MsgTask,FailedNodeList"


for L in 1 2
do
    for COLL in binary_bcast checked_correctedtree_bcast phased_checked_correctedtree_bcast fixed_correctedtree_bcast
    do
        for P in 15 31 63 127 255 511 1023 2047
        do
            OUT="$(./flogsim --P $P --o 1 --g 1 --L $L --faults none --coll $COLL)"
            OUT="P,$P"$'\n'"Collective,$COLL"$'\n'"L,$L"$'\n'"$OUT"
            echo "$OUT" | sed -e 's/[^,]*,//g' | tr '\n' ',' | sed -e 's/,$/\n/g'
        done
    done
done
