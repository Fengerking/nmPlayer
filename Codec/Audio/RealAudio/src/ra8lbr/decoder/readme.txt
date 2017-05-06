/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0 and Exhibits. 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
 * All Rights Reserved. 
 * 
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Community Source 
 * License Version 1.0 (the "RCSL"), including Attachments A though H, 
 * all available at http://www.helixcommunity.org/content/rcsl. 
 * You may also obtain the license terms directly from RealNetworks. 
 * You may not use this file except in compliance with the RCSL and 
 * its Attachments. There are no redistribution rights for the source 
 * code of this file. Please see the applicable RCSL for the rights, 
 * obligations and limitations governing use of the contents of the file. 
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 * https://rarvcode-tck.helixcommunity.org 
 * 
 * Contributor(s): 
 * 
 * ***** END LICENSE BLOCK ***** */ 

Notes about ra8lbr, the 32-bit fixed-point RA8 (low-bitrate) decoder

To build the reference decoder on x86 (Windows)
-----------------------------------------------
1) open projects\visualc\gecko2.dsw in Visual C++ 6.0
2) select testwrap as Active Project
3) choose Debug or Release build
4) build gecko2.exe (F7) - the output will be in either decoder\debug or decoder\release

This workspace contains two sub-projects:
  - gecko2.dsp builds a static library (gecko2.lib) which is the actual codec
  - testwrap.dsp builds a command-line executable which links in gecko2.lib and
      provides a simple test harness for using the codec

Only the files in gecko2.dsp need to be ported to the target platform.
  These are the *.c and *.h files located in the top-level directory (decoder) and 
  optionally in the asm\[platform] subdirectories (if assembly-language files are used).

The files in testwrap.dsp (located in decoder\testwrap\*.c) are only for testing, and ARE NOT
  part of the RealAudio codec. These SHOULD NOT be included in shipping products.
  Therefore, the test wrapper can be modified to suit your particular testing needs
  (for example, implementing platform-specific functions like file I/O on a new system).


To learn more about this codec
------------------------------

Look in the decoder\docs subdirectory

To test your implementation of this codec
-----------------------------------------

Look at the readme.txt in the subdirectory testutil archive, available 
  in the rarvcode-tck project.

This will explain how to use the test utilities and sample encoded audio files 
  which are located there.
