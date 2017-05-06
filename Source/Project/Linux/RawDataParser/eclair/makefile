#the root make file control sub-makefile
# make the sub-directory that you don't compile with the different makefile name, including the root dir, 
# otherwise it would be enter die-loop ;)

SUB_DIRS = $(dir $(shell find ./ -name 'Makefile'))

all:
	for dir in $(SUB_DIRS); \
	do \
		$(MAKE) -C $$dir; \
	done

clean:
	for dir in $(SUB_DIRS); \
	do \
	  $(MAKE) -C $$dir clean; \
	done
devel:
	for dir in $(SUB_DIRS); \
	do \
	  $(MAKE) -C $$dir devel; \
	done
