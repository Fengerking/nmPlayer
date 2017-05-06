	.text
	  .align 2
    .globl  _Chroma_InPlaceDeblock
_Chroma_InPlaceDeblock:
        @push     {r0-r11,lr}
		stmfd   r13!, {r0-r11, r14} @  14_1

        sub      sp,sp,#4
        ldr      r8,[sp,#0x38]
        ldr      r11,[sp,#0x3c]
        ldr      lr,[sp,#0x40]
        rsb      r8,r8,#0
        mov      r1,#0
        str      r8,[sp,#0]
al1.32a:
        ldr      r12,[sp,#8]
        ldrb     r5,[r0,#0]
        ldrb     r6,[r0,r2]
        ldrb     r9,[r12,r1,asr #1]
        sub      r10,r0,r2
        sub      r4,r0,r2,lsl #1
        cmp      r9,#0
        addeq    r1,r1,#1
        str 	 r1,[sp,#-4]
        addeq    r0,r0,r3
        beq      al1.260a
        @VLD4.8 {D14[5],D15[5],D16[5],D17[5]}, [R6],R1
       
        ldrb     r4,[r4,#0]
        ldrb     r12,[r10,#0]
        subs     r8,r5,r6
        rsbmi    r8,r8,#0
        subs     r7,r12,r4
        rsbmi    r7,r7,#0
        cmp      r8,r11
        cmplt    r7,r11
        ldrlt    r8,[sp,#0x38]
        ldr      r1,[sp,#0]
        sublt    r7,r5,r12
        cmplt    r7,r8
        bge      al1.260a
        
        
        cmp      r7,r1
        ble      al1.260a
        cmp      r9,#4
        beq      al1.220a
@        ldr      r1,[sp,#0x40]
        sub      r4,r4,r6
        add      r4,r4,r7,lsl #2
        add      r4,r4,#4
        ldrb     r1,[lr,r9]
        movs     r4,r4,asr #3
        beq      al1.260a
        
        
        add      r6,r1,#1
        
        cmp      r4,r6
        movgt    r4,r6
        rsb      r6,r6,#0
        cmp      r4,r6
        movlt    r4,r6
        add      r12,r12,r4
@        ldrb     r12,[lr,r12]
		usat	r12, #8, r12	
        sub      r1,r5,r4
@        ldrb     r1,[lr,r1]
		usat	r1, #8, r1	
        strb     r12,[r10,#0]
        
        strb     r1,[r0,#0]
        b        al1.260a
al1.220a:
        add      r5,r5,r6,lsl #1
        add      r5,r5,r4
        add      r12,r12,r4,lsl #1
        add      r12,r12,r6
        add      r5,r5,#2
        add      r12,r12,#2
        mov      r5,r5,asr #2
        mov      r12,r12,asr #2
        strb     r5,[r0,#0]
        strb     r12,[r10,#0]
al1.260a:
	    ldr	 	 r1,[sp,#-4]
	    add      r0,r0,r3
        add      r1,r1,#1
        cmp      r1,#8
        
        blt      al1.32a
        add      sp,sp,#0x14
        @pop      {r4-r11,pc}
		ldmfd   r13!, {r4-r11, r15} @  
      
