/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _ENCBITSTREAM_
#define _ENCBITSTREAM_

#include "../voMpegEnc.h"
#include "h263/voH263EncHuffman.h"
#include "../lowlevel/voMpegEncWriteBits.h"
#include "../voMpegProtect.h"


#define VIDOBJ_START_CODE		0x00000100 /* ..0x0000011f  */
#define VIDOBJLAY_START_CODE	0x00000120 /* ..0x0000012f */
#define VISOBJSEQ_START_CODE	0x000001b0
#define VISOBJSEQ_STOP_CODE		0x000001b1	
#define USERDATA_START_CODE		0x000001b2
#define GRPOFVOP_START_CODE		0x000001b3

#define VISOBJ_START_CODE		0x000001b5
#define VOP_START_CODE			0x000001b6

#define VISOBJ_TYPE_VIDEO       1
#define VIDOBJLAY_TYPE_SIMPLE       1
#define VIDOBJLAY_TYPE_ART_SIMPLE    10
#define VIDOBJLAY_TYPE_ASP    17

//VO_VOID InitMPEG4VLCTab(VO_VOID);

VO_S32 CalcIntraCoeffBits(const VO_S16 qcoeff[64], const VO_S32 direction);

VO_VOID Mpeg4WriteVolHeader(ENCHND * const enc_hnd, BitStream * const bs);

VO_VOID Mpeg4WriteVopHeader (ENCLOCAL * const enc_local, 
                                             VO_U32 framerate_base,VO_U32	pic_format,
						                     VO_U32 img_width, 
                                             VO_U32 img_height,
                                             BitStream * const bs,
                                             Statistics *sStat);

extern VO_VOID H263WritePicHeader (ENCLOCAL * const enc_local, 
                                                VO_U32 framerate_base,
                                                VO_U32	pic_format,
						                        VO_U32 img_width, 
                                                VO_U32 img_height,
                                                BitStream * const bs,
                                                Statistics *sStat);

VO_VOID EncodeH263InterMB (ENCLOCAL * const enc_local, 
                                        MACROBLOCK * pMB, 
				                        VO_S16 *qcoeff, 
                                        BitStream * bs, 
                                        Statistics * pStat, 
                                        Mpeg4Frame *img_cur, 
                                        VO_S32 stride);

VO_VOID EncodeH263IntraMB (ENCLOCAL * const enc_local, 
                                        MACROBLOCK * pMB,
                                        VO_S16 *qcoeff, 
                                        BitStream * bs, 
                                        Statistics * pStat);

VO_VOID EncodeMpeg4InterMB (ENCLOCAL * const enc_local, 
                                        MACROBLOCK * pMB, 
                                        VO_S16 *qcoeff, 
                                        BitStream * bs, 
                                        Statistics * pStat, 
                                        Mpeg4Frame *img_cur, 
                                        VO_S32 stride);

VO_VOID EncodeMpeg4IntraMB (ENCLOCAL * const enc_local, 
                                        MACROBLOCK * pMB,
                                        VO_S16 *qcoeff, 
                                        BitStream * bs, 
                                        Statistics * pStat);

VO_S32 voMpeg4WriteVIH(VO_VOID* p_hnd, VO_U8 *output);

typedef VO_VOID (ENCMBINTRA) (ENCLOCAL * const enc_local, 
                                    MACROBLOCK * pMB, 
                                    VO_S16 *qcoeff, 
                                    BitStream * bs, 
                                    Statistics * pStat);

typedef VO_VOID (ENCMBINTER) (ENCLOCAL * const enc_local, 
                                    MACROBLOCK * pMB, 
                                    VO_S16 *qcoeff, 
                                    BitStream * bs, 
                                    Statistics * pStat, 
                                    Mpeg4Frame *img_cur,
                                    VO_S32 stride);


#ifdef MPEG4_RESYNC
VO_VOID VOMpeg4Resync(ENCHND * const enc_hnd, 
                                    BitStream * bs, 
                                    const VO_S32 mb_x, 
                                    const VO_S32 mb_y, 
                                    const VO_S32 mb_width, 
                                    MACROBLOCK *pMB, 
                                    const VO_S32 bits);
#endif//MPEG4_RESYNC

extern	ENCMBINTRA EncodeH263IntraMB, EncodeMpeg4IntraMB;
extern	ENCMBINTER EncodeH263InterMB, EncodeMpeg4InterMB;

#endif // _ENCBITSTREAM_