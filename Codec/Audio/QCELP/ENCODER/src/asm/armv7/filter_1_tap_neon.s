;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;**************************************************************

;**********************************************************************
;  do_pole_filter_1_tap_interp  function
;**********************************************************************
;void do_pole_filter_1_tap_interp(
;		short                     *input,
;		short                     *output,
;		short                     numsamples,
;		struct POLE_FILTER_1_TAP  *filter,
;		short                     update_flag 
;		)

	AREA	|.text|, CODE, READONLY
        EXPORT   filter_1_tap_interp_asm
;******************************
; constant
;******************************
; filter->delay         Offset      0
; filter->coeff         Offset      2
; filter->max_order     Offset      4
; filter->frac          Offset      6
; filter->memory        Offset      8

;******************************
; ARM register 
;******************************
; *input               RN           0
; *output              RN           1
; numsamples           RN           2
; *filter              RN           3
; update_flag          RN           4

filter_1_tap_interp_asm     FUNCTION

          STMFD          r13!, {r4 - r12, r14}
          SUB            r13, r13, #620                   ; *tmpbuf = gArray0
          LDR            r4, [r3, #8]                     ; filter->memory
          LDRSH          r5, [r3, #4]                     ; filter->max_order
          MOV            r6, r13                          ; tmpbuf
          ADD            r4, r4, r5, LSL #1               ; filter->memory + filter->max_order - 1 
          SUB            r4, r4, #2  
	  ;r4 --- indata, r6 --- outdata
          MOV            r12, r5
          CMP            r2, #10
	  BEQ            Lable_10
	  CMP            r2, #20
	  BEQ            Lable_20
	  CMP            r2, #40
	  BEQ            Lable_40

Lable_10
LOOP10_1
          LDRSH          r7,  [r4], #-2                   ; *indata--
          LDRSH          r8,  [r4], #-2
          LDRSH          r9,  [r4], #-2 
          LDRSH          r10, [r4], #-2
          LDRSH          r11, [r4], #-2
          STRH           r7,  [r6], #2
          STRH           r8,  [r6], #2
          STRH           r9,  [r6], #2
          STRH           r10, [r6], #2
          STRH           r11, [r6], #2
          LDRSH          r7,  [r4], #-2                   ; *indata--
          LDRSH          r8,  [r4], #-2
          LDRSH          r9,  [r4], #-2 
          LDRSH          r10, [r4], #-2
          LDRSH          r11, [r4], #-2
          STRH           r7,  [r6], #2
          STRH           r8,  [r6], #2
          STRH           r9,  [r6], #2
          STRH           r10, [r6], #2
          STRH           r11, [r6], #2
          SUBS           r12, r12, #10
          BNE            LOOP10_1
          MOV            r6, r0
          ADD            r4, r13, r5, LSL #1              ; outdata = tmpbuf + filter->max_order

	  VLD1.S16       {D0, D1}, [r6]!
          LDR            r7, [r6]
          VST1.S16       {D0, D1}, [r4]!
          STR            r7, [r4]

          LDRSH          r6, [r3, #6]                   ; r6 --- filter->frac
          LDRSH          r7, [r3, #2]                   ; r7 --- filter->coeff
          LDRSH          r8, [r3]                       ; r8 --- filter->delay
          CMP            r6, #0
          BNE            Lable1
          ADD            r9, r13, r5, LSL #1            ; outdata = tmpbuf + filter->max_order
          SUB            r11, r5, r8
          ADD            r10, r13, r11, LSL #1          ; indata = tmpbuf + filter->max_order-filter->delay
	  VDUP.S16       D30, r7
	  MOV            r7, r9
	  VMOV.S32       Q6, #0x800
	  VMOV.S32       Q7, #0x800
	  VMOV.S32       Q8, #0x800
          VLD1.S16       {D0, D1}, [r10]!
	  VLD1.S16       {D2}, [r10]!
	  VLD1.S16       {D4, D5}, [r7]!
	  VLD1.S16       {D6}, [r7]!
          VMLAL.S16      Q6, D0, D30
	  VMLAL.S16      Q7, D1, D30
	  VMLAL.S16      Q8, D2, D30
	  VSHRN.S32      D0, Q6, #12
	  VSHRN.S32      D1, Q7, #12
	  VSHRN.S32      D2, Q8, #12
	  VADD.S16       D0, D0, D4
	  VADD.S16       D1, D1, D5
	  VADD.S16       D2, D2, D6
	  VST1.S16       {D0, D1}, [r9]!
	  VMOV.S16       r8, D2[0]
	  VMOV.S16       r12, D2[1]
	  STRH           r8, [r9], #2
	  STRH           r12, [r9], #2

Lable1
          LDR            r14, [r13, #660]                ;load updata_flag
          ADD            r9, r13, r5, LSL #1             ;indata
          MOV            r10, r1                         ;outdata = output
	  VLD1.S16       {D0, D1}, [r9]!
	  LDR            r6, [r9]          
	  VST1.S16       {D0, D1}, [r10]!
	  STR            r6, [r10]
          CMP            r14, #1
          BNE            filter_1_tap_interp_end
          ADD            r7, r5, r2
          SUB            r7, r7, #1
          ADD            r8, r13, r7, LSL #1
          LDR            r4, [r3, #8]                     ; filter->memory

          ; r4 --- filter->memory       r5 --- filter->max_order
          ; r8 --- indata = tmpbuf + filter->max_order + numsamples - 1
          ; outdata = filter->memory
LOOP10_2
          LDRSH          r6,  [r8], #-2
          LDRSH          r7,  [r8], #-2
          LDRSH          r9,  [r8], #-2
          LDRSH          r10, [r8], #-2
          LDRSH          r11, [r8], #-2
          STRH           r6,  [r4], #2
          STRH           r7,  [r4], #2
          STRH           r9,  [r4], #2
          STRH           r10, [r4], #2
          STRH           r11, [r4], #2   
          LDRSH          r6,  [r8], #-2
          LDRSH          r7,  [r8], #-2
          LDRSH          r9,  [r8], #-2
          LDRSH          r10, [r8], #-2
          LDRSH          r11, [r8], #-2
          STRH           r6,  [r4], #2
          STRH           r7,  [r4], #2
          STRH           r9,  [r4], #2
          STRH           r10, [r4], #2
          STRH           r11, [r4], #2
          SUBS           r5, r5, #10
          BNE            LOOP10_2 
          BL             filter_1_tap_interp_end


Lable_20
LOOP20_1
          LDRSH          r7,  [r4], #-2                   ; *indata--
          LDRSH          r8,  [r4], #-2
          LDRSH          r9,  [r4], #-2 
          LDRSH          r10, [r4], #-2
          LDRSH          r11, [r4], #-2
          STRH           r7,  [r6], #2
          STRH           r8,  [r6], #2
          STRH           r9,  [r6], #2
          STRH           r10, [r6], #2
          STRH           r11, [r6], #2
          LDRSH          r7,  [r4], #-2                   ; *indata--
          LDRSH          r8,  [r4], #-2
          LDRSH          r9,  [r4], #-2 
          LDRSH          r10, [r4], #-2
          LDRSH          r11, [r4], #-2
          STRH           r7,  [r6], #2
          STRH           r8,  [r6], #2
          STRH           r9,  [r6], #2
          STRH           r10, [r6], #2
          STRH           r11, [r6], #2
          SUBS           r12, r12, #10
          BNE            LOOP20_1
          MOV            r6, r0
          ADD            r4, r13, r5, LSL #1              ; outdata = tmpbuf + filter->max_order

	  VLD1.S16       {D0, D1, D2, D3}, [r6]!
          VLD1.S16       {D4}, [r6]!
          VST1.S16       {D0, D1, D2, D3}, [r4]!
          VST1.S16       {D4}, [r4]!

          LDRSH          r6, [r3, #6]                   ; r6 --- filter->frac
          LDRSH          r7, [r3, #2]                   ; r7 --- filter->coeff
          LDRSH          r8, [r3]                       ; r8 --- filter->delay
          CMP            r6, #0
          BNE            Lable2
          ADD            r9, r13, r5, LSL #1            ; outdata = tmpbuf + filter->max_order
          SUB            r11, r5, r8
          ADD            r10, r13, r11, LSL #1          ; indata = tmpbuf + filter->max_order-filter->delay
	  VDUP.S16       D30, r7
	  MOV            r7, r9
	  VMOV.S32       Q6, #0x800
	  VMOV.S32       Q7, #0x800
	  VMOV.S32       Q8, #0x800
	  VMOV.S32       Q9, #0x800
	  VMOV.S32       Q10, #0x800
          VLD1.S16       {D0, D1, D2, D3}, [r10]!
	  VLD1.S16       {D4}, [r10]!
	  VLD1.S16       {D6, D7, D8, D9}, [r7]!
	  VLD1.S16       {D10}, [r7]!
          VMLAL.S16      Q6, D0, D30
	  VMLAL.S16      Q7, D1, D30
	  VMLAL.S16      Q8, D2, D30
	  VMLAL.S16      Q9, D3, D30
	  VMLAL.S16      Q10, D4, D30
	  VSHRN.S32      D0, Q6, #12
	  VSHRN.S32      D1, Q7, #12
	  VSHRN.S32      D2, Q8, #12
	  VSHRN.S32      D3, Q9, #12
	  VSHRN.S32      D4, Q10, #12
	  VADD.S16       D0, D0, D6
	  VADD.S16       D1, D1, D7
	  VADD.S16       D2, D2, D8
	  VADD.S16       D3, D3, D9
	  VADD.S16       D4, D4, D10
	  VST1.S16       {D0, D1, D2, D3}, [r9]!
	  VST1.S16       {D4}, [r9]!
Lable2
          LDR            r14, [r13, #660]                ;load updata_flag
          ADD            r9, r13, r5, LSL #1              ;indata
          MOV            r10, r1                          ;outdata = output
	  VLD1.S16       {D0, D1, D2, D3}, [r9]!
	  VLD1.S16       {D4}, [r9]!
	  VST1.S16       {D0, D1, D2, D3}, [r10]!
	  VST1.S16       {D4}, [r10]!
          CMP            r14, #1
          BNE            filter_1_tap_interp_end
          ADD            r7, r5, r2
          SUB            r7, r7, #1
          ADD            r8, r13, r7, LSL #1
          LDR            r4, [r3, #8]                     ; filter->memory
	  ; r4 --- filter->memory       r5 --- filter->max_order
          ; r8 --- indata = tmpbuf + filter->max_order + numsamples - 1
          ; outdata = filter->memory
LOOP20_2
          LDRSH          r6,  [r8], #-2
          LDRSH          r7,  [r8], #-2
          LDRSH          r9,  [r8], #-2
          LDRSH          r10, [r8], #-2
          LDRSH          r11, [r8], #-2
          STRH           r6,  [r4], #2
          STRH           r7,  [r4], #2
          STRH           r9,  [r4], #2
          STRH           r10, [r4], #2
          STRH           r11, [r4], #2   
          LDRSH          r6,  [r8], #-2
          LDRSH          r7,  [r8], #-2
          LDRSH          r9,  [r8], #-2
          LDRSH          r10, [r8], #-2
          LDRSH          r11, [r8], #-2
          STRH           r6,  [r4], #2
          STRH           r7,  [r4], #2
          STRH           r9,  [r4], #2
          STRH           r10, [r4], #2
          STRH           r11, [r4], #2
          SUBS           r5, r5, #10
          BNE            LOOP20_2 
          BL             filter_1_tap_interp_end
Lable_40
LOOP40_1
          LDRSH          r7,  [r4], #-2                   ; *indata--
          LDRSH          r8,  [r4], #-2
          LDRSH          r9,  [r4], #-2 
          LDRSH          r10, [r4], #-2
          LDRSH          r11, [r4], #-2
          STRH           r7,  [r6], #2
          STRH           r8,  [r6], #2
          STRH           r9,  [r6], #2
          STRH           r10, [r6], #2
          STRH           r11, [r6], #2
          LDRSH          r7,  [r4], #-2                   ; *indata--
          LDRSH          r8,  [r4], #-2
          LDRSH          r9,  [r4], #-2 
          LDRSH          r10, [r4], #-2
          LDRSH          r11, [r4], #-2
          STRH           r7,  [r6], #2
          STRH           r8,  [r6], #2
          STRH           r9,  [r6], #2
          STRH           r10, [r6], #2
          STRH           r11, [r6], #2
          SUBS           r12, r12, #10
          BNE            LOOP40_1
          MOV            r6, r0
          ADD            r4, r13, r5, LSL #1              ; outdata = tmpbuf + filter->max_order

	  VLD1.S16       {D0, D1, D2, D3}, [r6]!
          VLD1.S16       {D4, D5, D6, D7}, [r6]!
	  VLD1.S16       {D8, D9}, [r6]!
          VST1.S16       {D0, D1, D2, D3}, [r4]!
          VST1.S16       {D4, D5, D6, D7}, [r4]!
	  VST1.S16       {D8, D9}, [r4]!
          LDRSH          r6, [r3, #6]                   ; r6 --- filter->frac
          LDRSH          r7, [r3, #2]                   ; r7 --- filter->coeff
          LDRSH          r8, [r3]                       ; r8 --- filter->delay
          CMP            r6, #0
          BNE            Lable3
          ADD            r9, r13, r5, LSL #1            ; outdata = tmpbuf + filter->max_order
          SUB            r11, r5, r8
          ADD            r10, r13, r11, LSL #1          ; indata = tmpbuf + filter->max_order-filter->delay
	  VDUP.S16       D30, r7
	  MOV            r7, r9
	  VMOV.S32       Q6, #0x800
	  VMOV.S32       Q7, #0x800
	  VMOV.S32       Q8, #0x800
	  VMOV.S32       Q9, #0x800
	  VMOV.S32       Q10, #0x800
          VLD1.S16       {D0, D1, D2, D3}, [r10]!
	  VLD1.S16       {D4}, [r10]!
	  VLD1.S16       {D6, D7, D8, D9}, [r7]!
	  VLD1.S16       {D10}, [r7]!
          VMLAL.S16      Q6, D0, D30
	  VMLAL.S16      Q7, D1, D30
	  VMLAL.S16      Q8, D2, D30
	  VMLAL.S16      Q9, D3, D30
	  VMLAL.S16      Q10, D4, D30
	  VSHRN.S32      D0, Q6, #12
	  VSHRN.S32      D1, Q7, #12
	  VSHRN.S32      D2, Q8, #12
	  VSHRN.S32      D3, Q9, #12
	  VSHRN.S32      D4, Q10, #12
	  VADD.S16       D0, D0, D6
	  VADD.S16       D1, D1, D7
	  VADD.S16       D2, D2, D8
	  VADD.S16       D3, D3, D9
	  VADD.S16       D4, D4, D10
	  VST1.S16       {D0, D1, D2, D3}, [r9]!
	  VST1.S16       {D4}, [r9]!
	  VMOV.S32       Q6, #0x800
	  VMOV.S32       Q7, #0x800
	  VMOV.S32       Q8, #0x800
	  VMOV.S32       Q9, #0x800
	  VMOV.S32       Q10, #0x800
          VLD1.S16       {D0, D1, D2, D3}, [r10]!
	  VLD1.S16       {D4}, [r10]!
	  VLD1.S16       {D6, D7, D8, D9}, [r7]!
	  VLD1.S16       {D10}, [r7]!
          VMLAL.S16      Q6, D0, D30
	  VMLAL.S16      Q7, D1, D30
	  VMLAL.S16      Q8, D2, D30
	  VMLAL.S16      Q9, D3, D30
	  VMLAL.S16      Q10, D4, D30
	  VSHRN.S32      D0, Q6, #12
	  VSHRN.S32      D1, Q7, #12
	  VSHRN.S32      D2, Q8, #12
	  VSHRN.S32      D3, Q9, #12
	  VSHRN.S32      D4, Q10, #12
	  VADD.S16       D0, D0, D6
	  VADD.S16       D1, D1, D7
	  VADD.S16       D2, D2, D8
	  VADD.S16       D3, D3, D9
	  VADD.S16       D4, D4, D10
	  VST1.S16       {D0, D1, D2, D3}, [r9]!
	  VST1.S16       {D4}, [r9]!
Lable3
          LDR            r14, [r13, #660]                ;load updata_flag
          ADD            r9, r13, r5, LSL #1              ;indata
          MOV            r10, r1                          ;outdata = output
	  VLD1.S16       {D0, D1, D2, D3}, [r9]!
	  VLD1.S16       {D4, D5, D6, D7}, [r9]!
	  VLD1.S16       {D8, D9}, [r9]!
	  VST1.S16       {D0, D1, D2, D3}, [r10]!
	  VST1.S16       {D4, D5, D6, D7}, [r10]!
	  VST1.S16       {D8, D9}, [r10]!
          CMP            r14, #1
          BNE            filter_1_tap_interp_end

          ADD            r7, r5, r2
          SUB            r7, r7, #1
          ADD            r8, r13, r7, LSL #1
          LDR            r4, [r3, #8]                     ; filter->memory
          ; r4 --- filter->memory       r5 --- filter->max_order
          ; r8 --- indata = tmpbuf + filter->max_order + numsamples - 1
          ; outdata = filter->memory
LOOP40_2
          LDRSH          r6,  [r8], #-2
          LDRSH          r7,  [r8], #-2
          LDRSH          r9,  [r8], #-2
          LDRSH          r10, [r8], #-2
          LDRSH          r11, [r8], #-2
          STRH           r6,  [r4], #2
          STRH           r7,  [r4], #2
          STRH           r9,  [r4], #2
          STRH           r10, [r4], #2
          STRH           r11, [r4], #2   
          LDRSH          r6,  [r8], #-2
          LDRSH          r7,  [r8], #-2
          LDRSH          r9,  [r8], #-2
          LDRSH          r10, [r8], #-2
          LDRSH          r11, [r8], #-2
          STRH           r6,  [r4], #2
          STRH           r7,  [r4], #2
          STRH           r9,  [r4], #2
          STRH           r10, [r4], #2
          STRH           r11, [r4], #2
          SUBS           r5, r5, #10
          BNE            LOOP40_2 
          BL             filter_1_tap_interp_end    
           
filter_1_tap_interp_end
    
          ADD        r13, r13, #620
          LDMFD      r13!, {r4 - r12, r15} 
          ENDFUNC
        
          END


