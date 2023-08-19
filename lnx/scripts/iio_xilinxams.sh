#!/bin/sh

XILINX_AMSIIO="/sys/bus/iio/devices/iio:device0"

function read_temp()
{
    VAL=$1
    OFS=`cat ${XILINX_AMSIIO}/in_${VAL}_temp_offset`
    RAW=`cat ${XILINX_AMSIIO}/in_${VAL}_temp_raw`
    SCALE=`cat ${XILINX_AMSIIO}/in_${VAL}_temp_scale`
    RES=`echo - | awk "{print $SCALE*($RAW+$(($OFS)))}"`
    echo "$VAL=$RES mC"
}

function read_vcc()
{
    VAL=$1
    RAW=`cat ${XILINX_AMSIIO}/in_${VAL}_raw`
    SCALE=`cat ${XILINX_AMSIIO}/in_${VAL}_scale`
    RES=`echo - | awk "{print $SCALE*$RAW}"`
    echo "$VAL=$RES mV"
}

read_temp "temp0_ps"
read_temp "temp1_remote"
read_temp "temp2_pl"

read_vcc "voltage0_vcc_pspll0"
read_vcc "voltage1_vcc_psbatt"
read_vcc "voltage2_vccint"
read_vcc "voltage3_vccbram"
read_vcc "voltage4_vccaux"
read_vcc "voltage5_vcc_psddrpll"
read_vcc "voltage6_vccpsintfpddr"
read_vcc "voltage7_vccpsintlp"
read_vcc "voltage8_vccpsintfp"
read_vcc "voltage9_vccpsaux"
read_vcc "voltage10_vccpsddr"
read_vcc "voltage11_vccpsio3"
read_vcc "voltage12_vccpsio0"
read_vcc "voltage13_vccpsio1"
read_vcc "voltage14_vccpsio2"
read_vcc "voltage15_psmgtravcc"
read_vcc "voltage16_psmgtravtt"
read_vcc "voltage17_vccams"
read_vcc "voltage18_vccint"
read_vcc "voltage19_vccaux"
read_vcc "voltage20_vccvrefp"
read_vcc "voltage21_vccvrefn"
read_vcc "voltage22_vccbram"
read_vcc "voltage23_vccplintlp"
read_vcc "voltage24_vccplintfp"
read_vcc "voltage25_vccplaux"
read_vcc "voltage26_vccams"

