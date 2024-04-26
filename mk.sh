#!/bin/bash

make clean
make -j4
sync
rm isc*.zip
zip isc$1.zip isc
sudo cp isc$1.zip /media/sf_Share/
echo "isc$1.zip Make Success!"