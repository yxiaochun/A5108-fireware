adb push bcmdhd_sta.bin /data/local/tmp
adb shell "su -c mount -o remount,rw /system"
adb shell "su -c cp /data/local/tmp/bcmdhd_sta.bin /system/etc/wifi"
adb shell "su -c chmod 644 /system/etc/wifi/bcmdhd_sta.bin"


adb shell "su -c ifconfig wlan0 down"
adb shell "su -c ifconfig wlan0 up"

adb shell "su -c /data/local/tmp/dhdutil -i wlan0 consoledump"

