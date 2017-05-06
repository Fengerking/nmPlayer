# Just 
# /home/huang_bafeng/Numen/trunk/Codec/Video/Decoder/H264/Projects/ndk/
 #cd Numen/trunk/Codec/Video/Decoder/H264/Projects/ndk/v7/release/HP_LIB/jni/
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
	cp -Rfp ../libs/armeabi-v7a/libvoH264Dec.so $(RELEASE_DIR)/v7/libvoH264Dec.so  
	
