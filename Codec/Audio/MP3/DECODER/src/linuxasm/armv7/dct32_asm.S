#include "voMP3DecID.h"
	.section .text
	.global	voMP3DecDCT32
dctIdx:
	.word	4	
	.word	2
	.word	2
	.word	1
	.word	1	
	.word	0
	.word	0
	.word	0
	.word	0	
	.word	1
	.word	1
	.word	3
voMP3DecDCT32:	@PROC
	stmdb     sp!, {r4 - r11, lr}
M660:
	mov			   r14, r0
	ldr		  	   r6 , L664
	ADR			   r12, dcttab @L664 + 4
	ADR			   r11, dctIdx @L664 + 8
	
	@first pass   
	VLD1.32			{D0, D1, D2, D3}, [r14]!						@ a0
	VLD1.32			{D4, D5, D6, D7}, [r14]!						@ a1	
	VLD1.32			{D8, D9, D10, D11}, [r14]!						@ a2
	VREV64.32		Q2, Q2	
	VLD1.32			{D12, D13, D14, D15}, [r14]!					@ a3
	VREV64.32		Q3, Q3	
	VREV64.32		Q6, Q6
	VREV64.32		Q7, Q7
	
	VSWP			D4, D5
	VSWP			D6, D7
	VSWP			D12, D13
	VSWP			D14, D15
	
	VLD3.32   		{D16, D18, D20}, [r12]!
	VSUB.S32		Q12, Q0, Q7										@ a0 - a3	
	VLD3.32   		{D17, D19, D21}, [r12]!	
	VADD.S32		Q11, Q0, Q7										@ b0 = a0 + a3@
	
	VLD1.32			{D30, D31}, 	[r11]!
	VSUB.S32		Q7, Q3, Q4										@ a1 - a2	
	VQDMULH.S32		Q12, Q12, Q8									@ b3 = MUL_32(*cptr++, a0 - a3) << (s0)@
	
	VADD.S32		Q0, Q3, Q4										@ b1 = a1 + a2@	
	VQDMULH.S32		Q7, Q7, Q9										@ b2 = MUL_32(*cptr++, a1 - a2) << (s1)@
	
	VSUB.S32		Q3, Q11, Q0										@ b0 - b1
	VSHL.S32		Q7, Q7, Q15	
	
	VADD.S32		Q4, Q11, Q0										@ buf[i] = b0 + b1@
	VSUB.S32		Q11, Q12, Q7									@ b3 - b2
	VQDMULH.S32		Q3, Q3, Q10										@ buf[15-i] = MUL_32(*cptr,   b0 - b1) << (s2)@
	
	VADD.S32		Q0, Q12, Q7										@ buf[16+i] = b2 + b3@
	VSUB.S32		Q12, Q1, Q6										@ a0 - a3	
	
	VQDMULH.S32		Q7, Q11, Q10									@ buf[31-i] = MUL_32(*cptr++, b3 - b2) << (s2)@
	VADD.S32		Q13, Q1, Q6										@ b0 = a0 + a3@ 
	
	VLD3.32   		{D16, D18, D20}, [r12]!
	VSUB.S32		Q1, Q2, Q5										@ a1 - a2		
	
	VLD3.32   		{D17, D19, D21}, [r12]!	
	VADD.S32		Q6, Q2, Q5										@ b1 = a1 + a2@		
	
	VLD1.32			{D30, D31}, 	[r11]!
	VQDMULH.S32		Q1, Q1, Q9										@ b2 = MUL_32(*cptr++, a1 - a2) << (s1)@
	
	VQDMULH.S32		Q12, Q12, Q8									@ b3 = MUL_32(*cptr++, a0 - a3) << (s0)@
	VSHL.S32		Q1, Q1, Q15
	
	VSUB.S32		Q5, Q13, Q6										@ b0 - b1
	VLD1.32			{D30, D31}, 	[r11]!

	VADD.S32		Q2, Q13, Q6										@ buf[i] = b0 + b1@ 
	VREV64.32		Q3, Q3
	VQDMULH.S32		Q5, Q5, Q10										@ buf[15-i] = MUL_32(*cptr,   b0 - b1) << (s2)@
	
	VREV64.32		Q7, Q7
	VSUB.S32		Q11, Q12, Q1									@ b3 - b2
	VADD.S32		Q6, Q12, Q1										@ buf[16+i] = b2 + b3@	

	VSWP			D6, D7
	VQDMULH.S32		Q1, Q11, Q10									@ buf[31-i] = MUL_32(*cptr++, b3 - b2) << (s2)@  
	VSHL.S32		Q5, Q5, Q15										@ buf[15-i]
	VSWP			D14, D15
	VSHL.S32		Q1, Q1, Q15										@ buf[31-i] 
	
	VREV64.32		Q5, Q5
	VREV64.32		Q1, Q1
	
	VSWP			D10, D11
	VSWP			D2, D3
	
	VTRN.32			Q4, Q5
	VTRN.32			Q2, Q3
	
	VTRN.32			Q0, Q1
	VTRN.32			Q6, Q7
	
	VLD3.32   		{D16, D18, D20}, [r12]!
	VSWP			D9, D0
	VLD3.32   		{D17, D19, D21}, [r12]!	
	VSWP			D11, D2
	VSWP			D5, D12
	VSWP			D7, D14

   @ second pass	
	VSUB.S32		Q11, Q4, Q7										@ a0 - a7
	VSUB.S32		Q13, Q1, Q2										@ a3 - a4
	
	VADD.S32		Q12, Q4, Q7										@ b0 = a0 + a7@		
	VADD.S32		Q14, Q1, Q2										@ b3 = a3 + a4
	
	VQDMULH.S32		Q13, Q13, Q9									@ b4 = MUL_32(*cptr++, a3 - a4) << 3@	
	VSUB.S32		Q4, Q12, Q14									@ b0 - b3
	
	VQDMULH.S32		Q11, Q11, Q8									@ b7 = MUL_32(*cptr++, a0 - a7) << 1@ 
	VADD.S32		Q7, Q12, Q14									@ a0 = b0 + b3@
	
	VSHL.S32		Q13, Q13, #2
	VQDMULH.S32		Q4, Q4, Q10										@ a3 = MUL_32(*cptr,   b0 - b3) << 1@
	
	VSUB.S32		Q1,	Q11, Q13									@ b7 - b4					
	VADD.S32		Q14, Q0, Q3										@ b2 = a2 + a5@
	
	VADD.S32		Q2, Q11, Q13									@ a4 = b4 + b7@
	VQDMULH.S32		Q1, Q1, Q10										@ a7 = MUL_32(*cptr++, b7 - b4) << 1@
	
	VSUB.S32		Q11, Q5, Q6										@ a1 - a6
	VLD3.32   		{D16, D18, D20}, [r12]!
	VADD.S32		Q12, Q5, Q6										@ b1 = a1 + a6@		
	VLD3.32   		{D17, D19, D21}, [r12]!
	
	VSUB.S32		Q13, Q0, Q3										@ a2 - a5	
	VQDMULH.S32		Q11, Q11, Q8									@ b6 = MUL_32(*cptr++, a1 - a6) << 1@
	
	VQDMULH.S32		Q13, Q13, Q9									@ b5 = MUL_32(*cptr++, a2 - a5) << 1@
	
	VSUB.S32		Q5, Q12, Q14									@ b1 - b2
	VSUB.S32		Q0, Q11, Q13									@ b6 - b5
	
	VADD.S32		Q6, Q12, Q14									@ a1 = b1 + b2@	
	VQDMULH.S32		Q5, Q5, Q10										@ a2 = MUL_32(*cptr,   b1 - b2) << 2@
	

	VADD.S32		Q3, Q11, Q13									@ a5 = b5 + b6@	
	VQDMULH.S32		Q0, Q0, Q10										@ a6 = MUL_32(*cptr++, b6 - b5) << 2@
	
	VSHL.S32		Q5, Q5, #1
	VDUP.32			Q15, r6
	VSHL.S32		Q0, Q0, #1
	
	VSUB.S32		Q11, Q7, Q6										@ a0 - a1
	VSUB.S32		Q13, Q4, Q5										@ a3 - a2
	
	VADD.S32		Q10, Q7, Q6										@ b0 = a0 + a1@
	VQDMULH.S32		Q11, Q11, Q15									@ b1 = MUL_32(COS4_0, a0 - a1) << 1@
	
	VADD.S32		Q12, Q4, Q5										@ b2 = a2 + a3@
	VQDMULH.S32		Q13, Q13, Q15									@ b3 = MUL_32(COS4_0, a3 - a2) << 1@
	
	VSUB.S32		Q5, Q2, Q3 										@ a4 - a5
	VSUB.S32		Q7, Q1, Q0										@ a7 - a6

	VADD.S32		Q4, Q2, Q3										@ b4 = a4 + a5@
	VQDMULH.S32		Q8, Q5, Q15										@ b5 = MUL_32(COS4_0, a4 - a5) << 1@
	
	VADD.S32		Q6, Q0, Q1										@ b6 = a6 + a7@
	VQDMULH.S32		Q7, Q7, Q15										@ b7 = MUL_32(COS4_0, a7 - a6) << 1@
	
	VADD.S32		Q12, Q12, Q13
	VTRN.32			Q10, Q11
	
	VADD.S32		Q5, Q8, Q7
	VADD.S32		Q6, Q6, Q7
	
	VTRN.32			Q12, Q13
	VADD.S32		Q4, Q4, Q6
	
	VSWP			D21, D24
	VADD.S32		Q6, Q8, Q6
	
	VSWP			D23, d26
	VTRN.32			Q4, Q5
	VTRN.32			Q6, Q7
	
	mov				r14, r0	
	
	VSWP			D9, D12
	VSWP			D11, D14	

	VST1.32			{Q10}, [r0]!
	VST1.32			{Q4}, [r0]!
	VST1.32			{Q11}, [r0]!
	VST1.32			{Q5}, [r0]!
	VST1.32			{Q12}, [r0]!
	VST1.32			{Q6}, [r0]!
	VST1.32			{Q13}, [r0]!
	VST1.32			{Q7}, [r0]!	

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
dcttab:
	.word	0x4013c251
	.word	0x518522fb
	.word	0x404f4672
	.word	0x40b345bd
	.word	0x6d0b20cf
	.word	0x42e13c10
	.word	0x41fa2d6d
	.word	0x41d95790
	.word	0x48919f44
	.word	0x43f93421
	.word	0x5efc8d96
	.word	0x52cb0e63
	.word	0x46cc1bc4
	.word	0x4ad81a97
	.word	0x64e2402e
	.word	0x4a9d9cf0
	.word	0x7c7d1db3
	.word	0x43e224a9
	.word	0x4fae3711
	.word	0x6b6fcf26
	.word	0x6e3c92c1
	.word	0x56601ea7
	.word	0x5f4cf6eb
	.word	0x519e4e04
	.word	0x4140fb46
	.word	0x52036742
	.word	0x4545e9ef
	.word	0xbebf04ba
	.word	0xadfc98be
	.word	0x4545e9ef
	.word	0x4140fb46
	.word	0x52036742
	.word	0x4545e9ef
	.word	0xbebf04ba
	.word	0xadfc98be
	.word	0x4545e9ef
	.word	0x4cf8de88
	.word	0x73326bbf
	.word	0x539eba45
	.word	0xb3072178
	.word	0x8ccd9441
	.word	0x539eba45
	.word	0x4cf8de88
	.word	0x73326bbf
	.word	0x539eba45
	.word	0xb3072178
	.word	0x8ccd9441
	.word	0x539eba45	
L664:
	.word       0x5a82799a


		


	.END

