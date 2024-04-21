#!/bin/bash

make clean
make -j4
sync
mv isc isc$1
zip isc$1.zip isc$1
sudo cp isc$1.zip /media/sf_Share/