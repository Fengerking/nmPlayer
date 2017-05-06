.set MC_ASM_ENABLED,	   1
.set DEQUANT_ASM_ENABLED, 1
.set IDCT_ASM_ENABLED,	1
.set DEBLOCK_ASM_ENABLED,	1
.set SAO_ASM_ENABLED,	1

.macro ldrConst @r0, v0, label
              b 1f
$2 : .long $1
1:
    ldr $0, $2
.endm