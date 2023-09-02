#!/bin/sh

function gpio_init()
{
for i in `seq 0 7` ; do 
  GPIO_IDX=$((338+78+$i)) #=416..423 gpio_emio[ 7: 0]=GPIO_STATUS
  echo ${GPIO_IDX} > /sys/class/gpio/export
  echo in > /sys/class/gpio/gpio${GPIO_IDX}/direction
done

for i in `seq 8 23` ; do 
  GPIO_IDX=$((338+78+$i)) #=424..439 gpio_emio[23: 8]=Firmware ID
  echo ${GPIO_IDX} > /sys/class/gpio/export
  echo in > /sys/class/gpio/gpio${GPIO_IDX}/direction
done

for i in `seq 40 43` ; do 
  GPIO_IDX=$((338+78+$i)) #=456..459 gpio_emio[43:40]=GPIO_CTL
  echo ${GPIO_IDX} > /sys/class/gpio/export
  echo out > /sys/class/gpio/gpio${GPIO_IDX}/direction
  echo 0 > /sys/class/gpio/gpio${GPIO_IDX}/value
done

GPIO_IDX=$((338+78+44)) #=460 gpio_emio[44]=GPIO_EN_AGC
echo ${GPIO_IDX} > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio${GPIO_IDX}/direction
echo 0 > /sys/class/gpio/gpio${GPIO_IDX}/value

#Note: Resource busy.
#GPIO_IDX=$((338+78+45)) #=461 gpio_emio[45]=GPIO_SYNC
#echo ${GPIO_IDX} > /sys/class/gpio/export
#echo out > /sys/class/gpio/gpio${GPIO_IDX}/direction
#echo 0 > /sys/class/gpio/gpio${GPIO_IDX}/value

#Note: Resource busy.
#GPIO_IDX=$((338+78+46)) #=462 gpio_emio[46]=GPIO_RESETB
#echo ${GPIO_IDX} > /sys/class/gpio/export
#echo out > /sys/class/gpio/gpio${GPIO_IDX}/direction
#echo 1 > /sys/class/gpio/gpio${GPIO_IDX}/value

GPIO_IDX=$((338+78+47)) #=463 gpio_emio[47]=UP_ENABLE
echo ${GPIO_IDX} > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio${GPIO_IDX}/direction
echo 0 > /sys/class/gpio/gpio${GPIO_IDX}/value

GPIO_IDX=$((338+78+48)) #=464 gpio_emio[48]=UP_TXNRX
echo ${GPIO_IDX} > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio${GPIO_IDX}/direction
echo 0 > /sys/class/gpio/gpio${GPIO_IDX}/value

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

function sw_init()
{
/etc/init.d/iiod stop
echo 268435456 > /sys/module/industrialio_buffer_dma/parameters/max_block_size #256Mbytes
cat /sys/module/industrialio_buffer_dma/parameters/max_block_size
/etc/init.d/iiod start
echo -n "SW init"
}

echo

case "$1" in
  start)
    gpio_init
    echo "."
    led_init
    echo "."
    sw_init
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

