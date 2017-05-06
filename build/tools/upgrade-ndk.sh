#!/bin/bash

rootpath=`pwd`;

# modify all Android.mk
njlist=`find . -name Android.mk`;
for nj in $njlist;
do
	# get abs path
	itemdir=`dirname $nj`;

	# remove rootpath to get relative path
	relapath=${itemdir#$rootpath};

	# strip all characters  
	toppath=${relapath//[a-zA-Z0-9. -_]/};

	# insert .. in front of every '/'
	toppath=${toppath//\//..\/};

	# trim off the last '/'
	toppath=${toppath%/};

	sed -e '/^LOCAL_PATH/a\\VOTOP?='$toppath'' -e '/VOUPDATE/d' -e '/ifeq ($(APP_OPTIM)/,/^endif/d' -e 's/LOCAL_CFLAGS\([ \t+:]*\)=[ \t]*$(VOABOUT)[ \t]*$(VOMM)/LOCAL_CFLAGS\1=/g' -e '/include $(BUILD_SHARED_LIBRARY)/i\\include $(VOTOP)/build/vondk.mk' -e 's/[$().a-zA-Z0-9_/]*ndkversion.cpp[ \t]*//g' -i $nj; 

done;

# modify all Application.mk
njlist=`find . -name Application.mk`;
for nj in $njlist;
do
	sed -e 's/APP_OPTIM[ ]*:=/APP_OPTIM?=/g' -i $nj; 
done;
