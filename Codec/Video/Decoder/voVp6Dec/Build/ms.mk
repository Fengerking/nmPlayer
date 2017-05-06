
# please list all objects needed by your module here

OBJS:=	build.o com_globals.o context.o crecon.o dquantize.o entropysetup.o gconst.o huffman.o loopfilter.o ModeMv.o TokenEntropy.o\
	boolhuff.o decodembs.o decodemode.o decodemv.o DFrameR.o DSystemDependant.o idct.o pb_globals.o readtokens_arith.o readtokens_data.o readtokens_huff.o reconmb.o vfwpbdll_if.o voVideoFrameBufManager.o\
	voVP6DecFront.o\
	on2_mem.o yv12config.o yv12extend.o
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../Src/\
		../../../Src/com_src\
	  ../../../Src/dec_src\
	  ../../../Src/front\
	  ../../../Src/plugin_src\
	  ../../../Inc\
	  ../../../../../../../Include

				
ifeq ($(VOTT), v4)
OBJS+=  
OBJS+= voFilterVP6ArmV4.o voIdctVP6ArmV4.o voMcVP6ArmV4.o
VOSRCDIR+= ../../../Src/armasm/ARMV4/gnu

endif				

ifeq ($(VOTT), v6)
OBJS+= 
OBJS+= voFilterVP6ArmV6.o voIdctVP6ArmV6.o voMcVP6ArmV6.o
VOSRCDIR+= ../../../Src/armasm/ARMV6/gnu
endif				

ifeq ($(VOTT), v7)
OBJS+= voFilterVP6ArmV7.o voIdctVP6ArmV7.o voMcVP6ArmV7.o
VOSRCDIR+= ../../../Src/armasm/ARMV7/gnu

endif	

ifeq ($(VOTT), v7s)
OBJS+= voFilterVP6ArmV7.o voIdctVP6ArmV7.o voMcVP6ArmV7.o
VOSRCDIR+= ../../../Src/armasm/ARMV7/gnu

endif	