**Petalinux 2020.2 build**  

---  
**Setup build environment**

```
source ~/opt/Xilinx/petalnx/2020.2/settings.sh
cd /mnt/src/gitrepos/Enclustra/lnx/petalnx/xu8_fmcxm105
```
---
**Petalinux config to extern U-Boot and LX Kernel sources**  
`petalinux-config`  
```
${PROOT}/../../u-boot/u-boot-rpt
${PROOT}/../../linux/linux-rpt
```
---  

**Build XU8:**
**NOTE:** Console terminal Width/Height for petalinux-config must be min 20 lines 80 columns!
```
petalinux-config --get-hw-description=../xu8_fmcxm105_xsa

petalinux-build -x mrproper
make -C ../../u-boot/ clean
make -C ../../linux/ clean
petalinux-build -c u-boot
petalinux-build -c kernel
petalinux-build -c device-tree
petalinux-build -c bootloader
petalinux-build -c rootfs
petalinux-build
petalinux-package --boot --fsbl ./images/linux/zynqmp_fsbl.elf --fpga ./images/linux/system.bit --pmufw ./images/linux/pmufw.elf --u-boot ./images/linux/u-boot.elf --atf ./images/linux/bl31.elf --force --output ./images/linux/BOOT.BIN
```
---
