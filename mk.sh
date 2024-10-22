#!/bin/bash

make clean
make -j4
sync
rm isc*.zip
rm system.bin
cp /home/t40/t40/Zeratul_T40_Release_20230410/out/camera/system.bin ./
zip isc$1.zip isc setup.sh system.bin
sudo cp isc$1.zip /media/sf_Share/
echo "isc$1.zip Make Success!"