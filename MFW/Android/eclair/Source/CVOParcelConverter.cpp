/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CVOParcelConverter.cpp

Contains:	CVOParcelConverter class file

Written by:	 Tom (Yu wei)

Change History (most recent first):
2010-05-15		YW			Create file

*******************************************************************************/

#include "CVOParcelConverter.h"

#define LOG_TAG "VOJNI"

#ifdef _LINUX_ANDROID

#include "String16.h"
#include "String8.h"

CVOParcelConverter CVOParcelConverter::gVOParcelConverter;

CVOParcelConverter::CVOParcelConverter(void)
{


}

CVOParcelConverter::~CVOParcelConverter()
{
	
}

// read the id and param from the parcel
int CVOParcelConverter::ReadParcel (const android::Parcel* in_pParcel, int* out_nID, int* out_nParam1, long* out_lParam2, char* out_szParam3)
{
	size_t len;
	const char16_t* szTemp = NULL;

	in_pParcel->setDataPosition(0); 	

	szTemp         =  in_pParcel->readString16Inplace(&len);
	*out_nID       =  in_pParcel->readInt32();
	*out_nParam1   =  in_pParcel->readInt32();
	*out_lParam2   =  (long) in_pParcel->readInt64();
	szTemp         =  in_pParcel->readString16Inplace(&len);

	android::String8 strReturn(szTemp);
	memcpy(out_szParam3, strReturn.string(), len);
	out_szParam3[len] = 0x00;

     /*
	LOGE("ID FROM INVOKE %d", *out_nID); 
	LOGE("param1: %d", *out_nParam1); 
	LOGE("param2: %ld", *out_lParam2); 	
	LOGE("param3 %s len %d", out_szParam3, len);
	//*/

    return 0;
}

// write the id and param to the parcel
int CVOParcelConverter::WriteParcel (android::Parcel* in_pParcel, int in_nID, int in_nParam1, long in_lParam2, char* in_szParam3)
{
    android::String16 str(in_szParam3);

	in_pParcel->writeInt32(in_nID);
	in_pParcel->writeInt32(in_nParam1);
	in_pParcel->writeInt64(in_lParam2);		
	in_pParcel->writeString16(str);

	return 0;
}




#endif // _LINUX_ANDROID