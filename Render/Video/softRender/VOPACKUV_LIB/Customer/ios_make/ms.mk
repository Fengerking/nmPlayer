
# please list all objects needed by your module here

OBJS:=
			 
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=
	  
VOCFLAGS+=	
	

ifeq ($(VOTT), pc)
VOMSRC += PackUV_VC.c

VOSRCDIR+= ../../../../VOPACKUV/ARMV4_VC

endif

				
ifeq ($(VOTT), v4)
VOMSRC += PackUV_VC.c

VOSRCDIR+= ../../../../VOPACKUV/ARMV4_VC

endif				

ifeq ($(VOTT), v6)
VOMSRC += PackUV_dest-stride_ios.s
		
VOSRCDIR+= ../../../../VOPACKUV/ARMV6

endif				

ifeq ($(VOTT), v7)
VOMSRC+= PackUV_v7_ios.s
		
VOSRCDIR+= ../../../../VOPACKUV/ARMV7

endif	
