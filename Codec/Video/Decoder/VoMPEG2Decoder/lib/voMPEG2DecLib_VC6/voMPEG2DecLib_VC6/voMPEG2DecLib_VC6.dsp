# Microsoft Developer Studio Project File - Name="voMPEG2DecLib_VC6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=voMPEG2DecLib_VC6 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "voMPEG2DecLib_VC6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "voMPEG2DecLib_VC6.mak" CFG="voMPEG2DecLib_VC6 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "voMPEG2DecLib_VC6 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "voMPEG2DecLib_VC6 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/VideoCodec/VOIMPEG2/VoMPEG2Decoder/mpeg2_d_sample", EHRBAAAA"
# PROP Scc_LocalPath "..\..\..\mpeg2_d_sample"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "voMPEG2DecLib_VC6 - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\..\..\voRelease\Include\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "voMPEG2DecLib_VC6 - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\..\..\voRelease\Include\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

# Name "voMPEG2DecLib_VC6 - Win32 Release"
# Name "voMPEG2DecLib_VC6 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\idct_add.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpeg2Decoder.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpeg2MB.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpeg2Parser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpegBuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpegMC.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpegMem.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\idct_add.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpeg2DecGlobal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpeg2Decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpeg2MB.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpeg2Parser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpeg4DID.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpegBuf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpegMC.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpegMem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpegPort.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\voMpegReadbits.h
# End Source File
# End Group
# End Target
# End Project
