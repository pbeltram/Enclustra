**Scripts**  

Copy files to `/opt/bin/` directory. `/opt` directory is mounted to partition on SD disk (`/dev/mmcblk1p3`). See in petalinux `fstab` file.  

`iio_xilinxams.sh` - Read Xilinx AMS values exposed via IIO device.   
Script `/opt/bin/startup.sh` is executed at system boot (configured in sys-init petalinux).  
`startup_xu8.sh` - Copy to XU8 board into `/opt/bin/startup.sh`.  
`startup_xu8_fmcomms3.sh` - Copy to XU8 AD-FMCOMMS3 board into `/opt/bin/startup.sh`.  

