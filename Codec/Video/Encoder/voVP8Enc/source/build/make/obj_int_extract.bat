REM   Copyright (c) 2011 The WebM project authors. All Rights Reserved.
REM
REM   Use of this source code is governed by a BSD-style license
REM   that can be found in the LICENSE file in the root of the source
REM   tree. An additional intellectual property rights grant can be found
REM   in the file PATENTS.  All contributing project authors may
REM   be found in the AUTHORS file in the root of the source tree.
echo on

set ROOT_SRC=../../

cl /I "./" /I "%ROOT_SRC%" /nologo /c "%ROOT_SRC%vp8/common/asm_com_offsets.c"
cl /I "./" /I "%ROOT_SRC%" /nologo /c "%ROOT_SRC%vp8/encoder/asm_enc_offsets.c"

obj_int_extract.exe rvds "asm_com_offsets.obj" > "asm_com_offsets.asm"
obj_int_extract.exe rvds "asm_enc_offsets.obj" > "asm_enc_offsets.asm"

obj_int_extract.exe gas "asm_com_offsets.obj" > "asm_com_offsets.S"
obj_int_extract.exe gas "asm_enc_offsets.obj" > "asm_enc_offsets.S"