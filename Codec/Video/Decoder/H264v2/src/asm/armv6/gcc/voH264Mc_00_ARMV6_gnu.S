@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

    .global  get_luma_00_16_ARMV6
    .global  get_luma_00_8_ARMV6
    .global  get_luma_00_4_ARMV6
    .global  add_luma_00_ARMV6

    .global  get_luma_00_16x16_ARMV6
    .global  get_luma_00_16x8_ARMV6
    .global  get_luma_00_8x16_ARMV6
    .global  get_luma_00_8x8_ARMV6
    .global  get_luma_00_8x4_ARMV6
    .global  get_luma_00_4x8_ARMV6
    .global  get_luma_00_4x4_ARMV6
    .global  wp_16_armv6
    .global  wp_8_armv6
    .global  wp_4_armv6
    .global  wp_2_armv6    
    .global  of_16_armv6
    .global  of_8_armv6
    .global  of_4_armv6
    .global  of_2_armv6
    .global  wbi_16_armv6
    .global  wbi_8_armv6
    .global  wbi_4_armv6
    .global  wbi_2_armv6
    .global  bi_16_armv6
    .global  bi_8_armv6
    .global  bi_4_armv6
    .global  bi_2_armv6
    


	.align 8

 pSrc           .req r0
 pSrc_stride     .req r1

 pDst            .req r2
 pDst_stride     .req r3

 n_partW          .req r6
 n_partH          .req r7

@// Declare other intermediate registers
 n_x0             .req r4
 n_x1             .req r5
 n_x2             .req r8
 n_x3             .req r9
 Temp           .req r12

@extern void get_luma_00_16_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_16_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align016
     B       Align116
     B       Align216
     B       Align316
Align016:
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x1, [pSrc], pSrc_stride
     STR   n_x0, [pDst], pDst_stride
     LDR   n_x2, [pSrc], pSrc_stride
     STR   n_x1, [pDst], pDst_stride
     LDR   n_x3, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     STR   n_x3, [pDst], pDst_stride
     
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x1, [pSrc], pSrc_stride
     STR   n_x0, [pDst], pDst_stride
     LDR   n_x2, [pSrc], pSrc_stride
     STR   n_x1, [pDst], pDst_stride
     LDR   n_x3, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     STR   n_x3, [pDst], pDst_stride

     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x1, [pSrc], pSrc_stride
     STR   n_x0, [pDst], pDst_stride
     LDR   n_x2, [pSrc], pSrc_stride
     STR   n_x1, [pDst], pDst_stride
     LDR   n_x3, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     STR   n_x3, [pDst], pDst_stride

     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x1, [pSrc], pSrc_stride
     STR   n_x0, [pDst], pDst_stride
     LDR   n_x2, [pSrc], pSrc_stride
     STR   n_x1, [pDst], pDst_stride
     LDR   n_x3, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     STR   n_x3, [pDst], pDst_stride     
     B     CopyEnd16
Align116:
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     STR   n_x2, [pDst], pDst_stride

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     STR   n_x2, [pDst], pDst_stride

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     STR   n_x2, [pDst], pDst_stride

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     STR   n_x2, [pDst], pDst_stride     
     B       CopyEnd16  
Align216:
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride        
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride      

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride        
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride  

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride        
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride      

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride        
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride      
     B     CopyEnd16  
Align316: 
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride    

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride 

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride 

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride      
CopyEnd16:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}   


@extern void get_luma_00_8_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_8_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align08
     B       Align18
     B       Align28
     B       Align38
Align08:
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x1, [pSrc], pSrc_stride
     STR   n_x0, [pDst], pDst_stride
     LDR   n_x2, [pSrc], pSrc_stride
     STR   n_x1, [pDst], pDst_stride
     LDR   n_x3, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     STR   n_x3, [pDst], pDst_stride
     
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x1, [pSrc], pSrc_stride
     STR   n_x0, [pDst], pDst_stride
     LDR   n_x2, [pSrc], pSrc_stride
     STR   n_x1, [pDst], pDst_stride
     LDR   n_x3, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     STR   n_x3, [pDst], pDst_stride

     B     CopyEnd8
Align18:
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     STR   n_x2, [pDst], pDst_stride

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     STR   n_x2, [pDst], pDst_stride

     B       CopyEnd8  
Align28:
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride        
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride      

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride        
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride  

     B     CopyEnd8  
Align38: 
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride    

     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride 
 
CopyEnd8:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}       

@extern void get_luma_00_4_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_4_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align04
     B       Align14
     B       Align24
     B       Align34
Align04:
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x1, [pSrc], pSrc_stride
     STR   n_x0, [pDst], pDst_stride
     LDR   n_x2, [pSrc], pSrc_stride
     STR   n_x1, [pDst], pDst_stride
     LDR   n_x3, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     STR   n_x3, [pDst], pDst_stride
     B     CopyEnd4
Align14:
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #8
     ORR   n_x0, n_x0, n_x1, LSL #24
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #8
     ORR   n_x2, n_x2, n_x3, LSL #24
     STR   n_x2, [pDst], pDst_stride
     
     B       CopyEnd4  
Align24:
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride        
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #16
     ORR   n_x0, n_x0, n_x1, LSL #16
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #16
     ORR   n_x2, n_x2, n_x3, LSL #16
     STR   n_x2, [pDst], pDst_stride      

     B     CopyEnd4  
Align34: 
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride
     LDR   n_x1, [pSrc, #4]
     LDR   n_x0, [pSrc], pSrc_stride
     LDR   n_x3, [pSrc, #4]
     LDR   n_x2, [pSrc], pSrc_stride
     MOV   n_x0, n_x0, LSR #24
     ORR   n_x0, n_x0, n_x1, LSL #8
     STR   n_x0, [pDst], pDst_stride
     MOV   n_x2, n_x2, LSR #24
     ORR   n_x2, n_x2, n_x3, LSL #8
     STR   n_x2, [pDst], pDst_stride    

CopyEnd4:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}   

@extern void get_luma_00_wxh_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride, VO_S32 w, VO_S32 h)	
get_luma_00_wxh_ARMV6:
     STMFD   sp!, {r0-r11, lr}
    
     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align0_wxh
     B       Align1_wxh
     B       Align2_wxh
     B       Align3_wxh
Align0_wxh:
     LDR   r4, [pSrc], pSrc_stride
     LDR   r5, [pSrc], pSrc_stride
     STR   r4, [pDst], pDst_stride
     LDR   r6, [pSrc], pSrc_stride
     STR   r5, [pDst], pDst_stride
     LDR   r7, [pSrc], pSrc_stride
     STR   r6, [pDst], pDst_stride
     STR   r7, [pDst], pDst_stride
     B     CopyEnd_wxh
Align1_wxh:
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #8
     ORR   r6, r6, r7, LSL #24
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     STR   r6, [pDst], pDst_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #8
     ORR   r6, r6, r7, LSL #24
     STR   r6, [pDst], pDst_stride
     
     B       CopyEnd_wxh  
Align2_wxh:
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #16
     ORR   r6, r6, r7, LSL #16
     STR   r6, [pDst], pDst_stride        
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #16
     ORR   r6, r6, r7, LSL #16
     STR   r6, [pDst], pDst_stride      

     B     CopyEnd_wxh  
Align3_wxh: 
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #24
     ORR   r6, r6, r7, LSL #8
     STR   r6, [pDst], pDst_stride
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #24
     ORR   r6, r6, r7, LSL #8
     STR   r6, [pDst], pDst_stride    

CopyEnd_wxh:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 
    


@extern void get_luma_00_16x16_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_16x16_ARMV6:
    STMFD   sp!, {r0-r11, lr}

     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     MOV     r9, #7
@     SUB     pSrc, pSrc, pSrc_stride
@     SUB     pDst, pDst, pDst_stride
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align0_16x16
     B       Align1_16x16
     B       Align2_16x16
     B       Align3_16x16
Align0_16x16:     
     LDRD  r6, r7, [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride
     STRD  r6, r7, [pDst, #8]
     STRD  r4, r5, [pDst], pDst_stride
       

     LDRD  r6, r7, [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride
     STRD  r6, r7, [pDst, #8]
     STRD  r4, r5, [pDst], pDst_stride

     SUBS r9, r9, #1
     BCS  Align0_16x16
     
     B     CopyEnd4_16x16
Align1_16x16:
     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride

     MOV   r10, r6, LSR #8
     ORR   r10, r10, r7, LSL #24
     MOV   r11, r7, LSR #8
     ORR   r11, r11, r8, LSL #24
     STRD  r10, r11, [pDst, #8]
     
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     MOV   r5, r5, LSR #8
     ORR   r5, r5, r6, LSL #24
     STRD  r4, r5, [pDst], pDst_stride
     

     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride

     MOV   r10, r6, LSR #8
     ORR   r10, r10, r7, LSL #24
     MOV   r11, r7, LSR #8
     ORR   r11, r11, r8, LSL #24
     STRD  r10, r11, [pDst, #8]
     
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     MOV   r5, r5, LSR #8
     ORR   r5, r5, r6, LSL #24
     STRD  r4, r5, [pDst], pDst_stride


     SUBS r9, r9, #1
     BCS  Align1_16x16
               
     B       CopyEnd4_16x16  
Align2_16x16:

     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride
     
     MOV   r10, r6, LSR #16
     ORR   r10, r10, r7, LSL #16
     MOV   r11, r7, LSR #16
     ORR   r11, r11, r8, LSL #16
     STRD  r10, r11, [pDst, #8]

     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     MOV   r5, r5, LSR #16
     ORR   r5, r5, r6, LSL #16
     STRD  r4, r5, [pDst], pDst_stride     

     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride
     
     MOV   r10, r6, LSR #16
     ORR   r10, r10, r7, LSL #16
     MOV   r11, r7, LSR #16
     ORR   r11, r11, r8, LSL #16
     STRD  r10, r11, [pDst, #8]

     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     MOV   r5, r5, LSR #16
     ORR   r5, r5, r6, LSL #16
     STRD  r4, r5, [pDst], pDst_stride

     SUBS r9, r9, #1
     BCS  Align2_16x16

     B     CopyEnd4_16x16  
Align3_16x16: 
@ row 0
     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride

     MOV   r10, r6, LSR #24
     ORR   r10, r10, r7, LSL #8
     MOV   r11, r7, LSR #24
     ORR   r11, r11, r8, LSL #8
     STRD  r10, r11, [pDst, #8]
     
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     MOV   r5, r5, LSR #24
     ORR   r5, r5, r6, LSL #8
     STRD  r4, r5, [pDst], pDst_stride
     
@ row 1
     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride

     MOV   r10, r6, LSR #24
     ORR   r10, r10, r7, LSL #8
     MOV   r11, r7, LSR #24
     ORR   r11, r11, r8, LSL #8
     STRD  r10, r11, [pDst, #8]
     
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     MOV   r5, r5, LSR #24
     ORR   r5, r5, r6, LSL #8
     STRD  r4, r5, [pDst], pDst_stride

     SUBS r9, r9, #1
     BCS  Align3_16x16     

CopyEnd4_16x16:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  


@extern void get_luma_00_16x8_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_16x8_ARMV6:
     STMFD   sp!, {r0-r11, lr}

     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     MOV     r9, #3
@     SUB     pSrc, pSrc, pSrc_stride
@     SUB     pDst, pDst, pDst_stride
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align0_16x8
     B       Align1_16x8
     B       Align2_16x8
     B       Align3_16x8
Align0_16x8:     
     LDRD  r6, r7, [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride
     STRD  r6, r7, [pDst, #8]
     STRD  r4, r5, [pDst], pDst_stride
       

     LDRD  r6, r7, [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride
     STRD  r6, r7, [pDst, #8]
     STRD  r4, r5, [pDst], pDst_stride


     SUBS r9, r9, #1
     BCS  Align0_16x8
     
     B     CopyEnd4_16x8
Align1_16x8:
     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride

     MOV   r10, r6, LSR #8
     ORR   r10, r10, r7, LSL #24
     MOV   r11, r7, LSR #8
     ORR   r11, r11, r8, LSL #24
     STRD  r10, r11, [pDst, #8]
     
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     MOV   r5, r5, LSR #8
     ORR   r5, r5, r6, LSL #24
     STRD  r4, r5, [pDst], pDst_stride
     

     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride

     MOV   r10, r6, LSR #8
     ORR   r10, r10, r7, LSL #24
     MOV   r11, r7, LSR #8
     ORR   r11, r11, r8, LSL #24
     STRD  r10, r11, [pDst, #8]
     
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     MOV   r5, r5, LSR #8
     ORR   r5, r5, r6, LSL #24
     STRD  r4, r5, [pDst], pDst_stride


     SUBS r9, r9, #1
     BCS  Align1_16x8
               
     B       CopyEnd4_16x8  
Align2_16x8:

     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride
     
     MOV   r10, r6, LSR #16
     ORR   r10, r10, r7, LSL #16
     MOV   r11, r7, LSR #16
     ORR   r11, r11, r8, LSL #16
     STRD  r10, r11, [pDst, #8]

     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     MOV   r5, r5, LSR #16
     ORR   r5, r5, r6, LSL #16
     STRD  r4, r5, [pDst], pDst_stride     

     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride
     
     MOV   r10, r6, LSR #16
     ORR   r10, r10, r7, LSL #16
     MOV   r11, r7, LSR #16
     ORR   r11, r11, r8, LSL #16
     STRD  r10, r11, [pDst, #8]

     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     MOV   r5, r5, LSR #16
     ORR   r5, r5, r6, LSL #16
     STRD  r4, r5, [pDst], pDst_stride

    
     SUBS r9, r9, #1
     BCS  Align2_16x8

     B     CopyEnd4_16x8  
Align3_16x8: 
@ row 0
     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride

     MOV   r10, r6, LSR #24
     ORR   r10, r10, r7, LSL #8
     MOV   r11, r7, LSR #24
     ORR   r11, r11, r8, LSL #8
     STRD  r10, r11, [pDst, #8]
     
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     MOV   r5, r5, LSR #24
     ORR   r5, r5, r6, LSL #8
     STRD  r4, r5, [pDst], pDst_stride
     
@ row 1
     LDRD  r6, r7, [pSrc, #8]
     LDR   r8,     [pSrc, #16]
     LDRD  r4, r5, [pSrc], pSrc_stride

     MOV   r10, r6, LSR #24
     ORR   r10, r10, r7, LSL #8
     MOV   r11, r7, LSR #24
     ORR   r11, r11, r8, LSL #8
     STRD  r10, r11, [pDst, #8]
     
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     MOV   r5, r5, LSR #24
     ORR   r5, r5, r6, LSL #8
     STRD  r4, r5, [pDst], pDst_stride
     

     SUBS r9, r9, #1
     BCS  Align3_16x8    

CopyEnd4_16x8:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  

@extern void get_luma_00_8x16_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_8x16_ARMV6:
    STMFD   sp!, {r0-r11, lr}

     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     MOV     r14, #7
@       SUB     pSrc, pSrc, pSrc_stride
@       SUB     pDst, pDst, pDst_stride
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align0_8x16
     B       Align1_8x16
     B       Align2_8x16
     B       Align3_8x16
Align0_8x16:
     LDRD  r4, r5,   [pSrc], pSrc_stride
     LDRD  r6, r7,   [pSrc], pSrc_stride
    
     STRD  r4, r5,   [pDst], pDst_stride
     STRD  r6, r7,   [pDst], pDst_stride 
    
     SUBS r14, r14, #1
     BCS  Align0_8x16
     
     B     CopyEnd4_8x16
Align1_8x16:     
     LDR   r8,     [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride
     LDR   r9,     [pSrc, #8]
     LDRD  r6, r7, [pSrc], pSrc_stride
     
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     MOV   r5, r5, LSR #8
     ORR   r5, r5, r8, LSL #24
     STRD  r4, r5, [pDst], pDst_stride
     MOV   r6, r6, LSR #8
     ORR   r6, r6, r7, LSL #24
     MOV   r7, r7, LSR #8
     ORR   r7, r7, r9, LSL #24
     STRD  r6, r7, [pDst], pDst_stride

     SUBS r14, r14, #1
     BCS  Align1_8x16     
          
     B       CopyEnd4_8x16  
Align2_8x16:

     LDR   r8,     [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride

     LDR   r9,     [pSrc, #8]
     LDRD  r6, r7, [pSrc], pSrc_stride
     
     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     MOV   r5, r5, LSR #16
     ORR   r5, r5, r8, LSL #16
     STRD  r4, r5, [pDst], pDst_stride
     MOV   r6, r6, LSR #16
     ORR   r6, r6, r7, LSL #16
     MOV   r7, r7, LSR #16
     ORR   r7, r7, r9, LSL #16
     STRD  r6, r7, [pDst], pDst_stride

     SUBS r14, r14, #1
     BCS  Align2_8x16

     B     CopyEnd4_8x16  
Align3_8x16: 
     LDR   r8,     [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride

     LDR   r9,     [pSrc, #8]
     LDRD  r6, r7, [pSrc], pSrc_stride

     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     MOV   r5, r5, LSR #24
     ORR   r5, r5, r8, LSL #8
     STRD  r4, r5, [pDst], pDst_stride
     
     MOV   r6, r6, LSR #24
     ORR   r6, r6, r7, LSL #8
     MOV   r7, r7, LSR #24
     ORR   r7, r7, r9, LSL #8
     STRD  r6, r7, [pDst], pDst_stride


     SUBS r14, r14, #1
     BCS  Align3_8x16     

CopyEnd4_8x16:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  
    
    

@extern void get_luma_00_8x8_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_8x8_ARMV6:
    STMFD   sp!, {r0-r11, lr}

     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     MOV     r14, #3
@       SUB     pSrc, pSrc, pSrc_stride
@       SUB     pDst, pDst, pDst_stride
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align0_8x8
     B       Align1_8x8
     B       Align2_8x8
     B       Align3_8x8
Align0_8x8:
     LDRD  r4, r5,   [pSrc], pSrc_stride
     LDRD  r6, r7,   [pSrc], pSrc_stride
    
     STRD  r4, r5,   [pDst], pDst_stride
     STRD  r6, r7,   [pDst], pDst_stride 
    
     SUBS r14, r14, #1
     BCS  Align0_8x8
     
     B     CopyEnd4_8x8
Align1_8x8:     
     LDR   r8,     [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride
     LDR   r9,     [pSrc, #8]
     LDRD  r6, r7, [pSrc], pSrc_stride
     
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     MOV   r5, r5, LSR #8
     ORR   r5, r5, r8, LSL #24
     STRD  r4, r5, [pDst], pDst_stride
     MOV   r6, r6, LSR #8
     ORR   r6, r6, r7, LSL #24
     MOV   r7, r7, LSR #8
     ORR   r7, r7, r9, LSL #24
     STRD  r6, r7, [pDst], pDst_stride

     SUBS r14, r14, #1
     BCS  Align1_8x8     
          
     B       CopyEnd4_8x8  
Align2_8x8:

     LDR   r8,     [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride

     LDR   r9,     [pSrc, #8]
     LDRD  r6, r7, [pSrc], pSrc_stride
     
     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     MOV   r5, r5, LSR #16
     ORR   r5, r5, r8, LSL #16
     STRD  r4, r5, [pDst], pDst_stride
     MOV   r6, r6, LSR #16
     ORR   r6, r6, r7, LSL #16
     MOV   r7, r7, LSR #16
     ORR   r7, r7, r9, LSL #16
     STRD  r6, r7, [pDst], pDst_stride

     SUBS r14, r14, #1
     BCS  Align2_8x8

     B     CopyEnd4_8x8  
Align3_8x8: 
     LDR   r8,     [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride

     LDR   r9,     [pSrc, #8]
     LDRD  r6, r7, [pSrc], pSrc_stride

     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     MOV   r5, r5, LSR #24
     ORR   r5, r5, r8, LSL #8
     STRD  r4, r5, [pDst], pDst_stride
     
     MOV   r6, r6, LSR #24
     ORR   r6, r6, r7, LSL #8
     MOV   r7, r7, LSR #24
     ORR   r7, r7, r9, LSL #8
     STRD  r6, r7, [pDst], pDst_stride


     SUBS r14, r14, #1
     BCS  Align3_8x8     

CopyEnd4_8x8:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 
    

@extern void get_luma_00_8x4_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_8x4_ARMV6:
    STMFD   sp!, {r0-r11, lr}

     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     MOV     r14, #1
@       SUB     pSrc, pSrc, pSrc_stride
@       SUB     pDst, pDst, pDst_stride
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align0_8x4
     B       Align1_8x4
     B       Align2_8x4
     B       Align3_8x4
Align0_8x4:
     LDRD  r4, r5,   [pSrc], pSrc_stride
     LDRD  r6, r7,   [pSrc], pSrc_stride
    
     STRD  r4, r5,   [pDst], pDst_stride
     STRD  r6, r7,   [pDst], pDst_stride 
    
     SUBS r14, r14, #1
     BCS  Align0_8x4
     
     B     CopyEnd4_8x4
Align1_8x4:     
     LDR   r8,     [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride
     LDR   r9,     [pSrc, #8]
     LDRD  r6, r7, [pSrc], pSrc_stride
     
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     MOV   r5, r5, LSR #8
     ORR   r5, r5, r8, LSL #24
     STRD  r4, r5, [pDst], pDst_stride
     MOV   r6, r6, LSR #8
     ORR   r6, r6, r7, LSL #24
     MOV   r7, r7, LSR #8
     ORR   r7, r7, r9, LSL #24
     STRD  r6, r7, [pDst], pDst_stride

     SUBS r14, r14, #1
     BCS  Align1_8x4     
          
     B       CopyEnd4_8x4 
Align2_8x4:

     LDR   r8,     [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride

     LDR   r9,     [pSrc, #8]
     LDRD  r6, r7, [pSrc], pSrc_stride
     
     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     MOV   r5, r5, LSR #16
     ORR   r5, r5, r8, LSL #16
     STRD  r4, r5, [pDst], pDst_stride
     MOV   r6, r6, LSR #16
     ORR   r6, r6, r7, LSL #16
     MOV   r7, r7, LSR #16
     ORR   r7, r7, r9, LSL #16
     STRD  r6, r7, [pDst], pDst_stride

     SUBS r14, r14, #1
     BCS  Align2_8x4

     B     CopyEnd4_8x4  
Align3_8x4: 
     LDR   r8,     [pSrc, #8]
     LDRD  r4, r5, [pSrc], pSrc_stride

     LDR   r9,     [pSrc, #8]
     LDRD  r6, r7, [pSrc], pSrc_stride

     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     MOV   r5, r5, LSR #24
     ORR   r5, r5, r8, LSL #8
     STRD  r4, r5, [pDst], pDst_stride
     
     MOV   r6, r6, LSR #24
     ORR   r6, r6, r7, LSL #8
     MOV   r7, r7, LSR #24
     ORR   r7, r7, r9, LSL #8
     STRD  r6, r7, [pDst], pDst_stride

     SUBS r14, r14, #1
     BCS  Align3_8x4     

CopyEnd4_8x4:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 

@extern void get_luma_00_4x8_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_4x8_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     MOV     r14, #1
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align0_4x8
     B       Align1_4x8
     B       Align2_4x8
     B       Align3_4x8
Align0_4x8:
     LDR   r4, [pSrc], pSrc_stride
     LDR   r5, [pSrc], pSrc_stride
     STR   r4, [pDst], pDst_stride
     LDR   r6, [pSrc], pSrc_stride
     STR   r5, [pDst], pDst_stride
     LDR   r7, [pSrc], pSrc_stride
     STR   r6, [pDst], pDst_stride
     STR   r7, [pDst], pDst_stride

     SUBS r14, r14, #1
     BCS  Align0_4x8     
     B     CopyEnd_4x8
Align1_4x8:
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #8
     ORR   r6, r6, r7, LSL #24
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     STR   r6, [pDst], pDst_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #8
     ORR   r6, r6, r7, LSL #24
     STR   r6, [pDst], pDst_stride

     SUBS r14, r14, #1
     BCS  Align1_4x8          
     B       CopyEnd_4x8  
Align2_4x8:
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #16
     ORR   r6, r6, r7, LSL #16
     STR   r6, [pDst], pDst_stride        
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #16
     ORR   r6, r6, r7, LSL #16
     STR   r6, [pDst], pDst_stride      

     SUBS r14, r14, #1
     BCS  Align2_4x8          
     B     CopyEnd_4x8  
Align3_4x8: 
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #24
     ORR   r6, r6, r7, LSL #8
     STR   r6, [pDst], pDst_stride
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #24
     ORR   r6, r6, r7, LSL #8
     STR   r6, [pDst], pDst_stride    

     SUBS r14, r14, #1
     BCS  Align3_4x8          

CopyEnd_4x8:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 
    

@extern void get_luma_00_4x4_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
get_luma_00_4x4_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
     AND     Temp, pSrc, #3
     BIC     pSrc, pSrc, #3 
     ADD     pc, pc, Temp, LSL #2
     NOP
     B       Align0_4x4
     B       Align1_4x4
     B       Align2_4x4
     B       Align3_4x4
Align0_4x4:
     LDR   r4, [pSrc], pSrc_stride
     LDR   r5, [pSrc], pSrc_stride
     STR   r4, [pDst], pDst_stride
     LDR   r6, [pSrc], pSrc_stride
     STR   r5, [pDst], pDst_stride
     LDR   r7, [pSrc], pSrc_stride
     STR   r6, [pDst], pDst_stride
     STR   r7, [pDst], pDst_stride
     B     CopyEnd_4x4
Align1_4x4:
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #8
     ORR   r6, r6, r7, LSL #24
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     STR   r6, [pDst], pDst_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #8
     ORR   r4, r4, r5, LSL #24
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #8
     ORR   r6, r6, r7, LSL #24
     STR   r6, [pDst], pDst_stride
     
     B       CopyEnd_4x4  
Align2_4x4:
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #16
     ORR   r6, r6, r7, LSL #16
     STR   r6, [pDst], pDst_stride        
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #16
     ORR   r4, r4, r5, LSL #16
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #16
     ORR   r6, r6, r7, LSL #16
     STR   r6, [pDst], pDst_stride      

     B     CopyEnd_4x4  
Align3_4x4: 
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #24
     ORR   r6, r6, r7, LSL #8
     STR   r6, [pDst], pDst_stride
     LDR   r5, [pSrc, #4]
     LDR   r4, [pSrc], pSrc_stride
     LDR   r7, [pSrc, #4]
     LDR   r6, [pSrc], pSrc_stride
     MOV   r4, r4, LSR #24
     ORR   r4, r4, r5, LSL #8
     STR   r4, [pDst], pDst_stride
     MOV   r6, r6, LSR #24
     ORR   r6, r6, r7, LSL #8
     STR   r6, [pDst], pDst_stride    

CopyEnd_4x4:        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 
    
  @void wp_16(VO_U8 *p_Dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 n_h)
@{
@	VO_S32 j,i;
@	for(j = n_h;j != 0;j--)
@	{
@		for(i = 2;i != 0;i--)
@		{
@			p_Dst[0] = clip[rshift_rnd((wp_scale*p_Dst[0]), weight_denom)+wp_offset];      
@	                            p_Dst[1] = clip[rshift_rnd((wp_scale*p_Dst[1]), weight_denom)+wp_offset];
@			p_Dst[2] = clip[rshift_rnd((wp_scale*p_Dst[2]), weight_denom)+wp_offset];      
@	                            p_Dst[3] = clip[rshift_rnd((wp_scale*p_Dst[3]), weight_denom)+wp_offset];
@			p_Dst[4] = clip[rshift_rnd((wp_scale*p_Dst[4]), weight_denom)+wp_offset];      
@	                            p_Dst[5] = clip[rshift_rnd((wp_scale*p_Dst[5]), weight_denom)+wp_offset];
@			p_Dst[6] = clip[rshift_rnd((wp_scale*p_Dst[6]), weight_denom)+wp_offset];      
@	                            p_Dst[7] = clip[rshift_rnd((wp_scale*p_Dst[7]), weight_denom)+wp_offset];
@			p_Dst+=8;
@		}
@		p_Dst+=(dst_stride-16);
@	}
@}

 weight_denom = 0x34
 n_h       = 0x38

@extern void wp_16_armv6(VO_U8 *p_Dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 n_h)
@r0 p_Dst
@r1 dst_stride
@r2 wp_scale
@r3 wp_offset
@r4 weight_denom
@r5 n_h
@r11 n_w
wp_16_armv6:  
    STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #weight_denom]
    LDR  r5, [sp, #n_h]
    CMP  r4, #0
 @   LDR  r11, =0x00010001
 @   SMMUL  r3, r3, r11
    LDR  r12, =0xFFFF
    AND r3 , r3, r12
    ORR r3, r3, r3, ROR #16
    MOV  r11, #3    
    BLS  w_neg
    MOV  r14, r4
    LDR  r12, =0x00010001
    RSB  r7, r14, #16
    RSB  r12, r12, r12, LSL r7
    SUB  r4, r4, #1
    MOV  r6, #1
    MOV  r4, r6, LSL r4
 @   MOV  r4, r6,
Loop_y1:
    LDRB  r7, [r0], #1
    SMLABB r7, r7, r2, r4
    LDRB  r8, [r0], #1
    SMLABB r8, r8, r2, r4
    LDRB  r9, [r0], #1
    SMLABB r9, r9, r2, r4
    LDRB  r10, [r0], #1
    SMLABB r10, r10, r2, r4
    ASR   r7, r7, r14
    ASR   r8, r8, r14
    ASR   r9, r9, r14
    ASR   r10, r10, r14
    PKHBT  r8, r8, r10, LSL #16
    PKHBT  r7, r7, r9, LSL #16
@    AND    r8, r12, R8, ASR r14 
@    AND    r7, r12, R7, ASR r14       
    SADD16 r8, r8, r3
    SADD16 r7, r7, r3
    USAT16 r8, #8, r8
    USAT16 r7, #8, r7
    ORR    r7, r7, r8, LSL #8
    SUBS   r11, r11, #1
    STR    r7, [r0, #-4]
    BCS    Loop_y1
    MOV  r11, #3 
    SUBS   r5, r5, #1
    SUB    r0, r0, #16
    ADD    r0, r0, r1
    BGT    Loop_y1    
    B Loop_end
w_neg:    
    MOV r7, #0
    SUB r4, r7, r4
    MOV r12, r2, LSL #16
Loop_y2:    
    LDR r7, [r0]
    UXTB16     r8, r7
    UXTB16     r9, r7, ROR #8
    SMMUL     r10, r8, r12
    MOV       r10, r10, LSL #16
    SMLAD     r10, r8, r2, r10
    SMMUL    r14, r9, r12
    MOV      r14, r14, LSL #16
    SMLAD      r14, r9, r2, r14
    MOV      r10, r10, LSL r4
    MOV      r14, r14, LSL r4
    SADD16   r10, r3, r10 
    SADD16   r14, r3, r14
    USAT16   r10, #8, r10
    USAT16   r14, #8, r14
    ORR      r10, r10, r14, LSL #8
    SUBS   r11, r11, #1
    STR      r10, [r0], #4
    BCS    Loop_y2
    MOV  r11, #3 
    SUBS   r5, r5, #1
    SUB    r0, r0, #16
    ADD    r0, r0, r1
    BGT    Loop_y2        
Loop_end:

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  

@extern void wp_8_armv6(VO_U8 *p_Dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 n_h)
@r0 p_Dst
@r1 dst_stride
@r2 wp_scale
@r3 wp_offset
@r4 weight_denom
@r5 n_h
@r11 n_w
wp_8_armv6:  
    STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #weight_denom]
    LDR  r5, [sp, #n_h]
    CMP  r4, #0
 @   LDR  r11, =0x00010001
 @   SMMUL  r3, r3, r11
    LDR  r12, =0xFFFF
    AND r3 , r3, r12
    ORR r3, r3, r3, ROR #16
    MOV  r11, #1    
    BLS  w_neg_wp8
    MOV  r14, r4
    LDR  r12, =0x00010001
    RSB  r7, r14, #16
    RSB  r12, r12, r12, LSL r7
    SUB  r4, r4, #1
    MOV  r6, #1
    MOV  r4, r6, LSL r4
 @   MOV  r4, r6,
Loop_y1_wp8:
    LDRB  r7, [r0], #1
    SMLABB r7, r7, r2, r4
    LDRB  r8, [r0], #1
    SMLABB r8, r8, r2, r4
    LDRB  r9, [r0], #1
    SMLABB r9, r9, r2, r4
    LDRB  r10, [r0], #1
    SMLABB r10, r10, r2, r4
    ASR   r7, r7, r14
    ASR   r8, r8, r14
    ASR   r9, r9, r14
    ASR   r10, r10, r14    
    PKHBT  r8, r8, r10, LSL #16
    PKHBT  r7, r7, r9, LSL #16
@    AND    r8, r12, R8, ASR r14 
@    AND    r7, r12, R7, ASR r14       
    SADD16 r8, r8, r3
    SADD16 r7, r7, r3
    USAT16 r8, #8, r8
    USAT16 r7, #8, r7
    ORR    r7, r7, r8, LSL #8
    SUBS   r11, r11, #1
    STR    r7, [r0, #-4]
    BCS    Loop_y1_wp8
    MOV  r11, #1 
    SUBS   r5, r5, #1
    SUB    r0, r0, #8
    ADD    r0, r0, r1
    BGT    Loop_y1_wp8    
    B Loop_end_wp8
w_neg_wp8:    
    MOV r7, #0
    SUB r4, r7, r4
    MOV r12, r2, LSL #16
Loop_y2_wp8:    
    LDR r7, [r0]
    UXTB16     r8, r7
    UXTB16     r9, r7, ROR #8
    SMMUL     r10, r8, r12
    MOV       r10, r10, LSL #16
    SMLAD     r10, r8, r2, r10
    SMMUL    r14, r9, r12
    MOV      r14, r14, LSL #16
    SMLAD      r14, r9, r2, r14
    MOV      r10, r10, LSL r4
    MOV      r14, r14, LSL r4
    SADD16   r10, r3, r10 
    SADD16   r14, r3, r14
    USAT16   r10, #8, r10
    USAT16   r14, #8, r14
    ORR      r10, r10, r14, LSL #8
    SUBS   r11, r11, #1
    STR      r10, [r0], #4
    BCS    Loop_y2_wp8
    MOV  r11, #1 
    SUBS   r5, r5, #1
    SUB    r0, r0, #8
    ADD    r0, r0, r1
    BGT    Loop_y2_wp8        
Loop_end_wp8:

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}      


@extern void wp_4_armv6(VO_U8 *p_Dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 n_h)
@r0 p_Dst
@r1 dst_stride
@r2 wp_scale
@r3 wp_offset
@r4 weight_denom
@r5 n_h
@r11 n_w
wp_4_armv6:  
    STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #weight_denom]
    LDR  r5, [sp, #n_h]
    CMP  r4, #0
 @   LDR  r11, =0x00010001
 @   SMMUL  r3, r3, r11
    LDR  r12, =0xFFFF
    AND r3 , r3, r12
    ORR r3, r3, r3, ROR #16
@    MOV  r11, #1    
    BLS  w_neg_wp4
    MOV  r14, r4
    LDR  r12, =0x00010001
    RSB  r7, r14, #16
    RSB  r12, r12, r12, LSL r7
    SUB  r4, r4, #1
    MOV  r6, #1
    MOV  r4, r6, LSL r4
 @   MOV  r4, r6,
Loop_y1_wp4:
    LDRB  r7, [r0], #1
    SMLABB r7, r7, r2, r4
    LDRB  r8, [r0], #1
    SMLABB r8, r8, r2, r4
    LDRB  r9, [r0], #1
    SMLABB r9, r9, r2, r4
    LDRB  r10, [r0], #1
    SMLABB r10, r10, r2, r4
    ASR   r7, r7, r14
    ASR   r8, r8, r14
    ASR   r9, r9, r14
    ASR   r10, r10, r14    
    
    PKHBT  r8, r8, r10, LSL #16
    PKHBT  r7, r7, r9, LSL #16
@    AND    r8, r12, R8, LSR r14 
@    AND    r7, r12, R7, LSR r14       
    SADD16 r8, r8, r3
    SADD16 r7, r7, r3
    USAT16 r8, #8, r8
    USAT16 r7, #8, r7
    ORR    r7, r7, r8, LSL #8
@    SUBS   r11, r11, #1
    STR    r7, [r0, #-4]
@    BCS    Loop_y1_wp4
@    MOV  r11, #1 
    SUBS   r5, r5, #1
    SUB    r0, r0, #4
    ADD    r0, r0, r1
    BGT    Loop_y1_wp4   
    B Loop_end_wp4
w_neg_wp4:    
    MOV r7, #0
    SUB r4, r7, r4
    MOV r12, r2, LSL #16
Loop_y2_wp4:    
    LDR r7, [r0]
    UXTB16     r8, r7
    UXTB16     r9, r7, ROR #8
    SMMUL     r10, r8, r12
    MOV       r10, r10, LSL #16
    SMLAD     r10, r8, r2, r10
    SMMUL    r14, r9, r12
    MOV      r14, r14, LSL #16
    SMLAD      r14, r9, r2, r14
    MOV      r10, r10, LSL r4
    MOV      r14, r14, LSL r4
    SADD16   r10, r3, r10 
    SADD16   r14, r3, r14
    USAT16   r10, #8, r10
    USAT16   r14, #8, r14
    ORR      r10, r10, r14, LSL #8
@    SUBS   r11, r11, #1
    STR      r10, [r0], #4
@    BCS    Loop_y2_wp4
@    MOV  r11, #3 
    SUBS   r5, r5, #1
    SUB    r0, r0, #4
    ADD    r0, r0, r1
    BGT    Loop_y2_wp4        
Loop_end_wp4:

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}     
    

@extern void wp_2_armv6(VO_U8 *p_Dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 n_h)
@r0 p_Dst
@r1 dst_stride
@r2 wp_scale
@r3 wp_offset
@r4 weight_denom
@r5 n_h
@r11 n_w
wp_2_armv6:  
    STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #weight_denom]
    LDR  r5, [sp, #n_h]
    CMP  r4, #0
 @   LDR  r11, =0x00010001
 @   SMMUL  r3, r3, r11
    LDR  r12, =0xFFFF
    AND r3 , r3, r12
    ORR r3, r3, r3, ROR #16
@    MOV  r11, #1    
    BLS  w_neg_wp2
    MOV  r14, r4
    LDR  r12, =0x00010001
    RSB  r7, r14, #16
    RSB  r12, r12, r12, LSL r7
    SUB  r4, r4, #1
    MOV  r6, #1
    MOV  r4, r6, LSL r4
 @   MOV  r4, r6,
Loop_y1_wp2:
    LDRB  r7, [r0], #1
    SMLABB r7, r7, r2, r4
    LDRB  r8, [r0], #-1
    SMLABB r8, r8, r2, r4
@    LDRB  r9, [r0], #1
@    SMLABB r9, r9, r2, r4
@    LDRB  r10, [r0], #1
@    SMLABB r10, r10, r2, r4
 @   PKHBT  r8, r8, r10, LSL #16
    ASR r7, r7, r14
    ASR r8, r8, r14
    PKHBT  r7, r7, r8, LSL #16
@    AND    r8, r12, R8, LSR r14 
@    AND    r7, r12, R7, LSR r14       
 @   SADD16 r8, r8, r3
    SADD16 r7, r7, r3
 @   USAT16 r8, #8, r8
    USAT16 r7, #8, r7
    ORR    r7, r7, r7, LSR #8
@    SUBS   r11, r11, #1
    STRH    r7, [r0]
@    BCS    Loop_y1_wp4
@    MOV  r11, #1 
    SUBS   r5, r5, #1
@    SUB    r0, r0, #16
    ADD    r0, r0, r1
    BGT    Loop_y1_wp2   
    B Loop_end_wp2
w_neg_wp2:    
    MOV r7, #0
    SUB r4, r7, r4
    MOV r12, r2, LSL #16
Loop_y2_wp2:    
    LDRH r7, [r0]
    UXTB16      r8, r7
    UXTAB16     r9, r8, r7, ROR #24
    
    SMMUL     r10, r9, r12
    MOV       r10, r10, LSL #16
    SMLAD     r10, r9, r2, r10
 @   SMMUL    r14, r9, r12
 @   MOV      r14, r14, LSL #16
 @   SMLAD      r14, r9, r2, r14
 @   MOV      r10, r10, LSL r4
 @   MOV      r14, r14, LSL r4
    SADD16   r10, r3, r10 
 @   SADD16   r14, r3, r14
    USAT16   r10, #8, r10
  @  USAT16   r14, #8, r14
    ORR      r10, r10, r10, LSR #8
@    SUBS   r11, r11, #1
    STRH      r10, [r0]
@    BCS    Loop_y2_wp4
@    MOV  r11, #3 
    SUBS   r5, r5, #1
 @   SUB    r0, r0, #2
    ADD    r0, r0, r1
    BGT    Loop_y2_wp2        
Loop_end_wp2:

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}    

@void of_16(VO_U8 *p_Dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 n_h)
@{
@	VO_S32 j,i;
@	for(j = n_h;j != 0;j--)
@	{
@		for(i = 2;i != 0;i--)
@		{
@			p_Dst[0] = clip[p_Dst[0]+wp_offset];      
@	        p_Dst[1] = clip[p_Dst[1]+wp_offset];
@			p_Dst[2] = clip[p_Dst[2]+wp_offset];      
@	        p_Dst[3] = clip[p_Dst[3]+wp_offset];
@			p_Dst[4] = clip[p_Dst[4]+wp_offset];      
@	        p_Dst[5] = clip[p_Dst[5]+wp_offset];
@			p_Dst[6] = clip[p_Dst[6]+wp_offset];      
@	        p_Dst[7] = clip[p_Dst[7]+wp_offset];
@			p_Dst+=8;
@		}
@		p_Dst+=(dst_stride-16);
@	}
@}
@extern void of_16_armv6(VO_U8 *p_Dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 n_h)
@r0 p_Dst
@r1 dst_stride
@r2 wp_offset
@r3 n_h
of_16_armv6:

    STMFD   sp!, {r0-r11, lr}

    MOV  r11, #1
    LDR  r12, =0xFFFF
    AND r2 , r2, r12
    ORR r2, r2, r2, ROR #16
  
Loop_y3:
    LDR  r4, [r0], #4
    UXTAB16 r5, r2, r4
    UXTAB16 r6, r2, r4, ROR #8
    LDR     r7, [r0]
    USAT16  r5, #8, r5
    USAT16  r6, #8, r6
    ORR     r5, r5, r6, LSL #8
    STR     r5, [r0, #-4]
    UXTAB16 r8, r2, r7
    UXTAB16 r9, r2, r7, ROR #8
    USAT16  r8, #8, r8
    USAT16  r9, #8, r9
    SUBS    r11, r11, #1
    ORR     r8, r8, r9, LSL #8
    STR     r8, [r0], #4 
    BCS     Loop_y3
    MOV     r11, #1 
    SUBS    r3, r3, #1
    SUB     r0, r0, #16
    ADD     r0, r0, r1
    BGT     Loop_y3   

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  

@extern void of_8_armv6(VO_U8 *p_Dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 n_h)
@r0 p_Dst
@r1 dst_stride
@r2 wp_offset
@r3 n_h
of_8_armv6:

    STMFD   sp!, {r0-r11, lr}

@    MOV  r11, #1
    LDR  r12, =0xFFFF
    AND r2 , r2, r12
    ORR r2, r2, r2, ROR #16
  
Loop_y_of8:
    LDR  r4, [r0], #4
    UXTAB16 r5, r2, r4
    UXTAB16 r6, r2, r4, ROR #8
    LDR     r7, [r0]
    USAT16  r5, #8, r5
    USAT16  r6, #8, r6
    ORR     r5, r5, r6, LSL #8
    STR     r5, [r0, #-4]
    UXTAB16 r8, r2, r7
    UXTAB16 r9, r2, r7, ROR #8
    USAT16  r8, #8, r8
    USAT16  r9, #8, r9
@    SUBS    r11, r11, #1
    ORR     r8, r8, r9, LSL #8
    STR     r8, [r0], #4 
 @   BCS     Loop_y_of8
@    MOV     r11, #1 
    SUBS    r3, r3, #1
    SUB     r0, r0, #8
    ADD     r0, r0, r1
    BGT     Loop_y_of8   

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  

@extern void of_4_armv6(VO_U8 *p_Dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 n_h)
@r0 p_Dst
@r1 dst_stride
@r2 wp_offset
@r3 n_h
of_4_armv6:

    STMFD   sp!, {r0-r11, lr}

 @   MOV  r11, #1
    LDR  r12, =0xFFFF
    AND r2 , r2, r12
    ORR r2, r2, r2, ROR #16
  
Loop_y_of4:
    LDR  r4, [r0]
    UXTAB16 r5, r2, r4
    UXTAB16 r6, r2, r4, ROR #8
@    LDR     r7, [r0]
    USAT16  r5, #8, r5
    USAT16  r6, #8, r6
    ORR     r5, r5, r6, LSL #8
    STR     r5, [r0]
@    UXTAB16 r8, r2, r7
@    UXTAB16 r9, r2, r7, ROR #8
@    USAT16  r8, #8, r8
@    USAT16  r9, #8, r9
@    SUBS    r11, r11, #1
@    ORR     r8, r8, r9, LSL #8
@    STR     r8, [r0], #4 
 @   BCS     Loop_y_of4
 @   MOV     r11, #1 
    SUBS    r3, r3, #1
 @   SUB     r0, r0, #16
    ADD     r0, r0, r1
    BGT     Loop_y_of4   

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}     
    

@extern void of_2_armv6(VO_U8 *p_Dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 n_h)
@r0 p_Dst
@r1 dst_stride
@r2 wp_offset
@r3 n_h
of_2_armv6:

    STMFD   sp!, {r0-r11, lr}

 @   MOV  r11, #1
    LDR  r12, =0xFFFF
    AND r2 , r2, r12
    ORR r2, r2, r2, ROR #16
  
Loop_y_of2:
    LDRH  r4, [r0]
    ORR   r5, r4, r4, ror #24
    UXTAB16 r5, r2, r5
@    UXTAB16 r6, r2, r4, ROR #24
@    LDR     r7, [r0]
    USAT16  r5, #8, r5
@    USAT16  r6, #8, r6
    ORR     r5, r5, r5, LSR #8
    STRH    r5, [r0]
@    UXTAB16 r8, r2, r7
@    UXTAB16 r9, r2, r7, ROR #8
@    USAT16  r8, #8, r8
@    USAT16  r9, #8, r9
@    SUBS    r11, r11, #1
@    ORR     r8, r8, r9, LSL #8
@    STR     r8, [r0], #4 
 @   BCS     Loop_y_of4
 @   MOV     r11, #1 
    SUBS    r3, r3, #1
 @   SUB     r0, r0, #16
    ADD     r0, r0, r1
    BGT     Loop_y_of2   

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}     

 wp_scale_l0  = 0x34
 wp_scale_l1  = 0x38
 wp_offset    = 0x3C
 wbi_denom    = 0x40
 wbi_height   = 0x44

@//add by Really Yang 20110414
@extern void wbi_16_armv6(VO_U8 *p_Dst,VO_S32 dst_stride, VO_U8 *p_Src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
@                    VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 n_h)
@   p_Dst[0] = clip[rshift_rnd_sf((wp_scale_l0*p_Src[0]+wp_scale_l1*p_Dst[0]),weight_denom)+wp_offset];
@r0 p_Dst
@r1 dst_stride
@r2 p_Src
@r3 src_stride
@r4 wp_scale_l1 | wp_scale_l0
@r5 1 << (weight_denom -1)
@r6 wp_offset
@r7 n_w >> 2
@r8 n_h
wbi_16_armv6:
   STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #wp_scale_l0]
    LDR  r5, [sp, #wp_scale_l1]
    LDR  r6, [sp, #wp_offset]
    LDR  r7, [sp, #wbi_denom]
    LDR  r8, [sp, #wbi_height]
    
@    CMP  r7, #0
 @   LDR  r11, =0x00010001
 @   SMMUL  r3, r3, r11
     LDR  r14, =0xFFFF
    AND r6 , r6, r14
    ORR r6, r6, r6, ROR #16

    AND r4, r4, r14
    AND r5, r5, r14
    ORR r4, r4, r5, LSL #16
    MOV r9, #1
    MOV r14, r7
    SUB r5, r7, #1
    MOV r5, r9, LSL r5
  
    MOV  r7, #3    
@    BLS  w_neg_wbi16
  
 @   MOV  r4, r6,
Loop_y1_wbi16:
    LDRB  r9, [r2], #1  @p_Src
    LDRB  r10, [r0], #1 @p_Dst
    ORR   r9, r10, LSL #16
    SMLAD r9, r9, r4, r5

    LDRB  r10, [r2], #1  @p_Src
    LDRB  r11, [r0], #1 @p_Dst
    ORR   r10, r11, LSL #16
    SMLAD r10, r10, r4, r5


    LDRB  r11, [r2], #1  @p_Src
    LDRB  r12, [r0], #1 @p_Dst
    ORR   r11, r12, LSL #16
    SMLAD r11, r11, r4, r5

    ASR   r9, r9, r14
    ASR   r11, r11, r14
    PKHBT  r11, r9, r11, LSL #16   

    LDRB  r12, [r2], #1  @p_Src
    LDRB  r9, [r0], #1 @p_Dst
    ORR   r12, r9, LSL #16
    SMLAD r12, r12, r4, r5

    ASR   r10, r10, r14
    ASR   r12, r12, r14
    
    PKHBT  r12, r10, r12, LSL #16
    
@    AND    r12, r7, R12, LSR r14 
@    AND    r11, r7, R11, LSR r14       
    SADD16 r12, r12, r6
    SADD16 r11, r11, r6
    USAT16 r12, #8, r12
    USAT16 r11, #8, r11
    ORR    r11, r11, r12, LSL #8
    
    SUBS   r7, r7, #1
    STR    r11, [r0, #-4]
    BCS    Loop_y1_wbi16
    MOV    r7, #3 
    SUBS   r8, r8, #1
    SUB    r0, r0, #16
    ADD    r0, r0, r1
    SUB    r2, r2, #16
    ADD    r2, r2, r3       
    BGT    Loop_y1_wbi16    

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  

@//add by Really Yang 20110414
@extern void wbi_8_armv6(VO_U8 *p_Dst,VO_S32 dst_stride, VO_U8 *p_Src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
@                    VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 n_h)
@   p_Dst[0] = clip[rshift_rnd_sf((wp_scale_l0*p_Src[0]+wp_scale_l1*p_Dst[0]),weight_denom)+wp_offset];
@r0 p_Dst
@r1 dst_stride
@r2 p_Src
@r3 src_stride
@r4 wp_scale_l1 | wp_scale_l0
@r5 1 << (weight_denom -1)
@r6 wp_offset
@r7 n_w >> 2
@r8 n_h
wbi_8_armv6:
   STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #wp_scale_l0]
    LDR  r5, [sp, #wp_scale_l1]
    LDR  r6, [sp, #wp_offset]
    LDR  r7, [sp, #wbi_denom]
    LDR  r8, [sp, #wbi_height]
    
@    CMP  r7, #0
 @   LDR  r11, =0x00010001
 @   SMMUL  r3, r3, r11
    LDR  r14, =0xFFFF
    AND r6 , r6, r14
    ORR r6, r6, r6, ROR #16

    AND r4, r4, r14
    AND r5, r5, r14
    ORR r4, r4, r5, LSL #16
    MOV r9, #1
    MOV r14, r7
    SUB r5, r7, #1
    MOV r5, r9, LSL r5
  
    MOV  r7, #1    
@    BLS  w_neg_wbi16
  
 @   MOV  r4, r6,
Loop_y1_wbi8:
    LDRB  r9, [r2], #1  @p_Src
    LDRB  r10, [r0], #1 @p_Dst
    ORR   r9, r10, LSL #16
    SMLAD r9, r9, r4, r5

    LDRB  r10, [r2], #1  @p_Src
    LDRB  r11, [r0], #1 @p_Dst
    ORR   r10, r11, LSL #16
    SMLAD r10, r10, r4, r5


    LDRB  r11, [r2], #1  @p_Src
    LDRB  r12, [r0], #1 @p_Dst
    ORR   r11, r12, LSL #16
    SMLAD r11, r11, r4, r5

    ASR   r9, r9, r14
    ASR   r11, r11, r14
    PKHBT  r11, r9, r11, LSL #16   

    LDRB  r12, [r2], #1  @p_Src
    LDRB  r9, [r0], #1 @p_Dst
    ORR   r12, r9, LSL #16
    SMLAD r12, r12, r4, r5

    ASR   r10, r10, r14
    ASR   r12, r12, r14
    
    PKHBT  r12, r10, r12, LSL #16
    
@    AND    r12, r7, R12, LSR r14 
@    AND    r11, r7, R11, LSR r14       
    SADD16 r12, r12, r6
    SADD16 r11, r11, r6
    USAT16 r12, #8, r12
    USAT16 r11, #8, r11
    ORR    r11, r11, r12, LSL #8
    
    SUBS   r7, r7, #1
    STR    r11, [r0, #-4]
    BCS    Loop_y1_wbi8
    MOV    r7, #1 
    SUBS   r8, r8, #1
    SUB    r0, r0, #8
    ADD    r0, r0, r1
    SUB    r2, r2, #8
    ADD    r2, r2, r3    
    BGT    Loop_y1_wbi8    

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  
  

@//add by Really Yang 20110414
@extern void wbi_4_armv6(VO_U8 *p_Dst,VO_S32 dst_stride, VO_U8 *p_Src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
@                    VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 n_h)
@   p_Dst[0] = clip[rshift_rnd_sf((wp_scale_l0*p_Src[0]+wp_scale_l1*p_Dst[0]),weight_denom)+wp_offset];
@r0 p_Dst
@r1 dst_stride
@r2 p_Src
@r3 src_stride
@r4 wp_scale_l1 | wp_scale_l0
@r5 1 << (weight_denom -1)
@r6 wp_offset
@r7 n_w >> 2
@r8 n_h
wbi_4_armv6:
   STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #wp_scale_l0]
    LDR  r5, [sp, #wp_scale_l1]
    LDR  r6, [sp, #wp_offset]
    LDR  r7, [sp, #wbi_denom]
    LDR  r8, [sp, #wbi_height]
    
@    CMP  r7, #0
 @   LDR  r11, =0x00010001
 @   SMMUL  r3, r3, r11
     LDR  r14, =0xFFFF
    AND r6 , r6, r14
    ORR r6, r6, r6, ROR #16

    AND r4, r4, r14
    AND r5, r5, r14
    ORR r4, r4, r5, LSL #16
    MOV r9, #1
    MOV r14, r7
    SUB r5, r7, #1
    MOV r5, r9, LSL r5
  
@    MOV  r7, #1    
@    BLS  w_neg_wbi16
  
 @   MOV  r4, r6,
Loop_y1_wbi4:
    LDRB  r9, [r2], #1  @p_Src
    LDRB  r10, [r0], #1 @p_Dst
    ORR   r9, r10, LSL #16
    SMLAD r9, r9, r4, r5

    LDRB  r10, [r2], #1  @p_Src
    LDRB  r11, [r0], #1 @p_Dst
    ORR   r10, r11, LSL #16
    SMLAD r10, r10, r4, r5


    LDRB  r11, [r2], #1  @p_Src
    LDRB  r12, [r0], #1 @p_Dst
    ORR   r11, r12, LSL #16
    SMLAD r11, r11, r4, r5

    ASR   r9, r9, r14
    ASR   r11, r11, r14
    PKHBT  r11, r9, r11, LSL #16   

    LDRB  r12, [r2], #1  @p_Src
    LDRB  r9, [r0], #1 @p_Dst
    ORR   r12, r9, LSL #16
    SMLAD r12, r12, r4, r5

    ASR   r10, r10, r14
    ASR   r12, r12, r14
    
    PKHBT  r12, r10, r12, LSL #16
    
@    AND    r12, r7, R12, LSR r14 
@    AND    r11, r7, R11, LSR r14       
    SADD16 r12, r12, r6
    SADD16 r11, r11, r6
    USAT16 r12, #8, r12
    USAT16 r11, #8, r11
    ORR    r11, r11, r12, LSL #8
    
 @   SUBS   r7, r7, #1
    STR    r11, [r0, #-4]!
 @   BCS    Loop_y1_wbi8
 @   MOV    r7, #1 
    SUBS   r8, r8, #1
@    SUB    r0, r0, #4
    ADD    r0, r0, r1
    SUB    r2, r2, #4
    ADD    r2, r2, r3        
    BGT    Loop_y1_wbi4    

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 

@//add by Really Yang 20110414
@extern void wbi_2_armv6(VO_U8 *p_Dst,VO_S32 dst_stride, VO_U8 *p_Src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
@                    VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 n_h)
@   p_Dst[0] = clip[rshift_rnd_sf((wp_scale_l0*p_Src[0]+wp_scale_l1*p_Dst[0]),weight_denom)+wp_offset];
@r0 p_Dst
@r1 dst_stride
@r2 p_Src
@r3 src_stride
@r4 wp_scale_l1 | wp_scale_l0
@r5 1 << (weight_denom -1)
@r6 wp_offset
@r7 n_w >> 2
@r8 n_h
wbi_2_armv6:
   STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #wp_scale_l0]
    LDR  r5, [sp, #wp_scale_l1]
    LDR  r6, [sp, #wp_offset]
    LDR  r7, [sp, #wbi_denom]
    LDR  r8, [sp, #wbi_height]
    
@    CMP  r7, #0
 @   LDR  r11, =0x00010001
 @   SMMUL  r3, r3, r11
     LDR  r14, =0xFFFF
    AND r6 , r6, r14
    ORR r6, r6, r6, ROR #16

    AND r4, r4, r14
    AND r5, r5, r14
    ORR r4, r4, r5, LSL #16
    MOV r9, #1
    MOV r14, r7
    SUB r5, r7, #1
    MOV r5, r9, LSL r5
  
@    MOV  r7, #1    
@    BLS  w_neg_wbi16
  
 @   MOV  r4, r6,
Loop_y1_wbi2:
    LDRB  r9, [r2], #1  @p_Src
    LDRB  r10, [r0], #1 @p_Dst
    ORR   r9, r10, LSL #16
    SMLAD r9, r9, r4, r5

    LDRB  r10, [r2], #1  @p_Src
    LDRB  r11, [r0], #1 @p_Dst
    ORR   r10, r11, LSL #16
    SMLAD r10, r10, r4, r5

    ASR   r10, r10, r14
    ASR   r9, r9, r14
    
    PKHBT  r12, r9, r10, LSL #16
    
@    AND    r12, r7, R12, LSR r14 
@    AND    r11, r7, R11, LSR r14       
    SADD16 r12, r12, r6
    USAT16 r12, #8, r12
    ORR    r12, r12, r12, LSR #8
    
 @   SUBS   r7, r7, #1
    STRH    r12, [r0, #-2]!
 @   BCS    Loop_y1_wbi8
 @   MOV    r7, #1 
    SUBS   r8, r8, #1
@    SUB    r0, r0, #2
    ADD    r0, r0, r1
    SUB    r2, r2, #2
    ADD    r2, r2, r3    
    BGT    Loop_y1_wbi2    

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  

 BI_HEIGHT  = 0x34    

@extern void bi_16_armv6(VO_U8 *p_Dst, VO_S32 dst_stride,VO_U8 *p_Src,VO_S32 src_stride,VO_S32 n_h);
@r0 p_Dst
@r1 dst_stride
@r2 p_Src
@r3 src_stride
@r4 n_h
bi_16_armv6:
    STMFD   sp!, {r0-r11, lr}

    MOV  r11, #1
    LDR  r4, [sp, #BI_HEIGHT]
  
Loop_y_bi16:
    LDR  r5, [r0], #4
    LDR  r6, [r2], #4
    UHSUB8 r5, r6, r5
    USUB8 r6, r6, r5
    LDR  r7, [r0], #4
    LDR  r8, [r2], #4
    UHSUB8 r7, r8, r7
    STR   r6, [r0, #-8]   
    USUB8 r8, r8, r7   
    SUBS    r11, r11, #1
    STR   r8, [r0, #-4]
    BCS     Loop_y_bi16
    MOV     r11, #1 
    SUBS    r4, r4, #1
    SUB     r0, r0, #16
    ADD     r0, r0, r1
    SUB     r2, r2, #16
    ADD     r2, r2, r3    
    BGT     Loop_y_bi16   

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  

@extern void bi_8_armv6(VO_U8 *p_Dst, VO_S32 dst_stride,VO_U8 *p_Src,VO_S32 src_stride,VO_S32 n_h);
@r0 p_Dst
@r1 dst_stride
@r2 p_Src
@r3 src_stride
@r4 n_h	
bi_8_armv6:
    STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #BI_HEIGHT]
  
Loop_y_bi8:
    LDR  r5, [r0], #4
    LDR  r6, [r2], #4
    UHSUB8 r5, r6, r5
    USUB8 r6, r6, r5
    LDR  r7, [r0], #4
    LDR  r8, [r2], #4
    UHSUB8 r7, r8, r7
    STR   r6, [r0, #-8]   
    USUB8 r8, r8, r7   
    STR   r8, [r0, #-4]

    SUBS    r4, r4, #1
    SUB     r0, r0, #8
    ADD     r0, r0, r1
    SUB     r2, r2, #8
    ADD     r2, r2, r3    
    BGT     Loop_y_bi8   

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  

@extern void bi_4_armv6(VO_U8 *p_Dst, VO_S32 dst_stride,VO_U8 *p_Src,VO_S32 src_stride,VO_S32 n_h);
@r0 p_Dst
@r1 dst_stride
@r2 p_Src
@r3 src_stride
@r4 n_h	
bi_4_armv6:
    STMFD   sp!, {r0-r11, lr}
    
    LDR  r4, [sp, #BI_HEIGHT]
Loop_y_bi4:
    LDR  r5, [r0]
    LDR  r6, [r2],r3
    UHSUB8 r5, r6, r5
    USUB8 r6, r6, r5
    STR   r6, [r0],r1   

    SUBS    r4, r4, #1
@    SUB     r0, r0, #4
@    ADD     r0, r0, r1
@    SUB     r2, r2, #4
@    ADD     r2, r2, r3    
    BGT     Loop_y_bi4   

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}  

@extern void bi_2_armv6(VO_U8 *p_Dst, VO_S32 dst_stride,VO_U8 *p_Src,VO_S32 src_stride,VO_S32 n_h);
@r0 p_Dst
@r1 dst_stride
@r2 p_Src
@r3 src_stride
@r4 n_h	
bi_2_armv6:
    STMFD   sp!, {r0-r11, lr}

    LDR  r4, [sp, #BI_HEIGHT]
Loop_y_bi2:
    LDRH  r5, [r0]
    LDRH  r6, [r2],r3
    UHSUB8 r5, r6, r5
    USUB8 r6, r6, r5
    STRH   r6, [r0],r1   

    SUBS    r4, r4, #1
@    SUB     r0, r0, #4
@    ADD     r0, r0, r1
@    SUB     r2, r2, #4
@    ADD     r2, r2, r3    
    BGT     Loop_y_bi2   

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}      
  

@extern void add_luma_00_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride)	
add_luma_00_ARMV6:
    STMFD   sp!, {r0-r11, lr}

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}      
    
	@.end
