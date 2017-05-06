	AREA	|.rdata|, DATA, READONLY
|dcttab|
	DCD	0x4013c251
	DCD	0x518522fb
	DCD	0x404f4672
	DCD	0x40b345bd
	DCD	0x6d0b20cf
	DCD	0x42e13c10
	DCD	0x41fa2d6d
	DCD	0x41d95790
	DCD	0x48919f44
	DCD	0x43f93421
	DCD	0x5efc8d96
	DCD	0x52cb0e63
	DCD	0x46cc1bc4
	DCD	0x4ad81a97
	DCD	0x64e2402e
	DCD	0x4a9d9cf0
	DCD	0x7c7d1db3
	DCD	0x43e224a9
	DCD	0x4fae3711
	DCD	0x6b6fcf26
	DCD	0x6e3c92c1
	DCD	0x56601ea7
	DCD	0x5f4cf6eb
	DCD	0x519e4e04
	DCD	0x4140fb46
	DCD	0x52036742
	DCD	0x4545e9ef
	DCD	0x4cf8de88
	DCD	0x73326bbf
	DCD	0x539eba45
	DCD	0xbebf04ba
	DCD	0xadfc98be
	DCD	0x4545e9ef
	DCD	0xb3072178
	DCD	0x8ccd9441
	DCD	0x539eba45
	DCD	0x4140fb46
	DCD	0x52036742
	DCD	0x4545e9ef
	DCD	0x4cf8de88
	DCD	0x73326bbf
	DCD	0x539eba45
	DCD	0xbebf04ba
	DCD	0xadfc98be
	DCD	0x4545e9ef
	DCD	0xb3072178
	DCD	0x8ccd9441
	DCD	0x539eba45
	
	EXPORT	|voMP3DecDCT32|

	AREA	|.text| , CODE, READONLY

|voMP3DecDCT32|	PROC
	stmdb     sp!, {r4 - r11, lr}
	sub       sp, sp, #0x18
|$M660|
	mov       r12, r0
	str       r1,  [sp, #0x4]	
	str       r2,  [sp, #0x8]
	str       r3,  [sp, #0xC]

	ldr		  r14, |$L664| + 4
	
; 80   :     int i, s, tmp, es;
; 81   :     const int *cptr = dcttab;
; 82   :     int a0, a1, a2, a3, a4, a5, a6, a7;
; 83   :     int b0, b1, b2, b3, b4, b5, b6, b7;
; 84   : 	int *d;
; 85   : 
; 86   : 	/* first pass */    
; 87   : 	D32FP(0, 1, 5, 1);
; 88   : 	D32FP(1, 1, 3, 1);
	ldr		  r0, [r12, #0x0]	
	ldr		  r1, [r12, #0x4]	
	ldr       r2, [r12, #0x38]
	ldr       r3, [r12, #0x3C]
	ldr 	  r4, [r12, #0x40]
	ldr 	  r5, [r12, #0x44]
	ldr 	  r6, [r12, #0x78]
	ldr 	  r7, [r12, #0x7C]	
	ldr       r10, [r14], #4
	ldr       r11, [r14], #4

	;r0 = a0; r3 = a1; r4 = a2; r7 = a3 
	sub		  r9, r0, r7				;b3 = a0 - a3
	add		  r8, r0, r7				;b0 = a0 + a3
	mov		  r9, r9, lsl #1
	sub		  r7, r3, r4				;b2 = a1 - a2;
	smull	  r0, r9, r10, r9
	mov		  r7, r7, lsl #5
	add		  r0, r3, r4				;b1 = a1 + a2;	
	smull	  r10, r7, r11, r7
	sub		  r3, r8, r0				; b0 - b1	
	add		  r4, r8, r0				; b0 + b1
	ldr       r10, [r14], #4
	mov		  r3, r3, lsl #1
	ldr       r11, [r14], #4	
	smull	  r0, r3, r10, r3
	sub		  r8, r9, r7 				; b3 - b2	
	add		  r0, r9, r7				; b3 + b2	
	mov		  r9, r8, lsl #1
	smull 	  r7, r9, r10, r9
	
	;r1 = a0; r2 = a1; r5 = a2; r6 = a3 
	sub		  r8, r1, r6				;b3 = a0 - a3
	add		  r7, r1, r6				;b0 = a0 + a3
	mov		  r8, r8, lsl #1
	sub		  r6, r2, r5				;b2 = a1 - a2
	smull	  r10, r8, r11, r8
	add		  r1, r2, r5				;b1 = a1 + a2
	ldr       r10, [r14], #4
	mov		  r6, r6, lsl #3
	ldr       r11, [r14], #4
	smull	  r2, r6, r10, r6
	add		  r5, r7, r1				; b0 + b1
	sub		  r2, r7, r1				; b0 - b1	
	add		  r1, r8, r6				; b3 + b2
	mov		  r2, r2, lsl #1
	sub		  r7, r8, r6				; b3 - b2
	smull	  r10, r2, r11, r2	
	str 	  r4, [r12, #0x0]
	str 	  r5, [r12, #0x4]
	mov		  r7, r7, lsl #1
	str 	  r2, [r12, #0x38]
	str 	  r3, [r12, #0x3C]
	smull	  r10, r8, r7, r11
	str 	  r0, [r12, #0x40]
	str 	  r1, [r12, #0x44]
	str 	  r8, [r12, #0x78]	
	str 	  r9, [r12, #0x7C]	
	
; 89   : 	D32FP(2, 1, 3, 1);
; 90   : 	D32FP(3, 1, 2, 1);

	ldr 	  r0, [r12, #0x8]	
	ldr 	  r1, [r12, #0xC]	
	ldr       r2, [r12, #0x30]
	ldr       r3, [r12, #0x34]
	ldr 	  r4, [r12, #0x48]
	ldr		  r5, [r12, #0x4C]
	ldr 	  r6, [r12, #0x70]	
	ldr 	  r7, [r12, #0x74]	
	ldr       r10, [r14], #4
	ldr       r11, [r14], #4
	sub		  r9, r0, r7				;b3 = a0 - a3
	add		  r8, r0, r7				;b0 = a0 + a3
	mov		  r9, r9, lsl #1
	sub		  r7, r3, r4				;b2 = a1 - a2;
	smull	  r0, r9, r10, r9
	mov		  r7, r7, lsl #3
	add		  r0, r3, r4				;b1 = a1 + a2;	
	smull	  r10, r7, r11, r7
	sub		  r3, r8, r0				; b0 - b1	
	add		  r4, r8, r0				; b0 + b1
	ldr       r10, [r14], #4
	mov		  r3, r3, lsl #1
	ldr       r11, [r14], #4
	smull	  r0, r3, r10, r3
	sub		  r8, r9, r7 				; b3 - b2	
	add		  r0, r9, r7				; b3 + b2	
	mov		  r9, r8, lsl #1
	smull 	  r7, r9, r10, r9
	
	;r1 = a0; r2 = a1; r5 = a2; r6 = a3 
	sub		  r8, r1, r6				;b3 = a0 - a3
	add		  r7, r1, r6				;b0 = a0 + a3
	mov		  r8, r8, lsl #1
	sub		  r6, r2, r5				;b2 = a1 - a2
	smull	  r10, r8, r11, r8
	add		  r1, r2, r5				;b1 = a1 + a2
	ldr       r10, [r14], #4
	mov		  r6, r6, lsl #2
	ldr       r11, [r14], #4
	smull	  r2, r6, r10, r6
	add		  r5, r7, r1				; b0 + b1	
	sub		  r2, r7, r1				; b0 - b1
	add		  r1, r8, r6				; b3 + b2
	mov		  r2, r2, lsl #1
	sub		  r7, r8, r6				; b3 - b2
	smull	  r10, r2, r11, r2
	str 	  r4, [r12, #0x8]
	str 	  r5, [r12, #0xC]
	mov		  r7, r7, lsl #1
	str 	  r2, [r12, #0x30]
	str 	  r3, [r12, #0x34]
	smull	  r10, r8, r7, r11
	str 	  r0, [r12, #0x48]
	str 	  r1, [r12, #0x4C]
	str 	  r8, [r12, #0x70]	
	str 	  r9, [r12, #0x74]	
		
;	D32FP(4, 1, 2, 1);
;	D32FP(5, 1, 1, 2);
	ldr 	  r0, [r12, #0x10]	
	ldr 	  r1, [r12, #0x14]	
	ldr       r2, [r12, #0x28]
	ldr       r3, [r12, #0x2C]
	ldr 	  r4, [r12, #0x50]
	ldr 	  r5, [r12, #0x54]
	ldr 	  r6, [r12, #0x68]
	ldr 	  r7, [r12, #0x6C]	
	ldr       r10, [r14], #4
	ldr       r11, [r14], #4
	sub		  r9, r0, r7				;b3 = a0 - a3
	add		  r8, r0, r7				;b0 = a0 + a3
	mov		  r9, r9, lsl #1
	sub		  r7, r3, r4				;b2 = a1 - a2;
	smull	  r0, r9, r10, r9
	mov		  r7, r7, lsl #2
	add		  r0, r3, r4				;b1 = a1 + a2;	
	smull	  r10, r7, r11, r7
	sub		  r3, r8, r0				; b0 - b1	
	add		  r4, r8, r0				; b0 + b1
	ldr       r10, [r14], #4
	mov		  r3, r3, lsl #1
	ldr       r11, [r14], #4
	smull	  r0, r3, r10, r3
	sub		  r8, r9, r7 				; b3 - b2	
	add		  r0, r9, r7				; b3 + b2
	mov		  r9, r8, lsl #1
	smull 	  r7, r9, r10, r9
	
	;r1 = a0; r2 = a1; r5 = a2; r6 = a3 
	sub		  r8, r1, r6				;b3 = a0 - a3
	add		  r7, r1, r6				;b0 = a0 + a3
	mov		  r8, r8, lsl #1
	sub		  r6, r2, r5				;b2 = a1 - a2
	smull	  r10, r8, r11, r8
	add		  r1, r2, r5				;b1 = a1 + a2
	ldr       r10, [r14], #4
	mov		  r6, r6, lsl #1
	ldr       r11, [r14], #4
	smull	  r2, r6, r10, r6
	add		  r5, r7, r1				; b0 + b1
	sub		  r2, r7, r1				; b0 - b1		
	add		  r1, r8, r6				; b3 + b2
	mov		  r2, r2, lsl #2
	sub		  r7, r8, r6				; b3 - b2
	smull	  r10, r2, r11, r2
	str 	  r4, [r12, #0x10]
	str 	  r5, [r12, #0x14]
	mov		  r7, r7, lsl #2
	str 	  r2, [r12, #0x28]
	str 	  r3, [r12, #0x2C]
	smull	  r10, r8, r7, r11
	str 	  r0, [r12, #0x50]
	str 	  r1, [r12, #0x54]
	str 	  r8, [r12, #0x68]
	str 	  r9, [r12, #0x6C]			

;	D32FP(6, 1, 1, 2);
;	D32FP(7, 1, 1, 4);		
	ldr 	  r0, [r12, #0x18]	
	ldr 	  r1, [r12, #0x1C]	
	ldr       r2, [r12, #0x20]
	ldr       r3, [r12, #0x24]
	ldr 	  r4, [r12, #0x58]
	ldr 	  r5, [r12, #0x5C]
	ldr 	  r6, [r12, #0x60]
	ldr 	  r7, [r12, #0x64]	
	ldr       r10, [r14], #4
	ldr       r11, [r14], #4
	sub		  r9, r0, r7				;b3 = a0 - a3
	add		  r8, r0, r7				;b0 = a0 + a3
	mov		  r9, r9, lsl #1
	sub		  r7, r3, r4				;b2 = a1 - a2;
	smull	  r0, r9, r10, r9
	mov		  r7, r7, lsl #1
	add		  r0, r3, r4				;b1 = a1 + a2;	
	smull	  r10, r7, r11, r7
	sub		  r3, r8, r0				; b0 - b1	
	add		  r4, r8, r0				; b0 + b1
	ldr       r10, [r14], #4
	mov		  r3, r3, lsl #2
	ldr       r11, [r14], #4	
	smull	  r0, r3, r10, r3
	sub		  r8, r9, r7 				; b3 - b2	
	add		  r0, r9, r7				; b3 + b2	
	mov		  r9, r8, lsl #2
	smull 	  r7, r9, r10, r9
	
	;r1 = a0; r2 = a1; r5 = a2; r6 = a3 
	sub		  r8, r1, r6				;b3 = a0 - a3
	add		  r7, r1, r6				;b0 = a0 + a3
	mov		  r8, r8, lsl #1
	sub		  r6, r2, r5				;b2 = a1 - a2
	smull	  r10, r8, r11, r8
	add		  r1, r2, r5				;b1 = a1 + a2
	ldr       r10, [r14], #4
	mov		  r6, r6, lsl #1
	ldr       r11, [r14], #4	
	smull	  r2, r6, r10, r6
	add		  r5, r7, r1				;b0 + b1
	sub		  r2, r7, r1				;b0 - b1		
	add		  r1, r8, r6				;b3 + b2
	mov		  r2, r2, lsl #4
	sub		  r7, r8, r6				;b3 - b2
	smull	  r10, r2, r11, r2
	str 	  r4, [r12, #0x18]
	str 	  r5, [r12, #0x1C]
	mov		  r7, r7, lsl #4
	str 	  r2, [r12, #0x20]
	str 	  r3, [r12, #0x24]
	smull	  r10, r8, r7, r11
	str 	  r0, [r12, #0x58]
	str 	  r1, [r12, #0x5C]
	str 	  r8, [r12, #0x60]
	str 	  r9, [r12, #0x64]

	ldr		  r6, |$L664|
	mov		  r7, #4
	str 	  r6, [sp, #0x10]
	str 	  r7, [sp, #0x14]
	
|$L659|	
; 95   : 
; 96   : 	/* second pass */
; 97   : 	for (i = 4; i > 0; i--) {
; 98   : 		a0 = buf[0]; 	    a7 = buf[7];		a3 = buf[3];	    a4 = buf[4];

	ldr        r0, [r12, #0]
	ldr        r1, [r12, #4]
	ldr        r2, [r12, #0x8]
	ldr        r3, [r12, #0xC]
	ldr        r4, [r12, #0x10]
	ldr        r5, [r12, #0x14]
	ldr        r6, [r12, #0x18]
	ldr        r7, [r12, #0x1C]
; 99   : 		b0 = a0 + a7;	    b7 = MUL_32(*cptr++, a0 - a7) << 1;
; 100  : 		b3 = a3 + a4;	    b4 = MUL_32(*cptr++, a3 - a4) << 3;
; 101  : 		a0 = b0 + b3;	    a3 = MUL_32(*cptr,   b0 - b3) << 1;
; 102  : 		a4 = b4 + b7;		a7 = MUL_32(*cptr++, b7 - b4) << 1;
; 103  : 
; 104  : 		a1 = buf[1];	    a6 = buf[6];	    a2 = buf[2];	    a5 = buf[5];
	ldr       r10, [r14], #4
	ldr       r11, [r14], #4
	sub		  r9, r0, r7
	add		  r8, r0, r7
	mov		  r9, r9, lsl #1
		
	sub		  r7, r3, r4
	smull	  r0, r9, r10, r9
	mov		  r7, r7, lsl #3
	add		  r0, r3, r4	
	smull     r10, r7, r11, r7
	
	sub		  r4, r8, r0
	add		  r3, r8, r0
	mov		  r4, r4, lsl #1
	ldr       r10, [r14], #4
	ldr       r11, [r14], #4
	sub		  r8, r9, r7
	smull	  r0, r4, r10, r4
	
	mov		  r8, r8, lsl #1
	add		  r0, r9, r7	
	smull	  r7, r8, r10, r8		
	 	
; 105  : 		b1 = a1 + a6;	    b6 = MUL_32(*cptr++, a1 - a6) << 1;
; 106  : 		b2 = a2 + a5;	    b5 = MUL_32(*cptr++, a2 - a5) << 1;
; 107  : 		a1 = b1 + b2;	    a2 = MUL_32(*cptr,   b1 - b2) << 2;
; 108  : 		a5 = b5 + b6;	    a6 = MUL_32(*cptr++, b6 - b5) << 2;
	sub		  r9, r1, r6
	add		  r7, r1, r6
	mov		  r9, r9, lsl #1
		
	sub		  r6, r2, r5
	smull	  r10, r9, r11, r9		
	add		  r1, r2, r5
	mov		  r6, r6, lsl #1
	ldr       r10, [r14], #4
	ldr       r11, [r14], #4
	sub		  r5, r7, r1
	smull	  r2, r6, r10, r6
	
	mov		  r5, r5, lsl #2
	add		  r2, r7, r1
	smull	  r10, r5, r11, r5
	
	sub		  r7, r9, r6
	add		  r1, r9, r6
	mov		  r7, r7, lsl #2
	
; 110  : 		b0 = a0 + a1;	    b1 = MUL_32(COS4_0, a0 - a1) << 1;
; 111  : 		b2 = a2 + a3;	    b3 = MUL_32(COS4_0, a3 - a2) << 1;

	add		  r6, r3, r2
	smull	  r10, r9, r7, r11
	sub		  r7, r3, r2
	ldr 	  r11, [sp, #0x14]
	ldr 	  r10, [sp, #0x10]
	mov		  r7, r7, lsl #1
		
	add		  r2, r4, r5
	smull	  r3, r7, r10, r7
	str 	  r6, [r12, #0]		; 112  : 		buf[0] = b0;	    buf[1] = b1;
	str	      r7, [r12, #4]		; 112  : 		buf[0] = b0;	    buf[1] = b1;
	sub		  r3, r4, r5
	sub		  r7, r0, r1
	mov		  r3, r3, lsl #1
	add		  r6, r0, r1
	smull	  r4, r3, r10, r3	

; 115  : 		b4 = a4 + a5;	    b5 = MUL_32(COS4_0, a4 - a5) << 1;
; 116  : 		b6 = a6 + a7;	    b7 = MUL_32(COS4_0, a7 - a6) << 1;
; 117  : 		b6 += b7;
; 118  : 		buf[4] = b4 + b6;	buf[5] = b5 + b7;
; 119  : 		buf[6] = b5 + b6;	buf[7] = b7;

	mov		  r7, r7, lsl #1
	add		  r2, r2, r3	
	str 	  r2, [r12, #0x8]	; 113  : 		buf[2] = b2 + b3;	buf[3] = b3;
	str 	  r3, [r12, #0xC]	; 113  : 		buf[2] = b2 + b3;	buf[3] = b3;
	smull	  r0, r7, r10, r7	
	sub		  r3, r8, r9
	add		  r2, r8, r9
	mov		  r3, r3, lsl #1	
	smull	  r9, r5, r3, r10
	
	add		  r2, r5, r2
	add		  r1, r7, r5
	add		  r0, r6, r2
	add		  r4, r7, r2
	
	str 	  r0, [r12, #0x10]
	str 	  r1, [r12, #0x14]
	str 	  r4, [r12, #0x18]
	str 	  r5, [r12, #0x1C]

	sub		  r11, r11,#1
	add		  r12, r12, #32
	str		  r11, [sp, #0x14]
	cmp       r11, #0
	bhi       |$L659|
	
	sub		  r14, r12, #0x80

	ldr       r3,  [sp, #0xC]
	ldr       r2,  [sp, #0x8]
	ldr       r1,  [sp, #0x4]	
    sub		  r12, r2, r3 
	ldr		  r4, [r14, #0]  
	cmp		  r3, #0                     
   	moveq	  r10, #1088                  
   	movne	  r10, #0               
    and		  r12, r12, #7               
    mov		  r4, r4, asr #7             
    add		  r12, r10, r12			;((offset - oddBlock) & 7) + (oddBlock ? 0 : VBUF_LENGTH);
    cmp		  r3, #0      
    add		  r12, r1, r12, lsl #2  ;d = dest + ((offset - oddBlock) & 7) + (oddBlock ? 0 : VBUF_LENGTH);    
    add		  r0, r12, #4096 
    str		  r4, [r0, #32]              
    str		  r4, [r0, #0]               
    moveq	  r11, #0                     
    movne	  r11, #1088                  

    ldr		  r4, [r14, #4]                    
    add		  r2, r1, r2, lsl #2  
    ldr       r9, [r14, #116]
    add		  r1, r2, r11, lsl #2    ;d = dest + offset + (oddBlock ? VBUF_LENGTH  : 0);     
  	mov		  r4, r4, asr #7         ;s = buf[ 1];				d[0] = d[8] = s >> 7;	d += 64;  
    ldr		  r11, [r14, #100] 	 
	ldr       r2, [r14, #68] 	
    add	      r0, r11, r9            ;tmp = buf[25] + buf[29]; 	        
    str		  r4, [r1, #32]     
    add	      r3, r2, r0 	            
    str		  r4, [r1, #0]            
                   
    mov		  r3, r3, asr #7         ;s = buf[17] + tmp;			d[0] = d[8] = s >> 7;	d += 64;             
    str       r3, [r1, #288]                  
    str       r3, [r1, #256] 	 
	ldr       r8, [r14, #52] 	                                 
    ldr		  r5, [r14, #36]                            
    ldr       r7, [r14, #84]   	               
    add       r3, r5, r8 
    add       r0, r7, r0  	        
    mov       r3, r3, asr #7         ;s = buf[ 9] + buf[13];		d[0] = d[8] = s >> 7;	d += 64; 
	mov       r0, r0, asr #7         ;s = buf[21] + tmp;			d[0] = d[8] = s >> 7;	d += 64;        
	str       r3, [r1, #544]                      
    str       r3, [r1, #512]   
    str       r0, [r1, #800]  	           
    str       r0, [r1, #768]  
    ldr       r6, [r14, #108]         
    ldr       r3, [r14, #20]            
    add		  r0, r6, r9              ;tmp = buf[29] + buf[27];
    mov		  r3, r3, asr #7          ;d[0] = d[8] = s >> 7;	d += 64;
    add		  r4, r7, r0 
    str       r3, [r1, #1056]           
    str       r3, [r1, #1024]   
    ldr       r7, [r14, #44] 	        
    mov       r3, r4, asr #7          ;s = buf[21] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
    add       r4, r8, r7 
    ldr       r9, [r14, #76] 
    str       r3, [r1, #1312]  	         
    str       r3, [r1, #1280]
    add       r0, r9, r0 	                 
    mov       r3, r4, asr #7           ;s = buf[13] + buf[11];		d[0] = d[8] = s >> 7;	d += 64;
	mov       r0, r0, asr #7           ;s = buf[19] + tmp;			d[0] = d[8] = s >> 7;	d += 64;  
    str		  r3, [r1, #1568] 
    str		  r3, [r1, #1536]          
    str       r0, [r1, #1824]           
    str       r0, [r1, #1792]          
      
    ldr       r10, [r14, #124]         ;tmp = buf[31];
    ldr       r3, [r14, #12]          
    add       r0, r6, r10              ;tmp = buf[27] + buf[31];
    mov       r3, r3, asr #7          
    str       r3, [r1, #2080]         
    str       r3, [r1, #2048]          ;s = buf[ 3];			    d[0] = d[8] = s >> 7;	d += 64;
    add		  r4, r9, r0              
    mov       r3, r4, asr #7           ;s = buf[19] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
    ldr       r9, [r14, #60]
    str       r3, [r1, #2336]         
    str       r3, [r1, #2304]  		         
    add       r3, r9, r7               ;s = buf[11] + buf[15];		d[0] = d[8] = s >> 7;	d += 64; 
    ldr       r8, [r14, #92] 
    mov       r3, r3, asr #7   
    add       r0, r8, r0               ;s = buf[23] + tmp;			d[0] = d[8] = s >> 7;	d += 64;	       
    str       r3, [r1, #2592]         
    mov       r7, r0, asr #7     
    str		  r3, [r1, #2560]  	     
    str       r7, [r1, #2848]         
    str       r7, [r1, #2816]         
    ldr       r4, [r14, #28]          
    mov       r3, r10, asr #7          
    mov       r4, r4, asr #7           ;s = buf[ 7];				d[0] = d[8] = s >> 7;	d += 64;
    add       r0, r8, r10              ;s = buf[23] + tmp;			d[0] = d[8] = s >> 7;	d += 64; 
    str       r4, [r1, #3104]      
    mov       r7, r0, asr #7 	   
    str       r4, [r1, #3072]              
    str       r7, [r1, #3360]         
    str       r7, [r1, #3328]     
    mov       r0, r9, asr #7           ;s = buf[15];				d[0] = d[8] = s >> 7;	d += 64;	   
    str       r3, [r1, #3872]         
    str       r3, [r1, #3840]         
    str       r0, [r1, #3616]         
    str       r0, [r1, #3584]         
 
    ldr       r1, [r14, #4]           
    mov		  r0, r12      
	mov       r4, r1, asr #7           ;s = buf[ 1];				d[0] = d[8] = s >> 7;	d += 64;
    ldr       r9, [r14, #120]  
    ldr       r8, [r14, #56]  
    add       r1, r9, r11              ;tmp = buf[30] + buf[25];  
    str       r4, [r0, #96]          
    add       r2, r2, r1               ;s = buf[17] + tmp;			d[0] = d[8] = s >> 7;	d += 64;	 
    str		  r4, [r0, #64]     
    mov       r2, r2, asr #7    
    add       r4, r5, r8               ;s = buf[14] + buf[ 9];		d[0] = d[8] = s >> 7;	d += 64;	       
    str       r2, [r0, #352]          
    str       r2, [r0, #320]        
    ldr       r6, [r14, #88]
    mov       r2, r4, asr #7    
	add       r1, r6, r1               ;s = buf[22] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
    ldr       r5, [r14, #104] 		      
    str       r2, [r0, #608]          
    str       r2, [r0, #576]          
    mov       r1, r1, asr #7   
	ldr       r2, [r14, #24]      
    str       r1, [r0, #864]         
    str       r1, [r0, #832]                 
    mov       r4, r2, asr #7           ;s = buf[ 6];				d[0] = d[8] = s >> 7;	d += 64;
    add       r1, r5, r9               ;tmp = buf[26] + buf[30];
    str       r4, [r0, #1120]        
    add       r2, r6, r1               ;s = buf[22] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
    str       r4, [r0, #1088]    
    mov       r2, r2, asr #7        
    ldr       r9, [r14, #40]  
	ldr       r6, [r14, #72]  
    str       r2, [r0, #1376]       
	add       r4, r9, r8               ;s = buf[10] + buf[14];		d[0] = d[8] = s >> 7;	d += 64
    str       r2, [r0, #1344]          
    mov       r2, r4, asr #7        
	add       r1, r6, r1               ;s = buf[18] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
    str       r2, [r0, #1632]       
    mov       r1, r1, asr #7 
	ldr       r4, [r14, #8]            ;s = buf[ 2];				d[0] = d[8] = s >> 7;	d += 64; 
    ldr       r8, [r14, #112]  
    str       r2, [r0, #1600]                
    str       r1, [r0, #1888]             
    str       r1, [r0, #1856]  
    ldr       r7, [r14, #80]	    
    add       r1, r8, r5               ;tmp = buf[28] + buf[26];	   
    mov       r2, r4, asr #7              
    ldr       r5, [r14, #48]    
    str       r2, [r0, #2144]        
    add       r4, r6, r1		       ;s = buf[18] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
    str       r2, [r0, #2112]   
    mov       r4, r4, asr #7    
    add       r2, r5, r9                ;s = buf[12] + buf[10];		d[0] = d[8] = s >> 7;	d += 64;
    str       r4, [r0, #2400]   
    mov       r2, r2, asr #7 
    str       r4, [r0, #2368]       
    add       r1, r1, r7                ;s = buf[20] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
    str       r2, [r0, #2656]  
    mov       r1, r1, asr #7  	 
    str       r2, [r0, #2624]     
    str       r1, [r0, #2912]   
    str       r1, [r0, #2880]   
    ldr       r2, [r14, #16]    
    ldr       r3, [r14, #96]  		
    mov       r4, r2, asr #7            ;s = buf[ 4];				d[0] = d[8] = s >> 7;	d += 64;
    add       r1, r3, r8                ;tmp = buf[24] + buf[28];
    str       r4, [r0, #3168]   
    add       r2, r7, r1                ;s = buf[20] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
    str       r4, [r0, #3136] 
    mov       r2, r2, asr #7 
    ldr       r3, [r14, #32]       
    str       r2, [r0, #3424]   
    add       r4, r3, r5		    	;s = buf[ 8] + buf[12];		d[0] = d[8] = s >> 7;	d += 64;
    str       r2, [r0, #3392]  
    ldr       r3, [r14, #64] 	 	
    mov       r2, r4, asr #7  
    add       r1, r3, r1  	  
    str       r2, [r0, #3680]   
    mov       r1, r1, asr #7		    ;s = buf[16] + tmp;			d[0] = d[8] = s >> 7; 
    str       r2, [r0, #3648]     
    str       r1, [r0, #3936]   
    str       r1, [r0, #3904]   

	add       sp, sp, #0x18
	ldmia     sp!, {r4 - r11, pc}
|$L664|
	DCD       0x5a82799a
	DCD       |dcttab|
|$M661|

	ENDP  ; |voMP3DecDCT32|
	END
