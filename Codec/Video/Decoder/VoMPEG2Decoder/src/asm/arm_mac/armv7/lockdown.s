
; Cache Lockdown Code
; ===================

    AREA CacheLockdownArea, CODE, READONLY

    EXPORT  LockDCacheWay

    CODE32

;/****************************************************************************
; *  LockICacheWay                                                           *
; *  -------------                                                           *
; *  Description:                                                            *
; *    prefetches to and locks an D-cache way                                *
; *                                                                          *
; *  Inputs:                                                                 *
; *    - piBlockAdr - address of block to lock down                          *
; *    - iBlockSize - size in bytes of block to lock down                    *
; *                              (NB: must be less than way size)            *
; *    - iWayToLockDown - way to lock down the block into                    *
; *    - yFlush - !0 to flush entire cache before prefetch,                  *
; *               0 for no flushing before prefetch                          *
; *                                                                          *
; *  Outputs:                                                                *
; *    locks a block into a way                                              *
; *                                                                          *
; *  Return Value:                                                           *
; *    none                                                                  *
; ****************************************************************************/
;// C prototype: extern void LockDCacheWay(int* piBlockAdr, int iBlockSize, int iWayToLockDown, int yFlush);

LockDCacheWay
    STMFD       sp!, {r4-r11}           ; Push onto Full Descending stack  

    MOV r4, #0                          ; Dummy value
	MOV r5, #1
	
	; if r3!=0 then flush entire I-cache before prefetch
	CMP r3, #0                          ; if r3!=0
    MCRNE p15, 0, r4, c7, c5, 0         ; Invalidate entire inst cache

	; get cache line size in bytes
   	MRC p15, 0, r11, c0, c0, 1          ; Get cache type register
    AND r11, r11, #0x3000               ; Mask bits 1:0 to give line size
    ;LSR	r11, r11, #0x8					; Shift to give line size in bytes
    MOV	r11, r11,LSR #0x8					; Shift to give line size in bytes

    ; Set appropriate bits for ways to lock down (using input parameter in r2)
    MOV r6, #0xFFFFFFFF
	BIC r6, r6, r5, LSL r2


	MRC p15, 0, r7, c9, c0, 0           ; Read data cache lockdown reg
	MCR p15, 0, r4, c7, c10, 4          ; Data synch barrier
	MCR p15, 0, r6, c9, c0, 0           ; Write data cache lockdown reg
	MCR p15, 0, r4, c7, c10, 4          ; Data synch barrier


   	ADD r1, r0, r1                      ; Addr of byte after end of block
    SUB r1, r1, #1                      ; Decrement r1 (to give address of last byte in block)
    
  	; work out mask dependent on line size (to allow working out of address of start of line)
  	ADD r6, r5, r11, LSR #3             ; r6 = 1 + r11 >> 3
   	;LSL r6, r5, r6                      ; r6 = 1 << (1 + r11 >> 3)
   	MOV r6, r5,LSL r6                      ; r6 = 1 << (1 + r11 >> 3)
   	SUB r6,r6, #1                          ; bit mask to clear bits to work out line number (set)
   	BIC r0, r0, r6                      ; r0 = start of first line to cache
	BIC r1, r1, r6                      ; r1 = start of last line to cache


    MCRR p15, 2, r1, r0, c12            ; Prefetch data cache range

    ; wait until the block transfer has completed before continuing
BlockTransferLoop
    MRC p15, 0, r2, c7, c12, 4          ; Read Block Transfer Status Register
    TST r2, #1                          ; Check if bottom bit set (set indicates block prefetch operation in progress)
    BNE BlockTransferLoop               ; If bottom bit set, then loop


    ORR r7, r7, r5, LSL r2              ; Set appropriate bit for way to lock down


	MCR p15, 0, r4, c7, c10, 4          ; Data synch barrier
	MCR p15, 0, r7, c9, c0, 0           ; Write inst cache lockdown reg
	MCR p15, 0, r4, c7, c10, 4          ; Data synch barrier


	LDMFD   sp!, {r4-r11}               ; Pop from a Full Descending stack
    BX lr                               ; return

    END
