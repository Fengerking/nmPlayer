# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../../Include \
		../../../inc \
    ../../../src \
    ../../../src/ios

# please list all objects needed by your target here
VOMSRC:= fdct.c jcommon.c jdcttrans.c jencode.c jexif.c jhuffum.c  jmark.c \
       jResize.c jsrcdata.c jtab.c mem_align.c vo_jpegEnc_api.c
			
ifeq ($(VOTT), v4)
VOMSRC+= fdctquant.S
endif				

ifeq ($(VOTT), v6)
VOMSRC+= fdct_armv6.S get420_data.S 
endif				

ifeq ($(VOTT), v7)
VOMSRC+= get420_data.S fdct_armv7.S
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= get420_data.S fdct_armv7.S
endif			
