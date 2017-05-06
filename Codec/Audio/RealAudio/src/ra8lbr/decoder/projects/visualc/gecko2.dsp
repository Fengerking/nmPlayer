# Microsoft Developer Studio Project File - Name="gecko2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gecko2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gecko2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gecko2.mak" CFG="gecko2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gecko2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gecko2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gecko2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gecko2___Win32_Release"
# PROP BASE Intermediate_Dir "gecko2___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\release"
# PROP Intermediate_Dir "..\..\rel_obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\..\..\..\common\include" /I "..\..\..\..\..\..\..\common\runtime\pub" /D "_WINDOWS" /D "NDEBUG" /D "WIN32" /D "REL_ENABLE_ASSERTS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"rel_obj/gecko2.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "gecko2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gecko2___Win32_Debug"
# PROP BASE Intermediate_Dir "gecko2___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\debug"
# PROP Intermediate_Dir "..\..\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "D:\source_safe_project\Visualizer\RealFormatSDK\src\codecs\common\include\hlxclib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FD /GZ /c
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

# Name "gecko2 - Win32 Release"
# Name "gecko2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\bitpack.c
# End Source File
# Begin Source File

SOURCE=..\..\buffers.c
# End Source File
# Begin Source File

SOURCE=..\..\category.c
# End Source File
# Begin Source File

SOURCE=..\..\couple.c
# End Source File
# Begin Source File

SOURCE=..\..\envelope.c
# End Source File
# Begin Source File

SOURCE=..\..\fft.c
# End Source File
# Begin Source File

SOURCE=..\..\gainctrl.c
# End Source File
# Begin Source File

SOURCE=..\..\gecko2codec.c
# End Source File
# Begin Source File

SOURCE=..\..\huffman.c
# End Source File
# Begin Source File

SOURCE=..\..\hufftabs.c
# End Source File
# Begin Source File

SOURCE=..\..\mlt.c
# End Source File
# Begin Source File

SOURCE=..\..\sqvh.c
# End Source File
# Begin Source File

SOURCE=..\..\trigtabs.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
