/usr/script/hifb-Qt-enable.sh
insmod /usr/ko/hi_cipher.ko
. /usr/qt5.12/qt_env.sh
/user/quface-app/bin/supervisord -c /user/quface-app/etc/supervisord.conf -d
echo "end of sample.sh"