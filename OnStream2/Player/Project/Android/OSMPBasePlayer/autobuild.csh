#! /bin/csh -f

#OSMPBasePlayer.jar build here

set curDir = `pwd`

while ($#argv >= 1) 
	if ("$argv[1]" == "-h") then
		echo "Usage: $1 [-h] [-cleanup]"
		echo "       -h      : show command help"
		echo "       -cleanup: clean up"
		exit 0
    else if ("$argv[1]" == "-cleanup") then
		shift
		rm -fr ./bin
		rm -fr ./gen
		#rm -fr ./doc 
		shift
		continue
    else
		echo "Error,unknown command line option $argv[1],try $0 -h for help"
		exit 1
	endif
end

# echo "---> voOSBasePlayer - 1: Check android sdk ..."
# which aapt >& /dev/null
# if ($status != 0) then
    # echo "Android sdk not found. please download and install from "
    # echo "http://developer.android.com/index/html"
    # exit 1
# endif

# echo "---> voOSBasePlayer - 2: Check java sdk ..."
# which javac >& /dev/null
# if ($status != 0) then
    # echo "Java sdk not found, please download and install..."
    # exit 1
# endif
# echo "---> Java SDK found [OK] ..."

# echo "---> voOSBasePlayer - 1: Build voOSUtils.jar ..."
# cd ../OSMPUtils
# ./autobuild.csh -cleanup
# ./autobuild.csh
# cd $curDir
# echo "---> Build voOSUtils.jar class [OK] ..."

rm -fr ./bin
rm -fr ./gen

set JarOutDir = "../../../../Jars"

echo "---> voOSBasePlayer - 1: Build voOSEngine.jar and voOSUtils.jar..."
if (! -e ${JarOutDir}/voOSEngine.jar) then
	cd ../../../../Engine/Project/Android/OSMPEngine/
	chmod +x ./autobuild.csh
	./autobuild.csh
	cd $curDir
endif
echo "---> Build voOSEngine.jar class [OK] ..."

echo "---> voOSBasePlayer - 2: Build voOSDataSource.jar ..."
if (! -e ${JarOutDir}/voOSDataSource.jar) then
	cd ../../../../DataSource/Project/Android/OSMPDataSource/
	chmod +x ./autobuild.csh
	./autobuild.csh
	cd $curDir
endif
echo "---> Build voOSDataSource.jar class [OK] ..."

echo "---> voOSBasePlayer - 3: Get android SDK path here ..."
set target = `grep "target=" "project.properties" | awk -F "=" '{print $2}'`
set sdkAndroidPath = `which android`
if (0 != $status) then
    echo "Make sure you have installed android SDK and set env variable for Linux"
    exit 1
endif
set sdkPath = $sdkAndroidPath:h:h
echo "---> Get android SDK path [OK] ..."

echo "---> voOSBasePlayer - 5: Build Android Gen R.java ..."
cd $curDir
mkdir -p ./gen
aapt package -m -f -J gen -S res -I $sdkPath/platforms/$target/android.jar -M AndroidManifest.xml
if ($status != 0) then
    echo "Fail to Build R.java"
    exit 1
endif

echo "---> voOSBasePlayer - 6: Build java source (class) ..."
mkdir -p ./bin
javac -encoding UTF-8 -target 1.6 -bootclasspath $sdkPath/platforms/$target/android.jar \
      -classpath ${JarOutDir}/voOSUtils.jar:${JarOutDir}/voOSDataSource.jar:${JarOutDir}/voOSEngine.jar \
      -d bin $curDir/src/com/visualon/OSMPBasePlayer/*.java
echo "---> Build java class [OK] ..."

echo "---> voOSBasePlayer - 7: Build android resource package here"
aapt package -f -M ./AndroidManifest.xml -S ./res -I $sdkPath/platforms/$target/android.jar -F $curDir/bin/res 
echo "---> Build resource package over " 

echo "---> voOSBasePlayer - 8: Build voOSBasePlayer.jar "
mkdir -p {com}/{visualon}/{OSMPBasePlayer}
cp -fu ./bin/com/visualon/OSMPBasePlayer/*.class ./com/visualon/OSMPBasePlayer/
jar -cvf ${JarOutDir}/voOSBasePlayer.jar ./com/visualon/OSMPBasePlayer/*.class ./assets/*
if ($status != 0) then
    echo "Fail to build voOSBasePlayer.jar"
    exit 1
endif
echo "---> Build voOSBasePlayer.jar finished [OK]"
echo "---> Cleaning temp file and folder..."
sleep 1
rm -fr ./com
rm -fr ./bin
rm -fr ./gen
rm ./libs/*.jar
#rm -fr ./doc 
echo "---> Finished voOSBasePlayer.jar building ..."
