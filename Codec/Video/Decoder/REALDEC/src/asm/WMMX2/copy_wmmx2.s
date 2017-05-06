;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2010		            *
;								 	                                    *
;***********************************************************************/

	AREA	|.text|, CODE
	
	EXPORT	WMMX2_CopyBlock8x8
	EXPORT	WMMX2_AddBlock8x8
	EXPORT	WMMX2_CopyBlock16x16
	EXPORT	WMMX2_AddBlock16x16

;-----------------------------------------------------------------------------------------------------
;void RV_FASTCALL CopyBlock8x8(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch)
;-----------------------------------------------------------------------------------------------------
   ALIGN 16
WMMX2_CopyBlock8x8	PROC
    stmdb     sp!,{r4-r11,lr}

	wldrd     wr0,[r0]        ;0
	add       r0,r0,r2
	wldrd     wr1,[r0]        ;1
	add       r0,r0,r2
	wldrd     wr2,[r0]        ;2
	add       r0,r0,r2
	wldrd     wr3,[r0]        ;3
	add       r0,r0,r2
	wldrd     wr4,[r0]        ;4
	add       r0,r0,r2
	wldrd     wr5,[r0]        ;5
	add       r0,r0,r2
	wldrd     wr6,[r0]        ;6
	add       r0,r0,r2
	wldrd     wr7,[r0]        ;7
	
	wstrd     wr0,[r1]
	add       r1,r1,r3
	wstrd     wr1,[r1]
	add       r1,r1,r3
	wstrd     wr2,[r1]
	add       r1,r1,r3
	wstrd     wr3,[r1]
	add       r1,r1,r3
	wstrd     wr4,[r1]
	add       r1,r1,r3
	wstrd     wr5,[r1]
	add       r1,r1,r3
	wstrd     wr6,[r1]
	add       r1,r1,r3
	wstrd     wr7,[r1]
	
    ldmia     sp!,{r4-r11,pc}
	ENDP	

;-----------------------------------------------------------------------------------------------------
;void RV_FASTCALL AddBlock8x8(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch)
;-----------------------------------------------------------------------------------------------------
   ALIGN 8
WMMX2_AddBlock8x8	PROC
    stmdb     sp!,{r4-r11,lr}

	wldrd     wr0,[r0]        ;0
	add       r0,r0,r3
	wldrd     wr8,[r1]
	add       r1,r1,r3
	wldrd     wr1,[r0]        ;1
	add       r0,r0,r3
	wldrd     wr9,[r1]
	add       r1,r1,r3
	wldrd     wr2,[r0]        ;2
	add       r0,r0,r3
	wldrd     wr10,[r1]
	add       r1,r1,r3
	wldrd     wr3,[r0]        ;3
	add       r0,r0,r3
	wldrd     wr11,[r1]
	add       r1,r1,r3
	wldrd     wr4,[r0]        ;4
	add       r0,r0,r3
	wldrd     wr12,[r1]
	add       r1,r1,r3
	wldrd     wr5,[r0]        ;5
	add       r0,r0,r3
	wldrd     wr13,[r1]
	add       r1,r1,r3
	wldrd     wr6,[r0]        ;6
	add       r0,r0,r3
	wldrd     wr14,[r1]
	add       r1,r1,r3
	wldrd     wr7,[r0]        ;7
	wldrd     wr15,[r1]
	
	wavg2br   wr0,wr0,wr8
	wstrd     wr0,[r2]
	add       r2,r2,r3
	wavg2br   wr1,wr1,wr9
	wstrd     wr1,[r2]
	add       r2,r2,r3
	wavg2br   wr2,wr2,wr10
	wstrd     wr2,[r2]
	add       r2,r2,r3
	wavg2br   wr3,wr3,wr11
	wstrd     wr3,[r2]
	add       r2,r2,r3
	wavg2br   wr4,wr4,wr12
	wstrd     wr4,[r2]
	add       r2,r2,r3
	wavg2br   wr5,wr5,wr13
	wstrd     wr5,[r2]
	add       r2,r2,r3
	wavg2br   wr6,wr6,wr14
	wstrd     wr6,[r2]
	add       r2,r2,r3
	wavg2br   wr7,wr7,wr15
	wstrd     wr7,[r2]
	
    ldmia     sp!,{r4-r11,pc}

	ENDP	

;-----------------------------------------------------------------------------------------------------
;void RV_FASTCALL CopyBlock16x16(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch)
;-----------------------------------------------------------------------------------------------------
   ALIGN 16
WMMX2_CopyBlock16x16	PROC
    stmdb     sp!,{r4-r11,lr}
    
    mov	      r9,#2
    
CopyBlock16x16Loop
	wldrd     wr0,[r0]         ;0
	wldrd     wr1,[r0,#8]
	add       r0,r0,r2
	wldrd     wr2,[r0]         ;1
	wldrd     wr3,[r0,#8]
	add       r0,r0,r2
	wldrd     wr4,[r0]         ;2
	wldrd     wr5,[r0,#8]
	add       r0,r0,r2
	wldrd     wr6,[r0]         ;3
	wldrd     wr7,[r0,#8]
	add       r0,r0,r2
	wldrd     wr8,[r0]         ;4
	wldrd     wr9,[r0,#8]
	add       r0,r0,r2
	wldrd     wr10,[r0]        ;5
	wldrd     wr11,[r0,#8]
	add       r0,r0,r2
	wldrd     wr12,[r0]        ;6
	wldrd     wr13,[r0,#8]
	add       r0,r0,r2
	wldrd     wr14,[r0]        ;7
	wldrd     wr15,[r0,#8]
	add       r0,r0,r2
	
	wstrd     wr0,[r1]
	wstrd     wr1,[r1,#8]
	add       r1,r1,r3
	wstrd     wr2,[r1]
	wstrd     wr3,[r1,#8]
	add       r1,r1,r3
	wstrd     wr4,[r1]
	wstrd     wr5,[r1,#8]
	add       r1,r1,r3
	wstrd     wr6,[r1]
	wstrd     wr7,[r1,#8]
	add       r1,r1,r3
	wstrd     wr8,[r1]
	wstrd     wr9,[r1,#8]
	add       r1,r1,r3
	wstrd     wr10,[r1]
	wstrd     wr11,[r1,#8]
	add       r1,r1,r3
	wstrd     wr12,[r1]
	wstrd     wr13,[r1,#8]
	add       r1,r1,r3
	wstrd     wr14,[r1]
	wstrd     wr15,[r1,#8]	
	add       r1,r1,r3
	
	subs      r9,r9,#1 
	bgt       CopyBlock16x16Loop
	
    ldmia     sp!,{r4-r11,pc}
	ENDP	

;-----------------------------------------------------------------------------------------------------
;void RV_FASTCALL AddBlock16x16(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch)
;-----------------------------------------------------------------------------------------------------
   ALIGN 8
WMMX2_AddBlock16x16	PROC
    stmdb     sp!,{r4-r11,lr}
    
    mov       r9,#4 

AddBlock16x16Loop
	wldrd     wr0,[r0]        ;0
	wldrd     wr1,[r0,#8]
	add       r0,r0,r3
	wldrd     wr8,[r1]
	wldrd     wr9,[r1,#8]
	add       r1,r1,r3
	wldrd     wr2,[r0]        ;1
	wldrd     wr3,[r0,#8]
	add       r0,r0,r3
	wldrd     wr10,[r1]
	wldrd     wr11,[r1,#8]
	add       r1,r1,r3
	wldrd     wr4,[r0]        ;2
	wldrd     wr5,[r0,#8]
	add       r0,r0,r3
	wldrd     wr12,[r1]
	wldrd     wr13,[r1,#8]
	add       r1,r1,r3
	wldrd     wr6,[r0]        ;3
	wldrd     wr7,[r0,#8]
	add       r0,r0,r3
	wldrd     wr14,[r1]
	wldrd     wr15,[r1,#8]
	add       r1,r1,r3
	
	wavg2br   wr0,wr0,wr8
	wstrd     wr0,[r2]
	wavg2br   wr1,wr1,wr9
	wstrd     wr1,[r2,#8]
	add       r2,r2,r3
	wavg2br   wr2,wr2,wr10
	wstrd     wr2,[r2]
	wavg2br   wr3,wr3,wr11
	wstrd     wr3,[r2,#8]
	add       r2,r2,r3
	wavg2br   wr4,wr4,wr12
	wstrd     wr4,[r2]
	wavg2br   wr5,wr5,wr13
	wstrd     wr5,[r2,#8]
	add       r2,r2,r3
	wavg2br   wr6,wr6,wr14
	wstrd     wr6,[r2]
	wavg2br   wr7,wr7,wr15
	wstrd     wr7,[r2,#8]
	add       r2,r2,r3
	
	subs      r9,r9,#1 
	bgt       AddBlock16x16Loop
	
    ldmia     sp!,{r4-r11,pc}

	ENDP		