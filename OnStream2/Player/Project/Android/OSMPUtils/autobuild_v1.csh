#! /bin/csh -f

#OSMPUtils.jar build here
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
		rm -fr ./doc 
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

echo "---> OSMPUtils.jar Step 1: Check android sdk ..."
which aapt >& /dev/null
if ($status != 0) then
    echo "Android sdk not found. please download and install from "
    echo "http://developer.android.com/index/html"
    exit 1
endif

echo "---> OSMPUtils.jar Step 2: Check java sdk ..."
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

cd $curDir
mkdir -p ./gen
echo "---> OSMPUtils.jar Step 3: Build Android Gen R.java ..."
cd $curDir
aapt package -m -f -J $curDir/gen -S $curDir/res -I $sdkPath/platforms/$target/android.jar -M $curDir/AndroidManifest.xml
if ($status != 0) then
    echo "Fail to Build R.java"
    exit 1
endif

echo "---> OSMPUtils.jar Step 4: Build java source (class) ..."
cd $curDir
mkdir -p ./bin

javac -encoding UTF-8 -target 1.6 -bootclasspath $sdkPath/platforms/$target/android.jar -d $curDir/bin $curDir/src/com/visualon/OSMPUtils/*.java $curDir/src/com/visualon/OSMPSubTitle/*.java
echo "---> Build java class [OK] ..."

#echo "---> OSMPUtils.jar Step 5: Building Android dex ..."
#dx --dex --output=$curDir/bin/classes.dex $curDir/bin
#echo "---> Building Android Dex [OK] ..."

echo "---> OSMPUtils.jar Step 5: Build android resource package here"
aapt package -f -M ./AndroidManifest.xml -S ./res -I $sdkPath/platforms/$target/android.jar -F $curDir/bin/res 
echo "---> Build resource package [OK] " 

#echo "---> OSMPUtils.jar Step 6: Build Java Doc ...."
#set classFile=`find $sdkPath/platforms/$target -maxdepth 1 -type f -name android.jar`
#set javaList=`find src/com/visualon/OSMPUtils -name "*.java" `
#mkdir -p doc
#javadoc -d doc -quiet -header '</EM></TD><TD ALIGN="right" VALIGN="middle" ROWSPAN=3><EM>VisualOn OnStream SDK Reference Manual</EM></TD><TD ALIGN="right" VALIGN="middle" ROWSPAN=3><EM><img src="../../../visualon_logo.png" height="50" width="50" border="0" /></EM>' \
#    -classpath "voOSUtils.jar:$classFile" $javaList
#if ($status != 0) then
#    echo "$prefix==== Error: Java doc files creation failed ===="
#    exit 1
#endif

echo "---> OSMPUtils.jar Step 6: Build voOSUtils.jar "
mkdir -p {com}/{visualon}/{OSMPUtils,OSMPSubTitle}
cp -fu ./bin/com/visualon/OSMPUtils/*.class ./com/visualon/OSMPUtils/
cp -fu ./bin/com/visualon/OSMPSubTitle/*.class ./com/visualon/OSMPSubTitle/
#set classlist=`find ./com/visualon/OSMPUtils -name "*.class"`
#echo $classlist
jar -cvf voOSUtils.jar ./com/visualon/OSMPUtils/*.class ./com/visualon/OSMPSubTitle/*.class
if ($status != 0) then
    echo "Fail to build voOSUtils.jar"
    exit 1
endif
echo "---> Build voOSUtils.jar finished [OK]..."

echo "---> OSMPUtils.jar Step 7: Cleaning temp file and folder..."
sleep 1
rm -fr ./com
rm -fr ./bin
rm -fr ./gen
#rm -fr ./doc 
echo "---> Cleaning [OK]..."

echo "---> Finished voOSUtils.jar building ..."
