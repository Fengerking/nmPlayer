@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2011 					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

	.global	Idct4x4Add_ARMV6
	.global	Idct8x8Add_ARMV6
	.global  Idct4x4DCAdd_ARMV6
	
 MPR_STRIDE =	 16

 .align 8

Idct4x4DCAdd_ARMV6_old:
@r0  dc
@r1  imgbuf1
@r2  dst_stride 
@                mov      r3,#3                                
@ loop:          pld	 [r1, #4]
@                ldrb     r12,[r1,#0]                          
@                subs     r3,r3,#1                             
@                add      r12,r12,r0                           
@                usat     r12,#8,r12                           
@                strb     r12,[r1,#0]                          
@                ldrb     r12,[r1,#1]                          
@                add      r12,r12,r0                           
@                usat     r12,#8,r12                           
@                strb     r12,[r1,#1]                          
@                ldrb     r12,[r1,#2]                          
@                add      r12,r12,r0                           
@                usat     r12,#8,r12                           
@                strb     r12,[r1,#2]                          
@                ldrb     r12,[r1,#3]                          
@                add      r12,r12,r0                           
@                usat     r12,#8,r12                           
@                strb     r12,[r1,#3]                          
@                add      r1,r1,r2                             
@                bpl      loop                
@                bx      lr  

                pld	 [r1, #4]
                ldrb     r12,[r1,#0]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#0]                          
                ldrb     r12,[r1,#1]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#1]                          
                ldrb     r12,[r1,#2]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#2]                          
                ldrb     r12,[r1,#3]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#3]                          
                add      r1,r1,r2

                pld	 [r1, #4]
                ldrb     r12,[r1,#0]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#0]                          
                ldrb     r12,[r1,#1]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#1]                          
                ldrb     r12,[r1,#2]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#2]                          
                ldrb     r12,[r1,#3]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#3]                          
                add      r1,r1,r2

                pld	 [r1, #4]
                ldrb     r12,[r1,#0]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#0]                          
                ldrb     r12,[r1,#1]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#1]                          
                ldrb     r12,[r1,#2]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#2]                          
                ldrb     r12,[r1,#3]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#3]                          
                add      r1,r1,r2

                pld	 [r1, #4]
                ldrb     r12,[r1,#0]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#0]                          
                ldrb     r12,[r1,#1]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#1]                          
                ldrb     r12,[r1,#2]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#2]                          
                ldrb     r12,[r1,#3]                          
                add      r12,r12,r0                           
                usat     r12,#8,r12                           
                strb     r12,[r1,#3]
                add      r1,r1,r2
                
                bx       lr

Idct4x4DCAdd_ARMV6:
@r0  dc
@r1  imgbuf1
@r2  dst_stride 
                stmfd   sp!, {r0-r11, lr}
                 
                ldr     r3, =0x00010001
                ldr     r10, =0xFFFF
@                mov     r5, #0
                and     r0, r0, r10
                mul     r0, r0, r3

                ldr     r4, [r1]
                uxtb16  r6, r4
                uxtb16  r7, r4, ror #8
                sadd16  r6, r6, r0
                sadd16  r7, r7, r0
                ldr     r5, [r1, r2]
                usat16  r6, #8, r6
                usat16  r7, #8, r7
                orr     r6, r6, r7, lsl #8
                str     r6, [r1], r2

@                ldr     r5, [r1]
                uxtb16  r8, r5
                uxtb16  r9, r5, ror #8
                sadd16  r8, r8, r0
                sadd16  r9, r9, r0
                usat16  r8, #8, r8
                usat16  r9, #8, r9
                orr     r8, r8, r9, lsl #8
                str     r8, [r1], r2


                ldr     r4, [r1]
                uxtb16  r6, r4
                uxtb16  r7, r4, ror #8
                sadd16  r6, r6, r0
                sadd16  r7, r7, r0
                ldr     r5, [r1, r2]               
                usat16  r6, #8, r6
                usat16  r7, #8, r7
                orr     r6, r6, r7, lsl #8
                str     r6, [r1], r2
                
                uxtb16  r8, r5
                uxtb16  r9, r5, ror #8
                sadd16  r8, r8, r0
                sadd16  r9, r9, r0
                usat16  r8, #8, r8
                usat16  r9, #8, r9
                orr     r8, r8, r9, lsl #8
                str     r8, [r1]     
                
                add     sp,sp,#0x10
                ldmfd   sp!, {r4-r11, pc}           

  
                
  
 
Idct4x4Add_ARMV6_10bit:
@r0  tblock
@r1  imgbuf1
@r2  dst_stride 
              stmfd   r13!, {r3-r11, r14} @                                                
              sub     r13, r13, #64                                                        
@horizontal0                                                                               
              pld            [r0, #8]                                                      
              ldrsh          r4,[r0],#2                                                    
              ldrsh          r5,[r0],#2                                                    
              ldrsh          r6,[r0],#2                                                    
              ldrsh          r7,[r0],#2                                                    
                                                                                           
              add            r8,r4,r6                                                      
              sub            r9,r4,r6                                                      
              rsb            r10,r7,r5,asr #1                                              
              add            r11,r5,r7,asr #1                                              
                                                                                           
              add            r4,r8,r11                                                     
              add            r5,r9,r10                                                     
              sub            r6,r9,r10                                                     
              sub            r7,r8,r11                                                     
                                                                                           
                                                                                           
              strd           r4,[r13, #0]                                                  
              @str           r5,[r13, #4]                                                  
              strd           r6,[r13, #8]                                                  
              @str           r7,[r13, #12]                                                 
                                                                                           
 @horizontal1      
              pld            [r0, #8]   
              ldrsh          r4,[r0],#2                                                    
              ldrsh          r5,[r0],#2                                                    
              ldrsh          r6,[r0],#2                                                    
              ldrsh          r7,[r0],#2                                                    
                                                                                           
              add            r8,r4,r6                                                      
              sub            r9,r4,r6                                                      
              rsb            r10,r7,r5,asr #1                                              
              add            r11,r5,r7,asr #1                                              
                                                                                           
              add            r4,r8,r11                                                     
              add            r5,r9,r10                                                     
              sub            r6,r9,r10                                                     
              sub            r7,r8,r11                                                     
                                                                                           
                                                                                           
              strd           r4,[r13, #16]                                                 
              @str           r5,[r13, #20]                                                 
              strd           r6,[r13, #24]                                                 
              @str           r7,[r13, #28]                                                 
                                                                                           
 @horizontal2                                                
              pld            [r0, #8] 
              ldrsh          r4,[r0],#2                                                    
              ldrsh          r5,[r0],#2                                                    
              ldrsh          r6,[r0],#2                                                    
              ldrsh          r7,[r0],#2                                                    
                                                                                           
              add            r8,r4,r6                                                      
              sub            r9,r4,r6                                                      
              rsb            r10,r7,r5,asr #1                                              
              add            r11,r5,r7,asr #1                                              
                                                                                           
              add            r4,r8,r11                                                     
              add            r5,r9,r10                                                     
              sub            r6,r9,r10                                                     
              sub            r7,r8,r11                                                     
                                                                                           
                                                                                           
              strd           r4,[r13, #32]                                                 
              @str           r5,[r13, #36]                                                 
              strd           r6,[r13, #40]                                                 
              @str           r7,[r13, #44]                                                 
                                                                                           
 @horizontal3     
              pld            [r0, #8] 
              ldrsh          r4,[r0],#2                                                    
              ldrsh          r5,[r0],#2                                                    
              ldrsh          r6,[r0],#2                                                    
              ldrsh          r7,[r0],#2                                                    
                                                                                           
              add            r8,r4,r6                                                      
              sub            r9,r4,r6                                                      
              rsb            r10,r7,r5,asr #1                                              
              add            r11,r5,r7,asr #1                                              
                                                                                           
              add            r4,r8,r11                                                     
              add            r5,r9,r10                                                     
              sub            r6,r9,r10                                                     
              sub            r7,r8,r11                                                     
                                                                                           
                                                                                           
              strd           r4,[r13, #48]                                                 
              @str           r5,[r13, #52]                                                 
              strd           r6,[r13, #56]                                                 
              @str           r7,[r13, #60]                                                 
                                                                                           
@vertical0                                                                                 
              add           r3, r2, r2, lsl#1                                              
                                                                                           
              ldr           r4,[r13, #0]                                                   
              ldr           r5,[r13, #16]                                                  
              ldr           r6,[r13, #32]                                                  
              ldr           r7,[r13, #48]                                                  
                                                                                           
              add            r4, r4, #32                                                   
              add            r8,r4,r6                                                      
              sub            r9,r4,r6                                                      
              rsb            r10,r7,r5,asr #1                                              
              add            r11,r5,r7,asr #1                                              
                                                                                           
              add            r4,r8,r11                                                     
              ldrb           r5, [r1, #0]                                                  
              add            r4,r5,r4,asr #6                                               
              usat           r4, #8, r4                                                    
              strb           r4, [r1, #0]                                                  
                                                                                           
              add            r6,r9,r10                                                     
              ldrb           r7, [r1, r2]                                                  
              add            r6,r7,r6,asr #6                                               
              usat           r6, #8, r6                                                    
              strb           r6, [r1, r2]                                                  
                                                                                           
              sub            r4,r9,r10                                                     
              ldrb           r5, [r1, r2,lsl #1]                                           
              add            r4,r5,r4,asr #6                                               
              usat           r4, #8, r4                                                    
              strb           r4, [r1, r2,lsl #1]                                           
                                                                                           
              sub            r6,r8,r11                                                     
              ldrb           r7, [r1, r3]                                                  
              add            r6,r7,r6,asr #6                                               
              usat           r6, #8, r6                                                    
              strb           r6, [r1, r3]                                                  
                                                                                           
              add            r1, r1, #1                                                    
                                                                                           
@vertical1                                                                                 
              ldr           r4,[r13, #4]                                                   
              ldr           r5,[r13, #20]                                                  
              ldr           r6,[r13, #36]                                                  
              ldr           r7,[r13, #52]                                                  
                                                                                           
              add            r4, r4, #32                                                   
              add            r8,r4,r6                                                      
              sub            r9,r4,r6                                                      
              rsb            r10,r7,r5,asr #1                                              
              add            r11,r5,r7,asr #1                                              
                                                                                           
              add            r4,r8,r11                                                     
              ldrb           r5, [r1, #0]                                                  
              add            r4,r5,r4,asr #6                                               
              usat           r4, #8, r4                                                    
              strb           r4, [r1, #0]                                                  
                                                                                           
              add            r6,r9,r10                                                     
              ldrb           r7, [r1, r2]                                                  
              add            r6,r7,r6,asr #6                                               
              usat           r6, #8, r6                                                    
              strb           r6, [r1, r2]                                                  
                                                                                           
              sub            r4,r9,r10                                                     
              ldrb           r5, [r1, r2,lsl #1]                                           
              add            r4,r5,r4,asr #6                                               
              usat           r4, #8, r4                                                    
              strb           r4, [r1, r2,lsl #1]                                           
                                                                                           
              sub            r6,r8,r11                                                     
              ldrb           r7, [r1, r3]                                                  
              add            r6,r7,r6,asr #6                                               
              usat           r6, #8, r6                                                    
              strb           r6, [r1, r3]                                                  
                                                                                           
              add            r1, r1, #1                                                    
                                                                                           
 @vertical2                                                                                
              ldr           r4,[r13, #8]                                                   
              ldr           r5,[r13, #24]                                                  
              ldr           r6,[r13, #40]                                                  
              ldr           r7,[r13, #56]                                                  
                                                                                           
              add            r4, r4, #32                                                   
              add            r8,r4,r6                                                      
              sub            r9,r4,r6                                                      
              rsb            r10,r7,r5,asr #1                                              
              add            r11,r5,r7,asr #1                                              
                                                                                           
              add            r4,r8,r11                                                     
              ldrb           r5, [r1, #0]                                                  
              add            r4,r5,r4,asr #6                                               
              usat           r4, #8, r4                                                    
              strb           r4, [r1, #0]                                                  
                                                                                           
              add            r6,r9,r10                                                     
              ldrb           r7, [r1, r2]                                                  
              add            r6,r7,r6,asr #6                                               
              usat           r6, #8, r6                                                    
              strb           r6, [r1, r2]                                                  
                                                                                           
              sub            r4,r9,r10                                                     
              ldrb           r5, [r1, r2,lsl #1]                                           
              add            r4,r5,r4,asr #6                                               
              usat           r4, #8, r4                                                    
              strb           r4, [r1, r2,lsl #1]                                           
                                                                                           
              sub            r6,r8,r11                                                     
              ldrb           r7, [r1, r3]                                                  
              add            r6,r7,r6,asr #6                                               
              usat           r6, #8, r6                                                    
              strb           r6, [r1, r3]                                                  
                                                                                           
              add            r1, r1, #1                                                    
                                                                                           
@vertical3                                                                               
              ldr           r4,[r13, #12]                                                  
              ldr           r5,[r13, #28]                                                  
              ldr           r6,[r13, #44]                                                  
              ldr           r7,[r13, #60]                                                  
                                                                                           
              add            r4, r4, #32                                                   
              add            r8,r4,r6                                                      
              sub            r9,r4,r6                                                      
              rsb            r10,r7,r5,asr #1                                              
              add            r11,r5,r7,asr #1                                              
                                                                                           
              add            r4,r8,r11                                                     
              ldrb           r5, [r1, #0]                                                  
              add            r4,r5,r4,asr #6                                               
              usat           r4, #8, r4                                                    
              strb           r4, [r1, #0]                                                  
                                                                                           
              add            r6,r9,r10                                                     
              ldrb           r7, [r1, r2]                                                  
              add            r6,r7,r6,asr #6                                               
              usat           r6, #8, r6                                                    
              strb           r6, [r1, r2]                                                  
                                                                                           
              sub            r4,r9,r10                                                     
              ldrb           r5, [r1, r2,lsl #1]                                           
              add            r4,r5,r4,asr #6                                               
              usat           r4, #8, r4                                                    
              strb           r4, [r1, r2,lsl #1]                                           
                                                                                           
              sub            r6,r8,r11                                                     
              ldrb           r7, [r1, r3]                                                  
              add            r6,r7,r6,asr #6                                               
              usat           r6, #8, r6                                                    
              strb           r6, [r1, r3]                                                  
                                                                                           
              add            sp,sp,#0x40                                                   
              ldmfd          r13!, {r3-r11, r15} @  
              

Idct4x4Add_ARMV6:
@r0  tblock
@r1  imgbuf1
@r2  dst_stride 
        stmfd   r13!, {r4-r12, r14} @                                                            
        ldmia   r0, {r3-r10}

        mov     r0, #0
 @       ldr     r0, [r1,#-2]

        pkhtb   r11, r5, r3, asr #16  
        pkhbt   r3,  r3, r5, lsl #16 
        pkhtb   r12, r6, r4, asr #16
        pkhbt   r4,  r4, r6, lsl #16
        pkhtb   r5,  r9, r7, asr #16
        pkhbt   r7,  r7, r9, lsl #16
        pkhtb   r6,  r10, r8, asr #16
        pkhbt   r8,  r8, r10, lsl #16

        sadd16  r10,  r3, r4     @z0
        ssub16  r9,  r3, r4     @z1
        shadd16 r3,  r11, r0
        shadd16 r4,  r12, r0
        ssub16  r3, r3, r12     @z2
        sadd16  r4, r11, r4     @z3
        sadd16  r11, r10, r4    @z0 + z3  0 4 8  12  r11  0  1
        sadd16  r12, r9,  r3    @z1 + z2  1 5 9  13  r12  4  5
        ssub16  r3,  r9, r3     @z1 - z2  2 6 10 14  r3   8  9 
        ssub16  r4,  r10, r4    @z0 - z3  3 7 11 15  r4  12 13

        sadd16  r10,  r7, r8     @z0
        ssub16  r9,  r7, r8     @z1
        shadd16 r7,  r5, r0
        shadd16 r8,  r6, r0
        ssub16  r7, r7, r6     @z2
        sadd16  r8, r5, r8     @z3
        sadd16  r5, r10, r8    @z0 + z3    r5   2  3
        sadd16  r6, r9,  r7    @z1 + z2    r6   6  7
        ssub16  r7,  r9, r7     @z1 - z2   r7  10 11
        ssub16  r8,  r10, r8    @z0 - z3   r8  14 15

        pkhtb   r10, r3, r11, asr #16 @    r10  1 9
        pkhbt   r9,  r11, r3, lsl #16 @    r9   0 8
        pkhtb   r11, r7,  r5, asr #16 @    r11  3 11 
        pkhbt   r3,  r5,  r7, lsl #16 @    r3   2 10 
        pkhtb   r7, r4, r12, asr #16  @    r7   5 13
        pkhbt   r5,  r12, r4, lsl #16  @   r5   4 12
        pkhtb   r12,  r8, r6, asr #16 @    r12  7 15
        pkhbt   r4,  r6, r8, lsl #16 @     r4   6 14

        sadd16  r6, r9, r3   @z0
        ssub16  r8, r9, r3   @z1
        shadd16 r9, r10, r0
        shadd16 r3, r11, r0
        ssub16  r9, r9, r11   @z2
        sadd16  r3, r10, r3   @z3
        sadd16  r10, r6, r3    @ z0 + z3   0  1  2  3    r10  0  2  
        sadd16  r11, r8, r9     @ z1 + z2  4  5  6  7    r11  4  6
        ssub16  r9, r8, r9      @ z1 - z2  8  9 10 11    r9   8  10
        ssub16  r3, r6, r3    @ z0 - z3   12 13 14 15    r3  12  14

        sadd16  r6,  r5, r4  @z0
        ssub16  r8,  r5, r4  @z1
        shadd16 r4, r7, r0
        shadd16 r5, r12, r0
        ssub16  r12, r4, r12   @z2
        sadd16  r7, r7, r5   @z3
        sadd16  r4, r7,  r6 @z0 + z3      r4   1   3
        sadd16  r5, r8, r12  @z1 + z2     r5   5   7
        ssub16  r12, r8, r12  @z1 - z2    r12  9  11
        ssub16  r7, r6, r7 @z0 - z3       r7   13 15
        
        ldr     r0, =0x00208020
        ldr     r6, =0xffff03ff
        mov     r8, #0x200
        
        @first row
        sadd16  r10, r10, r0
        sadd16  r4, r4, r0
        and     r10, r6, r10, asr #6
        and     r4, r6, r4, asr #6
        ssub16  r10, r10, r8
        ssub16  r4, r4, r8
        
        @second row
        sadd16  r11, r11, r0
        sadd16  r5, r5, r0
        and     r11, r6, r11, asr #6
        and     r5, r6, r5, asr #6
        ssub16  r11, r11, r8
        ssub16  r5, r5, r8
        
        @three row
        sadd16  r9,  r9, r0
        sadd16  r12,  r12, r0
        and     r9, r6, r9, asr #6
        and     r12, r6, r12, asr #6
        ssub16  r9, r9, r8
        ssub16  r12, r12, r8
               
        @four row
        sadd16  r3,  r3, r0
        sadd16  r7,  r7, r0
        and     r3, r6, r3, asr #6
        and     r7, r6, r7, asr #6
        ssub16  r3, r3, r8
        ssub16  r7, r7, r8
               
        ldr     r6, =0xFF00FF
        
        @first row out
        ldr   r0, [r1]
        and   r8, r6, r0
        and   r14, r6, r0, lsr #8
              
        qadd16  r8, r8, r10
        qadd16  r14,r14, r4
        usat16  r8, #8, r8
        usat16  r14, #8, r14
        
        orr    r8, r8, r14, lsl #8      
        str   r8, [r1] 
 
        @second row out
        ldr   r0, [r1,r2]
        and   r8, r6, r0
        and   r14, r6, r0, lsr #8
              
        qadd16  r8, r8, r11
        qadd16  r14, r14, r5
        usat16  r8, #8, r8
        usat16  r14, #8, r14
        
        orr    r8, r8, r14, lsl #8      
        str   r8, [r1,r2] 
 
        @three row out
        ldr   r0, [r1, r2, lsl #1]
        and   r8, r6, r0
        and   r14, r6, r0, lsr #8
              
        qadd16  r8, r8, r9
        qadd16  r14, r14, r12
        usat16  r8, #8, r8
        usat16  r14, #8, r14
        
        orr    r8, r8, r14, lsl #8      
        str   r8, [r1, r2, lsl #1] 
 
        @four row out
        add   r1, r1, r2
        ldr   r0, [r1, r2, lsl #1]
        and   r8, r6, r0
        and   r14, r6, r0, lsr #8
              
        qadd16  r8, r8, r3
        qadd16  r14, r14, r7
        usat16  r8, #8, r8
        usat16  r14, #8, r14
        
        orr    r8, r8, r14, lsl #8      
        str   r8, [r1, r2, lsl #1]                 
                                                        
        ldmfd          r13!, {r4-r12, r15}     

 pTBLOCK = 0x00
 pImgBuf = 0x04
 pStride = 0x08
 pStep   = 0x0C

Idct8x8Add_ARMV6:
@r0  tblock
@r1  imgbuf1
@r2  dst_stride 
    MOV    r3, #7
    STMFD   sp!, {r0-r11, lr}

v6_idct_hor:
    ldmia   r0, {r3-r10}

    mov     r0, #0
 @       ldr     r0, [r1,#-2]
    pkhtb   r11, r7, r3, asr #16 @ n_t9   | n_t1 
    pkhbt   r3,  r3, r7, lsl #16 @ n_t8   | n_t0
    pkhtb   r12, r8, r4, asr #16 @ n_t11 | n_t3 
    pkhbt   r4,  r4, r8, lsl #16 @ n_t10 | n_t2
    pkhbt   r7,  r5, r9, lsl #16 @ t12 | n_t4
    pkhtb   r5,  r9, r5, asr #16 @ t13 | n_t5   
    pkhbt   r8,  r6, r10, lsl #16 @t14 | n_t6  
    pkhtb   r6,  r10, r6, asr #16 @t15 | n_t7

    sadd16  r10,  r3, r7     @n_t0 + n_t4 =>  a0
    ssub16  r9,  r3, r7      @n_t0 - n_t4 =>  a1
    shadd16 r3,  r4, r0      @n_t2 >> 1
    shadd16 r14,  r8, r0      @n_t6 >> 1
    ssub16  r7, r8, r3       @n_t6 - (n_t2 >> 1) => a2
    sadd16  r4, r4, r14       @n_t2 + (n_t6 >> 1) => a3
    
    sadd16  r3, r10, r4     @b0 = a0 + a3
    ssub16  r8, r9,  r7     @b2 = a1 -a2
    sadd16  r14, r9,  r7     @b4 = a1 + a2
    ssub16  r4, r10, r4     @b6 = a0 - a3

    sadd16  r10,  r5, r12    @n_t5 + n_t3
    ssub16  r7,  r5, r12     @n_t5 - n_t3
    shadd16 r2,  r11, r0     @n_t1 >> 1 
    shadd16 r1,  r6, r0      @n_t7 >> 1 
    ssub16  r7, r7, r6       @-n_t3 + n_t5 - n_t7
    sadd16  r10, r10, r11    @n_t3 + n_t5 + n_t1
    ssub16  r7, r7, r1       @-n_t3 + n_t5 - n_t7 - (n_t7 >> 1) =>  a0
    sadd16  r10, r10, r2    @n_t3 + n_t5 + n_t1 + (n_t1 >> 1)  =>  a3

    sadd16  r2,  r6, r11    @n_t7 + n_t1
    ssub16  r1,  r6, r11    @n_t7 - n_t1
    shadd16 r11, r12, r0     @n_t3 >> 1 
    shadd16 r6,  r5, r0      @n_t5 >> 1 
    ssub16  r12, r2, r12     @n_t1 + n_t7 - n_t3
    sadd16  r5, r1, r5       @-n_t1 + n_t7 + n_t5
    ssub16  r12, r12, r11    @n_t1 + n_t7 - n_t3 -( n_t3 >> 1) =>  a1
    sadd16  r5, r5, r6       @-n_t1 + n_t7 + n_t5 + (n_t5 >> 1)  =>  a2

    shadd16 r1, r7, r0
    shadd16 r1, r1, r0
    shadd16 r2, r10, r0
    shadd16 r2, r2, r0
    shadd16 r6, r12, r0
    shadd16 r6, r6, r0
    shadd16 r11, r5, r0
    shadd16 r11, r11, r0

    sadd16 r2, r7, r2     @b1 = a0 + (a3>>2);    
    sadd16 r11, r12, r11  @b3 =  a1 + (a2>>2); 
    ssub16 r0,  r5, r6    @b5 =  a2 - (a1>>2);
    ssub16 r12, r10, r1   @b7 =  a3 - (a0>>2);

    ssub16 r10,r3, r12   @ b0 -b7
    sadd16 r3, r3, r12   @ b0 + b7
    sadd16 r6, r4, r2    @ b6 + b1
    ssub16 r7, r4, r2    @ b6 - b1    
    ssub16 r4, r8, r0    @ b2 - b5
    sadd16 r9, r8, r0    @ b2 + b5    
    sadd16 r5, r14, r11   @ b4 + b3
    ssub16 r8, r14, r11   @ b4 - b3

    ldr r0, [sp, #pTBLOCK]    @t00 n_t10 t01 n_t11 t02 t12 t03 t13
    ldr r2, [sp, #pStep]      @t04 t14 t05 t15 t06 t16 t07 t17
    subs r2, #2               @t20 t30 t21 t31 t22 t32 t23 t33
    STMIA r0!, {r3-r10}       @t24 t34 t25 t35 t26 t36 t27 t37
    str r2, [sp, #pStep]      @t40 t50 t41 t51 t42 t52 t43 t53
    str r0, [sp, #pTBLOCK]    @t44 t54 t45 t55 t46 t56 t47 t57
    bcs  v6_idct_hor          @t60 t70 t61 t71 t62 t72 t63 t73
                              @t64 t74 t65 t75 t66 t76 t67 t77
    sub r0, #0x80          
    mov  r2, #7           
    str r0, [sp, #pTBLOCK]
    str r2, [sp, #pStep]  
                          
v6_idct_ver:
    ldrd  r2 ,r3,  [r0], #32 @t00 n_t10 t01 n_t11
    ldrd  r4, r5,  [r0], #32 @t20 t30 t21 t31
    ldrd  r6, r7,  [r0], #32 @t40 t50 t41 t51
    ldrd  r8, r9,  [r0]      @t60 t70 t61 t71
    
    pkhtb  r11, r3, r2, asr #16 @ n_t11 | n_t10
    pkhbt  r3,  r2, r3, lsl #16 @ t01 | t00
    pkhtb  r12, r5, r4, asr #16
    pkhbt  r4,  r4, r5, lsl #16
    pkhtb  r5,  r7, r6, asr #16
    pkhbt  r7,  r6, r7, lsl #16
    pkhtb  r6,  r9, r8, asr #16
    pkhbt  r8,  r8, r9, lsl #16
    
    mov     r0, #0
 @       ldr     r0, [r1,#-2]

    sadd16  r10,  r3, r7     @n_t0 + n_t4 =>  a0
    ssub16  r9,  r3, r7      @n_t0 - n_t4 =>  a1
    shadd16 r3,  r4, r0      @n_t2 >> 1
    shadd16 r14,  r8, r0      @n_t6 >> 1
    ssub16  r7, r8, r3       @n_t6 - (n_t2 >> 1) => a2
    sadd16  r4, r4, r14       @n_t2 + (n_t6 >> 1) => a3
    
    sadd16  r3, r10, r4     @b0 = a0 + a3
    ssub16  r8, r9,  r7     @b2 = a1 -a2
    sadd16  r14, r9,  r7     @b4 = a1 + a2
    ssub16  r4, r10, r4     @b6 = a0 - a3

    sadd16  r10,  r5, r12    @n_t5 + n_t3
    ssub16  r7,  r5, r12     @n_t5 - n_t3
    shadd16 r2,  r11, r0     @n_t1 >> 1 
    shadd16 r1,  r6, r0      @n_t7 >> 1 
    ssub16  r7, r7, r6       @-n_t3 + n_t5 - n_t7
    sadd16  r10, r10, r11    @n_t3 + n_t5 + n_t1
    ssub16  r7, r7, r1       @-n_t3 + n_t5 - n_t7 - (n_t7 >> 1) =>  a0
    sadd16  r10, r10, r2    @n_t3 + n_t5 + n_t1 + (n_t1 >> 1)  =>  a3

    sadd16  r2,  r6, r11    @n_t7 + n_t1
    ssub16  r1,  r6, r11    @n_t7 - n_t1
    shadd16 r11, r12, r0     @n_t3 >> 1 
    shadd16 r6,  r5, r0      @n_t5 >> 1 
    ssub16  r12, r2, r12     @n_t1 + n_t7 - n_t3
    sadd16  r5, r1, r5       @-n_t1 + n_t7 + n_t5
    ssub16  r12, r12, r11    @n_t1 + n_t7 - n_t3 -( n_t3 >> 1) =>  a1
    sadd16  r5, r5, r6       @-n_t1 + n_t7 + n_t5 + (n_t5 >> 1)  =>  a2

    shadd16 r1, r7, r0
    shadd16 r1, r1, r0
    shadd16 r2, r10, r0
    shadd16 r2, r2, r0
    shadd16 r6, r12, r0
    shadd16 r6, r6, r0
    shadd16 r11, r5, r0
    shadd16 r11, r11, r0

    sadd16 r2, r7, r2     @b1 = a0 + (a3>>2);    
    sadd16 r11, r12, r11  @b3 =  a1 + (a2>>2); 
    ssub16 r0,  r5, r6    @b5 =  a2 - (a1>>2);
    ssub16 r12, r10, r1   @b7 =  a3 - (a0>>2);

    ssub16 r10,r3, r12   @ b0 -b7
    sadd16 r3, r3, r12   @ b0 + b7
    sadd16 r6, r4, r2    @ b6 + b1
    ssub16 r7, r4, r2    @ b6 - b1    
    ssub16 r4, r8, r0    @ b2 - b5
    sadd16 r9, r8, r0    @ b2 + b5    
    sadd16 r5, r14, r11   @ b4 + b3
    ssub16 r8, r14, r11   @ b4 - b3

    ldr     r0, =0x00208020
    ldr     r1, =0xffff03ff
    mov     r2, #0x200
    ldr     r11, [sp, #pImgBuf]
    ldr     r12, =0xFF00FF

@    next row 0          
    sadd16  r3, r3, r0
    ldrh    r14, [r11]  
    and     r3, r1, r3, asr #6
    ssub16  r3, r3, r2

  @  ldrh    r14, [r11]
    uxtab16 r14, r14, r14, ror #24 
    and     r14, r14, r12

    qadd16  r3, r3, r14
    usat16  r3, #8, r3
    ldr     r14, [sp, #pStride]    
    orr     r3, r3, r3, lsr #8
    strh    r3, [r11], r14

@    next row 1  
    sadd16  r4, r4, r0
    ldrh    r3, [r11]
    and     r4, r1, r4, asr #6    
    ssub16  r4, r4, r2
    
    uxtab16 r3, r3, r3, ror #24 
    and     r3, r3, r12

    qadd16  r4, r4, r3
    usat16  r4, #8, r4
    orr     r4, r4, r4, lsr #8
    strh    r4, [r11], r14
    
@    next row 2   
    sadd16  r5, r5, r0
    ldrh    r3, [r11]
    and     r5, r1, r5, asr #6
    ssub16  r5, r5, r2
    
    uxtab16 r3, r3, r3, ror #24 
    and     r3, r3, r12

    qadd16  r5, r5, r3
    usat16  r5, #8, r5
    orr     r5, r5, r5, lsr #8
    strh    r5, [r11], r14
    
@    next row 3    
    sadd16  r6, r6, r0
    ldrh    r3, [r11]
    and     r6, r1, r6, asr #6
    ssub16  r6, r6, r2
    
    uxtab16 r3, r3, r3, ror #24 
    and     r3, r3, r12

    qadd16  r6, r6, r3
    usat16  r6, #8, r6
    orr     r6, r6, r6, lsr #8
    strh    r6, [r11], r14
    
@    next row 4    
    sadd16  r7, r7, r0
    ldrh    r3, [r11]
    and     r7, r1, r7, asr #6
    ssub16  r7, r7, r2
    
    uxtab16 r3, r3, r3, ror #24 
    and     r3, r3, r12

    qadd16  r7, r7, r3
    usat16  r7, #8, r7
    orr     r7, r7, r7, lsr #8
    strh    r7, [r11], r14
    
@    next row 5   
    sadd16  r8, r8, r0
    ldrh    r3, [r11]
    and     r8, r1, r8, asr #6
    ssub16  r8, r8, r2
    
    uxtab16 r3, r3, r3, ror #24 
    and     r3, r3, r12

    qadd16  r8, r8, r3
    usat16  r8, #8, r8
    orr     r8, r8, r8, lsr #8
    strh    r8, [r11], r14
    
@    next row 6    
    sadd16  r9, r9, r0
    ldrh    r3, [r11]
    and     r9, r1, r9, asr #6
    ssub16  r9, r9, r2
    
    uxtab16 r3, r3, r3, ror #24 
    and     r3, r3, r12

    qadd16  r9, r9, r3
    usat16  r9, #8, r9
    orr     r9, r9, r9, lsr #8
    strh    r9, [r11], r14
    
@    next row 7    
    sadd16  r10, r10, r0
    ldrh    r3, [r11]
    and     r10, r1, r10, asr #6
    ssub16  r10, r10, r2

    uxtab16 r3, r3, r3, ror #24 
    and     r3, r3, r12

    qadd16  r10, r10, r3
    usat16  r10, #8, r10
    orr     r10, r10, r10, lsr #8
    strh    r10, [r11], r14
    
    sub r11, r14, lsl #3
    add r11, r11, #2
    ldr r0, [sp, #pTBLOCK]
    ldr r2, [sp, #pStep]
    subs r2, #2
    str r11, [sp, #pImgBuf]
    str r2, [sp, #pStep]
    add r0, r0, #8
    str r0, [sp, #pTBLOCK]
    bcs  v6_idct_ver  

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}      

avg_ARMV6:
 @r0 a
 @r1 b
 @r2 c
 @ (a + 2 * b + c + 2) >> 2
    STMFD   sp!, {r0-r11, lr}
    UHADD8  r2, r2, r0
    UHSUB8  r2, r1, r2
    USUB8   r0, r1, r2  

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}           
	      
  @.end

