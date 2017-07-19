#!/usr/bin/env bash

clear
echo "Hello $USER"
echo "Today is \c ";date

EXEC=ADDR_FOR_LSH
TEST_INPUT_PRE=TEST
TEST_TEMPALTE=./test_tmp
INPUT_PRE=PAGEII_Chr
INPUT_SUF_MAP=.phased.filtered.map3
INPUT_SUF_PED=.phased.filtered.ped
OUTPUT_BASE=result
LOG_BASE=LOG

for(( i = 1 ; i <= 2 ; i++))
do
#echo "map $INPUT_PRE$i$INPUT_SUF_MAP"
#echo "ped $INPUT_PRE$i$INPUT_SUF_PED"
test_addr="$TEST_INPUT_PRE$i"
echo $test_addr
cat $TEST_TEMPALTE > $test_addr
echo "map $INPUT_PRE$i$INPUT_SUF_MAP" >> $test_addr
echo "ped $INPUT_PRE$i$INPUT_SUF_PED" >> $test_addr
echo "output $OUTPUT_BASE$i" >> $test_addr
time $EXEC $test_addr > $LOG_BASE$i
done

echo Everything is "done" successfully