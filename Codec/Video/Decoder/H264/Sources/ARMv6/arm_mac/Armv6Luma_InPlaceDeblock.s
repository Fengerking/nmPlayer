		.text
	  .align 2
    .globl  _Luma_InPlaceDeblock
_Luma_InPlaceDeblock:

@luma_inplacedeblock proc
        @push     {r0-r11,lr}
		stmfd   r13!, {r0-r11, r14} @  14_1
        sub      sp,sp,#0x28
        mov      r8,r2,lsl #1
        @for(j = 0@ j < 16@ j++, pt2 += srcinc)
        add      r3,r8,r2
        str      r3,[sp,#0]
        ldr      r3,[sp,#0x34]
        ldr      r7,[sp,#0x5c]
        add      r1,r3,r3,lsl #1
        str      r1,[sp,#0x24]
        mov      r1,#2
        add      r3,r1,r7,asr #2
        mov      r6,#0
        str      r3,[sp,#0x20]
        @str      r8,[sp,#4]
        @if(!(str = pt1[j>>2])){
al1.56a:
        ldr      r1,[sp,#0x2c]@r1 = pt1
        @ldr      r8,[sp,#4]@l1  = pt2[-inc2] @
        mov      r8,r2,lsl #1
        sub      r11,r0,r2
        
        ldrb     r3,[r1,r6,asr #2]@r6=j,r3=str
        sub      r4,r0,r8
        str      r4,[sp,#0x1c]
        
        cmp      r3,#0
        ldreq    r1,[sp,#0x24]
        str      r3,[sp,#0xc]
        addeq    r6,r6,#3
        addeq    r0,r1,r0
        streq    r6,[sp,#0x4]
        beq      al1.760a
        
        
        ldrb     r1,[r4,#0]
        str      r11,[sp,#0x18]
        ldrb     r5,[r0,#0]
        ldrb     r12,[r0,r2]
        ldrb     r4,[r11,#0]
        ldr      r11,[sp,#0x60]
        subs     r3,r5,r12
        rsbmi    r3,r3,#0
        subs     r7,r4,r1
        rsbmi    r7,r7,#0
        cmp      r3,r11
        ldrlt    r11,[sp,#0x60]@
        str		 r6,[sp,#0x4]
        sub      r9,r5,r4
        
        cmplt    r7,r11
        bge      al1.760a
        
        cmp      r9,#0
        ldr      r7,[sp,#0x5c]
        rsble    r3,r9,#0
        movgt    r3,r9
        cmp      r3,r7
        str      r3,[sp,#0x10]
        bge      al1.760a
        
        ldr      r3,[sp,#0]
        ldr      r11,[sp,#0x60]@
        ldrb     r8,[r0,r2,lsl #1]
        
        sub      r3,r0,r3
        str      r3,[sp,#0x14]
        ldrb     r7,[r3,#0]
        
        subs     r10,r5,r8
        rsbmi    r10,r10,#0
        subs     r3,r4,r7
        rsbmi    r3,r3,#0
        cmp      r10,r11
        ldr      r11,[sp,#0x60]
        movge    r10,#0
        movlt    r10,#1
        cmp      r3,r11
        movge    r3,#0
        movlt    r3,#1
        str      r3,[sp,#8]
        ldr      r3,[sp,#0xc]@
        @if(str != 4)
        mov      lr,r10
        add      r10,r4,r5
        cmp      r3,#4
        
        beq      al1.512a
        ldr      r3,[sp,#0x64]
        ldr      r11,[sp,#0xc]@todo
        
        ldrb     r3,[r3,r11]
        sub      r11,r1,r12
        add      r9,r11,r9,lsl #2
        add      r9,r9,#4
        ldr      r6,[sp,#8]@
        movs     r9,r9,asr #3
        beq      al1.384a
        
        
        
        add      r6,r3,r6
        add      r11,r6,lr
        cmp      r9,r11
        movgt    r9,r11
        rsb      r11,r11,#0
        cmp      r9,r11
        movlt    r9,r11
        
@        ldr      r11,[sp,#0x68]
        ldr      r6,[sp,#0x18]
        add      r4,r4,r9
@        ldrb     r4,[r11,r4]@todo
		usat	r11, #8, r4
        
        
@        ldr      r11,[sp,#0x68]@
        sub      r4,r5,r9   
@        ldrb     r4,[r11,r4]
		usat	r4, #8, r4
        strb     r11,[r6,#0]
        strb     r4,[r0,#0]
al1.384a:
        ldr      r11,[sp,#8]
        add      r4,r10,#1
        cmp      r11,#0
        add      r4,r7,r4,asr #1
@        cmp      r11,#0
        beq      al1.452a
        
        rsbs     r1,r1,r4,asr #1
        beq      al1.452a
        ldr      r6,[sp,#0x1c]
        cmp      r1,r3
        movgt    r1,r3
        rsb      r4,r3,#0
        ldrb     r6,[r6,#0]
        cmp      r1,r4
        movlt    r1,r4
        ldr      r4,[sp,#0x1c]@todo
        
        add      r1,r6,r1
        
        strb     r1,[r4,#0]
al1.452a:
        cmp      lr,#0
        beq      al1.760a
        add      r1,r10,#1

        ldrb     r6,[r0,r2]
        add      r1,r8,r1,asr #1
@
        rsbs     r1,r12,r1,asr #1

        beq      al1.760a
@        ldrb     r6,[r0,r2]
        cmp      r1,r3
        movgt    r1,r3
        rsb      r3,r3,#0
        cmp      r1,r3
        movlt    r1,r3
        
        add      r1,r6,r1
        strb     r1,[r0,r2]
        b        al1.760a
al1.512a:
        ldr      r3,[sp,#0x20]
        ldr      r9,[sp,#0x10]@todo
        add      r6,r10,r12
        
        
        cmp      r3,r9
        movle    r3,#0
        movgt    r3,#1
        tst      lr,r3
        beq      al1.616a
        
        
        add      r9,r1,r6,lsl #1
        add      r9,r9,r8
        add      r9,r9,#4
        add      r5,r6,r8
        mov      r9,r9,asr #3

        strb     r9,[r0,#0]
        add      r9,r5,#2
        ldr      r6,[sp,#0]
        mov      r9,r9,asr #2
        ldrb     r11,[r0,r6]
@        strb     r9,[r0,r2]
@        ldr      r9,[sp,#0]
@        ldrb     r9,[r0,r6]
        add      r6,r11,r8
@        add      r5,r5,r8,lsl #1
        @ldr      r8,[sp,#4]
        strb     r9,[r0,r2]
        add      r5,r5,r6,lsl #1
        add      r5,r5,#4
        mov      r8,r2,lsl #1
        mov      r5,r5,asr #3
        strb     r5,[r0,r8]
        b        al1.636a
al1.616a:
        add      r5,r5,r12,lsl #1
        add      r5,r5,r1
        add      r5,r5,#2
@
        mov     r5,r5,asr #2
        strb     r5,[r0,#0]
al1.636a:
        ldr      r11,[sp,#8]@todo
        add      r6,r10,r1
        
        
        tst      r11,r3
        beq      al1.736a
       
        add      r3,r12,r6,lsl #1
        add      r3,r3,r7
@        ldr      r11,[sp,#0x18]
        add      r3,r3,#4
        ldr      r11,[sp,#0x18]
        mov      r3,r3,asr #3
        strb     r3,[r11,#0]
        add      r1,r6,r7
@        ldr      r4,[sp,#0x1c]
        add      r3,r1,#2
        ldr      r4,[sp,#0x1c]
        mov      r3,r3,asr #2
 @       strb     r3,[r4,#0]
        @ldr      r8,[sp,#4]
        mov      r8,r2,lsl #1
       strb     r3,[r4,#0]
        sub      r3,r0,r8,lsl #1
        ldrb     r6,[r3,#0]
        ldr      r3,[sp,#0x14]
        add      r6,r6,r7
        add      r1,r1,#4
        add      r1,r1,r6,lsl #1
@
        mov      r1,r1,asr #3
        strb     r1,[r3,#0]
        b        al1.760a
al1.736a:
		ldr      r11,[sp,#0x18]
        add      r1,r4,r1,lsl #1
        add      r1,r1,r12
        
        add      r1,r1,#2
@
        mov      r1,r1,asr #2
        strb     r1,[r11,#0]
al1.760a:
		ldr		 r6,[sp,#0x4]
        ldr      r3,[sp,#0x34]
        add      r6,r6,#1
        cmp      r6,#0x10
        add      r0,r0,r3
        blt      al1.56a
        add      sp,sp,#0x38
        @pop      {r4-r11,pc}
		ldmfd   r13!, {r4-r11, r15} @  
     
