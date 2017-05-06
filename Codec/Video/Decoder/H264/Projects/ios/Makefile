# Just acting as Father Makefile of Modules
# please keep the name 'makefile' unchanged
 #cd Numen/trunk/Codec/Video/Decoder/H264/Projects/Android_Debug
# Module Subdirs
VOMSD:=$(dir $(shell find . -name 'Makefile'))

all:
	
	-$(MAKE) -C ./make_v6/bp; 
	$(MAKE) -C ./make_v6/dec; 
	-$(MAKE) -C ./make_v6Debug/bp; 
	$(MAKE) -C ./make_v6Debug/dec; 
	-$(MAKE) -C ./make_v7/bp; 
	$(MAKE) -C ./make_v7/dec; 
	-$(MAKE) -C ./make_v7Debug/bp; 
	$(MAKE) -C ./make_v7Debug/dec;
	-$(MAKE) -C ./make_simulator/bp; 
	$(MAKE) -C ./make_simulator/dec; 
	-$(MAKE) -C ./make_simulatorDebug/bp; 
	$(MAKE) -C ./make_simulatorDebug/dec; 

.PHONY:clean devel
clean:
	for dir in $(VOMSD); \
		do \
			$(MAKE) -C $$dir clean; \
		done

devel:
	for dir in $(VOMSD); \
		do \
			$(MAKE) -C $$dir devel; \
		done
