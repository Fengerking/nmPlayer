# Just acting as Father Makefile of Modules
# please keep the name 'makefile' unchanged
 #cd Numen/trunk/Codec/Video/Decoder/H264/Projects/Android
# Module Subdirs
VOMSD:=$(dir $(shell find . -name 'Makefile'))

all:
	
	#-$(MAKE) -C ./eclairv4/baseline
	#$(MAKE) -C ./eclairv4/non_baseline 
	-$(MAKE) -C ./eclairv6/baseline 
	$(MAKE) -C ./eclairv6/non_baseline
	-$(MAKE) -C ./eclairv7/baseline 
	$(MAKE) -C ./eclairv7/non_baseline 
	-$(MAKE) -C ./eclairv7Debug/baseline 
	$(MAKE) -C ./eclairv7Debug/non_baseline 
	-$(MAKE) -C ./eclairv6Debug/baseline 
	$(MAKE) -C ./eclairv6Debug/non_baseline 
.PHONY:clean devel
clean:
	$(MAKE) -C ./eclairv6/baseline  clean
	$(MAKE) -C ./eclairv6/non_baseline   clean
	$(MAKE) -C ./eclairv7/baseline   clean
	$(MAKE) -C ./eclairv7/non_baseline   clean
	$(MAKE) -C ./eclairv7Debug/baseline   clean
	$(MAKE) -C ./eclairv7Debug/non_baseline   clean
	$(MAKE) -C ./eclairv6Debug/baseline   clean
	$(MAKE) -C ./eclairv6Debug/non_baseline   clean

devel:

	#$(MAKE) -C ./eclairv4/non_baseline devel 
	$(MAKE) -C ./eclairv6/non_baseline devel 
	$(MAKE) -C ./eclairv7/non_baseline devel 
	$(MAKE) -C ./eclairv7Debug/non_baseline devel 
	$(MAKE) -C ./eclairv6Debug/non_baseline devel 

