# Just acting as Father Makefile of Modules
# please keep the name 'makefile' unchanged
 #cd Numen/trunk/Source/RTSP/Project/Linux
 #svn ci ../../../../../voRelease/Customer/Google/eclair/so/v4  -m "enlarge the anti-jitter buffer to 30" 
 #svn ci ../../../../../voRelease/Customer/Google/cupcake/so/v6  -m "enlarge the anti-jitter buffer to 30" 
 #svn ci ../../../../../trunk/Source/RTSP/  -m "enlarge the anti-jitter buffer to 30" 
# Module Subdirs
VOMSD:=$(dir $(shell find . -name 'Makefile'))

all:
	for dir in $(VOMSD); \
		do \
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
			$(MAKE) -C $$dir devel; \
		done
