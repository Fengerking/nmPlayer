
/*!
 ************************************************************************
 * \file image.h
 *
 ************************************************************************
 */

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "mbuffer.h"

// this one is empty. keep it, maybe we will move some image.c function 
// declarations here


#ifdef SHOW_SNR
void find_snr(struct snr_par *snr, StorablePicture *p, FILE *p_ref);
#endif //SHOW_SNR

avdInt32  picture_order(ImageParameters *img);
void get_block(ImageParameters *img,TMBsProcessor *info,int ref_frame, StorablePicture **list, int x_pos, int y_pos, int block[BLOCK_SIZE][BLOCK_SIZE]);
void GetBlockChroma4x4(ImageParameters *img,TMBsProcessor *info,avdNativeInt xOffset, avdNativeInt yOffset, avdUInt8* refStart, avdUInt8 *byteOutStart, avdNativeInt outLen, avdNativeInt *clpHt);
void GetBlockChroma2x2(ImageParameters *img,TMBsProcessor *info,avdNativeInt xOffset, avdNativeInt yOffset, avdUInt8* refStart, avdUInt8 *byteOutStart, avdNativeInt outLen, avdNativeInt *clpHt);

#endif

