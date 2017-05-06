# Just acting as Father Makefile of Modules
# please keep the name 'makefile' unchanged
 #cd Numen/trunk/Codec/Video/Decoder/H264/Projects/ndk/v6/release/HP_LIB/jni/
# Module Subdirs
MAKE:=ndk-build
VOMSD:=$(dir $(shell find . -name 'Android.mk'))
RELEASE_DIR:=../../../../../../../../../../../voRelease/Android_ndk/so

all:
	
	-$(MAKE) -C ../../BP_LIB/jni/ -B
	$(MAKE)  -B
	
.PHONY:clean devel

clean:
	-$(MAKE) -C ../../BP_LIB/jni/ clean
	$(MAKE)  clean
	
devel:
	cp -Rfp ../libs/armeabi/libvoH264Dec.so $(RELEASE_DIR)/v6/libvoH264Dec.so  
	
