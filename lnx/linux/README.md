
---
**Linux kernel petalinux build**  

This directory is configured as external Linux kernel in Petalinux build.  
Original git repo is forked from ```analogdevicesinc/linux``` git commit=`1a8119431b3d0f59bf8de292958456837cc30aad`.  
NOTE: Branch `master-xilinx-2020.2` was deleted in `analogdevicesinc/linux`.
Enclustra changes are made and kept in ```pb_enclustra_2020.2_devel``` branch.  

---

**Build**  

```
source ~/opt/Xilinx/Vitis/2020.2/settings64.sh
make clean
make
```

---
