# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../Include \
		../../inc \
    ../../src \
    ../../src/gun

# please list all objects needed by your target here
OBJS:= fdct.o jcommon.o jdcttrans.o jencode.o jexif.o jhuffum.o  jmark.o \
       jResize.o jsrcdata.o jtab.o mem_align.o vo_jpegEnc_api.o vo_mjpegEnc_api.o
			
ifeq ($(VOTT), v4)
OBJS+= fdctquant.o
endif				

ifeq ($(VOTT), v6)
OBJS+= fdct_armv6.o get420_data.o 
endif				

ifeq ($(VOTT), v7)
OBJS+= get420_data.o fdct_armv7.o
endif			
