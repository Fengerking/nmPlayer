#!/bin/csh -f

set JarOutDir = "../../../../Jars"
if (! -e $JarOutDir/debug) then
	mkdir -p ${JarOutDir}/debug
endif

echo "Begin to build voOSUtils_debug.jar"
set currentPath = `pwd`
if (! -f autobuild.csh) then
	echo "autobuild.csh doesn't exist in `pwd`"
	exit 1
endif
chmod +x ./autobuild_v1.csh
./autobuild_v1.csh
if ($status != 0) then
        echo "Fail to call autobuild.csh"
        exit 1
endif
mv voOSUtils.jar voOSUtils.jar_bak

echo "Begin to build voOSUtils_release.jar"

set voLogPath = ./src/com/visualon/OSMPUtils/
cd $voLogPath
echo "Current path is `pwd`"

#edit voLog.java private static final boolean m_bPrintLog = true -- >false
if (! -f voLog.java) then 
	echo "voLog.java doesn't exist in `pwd`"
	exit 1
endif
#--find and change true to false
sed -i 's/m_bPrintLog = true/m_bPrintLog = false/g' voLog.java
#try another build

cd $currentPath
./autobuild_v1.csh
if ($status != 0) then
	echo "Fail to call autobuild.csh"
	exit 1
endif
sed -i 's/m_bPrintLog = false/m_bPrintLog = true/g' $voLogPath/voLog.java

#rename file name
mv voOSUtils.jar ${JarOutDir}/voOSUtils.jar
mv voOSUtils.jar_bak ${JarOutDir}/debug/voOSUtils.jar

#Copy file to dst
#cp -fu voOSUtils.jar ../../../../DataSource/Project/Android/OSMPDataSource/libs/
#cp -fu voOSUtils.jar ../../../../Engine/Project/Android/OSMPEngine/libs
