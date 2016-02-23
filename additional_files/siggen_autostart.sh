#! /bin/sh
# /etc/init.d/siggen_autostart
echo 51 > /sys/class/gpio/export
chmod 777 /sys/class/gpio/gpio51/value
cat /sys/class/gpio/gpio51/value >> /etc/pwm/log.txt

if grep -Fxq "1" /sys/class/gpio/gpio51/value 
then
	echo 'laeuft' >> log2.txt
	/root/siggen &
	
else
	echo "laeuft nicht" >> /etc/pwm/log2.txt

fi
