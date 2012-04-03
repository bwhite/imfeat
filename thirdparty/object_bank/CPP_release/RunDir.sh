#!/bin/bash
#./RunDir.sh dirlist ../Indoor_Images/ outputs/ 17
#            input list file | base input directory | base output directory | target index
INPUTBASEDIR=$1;
OUTPUTBASEDIR=$2;

for f in `ls $INPUTBASEDIR`  
do
    mkdir $OUTPUTBASEDIR$f/;
    ./OBmain $INPUTBASEDIR$f/ $OUTPUTBASEDIR$f/;
done

