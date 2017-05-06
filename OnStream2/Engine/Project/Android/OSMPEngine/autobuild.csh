#! /bin/csh -f
#voOSEngine.jar build here
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

set JarOutDir = "../../../../Jars"
if (! -e ${JarOutDir}/voOSUtils.jar) then
	cd ../../../../Player/Project/Android/OSMPUtils/
	chmod +x ./autobuild.csh
	./autobuild.csh
endif
cd $curDir

rm -fr ./bin
rm -fr ./gen

echo "---> voOSEngine.jar - Step 2:Get android SDK path ..."
set target = `grep "target=" "project.properties" | awk -F "=" '{print $2}'`
set sdkAndroidPath = `which android`
set sdkPath = $sdkAndroidPath:h:h
if ($status != 0) then
    echo "Make sure you have installed android SDK and set env variable for Linux"
    exit 1
endif
echo "---> Get android SDK path finished ..."

# echo "---> voOSEngine.jar - Step 1: Check android sdk ..."
# which aapt >& /dev/null
# if ($status != 0) then
    # echo "Android sdk not found. please download and install from "
    # echo "http://developer.android.com/index/html"
    # exit 1
# endif

# echo "---> voOSEngine.jar - Step 2: Check java sdk ..."
# which javac >& /dev/null
# if ($status != 0) then
    # echo "Java sdk not found, please download and install..."
    # echo " http://java.oracle.com "
    # exit 1
# endif
# echo "---> Java SDK found [OK] ..."

echo "---> voOSEngine.jar - Step 3: Build Android Gen R.java ..."
mkdir -p {gen}
aapt package -m -f -J gen -S res -I $sdkPath/platforms/$target/android.jar -M AndroidManifest.xml
if ($status != 0) then
    echo "Fail to Build R.java"
    exit 1
endif

echo "---> voOSEngine.jar - Step 4: Build java source (class) ..."
mkdir -p ./bin
javac -encoding UTF-8 -target 1.6 -bootclasspath $sdkPath/platforms/$target/android.jar -classpath ${JarOutDir}/voOSUtils.jar -d bin src/com/visualon/OSMPEngine/*.java $curDir/src/com/visualon/widget/*.java
echo "---> Build java class [OK]... " 

#echo "---> voOSEngine.jar -  Step 5: Building Android dex ..."
#dx --dex --output=$curDir/bin/classes.dex $curDir/bin
#echo "---> Building Android dex over ..."

echo "---> voOSEngine.jar -  Step 5: Build android resource package here"
aapt package -f -M ./AndroidManifest.xml -S ./res -I $sdkPath/platforms/$target/android.jar -F $curDir/bin/res 
echo "---> Build resource package [OK]... " 

# echo "---> voOSEngine.jar -  Step 7: Build Java Doc ...."
# set classFile=`find $sdkPath/platforms/$target -maxdepth 1 -type f -name android.jar`
# set javaList=`find src/com/visualon/OSMPEngine -name "*.java" `
# set voOSUtilsList = `find ../../../../Player/Project/Android/OSMPUtils/src/com/visualon/OSMPUtils -name "*.java" `
# echo $voOSUtilsList
# mkdir -p {doc}/{voOSEngine,voOSUtils}
# javadoc -d doc/voOSEngine -quiet -header '</EM></TD><TD ALIGN="right" VALIGN="middle" ROWSPAN=3><EM>VisualOn OnStream SDK Reference Manual</EM></TD><TD ALIGN="right" VALIGN="middle" ROWSPAN=3><EM><img src="../../../visualon_logo.png" height="50" width="50" border="0" /></EM>' \
#      -classpath "voOSEngine.jar:$classFile" $javaList
# javadoc -d doc/voOSUtils -quiet -header '</EM></TD><TD ALIGN="right" VALIGN="middle" ROWSPAN=3><EM>VisualOn OnStream SDK Reference Manual</EM></TD><TD ALIGN="right" VALIGN="middle" ROWSPAN=3><EM><img src="../../../visualon_logo.png" height="50" width="50" border="0" /></EM>' \
#      -classpath "voOSUtils.jar:$classFile" $voOSUtilsList	
# if ($status != 0) then
#      echo "$prefix==== Error: Java doc files creation failed ===="
#      exit 1
# endif

echo "---> voOSEngine.jar -  Step 6: Build voOSEngine.jar "
mkdir -p {com}/{visualon}/{OSMPEngine,widget}
cp -fu ./bin/com/visualon/OSMPEngine/*.class ./com/visualon/OSMPEngine/
cp -fu ./bin/com/visualon/widget/*.class ./com/visualon/widget/
#set classlist=`find ./com/visualon/OSMPEngine -name "*.class"`
#echo $classlist
jar -cvf ${JarOutDir}/voOSEngine.jar ./com/visualon/OSMPEngine/*.class ./com/visualon/widget/*.class
if ($status != 0) then
    echo "Fail to build voOSEngine.jar"
    exit 1
endif
echo "---> Build voOSEngine.jar finished [OK]..."

echo "---> voOSEngine.jar -  Step 7: Cleaning temp file and folder..."
sleep 1
rm -fr ./com
rm -fr ./bin
rm -fr ./gen
#rm -fr ./doc 
echo "---> Cleaning [OK]..."

echo "---> Finished voOSEngine.jar building ..."
