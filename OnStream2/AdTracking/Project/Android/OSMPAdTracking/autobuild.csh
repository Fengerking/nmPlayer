#!/bin/csh

#OSMPUtils.jar build here
set curDir = `pwd`
set jarName = "voOSTracking.jar"
set workDir = $curDir
set doClearup = 1
set comvisualon = "com/visualon"
set classPara = "./$comvisualon/OSMPAdTracking/*.class"
set javaPara = "$curDir/src/$comvisualon/OSMPAdTracking/*.java"
set compileMode = "release"
if ($?WB_OS) then
	set compileMode = $WB_OPTIM
endif

set JarOutDir = "../../../../Jars"
set ThirdAdmsJar = "../../../../../Thirdparty/Omniture/jar/admsAppLibrary.jar"
if (! -e $ThirdAdmsJar) then
	set ThirdAdmsJar = "$WB_SRC_DIR/Thirdparty/Omniture/jar/admsAppLibrary.jar"
endif
set ThirdComscoreJar = "../../../../../Thirdparty/ComScore/jar/comscore.jar"
if (! -e $ThirdComscoreJar) then
	set ThirdComscoreJar = "$WB_SRC_DIR/Thirdparty/ComScore/jar/comscore.jar"
endif
set ThirdNielsenJar = "../../../../../Thirdparty/Nielsen/jar/nielsenvideobeaconactivity.jar"
if (! -e $ThirdNielsenJar) then
	set ThirdNielsenJar = "$WB_SRC_DIR/Thirdparty/Nielsen/jar/nielsenvideobeaconactivity.jar"
endif
if (! -e $JarOutDir) then
	mkdir -p ${JarOutDir}/debug
endif

cd $curDir
while ($#argv >= 1) 
	if ("$1" == "-h") then
		echo "Usage: $1 [-h] [-cleanup]"
		echo "       -h      : show command help"
		echo "       -cleanup: clean up"
		exit 0
    else if ("$1" == "-cleanup") then
		shift
		set doClearup = 1
    else if ("$1" == "-i") then
		shift
		set workDir = $1
		shift
    else if ("$1" == "-j") then
		shift
		set jarName = $1
		shift
    else
		echo "Error,unknown command line option $1,try $0 -h for help"
		exit 1
	endif
end

if ("$doClearup" == "1") then
	echo ${jarName} clear up
	rm -fr $workDir/bin
	rm -fr $workDir/gen
endif

if (! -e ${JarOutDir}/voOSPlayer.jar) then
	cd ../../../../Player/Project/Android/SecurePlayer/VODXLibrary/
	chmod +x ./autobuild.csh
	./autobuild.csh
	cd $curDir
endif

echo "---> ${jarName} Step 1: Check android sdk ..."
which aapt >& /dev/null
if ($status != 0) then
    echo "Android sdk not found. please download and install from "
    echo "http://developer.android.com/index/html"
    exit 1
endif

which javac >& /dev/null
if ($status != 0) then
    echo "Java sdk not found, please download and install..."
    echo " http://java.oracle.com "
    exit 1
endif

cd $workDir
set target = `grep "target=" "project.properties" | awk -F "=" '{print $2}'`
set sdkAndroidPath = `which android`
if (0 != $status) then
    echo "Make sure you have installed android SDK and set env variable for Linux"
    exit 1
endif
set sdkPath = $sdkAndroidPath:h:h

#cd $workDir/src/$comvisualon
#foreach line(`ls`)
#echo $line
#	set javaPara = "$javaPara $workDir/src/${comvisualon}/${line}/*.java"
#	set classPara = "$classPara $workDir/${comvisualon}/${line}/*.class"
#end

cd $workDir

set srcToModify = "src/com/visualon/omsptracking/VOOSMPOmnitureTracking.java"
if ( $compileMode == "debug" ) then
	sed -i -e 's/setReportSuiteIDs("cbsicbscomapp,cnetbsiall")/setReportSuiteIDs("cbsicbscomapp-dev,cnetbsiall-dev")/' $srcToModify
	set jarName = "debug/voOSTracking.jar"
endif

mkdir -p $workDir/gen
echo "---> ${jarName} Step 2: Build Android Gen R.java ..."
aapt package -m -f -J $workDir/gen -S $workDir/res -I $sdkPath/platforms/$target/android.jar -M $workDir/AndroidManifest.xml
if ($status != 0) then
    echo "Fail to Build R.java"
    exit 1
endif

echo "---> ${jarName} Step 3: Build java source (class) ..."
cd $workDir
mkdir -p $workDir/bin
javac -encoding UTF-8 -target 1.6 -bootclasspath $sdkPath/platforms/$target/android.jar  \
	-classpath ${JarOutDir}/voOSPlayer.jar:${ThirdAdmsJar}:${ThirdNielsenJar}:${ThirdComscoreJar}:${JarOutDir}/voOSUtils.jar:${JarOutDir}/voOSDataSource.jar:${JarOutDir}/voOSEngine.jar:${JarOutDir}/voOSHDMICheck.jar:${JarOutDir}/voOSBasePlayer.jar \
	-d $workDir/bin $javaPara
echo "---> Build java class [OK] ..."


echo "---> ${jarName} Step 4: Build android resource package here"
aapt package -f -M $workDir/AndroidManifest.xml -S ./res -I $sdkPath/platforms/$target/android.jar -F $workDir/bin/res 
echo "---> Build resource package [OK] " 


echo "---> ${jarName} Step 5: Build jar "


cd $workDir/src/$comvisualon
foreach line(`ls`)
	mkdir -p $workDir/${comvisualon}/${line}
	cp -fu $workDir/bin/${comvisualon}/${line}/*.class $workDir/${comvisualon}/${line}
end
cd $workDir

jar -cvf ${JarOutDir}/${jarName} ${classPara}

if ($status != 0) then
    echo "ERROR: Fail to build ${jarName}"
    exit 1
endif
echo "---> Build ${jarName} finished [OK]..."

echo "---> ${jarName} Step 6: Cleaning temp file and folder..."
sleep 1
rm -fr $workDir/com
rm -fr $workDir/bin
rm -fr $workDir/gen
echo "---> Cleaning [OK]..."

echo "---> Finished ${jarName} building ..."
