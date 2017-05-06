#! /bin/csh -f

echo "================ Begin to build OMX AL Wrapper ================"
set gru = ..
set curDir = `pwd`
cd $curDir
cd $curDir/linux/ndk/v6/jni/
ndk-build clean
ndk-build
cp ../libs/armeabi-v7a/libvoOMXALWrap.so ../../../../../../../../voProduct/trunk/OSMP+V2/trunk/Android/DemoPlayer/lib/armeabi/

echo "================ Begin to Package new APK ================"
cd $curDir
cd ../../../../voProduct/trunk/OSMP+V2/trunk/Android/Tools/
csh autoSigned.csh

echo "================ Begin to Install APK ================"
adb uninstall com.visualon.osmpDemoPlayer
adb install ./*.apk
echo "================ Install complete ================"