<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: mpeg2_d_evc - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP31C.tmp" with contents
[
/nologo /W3 /Oxt /I "..\..\..\..\..\..\voRelease\Include\\" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /Fp"ARMV4Rel/mpeg2_d_evc.pch" /YX /Fo"ARMV4Rel/" /MC /c 
"H:\voRelease\SampleCode\voMPEG2_D_SAMLE.c"
]
Creating command line ""C:\Program Files\Microsoft eMbedded C++ 4.0\EVC\WCE420\bin\xicle42" @C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP31C.tmp" 
Creating temporary file "C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP31D.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:no /pdb:"ARMV4Rel/mpeg2_d_evc.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4Rel/mpeg2_d_evc.exe" /subsystem:windowsce,4.20 /align:"4096" /MACHINE:ARM 
.\ARMV4Rel\voMPEG2_D_SAMLE.obj
\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\lib\voMPEG2DecLib_EVC\ARMV4Rel\voMPEG2DecLib_EVC.lib
]
Creating command line ""C:\Program Files\Microsoft eMbedded C++ 4.0\EVC\WCE420\bin\xilinke42" @C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP31D.tmp"
<h3>Output Window</h3>
Compiling...
voMPEG2_D_SAMLE.c
Linking...
xilinke42 starting link
voMPEG2_D_SAMLE.obj : error LNK2001: unresolved external symbol clock
corelibc.lib(pegwmain.obj) : error LNK2019: unresolved external symbol WinMain referenced in function WinMainCRTStartup
ARMV4Rel/mpeg2_d_evc.exe : fatal error LNK1120: 2 unresolved externals
Error executing C:\Program Files\Microsoft eMbedded C++ 4.0\EVC\WCE420\bin\xilinke42.



<h3>Results</h3>
mpeg2_d_evc.exe - 3 error(s), 0 warning(s)
</pre>
</body>
</html>
