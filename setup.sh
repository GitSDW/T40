# F/W Update
if [ -e "/dev/shm/system.bin" ]; then
	echo "F/W System Change"
	flashcp /dev/shm/system.bin /dev/mtd5
	echo "Flashcp Finish!!"
else
	echo "F/W Card Change"
	rm /tmp/mnt/sdcard/isc_bak
	mv /tmp/mnt/sdcard/isc /tmp/mnt/sdcard/isc_bak
	cp /dev/shm/isc /tmp/mnt/sdcard/isc
	chmod 777 /tmp/mnt/sdcard/isc
fi

# Tag Update
if [ -e "/dev/shm/tag.bin" ]; then
	echo "tag change start!"
	flashcp /dev/shm/tag.bin /dev/mtd1
	echo "Flashcp Finish!!!"
fi

# Executation Update
if [ -e "/dev/shm/vexe.sh" ]; then
	echo "vexe.sh chnge start!"
	cp /dev/shm/vexe.sh /tmp/mnt/sdcard
fi

# Mute Update
if [ -e "/dev/shm/mute_msg.sh" ]; then
	echo "Mute chnge start!"
	cp /dev/shm/mute_msg.sh /tmp/mnt/sdcard
fi

if [ -d "/dev/shm/effects" ]; then
	echo "Sound File Cnage!"
	cp /dev/shm/effects/*.wav /tmp/mnt/sdcard/effects
fi

sync
sync
