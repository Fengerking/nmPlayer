#ifndef _CNST_TCX_STEREO_H
#define _CNST_TCX_STEREO_H

#define  D_STEREO_TCX	5
#define  L_OVLP_2k	32

#define  L_TCX_LB	192
#define  NPRM_RE8_D     (L_TCX_LB+(L_TCX_LB/8))
#define  NPRM_TCX80_D   (2+NPRM_RE8_D)  /* TCX 80ms */
#define  NPRM_TCX40_D   (2+(NPRM_RE8_D/2))      /* TCX 40ms */
#define  NPRM_TCX20_D   (2+(NPRM_RE8_D/4))      /* TCX 20ms */
#define  NPRM_DIV_D     (NPRM_TCX20_D)  /* buffer size = NB_DIV*NPRM_DIV */

#define  TOT_PRM_80		L_TCX_LB
#define  TOT_PRM_40		(L_TCX_LB/2)
#define  TOT_PRM_20		(L_TCX_LB/4)

#define  NPRM_DIV_TCX_STEREO     (NPRM_TCX20_D) /* buffer size = NB_DIV*NPRM_DIV */

#define  TOT_PRM_80		L_TCX_LB
#define  TOT_PRM_40		(L_TCX_LB/2)
#define  TOT_PRM_20		(L_TCX_LB/4)

#define  TCX_STEREO_DELAY_2k (((D_BPF + L_BSP + 2*D_NC)*5/32) + D_STEREO_TCX)   /* 65 + 5 = 70 which is the available look back */

#define TCX_L_FFT_2k	(L_FRAME_2k + L_OVLP_2k)

#define ECU_WIEN_ORD	8
#define L_SUBFR_2k	10
#endif
