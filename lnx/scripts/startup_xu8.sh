#!/bin/sh

function gpio_init()
{
GPIO_IDX=$((338+78+0)) #=416 gpio_emio[0]=FREE
echo ${GPIO_IDX} > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio${GPIO_IDX}/direction

GPIO_IDX=$((338+78+1)) #=417 gpio_emio[1]=FREE
echo ${GPIO_IDX} > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio${GPIO_IDX}/direction

GPIO_IDX=$((338+78+2)) #=418 gpio_emio[2]=FREE
echo ${GPIO_IDX} > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio${GPIO_IDX}/direction

GPIO_IDX=$((338+78+3)) #=419 gpio_emio[3]=GT_POWER_GOOD
echo ${GPIO_IDX} > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio${GPIO_IDX}/direction

GPIO_IDX=$((338+78+4)) #=420 gpio_emio[4]=LINK_STATUS
echo ${GPIO_IDX} > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio${GPIO_IDX}/direction

GPIO_IDX=$((338+78+5)) #=421 gpio_emio[5]=FREE
echo ${GPIO_IDX} > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio${GPIO_IDX}/direction

GPIO_IDX=$((338+78+6)) #=422 gpio_emio[6]=SYS_RST
echo ${GPIO_IDX} > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio${GPIO_IDX}/direction
echo 0 > /sys/class/gpio/gpio${GPIO_IDX}/value

GPIO_IDX=$((338+78+7)) #=423 gpio_emio[7]=GT_RST
echo ${GPIO_IDX} > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio${GPIO_IDX}/direction
echo 0 > /sys/class/gpio/gpio${GPIO_IDX}/value

for i in `seq 8 23` ; do 
  GPIO_IDX=$((338+78+$i)) #=424..439 gpio_emio[23: 8]=Firmware ID
  echo ${GPIO_IDX} > /sys/class/gpio/export
  echo in > /sys/class/gpio/gpio${GPIO_IDX}/direction
done
echo -n "GPIO init"
}

function led_init()
{
echo netdev > /sys/class/leds/led0/trigger
echo netdev > /sys/class/leds/led1/trigger
echo eth0 > /sys/class/leds/led0/device_name
echo eth0 > /sys/class/leds/led1/device_name
echo 1 > /sys/class/leds/led0/link
echo 1 > /sys/class/leds/led1/rx
echo 1 > /sys/class/leds/led1/tx
echo -n "LED init"
}

echo

case "$1" in
  start)
    gpio_init
    echo "."
    led_init
    echo "."
    ;;
  stop)
#    echo -n "Stopping startup script"
    ;;
  *)
    echo "ERROR:$0: Wrong autostartup script parameter($1)"
    exit 1
esac

exit 0

