
# please list all objects needed by your module here

OBJS:=420n422ToRgb16_mb_c.o ccConstant.o dirender_newsdk.o softRender_newsdk.o\
			 yuv2rgb16_mb.o yuv2yuv.o
			 
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../../src \
	  ../../../../inc \
	  ../../../../../../../Include\
	  
VOCFLAGS+=-DNEW_SDK -DRVDS	
				
ifeq ($(VOTT), v4)
OBJS+= no_resize_arm2.o no_resize_arm3.o no_resize_arm4.o\
		yuv2rgb16_mb_arm2.o yuv2rgb16_mb_arm3.o yuv2rgb16_mb_arm4.o\
		no_resize_arm_180.o yuv2rgb16_mb_arm_180.o yuv2yuv_arm.o

VOSRCDIR+= ../../../../arm_asm_gun

VOCFLAGS+=-DVOARMV4
endif				

ifeq ($(VOTT), v6)
OBJS+= no_resize_arm2.o no_resize_arm3.o no_resize_arm4.o\
		yuv2rgb16_mb_arm2.o yuv2rgb16_mb_arm3.o yuv2rgb16_mb_arm4.o\
		no_resize_arm_180.o yuv2rgb16_mb_arm_180.o yuv2yuv_arm.o
		
VOSRCDIR+= ../../../../arm_asm_gun

VOCFLAGS+=-DVOARMV6
endif				

ifeq ($(VOTT), v7)
OBJS+= no_resize_arm2.o no_resize_arm3.o no_resize_arm4.o\
		yuv2rgb16_mb_arm2.o yuv2rgb16_mb_arm3.o yuv2rgb16_mb_arm4.o\
		no_resize_arm_180.o yuv2rgb16_mb_arm_180.o yuv2yuv_arm.o
		
VOSRCDIR+= ../../../../arm_asm_gun

VOCFLAGS+=-DVOARMV7
endif	
