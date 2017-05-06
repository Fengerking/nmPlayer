
/*!
 *************************************************************************************
 * \file header.h
 * 
 * \brief
 *    Prototypes for header.c
 *************************************************************************************
 */

#ifndef _HEADER_H_
#define _HEADER_H_

avdNativeInt FirstPartOfSliceHeader(ImageParameters *img,Bitstream *currStream);
avdNativeInt RestOfSliceHeader(ImageParameters *img,Bitstream *currStream);
void dec_ref_pic_marking(ImageParameters *img,Bitstream *currStream);

void decode_poc(ImageParameters *img);
int  dumppoc   (ImageParameters *img);

#endif

