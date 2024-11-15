# F/W Update
if [ -e "/dev/shm/system.bin" ]; then
	echo "F/W System Change"
	flashcp /dev/shm/system.bin /dev/mtd5
	echo "Flashcp Finish!!"
fi

if [ -e "/dev/shm/isc" ]; then
	echo "F/W SD Card Change"
	# rm /tmp/mnt/sdcard/isc_bak
	# mv /tmp/mnt/sdcard/isc /tmp/mnt/sdcard/isc_bak
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
	echo "vexe.sh change start!"
	cp /dev/shm/vexe.sh /tmp/mnt/sdcard
fi

# Mute Update
if [ -e "/dev/shm/mute_msg.sh" ]; then
	echo "Mute change start!"
	cp /dev/shm/mute_msg.sh /tmp/mnt/sdcard
fi

# if [ -d "/dev/shm/effects" ]; then
# 	echo "Sound File Change!"
# 	cp /dev/shm/effects/*.wav /tmp/mnt/sdcard/effects
# fi

if [ -e "/dev/shm/effects.zip" ]; then
	echo "Sound File All Change!"
	rm /tmp/mnt/sdcard/effects -rf
	rm /dev/shm/effects -rf
	mkdir /dev/shm/effects
	unzip /dev/shm/effects.zip -d /dev/shm/effects
	cp /dev/shm/effects /tmp/mnt/sdcard -r
fi

# if [ -e "/etc/vexe.sh" ]; then
# 	echo "Trash Clear for Flash System!"
# 	rm /tmp/mnt/sdcard/isc
# 	rm /tmp/mnt/sdcard/vexe.sh
# 	rm /tmp/mnt/sdcard/must_msg.sh
# fi

sync

sleep 3

sync