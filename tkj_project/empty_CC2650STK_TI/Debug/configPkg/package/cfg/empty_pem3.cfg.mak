# invoke SourceDir generated makefile for empty.pem3
empty.pem3: .libraries,empty.pem3
.libraries,empty.pem3: package/cfg/empty_pem3.xdl
	$(MAKE) -f /home/linuxlite/Downloads/empty_CC2650STK_TI_2023klkl/empty_CC2650STK_TI_2023/empty_CC2650STK_TI/src/makefile.libs

clean::
	$(MAKE) -f /home/linuxlite/Downloads/empty_CC2650STK_TI_2023klkl/empty_CC2650STK_TI_2023/empty_CC2650STK_TI/src/makefile.libs clean

