#	\\10.2.64.166
#	cd svn/trunk/Render/Video/softRender/dirender_PPC2005/Customer/Google/
#	make clean ; make ;make devel 

# Just acting as Father Makefile of Modules
# please keep the name 'makefile' unchanged
 
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
