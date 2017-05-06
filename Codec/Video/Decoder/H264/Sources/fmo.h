
/*!
 ***************************************************************************
 *
 * \file fmo.h
 *
 * \brief
 *    Support for Flexilble Macroblock Ordering (FMO)
 *
 * \date
 *    19 June, 2002
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 **************************************************************************/

#ifndef _FMO_H_
#define _FMO_H_


avdNativeInt FmoInit (ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps);
avdNativeInt FmoFinit (ImageParameters *img);

avdNativeInt FmoGetNumberOfSliceGroup(ImageParameters *img);
avdNativeInt FmoGetLastMBOfPicture(ImageParameters *img);
avdNativeInt FmoGetLastMBInSliceGroup(ImageParameters *img,avdNativeInt SliceGroup);
avdNativeInt FmoGetSliceGroupId (ImageParameters *img,avdNativeInt mb);
avdNativeInt FmoGetNextMBNr (ImageParameters *img,avdNativeInt CurrentMbNr);

#endif
