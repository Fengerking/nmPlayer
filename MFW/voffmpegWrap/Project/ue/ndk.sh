
cd /Volumes/Data/Project/Numen/trunk/MFW/voffmpeg/Project/linux/ndk/jni

ndk-build 2>&1

adb push ../libs/armeabi/libvoH264Dec.so /data/data/com.visualon.osmpDemoPlayer/lib/libvoH264Dec_v7.so
adb push ../libs/armeabi/libvoH264Dec.so /data/data/com.visualon.osmpDemoPlayer/lib/libvoH264Dec.so
#adb push ../libs/armeabi/libvompEngn.so /data/local/OnStreamPlayer/lib