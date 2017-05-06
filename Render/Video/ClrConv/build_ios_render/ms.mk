# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../Include \
           ../../src/\
           ../../src/C_CODE

# please list all objects needed by your target here
VOMSRC:= ccConstant.c ccMain.c ccRGBToRGB.c ccRGBToRGBMB.c  ccYUVToRGB.c ccYUVToRGBMB.c  ccYUVToYUV.c
			
#ifeq ($(VOTT), v4)
#VOMSRC+= no_resize_arm2.o no_resize_arm3.o no_resize_arm4.o no_resize_arm_180.o yuv2rgb16_mb_arm2.o \
#       yuv2rgb16_mb_arm3.o yuv2rgb16_mb_arm4.o yuv2rgb16_mb_arm_180.o yuv2yuv_arm.o yuv2argb32_mb_arm_180.o \
#       yuv2argb32_mb_arm2.o yuv2argb32_mb_arm3.o yuv2argb32_mb_arm4.o yuv2argb32_no_resize_arm_180.o \
#       yuv2argb32_no_resize_arm2.o yuv2argb32_no_resize_arm3.o yuv2argb32_no_resize_arm4.o yuv2rgb32_mb_arm_180.o \
#       yuv2rgb32_mb_arm2.o yuv2rgb32_mb_arm3.o yuv2rgb32_mb_arm4.o yuv2rgb32_no_resize_arm_180.o \
#       yuv2rgb32_no_resize_arm2.o yuv2rgb32_no_resize_arm3.o yuv2rgb32_no_resize_arm4.o
#VOSRCDIR+= ../../src/arm_mac/armv4
#endif	

#ifeq ($(VOTT), v5)
#VOMSRC+= no_resize_arm2.o no_resize_arm3.o no_resize_arm4.o no_resize_arm_180.o yuv2rgb16_mb_arm2.o \
#       yuv2rgb16_mb_arm3.o yuv2rgb16_mb_arm4.o yuv2rgb16_mb_arm_180.o yuv2yuv_arm.o yuv2argb32_mb_arm_180.o \
#       yuv2argb32_mb_arm2.o yuv2argb32_mb_arm3.o yuv2argb32_mb_arm4.o yuv2argb32_no_resize_arm_180.o \
#       yuv2argb32_no_resize_arm2.o yuv2argb32_no_resize_arm3.o yuv2argb32_no_resize_arm4.o yuv2rgb32_mb_arm_180.o \
#       yuv2rgb32_mb_arm2.o yuv2rgb32_mb_arm3.o yuv2rgb32_mb_arm4.o yuv2rgb32_no_resize_arm_180.o \
#       yuv2rgb32_no_resize_arm2.o yuv2rgb32_no_resize_arm3.o yuv2rgb32_no_resize_arm4.o
#       
#VOSRCDIR+= ../../src/arm_mac/armv4
#endif				

ifeq ($(VOTT), v6)
VOMSRC+= no_resize_arm3.s\
no_resize_arm4.s\
no_resize_arm_180.s\
voYUV420toRGB_16_v6.s\
voYUV420toRGB_24_v6.s\
yuv2argb32_mb_arm2.s\
yuv2argb32_mb_arm3.s\
yuv2argb32_mb_arm4.s\
yuv2argb32_mb_arm_180.s\
yuv2argb32_no_resize_arm2.s\
yuv2argb32_no_resize_arm3.s\
yuv2argb32_no_resize_arm4.s\
yuv2argb32_no_resize_arm_180.s\
yuv2rgb16_mb_arm2.s\
yuv2rgb16_mb_arm3.s\
yuv2rgb16_mb_arm4.s\
yuv2rgb16_mb_arm_180.s\
yuv2rgb32_mb_arm2.s\
yuv2rgb32_mb_arm3.s\
yuv2rgb32_mb_arm4.s\
yuv2rgb32_mb_arm_180.s\
yuv2rgb32_no_resize_arm2.s\
yuv2rgb32_no_resize_arm3.s\
yuv2rgb32_no_resize_arm4.s\
yuv2rgb32_no_resize_arm_180.s\
yuv2yuv_arm.s

VOSRCDIR+= ../../src/arm_mac/armv6
endif				

ifeq ($(VOTT), v7)
VOMSRC+= voRGB888toRGB16_8x2n_armv7.s\
voYUV2YUV_8x2n_armv7.s\
voYUV2YUV_8x2n_armv7L90.s\
voYUV2YUV_8x2n_armv7R180.s\
voYUV2YUV_8x2n_armv7R90.s\
voYUV2YUV_8x8_armv7R90L90R180.s\
voYUV420toRGB16_8nx2n_armv7.s\
voYUV420toRGB32_8nx2n_armv7.s\
voYUV420toRGB_24_v7.s\
yuv2yuv_arm.s\
YUV420_YUV422.s\

VOSRCDIR+= ../../src/arm_mac/armv7
endif			
