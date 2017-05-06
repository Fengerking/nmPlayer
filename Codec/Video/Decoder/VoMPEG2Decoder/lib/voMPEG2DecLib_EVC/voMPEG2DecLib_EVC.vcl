<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: voMPEG2DecLib_EVC - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP318.bat" with contents
[
@echo off
armasm -cpu ARM1136 ..\..\src\asm\mpeg4_MC_arm.asm ".\ARMV4Rel/mpeg4_MC_arm.obj" -list ".\ARMV4Rel/mpeg4_MC_arm.lst"
]
Creating command line "C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP318.bat"
Creating temporary file "C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP319.bat" with contents
[
@echo off
armasm -cpu ARM1136 ..\..\src\asm\idct_arm.asm ".\ARMV4Rel/idct_arm.obj" -list ".\ARMV4Rel/idct_arm.lst"
]
Creating command line "C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP319.bat"
Creating temporary file "C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP31A.tmp" with contents
[
/nologo /W3 /Oxt /I "..\..\..\..\..\..\voRelease\Include\\" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "NDEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "_LIB" /D "VOARM" /Fp"ARMV4Rel/voMPEG2DecLib_EVC.pch" /YX /Fo"ARMV4Rel/" /MC /c 
"H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\idct_add.c"
"H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c"
"H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2MB.c"
"H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c"
"H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpegBuf.c"
"H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpegMC.c"
"H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpegMem.c"
]
Creating command line ""C:\Program Files\Microsoft eMbedded C++ 4.0\EVC\WCE420\bin\xicle42" @C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP31A.tmp" 
Performing Custom Build Step on ..\..\src\asm\mpeg4_MC_arm.asm
Microsoft (R) ARM Macro Assembler Version 14.00.60131
Copyright (C) Microsoft Corporation.  All rights reserved.

Performing Custom Build Step on ..\..\src\asm\idct_arm.asm
Microsoft (R) ARM Macro Assembler Version 14.00.60131
Copyright (C) Microsoft Corporation.  All rights reserved.

Creating temporary file "C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP31B.tmp" with contents
[
/nologo /out:"ARMV4Rel\voMPEG2DecLib_EVC.lib" 
.\ARMV4Rel\idct_add.obj
.\ARMV4Rel\idct_arm.obj
.\ARMV4Rel\mpeg4_MC_arm.obj
.\ARMV4Rel\voMpeg2Decoder.obj
.\ARMV4Rel\voMpeg2MB.obj
.\ARMV4Rel\voMpeg2Parser.obj
.\ARMV4Rel\voMpegBuf.obj
.\ARMV4Rel\voMpegMC.obj
.\ARMV4Rel\voMpegMem.obj
]
Creating command line "link.exe -lib @C:\DOCUME~1\YU_REN~1\LOCALS~1\Temp\RSP31B.tmp"
<h3>Output Window</h3>
Compiling...
idct_add.c
voMpeg2Decoder.c
H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1631): warning #188: enumerated type mixed with another type
    return VORC_COM_OUT_OF_MEMORY;
           ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1638): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1657): warning #188: enumerated type mixed with another type
    return VORC_COM_WRONG_STATUS;
           ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1661): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1713): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1731): warning #188: enumerated type mixed with another type
    return VORC_COM_WRONG_STATUS;
           ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1748): warning #188: enumerated type mixed with another type
    return VORC_COM_WRONG_PARAM_ID;
           ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1751): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1770): warning #188: enumerated type mixed with another type
    return VORC_COM_WRONG_STATUS;
           ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1786): warning #188: enumerated type mixed with another type
      return VORC_COM_WRONG_STATUS;
             ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1830): warning #188: enumerated type mixed with another type
    return VORC_COM_WRONG_PARAM_ID;
           ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Decoder.c(1832): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

voMpeg2MB.c
voMpeg2Parser.c
H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(182): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(187): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(191): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(195): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(199): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(256): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(260): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(264): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(268): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(272): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpeg2Parser.c(365): warning #188: enumerated type mixed with another type
   return VORC_OK;
          ^

voMpegBuf.c
H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpegBuf.c(128): warning #186: pointless comparison of unsigned integer with zero
   for (i = 0; i < EDGE_SIZE; i++) {
                 ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpegBuf.c(144): warning #186: pointless comparison of unsigned integer with zero
   for (i = 0; i < EDGE_SIZE; i++) {
                 ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpegBuf.c(157): warning #186: pointless comparison of unsigned integer with zero
   for (i = 0; i < EDGE_SIZE2; i++) {
                 ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpegBuf.c(172): warning #186: pointless comparison of unsigned integer with zero
   for (i = 0; i < EDGE_SIZE2; i++) {
                 ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpegBuf.c(185): warning #186: pointless comparison of unsigned integer with zero
   for (i = 0; i < EDGE_SIZE2; i++) {
                 ^

H:\Visualizer\VideoCodec\VoiMPEG2\VoMPEG2Decoder\src\voMpegBuf.c(200): warning #186: pointless comparison of unsigned integer with zero
   for (i = 0; i < EDGE_SIZE2; i++) {
                 ^

voMpegMC.c
voMpegMem.c
Creating library...




<h3>Results</h3>
voMPEG2DecLib_EVC.lib - 0 error(s), 29 warning(s)
</pre>
</body>
</html>
