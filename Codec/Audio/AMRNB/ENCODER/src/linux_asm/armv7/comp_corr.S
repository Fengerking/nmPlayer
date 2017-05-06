@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void comp_corr ( 
@    Word16 scal_sig[],  /* i   : scaled signal.                          */
@    Word16 lag_max,     /* i   : maximum lag                             */
@    Word16 lag_min,     /* i   : minimum lag                             */
@    Word32 corr[])      /* o   : correlation of selected lag             */
        #include "voAMRNBEncID.h"
        .section  .text
	.global   comp_corr_asm

comp_corr_asm:
        STMFD       r13!, {r4 - r12, r14} 
        MOV         r4, r1                     @r4 = lag_max
  
First_loop:

        VMOV.S32        Q12, #0                              @ Initialise partial accumulators to zero
        MOV             r9, r0
        SUB             r10, r9, r4, LSL #1
       
       
        VLD1.S16        {D0, D1, D2, D3}, [r9]!  
        VLD1.S16        {D4, D5, D6, D7}, [r9]!  
        VLD1.S16        {D8, D9}, [r9]!
                   
        VLD1.S16        {D10, D11, D12, D13}, [r10]!  
        VLD1.S16        {D14, D15, D16, D17}, [r10]! 
        VLD1.S16        {D18, D19}, [r10]!  
                      
                         
        VQDMLAL.S16     Q12, D0, D10
        VQDMLAL.S16     Q12, D1, D11
        VQDMLAL.S16     Q12, D2, D12
        VQDMLAL.S16     Q12, D3, D13
        VQDMLAL.S16     Q12, D4, D14
        VQDMLAL.S16     Q12, D5, D15
        VQDMLAL.S16     Q12, D6, D16
        VQDMLAL.S16     Q12, D7, D17
        VQDMLAL.S16     Q12, D8, D18
        VQDMLAL.S16     Q12, D9, D19  
                
        VLD1.S16        {D0, D1, D2, D3}, [r9]!  
        VLD1.S16        {D4, D5, D6, D7}, [r9]!  
        VLD1.S16        {D8, D9}, [r9]!
                   
        VLD1.S16        {D10, D11, D12, D13}, [r10]!  
        VLD1.S16        {D14, D15, D16, D17}, [r10]! 
        VLD1.S16        {D18, D19}, [r10]!  
                      
                         
        VQDMLAL.S16     Q12, D0, D10
        VQDMLAL.S16     Q12, D1, D11
        VQDMLAL.S16     Q12, D2, D12
        VQDMLAL.S16     Q12, D3, D13
        VQDMLAL.S16     Q12, D4, D14
        VQDMLAL.S16     Q12, D5, D15
        VQDMLAL.S16     Q12, D6, D16
        VQDMLAL.S16     Q12, D7, D17
        VQDMLAL.S16     Q12, D8, D18
        VQDMLAL.S16     Q12, D9, D19  
           

        VQADD.S32       D24, D24, D25

        VPADD.S32       D24, D24, D24
        VMOV.S32        r5,  D24[0]
        
        SUB             r11, r3, r4, LSL #2
        SUB             r4, r4, #1        
        STR             r5, [r11]
        CMP             r4, r2
        BGE             First_loop

comp_corr_end:  
        LDMFD           r13!, {r4 - r12, r15}
        .END
