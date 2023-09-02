
---  
**QSPI partitioning**  

```
Address     Size
0x0000_0000 0x0400_0000 qspi-whole    64MByte
0x0000_0000 0x00C0_0000 qspi-boot    12MByte
0x00C0_0000 0x0010_0000 qspi-bootenv  1MByte
0x00D0_0000 0x02B0_0000 qspi-linux   45MByte
0x0380_0000 0x0080_0000 qspi-flashfs  8MByte
```

---

**SD card layout (32GByte SD card `sudo fdisk -l /dev/sdf` example)**  
```
Disk /dev/sdf: 29,1 GiB, 31272730624 bytes, 61079552 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x999aa2f8

Device     Boot    Start      End  Sectors  Size Id Type
/dev/sdf1  *        2048  4196351  4194304    2G  b W95 FAT32
/dev/sdf2        4196352 25167871 20971520   10G 83 Linux
/dev/sdf3       25167872 61079551 35911680 17,1G 83 Linux
```

**/dev/sdf1 on /media/$USER/boot type vfat**  
Format as FAT32.
Contains BOOT.bin, image.ub and uboot.scr.
Min size: 64MByte.

**/dev/sdf2 on /media/$USER/rootfs type ext4**  
Format as EXT4.
Contains Linux rootfs.
Min size: 256MByte.

**/dev/sdf3 on /media/$USER/userfs type ext4**  
Format as EXT4.
**NOTE**: Mounted as `/opt` directory on Mercury XU8 ST1 board.

---

**Copy local devicetree, u-boot and LX kernel build files**
```
cp ../../u-boot/u-boot-rpt/u-boot.elf ./u-boot.elf
md5sum ../../u-boot/u-boot-rpt/u-boot.elf ./u-boot.elf

cp ../../devicetree/system.dtb ./system.dtb
md5sum ../../devicetree/system.dtb ./system.dtb

cp ../../linux/linux-rpt/arch/arm64/boot/Image.gz ./Image.gz
md5sum ../../linux/linux-rpt/arch/arm64/boot/Image.gz ./Image.gz
```

---

**Build**  

**NOTE**: To get `mkimage` tool install `u-boot-tools` package.

```
make help
make build_boot build_lx
```

---

**Copy built files to local mount SD card**  

```  
cp ./bin/BOOT.BIN /media/$USER/boot/
cp ./bin/image.ub /media/$USER/boot/
cp ./uboot.scr /media/$USER/boot/
md5sum ./bin/BOOT.BIN /media/$USER/boot/BOOT.BIN
md5sum ./bin/image.ub /media/$USER/boot/image.ub
md5sum ./uboot.scr /media/$USER/boot/uboot.scr
```

---

**Copy Boot files via remote scp**  
```
scp ./bin/BOOT.BIN root@xu8:/media/sd-mmcblk1p1/
scp ./bin/image.ub root@xu8:/media/sd-mmcblk1p1/
scp ./uboot.scr root@xu8:/media/sd-mmcblk1p1/uboot.scr
```

Check md5sum on board:  
```
md5sum /media/sd-mmcblk1p1/BOOT.BIN
md5sum /media/sd-mmcblk1p1/image.ub
md5sum /media/sd-mmcblk1p1/uboot.scr
```
---

**Copy Linux rootfs to SD card**  
**NOTE: Double check your SD card device partition path (```/dev/sdX2```)!**  
**NOTE: Format will destroy data on ```/dev/sdX2``` partition!**  
**NOTE: Commands executed below can destroy your PC disk data, if directed to wrong device partition!**  
**NOTE: If you don't know what you are doing, than don't do it!**  
**Execution of commands below is your solely responsibility.**  
**You have been warned!**

```
sudo mount
sudo umount /dev/sdc2
sudo mkfs.ext4 /dev/sdc2
sudo e2label /dev/sdc2 rootfs
udisksctl mount -b /dev/sdc2
sudo su -c 'tar xvf ./rootfs.tar.gz -C /media/$SUDO_USER/rootfs/'
```

---
**Command to create U-Boot boot.scr:**
```  
source ~/opt/Xilinx/Vitis/2020.2/settings64.sh
mkimage -c none -A arm64 -T script -d boot.cmd.default boot.scr
```

---

