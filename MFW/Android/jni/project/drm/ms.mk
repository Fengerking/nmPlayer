
# please list all objects needed by your module here
OBJS:=  CvoBaseObject.o \
		CDllLoad.o      \
		CDrmEngine.o    \
		vomutex.o       \
		voDivXDRMJni.o 
	

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../source/drm            \
		   ../../../../../Include      \
           ../../../../../Include/vome \
           ../../../../../Common       \
           ../../../../voME/Common