
# Module Subdirs
MAKE:=ndk-build
VOMSD:=$(dir $(shell find . -name 'Android.mk'))

all:
	for dir in $(VOMSD); \
	do \
		echo $$dir; \
		$(MAKE)  -C $$dir V=1; \
	done

.PHONY:clean devel
clean:
	for dir in $(VOMSD); \
	do \
		$(MAKE) -C $$dir clean; \
	done

devel:
	for dir in $(VOMSD); \
	do \
		cp -Rfp $$dir/../libs/armeabi/*.so ~/ 
	done

v7_release:
	$(MAKE)  -C  v7/jni/ V=1 

v7_so:
	adb push  ./v7/libs/armeabi-v7a/libvoH264Dec.so  /data/local/libvoH264Dec_v7.so
