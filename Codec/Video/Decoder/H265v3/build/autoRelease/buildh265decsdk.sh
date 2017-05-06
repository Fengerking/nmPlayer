#!/bin/bash
#program:
#        Build codec sdk for all platforms
#History:
#2012/01/08        Renjie create
#2013/02/20        Lina modify
#useage: Please set the global config for your sdk case
#

#build tool
MAKE=buildh265decsdk.sh

#display usage
	echo "--------------------------------------------------------------"
	echo "           build for h265  decoder "
	echo "--------------------------------------------------------------"
	echo ""
	




#global config
#versions WIN32 ANDROIDNDKV6, ANDROIDNDKV7, IOSV6, IOSV7
versions="WIN32 ANDROIDNDKV6 ANDROIDNDKV7 IOSV6 IOSV7"

version=$1

#auto-release dir
autorelease="."

if [ "$version" == "WIN32" ]; then
echo -e "Start create sdk for WIN32! \n"
"C:/Program Files (x86)/Microsoft Visual Studio 9.0/Common7/IDE/devenv.exe" ../../sample/vc9/voH265DecSample.vcproj /clean "Release|Win32"
"C:/Program Files (x86)/Microsoft Visual Studio 9.0/Common7/IDE/devenv.exe" ../../sample/vc9/voH265DecSample.vcproj /rebuild "Release|Win32"

#lib file           
libfile=$autorelease/../../sample/vc9/Release/voH265DecLib.lib

#compare App           
compareApp=$autorelease/tools/win32/fileDiff

#Generate report App           
generateReportApp=$autorelease/tools/win32/generateReport

#Ref App           
RefApp=$autorelease/TAppDecoder.exe

#sample App           
sampleApp=$autorelease/../../sample/vc9/Release/voH265DecSample.exe

elif [ "$version" == "ANDROIDNDKV6" ]; then 
echo -e "Start create sdk for ANDROIDNDKV6! \n"
#toDo

elif [ "$version" == "ANDROIDNDKV7" ]; then 
echo -e "Start create sdk for ANDROIDNDKV7! \n"
cd  $autorelease/../Customer/ndk/v7/jni
ndk-build -B
cd  ../../../../autoRelease

mkdir -p autoRelease/buildAndroidV7/jni

#Makefile          
makefile=$autorelease/../sample/android/v7/jni/Android.mk
cp $makefile autoRelease/buildAndroidV7/jni

#lib file           
libfile=$autorelease/../Customer/ndk/v7/libs/armeabi-v7a/libvoH265Dec.so

#compare App           
compareApp=$autorelease/tools/linux/fileDiff

#Generate report App           
generateReportApp=$autorelease/tools/linux/generateReport

#Ref App           
RefApp=$autorelease/TAppDecoder.exe

elif [ "$version" == "IOSV6" ]; then 
echo -e "Start create sdk for IOSV6! \n"
#toDo

elif [ "$version" == "IOSV7" ]; then 
echo -e "Start create sdk for IOSV7! \n"
#toDo

else

echo "Usage: " $MAKE " [version] "	
echo " "
echo "For e.g.:"
echo "       "           $MAKE  WIN32 
echo " "
exit 1

fi


#copy lib
if [ -d autoRelease/lib ]; then
rm -rf autoRelease/lib
fi
mkdir autoRelease/lib
cp $libfile autoRelease/lib
echo copied lib!

#build sample APP if necessary
if [ "$version" == "ANDROIDNDKV7" ]; then
echo -e "Start build sample APP for ANDROIDNDKV7! \n"

cd autoRelease/buildAndroidV7/jni
ndk-build -B
cd ../../../

#sample App           
sampleApp=autoRelease/buildAndroidV7/libs/armeabi/voH265DecSample

elif [ "$version" == "IOSV6" ]; then 
echo -e "Start create sdk for IOSV6! \n"
#toDo

elif [ "$version" == "IOSV7" ]; then 
echo -e "Start create sdk for IOSV7! \n"
#toDo

fi

#copy sample APP
if [ -d autoRelease/sampleApp ]; then
rm -rf autoRelease/sampleApp
fi
mkdir autoRelease/sampleApp
cp $sampleApp autoRelease/sampleApp
echo copied sampleApp!


#prepare autoTest
if [ -d autoRelease/autoTest ]; then
rm -rf autoRelease/autoTest
fi
mkdir autoRelease/autoTest

mkdir autoRelease/autoTest/input
mkdir autoRelease/autoTest/ref
mkdir autoRelease/autoTest/tools

cp ./test_vectors.sh autoRelease/autoTest
cp $RefApp autoRelease/autoTest/tools
cp $sampleApp autoRelease/autoTest/tools
cp $compareApp autoRelease/autoTest/tools
cp $generateReportApp autoRelease/autoTest/tools

cp $libfile autoRelease/autoTest/tools

echo -e "Finish build sdk!"
