#!/bin/bash
#./ClusterExmaple.sh dirlist ../Indoor_Images/ outputs/ 17
#            input list file | base input directory | base output directory | target index
DIRFILE=$1;
INPUTBASEDIR=$2;
OUTPUTBASEDIR=$3;
TARGET=$4;
CTR=0
for dir in $(cat $DIRFILE)
do
if [ $CTR -eq $TARGET ]; # || $TARGET=='']
then
mkdir -p $OUTPUTBASEDIR$dir;
# ./OBmain -P 1 $INPUTBASEDIR$dir/ $OUTPUTBASEDIR$dir/
./OBmain $INPUTBASEDIR$dir/ $OUTPUTBASEDIR$dir/ 
fi

CTR=$(($CTR+1))
done
