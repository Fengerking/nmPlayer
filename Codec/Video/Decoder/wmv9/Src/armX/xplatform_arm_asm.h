;*@@@+++@@@@******************************************************************
;
; Microsoft Windows Media
; Copyright (C) Microsoft Corporation. All rights reserved.
;
;*@@@---@@@@******************************************************************
; ARM Asm Include File 

PRO_VER				EQU		1   ; 0:simple profile, 1:main profile

PROFILE             EQU 0       ; always checkin with profile equ 0, otherwise match VC project settings

DECODEREPEATPAD_PROFILE         EQU 40      ; match autoprofile.h
DECINVIBQUANTESCCODE_PROFILE    EQU 15      ; match autoprofile.h

	IF UNDER_CE != 0
		MACRO 
		WMV_LEAF_ENTRY $function				          
		LEAF_ENTRY $function
		MEND

		MACRO
		WMV_ENTRY_END	
		ENTRY_END			
		MEND
	ELSE
		MACRO 
		WMV_LEAF_ENTRY $function
$function
		MEND

		MACRO
		WMV_ENTRY_END			
		MEND
	ENDIF

    IF PROFILE = 1

        IMPORT  |AsmFunctionProfileStart|
        IMPORt  |AsmFunctionProfileStop|

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; FUNCTION_PROFILE_START(fpDecode,DECODEPMB_PROFILE);
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        MACRO             
        FUNCTION_PROFILE_START $profileID
        STMFD   sp!, {r0-r3}
        MOV     R0, #$profileID
        BL      AsmFunctionProfileStart
        LDMFD   sp!, {r0-r3}
        MEND

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; FUNCTION_PROFILE_STOP(fpDecode,DECODEPMB_PROFILE);
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        MACRO             
        FUNCTION_PROFILE_STOP
        STMFD   sp!, {r0-r3}
        BL      AsmFunctionProfileStop
        LDMFD   sp!, {r0-r3}
        MEND

    ELSE

        MACRO             
        FUNCTION_PROFILE_START $profileID
        MEND

        MACRO             
        FUNCTION_PROFILE_STOP
        MEND

    ENDIF


    IF PROFILE_FRAMES_FUNCTION_COUNT = 1

        IMPORT WMVFunctionCountFrameProfileArm

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; use FRAME_PROFILE_COUNT as the second instruction in an ARM asm routine where the first saves at least the link (r14) reg.
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        MACRO             
        FRAME_PROFILE_COUNT
        bl    WMVFunctionCountFrameProfileArm
        MEND

        MACRO             
        FRAME_PROFILE_COUNT_SAVE_LR
        str     lr, [sp, #-4]!
        FRAME_PROFILE_COUNT
        ldr     lr, [sp], #+4
        MEND

    ELSE  ; PROFILE_FRAMES_FUNCTION_COUNT

        MACRO             
        FRAME_PROFILE_COUNT
        MEND

        MACRO             
        FRAME_PROFILE_COUNT_SAVE_LR
        MEND

    ENDIF ; PROFILE_FRAMES_FUNCTION_COUNT



    END
