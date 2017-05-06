# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../Include \
    ../../src/ \
    ../../src/C_CODE\
    ../../../../../Common

# please list all objects needed by your target here
OBJS:= ccConstant.o ccMain.o ccRGBToRGB.o ccRGBToRGBMB.o  ccYUVToRGB.o ccYUVToRGBMB.o  ccYUVToYUV.o
			
ifeq ($(VOTT), v4)
OBJS+= no_resize_arm2.o no_resize_arm3.o no_resize_arm4.o no_resize_arm_180.o yuv2rgb16_mb_arm2.o \
       yuv2rgb16_mb_arm3.o yuv2rgb16_mb_arm4.o yuv2rgb16_mb_arm_180.o yuv2yuv_arm.o yuv2argb32_mb_arm_180.o \
       yuv2argb32_mb_arm2.o yuv2argb32_mb_arm3.o yuv2argb32_mb_arm4.o yuv2argb32_no_resize_arm_180.o \
       yuv2argb32_no_resize_arm2.o yuv2argb32_no_resize_arm3.o yuv2argb32_no_resize_arm4.o yuv2rgb32_mb_arm_180.o \
       yuv2rgb32_mb_arm2.o yuv2rgb32_mb_arm3.o yuv2rgb32_mb_arm4.o yuv2rgb32_no_resize_arm_180.o \
       yuv2rgb32_no_resize_arm2.o yuv2rgb32_no_resize_arm3.o yuv2rgb32_no_resize_arm4.o
VOSRCDIR+= ../../src/gun/armv4
endif	

ifeq ($(VOTT), v5)
OBJS+= no_resize_arm2.o no_resize_arm3.o no_resize_arm4.o no_resize_arm_180.o yuv2rgb16_mb_arm2.o \
       yuv2rgb16_mb_arm3.o yuv2rgb16_mb_arm4.o yuv2rgb16_mb_arm_180.o yuv2yuv_arm.o yuv2argb32_mb_arm_180.o \
       yuv2argb32_mb_arm2.o yuv2argb32_mb_arm3.o yuv2argb32_mb_arm4.o yuv2argb32_no_resize_arm_180.o \
       yuv2argb32_no_resize_arm2.o yuv2argb32_no_resize_arm3.o yuv2argb32_no_resize_arm4.o yuv2rgb32_mb_arm_180.o \
       yuv2rgb32_mb_arm2.o yuv2rgb32_mb_arm3.o yuv2rgb32_mb_arm4.o yuv2rgb32_no_resize_arm_180.o \
       yuv2rgb32_no_resize_arm2.o yuv2rgb32_no_resize_arm3.o yuv2rgb32_no_resize_arm4.o
       
VOSRCDIR+= ../../src/gun/armv4
endif				

ifeq ($(VOTT), v6)
OBJS+= voYUV420toRGB_16_v6.o no_resize_arm3.o no_resize_arm4.o no_resize_arm_180.o yuv2rgb16_mb_arm2.o \
       yuv2rgb16_mb_arm3.o yuv2rgb16_mb_arm4.o yuv2rgb16_mb_arm_180.o yuv2yuv_arm.o voYUV420toRGB_24_v6.o yuv2argb32_mb_arm_180.o \
       yuv2argb32_mb_arm2.o yuv2argb32_mb_arm3.o yuv2argb32_mb_arm4.o yuv2argb32_no_resize_arm_180.o \
       yuv2argb32_no_resize_arm2.o yuv2argb32_no_resize_arm3.o yuv2argb32_no_resize_arm4.o yuv2rgb32_mb_arm_180.o \
       yuv2rgb32_mb_arm2.o yuv2rgb32_mb_arm3.o yuv2rgb32_mb_arm4.o yuv2rgb32_no_resize_arm_180.o \
       yuv2rgb32_no_resize_arm2.o yuv2rgb32_no_resize_arm3.o yuv2rgb32_no_resize_arm4.o
VOSRCDIR+= ../../src/gun/armv6
endif				

ifeq ($(VOTT), v7)
OBJS+= voRGB888toRGB16_8x2n_armv7.o voYUV2YUV_8x2n_armv7.o voYUV2YUV_8x2n_armv7L90.o voYUV2YUV_8x2n_armv7R90.o \
       voYUV2YUV_8x2n_armv7R180.o voYUV2YUV_8x8_armv7R90L90R180.o voYUV420toRGB16_8nx2n_armv7.o \
       voYUV420toRGB_24_v7.o yuv2yuv_arm.o YUV420_YUV422.o voYUV420toRGB32_8nx2n_armv7.o
VOSRCDIR+= ../../src/gun/armv7
endif			
