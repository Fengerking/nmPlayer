; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: byte_swap.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
;  
; REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM   
; Portions Copyright (c) 1995-2005 RealNetworks, Inc.   
; All Rights Reserved.   
;   
; The contents of this file, and the files included with this file, 
; are subject to the current version of the RealNetworks Community 
; Source License (the "RCSL"), including Attachment G and any 
; applicable attachments, all available at 
; http://www.helixcommunity.org/content/rcsl.  You may also obtain 
; the license terms directly from RealNetworks.  You may not use this 
; file except in compliance with the RCSL and its Attachments. There 
; are no redistribution rights for the source code of this 
; file. Please see the applicable RCSL for the rights, obligations 
; and limitations governing use of the contents of the file. 
;   
; This file is part of the Helix DNA Technology. RealNetworks is the 
; developer of the Original Code and owns the copyrights in the 
; portions it created. 
;   
; This file, and the files included with this file, is distributed 
; and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY 
; KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS 
; ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES 
; OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET 
; ENJOYMENT OR NON-INFRINGEMENT. 
;   
; Technology Compatibility Kit Test Suite(s) Location:   
; https://rarvcode-tck.helixcommunity.org   
;   
; Contributor(s):   
;   
; ***** END LICENSE BLOCK ***** 

;===============================================================================
; FILE NAME : byte_swap.s
;===============================================================================
;   Version Number          Revision Date               Author's Name
;   ==============          =============               =============
;   Initial Version         4th Feb,2004              Raghvendra Kamathankar
;===============================================================================
; CodeGen Tools Version :  2.50   
; Code Composer Version :  2.1  
; List of functions in the file : _byte_swap
;===============================================================================
; INCLUDE FILES
;===============================================================================
          .mmregs
          .text  
;===============================================================================
;void byte_swap(
;                  int buffer[]         :XAR0   (i/o) 
;                  int length           :T0     (i)   
;              )

;===============================================================================
; EXPORT FUNCTIONS:
         .def        _RA_TNI_byte_swap
         .def        _RA_TNI_byte_shift
         .def        _RA_TNI_byte_swap_shift
;===============================================================================

_RA_TNI_byte_swap:
		 .noremark 5688
		 .noremark 5673
         MOV         XAR0,XAR1
         ; if T0 == 0 return
         RETCC 		T0 == #0	
         SUB         #1,T0
         MOV         T0,BRC0
         BCLR        SXMD

         MOV         *AR0+,AC0
         AND         #0x00FF,AC0,AC2
       ||RPTBLOCAL   swap_loop-1
         SFTS        AC2,#8
         OR          AC0<<#-8,AC2
       ||MOV         *AR0+,AC0
         MOV         AC2,*AR1+
       ||AND         #0x00FF,AC0,AC2
swap_loop:
         BSET        SXMD
         .remark 5688
		 .remark 5673
         RET

_RA_TNI_byte_shift:
		 .noremark 5688
		 .noremark 5673
         MOV         XAR0,XAR1
         ; if T0 == 0 return
         RETCC 		T0 == #0
         SUB         #1,T0
         MOV         T0,BRC0
         BCLR        SXMD

         MOV         #0xFF00,T1
         MOV         #0x00FF,T0

         AND         *AR0+,T0,AC0
         RPTBLOCAL   shift_loop-1
         SFTS        AC0,#8,AC2
         AND         *AR0,T1,AC1
         OR          AC1<<#-8,AC2
         MOV         AC2,*AR1+
       ||AND         *AR0+,T0,AC0         
shift_loop:
         BSET        SXMD
		 .remark 5688
		 .remark 5673         
         RET


_RA_TNI_byte_swap_shift:
		 .noremark 5688
         MOV         XAR0,XAR1
         SUB         #1,T0
         MOV         T0,BRC0
         BCLR        SXMD
         MOV         #0xFF00,T0
         MOV         #0x00FF,T1

         AND         *AR0+,T0,AC0
         
       ||RPTBLOCAL   swap_shift_loop-1
         AND         *AR0,T1,AC1
         OR          AC0,AC1
         MOV         AC1,*AR1+
       ||AND         *AR0+,T0,AC0         
swap_shift_loop:
         BSET        SXMD
		 .remark 5688         
         RET     
         
