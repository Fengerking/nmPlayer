
/*!
 ************************************************************************
 * \file  memalloc.h
 *
 * \brief
 *    Memory allocation and free helper funtions
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 ************************************************************************
 */

#ifndef _MEMALLOC_H_
#define _MEMALLOC_H_

#include "global.h"

avdInt32  get_mem2D(ImageParameters *img,avdUInt8 ***array2D, avdNativeInt rows, avdNativeInt columns);
avdInt32  get_mem3D(ImageParameters *img,avdUInt8 ****array2D, avdNativeInt frames, avdNativeInt rows, avdNativeInt columns);
avdInt32  get_mem4D(ImageParameters *img,avdUInt8 *****array4D, avdNativeInt idx, avdNativeInt frames, avdNativeInt rows, avdNativeInt columns );

void free_mem2D(ImageParameters *img,avdUInt8 **array2D);
void free_mem3D(ImageParameters *img,avdUInt8 ***array2D, avdNativeInt frames);
void free_mem4D(ImageParameters *img,avdUInt8 ****array4D, avdNativeInt idx, avdNativeInt frames );

void no_mem_exit(char *where);
#endif
