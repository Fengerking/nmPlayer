;************************************************************************
;                                                                                         *
;	VisualOn, Inc Confidential and Proprietary, 2005                 *
;								 	                                                      *
;***********************************************************************/

	AREA	|text|, CODE

	;-------------------------------------------------------
	;EXPORT ArmIdctA
	;EXPORT ArmIdctB
	;EXPORT ArmIdctC
	;EXPORT ArmIdctD  ;huwei 20080219 downsmaple
	;EXPORT ArmIdctE  ;huwei 20080618 Bframe_field
	;EXPORT ArmIdctF
	;EXPORT ArmIdctG  ;huwei 20090904 IDCT4x4
	;-------------------------------------------------------
	
	EXPORT __voMPEG2D0182
	EXPORT __voMPEG2D0183
	EXPORT __voMPEG2D0184
	EXPORT __voMPEG2D0185  ;huwei 20080219 downsmaple
	EXPORT __voMPEG2D0238  ;huwei 20080616 Bframe_field
	EXPORT __voMPEG2D0241  ;huwei 20080617 Bframe_field
	EXPORT __voMPEG2D0257  ;huwei 20090904 IDCT4x4

	ALIGN 16
	macro	
	arm_transc8x8_t $Name, $ARMv6
$Name PROC	
	if $ARMv6>0
        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14}
		; r0 = v, r1 = Dst, r2 = DstPitch, r3 = Src

	;const VO_U8* SrcEnd = Src + 8*SrcPitch (int SrcPitch = 8;)
		ldr		r14,[sp,#36]	;SrcPitch
;        add     r6, r3, #64
	;if (vv==0)
        cmp     r0, #0                           
        bne     $Name.outCopyBlock8x8_asm_ARMv6     
		             
$Name.CopyBlock8x8_asm_ARMv6
 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14 
 		ldrd     r8, [r3], r14        
 		ldrd     r10, [r3], r14	
        strd     r4, [r1], r2 
        strd     r6, [r1], r2 
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14 
 		ldrd     r8, [r3], r14        
 		ldrd     r10, [r3], r14	
        strd     r4, [r1], r2 
        strd     r6, [r1], r2 
        strd     r8, [r1], r2 
        strd     r10, [r1], r2
		    		  				                         

	;retrun end arm_transc8x8              
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15}

$Name.outCopyBlock8x8_asm_ARMv6

		blt     $Name.little_begin_ARMv6 

$Name.big_begin_ARMv6_ARMv6	

        mov     r12, r0                                         
        orr     r12, r12, r12, lsl #8            
        orr     r0, r12, r12, lsl #16    
		        
 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqadd8	r8, r4, r0
		uqadd8	r9, r5, r0
		uqadd8	r10, r6, r0
		uqadd8	r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqadd8	r8, r4, r0
		uqadd8	r9, r5, r0
		uqadd8	r10, r6, r0
		uqadd8	r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqadd8	r8, r4, r0
		uqadd8	r9, r5, r0
		uqadd8	r10, r6, r0
		uqadd8	r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqadd8	r8, r4, r0
		uqadd8	r9, r5, r0
		uqadd8	r10, r6, r0
		uqadd8	r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 													                                             

	;retrun end arm_transc8x8                   
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} 


		
$Name.little_begin_ARMv6
                      
        rsb   r12, r0, #0                       
        orr     r12, r12, r12, lsl #8            
        orr     r0, r12, r12, lsl #16            


 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqsub8	r8, r4, r0
		uqsub8	r9, r5, r0
		uqsub8	r10, r6, r0
		uqsub8	r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqsub8	r8, r4, r0
		uqsub8	r9, r5, r0
		uqsub8	r10, r6, r0
		uqsub8	r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqsub8	r8, r4, r0
		uqsub8	r9, r5, r0
		uqsub8	r10, r6, r0
		uqsub8	r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqsub8	r8, r4, r0
		uqsub8	r9, r5, r0
		uqsub8	r10, r6, r0
		uqsub8	r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

	;retrun end arm_transc8x8                   
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} 

	else

        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14}
		; r0 = v, r1 = Dst, r2 = DstPitch, r3 = Src

	;const VO_U8* SrcEnd = Src + 8*SrcPitch (int SrcPitch = 8;)
		ldr		r12,[sp,#36]	;SrcPitch
;        add     r6, r3, #64
	;if (vv==0)
        cmp     r0, #0
		sub     r12, r12, #4
		sub		r2, r2, #4                        
        bne     $Name.outCopyBlock8x8_asm     
		             
$Name.CopyBlock8x8_asm   

        ldr     r0, [r3], #4   
        ldr     r4, [r3], r12                      
        ldr     r5, [r3], #4                      
        ldr     r6, [r3], r12                    
        ldr     r7, [r3], #4    
        ldr     r8, [r3], r12                    
        ldr     r9, [r3], #4   
        ldr     r14, [r3], r12                    
        str     r0, [r1], #4   
        str     r4, [r1], r2
        str     r5, [r1], #4 
        str     r6, [r1], r2
        str     r7, [r1], #4    
        str     r8, [r1], r2 
        str     r9, [r1], #4 
        str     r14, [r1], r2


        ldr     r0, [r3], #4   
        ldr     r4, [r3], r12                      
        ldr     r5, [r3], #4                      
        ldr     r6, [r3], r12                    
        ldr     r7, [r3], #4    
        ldr     r8, [r3], r12                    
        ldr     r9, [r3], #4   
        ldr     r14, [r3], r12                    
        str     r0, [r1], #4   
        str     r4, [r1], r2
        str     r5, [r1], #4 
        str     r6, [r1], r2
        str     r7, [r1], #4    
        str     r8, [r1], r2 
        str     r9, [r1], #4 
        str     r14, [r1], r2		  		    		  				                         

	;retrun end arm_transc8x8              
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15}

$Name.outCopyBlock8x8_asm

		mov		r6, r12    
		mov		r8, #8  
        ldr     r10, MaskCarry
		str		r8,[sp,#36]	;count		  
		blt     $Name.little_begin 

$Name.big_begin	

        mov     r12, r0                                         

	;v |= v << 8;
        orr     r12, r12, r12, lsl #8            

	;v |= v << 16;
        orr     r5, r12, r12, lsl #16    
		        
$Name.big_loop_do

	;a = ((VO_U32*)Src)[0];                
        ldr     r8, [r3], #4  
	;d = ((VO_U32*)Src)[1];
        ldr     r4, [r3], r6                     
                       
	;b = a + v;
        add     r14, r5, r8                      

	;c = a & v;
        and     r7, r5, r8                        

	;a ^= v;	
        eor     r9, r8, r5                        

	;a &= ~b;
        mvn     r8, r14                           

        and     r8, r9, r8                        

	;a |= c;	
        orr     r7, r8, r7                        

	;a &= MaskCarry;
        and     r12, r7, r10                      

	;c = a << 1;	b -= c;	
        sub     r14, r14, r12, lsl #1             

	;b |= c - (a >> 7);
        mov     r7, r12, lsr #7                   
        rsb     r12, r7, r12, lsl #1              

        orr     r11, r14, r12                                                                  

	;b = d + v;
        add     r12, r5, r4                       

	;c = d & v;	
        and     r7, r5, r4                        

	;d ^= v;	
        eor     r8, r4, r5                        

	;d &= ~b;
        mvn     r4, r12                           

        and     r4, r8, r4                        

	;d |= c;
        orr     r4, r7, r4                        

	;d &= MaskCarry;
        and     r4, r4, r10                       

	;c = d << 1;	b -= c;	
        sub     r14, r12, r4, lsl #1              

	;b |= c - (d >> 7);
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1              

        orr     r12, r14, r12                     
		ldr		r8,[sp,#36]	;count    		                        
	;((VO_U32*)Dst)[0] = b;
        str     r11, [r1], #4
	;((VO_U32*)Dst)[1] = b;
        str     r12, [r1], r2                     

	;Dst += DstPitch;
;        add     r1, r1, r2                        

	;Src += SrcPitch;
;        add     r3, r3, #8                        

	;while (Src != SrcEnd);	
;        cmp     r3, r6

		subs	r8, r8, #1     
		str		r8,[sp,#36]	;count                 
        bne     $Name.big_loop_do                             

	;retrun end arm_transc8x8                   
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} 


		
$Name.little_begin
                      
        rsb   r12, r0, #0                       

	;v |= v << 8;
        orr     r12, r12, r12, lsl #8            

	;v |= v << 16;
        orr     r5, r12, r12, lsl #16            
$Name.little_loop_do

	;a = ((VO_U32*)Src)[0];                
        ldr     r8, [r3], #4  
	;d = ((VO_U32*)Src)[1];
        ldr     r4, [r3], r6                     

	;a = ~a;	
        mvn   r8, r8 
       	mvn   r4, r4                                      

	;b = a + v;
        add     r14, r5, r8                      

	;c = a & v;
        and     r7, r5, r8                        

	;a ^= v;	
        eor     r9, r8, r5                        

	;a &= ~b;
        mvn     r8, r14                           

        and     r8, r9, r8                        

	;a |= c;	
        orr     r7, r8, r7                        

	;a &= MaskCarry;
        and     r12, r7, r10                      

	;c = a << 1;	b -= c;	
        sub     r14, r14, r12, lsl #1             

	;b |= c - (a >> 7);
        mov     r7, r12, lsr #7                   
        rsb     r12, r7, r12, lsl #1              

        orr     r11, r14, r12                                                                  

	;b = d + v;
        add     r12, r5, r4                       

	;c = d & v;	
        and     r7, r5, r4                        

	;d ^= v;	
        eor     r8, r4, r5                        

	;d &= ~b;
        mvn     r4, r12                           

        and     r4, r8, r4                        

	;d |= c;
        orr     r4, r7, r4                        

	;d &= MaskCarry;
        and     r4, r4, r10                       

	;c = d << 1;	b -= c;	
        sub     r14, r12, r4, lsl #1              

	;b |= c - (d >> 7);
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1              

        orr     r12, r14, r12                     
                      
	;b = ~b;	
        mvn   r11, r11  
	;b = ~b;
        mvn   r12, r12   
		ldr		r8,[sp,#36]	;count 		      		                        
	;((VO_U32*)Dst)[0] = b;
        str     r11, [r1], #4
	;((VO_U32*)Dst)[1] = b;
        str     r12, [r1], r2                     

	;Dst += DstPitch;
;        add     r1, r1, r2                        

	;Src += SrcPitch;
;        add     r3, r3, #8                        

	;while (Src != SrcEnd);	
;        cmp     r3, r6 

		subs	r8, r8, #1     
		str		r8,[sp,#36]	;count                                
        bne     $Name.little_loop_do                             

	;retrun end arm_transc8x8                   
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} 
	endif
	mend

; r6 Block
; r7,r8 must be saved
	ALIGN 16
	macro
	RowConst_t $Name,$ARMv6
$Name PROC
	add     r0, r0, #0x20  ; 0x20 = 32
	cmp     r7, #0
	mov     r3, r0, asr #6
	beq     $Name.RowConst_NoSrc
	cmp     r3, #0
	beq	$Name.RowConst_Zero
	blt     $Name.RowConst_Sub

$Name.RowConst_Add
	if $ARMv6>0
	ldr     r2, [r7]
	orr     r3, r3, r3, lsl #8
	ldr     r4, [r7, #4]
	orr     r3, r3, r3, lsl #16
	uqadd8	r11, r2, r3
	uqadd8	r5, r4, r3
	str     r11, [r8]
	ldr	r2, [sp, #48]	;SrcStride
	str     r5, [r8, #4]
;	add	r7, r7, #8	;source stride
	add	r7, r7, r2	;source stride
	mov	pc,lr
	else
	ldr     r0, CarryMask
	ldr     r2, [r7]
	orr     r3, r3, r3, lsl #8
	orr     r3, r3, r3, lsl #16
	add     r4, r2, r3
	eor     r11, r2, r3
	and     r2, r3, r2
	bic     r11, r11, r4
	orr     r11, r11, r2
	and     r5, r11, r0
	mov     r12, r5, lsl #1
	sub     r10, r4, r12
	sub     r11, r12, r5, lsr #7
	ldr     r2, [r7, #4]
	orr     r11, r11, r10
	str     r11, [r8]
	add     r4, r2, r3
	eor     r11, r2, r3
	and     r2, r3, r2
	bic     r11, r11, r4
	orr     r11, r11, r2
	and     r5, r11, r0
	mov     r12, r5, lsl #1
	sub     r10, r4, r12
	sub     r11, r12, r5, lsr #7
	orr     r11, r11, r10
	ldr	r2, [sp, #48]	;SrcStride
	str     r11, [r8, #4]
;	add	r7, r7, #8			;source stride
	add	r7, r7, r2			;source stride
	mov	pc,lr	
	endif
$Name.RowConst_Sub
	if $ARMv6>0
	ldr     r2, [r7]
	rsb     r3, r3, #0
	orr     r3, r3, r3, lsl #8
	ldr     r4, [r7, #4]
	orr     r3, r3, r3, lsl #16
	uqsub8	r11, r2, r3
	uqsub8	r5, r4, r3
	str     r11, [r8]
	ldr	r2, [sp, #48]	;SrcStride
	str     r5, [r8, #4]
;	add	r7, r7, #8			;source stride
	add	r7, r7, r2			;source stride
	mov	pc,lr
	else
	ldr     r0, CarryMask
	ldr     r2, [r7]
	rsb     r3, r3, #0
	orr     r3, r3, r3, lsl #8
	orr     r3, r3, r3, lsl #16
	mvn	r2, r2
	add     r4, r2, r3
	eor     r11, r2, r3
	and     r2, r3, r2
	bic     r11, r11, r4
	orr     r11, r11, r2
	and     r5, r11, r0
	mov     r12, r5, lsl #1
	sub     r10, r4, r12
	sub     r11, r12, r5, lsr #7
	ldr     r2, [r7, #4]
	orr     r11, r11, r10
	mvn	r11, r11
	str     r11, [r8]
	mvn	r2, r2
	add     r4, r2, r3
	eor     r11, r2, r3
	and     r2, r3, r2
	bic     r11, r11, r4
	orr     r11, r11, r2
	and     r5, r11, r0
	mov     r12, r5, lsl #1
	sub     r10, r4, r12
	sub     r11, r12, r5, lsr #7
	orr     r11, r11, r10
	mvn	r11, r11
	ldr	r2, [sp, #48]	;SrcStride
	str     r11, [r8, #4]
;	add	r7, r7, #8			;source stride
	add	r7, r7, r2			;source stride
	mov	pc,lr	
	endif	
$Name.RowConst_Zero
	ldr     r1, [r7]
	ldr     r2, [r7, #4]
	str     r1, [r8]
	str     r2, [r8, #4]
;	add		r7, r7, #8			;source stride
	ldr		r2, [sp, #48]	;SrcStride
	add		r7, r7, r2			;source stride

	mov		pc,lr

$Name.RowConst_NoSrc
	cmp     r3, #0
	movmi   r3, #0
	cmppl   r3, #255
	movgt   r3, #255
	orr     r3, r3, r3, lsl #8
	orr     r3, r3, r3, lsl #16
	str     r3, [r8]
	str     r3, [r8, #4]
	mov		pc,lr

	mend

	macro
	MCol8 $Name,$Rotate,$Pitch

$Name PROC

; r10 = x0
; r4  = x1
; r2  = x2
; r1  = x3
; r3  = x4
; r12 = x5
; r0  = x6
; r5  = x7
; r11 = x8  
; r9  = tmp (x567)

	ldrsh     r4, [r6, #4*$Pitch]
	ldrsh     r0, [r6, #5*$Pitch]
	ldrsh     r12,[r6, #7*$Pitch]
	ldrsh     r5, [r6, #3*$Pitch]
	ldrsh     r2, [r6, #6*$Pitch]
	ldrsh     r1, [r6, #2*$Pitch]
	ldrsh     r3, [r6, #1*$Pitch]
	ldrsh     r10,[r6]
	if $Rotate>0 
	add		  r6,r6,r9
	endif

	orr       r9, r12, r0
	orr       r9, r9, r5
	orr       r11, r9, r2
	orr       r11, r11, r4
	orrs      r11, r11, r1

	bne       $Name.LABMB 
 	cmp       r3, #0
	bne       $Name.LABMA
	if $Rotate=0
	cmp       r10, #0
	beq       $Name.LABZ
	endif
	mov       r10, r10, lsl #3
	strh      r10, [r6]
	strh      r10, [r6, #0x10]
	strh      r10, [r6, #0x20]
	strh      r10, [r6, #0x30]
	strh      r10, [r6, #0x40]
	strh      r10, [r6, #0x50]
	strh      r10, [r6, #0x60]
	strh      r10, [r6, #0x70]
$Name.LABZ
	mov		pc,lr

$Name.LABMA							;x0,x4
	mov       r11, r3
	mov       r2, #0x8D, 30  ; 0x234 = 564
	orr       r2, r2, #1
	mov       r9, r3
	mul       r2, r11, r2
	mov       r11, #0xB1, 28  ; 0xB10 = 2832
	orr       r11, r11, #9
	mul       r4, r9, r11
	mov       r11, #0x96, 28  ; 0x960 = 2400
	orr       r11, r11, #8
	mul       r5, r9, r11
	mov       r11, #0x19, 26  ; 0x640 = 1600
	mov       r1, r10, lsl #11
	orr       r11, r11, #9
	mul       r0, r3, r11
	add       r1, r1, #0x80  ; 0x80 = 128

	add       r3, r4, r1
	add       r11, r5, r1
	mov       r3, r3, asr #8
	mov       r11, r11, asr #8
	strh      r3, [r6]
	strh      r11, [r6, #0x10]  ; 0x10 = 16

	add       r3, r0, r1
	add       r11, r2, r1
	mov       r3, r3, asr #8
	mov       r11, r11, asr #8
	strh      r3, [r6, #0x20]  ; 0x20 = 32
	strh      r11, [r6, #0x30]  ; 0x30 = 48

	sub       r3, r1, r2
	sub       r11, r1, r0
	mov       r3, r3, asr #8
	mov       r11, r11, asr #8
	strh      r3, [r6, #0x40]  ; 0x40 = 64
	strh      r11, [r6, #0x50]  ; 0x50 = 80

	sub       r3, r1, r5
	sub       r11, r1, r4
	mov       r3, r3, asr #8
	mov       r11, r11, asr #8
	strh      r3, [r6, #0x60]  ; 0x60 = 96
	strh      r11, [r6, #0x70]  ; 0x70 = 112
	mov		pc,lr

$Name.LABMB						;x0,x1,x2,x3
	orrs      r11, r9, r3
	bne       $Name.LABMC
	mov       r3, r10, lsl #11
	add       r3, r3, #128
	mov       r9, #0x45, 28  ; 0x450 = 1104
	add       r5, r3, r4, lsl #11
	add       r11, r2, r1
	orr       r9, r9, #4
	sub       r3, r3, r4, lsl #11
	mul       r4, r11, r9
	mov       r11, #0x3B, 26  ; 0xEC0 = 3776
	orr       r11, r11, #8
	mul       r11, r2, r11
	sub       r2, r4, r11
	mov       r11, #0x62, 28  ; 0x620 = 1568
	mul       r11, r1, r11
	add       r0, r2, r3
	add       r1, r11, r4
	add       r4, r5, r1
	sub       r3, r3, r2
	sub       r5, r5, r1
	mov       r1, r4, asr #8
	mov       r3, r3, asr #8
	mov       r2, r0, asr #8
	mov       r4, r5, asr #8
	strh      r1, [r6,#0x00]
	strh      r2, [r6,#0x10]
	strh      r3, [r6,#0x20]
	strh      r4, [r6,#0x30]
	strh      r4, [r6,#0x40] 
	strh      r3, [r6,#0x50] 
	strh      r2, [r6,#0x60] 
	strh      r1, [r6,#0x70] 
	mov		pc,lr

$Name.LABMC						;x0,x1,x2,x3,x4,x5,x6,x7

	mov     r9, #0x8D, 30  
	orr     r9, r9, #1			;W7
	add     r11, r12, r3
	mul     r11, r9, r11		;x8 = W7 * (x5 + x4)

	mov     r9, #0x8E, 28  
	orr     r9, r9, #4			;W1-W7
	mla     r3, r9, r3, r11		;x4 = x8 + (W1-W7) * x4

	mvn     r9, #0xD40
	eor     r9, r9, #0xD		;-W1-W7
	mla     r12, r9, r12, r11	;x5 = x8 + (-W1-W7) * x5

	mov     r9, #0x96, 28		;
	orr     r9, r9, #8			;W3
	add     r11, r0, r5
	mul     r11, r9, r11		;x8 = W3 * (x6 + x7)
								
	mvn     r9, #0x310
	eor     r9, r9, #0xE		;W5-W3
	mla     r0, r9, r0, r11		;x6 = x8 + (W5-W3) * x6

	mvn     r9, #0xFB0			;-W3-W5
	mla     r5, r9, r5, r11		;x7 = x8 + (-W3-W5) * x7

	mov     r10, r10, lsl #11
	add     r10, r10, #128		;x0 = (x0 << 11) + 128
	add		r11, r10,r4,lsl #11 ;x8 = x0 + (x1 << 11)
	sub		r10, r10,r4,lsl #11 ;x0 = x0 - (x1 << 11)

	mov     r9, #0x45, 28  
	orr     r9, r9, #4			;W6
	add		r4, r1, r2
	mul		r4, r9, r4			;x1 = W6 * (x3 + x2)

	mvn     r9, #0xEC0
	eor     r9, r9, #0x7		;-W2-W6
	mla     r2, r9, r2, r4		;x2 = x1 + (-W2-W6) * x2

	mov     r9, #0x620			;W2-W6
	mla     r1, r9, r1, r4		;x3 = x1 + (W2-W6) * x3

	add		r4, r3, r0			;x1 = x4 + x6
	sub		r3, r3, r0			;x4 -= x6
	add		r0, r12,r5			;x6 = x5 + x7
	sub		r12,r12,r5			;x5 -= x7
	add		r5, r11,r1			;x7 = x8 + x3
	sub		r11,r11,r1			;x8 -= x3
	add		r1, r10,r2			;x3 = x0 + x2
	sub		r10,r10,r2			;x0 -= x2

	add		r9, r3, r12			;x4 + x5
	sub		r3, r3, r12			;x4 - x5
	mov		r12, #181
	mul		r2, r9, r12			;181 * (x4 + x5)
	mul		r9, r3, r12			;181 * (x4 - x5)
	add		r2, r2, #128		;x2 = 181 * (x4 + x5) + 128
	add		r3, r9, #128		;x4 = 181 * (x4 - x5) + 128

	add		r9,r5,r4			
	sub		r5,r5,r4			
	mov		r9,r9,asr #8		;(x7 + x1) >> 8
	mov		r5,r5,asr #8		;(x7 - x1) >> 8
	strh	r9,[r6,#0x00]
	strh	r5,[r6,#0x70]

	add		r9,r1,r2,asr #8
	sub		r1,r1,r2,asr #8			
	mov		r9,r9,asr #8		;(x3 + x2) >> 8
	mov		r1,r1,asr #8		;(x3 - x2) >> 8
	strh	r9,[r6,#0x10]
	strh	r1,[r6,#0x60]

	add		r9,r10,r3,asr #8			
	sub		r10,r10,r3,asr #8			
	mov		r9,r9,asr #8		;(x0 + x4) >> 8
	mov		r10,r10,asr #8		;(x0 - x4) >> 8
	strh	r9,[r6,#0x20]
	strh	r10,[r6,#0x50]

	add		r9,r11,r0			
	sub		r11,r11,r0			
	mov		r9,r9,asr #8		;(x8 + x6) >> 8
	mov		r11,r11,asr #8		;(x8 - x6) >> 8
	strh	r9,[r6,#0x30]
	strh	r11,[r6,#0x40]

	mov		pc,lr
	mend

	MCol8 Col8,0,16
	MCol8 Col8Swap,1,2

; r0 Block[0]
; r6 Block
; r7 Src
; r8 Dst
   ALIGN 8
CarryMaskNew	DCD 0x80808080
WxW1		dcd 0x0B19
WxW2		dcd 0x0A74
WxW3		dcd 0x0968
WxW4		dcd 0x00B5
WxW5		dcd 0x0649
WxW6		dcd 0x0454
WxW7		dcd 0x0235	
		
WxC1xC7		dcd 0x0B190235			;xC7 = D0.S16[0] xC1 = D0.S16[1]		
WxC3xC5		dcd 0x09680649			;xC5 = D0.S16[2] xC3 = D0.S16[3]			
WxC2xC6		dcd 0x0A740454			;xC6 = D1.S16[0] xC2 = D1.S16[1]		
WxC4xC4		dcd 0x00B500B5			;xC4 = D1.S32[1]/D1.S32[2]
Wx80808080	dcd 0x80808080			;
Wx04040000	dcd 0x04040000 


WxC5xC7		dcd 0x06490235			;xC7 = D0.S16[0] xC5 = D0.S16[1]		
WxC3xC1		dcd 0x09680B19			;xC1 = D0.S16[2] xC3 = D0.S16[3]		
Wx00xC4		dcd 0x000000B5			;xC6 = D1.S32[1]/D1.S32[2]

	ALIGN 4
Col8_4x4 PROC
	ldrsh	r1, [r0]
	ldrsh	r2, [r0, #16]
	ldrsh	r3, [r0, #32]
	ldrsh	r4, [r0, #48]
	
	orrs	r7, r3, r4		; r7 = ip2|ip3
	bne	allx1_4_t_lab		; (ip2|ip3)!=0	
	
 	cmp	r2, #0				
	bne	only_x1x2t_lab_4x4		; x2!=0
	
	cmp	r1, #0
	bne	only_x1t_lab_4x4		; x1!=0
	mov	pc, lr
	
only_x1t_lab_4x4
;//ip0
;	E = (idct_t)(Blk[0*8]<< 3);
;	ip[0~7*8] = E;    					    																																										    ip[7*8] =
    	mov	r5, r1, lsl #3
	strh	r5, [r0]
	strh	r5, [r0, #16]
	strh	r5, [r0, #32]
	strh	r5, [r0, #48]
	strh	r5, [r0, #64]
	strh	r5, [r0, #80]
	strh	r5, [r0, #96]
	strh	r5, [r0, #112]
	mov	pc, lr
only_x1x2t_lab_4x4
;//ip0, 1
;	A = W1 * Blk[1*8];
;	B = W7 * Blk[1*8];
;	E = Blk[0*8] << 11;
;	E += 128;	
;	Add = 181 * ((A - B)>> 8);		
;	Bdd = 181 * ((A + B)>> 8);
;	Blk[0*8] = (idct_t)((E + A) >> 8);
;	Blk[7*8] = (idct_t)((E - A) >> 8);		
;	Blk[1*8] = (idct_t)((E + Bdd) >> 8);
;	Blk[6*8] = (idct_t)((E - Bdd) >> 8);
;	Blk[3*8] = (idct_t)((E + B) >> 8);
;	Blk[4*8] = (idct_t)((E - B) >> 8);				
;	Blk[2*8] = (idct_t)((E + Add) >> 8);
;	Blk[5*8] = (idct_t)((E - Add) >> 8);

	ldr	r10, WxW1
	ldr	r11, WxW7
	mov	r12, #0x00b5	;WxW4	
	mul	r5, r2, r10	;A = M(xC1S7, ip[1*8]);
	mul	r6, r2, r11	;B = M(xC7S1, ip[1*8]);	
	mov	r7, r1, lsl #11	;E = Blk[0*8] << 11
	add	r7, r7, #128	;E += 128;
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	;(A - B)>> 8
	mov	r4, r4, asr #8	;(A + B)>> 8
	mul	r3, r12, r3	;Add = 181 * ((A - B)>> 8);
	mul	r4, r12, r4	;Bdd = 181 * ((A + B)>> 8);
	
	add	r1, r7, r5	;E + A
	sub	r2, r7, r5	;E - A
	add	r8, r7, r4	;E + Bdd
	sub	r4, r7, r4	;E - Bdd	
	add	r5, r7, r6	;E + B
	sub	r6, r7, r6	;E - B	
	add	r9, r7, r3	;E + Add
	sub	r3, r7, r3	;E - Add
	
	mov	r1, r1, asr #8	;
	mov	r2, r2, asr #8	;
	mov	r8, r8, asr #8	;
	mov	r4, r4, asr #8	;
	mov	r5, r5, asr #8	;
	mov	r6, r6, asr #8	;
	mov	r9, r9, asr #8	;
	mov	r3, r3, asr #8	;
	
	strh	r1, [r0]
	strh	r8, [r0, #16]
	strh	r9, [r0, #32]
	strh	r5, [r0, #48]
	strh	r6, [r0, #64]
	strh	r3, [r0, #80]
	strh	r4, [r0, #96]
	strh	r2, [r0, #112]	
	mov	pc, lr
	
allx1_4_t_lab            
;//ip0,1,2,3                                             
;	Cd = A + C;	W1 * Blk[1*8] + W3 * Blk[3*8]
;	Dd = B + D;	W7 * Blk[1*8] - W5 * Blk[3*8]
;	Ad = A - C;	W1 * Blk[1*8] - W3 * Blk[3*8]
;	Bd = B - D;	W7 * Blk[1*8] + W5 * Blk[3*8]
;	Add = (181 * (((Ad - Bd)) >> 8));		
;	Bdd = (181 * (((Ad + Bd)) >> 8));				
;	E = Blk[0*8] << 11;
;	E += 128;		
;	G = W2 * Blk[2*8];		
;	H = W6 * Blk[2*8]; 
;	Ed = E - G;
;	Fd = E + G;
;	Gd = E - H;		
;	Hd = E + H;
;	Blk[0*8] = (idct_t)((Fd + Cd) >> 8);
;	Blk[7*8] = (idct_t)((Fd - Cd) >> 8);		
;	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8);
;	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8);
;	Blk[3*8] = (idct_t)((Ed + Dd) >> 8);
;	Blk[4*8] = (idct_t)((Ed - Dd) >> 8);				
;	Blk[2*8] = (idct_t)((Gd + Add) >> 8);
;	Blk[5*8] = (idct_t)((Gd - Add) >> 8);

; r9, r10, r11, r12 free now
	ldr	r9, WxW1
	ldr	r10, WxW7

	mul     r12, r9,  r2		;A = M(xC1S7, ip[1*8])
	mul     r2,  r10, r2		;B = M(xC7S1, ip[1*8])	
	
; r9, r10, r11, r8 free now		
	ldr	r9, WxW3
	ldr	r10, WxW5

	mul     r8, r9,  r4		;C = M(xC3S5, ip[3*8])
	mul     r4,  r10,  r4		;(-D) = M(xC5S3, ip[3*8])

; r9, r10, r11, r6 free now
	mov	r10, #0x00b5	;WxW4
	sub	r11, r12, r8	;Ad = A - C
	add	r6, r2, r4	;Bd = B - D;	B + (-D)
	sub	r9, r11, r6
	add	r11, r11, r6
	mov	r6, r9, asr #8	;(Ad - Bd)) >> 8
	mov	r11, r11, asr #8	;(Ad + Bd)) >> 8
	mul	r6, r10, r6	;Add = (181 * (((Ad - Bd)) >> 8));
	mul	r11, r10, r11	;Bdd = (181 * (((Ad + Bd)) >> 8));	
		
	add	r8, r12, r8	;Cd = A + C
	sub	r4, r2, r4	;Dd = B + D;	B - (-D)
; r9, r10, r2, r12 free now	Add = r6, Bdd = r11, Cd = r8, Dd = r4		
	ldr	r9, WxW2
	ldr	r10, WxW6

	mul     r12, r9,  r3		;G = M(xC2S6, ip[2*8])
	mul     r2,  r10, r3		;H = M(xC6S2, ip[2*8])		
; r9, r10, r3, r7 free now	G = r12, H = r2
	mov	r1, r1, asl #11
	add	r3, r1, #128
				
; r9, r10 free now		E = r3, F = r3
	sub	r9, r3, r12	;Ed = E - G;
	add	r10, r3, r12	;Fd = E + G;
	add	r12, r3, r2	;Hd = F + H;	
	sub	r3, r3, r2	;Gd = F - H;
	
; r7, r2  free now     		;Ed = r9, Fd = r10, Gd = r3, Hd = r12
				;Add = r6, Bdd = r11, Cd = r8, Dd = r4
				
	add	r2, r10, r8	;Fd + Cd
	sub	r7, r10, r8	;Fd - Cd
	add	r8, r12, r11	;Hd + Bdd
	sub	r10, r12, r11	;Hd - Bdd	
	add	r11, r9, r4	;Ed + Dd
	sub	r12, r9, r4	;Ed - Dd	
	add	r4, r3, r6	;Gd + Add
	sub	r9, r3, r6	;Gd - Add

; r3, r6  free now
		
	mov	r2, r2, asr #8		;
	mov	r7, r7, asr #8		;
	mov	r8, r8, asr #8		;
	mov	r10, r10, asr #8	;
	mov	r11, r11, asr #8	;
	mov	r12, r12, asr #8	;
	mov	r4, r4, asr #8		;
	mov	r9, r9, asr #8		;

	strh	r2, [r0]
	strh	r8, [r0, #16]
	strh	r4, [r0, #32]
	strh	r11, [r0, #48]
	strh	r12, [r0, #64]
	strh	r9, [r0, #80]
	strh	r10, [r0, #96]
	strh	r7, [r0, #112]	
	mov		pc,lr
	ENDP
	
	ALIGN 4
Row8src_4x4 PROC
;	ldrd		r2, [r0]	; r2 = x1|x0 r3 = x3|x2
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
	cmp	r3, #0
	bne	allx1_4_t_lab_Rowsrc	; (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		; r3 = x1	

	bne	only_x1x2t_lab_Rowsrc_4x4	; x2!=0
	ldr	r6, [sp, #4]		; Src = [sp, #4]
	ldr	r8, [sp, #52]		; src_stride = [sp, #52]
	ldr	r7, [sp, #12]		; dst = [sp, #12]
	ldr	r4, [sp, #8]		; dst_stride = [sp, #8]
	add	r8, r6, r8
	add	r4, r7, r4	
	str	r8, [sp, #4]		; Src = [sp, #4]
	str	r4, [sp, #12]		; dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rowsrc_4x4	; x1!=0
;if(src)
;{
;	if(dst !=src )
;	{
;		dst[0] = src[0];
;		dst[1] = src[1];
;		dst[2] = src[2];
;		dst[3] = src[3];
;		dst[4] = src[4];
;		dst[5] = src[5];
;		dst[6] = src[6];
;		dst[7] = src[7];
;	}
;	src += stride;
;}	
	cmp	r6, r7	
	moveq	pc, lr
	ldr	r2, [r6]
	ldr	r3, [r6, #4]								 		
;	strd		r2, [r7]
	str	r2, [r7]
	str	r3, [r7, #4]		
	mov	pc, lr

only_x1t_lab_Rowsrc_4x4
;if(ip[0])	//ip0		; r2 = x1|x0
;{
;	E = (Blk[0] + 32)>>6;
;        dst[0] = SAT(((src[0] + E)));
;        dst[7] = SAT(((src[7] + E)));
;
;        dst[1] = SAT(((src[1] + E)));
;        dst[2] = SAT(((src[2] + E)));
;
;        dst[3] = SAT(((src[3] + E)));
;        dst[4] = SAT(((src[4] + E)));
;
;        dst[5] = SAT(((src[5] + E)));
;        dst[6] = SAT(((src[6] + E)));
;	src += stride;
;    }
;}
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	; r2 = x0
	add	r1, r2, #32

	ldr	r8, [r6]
	ldr	r4, [r6, #4]
	movs	r1, r1, asr #6	
	streq	r8, [r7]
	streq	r4, [r7, #4]	;strdeq	r2, [r7]		
	moveq	pc, lr
						
    ldr     r10, CarryMaskNew 
	blt     armv4_little_begin_4x4 
armv4_big_begin_4x4	
                                      
	;v |= v << 8;
        orr     r12, r1, r1, lsl #8            
	;v |= v << 16;
        orr     r5, r12, r12, lsl #16    	        

	;a = ((VO_U32*)Src)[0];                
;        ldr     r8, [r3], #4  
	;d = ((VO_U32*)Src)[1];
;        ldr     r4, [r3], r6                                       
	;b = a + v;
        add     r3, r5, r8
	;c = a & v;
        and     r1, r5, r8
	;a ^= v;	
        eor     r9, r8, r5
	;a &= ~b;
        mvn     r8, r3
        and     r8, r9, r8
	;a |= c;	
        orr     r1, r8, r1
	;a &= MaskCarry;
        and     r12, r1, r10
	;c = a << 1;	b -= c;	
        sub     r3, r3, r12, lsl #1
	;b |= c - (a >> 7);
        mov     r1, r12, lsr #7                   
        rsb     r12, r1, r12, lsl #1
        orr     r11, r3, r12
	;b = d + v;
        add     r12, r5, r4
	;c = d & v;	
        and     r1, r5, r4
	;d ^= v;	
        eor     r8, r4, r5
	;d &= ~b;
        mvn     r4, r12
        and     r4, r8, r4
	;d |= c;
        orr     r4, r1, r4
	;d &= MaskCarry;
        and     r4, r4, r10
	;c = d << 1;	b -= c;	
        sub     r3, r12, r4, lsl #1
	;b |= c - (d >> 7);
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1
        orr     r12, r3, r12 		                        
	;((VO_U32*)Dst)[0] = b;
        str     r11, [r7]
	;((VO_U32*)Dst)[1] = b;
        str     r12, [r7, #4]
	mov	pc, lr      
		
armv4_little_begin_4x4
                      
        rsb   r12, r1, #0
	;v |= v << 8;
        orr     r12, r12, r12, lsl #8
	;v |= v << 16;
        orr     r5, r12, r12, lsl #16           

	;a = ((VO_U32*)Src)[0];                
;        ldr     r8, [r3], #4  
	;d = ((VO_U32*)Src)[1];
;        ldr     r4, [r3], r6                     
	;a = ~a;	
        mvn   r8, r8 
       	mvn   r4, r4                                      
	;b = a + v;
        add     r3, r5, r8                      
	;c = a & v;
        and     r1, r5, r8                        
	;a ^= v;	
        eor     r9, r8, r5                        
	;a &= ~b;
        mvn     r8, r3                           
        and     r8, r9, r8                        
	;a |= c;	
        orr     r1, r8, r1                        
	;a &= MaskCarry;
        and     r12, r1, r10                      
	;c = a << 1;	b -= c;	
        sub     r3, r3, r12, lsl #1             
	;b |= c - (a >> 7);
        mov     r1, r12, lsr #7                   
        rsb     r12, r1, r12, lsl #1              
        orr     r11, r3, r12
	;b = d + v;
        add     r12, r5, r4
	;c = d & v;	
        and     r1, r5, r4                        
	;d ^= v;	
        eor     r8, r4, r5                        
	;d &= ~b;
        mvn     r4, r12                           
        and     r4, r8, r4                        
	;d |= c;
        orr     r4, r1, r4                        
	;d &= MaskCarry;
        and     r4, r4, r10                       
	;c = d << 1;	b -= c;	
        sub     r3, r12, r4, lsl #1              
	;b |= c - (d >> 7);
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1              
        orr     r12, r3, r12                  
	;b = ~b;	
        mvn   r11, r11  
	;b = ~b;
        mvn   r12, r12	      		                        
	;((VO_U32*)Dst)[0] = b;
        str     r11, [r7]
	;((VO_U32*)Dst)[1] = b;
        str     r12, [r7, #4]
	mov	pc, lr
	
only_x1x2t_lab_Rowsrc_4x4
;//ip0,1		; r2 = x1|x0
;{
;	A = W1 * Blk[1*8];
;	B = W7 * Blk[1*8];
;;	E = Blk[0*8] << 11;	;E = (Blk[0] + 32) << 11
;;	E += 128;		
;	Add = 181 * ((A - B) >> 8);		
;	Bdd = 181 * ((A + B) >> 8);	
;	Dst[0] = SAT_new(((Src[0] + ((E + A )>>17))));
;	Dst[7] = SAT_new(((Src[7] + ((E - A )>>17))));	
;	Dst[1] = SAT_new(((Src[1] + ((E + Bdd )>>17))));
;	Dst[6] = SAT_new(((Src[6] + ((E - Bdd )>>17))));	
;	Dst[3] = SAT_new(((Src[3] + ((E + B )>>17))));
;	Dst[4] = SAT_new(((Src[4] + ((E - B )>>17))));	
;	Dst[2] = SAT_new(((Src[2] + ((E + Add )>>17))));
;	Dst[5] = SAT_new(((Src[5] + ((E - Add )>>17))));		 				
;}

; r2 = x1|x0
	mov	r1, r2, asr #16	; r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	; r2 = x0
	
	ldr	r10, WxW1
	ldr	r11, WxW7
	mov	r12, #0x00b5	;WxW4	
	mul	r5, r1, r10	;A = M(xC1S7, ip[1*8]);
	mul	r6, r1, r11	;B = M(xC7S1, ip[1*8]);
	add	r2, r2, #32	
	mov	r7, r2, lsl #11	;E = (Blk[0] + 32) << 11;
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	;(A - B)>> 8
	mov	r4, r4, asr #8	;(A + B)>> 8
	mul	r3, r12, r3	;Add = 181 * ((A - B)>> 8);
	mul	r4, r12, r4	;Bdd = 181 * ((A + B)>> 8);
	
	add	r1, r7, r5	;E + A
	sub	r2, r7, r5	;E - A
	add	r8, r7, r4	;E + Bdd
	sub	r4, r7, r4	;E - Bdd	
	add	r5, r7, r6	;E + B
	sub	r6, r7, r6	;E - B	
	add	r9, r7, r3	;E + Add
	sub	r3, r7, r3	;E - Add
;	r1, r8, r9, r5, r6, r3, r4, r2
;	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		; Src = [sp, #4]	
	ldr	r10, [sp, #52]		; src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		; Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #17
	add     r8, r11, r8, asr #17
	add     r9, r12, r9, asr #17
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #17
	add     r6, r11, r6, asr #17
	add     r3, r12, r3, asr #17
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #17
	add     r2, r11, r2, asr #17							

;	r1, r8, r9, r5, r6, r3, r4, r2							
;	r7, r10, r11, r12 free now	
	ldr	r7, [sp, #12]		; dst = [sp, #12]
	ldr	r10, [sp, #8]		; dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00							
;	r1, r8, r9, r5, r6, r3, r4, r2							
;	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		; dst = [sp, #12]
	mov	pc, lr


allx1_4_t_lab_Rowsrc
;//ip0,1,2,3                                             
;	Cd = A + C;	W1 * Blk[1*8] + W3 * Blk[3*8]
;	Dd = B + D;	W7 * Blk[1*8] - W5 * Blk[3*8]
;	Ad = A - C;	W1 * Blk[1*8] - W3 * Blk[3*8]
;	Bd = B - D;	W7 * Blk[1*8] + W5 * Blk[3*8]
;	Add = (181 * (((Ad - Bd)) >> 8));		
;	Bdd = (181 * (((Ad + Bd)) >> 8));				
;;	E = Blk[0*8] << 11;	;E = (Blk[0] + 32) << 11
;;	E += 128;		
;	G = W2 * Blk[2*8];		
;	H = W6 * Blk[2*8]; 
;	Ed = E - G;
;	Fd = E + G;
;	Gd = E - H;		
;	Hd = E + H;
;	Blk[0*8] = (idct_t)((Fd + Cd) >> 8);
;	Blk[7*8] = (idct_t)((Fd - Cd) >> 8);		
;	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8);
;	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8);
;	Blk[3*8] = (idct_t)((Ed + Dd) >> 8);
;	Blk[4*8] = (idct_t)((Ed - Dd) >> 8);				
;	Blk[2*8] = (idct_t)((Gd + Add) >> 8);
;	Blk[5*8] = (idct_t)((Gd - Add) >> 8);
; r2 = x1|x0 r3 = x3|x2
;;	r2 = x1|x0 r3 = x3|x2
;;	r4 = x5|x4 r5 = x7|x6
	;	ip0~ip7 = r1~8
	mov	r1, r2, lsl #16
	mov	r1, r1, asr #16
	mov	r2, r2, asr #16
	
	mov	r4, r3, asr #16	
	mov	r3, r3, lsl #16
	mov	r3, r3, asr #16					

; r9, r10, r11, r12 free now
	ldr	r9, WxW1
	ldr	r10, WxW7

	mul     r12, r9,  r2		;A = M(xC1S7, ip[1*8])
	mul     r2,  r10, r2		;B = M(xC7S1, ip[1*8])	
	
; r9, r10, r11, r8 free now		
	ldr	r9, WxW3
	ldr	r10, WxW5

	mul     r8, r9,  r4		;C = M(xC3S5, ip[3*8])
	mul     r4,  r10,  r4		;-D = M(xC5S3, ip[3*8])

; r9, r10, r11, r6 free now
	mov	r10, #0x00b5	;WxW4
	sub	r11, r12, r8	;Ad = A - C
	add	r6, r2, r4	;Bd = B - D;
	sub	r9, r11, r6
	add	r11, r11, r6
	mov	r6, r9, asr #8	;(Ad - Bd)) >> 8
	mov	r11, r11, asr #8	;(Ad + Bd)) >> 8
	mul	r6, r10, r6	;Add = (181 * (((Ad - Bd)) >> 8));
	mul	r11, r10, r11	;Bdd = (181 * (((Ad + Bd)) >> 8));	
		
	add	r8, r12, r8	;Cd = A + C
	sub	r4, r2, r4	;Dd = B + D;
; r9, r10, r2, r12 free now	Add = r6, Bdd = r11, Cd = r8, Dd = r4		
	ldr	r9, WxW2
	ldr	r10, WxW6

	mul     r12, r9,  r3		;G = M(xC2S6, ip[2*8])
	mul     r2,  r10, r3		;H = M(xC6S2, ip[2*8])	
; r9, r10, r3, r7 free now	G = r12, H = r2
	add	r3, r1, #32	;E = (Blk[0*8] + 32) << 11
	mov	r3, r3, asl #11
				
; r9, r10 free now		E = r3, F = r3
	sub	r9, r3, r12	;Ed = E - G;
	add	r10, r3, r12	;Fd = E + G;
	add	r12, r3, r2	;Hd = F + H;	
	sub	r3, r3, r2	;Gd = F - H;
	
; r7, r2  free now     		;Ed = r9, Fd = r10, Gd = r3, Hd = r12
				;Add = r6, Bdd = r11, Cd = r8, Dd = r4
				
	add	r1, r10, r8	;Fd + Cd
	sub	r2, r10, r8	;Fd - Cd
	add	r5, r9, r4	;Ed + Dd
	sub	r7, r9, r4	;Ed - Dd	r6	

	add	r8, r12, r11	;Hd + Bdd
	sub	r4, r12, r11	;Hd - Bdd	

	add	r9, r3, r6	;Gd + Add
	sub	r3, r3, r6	;Gd - Add
	mov	r6, r7
	
;	r1, r8, r9, r5, r6, r3, r4, r2
;	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		; Src = [sp, #4]	
	ldr	r10, [sp, #52]		; src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		; Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #17
	add     r8, r11, r8, asr #17
	add     r9, r12, r9, asr #17
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #17
	add     r6, r11, r6, asr #17
	add     r3, r12, r3, asr #17
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #17
	add     r2, r11, r2, asr #17							

;	r1, r8, r9, r5, r6, r3, r4, r2							
;	r7, r10, r11, r12 free now	
	ldr	r7, [sp, #12]		; dst = [sp, #12]
	ldr	r10, [sp, #8]		; dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00							
;	r1, r8, r9, r5, r6, r3, r4, r2							
;	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		; dst = [sp, #12]
	mov	pc, lr
	ENDP
	
		ALIGN 4
Row8nosrc_4x4 PROC
;	ldrd		r2, [r0]	; r2 = x1|x0 r3 = x3|x2
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
	cmp	r3, #0
	bne	allx1_4_t_lab_Rownosrc	; (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		; r3 = x1	

	bne	only_x1x2t_lab_Rownosrc_4x4	; x2!=0
	ldr	r7, [sp, #12]		; dst = [sp, #12]
	ldr	r4, [sp, #8]		; dst_stride = [sp, #8]
	add	r4, r7, r4	
	str	r4, [sp, #12]		; dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rownosrc_4x4	; x1!=0
;;{
;	Dst[0] =
;	Dst[7] =
;	Dst[1] =
;	Dst[6] =
;	Dst[3] =
;	Dst[4] =
;	Dst[2] =
;	Dst[5] = 0;
;;}	
	mov	r2, #0
;	strd		r2, [r7]
	str	r2, [r7]
	str	r2, [r7, #4]			
	mov	pc, lr
	
only_x1t_lab_Rownosrc_4x4
;if(ip[0])	//ip0		; r2 = x1|x0
;{
;	E = (Blk[0] + 32)>>6;
;;        dst[0] =
;;        dst[7] =
;;
;;        dst[1] =
;;        dst[2] =
;;
;;        dst[3] =
;;        dst[4] =
;;
;;        dst[5] =
;;        dst[6] = SAT_new(E);;
;}
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	; r2 = x0
	add	r1, r2, #32
	mov	r2, r1, asr #6
	orr     r2, r2, r2, lsl #8
	orr     r2, r2, r2, lsl #16
	str	r2, [r7]						
	str	r2, [r7, #4]						
	mov	pc, lr	
	
only_x1x2t_lab_Rownosrc_4x4
;//ip0,1		; r2 = x1|x0
;{
;	A = W1 * Blk[1*8];
;	B = W7 * Blk[1*8];
;;	E = Blk[0*8] << 11;	;E = (Blk[0] + 32) << 11
;;	E += 128;		
;	Add = 181 * ((A - B) >> 8);		
;	Bdd = 181 * ((A + B) >> 8);	
;	Dst[0] = SAT_new(((E + A )>>17));
;	Dst[7] = SAT_new(((E - A )>>17));	
;	Dst[1] = SAT_new(((E + Bdd )>>17));
;	Dst[6] = SAT_new(((E - Bdd )>>17));	
;	Dst[3] = SAT_new(((E + B )>>17));
;	Dst[4] = SAT_new(((E - B )>>17));	
;	Dst[2] = SAT_new(((E + Add )>>17));
;	Dst[5] = SAT_new(((E - Add )>>17));
;}
; r2 = x1|x0
	mov	r1, r2, asr #16	; r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	; r2 = x0
	
	ldr	r10, WxW1
	ldr	r11, WxW7
	mov	r12, #0x00b5	;WxW4	
	mul	r5, r1, r10	;A = M(xC1S7, ip[1*8]);
	mul	r6, r1, r11	;B = M(xC7S1, ip[1*8]);
	add	r2, r2, #32	
	mov	r7, r2, lsl #11	;E = (Blk[0] + 32) << 11;
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	;(A - B)>> 8
	mov	r4, r4, asr #8	;(A + B)>> 8
	mul	r3, r12, r3	;Add = 181 * ((A - B)>> 8);
	mul	r4, r12, r4	;Bdd = 181 * ((A + B)>> 8);
	
	add	r1, r7, r5	;E + A
	sub	r2, r7, r5	;E - A
	add	r8, r7, r4	;E + Bdd
	sub	r4, r7, r4	;E - Bdd	
	add	r5, r7, r6	;E + B
	sub	r6, r7, r6	;E - B	
	add	r9, r7, r3	;E + Add
	sub	r3, r7, r3	;E - Add
	
	mov     r1, r1, asr #17
	mov     r8, r8, asr #17
	mov     r9, r9, asr #17
	mov     r5, r5, asr #17
	mov     r6, r6, asr #17
	mov     r3, r3, asr #17
	mov     r4, r4, asr #17
	mov     r2, r2, asr #17

;	r1, r8, r9, r5, r6, r3, r4, r2							
;	r7, r10, r11, r12 free now	
	ldr	r7, [sp, #12]		; dst = [sp, #12]
	ldr	r10, [sp, #8]		; dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00								
;	r1, r8, r9, r5, r6, r3, r4, r2							
;	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		; dst = [sp, #12]
	mov	pc, lr
	
allx1_4_t_lab_Rownosrc
;//ip0,1,2,3                                             
;	Cd = A + C;	W1 * Blk[1*8] + W3 * Blk[3*8]
;	Dd = B + D;	W7 * Blk[1*8] - W5 * Blk[3*8]
;	Ad = A - C;	W1 * Blk[1*8] - W3 * Blk[3*8]
;	Bd = B - D;	W7 * Blk[1*8] + W5 * Blk[3*8]
;	Add = (181 * (((Ad - Bd)) >> 8));		
;	Bdd = (181 * (((Ad + Bd)) >> 8));				
;;	E = Blk[0*8] << 11;	E = (Blk[0] + 32) << 11;
;;	E += 128;		
;	G = W2 * Blk[2*8];		
;	H = W6 * Blk[2*8]; 
;	Ed = E - G;
;	Fd = E + G;
;	Gd = E - H;		
;	Hd = E + H;
;	Blk[0*8] = (idct_t)((Fd + Cd) >> 8);
;	Blk[7*8] = (idct_t)((Fd - Cd) >> 8);		
;	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8);
;	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8);
;	Blk[3*8] = (idct_t)((Ed + Dd) >> 8);
;	Blk[4*8] = (idct_t)((Ed - Dd) >> 8);				
;	Blk[2*8] = (idct_t)((Gd + Add) >> 8);
;	Blk[5*8] = (idct_t)((Gd - Add) >> 8);
; r2 = x1|x0 r3 = x3|x2
;;	r2 = x1|x0 r3 = x3|x2
;;	r4 = x5|x4 r5 = x7|x6
	;	ip0~ip7 = r1~8
	mov	r1, r2, lsl #16
	mov	r1, r1, asr #16
	mov	r2, r2, asr #16
	
	mov	r4, r3, asr #16	
	mov	r3, r3, lsl #16
	mov	r3, r3, asr #16					

; r9, r10, r11, r12 free now
	ldr	r9, WxW1
	ldr	r10, WxW7

	mul     r12, r9,  r2		;A = M(xC1S7, ip[1*8])
	mul     r2,  r10, r2		;B = M(xC7S1, ip[1*8])	
	
; r9, r10, r11, r8 free now		
	ldr	r9, WxW3
	ldr	r10, WxW5

	mul     r8, r9,  r4		;C = M(xC3S5, ip[3*8])
	mul     r4,  r10,  r4		;(-D) = M(xC5S3, ip[3*8])

; r9, r10, r11, r6 free now
	mov	r10, #0x00b5	;WxW4
	sub	r11, r12, r8	;Ad = A - C
	add	r6, r2, r4	;Bd = B - D;
	sub	r9, r11, r6
	add	r11, r11, r6
	mov	r6, r9, asr #8	;(Ad - Bd)) >> 8
	mov	r11, r11, asr #8	;(Ad + Bd)) >> 8
	mul	r6, r10, r6	;Add = (181 * (((Ad - Bd)) >> 8));
	mul	r11, r10, r11	;Bdd = (181 * (((Ad + Bd)) >> 8));	
		
	add	r8, r12, r8	;Cd = A + C
	sub	r4, r2, r4	;Dd = B + D;
; r9, r10, r2, r12 free now	Add = r6, Bdd = r11, Cd = r8, Dd = r4		
	ldr	r9, WxW2
	ldr	r10, WxW6

	mul     r12, r9,  r3		;G = M(xC2S6, ip[2*8])
	mul     r2,  r10, r3		;H = M(xC6S2, ip[2*8])	
; r9, r10, r3, r7 free now	G = r12, H = r2
	add	r3, r1, #32	;E = (Blk[0*8] + 32) << 11
	mov	r3, r3, asl #11
				
; r9, r10 free now		E = r3, F = r3
	sub	r9, r3, r12	;Ed = E - G;
	add	r10, r3, r12	;Fd = E + G;
	add	r12, r3, r2	;Hd = F + H;	
	sub	r3, r3, r2	;Gd = F - H;
	
; r7, r2  free now     		;Ed = r9, Fd = r10, Gd = r3, Hd = r12
				;Add = r6, Bdd = r11, Cd = r8, Dd = r4
				
	add	r1, r10, r8	;Fd + Cd
	sub	r2, r10, r8	;Fd - Cd
	add	r5, r9, r4	;Ed + Dd
	sub	r7, r9, r4	;Ed - Dd	r6
		
	add	r8, r12, r11	;Hd + Bdd
	sub	r4, r12, r11	;Hd - Bdd
		
	add	r9, r3, r6	;Gd + Add
	sub	r3, r3, r6	;Gd - Add
	mov	r6, r7
	
	mov     r1, r1, asr #17
	mov     r8, r8, asr #17
	mov     r9, r9, asr #17
	mov     r5, r5, asr #17
	mov     r6, r6, asr #17
	mov     r3, r3, asr #17
	mov     r4, r4, asr #17
	mov     r2, r2, asr #17	
;	r1, r8, r9, r5, r6, r3, r4, r2
;	r7, r10, r11, r12 free now

	ldr	r7, [sp, #12]		; dst = [sp, #12]
	ldr	r10, [sp, #8]		; dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00								
;	r1, r8, r9, r5, r6, r3, r4, r2							
;	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		; dst = [sp, #12]
	mov	pc, lr
	ENDP	


CarryMask	DCD 0x80808080
W1			DCW	2841                 ; 2048*sqrt(2)*cos(1*pi/16) 
W3			DCW 2408                 ; 2048*sqrt(2)*cos(3*pi/16) 
nW5			DCW 0xF9B7 ;-1609        ; 2048*sqrt(2)*cos(5*pi/16) 
W6			DCW 1108                 ; 2048*sqrt(2)*cos(6*pi/16) 
W7			DCW 565                  ; 2048*sqrt(2)*cos(7*pi/16) 
W2			DCW 2676                 ; 2048*sqrt(2)*cos(2*pi/16) 
; r6 Block
; r7 Src
; r8 Dst

	ALIGN 16
;ArmIdctA PROC
__voMPEG2D0182 PROC

	add		r0, r0, #128
	stmdb   sp!, {r0, r2, r4 - r12, lr}  ; r0=BlockEnd r2=DstStride

;	ldr		r2, [sp, #48]	;SrcStride

	sub		r6, r0, #128	;Block
	mov		r7, r3			;Src
	mov	    r8, r1			;Dst

	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8 
	sub     r6, r6, #6

Row4_Loop

	ldrsh   r4, [r6, #4]		;x3
	ldrsh   r5, [r6, #6]		;x7
	ldrsh   r3, [r6, #2]		;x4

	orr     r11, r5, r4
	ldrsh   r0, [r6]			;x0
	orrs    r11, r11, r3
	bne     Row4_NoConst

	bl		RowConst
	b		Row4_Next

Row4_NoConst
	cmp     r7, #0

	ldrsh	r10, W7
	ldrsh	r11, W1
	add     r0, r0, #32
	mov	r2, #4
	mov     r0, r0, lsl #8		;x0
	mla     r14, r3, r10, r2	;x5 = x4 * W7 + 4
	ldrsh	r10, W3
	mla     r3, r11, r3, r2		;x4 = x4 * W1 + 4
	mov     r14, r14, asr #3	;x5 >>= 3
	ldrsh	r11, nW5
	mla     r12, r5, r10, r2	;x6 = x7 * W3 + 4
	mov     r3, r3, asr #3		;x4 >>= 3
	ldrsh	r10, W6
	mla     r5, r11, r5, r2		;x7 = x7 * -W5 + 4
	ldrsh	r11, W2
	add     r9, r3, r12, asr #3	;x1 = x4 + (x6 >> 3)
	sub     r3, r3, r12, asr #3 ;x4 = x4 - (x6 >> 3)
	mla     r12, r4, r10, r2	;x2 = x3 * W6 + 4
	add     r10, r14, r5, asr #3	;x6 = x5 + (x7 >> 3)
	mla     r4, r11, r4, r2		;x3 = x3 * W2 + 4
	sub     r5, r14, r5, asr #3 ;x5 = x5 - (x7 >> 3)
	add     r14, r0, r4, asr #3 ;x7 = x0 + (x3 >> 3)
	sub     r4, r0, r4, asr #3	;x8 = x0 - (x3 >> 3)
	add     r2, r0, r12, asr #3;x3 = x0 + (x2 >> 3)
	sub     r0, r0, r12, asr #3	;x0 = x0 - (x2 >> 3)
	add     r1, r5, r3			
	mov     r11, #181
	mul     r12, r1, r11		;x2 = 181 * (x5 + x4)
	sub     r3, r3, r5
	mul     r1, r3, r11			;x4 = 181 * (x4 - x5)
	add     r12, r12, #128		;x2 += 128
	add     r3, r1, #128		;x4 += 128
	add     r1, r14, r9			;x5 = x7 + x1
	sub     r5, r14, r9			;x1 = x7 - x1
	add     r11, r2, r12, asr #8 ;x7 = x3 + (x2 >> 8)
	sub     r14, r2, r12, asr #8 ;x2 = x3 - (x2 >> 8)
	add     r9, r0, r3, asr #8	;x3 = x0 + (x4 >> 8)
	sub     r3, r0, r3, asr #8  ;x4 = x0 - (x4 >> 8)
	add     r12, r4, r10			;x0 = x8 + x6
	sub     r4,  r4, r10			;x6 = x8 - x6

	beq     Row4_NoSrc

	ldrb    r0, [r7]
	ldrb    r2, [r7, #7]
	ldrb    r10, [r7, #1]
	add     r1, r0, r1, asr #14
	add     r5, r2, r5, asr #14
	add     r11, r10, r11, asr #14
	ldrb    r2, [r7, #6]
	ldrb    r0, [r7, #2]
	ldrb    r10, [r7, #5]
	add     r14, r2, r14, asr #14
	add     r9, r0, r9, asr #14
	ldrb    r0, [r7, #3]
	ldrb    r2, [r7, #4]
	add     r3, r10, r3, asr #14
	add     r4, r2, r4, asr #14


	ldr		r2, [sp, #48]	;SrcStride
	add     r12, r0, r12, asr #14

	add		r7, r7, r2			;source stride
;	add		r7, r7, #8			;source stride

Row4_Sat
	orr     r0, r5, r14
	orr     r0, r0, r4
	orr     r0, r0, r1
	orr     r0, r0, r12
	orr     r0, r0, r11
	orr     r0, r0, r9
	orr     r0, r0, r3
	bics    r0, r0, #0xFF  ; 0xFF = 255
	beq     Row4_Write

	mov		r0, #0xFFFFFF00

	tst     r1, r0
	movne	r1, #0xFF
	movmi	r1, #0x00

	tst     r11, r0
	movne	r11, #0xFF
	movmi	r11, #0x00

	tst     r9, r0
	movne	r9, #0xFF
	movmi	r9, #0x00

	tst     r12, r0
	movne	r12, #0xFF
	movmi	r12, #0x00

	tst     r4, r0
	movne	r4, #0xFF
	movmi	r4, #0x00

	tst     r3, r0
	movne	r3, #0xFF
	movmi	r3, #0x00

	tst     r14, r0
	movne	r14, #0xFF
	movmi	r14, #0x00

	tst     r5, r0
	movne	r5, #0xFF
	movmi	r5, #0x00

Row4_Write
	strb    r1, [r8]
	strb    r11,[r8, #1]
	strb    r9, [r8, #2]
	strb    r12,[r8, #3]
	strb    r4, [r8, #4]
	strb    r3, [r8, #5]
	strb    r14,[r8, #6]
	strb    r5, [r8, #7]

Row4_Next
	ldr		r2, [sp, #4]	;DstStride
	ldr		r1, [sp, #0]	;BlockEnd

	add		r6,r6,#16		;Block += 16
	add		r8,r8,r2		;Dst += DstStride

	cmp		r6,r1
	bne		Row4_Loop

	ldmia   sp!, {r0,r2,r4 - r12, pc}  

Row4_NoSrc

	mov     r5, r5, asr #14
	mov     r14, r14, asr #14
	mov     r12, r12, asr #14
	mov     r1, r1, asr #14
	mov     r11, r11, asr #14
	mov     r9, r9, asr #14
	mov     r3, r3, asr #14
	mov     r4, r4, asr #14

	b		Row4_Sat
	ENDP

; r6 Block
; r7 Src
; r8 Dst


	ALIGN 16
;ArmIdctB PROC
__voMPEG2D0183 PROC

	add		r0, r0, #128
	stmdb   sp!, {r0, r2, r4 - r12, lr}  ; r0=BlockEnd r2=DstStride
;	ldr		r2, [sp, #48]	;SrcStride

	sub		r6, r0, #128	;Block
	mov		r7, r3			;Src
	mov	    r8, r1			;Dst

	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8 
	sub     r6, r6, #14

Row8_Loop
	ldrsh   r3, [r6, #2]		;x4
	ldrsh   r4, [r6, #4]		;x3
	ldrsh   r5, [r6, #6]		;x7
	ldrsh   r9, [r6, #8]		;x1
	ldrsh   r2, [r6, #10]		;x6
	ldrsh   r14,[r6, #12]		;x2
	ldrsh   r1, [r6, #14]		;x5

	orr     r11, r3, r4
	ldrsh   r0, [r6]			;x0
	orr	r11, r11, r5
	orr     r11, r11, r9
	orr     r11, r11, r2
	orr     r11, r11, r14
	orrs    r11, r11, r1
	bne     Row8_NoConst

	bl		RowConst
	b		Row8_Next

_W3			DCW 2408                 ; 2048*sqrt(2)*cos(3*pi/16) 
_W6			DCW 1108                 ; 2048*sqrt(2)*cos(6*pi/16) 
_W7			DCW 565                  ; 2048*sqrt(2)*cos(7*pi/16) 
_W1_nW7			DCW 2276
_W3_nW5			DCW 4017
_W2_nW6			DCW 1568
_nW1_nW7		DCW 0xF2B2 ;-3406
_W5_nW3			DCW 0xFCE1 ;-799
_nW2_nW6		DCW 0xF138 ;-3784

	ALIGN 16

Row8_NoConst
	cmp     r7, #0

	ldrsh	r10, _W7
	ldrsh	r11, _W1_nW7
	mov		r2, #4
;	x1 <<= 8;
;//	x11 = x1;
;	x0 = (Blk[0] << 8) + 8192;
	stmdb   sp!, {r6, r7, r8}; r1 = x5  can used
	sub	sp, sp, #160	; sp - 160
;add up
	add     r0, r0, #32
	add	r8, r3, r1
	mov     r0, r0, lsl #8		;x0
	
;	x8 = W7 * (x4 + x5) + 4;
	mla     r8, r10, r8, r2	;	x8 = W7 * (x4 + x5) + 4;	
	
		
;	x5 = (x8 + (_nW1_nW7) * x5) >> 3;
	mov	r9, r9, lsl #8
	ldrsh	r7, _nW1_nW7	
	str	r9, [sp, #8]	; r11
	str	r14, [sp, #12]	; x2
	mla     r14, r1, r7, r8	;	x5 = (x8 + (_nW1_nW7) * x5)

;	x4 = (x8 + (_W1_nW7) * x4) >> 3;
	ldrsh	r7, _W1_nW7	
	mov     r14, r14, asr #3	;x5 >>= 3
	mla     r3, r7, r3, r8	;	x4 = (x8 + (_W1_nW7) * x4)
	
;	x8 = W3 * (x6 + x7) + 4;
	ldrsh   r6, [r6, #10]		;x6
	mov     r3, r3, asr #3	;	x4 >>= 3
	ldrsh	r10, _W3	
	add	r7, r6, r5
	mla     r8, r7, r10, r2	;	x8 = W3 * (x6 + x7) + 4;	
	
;	x6 = (x8 + (_W5_nW3) * x6) >> 3;
	ldrsh	r7, _W5_nW3	
	ldrsh	r10, _W6
	mla     r12, r7, r6, r8	;	x6 = (x8 + (_W5_nW3) * x6)
;	x7 = (x8 - (_W3_nW5) * x7) >> 3;
	ldrsh	r7, _W3_nW5
	ldr	r6, [sp, #12]	; x2
	rsb	r7, r7, #0	
	mla     r5, r7, r5, r8	;	x7 = (x8 - (_W3_nW5) * x7)
	
	
;	x1 = W6 * (x3 + x2) + 4;
	add	r8, r6, r4
	mla     r8, r10, r8, r2	;	x8 = W7 * (x4 + x5) + 4;	
;	x2 = (x1 + (_nW2_nW6) * x2) >> 3;
	ldrsh	r7, _nW2_nW6	
	add     r9, r3, r12, asr #3	;x1 = x4 + (x6 >> 3)
	sub     r3, r3, r12, asr #3 ;x4 = x4 - (x6 >> 3)
	mla     r12, r7, r6, r8	;	x2 = (x1 + (_nW2_nW6) * x2)
;	x3 = (x1 + (_W2_nW6) * x3) >> 3;
	ldrsh	r7, _W2_nW6	
	add     r2, r14, r5, asr #3	;x6 = x5 + (x7 >> 3)
	mla     r4, r7, r4, r8	;	x3 = (x1 + (_W2_nW6) * x3)

	sub     r5, r14, r5, asr #3 ;x5 = x5 - (x7 >> 3)

;	x8 = x0 + x11;
;	x0 -= x11;
	ldr	r14, [sp, #8]
	add	r8, r0, r14
	sub	r0, r0, r14
	
;	x7 = x8 + x3;
;	x8 -= x3;

	add     r14, r8, r4, asr #3 ;x7 = x0 + (x3 >> 3)
	sub		r4, r8, r4, asr #3
	add     r10, r0, r12, asr #3;x3 = x0 + (x2 >> 3)
	sub     r0, r0, r12, asr #3	;x0 = x0 - (x2 >> 3)
	add     r1, r5, r3			
	mov     r11, #181
	mul     r12, r1, r11		;x2 = 181 * (x5 + x4)
	sub     r3, r3, r5
	mul     r1, r3, r11			;x4 = 181 * (x4 - x5)
	add     r12, r12, #128		;x2 += 128
	add     r3, r1, #128		;x4 += 128
	add     r1, r14, r9			;x5 = x7 + x1
	sub     r5, r14, r9			;x1 = x7 - x1
	add     r11, r10, r12, asr #8 ;x7 = x3 + (x2 >> 8)
	sub     r14, r10, r12, asr #8 ;x2 = x3 - (x2 >> 8)
	add     r9, r0, r3, asr #8	;x3 = x0 + (x4 >> 8)
	sub     r3, r0, r3, asr #8  ;x4 = x0 - (x4 >> 8)
	add     r12, r4, r2			;x0 = x8 + x6
	sub     r4,  r4, r2			;x6 = x8 - x6

    add     sp, sp, #160
	ldmia   sp!, {r6, r7, r8}  

;add up
	beq     Row8_NoSrc

	ldrb    r0, [r7]
	ldrb    r2, [r7, #7]
	ldrb    r10, [r7, #1]
	add     r1, r0, r1, asr #14
	add     r5, r2, r5, asr #14
	add     r11, r10, r11, asr #14
	ldrb    r2, [r7, #6]
	ldrb    r0, [r7, #2]
	ldrb    r10, [r7, #5]
	add     r14, r2, r14, asr #14
	add     r9, r0, r9, asr #14
	ldrb    r0, [r7, #3]
	ldrb    r2, [r7, #4]
	add     r3, r10, r3, asr #14
	add     r4, r2, r4, asr #14

	ldr		r2, [sp, #48]	;SrcStride
	add     r12, r0, r12, asr #14

	add		r7, r7, r2			;source stride
;	add		r7, r7, #8			;source stride

Row8_Sat
	orr     r0, r5, r14
	orr     r0, r0, r4
	orr     r0, r0, r1
	orr     r0, r0, r12
	orr     r0, r0, r11
	orr     r0, r0, r9
	orr     r0, r0, r3
	bics    r0, r0, #0xFF  ; 0xFF = 255
	beq     Row8_Write

	mov		r0, #0xFFFFFF00

	tst     r1, r0
	movne	r1, #0xFF
	movmi	r1, #0x00

	tst     r11, r0
	movne	r11, #0xFF
	movmi	r11, #0x00

	tst     r9, r0
	movne	r9, #0xFF
	movmi	r9, #0x00

	tst     r12, r0
	movne	r12, #0xFF
	movmi	r12, #0x00

	tst     r4, r0
	movne	r4, #0xFF
	movmi	r4, #0x00

	tst     r3, r0
	movne	r3, #0xFF
	movmi	r3, #0x00

	tst     r14, r0
	movne	r14, #0xFF
	movmi	r14, #0x00

	tst     r5, r0
	movne	r5, #0xFF
	movmi	r5, #0x00

Row8_Write
	strb    r1, [r8]
	strb    r11,[r8, #1]
	strb    r9, [r8, #2]
	strb    r12,[r8, #3]
	strb    r4, [r8, #4]
	strb    r3, [r8, #5]
	strb    r14,[r8, #6]
	strb    r5, [r8, #7]

Row8_Next
	ldr		r2, [sp, #4]	;DstStride
	ldr		r1, [sp, #0]	;BlockEnd

	add		r6,r6,#16		;Block += 16
	add		r8,r8,r2		;Dst += DstStride

	cmp		r6,r1
	bne		Row8_Loop

	ldmia   sp!, {r0,r2,r4 - r12, pc}  

Row8_NoSrc

	mov     r5, r5, asr #14
	mov     r14, r14, asr #14
	mov     r12, r12, asr #14
	mov     r1, r1, asr #14
	mov     r11, r11, asr #14
	mov     r9, r9, asr #14
	mov     r3, r3, asr #14
	mov     r4, r4, asr #14

	b		Row8_Sat
	ENDP
	
	ALIGN 16
;ArmIdctE PROC
__voMPEG2D0238 PROC

	add		r0, r0, #128
	stmdb   sp!, {r0, r2, r4 - r12, lr}  ; r0=BlockEnd r2=DstStride
;	ldr		r2, [sp, #48]	;SrcStride

	sub		r6, r0, #128	;Block
	mov		r7, r3			;Src
	mov	    r8, r1			;Dst

	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8 
	sub     r6, r6, #14

B8x4_Row8_Loop
	ldrsh   r3, [r6, #2]		;x4
	ldrsh   r4, [r6, #4]		;x3
	ldrsh   r5, [r6, #6]		;x7
	ldrsh   r9, [r6, #8]		;x1
	ldrsh   r2, [r6, #10]		;x6
	ldrsh   r14,[r6, #12]		;x2
	ldrsh   r1, [r6, #14]		;x5

	orr     r11, r3, r4
	ldrsh   r0, [r6]			;x0
	orr	    r11, r11, r5
	orr     r11, r11, r9
	orr     r11, r11, r2
	orr     r11, r11, r14
	orrs    r11, r11, r1
	bne     B8x4_Row8_NoConst
	bl		RowConst
	b		B8x4_Row8_Next
	

_B8x4_W3			DCW 2408                 ; 2048*sqrt(2)*cos(3*pi/16) 
_B8x4_W6			DCW 1108                 ; 2048*sqrt(2)*cos(6*pi/16) 
_B8x4_W7			DCW 565                  ; 2048*sqrt(2)*cos(7*pi/16) 
_B8x4_W1_nW7		DCW 2276
_B8x4_W3_nW5		DCW 4017
_B8x4_W2_nW6		DCW 1568
_B8x4_nW1_nW7		DCW 0xF2B2 ;-3406
_B8x4_W5_nW3		DCW 0xFCE1 ;-799
_B8x4_nW2_nW6		DCW 0xF138 ;-3784

	ALIGN 16

B8x4_Row8_NoConst
	cmp     r7, #0

	ldrsh	r10, _B8x4_W7
	ldrsh	r11, _B8x4_W1_nW7
	mov		r2, #4
;	x1 <<= 8;
;//	x11 = x1;
;	x0 = (Blk[0] << 8) + 8192;
	stmdb   sp!, {r6, r7, r8}; r1 = x5  can used
	sub	sp, sp, #160	; sp - 160
;add up
	add     r0, r0, #32
	add	    r8, r3, r1
	mov     r0, r0, lsl #8		;x0
	
;	x8 = W7 * (x4 + x5) + 4;
	mla     r8, r10, r8, r2	;	x8 = W7 * (x4 + x5) + 4;	
	
		
;	x5 = (x8 + (_nW1_nW7) * x5) >> 3;
	mov	r9, r9, lsl #8
	ldrsh	r7, _B8x4_nW1_nW7	
	str	r9, [sp, #8]	; r11
	str	r14, [sp, #12]	; x2
	mla     r14, r1, r7, r8	;	x5 = (x8 + (_nW1_nW7) * x5)

;	x4 = (x8 + (_W1_nW7) * x4) >> 3;
	ldrsh	r7, _B8x4_W1_nW7	
	mov     r14, r14, asr #3	;x5 >>= 3
	mla     r3, r7, r3, r8	;	x4 = (x8 + (_W1_nW7) * x4)
	
;	x8 = W3 * (x6 + x7) + 4;
	ldrsh   r6, [r6, #10]		;x6
	mov     r3, r3, asr #3	;	x4 >>= 3
	ldrsh	r10, _B8x4_W3	
	add	r7, r6, r5
	mla     r8, r7, r10, r2	;	x8 = W3 * (x6 + x7) + 4;	
	
;	x6 = (x8 + (_W5_nW3) * x6) >> 3;
	ldrsh	r7, _B8x4_W5_nW3	
	ldrsh	r10, _B8x4_W6
	mla     r12, r7, r6, r8	;	x6 = (x8 + (_W5_nW3) * x6)
;	x7 = (x8 - (_W3_nW5) * x7) >> 3;
	ldrsh	r7, _B8x4_W3_nW5
	ldr	r6, [sp, #12]	; x2
	rsb	r7, r7, #0	
	mla     r5, r7, r5, r8	;	x7 = (x8 - (_W3_nW5) * x7)
	
	
;	x1 = W6 * (x3 + x2) + 4;
	add	r8, r6, r4
	mla     r8, r10, r8, r2	;	x8 = W7 * (x4 + x5) + 4;	
;	x2 = (x1 + (_nW2_nW6) * x2) >> 3;
	ldrsh	r7, _B8x4_nW2_nW6	
	add     r9, r3, r12, asr #3	;x1 = x4 + (x6 >> 3)
	sub     r3, r3, r12, asr #3 ;x4 = x4 - (x6 >> 3)
	mla     r12, r7, r6, r8	;	x2 = (x1 + (_nW2_nW6) * x2)
;	x3 = (x1 + (_W2_nW6) * x3) >> 3;
	ldrsh	r7, _B8x4_W2_nW6	
	add     r2, r14, r5, asr #3	;x6 = x5 + (x7 >> 3)
	mla     r4, r7, r4, r8	;	x3 = (x1 + (_W2_nW6) * x3)

	sub     r5, r14, r5, asr #3 ;x5 = x5 - (x7 >> 3)

;	x8 = x0 + x11;
;	x0 -= x11;
	ldr	r14, [sp, #8]
	add	r8, r0, r14
	sub	r0, r0, r14
	
;	x7 = x8 + x3;
;	x8 -= x3;

	add     r14, r8, r4, asr #3 ;x7 = x0 + (x3 >> 3)
	sub		r4, r8, r4, asr #3
	add     r10, r0, r12, asr #3;x3 = x0 + (x2 >> 3)
	sub     r0, r0, r12, asr #3	;x0 = x0 - (x2 >> 3)
	add     r1, r5, r3			
	mov     r11, #181
	mul     r12, r1, r11		;x2 = 181 * (x5 + x4)
	sub     r3, r3, r5
	mul     r1, r3, r11			;x4 = 181 * (x4 - x5)
	add     r12, r12, #128		;x2 += 128
	add     r3, r1, #128		;x4 += 128
	add     r1, r14, r9			;x5 = x7 + x1
	sub     r5, r14, r9			;x1 = x7 - x1
	add     r11, r10, r12, asr #8 ;x7 = x3 + (x2 >> 8)
	sub     r14, r10, r12, asr #8 ;x2 = x3 - (x2 >> 8)
	add     r9, r0, r3, asr #8	;x3 = x0 + (x4 >> 8)
	sub     r3, r0, r3, asr #8  ;x4 = x0 - (x4 >> 8)
	add     r12, r4, r2			;x0 = x8 + x6
	sub     r4,  r4, r2			;x6 = x8 - x6

    add     sp, sp, #160
	ldmia   sp!, {r6, r7, r8}  

;add up
	beq     B8x4_Row8_NoSrc

	ldrb    r0, [r7]
	ldrb    r2, [r7, #7]
	ldrb    r10, [r7, #1]
	add     r1, r0, r1, asr #14
	add     r5, r2, r5, asr #14
	add     r11, r10, r11, asr #14
	ldrb    r2, [r7, #6]
	ldrb    r0, [r7, #2]
	ldrb    r10, [r7, #5]
	add     r14, r2, r14, asr #14
	add     r9, r0, r9, asr #14
	ldrb    r0, [r7, #3]
	ldrb    r2, [r7, #4]
	add     r3, r10, r3, asr #14
	add     r4, r2, r4, asr #14

	ldr		r2, [sp, #48]	;SrcStride
	add     r12, r0, r12, asr #14

	add		r7, r7, r2			;source stride
	;add		r7, r7, #8			;source stride

B8x4_Row8_Sat
	orr     r0, r5, r14
	orr     r0, r0, r4
	orr     r0, r0, r1
	orr     r0, r0, r12
	orr     r0, r0, r11
	orr     r0, r0, r9
	orr     r0, r0, r3
	bics    r0, r0, #0xFF  ; 0xFF = 255
	beq     B8x4_Row8_Write

	mov		r0, #0xFFFFFF00

	tst     r1, r0
	movne	r1, #0xFF
	movmi	r1, #0x00

	tst     r11, r0
	movne	r11, #0xFF
	movmi	r11, #0x00

	tst     r9, r0
	movne	r9, #0xFF
	movmi	r9, #0x00

	tst     r12, r0
	movne	r12, #0xFF
	movmi	r12, #0x00

	tst     r4, r0
	movne	r4, #0xFF
	movmi	r4, #0x00

	tst     r3, r0
	movne	r3, #0xFF
	movmi	r3, #0x00

	tst     r14, r0
	movne	r14, #0xFF
	movmi	r14, #0x00

	tst     r5, r0
	movne	r5, #0xFF
	movmi	r5, #0x00

B8x4_Row8_Write
	strb    r1, [r8]
	strb    r11,[r8, #1]
	strb    r9, [r8, #2]
	strb    r12,[r8, #3]
	strb    r4, [r8, #4]
	strb    r3, [r8, #5]
	strb    r14,[r8, #6]
	strb    r5, [r8, #7]

B8x4_Row8_Next
	ldr		r2, [sp, #4]	;DstStride
	ldr		r1, [sp, #0]	;BlockEnd

	add		r6,r6,#32		;Block += 32 huwei 20080618 Bframe_field
	add		r8,r8,r2		;Dst += DstStride

	cmp		r6,r1
	bne		B8x4_Row8_Loop

	ldmia   sp!, {r0,r2,r4 - r12, pc}  

B8x4_Row8_NoSrc

	mov     r5, r5, asr #14
	mov     r14, r14, asr #14
	mov     r12, r12, asr #14
	mov     r1, r1, asr #14
	mov     r11, r11, asr #14
	mov     r9, r9, asr #14
	mov     r3, r3, asr #14
	mov     r4, r4, asr #14

	b		B8x4_Row8_Sat
	ENDP

	ALIGN 16
;ArmIdctF PROC
__voMPEG2D0241 PROC

	add		r0, r0, #128
	stmdb   sp!, {r0, r2, r4 - r12, lr}  ; r0=BlockEnd r2=DstStride

;	ldr		r2, [sp, #48]	;SrcStride

	sub		r6, r0, #128	;Block
	mov		r7, r3			;Src
	mov	    r8, r1			;Dst

	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8 
	sub     r6, r6, #6

B4x4_Row4_Loop

	ldrsh   r4, [r6, #4]		;x3
	ldrsh   r5, [r6, #6]		;x7
	ldrsh   r3, [r6, #2]		;x4

	orr     r11, r5, r4
	ldrsh   r0, [r6]			;x0
	orrs    r11, r11, r3
	bne     B4x4_Row4_NoConst

	bl		RowConst
	b		B4x4_Row4_Next
	

B4x4_W1			DCW	2841                 ; 2048*sqrt(2)*cos(1*pi/16) 
B4x4_W3			DCW 2408                 ; 2048*sqrt(2)*cos(3*pi/16) 
B4x4_nW5		DCW 0xF9B7 ;-1609        ; 2048*sqrt(2)*cos(5*pi/16) 
B4x4_W6			DCW 1108                 ; 2048*sqrt(2)*cos(6*pi/16) 
B4x4_W7			DCW 565                  ; 2048*sqrt(2)*cos(7*pi/16) 
B4x4_W2			DCW 2676                 ; 2048*sqrt(2)*cos(2*pi/16) 

B4x4_Row4_NoConst
	cmp     r7, #0

	ldrsh	r10, B4x4_W7
	ldrsh	r11, B4x4_W1
	add     r0, r0, #32
	mov	r2, #4
	mov     r0, r0, lsl #8		;x0
	mla     r14, r3, r10, r2	;x5 = x4 * W7 + 4
	ldrsh	r10, B4x4_W3
	mla     r3, r11, r3, r2		;x4 = x4 * W1 + 4
	mov     r14, r14, asr #3	;x5 >>= 3
	ldrsh	r11, B4x4_nW5
	mla     r12, r5, r10, r2	;x6 = x7 * W3 + 4
	mov     r3, r3, asr #3		;x4 >>= 3
	ldrsh	r10, B4x4_W6
	mla     r5, r11, r5, r2		;x7 = x7 * -W5 + 4
	ldrsh	r11, B4x4_W2
	add     r9, r3, r12, asr #3	;x1 = x4 + (x6 >> 3)
	sub     r3, r3, r12, asr #3 ;x4 = x4 - (x6 >> 3)
	mla     r12, r4, r10, r2	;x2 = x3 * W6 + 4
	add     r10, r14, r5, asr #3	;x6 = x5 + (x7 >> 3)
	mla     r4, r11, r4, r2		;x3 = x3 * W2 + 4
	sub     r5, r14, r5, asr #3 ;x5 = x5 - (x7 >> 3)
	add     r14, r0, r4, asr #3 ;x7 = x0 + (x3 >> 3)
	sub     r4, r0, r4, asr #3	;x8 = x0 - (x3 >> 3)
	add     r2, r0, r12, asr #3;x3 = x0 + (x2 >> 3)
	sub     r0, r0, r12, asr #3	;x0 = x0 - (x2 >> 3)
	add     r1, r5, r3			
	mov     r11, #181
	mul     r12, r1, r11		;x2 = 181 * (x5 + x4)
	sub     r3, r3, r5
	mul     r1, r3, r11			;x4 = 181 * (x4 - x5)
	add     r12, r12, #128		;x2 += 128
	add     r3, r1, #128		;x4 += 128
	add     r1, r14, r9			;x5 = x7 + x1
	sub     r5, r14, r9			;x1 = x7 - x1
	add     r11, r2, r12, asr #8 ;x7 = x3 + (x2 >> 8)
	sub     r14, r2, r12, asr #8 ;x2 = x3 - (x2 >> 8)
	add     r9, r0, r3, asr #8	;x3 = x0 + (x4 >> 8)
	sub     r3, r0, r3, asr #8  ;x4 = x0 - (x4 >> 8)
	add     r12, r4, r10			;x0 = x8 + x6
	sub     r4,  r4, r10			;x6 = x8 - x6

	beq     B4x4_Row4_NoSrc

	ldrb    r0, [r7]
	ldrb    r2, [r7, #7]
	ldrb    r10, [r7, #1]
	add     r1, r0, r1, asr #14
	add     r5, r2, r5, asr #14
	add     r11, r10, r11, asr #14
	ldrb    r2, [r7, #6]
	ldrb    r0, [r7, #2]
	ldrb    r10, [r7, #5]
	add     r14, r2, r14, asr #14
	add     r9, r0, r9, asr #14
	ldrb    r0, [r7, #3]
	ldrb    r2, [r7, #4]
	add     r3, r10, r3, asr #14
	add     r4, r2, r4, asr #14


	ldr		r2, [sp, #48]	;SrcStride
	add     r12, r0, r12, asr #14

	add		r7, r7, r2			;source stride
	;add		r7, r7, #8			;source stride

B4x4_Row4_Sat
	orr     r0, r5, r14
	orr     r0, r0, r4
	orr     r0, r0, r1
	orr     r0, r0, r12
	orr     r0, r0, r11
	orr     r0, r0, r9
	orr     r0, r0, r3
	bics    r0, r0, #0xFF  ; 0xFF = 255
	beq     B4x4_Row4_Write

	mov		r0, #0xFFFFFF00

	tst     r1, r0
	movne	r1, #0xFF
	movmi	r1, #0x00

	tst     r11, r0
	movne	r11, #0xFF
	movmi	r11, #0x00

	tst     r9, r0
	movne	r9, #0xFF
	movmi	r9, #0x00

	tst     r12, r0
	movne	r12, #0xFF
	movmi	r12, #0x00

	tst     r4, r0
	movne	r4, #0xFF
	movmi	r4, #0x00

	tst     r3, r0
	movne	r3, #0xFF
	movmi	r3, #0x00

	tst     r14, r0
	movne	r14, #0xFF
	movmi	r14, #0x00

	tst     r5, r0
	movne	r5, #0xFF
	movmi	r5, #0x00

B4x4_Row4_Write
	strb    r1, [r8]
	strb    r11,[r8, #1]
	strb    r9, [r8, #2]
	strb    r12,[r8, #3]
	strb    r4, [r8, #4]
	strb    r3, [r8, #5]
	strb    r14,[r8, #6]
	strb    r5, [r8, #7]

B4x4_Row4_Next
	ldr		r2, [sp, #4]	;DstStride
	ldr		r1, [sp, #0]	;BlockEnd

	add		r6,r6,#32		;Block += 32 huwei 20080618 Bframe_field
	add		r8,r8,r2		;Dst += DstStride

	cmp		r6,r1
	bne		B4x4_Row4_Loop

	ldmia   sp!, {r0,r2,r4 - r12, pc}  

B4x4_Row4_NoSrc

	mov     r5, r5, asr #14
	mov     r14, r14, asr #14
	mov     r12, r12, asr #14
	mov     r1, r1, asr #14
	mov     r11, r11, asr #14
	mov     r9, r9, asr #14
	mov     r3, r3, asr #14
	mov     r4, r4, asr #14

	b		B4x4_Row4_Sat
	ENDP

	ALIGN 4
;ArmIdctG PROC
__voMPEG2D0257 PROC
        STMFD    sp!,{r4-r11,lr}	
; r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	sub	sp, sp, #16	
;	ldr	r9, [sp, #52]		; src_stride = [sp, #52]
	str	r3, [sp, #4]		; Src = [sp, #4]
	str	r2, [sp, #8]		; dst_stride = [sp, #8]
	str	r1, [sp, #12]		; dst = [sp, #12]
	
	bl      Col8_4x4  
	add     r0, r0, #2
	bl      Col8_4x4  
	add     r0, r0, #2	
	bl      Col8_4x4  
	add     r0, r0, #2	
	bl      Col8_4x4  	
	sub     r0, r0, #6
	
	ldr	r8, [sp, #4]		; Src = [sp, #4]	
 	cmp	r8, #0				
	beq	only_noSrc_lab_4x4		; Src=0	

	bl		Row8src_4x4
	add		r0, r0, #16			;Block += 16		
	bl		Row8src_4x4
	add		r0, r0, #16			;Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			;Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			;Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			;Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			;Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			;Block += 16
	bl		Row8src_4x4
	                                          		
	add	sp, sp, #16
    LDMFD    sp!,{r4-r11,pc}
        
only_noSrc_lab_4x4

	bl		Row8nosrc_4x4
	add		r0, r0, #16			;Block += 16		
	bl		Row8nosrc_4x4
	add		r0, r0, #16			;Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			;Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			;Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			;Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			;Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			;Block += 16	
	bl		Row8nosrc_4x4	
                                         		
	add	sp, sp, #16
    LDMFD    sp!,{r4-r11,pc}
        	
	ENDP
	
;huwei 20080219 downsmaple
	ALIGN 16
;ArmIdctD PROC
__voMPEG2D0185 PROC

	add	r0, r0, #128
	stmdb   sp!, {r0, r2, r4 - r12, lr}  ; r0=BlockEnd r2=DstStride

	sub		r6, r0, #128	;Block
	mov		r7, r3		;Src
	mov		r8, r1		;Dst

	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8  
	add     r6, r6, #2
	bl      Col8 
	sub     r6, r6, #6
Row8half_Loop

	ldrsh   r3, [r6, #2]		;d1
	ldrsh   r4, [r6, #4]		;d2
	ldrsh   r5, [r6, #6]		;d3

	orr     r11, r3, r4
	ldrsh   r0, [r6]		;d0
	orrs	r11, r11, r5
	bne     Row8half_NoConst

	add     r0, r0, #0x20  ; 0x20 = 32
	mov     r3, r0, asr #6
	cmp     r7, #0
	beq     RowConsthalf_NoSrc
	
	mov	r4, r3
	mov	r14, r3
	mov	r10, r3
	mov	r0, r3

	ldrb    r1, [r7]
	ldrb    r2, [r7, #1]
	ldrb    r11, [r7, #2]
	add     r4, r1, r4	;0
	add     r12, r2, r10	;1
	add     r3, r11, r0	;2
	ldrb    r2, [r7, #3]
	add     r14, r2, r14	;3

	ldr	r2, [sp, #48]	;SrcStride
	add	r7, r7, r2	;source stride

	b	Row8half_Sat
	
RowConsthalf_NoSrc
	cmp     r3, #0
	movmi   r3, #0
	cmppl   r3, #255
	movgt   r3, #255
	orr     r3, r3, r3, lsl #8
	orr     r3, r3, r3, lsl #16
	str     r3,[r8]
	b	Row8half_Next
		
hal_W3			DCW 2408                 ; 2048*sqrt(2)*cos(3*pi/16) 
hal_W6			DCW 1108                 ; 2048*sqrt(2)*cos(6*pi/16) 
hal_W7			DCW 565                  ; 2048*sqrt(2)*cos(7*pi/16) 
hal_W1_nW7		DCW 2276
hal_W3_nW5		DCW 4017
hal_W2_nW6		DCW 1568
hal_nW1_nW7		DCW 0xF2B2 ;-3406
hal_W5_nW3		DCW 0xFCE1 ;-799
hal_nW2_nW6		DCW 0xF138 ;-3784

	ALIGN 16

Row8half_NoConst
	ldrsh	r10, hal_W6
	mov	r2, #4
;	x1 <<= 8;
;	x0 = (Blk[0] << 8) + 8192;
	stmdb   sp!, {r6, r7, r8}; r1 = x5  can used

	sub	sp, sp, #160	; sp - 160
;add up	
;	x8 = W6 * (d1 + d3) + 4;
	add	r8, r3, r5
	ldrsh	r7, hal_nW2_nW6	
	mla     r8, r10, r8, r2	;x8 = W6 * (d1 + d3) + 4;	
	
		
;	x5 = (x8 + (hal_nW2_nW6) * d3) >> 3;
	add     r0, r0, #32	;Blk[0] + 32
	mov	r4, r4, lsl #8	; d2<<8
	mov     r0, r0, lsl #8	;d0<<8 + 32<<8
	str	r4, [sp, #8]	; r11
	str	r14, [sp, #12]	; x2
	mla     r14, r5, r7, r8	;	x5 = (x8 + (hal_nW2_nW6) * d3)

;	x4 = (x8 + (hal_W2_nW6) * d1) >> 3;
	ldrsh	r7, hal_W2_nW6	
	mla     r3, r7, r3, r8	;	x4 = (x8 + (hal_W2_nW6) * d1)
	mov     r14, r14, asr #3	;x5 >>= 3	
	mov     r3, r3, asr #3	;x4 >>= 3
			

;	x8 = x0 + x11;
;	x0 -= x11;
	ldr	r7, [sp, #8]
	add     sp, sp, #160
	add	r8, r0, r7		;d0+d2
	sub	r0, r0, r7		;d0-d2
	

	add     r4, r8, r3		;0
	add     r10, r0, r14		;1
	sub     r0, r0, r14		;2
	sub	r14, r8, r3		;3

	ldmia   sp!, {r6, r7, r8}  
	cmp     r7, #0
	beq     Row8half_NoSrc

	ldrb    r1, [r7]
	ldrb    r2, [r7, #1]
	ldrb    r11, [r7, #2]
	add     r4, r1, r4, asr #14	;0
	add     r12, r2, r10, asr #14	;1
	add     r3, r11, r0, asr #14	;2
	ldrb    r2, [r7, #3]
	add     r14, r2, r14, asr #14	;3

	ldr	r2, [sp, #48]	;SrcStride
	add	r7, r7, r2	;source stride

Row8half_Sat
	orr     r0, r12, r4
	orr     r0, r0, r14
	orr     r0, r0, r3

	bics    r0, r0, #0xFF  ; 0xFF = 255
	beq     Row8half_Write

	mov	r0, #0xFFFFFF00

	tst     r14, r0
	movne	r14, #0xFF
	movmi	r14, #0x00

	tst     r12, r0
	movne	r12, #0xFF
	movmi	r12, #0x00

	tst     r3, r0
	movne	r3, #0xFF
	movmi	r3, #0x00

	tst     r4, r0
	movne	r4, #0xFF
	movmi	r4, #0x00

Row8half_Write
	strb    r4, [r8]
	strb    r12,[r8, #1]
	strb    r3, [r8, #2]
	strb    r14,[r8, #3]

Row8half_Next
	ldr		r2, [sp, #4]	;DstStride
	ldr		r1, [sp, #0]	;BlockEnd

	add		r6,r6,#16	;Block += 16
	add		r8,r8,r2	;Dst += DstStride

	cmp		r6,r1
	bne		Row8half_Loop

	ldmia   sp!, {r0,r2,r4 - r12, pc}  

Row8half_NoSrc

	mov     r4, r4, asr #14         ;0
	mov     r14, r14, asr #14	;3
	mov     r12, r10, asr #14	;1
	mov     r3, r0, asr #14		;2

	b		Row8half_Sat
	ENDP

|MaskCarry|  
			DCD	0x80808080		;VO_U32 MaskCarry = 0x80808080U;

;	RowConst_t $Name,	$ARMv6
	RowConst_t RowConst,	0
;-------------------------------------------------------
;	arm_transc8x8_t $Name, 		$ARMv6
;	arm_transc8x8_t ArmIdctC,	0
;-------------------------------------------------------

;	arm_transc8x8_t $Name, 		$ARMv6
	arm_transc8x8_t __voMPEG2D0184,	0
	
	END


|MaskCarry|  
			DCD	0x80808080		;VO_U32 MaskCarry = 0x80808080U;

;	RowConst_t $Name,	$ARMv6
	RowConst_t RowConst,	0

;	arm_transc8x8_t $Name, 		$ARMv6
	arm_transc8x8_t ArmIdctC,	0

	END