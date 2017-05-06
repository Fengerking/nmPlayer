
# please list all objects needed by your module here
OBJS:=CBaseVideoRender.o CCCRRRFunc.o voVideoRender.o CDllLoad.o cmnMemory.o CvoBaseObject.o voCMutex.o voOSFunc.o \
CBaseConfig.o cmnFile.o

ifeq ($(VOVR), fb)
   OBJS+=CFBVideoRender.o
endif

ifeq ($(VOVR), sf)
   OBJS+=CSurfaceVRender.o
endif

ifeq ($(VOVR), sdl)
   OBJS+=CSDLDraw.o
endif

ifeq ($(VOVR), v4l2)
   OBJS+=voV4L2Render.o
endif

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
					../../../../../../Common \
					../../../Source 

ifeq ($(VOVR), fb)
   VOSRCDIR +=../../../Source/linux/fb 
endif

ifeq ($(VOVR), sf)
   VOSRCDIR +=../../../Source/linux/sf
endif


ifeq ($(VOVR), sdl)
   VOSRCDIR +=../../../Source/linux/sdl
endif

ifeq ($(VOVR), v4l2)
   VOSRCDIR +=../../../Source/linux/v4l2
endif
