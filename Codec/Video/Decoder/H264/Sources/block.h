
/*!
 ************************************************************************
 * \file block.h
 *
 * \author
 *  Inge Lille-Langøy               <inge.lille-langoy@telenor.com>    \n
 *  Telenor Satellite Services                                         \n
 *  P.O.Box 6914 St.Olavs plass                                        \n
 *  N-0130 Oslo, Norway
 *
 ************************************************************************
 */

#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "global.h"

#define DQ_BITS         6
#define DQ_ROUND        (1<<(DQ_BITS-1))

extern const avdUInt8 QP_SCALE_CR[52] ;
extern const avdUInt8 DIVIDE6[52];
#define DEQUANT_IDX(qp_rem, i, j)  (((qp_rem)<<4) | ((i)<<2) | j)
#define DEQUANT_IDX8(qp_rem, i, j)  (((qp_rem)<<6) | ((i)<<3) | j)

extern const avdUInt8  dequant_coef[96];
extern const avdUInt8 dequant_coef8[384];

#endif

