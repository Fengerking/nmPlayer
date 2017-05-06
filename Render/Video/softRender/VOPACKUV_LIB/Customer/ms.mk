
# please list all objects needed by your module here

OBJS:=
			 
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=
	  
VOCFLAGS+=	
	

ifeq ($(VOTT), pc)
OBJS+= PackUV_VC.o

VOSRCDIR+= ../../../../VOPACKUV/ARMV4_VC

endif

				
ifeq ($(VOTT), v4)
OBJS+= PackUV_VC.o

VOSRCDIR+= ../../../../VOPACKUV/ARMV4_VC

endif				

ifeq ($(VOTT), v6)
OBJS+= PackUV_dest-stride_gnu.o
		
VOSRCDIR+= ../../../../VOPACKUV/ARMV6

endif				

ifeq ($(VOTT), v7)
OBJS+= PackUV_v7_gnu.o
		
VOSRCDIR+= ../../../../VOPACKUV/ARMV7

endif	
