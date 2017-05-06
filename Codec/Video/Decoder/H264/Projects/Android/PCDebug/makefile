# Just acting as Father Makefile of Modules
# please keep the name 'makefile' unchanged
 #cd Numen/trunk/Codec/Video/Decoder/H264/Projects/Android/PC
# Module Subdirs
VOMSD:=$(dir $(shell find . -name 'Makefile'))

all:
	
	-$(MAKE) -C ./baseline
	$(MAKE) -C ./non_baseline 
.PHONY:clean devel
clean:
	for dir in $(VOMSD); \
		do \
			$(MAKE) -C $$dir clean; \
		done

devel:

	$(MAKE) -C ./non_baseline devel 

