@/*
@*  Copyright 2003 ~ 2009 by Visualon software Incorporated.
@*  All rights reserved. Property of Visualon software Incorporated.
@*  Restricted rights to use, duplicate or disclose this code are
@*  granted through contract.
@*  
@*/
@/***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver         Description             Author
@*    -----------     --------     ------------------        -----------
@*    07-20-2008        1.0        File imported from        Huaping Liu
@*                                             
@**********************************************************************/
@void Scale_sig(
@	       Word16 x[],                           /* (i/o) : signal to scale               */
@	       Word16 lg,                            /* (i)   : size of x[]                   */
@	       Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
@	       )
@r0 --- x[]
@r1 --- lg
@r2 --- exp
          #include "voAMRWBEncID.h"
          .text   .align 4 
	  .globl   _Scale_sig_opt

_Scale_sig_opt:

         stmfd         r13!, {r4 - r12, r14}
     
	 sub           r3, r1, #1                  @i = lg - 1
         cmp           r2, #0                      @Compare exp and 0
	 rsb           r7, r2, #0                  @exp = -exp
	 add           r10, r2, #16                @16 + exp
         add           r4, r0, r3, lsl #1          @x[i] address
	 mov           r8, #0x7fffffff
	 mov           r9, #0x8000
	 ble           LOOP2
	 
LOOP1:
         ldrsh          r5, [r4]                    @load x[i]
         mov           r12, r5, lsl r10
	 teq           r5, r12, asr r10
	 eorne         r12, r8, r5, asr #31
	 subs          r3, r3, #1
	 qadd          r11, r12, r9
	 mov           r12, r11, asr #16
	 strh          r12, [r4], #-2
	 bge           LOOP1
         bl            The_end

LOOP2:
         ldrsh          r5, [r4]                   @load x[i]
	 mov           r6, r5, lsl #16            @L_tmp = x[i] << 16
	 mov           r5, r6, asr r7             @L_tmp >>= exp
	 qadd          r11, r5, r9
	 mov           r12, r11, asr #16
	 subs          r3, r3, #1
	 strh          r12, [r4], #-2
	 bge           LOOP2

The_end:

         ldmfd         r13!, {r4 - r12, r15}
     
         @.END	 
        
	
	  


