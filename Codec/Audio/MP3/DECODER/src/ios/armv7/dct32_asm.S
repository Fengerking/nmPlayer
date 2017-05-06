#include "voMP3DecID.h"
	.text
	.align 4

        .globl  _dctIdx
        .globl  _dcttab        	
	.globl	_voMP3DecDCT32

_voMP3DecDCT32:	@PROC
	stmdb     sp!, {r4 - r11, lr}
M660:
	mov			   r12, r0
	ldr		  	   r6 , L664
	ldr			   r14, DCT32_Tab @L664 + 4
	ldr			   r11, DCT32_Idx @L664 + 8
	
	@first pass   
	vld1.32			{d0, d1, d2, d3}, [r12]!						@ a0
	vld1.32			{d4, d5, d6, d7}, [r12]!						@ a1	
	vld1.32			{d8, d9, d10, d11}, [r12]!						@ a2
	vrev64.32		q2, q2	
	vld1.32			{d12, d13, d14, d15}, [r12]!					@ a3
	vrev64.32		q3, q3	
	vrev64.32		q6, q6
	vrev64.32		q7, q7
	
	vswp			d4, d5
	vswp			d6, d7
	vswp			d12, d13
	vswp			d14, d15
	
	vld3.32   		{d16, d18, d20}, [r14]!
	vsub.s32		q12, q0, q7										@ a0 - a3	
	vld3.32   		{d17, d19, d21}, [r14]!	
	vadd.s32		q11, q0, q7										@ b0 = a0 + a3@
	
	vld1.32			{d30, d31}, 	[r11]!
	vsub.s32		q7, q3, q4										@ a1 - a2	
	vqdmulh.s32		q12, q12, q8									@ b3 = MUL_32(*cptr++, a0 - a3) << (s0)@
	
	vadd.s32		q0, q3, q4										@ b1 = a1 + a2@	
	vqdmulh.s32		q7, q7, q9										@ b2 = MUL_32(*cptr++, a1 - a2) << (s1)@
	
	vsub.s32		q3, q11, q0										@ b0 - b1
	vshl.s32		q7, q7, q15	
	
	vadd.s32		q4, q11, q0										@ buf[i] = b0 + b1@
	vsub.s32		q11, q12, q7									@ b3 - b2
	vqdmulh.s32		q3, q3, q10										@ buf[15-i] = MUL_32(*cptr,   b0 - b1) << (s2)@
	
	vadd.s32		q0, q12, q7										@ buf[16+i] = b2 + b3@
	vsub.s32		q12, q1, q6										@ a0 - a3	
	
	vqdmulh.s32		q7, q11, q10									@ buf[31-i] = MUL_32(*cptr++, b3 - b2) << (s2)@
	vadd.s32		q13, q1, q6										@ b0 = a0 + a3@ 
	
	vld3.32   		{d16, d18, d20}, [r14]!
	vsub.s32		q1, q2, q5										@ a1 - a2		
	
	vld3.32   		{d17, d19, d21}, [r14]!	
	vadd.s32		q6, q2, q5										@ b1 = a1 + a2@		
	
	vld1.32			{d30, d31}, 	[r11]!
	vqdmulh.s32		q1, q1, q9										@ b2 = MUL_32(*cptr++, a1 - a2) << (s1)@
	
	vqdmulh.s32		q12, q12, q8									@ b3 = MUL_32(*cptr++, a0 - a3) << (s0)@
	vshl.s32		q1, q1, q15
	
	vsub.s32		q5, q13, q6										@ b0 - b1
	vld1.32			{d30, d31}, 	[r11]!

	vadd.s32		q2, q13, q6										@ buf[i] = b0 + b1@ 
	vrev64.32		q3, q3
	vqdmulh.s32		q5, q5, q10										@ buf[15-i] = MUL_32(*cptr,   b0 - b1) << (s2)@
	
	vrev64.32		q7, q7
	vsub.s32		q11, q12, q1									@ b3 - b2
	vadd.s32		q6, q12, q1										@ buf[16+i] = b2 + b3@	

	vswp			d6, d7
	vqdmulh.s32		q1, q11, q10									@ buf[31-i] = MUL_32(*cptr++, b3 - b2) << (s2)@  
	vshl.s32		q5, q5, q15										@ buf[15-i]
	vswp			d14, d15
	vshl.s32		q1, q1, q15										@ buf[31-i] 
	
	vrev64.32		q5, q5
	vrev64.32		q1, q1
	
	vswp			d10, d11
	vswp			d2, d3
	
	vtrn.32			q4, q5
	vtrn.32			q2, q3
	
	vtrn.32			q0, q1
	vtrn.32			q6, q7
	
	vld3.32   		{d16, d18, d20}, [r14]!
	vswp			d9, d0
	vld3.32   		{d17, d19, d21}, [r14]!	
	vswp			d11, d2
	vswp			d5, d12
	vswp			d7, d14

   @ second pass	
	vsub.s32		q11, q4, q7										@ a0 - a7
	vsub.s32		q13, q1, q2										@ a3 - a4
	
	vadd.s32		q12, q4, q7										@ b0 = a0 + a7@		
	vadd.s32		q14, q1, q2										@ b3 = a3 + a4
	
	vqdmulh.s32		q13, q13, q9									@ b4 = MUL_32(*cptr++, a3 - a4) << 3@	
	vsub.s32		q4, q12, q14									@ b0 - b3
	
	vqdmulh.s32		q11, q11, q8									@ b7 = MUL_32(*cptr++, a0 - a7) << 1@ 
	vadd.s32		q7, q12, q14									@ a0 = b0 + b3@
	
	vshl.s32		q13, q13, #2
	vqdmulh.s32		q4, q4, q10										@ a3 = MUL_32(*cptr,   b0 - b3) << 1@
	
	vsub.s32		q1,	q11, q13									@ b7 - b4					
	vadd.s32		q14, q0, q3										@ b2 = a2 + a5@
	
	vadd.s32		q2, q11, q13									@ a4 = b4 + b7@
	vqdmulh.s32		q1, q1, q10										@ a7 = MUL_32(*cptr++, b7 - b4) << 1@
	
	vsub.s32		q11, q5, q6										@ a1 - a6
	vld3.32   		{d16, d18, d20}, [r14]!
	vadd.s32		q12, q5, q6										@ b1 = a1 + a6@		
	vld3.32   		{d17, d19, d21}, [r14]!
	
	vsub.s32		q13, q0, q3										@ a2 - a5	
	vqdmulh.s32		q11, q11, q8									@ b6 = MUL_32(*cptr++, a1 - a6) << 1@
	
	vqdmulh.s32		q13, q13, q9									@ b5 = MUL_32(*cptr++, a2 - a5) << 1@
	
	vsub.s32		q5, q12, q14									@ b1 - b2
	vsub.s32		q0, q11, q13									@ b6 - b5
	
	vadd.s32		q6, q12, q14									@ a1 = b1 + b2@	
	vqdmulh.s32		q5, q5, q10										@ a2 = MUL_32(*cptr,   b1 - b2) << 2@
	

	vadd.s32		q3, q11, q13									@ a5 = b5 + b6@	
	vqdmulh.s32		q0, q0, q10										@ a6 = MUL_32(*cptr++, b6 - b5) << 2@
	
	vshl.s32		q5, q5, #1
	vdup.32			q15, r6
	vshl.s32		q0, q0, #1
	
	vsub.s32		q11, q7, q6										@ a0 - a1
	vsub.s32		q13, q4, q5										@ a3 - a2
	
	vadd.s32		q10, q7, q6										@ b0 = a0 + a1@
	vqdmulh.s32		q11, q11, q15									@ b1 = MUL_32(COS4_0, a0 - a1) << 1@
	
	vadd.s32		q12, q4, q5										@ b2 = a2 + a3@
	vqdmulh.s32		q13, q13, q15									@ b3 = MUL_32(COS4_0, a3 - a2) << 1@
	
	vsub.s32		q5, q2, q3 										@ a4 - a5
	vsub.s32		q7, q1, q0										@ a7 - a6

	vadd.s32		q4, q2, q3										@ b4 = a4 + a5@
	vqdmulh.s32		q8, q5, q15										@ b5 = MUL_32(COS4_0, a4 - a5) << 1@
	
	vadd.s32		q6, q0, q1										@ b6 = a6 + a7@
	vqdmulh.s32		q7, q7, q15										@ b7 = MUL_32(COS4_0, a7 - a6) << 1@
	
	vadd.s32		q12, q12, q13
	vtrn.32			q10, q11
	
	vadd.s32		q5, q8, q7
	vadd.s32		q6, q6, q7
	
	vtrn.32			q12, q13
	vadd.s32		q4, q4, q6
	
	vswp			d21, d24
	vadd.s32		q6, q8, q6
	
	vswp			d23, d26
	vtrn.32			q4, q5
	vtrn.32			q6, q7
	
	mov				r14, r0	
	
	vswp			d9, d12
	vswp			d11, d14	

	vst1.32			{q10}, [r0]!
	vst1.32			{q4}, [r0]!
	vst1.32			{q11}, [r0]!
	vst1.32			{q5}, [r0]!
	vst1.32			{q12}, [r0]!
	vst1.32			{q6}, [r0]!
	vst1.32			{q13}, [r0]!
	vst1.32			{q7}, [r0]!	

    sub		  r12, r2, r3 			 @ offset - oddBlock
	ldr		  r4, [r14, #0]  		 @ s = buf[ 0]@
	cmp		  r3, #0                     
   	moveq	  r10, #1088 
   	moveq	  r11, #0                 
   	movne	  r10, #0                @ (oddBlock ? 0 : VBUF_LENGTH)@
    movne	  r11, #1088  			 @ (oddBlock ? VBUF_LENGTH  : 0)@
    and		  r12, r12, #7               
    add		  r12, r10, r12			 @ ((offset - oddBlock) & 7) + (oddBlock ? 0 : VBUF_LENGTH)@
    ssat	  r4, #16, r4, asr #9
    add		  r12, r1, r12, lsl #1   @ d = dest + ((offset - oddBlock) & 7) + (oddBlock ? 0 : VBUF_LENGTH)@    
    ldr       r9, [r14, #116]		 @ buf[29]   
    add		  r0, r12, #2048 
    add		  r2, r1, r2, lsl #1 
    strh	  r4, [r0, #16]              
    strh	  r4, [r0, #0]                     

 
    add		  r1, r2, r11, lsl #1    @ d = dest + offset + (oddBlock ? VBUF_LENGTH  : 0)@ 
    ldr		  r4, [r14, #4]
    ldr		  r11, [r14, #100] 	 	 @ buf[25]
    ssat	  r4, #16, r4, asr #9
	ldr       r2, [r14, #68]		 @ buf[17] 	
    add	      r0, r11, r9            @ tmp = buf[25] + buf[29]@ 	        
    strh	  r4, [r1, #16]     
 	add	      r3, r2, r0             @ s = buf[17] + tmp@			d[0] = d[8] = s >> 7@	d += 64@
    strh	  r4, [r1], #128     
    ssat	  r3, #16, r3, asr #9  
         
	ldr       r8, [r14, #52] 		 @ buf[13]	                                 
    ldr		  r5, [r14, #36]         @ buf[ 9]                    
    strh      r3, [r1, #16]                  
    strh      r3, [r1], #128 	             
    add       r4, r5, r8      		 @ s = buf[ 9] + buf[13]@		d[0] = d[8] = s >> 7@	d += 64@ 
    ldr       r7, [r14, #84]  		 @ buf[21]
    ssat	  r4, #16, r4, asr #9	               
    add       r0, r7, r0  	         @ s = buf[21] + tmp@
    
    strh      r4, [r1, #16]               
    ssat	  r0, #16, r0, asr #9   
    strh      r4, [r1], #128   
    
    ldr       r6, [r14, #108]        @ buf[27] 
    ldr       r3, [r14, #20]         @ buf[ 5]@ 
       
    strh      r0, [r1, #16]  	           
    strh      r0, [r1], #128 
            
    ssat	  r3, #16, r3, asr #9  
    add		  r0, r6, r9              @tmp = buf[29] + buf[27]@  
    
    strh      r3, [r1, #16]   
    add		  r4, r7, r0 			 @ s = buf[21] + tmp@			d[0] = d[8] = s >> 7@	d += 64@        
    strh      r3, [r1], #128   
    
    ldr       r7, [r14, #44] 	     @ buf[11]@  
    ssat	  r4, #16, r4,asr #9     @
    add       r3, r8, r7 			 @ s = buf[13] + buf[11]@		d[0] = d[8] = s >> 7@	d += 64@
    ldr       r9, [r14, #76] 		 @ buf[19]
    ssat	  r3, #16, r3, asr #9
    strh      r4, [r1, #16]  	         
    strh      r4, [r1], #128
    add       r0, r9, r0 	         @ s = buf[19] + tmp@			d[0] = d[8] = s >> 7@	d += 64@        
    strh	  r3, [r1, #16] 
    ssat	  r0, #16, r0, asr #9
    strh	  r3, [r1], #128       
       
    strh      r0, [r1, #16]           
    strh      r0, [r1], #128          
      
    ldr       r10, [r14, #124]         @ tmp = buf[31]@
    ldr       r3, [r14, #12]           @ buf[ 3]@
    add       r0, r6, r10              @ tmp = buf[27] + buf[31]@
    ssat	  r3, #16, r3, asr #9
    add		  r4, r9, r0			   @ s = buf[19] + tmp@			d[0] = d[8] = s >> 7@	d += 64@ 
    strh      r3, [r1, #16]   
    ssat	  r4, #16, r4, asr #9       
    strh      r3, [r1], #128           @ s = buf[ 3]@			    d[0] = d[8] = s >> 9@	d += 64@              
    strh      r4, [r1, #16]         
    strh      r4, [r1], #128  
    ldr       r9, [r14, #60]		   @ buf[15]@       
    add       r3, r9, r7               @ s = buf[11] + buf[15]@		d[0] = d[8] = s >> 9@	d += 64@ 
    ldr       r8, [r14, #92] 		   @ buf[23]
    ssat	  r3, #16, r3, asr #9
    add       r0, r8, r0               @ s = buf[23] + tmp@			d[0] = d[8] = s >> 9@	d += 64@
    strh      r3, [r1, #16] 
    ssat	  r0, #16, r0, asr #9
    strh	  r3, [r1], #128         
    strh      r0, [r1, #16]         
    strh      r0, [r1], #128         
    ldr       r4, [r14, #28]           @ s = buf[ 7]@				d[0] = d[8] = s >> 7@	d += 64@
    ssat      r3, #16, r10, asr #9        
    ssat	  r4, #16, r4, asr #9  
    add       r0, r8, r10              @s = buf[23] + tmp@			d[0] = d[8] = s >> 9@	d += 64@ 
    strh      r4, [r1, #16]    
    ssat	  r0, #16, r0, asr #9  
    strh      r4, [r1], #128              
    ssat	  r9, #16, r9, asr #9
    strh      r0, [r1, #16]         
    strh      r0, [r1], #128     
    strh      r9, [r1, #16]         
    strh      r9, [r1], #128 
    strh      r3, [r1, #16]         
    strh      r3, [r1]         
        
 
    ldr       r4, [r14, #4]           
    add		  r0, r12, #32      
	ssat	  r4, #16, r4, asr #9      @s = buf[ 1]@				d[0] = d[8] = s >> 9@	d += 64@
    ldr       r9, [r14, #120]  
    ldr       r8, [r14, #56]  
    add       r1, r9, r11              @tmp = buf[30] + buf[25]@  
    strh      r4, [r0, #16]          
    add       r2, r2, r1               @s = buf[17] + tmp@			d[0] = d[8] = s >> 9@	d += 64@	 
    strh	  r4, [r0], #128     
    ssat	  r2, #16, r2, asr #9
    add       r4, r5, r8               @s = buf[14] + buf[ 9]@		d[0] = d[8] = s >> 9@	d += 64@	       
    strh      r2, [r0, #16]          
    strh      r2, [r0], #128           @s1 = buf[17] + tmp@
    ldr       r6, [r14, #88]
    ssat	  r4, #16, r4, asr #9  
	add       r1, r6, r1               @s = buf[22] + tmp@			d[0] = d[8] = s >> 9@	d += 64@
    ldr       r5, [r14, #104] 		      
    strh      r4, [r0, #16]    
    ssat	  r1, #16, r1, asr #9      
    strh      r4, [r0], #128           @ s1 = buf[14] + buf[ 9]@
	ldr       r2, [r14, #24]      	   @ s = buf[ 6]@				d[0] = d[8] = s >> 7@	d += 64@
    strh      r1, [r0, #16]         
    strh      r1, [r0], #128           @ s1 = buf[22] + tmp@      
    ssat      r4, #16, r2, asr #9      
    add       r1, r5, r9               @ tmp = buf[26] + buf[30]@
    strh      r4, [r0, #16]        
    add       r2, r6, r1               @ s = buf[22] + tmp@			d[0] = d[8] = s >> 9@	d += 64@
    strh      r4, [r0], #128           @ s1 = buf[ 6]@
    
    ssat      r2, #16, r2, asr #9
    ldr       r9, [r14, #40]  		 
	ldr       r6, [r14, #72]  
    strh      r2, [r0, #16]       
	add       r4, r9, r8               @ s = buf[10] + buf[14]@		d[0] = d[8] = s >> 9@	d += 64
    strh      r2, [r0], #128           @ s1 = buf[22] + tmp@ 
    ssat	  r4, #16, r4, asr #9
	add       r1, r6, r1               @ s = buf[18] + tmp@			d[0] = d[8] = s >> 9@	d += 64@
    strh      r4, [r0, #16]  
    ssat	  r1, #16, r1, asr #9      
    strh      r4, [r0], #128 		   @ s1 = buf[10] + buf[14]@	
	ldr       r2, [r14, #8]            @ s = buf[ 2]@				d[0] = d[8] = s >> 9@	d += 64@ 
    ldr       r8, [r14, #112]                 
    strh      r1, [r0, #16]            @ s1 = buf[18] + tmp@	 
    strh      r1, [r0], #128      
    ssat	  r2, #16, r2, asr #9
    add       r1, r8, r5               @ tmp = buf[28] + buf[26]@	   
    ldr       r5, [r14, #48]    
    ldr       r7, [r14, #80]
    strh      r2, [r0, #16]        
    add       r4, r6, r1		       @ s = buf[18] + tmp@			d[0] = d[8] = s >> 9@	d += 64@
    strh      r2, [r0], #128   		   @ s1 = buf[ 2]@
    
    ssat	  r4, #16, r4, asr #9
    add       r2, r5, r9                @ s = buf[12] + buf[10]@		d[0] = d[8] = s >> 9@	d += 64@
    strh      r4, [r0, #16]   
    ssat	  r2, #16, r2, asr #9
    strh      r4, [r0], #128       		@ s1 = buf[18] + tmp@
    add       r1, r1, r7                @ s = buf[20] + tmp@			d[0] = d[8] = s >> 9@	d += 64@
    strh      r2, [r0, #16]   
    ssat	  r1, #16, r1, asr #9
    strh      r2, [r0], #128   			@ s1 = buf[12] + buf[10]@
    strh      r1, [r0, #16]   
    strh      r1, [r0], #128   			@ s1 = buf[20] + tmp@
    ldr       r2, [r14, #16]    
    ldr       r3, [r14, #96]  		
    ssat      r4, #16, r2, asr #9       @ s = buf[ 4]@				d[0] = d[8] = s >> 9@	d += 64@
    add       r1, r3, r8                @ tmp = buf[24] + buf[28]@
    strh      r4, [r0, #16]   
    add       r2, r7, r1                @ s = buf[20] + tmp@			d[0] = d[8] = s >> 9@	d += 64@
    strh      r4, [r0], #128 			@ s1 = buf[ 4]@
    
    ssat	  r2, #16, r2, asr #9
    ldr       r6, [r14, #32]  
    ldr       r3, [r14, #64]      
    strh      r2, [r0, #16]   
    add       r4, r6, r5		    	@ s = buf[ 8] + buf[12]@		d[0] = d[8] = s >> 9@	d += 64@
    strh      r2, [r0], #128   
    ssat	  r4, #16, r4, asr #9 	
  	add       r1, r3, r1   				@ s = buf[16] + tmp@			d[0] = d[8] = s >> 9@
    strh      r4, [r0, #16]   
    ssat	  r1, #16, r1, asr #9  
    strh      r4, [r0], #128        
    strh      r1, [r0, #16]   
    strh      r1, [r0]   


	ldmia     sp!, {r4 - r11, pc}
L664:
	.word       0x5a82799a

DCT32_Idx:
        .word       _dctIdx

DCT32_Tab:
        .word       _dcttab 

	@.ENd

