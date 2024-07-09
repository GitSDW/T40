# F/W Update
rm /tmp/mnt/sdcard/isc_bak
mv /tmp/mnt/sdcard/isc /tmp/mnt/sdcard/isc_bak
cp /dev/shm/isc /tmp/mnt/sdcard/isc
chmod 777 /tmp/mnt/sdcard/isc


# Tag Update
flashcp /dev/shm/tag.bin /dev/mtd1

sync
