#! /bin/csh -f

#voOSPlayer.jar build here

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

rm -fr ./bin
rm -fr ./gen
rm -fr ./doc 


set JarOutDir = "../../../../../Jars"

if (! -e ${JarOutDir}/voOSEngine.jar) then
	cd ../../../../../Engine/Project/Android/OSMPEngine/
	chmod +x ./autobuild.csh
	./autobuild.csh
	cd $curDir
endif
echo "---> Build voOSEngine.jar class [OK] ..."

if (! -e ${JarOutDir}/voOSDataSource.jar) then
	cd ../../../../../DataSource/Project/Android/OSMPDataSource/
	chmod +x ./autobuild.csh
	./autobuild.csh
	cd $curDir
endif
echo "---> Build voOSDataSource.jar class [OK] ..."

if (! -e ${JarOutDir}/voOSHDMICheck.jar) then
	cd ../../voHDMIStateCheckDemo/
	chmod +x ./autobuild.csh
	./autobuild.csh
	cd $curDir
endif
echo "---> Build voOSHDMICheck.jar class [OK] ..."

if (! -e ${JarOutDir}/voOSBasePlayer) then
	cd ../../OSMPBasePlayer/
	chmod +x ./autobuild.csh
	./autobuild.csh
	cd $curDir
endif
echo "---> Build voOSBasePlayer class [OK] ..."

echo "---> voOSPlayer.jar Step 1: Check android sdk ..."
which aapt >& /dev/null
if ($status != 0) then
    echo "Android sdk not found. please download and install from "
    echo "http://developer.android.com/index/html"
    exit 1
endif

echo "---> voOSPlayer.jar Step 2: Check java sdk ..."
which javac >& /dev/null
if ($status != 0) then
    echo "Java sdk not found, please download and install..."
    echo " http://java.oracle.com "
    exit 1
endif
echo "---> Java SDK found [OK] ..."

set target = `grep "target=" "project.properties" | awk -F "=" '{print $2}'`
set sdkAndroidPath = `which android`
if (0 != $status) then
    echo "Make sure you have installed android SDK and set env variable for Linux"
    exit 1
endif
set sdkPath = $sdkAndroidPath:h:h


echo "---> voOSPlayer - 4: Build Android Gen R.java ..."
cd $curDir
mkdir -p ./gen
aapt package -m -f -J gen -S res -I $sdkPath/platforms/$target/android.jar -M AndroidManifest.xml
if ($status != 0) then
    echo "Fail to Build R.java"
    exit 1
endif


echo "---> voOSPlayer - 5: Build java source (class) ..."
mkdir -p ./bin
javac -encoding UTF-8 -target 1.6 -bootclasspath $sdkPath/platforms/$target/android.jar \
      -classpath ${JarOutDir}/voOSUtils.jar:${JarOutDir}/voOSDataSource.jar:${JarOutDir}/voOSEngine.jar:${JarOutDir}/voOSHDMICheck.jar:${JarOutDir}/voOSBasePlayer.jar \
      -d bin $curDir/src/com/visualon/OSMPPlayer/*.java $curDir/src/com/visualon/OSMPPlayerImpl/*.java
echo "---> Build java class [OK] ..."

#echo "---> voOSPlayer - 6: Build android resource package here"
#aapt package -f -M ./AndroidManifest.xml -S ./res -I $sdkPath/platforms/$target/android.jar -F $curDir/bin/res 
#echo "---> Build resource package over " 

echo "---> voOSPlayer - 7: Build voOSPlayer.jar "
mkdir -p {com}/{visualon}/{OSMPPlayer,OSMPPlayerImpl}
cp -fu ./bin/com/visualon/OSMPPlayer/*.class ./com/visualon/OSMPPlayer/
cp -fu ./bin/com/visualon/OSMPPlayerImpl/*.class ./com/visualon/OSMPPlayerImpl/
jar -cvf ${JarOutDir}/voOSPlayer.jar ./com/visualon/OSMPPlayer/*.class ./com/visualon/OSMPPlayerImpl/*.class
if ($status != 0) then
    echo "Fail to build voOSPlayer.jar"
    exit 1
endif
echo "---> Build voOSPlayer.jar finished [OK]"
echo "---> Cleaning temp file and folder..."
sleep 1
rm -fr ./com
rm -fr ./bin
rm -fr ./gen


#echo "---> Step 8: Build Java Doc ...."
#rm -fr ./doc
#mkdir -p {./doc}
#set classFile=`find $sdkPath/platforms/$target -maxdepth 1 -type f -name android.jar`
#set VODXLibraryList=`find ./src/com/visualon/OSMPPlayer/ -name "*.java" -maxdepth 1 -type f`
#echo $VODXLibraryList

#echo "+++> Build voOSPlayer Doc... "
#javadoc -d ./doc -quiet -header '</EM></TD><TD ALIGN="right" VALIGN="middle" ROWSPAN=3><EM>VisualOn OnStream MediaPlayer+ API Reference Manual for Android #Platforms</EM></TD><TD ALIGN="right" VALIGN="middle" ROWSPAN=3><EM><img src="http://www.visualon.com/images/visualon_logo.png" height="50" width="50" #border="0" /></EM>' \
#    -classpath "../VODXPlayer/libs/VODXLibrary.jar:$classFile" $VODXLibraryList
#if ($status != 0) then
#    echo "Error: Java doc files creation failed ===="
#    exit 1
#endif

echo "---> Finished voOSPlayer.jar building ..."
