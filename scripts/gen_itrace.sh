#!/bin/bash

grep "EXEC pc" ./img/npc-ysyxsoc-log.txt | awk '{sub(/EXEC pc = /, ""); print}' > img/itrace 
sed -i '1i\30000000' img/itrace

line_count=$(wc -l < img/itrace)
echo "Processed file has $line_count lines."