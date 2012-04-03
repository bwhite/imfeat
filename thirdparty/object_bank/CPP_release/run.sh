rm ./outputs/*
make clean && make
./OBmain -M ModelList ./inputs/ ./outputs/
