#!/bin/bash
#program:
#        Create codec sdk for all platforms
#History:
#2012/01/08        Renjie create
#2013/02/20        Lina modify
#useage: just excute this script
#

echo -e "Start create sdk! \n"

#auto-release dir
autorelease="./"

#header files
headerfiles="$autorelease/../../../../../../Include/voH265.h
             $autorelease/../../../../../../Include/viMem.h
             $autorelease/../../../../../../Include/voVideo.h
             $autorelease/../../../../../../Include/voIndex.h
             $autorelease/../../../../../../Include/voMem.h
             $autorelease/../../../../../../Include/voIVCommon.h
             $autorelease/../../../../../../Include/voType.h"
#sample file           
samplecode=$autorelease/../../sample/voH265DecSample.c



#create autoRelease folder
if [ -d autoRelease ]; then
rm -rf autoRelease
fi
mkdir autoRelease

#copy header files
mkdir autoRelease/include
for item in $headerfiles
do
cp $item autoRelease/include
echo copied $item!
done


#copy sample code
mkdir autoRelease/sampleCode
cp $samplecode autoRelease/sampleCode
echo copied samle code!




echo -e "Finish create sdk!"
