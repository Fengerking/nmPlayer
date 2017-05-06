# List all object files here
OFILES = \
	voHttpSvrExport.o \
  voClientContext.o\
  voServerAnsycSocket.o

# Compiler command name

TCROOTPATH:=/opt/eclair
GCCVER:=4.4.0
TCPATH:=$(TCROOTPATH)/prebuilt/linux-x86/toolchain/arm-eabi-$(GCCVER)
CCTPRE:=$(TCPATH)/bin/arm-eabi-
AS:=$(CCTPRE)as
AR:=$(CCTPRE)ar
NM:=$(CCTPRE)nm
CC:=$(CCTPRE)gcc
GG:=$(CCTPRE)g++
LD:=$(CCTPRE)ld
SIZE:=$(CCTPRE)size
STRIP:=$(CCTPRE)strip
RANLIB:=$(CCTPRE)ranlib
OBJCOPY:=$(CCTPRE)objcopy
OBJDUMP:=$(CCTPRE)objdump
READELF:=$(CCTPRE)readelf
STRINGS:=$(CCTPRE)strings

VOTP:=sapphire-open
CCTLIB:= -L$(TCROOTPATH)/out/target/product/$(VOTP)/obj/lib
CCTINC:=-I$(TCROOTPATH)/system/core/include \
	-I$(TCROOTPATH)/hardware/libhardware/include \
	-I$(TCROOTPATH)/hardware/ril/include \
	-I$(TCROOTPATH)/hardware/libhardware_legacy/include \
	-I$(TCROOTPATH)/dalvik/libnativehelper/include \
	-I$(TCROOTPATH)/dalvik/libnativehelper/include/nativehelper \
	-I$(TCROOTPATH)/frameworks/base/include \
	-I$(TCROOTPATH)/frameworks/base/core/jni \
	-I$(TCROOTPATH)/frameworks/base/libs/audioflinger \
	-I$(TCROOTPATH)/external/skia/include \
	-I$(TCROOTPATH)/external/openssl/include \
	-I$(TCROOTPATH)/out/target/product/$(VOTP)/obj/include \
	-I$(TCROOTPATH)/bionic/libc/arch-arm/include \
	-I$(TCROOTPATH)/bionic/libc/include \
	-I$(TCROOTPATH)/bionic/libstdc++/include \
	-I$(TCROOTPATH)/bionic/libc/kernel/common \
	-I$(TCROOTPATH)/bionic/libc/kernel/arch-arm \
	-I$(TCROOTPATH)/bionic/libm/include \
	-I$(TCROOTPATH)/bionic/libm/include/arm \
	-I$(TCROOTPATH)/bionic/libthread_db/include \
	-I$(TCROOTPATH)/bionic/libm/arm \
	-I$(TCROOTPATH)/bionic/libm \
	-I$(TCROOTPATH)/frameworks/base/include/android_runtime
	
VOPREDEF := -Os -Wall -D_POSIX  -D_LINUX_ANDROID -D_DEBUG -D_VERBOSE -fpic -fPIC -D_READLOCAL
 
# Compiler flags applied to all files
CFLAGS = -I../../../../HttpServer -I../../../../../Include
   
TELDFLAGS := -nostdlib
 
CFLAGS +=  $(CCTINC) $(CCTLIB) $(TELDFLAGS) $(VOPREDEF) $(VOINC) 

LIBS = -lc -lutils -lcutils -llog -lm
 
VPATH = ./ ../../../../HttpServer  ../../../../../Include
 
EXENAME = ../../../../../voRelease/Linux/shared/voHttpServer.so
.cpp.o:
	$(CC) -c $(CFLAGS) $<
		
# Builds all object files and executable
${EXENAME} : $(OFILES)
	$(GG) $(CFLAGS) -shared -o ${EXENAME} $(OFILES) $(LIBS) 
	
# Clean up
clean :
	rm -f $(OFILES) ${EXENAME}	