;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;* File Name: NeonDeblockLuma.s
;*            
;*
;* Author: Number Huang
;r0:pSrcDst
;r1:srcdstStep
;r2:pAlpha
;r3:pBeta
;r4:pThresholds,//got from sp+4
;r5:pBS//got from sp+8
;****************************************************************

			EXPORT  NeonDeblockingChromaV_ASM     
           AREA    |.text|, CODE, READONLY
NeonDeblockingChromaV_ASM    PROC
;	int inc=srcdstStep;
;	int inc2 = (inc<<1);
;	int inc3 = inc2 + inc;
;	int i=1,j=0,str=0,edge=0;
;	avdUInt8 *pt2=NULL,*pt1	= pBS;
;	avdUInt8* srcY	= pSrcDst;
;	int srcInc=1;
;	avdUInt8* tc0=pThresholds;
;	avdUInt8* alpha2	=	pAlpha;
;	avdUInt8* beta2		=	pBeta;
;	avdUInt8* clip255 = &img->clip255[0];
;	int L0,L1,L2,R0,R1,R2,tmp1,tmp2,Beta,Alpha,Delta,AbsDelta,aq,ap,RL0,C0,c0,small_gap;


			STMFD     sp!, {r4-r11,lr};36 byte
 			MOV		  R11,#2							;r11 is loop count
 			LDR       R9,  [R2]					;r9=apha+1//inner alpha
 			LDR       R5,  [SP, #40];pt1
						
 			LDR       R4,  [SP, #36];tc0

 			LDR       R10, [R3]					;r10=beta+1//inner beta
 			
 			LDR       R7,  [R5];//, #0x8;r7=str[8]-str[11] 			
		
 			LDR       R12,  [R0, #4]					;r12
 			LDR       R0, [R0]					;r0 			
			AND		  R2,R9,#255			;alpha
			AND		  R3,R10,#255			;beta			
 			

										;r14:next_half flag
			CMP       	 R7, #0x0				;next edge if it is zero
 			BEQ		  NEXT_EDGE
	
BEGIN_EDGE			
;;;;;;;;;;;;;;;;;;;;;;;;;different bigain;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 			;ldr L1,L0,R0,R1
 			VDUP.32		Q15,R7											
 			SUB		R8,R0,#2
 			SUB		R6,R12,#2
 			;d1:L1,d2:L0,d3:R0,d4:R1
 			
 		    VLD4.8 {d6[0],d7[0],d8[0],d9[0]}, [R8],R1
 		    VLD4.8 {d6[1],d7[1],d8[1],d9[1]}, [R8],R1
 		    VLD4.8 {d6[2],d7[2],d8[2],d9[2]}, [R8],R1
 		    VLD4.8 {d6[3],d7[3],d8[3],d9[3]}, [R8],R1
 		    VLD4.8 {d6[4],d7[4],d8[4],d9[4]}, [R8],R1
 		    VLD4.8 {d6[5],d7[5],d8[5],d9[5]}, [R8],R1
 		    VLD4.8 {d6[6],d7[6],d8[6],d9[6]}, [R8],R1
 		    VLD4.8 {d6[7],d7[7],d8[7],d9[7]}, [R8]
			VZIP.8		D31,D30			;3
		    
 		    VLD4.8 {d13[0],d14[0],d15[0],d16[0]}, [R6],R1
 		    VLD4.8 {d13[1],d14[1],d15[1],d16[1]}, [R6],R1
 		    VLD4.8 {d13[2],d14[2],d15[2],d16[2]}, [R6],R1
 		    VLD4.8 {d13[3],d14[3],d15[3],d16[3]}, [R6],R1
 		    VLD4.8 {d13[4],d14[4],d15[4],d16[4]}, [R6],R1
 		    VLD4.8 {d13[5],d14[5],d15[5],d16[5]}, [R6],R1
 		    VLD4.8 {d13[6],d14[6],d15[6],d16[6]}, [R6],R1
 		    VLD4.8 {d13[7],d14[7],d15[7],d16[7]}, [R6]
			VCGT.s8		D30,D31,#0    		     		    
			vshr.s32 D10, D8, #0			
			vshr.s32 D8, D7, #0
			vshr.s32 D12, D9, #0									 		    

			vshr.s32 D7, D13, #0
			vshr.s32 D9, D14, #0
			vshr.s32 D11, D15, #0
			vshr.s32 D13, D16, #0									 		    
 			
;;;;;;;;;;;;;;;;;;;;;;;;;different end;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 			

			vshr.s64 D31, D30, #0
									
			VDUP.8	Q13,R2
 			;tmp1=R0-R1
			VABD.u8  	Q10,Q5,Q6												
 			VDUP.8	Q12,R3				
			;tmp2 = L0 - L1;
			VABD.u8  	Q11,Q4,Q3
			VSUB.s8  	Q14,Q5,Q4			
		
			;if((tmp1 >= Beta) || (tmp2 >= Beta)) coninue
			VCLT.u8	 	Q0,Q11,Q12;tmp2,tmp2,Beta_D
			VCLT.u8  	Q10,Q10,Q12;Flag,tmp1,Beta_D
			VAND.u8	 	Q0,Q0,Q15 
			VABS.s8	 	Q11,Q14	
			AND       R8, R7, #0xff						
			VAND.u8	 	Q0,Q0,Q10
			;Delta = R0 - L0;
			;if(abs(Delta) >= Alpha) continue
 		
			VCLT.u8	 	Q10,Q11,Q13			

 			CMP       R8, #0x4						

 			BEQ		STRTNGTH_4
		    ;tmp1		  = ((Delta << 2) + (L1 - R1) + 4)>>3;
 			ldr		R6,[r4];tc0
			VMOV.I16	  Q1,#4	
			VSHLL.s8		  Q12,D28,#2;(Delta << 2)
			VDUP.32		Q15,R6			
			VSHLL.s8		  Q13,D29,#2			 					    
			VAND.u8	 	Q0,Q0,Q10		    
 			
			VSUBL.u8	  Q10,D6,D12	;L1-R1
			VSUBL.u8	  Q11,D7,D13	;L1-R1			
			VZIP.8		D31,D30			;3								
				
			VADD.s16	  Q12,Q12,Q1;+4					
			VADD.s16	  Q13,Q13,Q1;+4
			VMOV.I8		  Q1,#1			
			vshr.s64 D30, D31, #0
			VADD.s16	  Q12,Q12,Q10;+(L1 - R1)
			VADD.s16	  Q13,Q13,Q11;+(L1 - R1)
			VADD.s8		Q1,Q1,Q15						
			VSHRN.s16	  D28,Q12,#3;>>3
			VSHRN.s16	  D29,Q13,#3;>>3		;now Q14 is tmp1	
			
			;c0 = (C0 + ap + aq) ; c0 = (tc0[j/2] + 1);	
 			;tmp1=CLIP3(-c,tmp1,c)
 			VMIN.s8		Q14,Q1,Q14
 			VNEG.s8		Q1,Q1
 			VMAX.s8		Q14,Q1,Q14
 			VAND.u8		Q1,Q0,Q14
			
			;L0  = clip255[(L0 + tmp1)];	
			VMOVL.s8	Q14,D2
			VMOVL.s8	Q15,D3			
			VADDW.u8 	Q14,Q14,D8	;L0_Q,L0_Q,tmp1_Q
			VADDW.u8 	Q15,Q15,D9	;L0_Q,L0_Q,tmp1_Q

			VQMOVUN.s16	D8,Q14
			VQMOVUN.s16	D9,Q15

			;R0  = clip255[(R0 - tmp1)];
			
			VMOVL.u8	Q14,D10
			VMOVL.u8	Q15,D11			
			VSUBW.s8 	Q14,Q14,D2	;L0_Q,L0_Q,tmp1_Q
			VSUBW.s8 	Q15,Q15,D3	;L0_Q,L0_Q,tmp1_Q
			VQMOVUN.s16	D10,Q14
			VQMOVUN.s16	D11,Q15

;;;;;;;;;;;;;;;;;;;;;;;;;different bigain;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			;store
 			SUB			R8,R0,#1
			VSWP.64		D9, D10 			
 			SUB			R6,R12,#1 			

 			VST2.8    {d8[0],d9[0]}, [R8],r1
 			VST2.8    {d8[1],d9[1]}, [R8],r1
 			VST2.8    {d8[2],d9[2]}, [R8],r1
 			VST2.8    {d8[3],d9[3]}, [R8],r1
 			VST2.8    {d8[4],d9[4]}, [R8],r1
 			VST2.8    {d8[5],d9[5]}, [R8],r1
 			VST2.8    {d8[6],d9[6]}, [R8],r1
 			VST2.8    {d8[7],d9[7]}, [R8]						;d1:L1,d2:L0,d3:R0,d4:R1
 			
 			VST2.8    {d10[0],d11[0]}, [R6],r1
 			VST2.8    {d10[1],d11[1]}, [R6],r1
 			VST2.8    {d10[2],d11[2]}, [R6],r1
 			VST2.8    {d10[3],d11[3]}, [R6],r1
 			VST2.8    {d10[4],d11[4]}, [R6],r1
 			VST2.8    {d10[5],d11[5]}, [R6],r1
 			VST2.8    {d10[6],d11[6]}, [R6],r1
 			VST2.8    {d10[7],d11[7]}, [R6]						;d1:L1,d2:L0,d3:R0,d4:R1
;;;;;;;;;;;;;;;;;;;;;;;;;different end;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 			B	NEXT_EDGE
STRTNGTH_4
			VMOV.I16	Q1,#2
			VADDL.u8  Q12,	D8,D12		;L0 + R1
			VADDL.u8  Q13,	D9,D13		;L0 + R1			
			VSHLL.u8	Q14,D6,#1	;L1<<1
			VSHLL.u8	Q15,D7,#1	;L1<<1				
			VAND.u8	 	Q0,Q0,Q10
;Q3 L1	;Q4 L0	;Q5 R0	;Q6 R1
;	pt2[-inc] = ((L1 << 1) + L0 + R1 + 2) >> 2; 	
			;+L1<<1							
			;L0 + R1 + 2
			VADD.S16	Q12,Q12,Q1		; + 2						
			VADD.S16	Q13,Q13,Q1		; + 2
			VSHLL.u8	Q10,D12,#1	;R1<<1			
			VADD.s16	Q12,Q12,Q14
			VADD.s16	Q13,Q13,Q15
			VSHLL.u8	Q15,D13,#1	;R1<<1				
			;((L1 << 1) + L0 + R1 + 2) >> 2
			VSHRN.s16	  D28,Q12,#2;>>2
			VADDL.u8  Q12,	D10,D6		;R0 + L1			
			VSHRN.s16	  D29,Q13,#2;>>2	;Q14
;	pt2[   0] = ((R1 << 1) + R0 + L1 + 2) >> 2;
			;+R1<<1				
			VADDL.u8  Q13,	D11,D7		;R0 + L1
			;R0 + L1 + 2
			VADD.S16	Q12,Q12,Q1		; + 2						
			VADD.S16	Q13,Q13,Q1		; + 2			
			VADD.s16	Q12,Q12,Q10
			VADD.s16	Q13,Q13,Q15
			VCEQ.u8		Q10,Q0,#0;tmp6_D,Flag,#0			
			;((R1 << 1) + R0 + L1 + 2) >> 2
			VSHRN.s16	  D30,Q12,#2;>>2
			VAND.s8		Q12,Q14,Q0			
			VSHRN.s16	  D31,Q13,#2;>>2	;Q15

			VAND.s8		Q13,Q4,Q10
			VORR.s8		Q4,Q12,Q13;	now Q4 is last L0
			
			VAND.s8		Q12,Q15,Q0
			VAND.s8		Q13,Q5,Q10
			VORR.s8		Q5,Q12,Q13;	now Q5 is last R0									
;;;;;;;;;;;;;;;;;;;;;;;;;different bigain;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
                                                                        
 			SUB			R8,R0,#1  
			VSWP.64		D9, D10  			                                                              
 			SUB			R6,R12,#1 			                                        
 			VST2.8    {d8[0],d9[0]}, [R8],r1                                                                
 			VST2.8    {d8[1],d9[1]}, [R8],r1                                                                
 			VST2.8    {d8[2],d9[2]}, [R8],r1                                                                
 			VST2.8    {d8[3],d9[3]}, [R8],r1                                                                
 			VST2.8    {d8[4],d9[4]}, [R8],r1                                                                
 			VST2.8    {d8[5],d9[5]}, [R8],r1                                                                
 			VST2.8    {d8[6],d9[6]}, [R8],r1                                                                
 			VST2.8    {d8[7],d9[7]}, [R8]						;d1:L1,d2:L0,d3:R0,d4:R1
 			                                                                                                
 			VST2.8    {d10[0],d11[0]}, [R6],r1                                                              
 			VST2.8    {d10[1],d11[1]}, [R6],r1                                                              
 			VST2.8    {d10[2],d11[2]}, [R6],r1                                                              
 			VST2.8    {d10[3],d11[3]}, [R6],r1                                                              
 			VST2.8    {d10[4],d11[4]}, [R6],r1                                                              
 			VST2.8    {d10[5],d11[5]}, [R6],r1                                                              
 			VST2.8    {d10[6],d11[6]}, [R6],r1                                                              
 			VST2.8    {d10[7],d11[7]}, [R6]						;d1:L1,d2:L0,d3:R0,d4:R1
			
NEXT_EDGE
			SUBS	R11,R11,#1
			BEQ		NeonDeblockChromaH_END 	
			ADD		R5,R5,#8
			ADD		r0,r0,#4
			ADD		r12,r12,#4
;;;;;;;;;;;;;;;;;;;;;;;;;different end;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			LDR	   	R7,[R5];

			ADD		r4,r4,#4;tc0
			
			
			LSR    		R2,R9,#8
			LSR		R3,R10,#8
			
 			CMP     R7, #0x0
 			BNE		BEGIN_EDGE	
					
NeonDeblockChromaH_END			
 			LDMFD          sp!, {r4 - r11,pc}  
			ENDP
			END    
