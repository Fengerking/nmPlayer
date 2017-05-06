# Microsoft Developer Studio Project File - Name="MAUDIA_D_IA32_C" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MAUDIA_D_IA32_C - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MAUDIO_D_IA32_C.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MAUDIO_D_IA32_C.mak" CFG="MAUDIA_D_IA32_C - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MAUDIA_D_IA32_C - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MAUDIA_D_IA32_C - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AudioCodec/VOI_MAD/example", WQKAAAAA"
# PROP Scc_LocalPath "..\..\example"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MAUDIA_D_IA32_C - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /GX /O2 /I "..\..\..\..\..\voiapp\voSDK\Include" /D "NDEBUG" /D "FPM_INTEL" /D "WIN32" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /D "ASO_ZEROCHECK" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\..\voiapp\voSDK\Lib\PC\voMP3Dec.lib"

!ELSEIF  "$(CFG)" == "MAUDIA_D_IA32_C - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Gm /GX /ZI /Od /I "../../../../../../voRelease/Include" /D "FPM_DEFAULT" /D "_LIB" /D "HAVE_CONFIG_H" /D "ASO_ZEROCHECK" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "DEBUG" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "MAUDIA_D_IA32_C - Win32 Release"
# Name "MAUDIA_D_IA32_C - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=..\..\src\bit.c
# End Source File
# Begin Source File

SOURCE=..\..\src\dct32.c
# End Source File
# Begin Source File

SOURCE=..\..\src\frame.c
# End Source File
# Begin Source File

SOURCE=..\..\src\huffman.c
# End Source File
# Begin Source File

SOURCE=..\..\src\layer12.c
# End Source File
# Begin Source File

SOURCE=..\..\src\layer3.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mem_align.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mp3dec.c
# End Source File
# Begin Source File

SOURCE=..\..\src\polyphase.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rq_table.c
# End Source File
# Begin Source File

SOURCE=..\..\src\stream.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\src\bit.h
# End Source File
# Begin Source File

SOURCE=..\..\src\config.h
# End Source File
# Begin Source File

SOURCE=..\..\src\frame.h
# End Source File
# Begin Source File

SOURCE=..\..\src\global.h
# End Source File
# Begin Source File

SOURCE=..\..\src\huffman.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mem_align.h
# End Source File
# Begin Source File

SOURCE=..\..\src\stream.h
# End Source File
# End Group
# End Target
# End Project
