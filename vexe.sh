trap '' SIGINT SIGTSTP

# ifconfig eth0 192.168.0.109 &
insmod /tmp/mnt/sdcard/soc-nna.ko &
insmod /tmp/mnt/sdcard/mpsys.ko &
insmod /tmp/mnt/sdcard/sample_pwm_core.ko
insmod /tmp/mnt/sdcard/sample_pwm_hal.ko
mkdir /usr/lib
cp /tmp/mnt/sdcard/hlib/libaudioProcess.so /usr/lib
# cp /tmp/mnt/sdcard/hlib/libaudioProcess.so /lib
mkdir /vtmp
mount -t tmpfs -o size=20M tmpfs /vtmp
# logcat &
# while true;do cat /proc/jz/isp/isp-w02;sleep 1;done &
# while true;do cat /proc/jz/isp/isp-m0;sleep 1;done &
# while true;do cat /proc/jz/isp/isp-fs;sleep 1;done &
# while true;do ./tmp/mnt/sdcard/impdbg --enc_info;sleep 1;done &
# while true;do ./tmp/mnt/sdcard/impdbg --sys;sleep 1;done &
echo "0 4 1 7" > /proc/sys/kernel/printk

program="/system/bin/isc"
program_bak="/tmp/mnt/sdcard/isc_bak"

log_file="/tmp/mnt/sdcard/fault.log"

#$program
#exit_code=$?

if [ -e "/system/bin/isc" ]; then
	echo "isc file check"
	$program &
	exit_code=$?
	echo "Program end !!!!!! $exit_code !!!!!!!!"
	if [ $exit_code -ne 0 ]; then
		echo "Program Fault!! Backup Program Execute!!"
		$program_bak &
	fi
else
	if [ -e "/tmp/mnt/sdcard/isc_bak" ]; then
		$program_bak &
	else
		echo "The Executable File Not Exist!!"
	fi
fi

/tmp/mnt/sdcard/./mute_msg.sh
