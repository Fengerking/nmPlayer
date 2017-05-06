
/*!
 **************************************************************************************
 * \file
 *    parset.h
 * \brief
 *    Picture and Sequence Parameter Sets, decoder operations
 *    This code reflects JVT version xxx
 * \date 25 November 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details) 
 *      - Stephan Wenger        <stewe@cs.tu-berlin.de>
 ***************************************************************************************
 */
#ifndef _PARSET_H_
#define _PARSET_H_


#include "parsetcommon.h"
#include "nalucommon.h"

void PPSConsistencyCheck (ImageParameters *img,pic_parameter_set_rbsp_t *pps);
void SPSConsistencyCheck (ImageParameters *img,seq_parameter_set_rbsp_t *sps);

void ProcessSPS (ImageParameters *img,NALU_t *nalu);
void ProcessPPS (ImageParameters *img,NALU_t *nalu);

avdNativeInt UseParameterSet (ImageParameters *img,avdNativeInt PicParsetId);
void UpdateSizeNActivateSPS (ImageParameters *img,seq_parameter_set_rbsp_t *sps);

#endif
