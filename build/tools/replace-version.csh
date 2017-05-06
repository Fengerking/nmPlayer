#!/bin/csh

set curDir = `pwd`

if ($#argv != 2) then 
  echo Usage:
  echo ./repalce-version.csh  old-version new-version
  echo ./replace-version.csh 3.7.0 3.7.1
  exit 0
endif

if ( ! -e ${curDir}/replace-version.csh ) then
  echo This program should be executed at (SVNRoot)/build/tools
  exit 0
endif

set p1 = `echo $1 | sed 's/\./\,/g'`
set p2 = `echo $2 | sed 's/\./\,/g'`

set fileA = "../config/version.txt"
set fileB = "../../OnStream2/BrowserPlugin/VisualOn/source/voPluginIE/voPlugInIE.rc"
set fileC = "../../OnStream2/BrowserPlugin/VisualOn/project/win32/npvoBrowserPlugin/config.h"
set fileD = "../../OnStream2/BrowserPlugin/VisualOn/source/voPluginIEWinless/voPlugInIE.rc"
set fileE = "../../OnStream2/BrowserPlugin/VisualOn/project/win32/npvoBrowserPlugin/npvoBrowserPlugin.rc"

sed -i -e "s/$1/$2/g" $fileA
sed -i -e "s/$1/$2/g" $fileB
sed -i -e "s/$1/$2/g" $fileC
sed -i -e "s/$1/$2/g" $fileD
sed -i -e "s/$p1/$p2/g" $fileE

