
# Module Subdirs
MAKE:=ndk-build
VOMSD:=$(dir $(shell find . -name 'Android.mk'))

all:
	for dir in $(VOMSD); \
	do \
		echo $$dir; \
		$(MAKE) -C $$dir; \
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
		echo $$dir; \
		if $(
		cp -Rfp $$dir/../libs/armeabi-/*.so ~/; \
	done

