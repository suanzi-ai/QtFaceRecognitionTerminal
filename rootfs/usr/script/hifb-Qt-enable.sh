#!/usr/bin/env sh

. /userdata/user.conf

cd /usr/sample
if [ $DISPLAY -ge 11 ];then
	#./hifb_en 3 &
	.
else
	#sh /usr/script/lcd-gpio.sh 1
	#sh /usr/script/lcd-pwm.sh 1
	sh /usr/script/mipi_tx_rest_our.sh 0
	#./hifb_en $LCD >&- &
fi




