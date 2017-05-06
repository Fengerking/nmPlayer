#ifndef _LIBDRA_HUFFUM_
#define _LIBDRA_HUFFUM_

#include "config.h"
#include "global.h"
#include "stream.h"
#include "bit.h"

#define  NUM_QHUFF			10

typedef struct _QIndexHuf{
	int			nDim;
	int			nNumhufCodes;
	int			nNumDimhuf;
	const int	*nHufCode_l;
	const int	*nHufCode_s;
} QIndexHuf;

#define		NCLUSTER_HUF		7

extern const int HuffDec1_7x1[7*3];

#define		HSBAND_HUF_L		64
#define     HSBAND_HUF_S		32

extern const int HuffDec2_64x1[64*3];
extern const int HuffDec3_32x1[32*3];

#define		MNHS_HUF_L			18
#define		MNHS_HUF_S			18

extern const int HuffDec4_18x1[18*3];
extern const int HuffDec5_18x1[18*3];

#define		QSTEPINDEX_HUF_L	116
#define		QSTEPINDEX_HUF_S	116

extern const int HuffDec6_116x1[116*3];
extern const int HuffDec7_116x1[116*3];

#define		QUOTIENT_HUF_L		16
#define		QUOTIENT_HUF_S		16

extern const int HuffDec8_16x1[16*3];
extern const int HuffDec9_16x1[16*3];


extern const int HuffDec10_81x4[81*3];
extern const int HuffDec11_25x2[25*3];
extern const int HuffDec12_81x2[81*3];
extern const int HuffDec13_289x2[289*3];
extern const int HuffDec14_31x1[31*3];
extern const int HuffDec15_63x1[63*3];
extern const int HuffDec16_127x1[127*3];
extern const int HuffDec17_255x1[255*3];
extern const int HuffDec18_256x1[256*3];


extern const int HuffDec19_81x4[81*3];
extern const int HuffDec20_25x2[25*3];
extern const int HuffDec21_81x2[81*3];
extern const int HuffDec22_289x2[289*3];
extern const int HuffDec23_31x1[31*3];
extern const int HuffDec24_63x1[63*3];
extern const int HuffDec25_127x1[127*3];
extern const int HuffDec26_255x1[255*3];
extern const int HuffDec27_256x1[256*3];

extern const QIndexHuf nQindexHufTab[10];


int HuffDec(Bitstream *bitptr, const int *pCodeBook, int nNumCodes);
int HuffDiff(Bitstream *bitptr, const int *pCodeBook, int nIndex, int nNumcodes);
int HuffDecRecurive(Bitstream *bitptr, const int *pCodeBook, int nNumCodes);
int ResetHuffIndex();

#endif