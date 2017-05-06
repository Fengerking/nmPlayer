#ifndef consts_fx_h
#define consts_fx_h

#include "cnst_fx.h"

/* RE8 lattice quantiser constants */
#define NB_SPHERE 32
#define NB_LEADER 36
#define NB_LDSIGN 224
#define NB_LDQ3   9
#define NB_LDQ4   27

/* LF ISF quantiser constants */
#define ORDER   16
#define ISF_GAP 128
#define N_SURV  4
#define SIZE_BK1  256
#define SIZE_BK2  256
#define SIZE_BK21 64
#define SIZE_BK22 128
#define SIZE_BK23 128
#define SIZE_BK24 32
#define SIZE_BK25 32
#define SIZE_BK21_36b 128
#define SIZE_BK22_36b 128
#define SIZE_BK23_36b 64

/* HF ISF quantiser constants */
#define Q_ISF_ORDER 8
#define SIZE_BK1_HF 4
#define SIZE_BK2_HF 128

/* HF gain quantiser constants */
#define Q_GN_ORDER        4 
#define SIZE_BK_HF        128
#define MEAN_GAIN_HF_FX   719  /*Q8*/

/* Resampling filter sizes and factors */
#define FAC1_48k  4
#define FAC1_44k  128
#define FAC1_32k  2
#define FAC1_28k8 4
#define FAC2_48k  15
#define FAC2_44k  441
#define FAC2_32k  5
#define FAC2_28k8 9
#define NCOEF_48k  45
#define NCOEF_44k  42
#define NCOEF_32k  30
#define NCOEF_28k8 27
#define NCOEF_24k  23
#define NCOEF_22k  21
#define NCOEF_16k  15
#define NCOEF_12k8 12

/* Max size of FFT */
#define  N_MAX      1152

/* For pitch predictor */
#define PIT_UP_SAMP      4
#define PIT_L_INTERPOL2  16
#define PIT_FIR_SIZE2    (PIT_UP_SAMP*PIT_L_INTERPOL2+1)

#endif
