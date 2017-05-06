/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CVOParcelConverter.h

Contains:	CVOParcelConverter header file

Written by:	 Tom (Yu Wei)

Change History (most recent first):
2010-05-15		YW			Create file

*******************************************************************************/

#ifndef __CVOParcelConverter_H__
#define __CVOParcelConverter_H__

#ifdef _LINUX_ANDROID
#include <sys/types.h>
#include <utils/Errors.h>
#include "binder/Parcel.h"



class CVOParcelConverter 
{
public:
	CVOParcelConverter (void);
	virtual ~CVOParcelConverter (void);


public:
	static CVOParcelConverter gVOParcelConverter;

public:
	static int ReadParcel  (const android::Parcel* in_pParcel, int* out_nID, int* out_nParam1, long* out_lParam2, char* out_szParam3);
    static int WriteParcel (android::Parcel* in_pParcel, int in_nID, int in_nParam1, long in_lParam2, char* in_szParam3);


};

#endif // _LINUX_ANDROID
#endif // __CVOParcelConverter_H__
