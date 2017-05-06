@*@@@+++@@@@******************************************************************
@
@ Microsoft Windows Media
@ Copyright (C) Microsoft Corporation. All rights reserved.
@
@*@@@---@@@@******************************************************************
@ ARM Asm Include File 

@------------------------------------------------------
@	//	Enable pld instruction in ASM code?
@------------------------------------------------------
.equ PLD_ENABLE          ,1   @// Enable PLD instruction or not in asm code. 1:enable, 0:disable
@*****************************************************************************************

.equ PRO_VER              , 1	@ 0:simple profile, 1:main profile

.equ PROFILE                         ,0
.equ DECODEREPEATPAD_PROFILE         ,40      @ match autoprofile.h
.equ DECINVIBQUANTESCCODE_PROFILE    ,15      @ match autoprofile.h

	.if UNDER_CE==0
		.macro WMV_LEAF_ENTRY function
			\function:
		.endm

		.macro WMV_ENTRY_END			
		.endm
	.else
		.macro WMV_LEAF_ENTRY function				          
			LEAF_ENTRY function
		.endm

		.macro WMV_ENTRY_END	
			ENTRY_END			
		.endm
	.endif

    .if PROFILE == 1

        IMPORT  |AsmFunctionProfileStart|
        IMPORt  |AsmFunctionProfileStop|

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        @ FUNCTION_PROFILE_START(fpDecode,DECODEPMB_PROFILE)@
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro             
        FUNCTION_PROFILE_START $profileID
        STMFD   sp!, {r0-r3}
        MOV     R0, #$profileID
        BL      AsmFunctionProfileStart
        LDMFD   sp!, {r0-r3}
        .endm

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        @ FUNCTION_PROFILE_STOP(fpDecode,DECODEPMB_PROFILE)@
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro             
        FUNCTION_PROFILE_STOP
        STMFD   sp!, {r0-r3}
        BL      AsmFunctionProfileStop
        LDMFD   sp!, {r0-r3}
        .endm

    .else

        .macro  FUNCTION_PROFILE_START profileID
        .endm

        .macro  FUNCTION_PROFILE_STOP
        .endm

    .endif


    .if PROFILE_FRAMES_FUNCTION_COUNT == 1

        IMPORT WMVFunctionCountFrameProfileArm

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        @ use FRAME_PROFILE_COUNT as the second instruction in an ARM asm routine where the first saves at least the link (r14) reg.
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro             
        FRAME_PROFILE_COUNT
        bl    WMVFunctionCountFrameProfileArm
        .endm

        .macro             
        FRAME_PROFILE_COUNT_SAVE_LR
        str     lr, [sp, #-4]!
        FRAME_PROFILE_COUNT
        ldr     lr, [sp], #+4
        .endm

    .else  @ PROFILE_FRAMES_FUNCTION_COUNT

        .macro  FRAME_PROFILE_COUNT
        .endm

        .macro  FRAME_PROFILE_COUNT_SAVE_LR
        .endm

    .endif @ PROFILE_FRAMES_FUNCTION_COUNT

    @.end

