
# please list all objects needed by your module here

VOMSRC:=	build.c com_globals.c context.c crecon.c dquantize.c entropysetup.c gconst.c huffman.c loopfilter.c ModeMv.c TokenEntropy.c\
	boolhuff.c decodembs.c decodemode.c decodemv.c DFrameR.c DSystemDependant.c idct.c pb_globals.c readtokens_arith.c readtokens_data.c readtokens_huff.c reconmb.c vfwpbdll_if.c voVideoFrameBufManager.c\
	voVP6DecFront.c\
	on2_mem.c yv12config.c yv12extend.c
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../Src/\
		../../../Src/com_src\
	  ../../../Src/dec_src\
	  ../../../Src/front\
	  ../../../Src/plugin_src\
	  ../../../Inc\
	  ../../../../../../../Include

				
ifeq ($(VOTT), v4)
VOMSRC+=  
VOMSRC+= voFilterVP6ArmV4.S voIdctVP6ArmV4.S voMcVP6ArmV4.S
VOSRCDIR+= ../../../Src/armasm/ARMV4/ios

endif				

ifeq ($(VOTT), v6)
VOMSRC+= 
VOMSRC+= voFilterVP6ArmV6.S voIdctVP6ArmV6.S voMcVP6ArmV6.S
VOSRCDIR+= ../../../Src/armasm/ARMV6/ios
endif				

ifeq ($(VOTT), v7)
VOMSRC+= voFilterVP6ArmV7.S voIdctVP6ArmV7.S voMcVP6ArmV7.S
VOSRCDIR+= ../../../Src/armasm/ARMV7/ios

endif	

ifeq ($(VOTT), v7s)
VOMSRC+= voFilterVP6ArmV7.S voIdctVP6ArmV7.S voMcVP6ArmV7.S
VOSRCDIR+= ../../../Src/armasm/ARMV7/ios

endif	
