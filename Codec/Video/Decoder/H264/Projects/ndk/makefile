# Just acting as Father Makefile of Modules
# please keep the name 'makefile' unchanged
 #cd Numen/trunk/Codec/Video/Decoder/H264/Projects/ndk
# Module Subdirs
MAKE:=ndk-build
VOMSD:=$(dir $(shell find . -name 'Android.mk'))
RELEASE_DIR:=../../../../../../../voRelease/Android_ndk/so

all:
	
	-$(MAKE) -C ./v6/debug/BP_LIB/jni/ -B
	$(MAKE) -C ./v6/debug/HP_LIB/jni/  -B
	-$(MAKE) -C ./v6/release/BP_LIB/jni/ -B
	$(MAKE) -C ./v6/release/HP_LIB/jni/	 -B
	-$(MAKE) -C ./v7/debug/BP_LIB/jni/	 -B
	$(MAKE) -C ./v7/debug/HP_LIB/jni/ 	 -B
	-$(MAKE) -C ./v7/release/BP_LIB/jni/	 -B
	$(MAKE) -C ./v7/release/HP_LIB/jni/	 -B
	
.PHONY:clean devel
clean:
	-$(MAKE) -C ./v6/debug/BP_LIB/jni/ clean
	$(MAKE) -C ./v6/debug/HP_LIB/jni/   clean
	-$(MAKE) -C ./v6/release/BP_LIB/jni/  clean
	$(MAKE) -C ./v6/release/HP_LIB/jni/	  clean
	-$(MAKE) -C ./v7/debug/BP_LIB/jni/	 clean
	$(MAKE) -C ./v7/debug/HP_LIB/jni/ 	  clean
	-$(MAKE) -C ./v7/release/BP_LIB/jni/	  clean
	$(MAKE) -C ./v7/release/HP_LIB/jni/	  clean

devel:

	cp -Rfp ./v6/debug/HP_LIB/libs/armeabi/libvoH264Dec_DebugV6.so $(RELEASE_DIR)/v6/libvoH264Dec_DebugV6.so  
	cp -Rfp ./v6/release/HP_LIB/libs/armeabi/libvoH264Dec.so $(RELEASE_DIR)/v6/libvoH264Dec.so					 
	cp -Rfp ./v7/debug/HP_LIB/libs/armeabi-v7a/libvoH264Dec_DebugV7.so $(RELEASE_DIR)/v7/libvoH264Dec_DebugV7.so  
	cp -Rfp ./v7/release/HP_LIB/libs/armeabi-v7a/libvoH264Dec.so  $(RELEASE_DIR)/v7/libvoH264Dec.so		

v7_so:
	adb push  ./v7/release/HP_LIB/libs/armeabi-v7a/libvoH264Dec.so  /data/data/com.visualon.mobitv/lib/libvoH264Dec_v7.so                            
	adb push  ./v7/release/HP_LIB/libs/armeabi-v7a/libvoH264Dec.so  /data/local/libvoH264Dec_v7.so                           

v7_release:
	-$(MAKE) -C ./v7/release/BP_LIB/jni/     -B
	$(MAKE) -C ./v7/release/HP_LIB/jni/      -B
	
	
