#!/bin/bash

MYSQL=$MYSQL_DIR/bin/mysql
MYSQL_REQUEST="$MYSQL --no-defaults -u user -h $DBSERVER -puser flogsim"

# Create tables from scratch
MYSQL_REQUEST_ROOT="ssh $DBSERVER $MYSQL --no-defaults -u root -paoeuaoeu mysql"
if ! $MYSQL_REQUEST_ROOT < $SCRIPT_DIR/init.sql
then
    echo "Failed to create table: $exit_status"
    exit 1
fi

COMBINATIONS=$(eval echo "$COLL+$L+$o+$g+$P+$k+$F")
for EXPERIMENT in $COMBINATIONS
do
    read COLL L o g P k F <<<$(IFS="+"; echo $EXPERIMENT)

    CONDUCTED=0

    echo $COLL $L $o $g $P $k $F $TOTAL
    echo "INSERT INTO experiment_plan (COLL,k,L,o,g,P,F,conducted,total) VALUES (\"$COLL\",$k,$L,$o,$g,$P,$F,$CONDUCTED,$TOTAL)" | $MYSQL_REQUEST
done
