           
           ;.global voAMRNBDecVo_Residu_asm
           ;.section .text
   
           AREA    |.text|, CODE, READONLY
           EXPORT  voAMRNBDecVo_Residu_asm  
           
voAMRNBDecVo_Residu_asm PROC
           STMFD              sp!, {r0-r11,lr}
           VLD1.S16           {D0, D1}, [r0]!                      ; load all prediction coefficient
           SUB                r1, r1, #14                         ; get  x[-7] address
           VMOV.S32           Q13, #0x8000
       
Residu_loop
   
           VLD1.S16           {D4, D5}, [r1]!                      ; load x[-7], x[-6],..., x[0]
           MOV                r8, r1
           VEXT.16            D6, D4, D5, #1                         ; dx1 --- x[-6], x[-5], x[-4], x[-3]

           VQDMULL.S16        Q10, D4, D1[3] 		        ;a[7].x[-7], a[7].x[-6], a[7].x[-5], a[7].x[-4]
           VEXT.16            D7, D4, D5, #2                         ; dx2 --- x[-5], x[-4], x[-3], x[-2]
           VQDMULL.S16        Q11, D5, D1[3]       		        ;a[7].x[-3], a[7].x[-2], a[7].x[-1]. a[7].x[0]
           VEXT.16            D8, D4, D5, #3                         ; dx3 --- x[-4], x[-3], x[-2], x[-1] 

           VLD1.S16           D4, [r8]!                             ; x[1], x[2], x[3], x[4]
           VEXT.16            D9, D5, D4, #1                         ; dx5 --- x[-2], x[-1], x[0], x[1]

           VQDMLAL.S16        Q10, D6, D1[2]                         ;a[6].x[-6], a[6].x[-5], a[6].x[-4], a[6].x[-3]
           VEXT.16            D10, D5, D4, #2                         ; dx6 --- x[-1], x[0], x[1], x[2]
           VQDMLAL.S16        Q11, D9, D1[2]                         ;a[6].x[-2], a[6].x[-1], a[6].x[0], a[6].x[1]
              
           VQDMLAL.S16        Q10, D7, D1[1]                         ;a[5].x[-5], a[5].x[-4], a[5].x[-3], a[5].x[-2]
           VEXT.16            D11, D5, D4, #3                         ; dx7 --- x[0], x[1], x[2], x[3]
           VQDMLAL.S16        Q11, D10, D1[1]                         ;a[5].x[-1], a[5].x[0], a[5].x[1], a[5].x[2]

           VQDMLAL.S16        Q10, D8, D1[0]                         ;a[4].x[-4], a[4].x[-3], a[4].x[-2], a[4].x[-1]
           VQDMLAL.S16        Q11, D11, D1[0]                         ;a[4].x[0], a[4].x[1], a[4].x[2], a[4].x[3]

           VQDMLAL.S16        Q10, D5, D0[3]                         ;a[3].x[-3], a[3].x[-2], a[3].x[-1], a[3].x[0]
           VQDMLAL.S16        Q11, D4, D0[3]                         ;a[3].x[1], a[3].x[2], a[3].x[3], a[3].x[4]

           VLD1.S16           D5, [r8]!                             ; x[5], x[6], x[7], x[8]
           VEXT.16            D6, D4, D5, #1                         ; x[2], x[3], x[4], x[5]
 
           VQDMLAL.S16        Q10, D9, D0[2]                         ; a[2].x[-2], a[2].x[-1], a[2].x[0], a[2].x[1]
           VEXT.16            D7, D4, D5, #2                         ; x[3], x[4], x[5], x[6]
           VQDMLAL.S16        Q11, D6, D0[2]                         ; a[2].x[2], a[2].x[3], a[2].x[4], a[2].x[5]

           VQDMLAL.S16        Q10, D10, D0[1]                         ; a[1].x[-1], a[1].x[0], a[1].x[1], a[1].x[2]
           VEXT.16            D8, D4, D5, #3                         ; x[4], x[5], x[6], x[7]
           VQDMLAL.S16        Q11, D7, D0[1]                         ; a[1].x[3], a[1].x[4], a[1].x[5], a[1].x[6]

           VQDMLAL.S16        Q10, D11, D0[0]                         ; a[0].x[0], a[0].x[1], a[0].x[2], a[0].x[3]
           VQDMLAL.S16        Q11, D8, D0[0]                         ; a[0].x[4], a[0].x[5], a[0].x[6], a[0].x[7]

           VQSHL.S32          Q10, Q10, #3
           VQSHL.S32          Q11, Q11, #3
  
           VQADD.S32          Q10, Q10, Q13
           VQADD.S32          Q11, Q11, Q13

           VSHRN.S32          D24, Q10, #16
           VSHRN.S32          D25, Q11, #16
          
           VST1.S16           {D24, D25}, [r2]!
           SUBS               r3, r3, #8
           BGT                Residu_loop

Vo_Residu_asm_end

           LDMFD          sp!, {r0 - r11,pc}      

           ENDP
           END




