/* ***** BEGIN LICENSE BLOCK *****
* Source last modified: $Id: rv_decode.c,v 1.1.1.1.2.2 2005/05/04 18:21:38 hubbe Exp $
* 
* REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
* Portions Copyright (c) 1995-2005 RealNetworks, Inc.
* All Rights Reserved.
* 
* The contents of this file, and the files included with this file,
* are subject to the current version of the Real Format Source Code
* Porting and Optimization License, available at
* https://helixcommunity.org/2005/license/realformatsource (unless
* RealNetworks otherwise expressly agrees in writing that you are
* subject to a different license).  You may also obtain the license
* terms directly from RealNetworks.  You may not use this file except
* in compliance with the Real Format Source Code Porting and
* Optimization License. There are no redistribution rights for the
* source code of this file. Please see the Real Format Source Code
* Porting and Optimization License for the rights, obligations and
* limitations governing use of the contents of the file.
* 
* RealNetworks is the developer of the Original Code and owns the
* copyrights in the portions it created.
* 
* This file, and the files included with this file, is distributed and
* made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL
* SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT
* OR NON-INFRINGEMENT.
* 
* Technology Compatibility Kit Test Suite(s) Location:
* https://rarvcode-tck.helixcommunity.org
* 
* Contributor(s):
* 
* ***** END LICENSE BLOCK ***** */

/* Simple, unified decoder frontend for RealVideo */
#include <string.h>
#include <stdlib.h>

#include "decoder.h"
#include "rv_decode.h"
#include "rv30backend.h"
#include "codec_defines.h"
#include "voRealVideo.h"
#include "rv30dec.h"
#include "basic.h"


/* rv_decode_destroy()
* Deletes decoder and backend instance, followed by frontend. */

VO_U32 rv_decode_destroy(rv_backend* pDecode)
{
	struct Decoder *t;
	VO_MEM_OPERATOR MemOperator;

	MemOperator.Free = NULL;

	t = (struct Decoder *)pDecode->pDecodeState;
	MemOperator.Free = t->m_pMemOP->Free;

	//TBD
	decoder_remove_threads(t);
	if (pDecode->fpFree){
		pDecode->fpFree(pDecode->pDecodeState);
	}	
	/* Free the decoder instance */
	/* Free the backend interface */
	FreeMem(pDecode, &MemOperator);	

	return VO_ERR_NONE;
}


/* rv_decode_init()
* Reads bitstream header, selects and initializes decoder backend.
* Returns zero on success, negative result indicates failure. */
/*YU: parser SPO, RPR data, StreamVersion in opaque data !!*/
VO_U32 rv_decode_init(rv_backend* pDecode, rv_format_info* pHeader)
{
	VO_U32 result = VO_ERR_NONE;
	UINT8* baseptr;
	UINT8 ucTmp;
	U32 i;

	VOUINT32 ulSPOExtra = 0;
	VOUINT32 ulEncodeSize = 0;
	VOUINT32 ulNumResampledImageSizes = 0;
	VOUINT32 ulMajorBitstreamVersion = 0;
	VOUINT32 ulMinorBitstreamVersion = 0;
	VOUINT32 ulWidth = 0, ulHeight = 0;//default
	VOUINT32 ulStreamVersion = pHeader->ulLength;//TBD
	VOUINT32 pDimensions[2*(8+1)];
	RV10_INIT pInitParams;
	struct Decoder *t;

	int w = 0xFF; /* For run-time endianness detection */
	t = (struct Decoder*)pDecode->pDecodeState;

	ulSPOExtra = 0;
	ulEncodeSize = 0;
	ulNumResampledImageSizes = 0;

	switch (pHeader->ulSubMOFTag)
	{
	case HX_RV30VIDEO_ID:
	case HX_RV40VIDEO_ID:
	case HX_RV89COMBO_ID:
		/* Select the decoder backend */
		pDecode->fpInit          = _RV40toYUV420Init;
		pDecode->fpDecode        = _RV40toYUV420Transform;
		//pDecode->fpCustomMessage = _RV40toYUV420CustomMessage;
		pDecode->fpFree          = _RV40toYUV420Free;
		break;
	default:
		pDecode->fpInit          = NULL;
		pDecode->fpDecode        = NULL;
		pDecode->fpCustomMessage = NULL;
		pDecode->fpFree          = NULL;
		/* unknown format */
		return HXR_NOT_SUPPORTED;
	}

	if (pHeader->ulOpaqueDataSize > 0)
	{
		/* Decode opaque data */
		baseptr = pHeader->pOpaqueData;

		/* ulSPOExtra contains CODEC options */
		MoveMem((UINT8*)&ulSPOExtra, baseptr, sizeof(VO_U32),t->m_pMemOP);	
		ulSPOExtra = IS_BIG_ENDIAN(w) ? ulSPOExtra : BYTE_SWAP_UINT32(ulSPOExtra);
			
		baseptr += sizeof(VOUINT32);

		/* ulStreamVersion */
		MoveMem((UINT8*)&ulStreamVersion, baseptr, sizeof(VO_U32),t->m_pMemOP);
		
		ulStreamVersion = IS_BIG_ENDIAN(w) ? ulStreamVersion : BYTE_SWAP_UINT32(ulStreamVersion);
		baseptr += sizeof(VOUINT32);

		ulMajorBitstreamVersion = HX_GET_MAJOR_VERSION(ulStreamVersion);
		ulMinorBitstreamVersion = HX_GET_MINOR_VERSION(ulStreamVersion);

		/* Decode extra opaque data */
		if (!(ulMinorBitstreamVersion & RAW_BITSTREAM_MINOR_VERSION))
		{
			if (ulMajorBitstreamVersion == RV20_MAJOR_BITSTREAM_VERSION ||
				ulMajorBitstreamVersion == RV30_MAJOR_BITSTREAM_VERSION)
			{
				/* RPR (Reference Picture Resampling) sizes */
				ulNumResampledImageSizes = (ulSPOExtra & RV40_SPO_BITS_NUMRESAMPLE_IMAGES)
					>> RV40_SPO_BITS_NUMRESAMPLE_IMAGES_SHIFT;
				/* loop over dimensions of possible resampled images sizes              */
				/* This byzantine method of extracting bytes is required to solve       */
				/* misaligned write problems in UNIX                                    */
				/* note 2 byte offset in pDimensions buffer for resampled sizes         */
				/* these 2 bytes are later filled with the native pels and lines sizes. */
				for(i = 0; i < ulNumResampledImageSizes; i++){
					//TBD
					ucTmp = *baseptr;
					baseptr+=sizeof(VO_U8);
					pDimensions[2*i+2] = (VO_U32)(ucTmp<<2); /* width */

					ucTmp = *baseptr;
					baseptr += sizeof(VO_U8);
					pDimensions[2*i+3] = (VO_U32)(ucTmp<<2); /* height */
				}
			}else if (ulMajorBitstreamVersion == RV40_MAJOR_BITSTREAM_VERSION){
				/* RV9 largest encoded dimensions */
				if (pHeader->ulOpaqueDataSize >= 12){
					MoveMem((UINT8*)&ulEncodeSize, baseptr, sizeof(VOUINT32),t->m_pMemOP);
					ulEncodeSize = IS_BIG_ENDIAN(w) ? ulEncodeSize : BYTE_SWAP_UINT32(ulEncodeSize);
				}
			}
		}

	}

	/* Set largest encoded dimensions */
	ulWidth = ((ulEncodeSize >> 14) & 0x3FFFC);
	if(!ulWidth)
		ulWidth = pHeader->usWidth;

	ulHeight = ((ulEncodeSize << 2) & 0x3FFFC);
	if(!ulHeight)
		ulHeight = pHeader->usHeight;

	if(!(ulWidth &&ulHeight))
		return VO_ERR_WRONG_STATUS;

	/* Prepare decoder init parameters */
	pInitParams.pels = (UINT16)ulWidth;
	pInitParams.lines = (UINT16)ulHeight;
	pInitParams.ulStreamVersion = ulStreamVersion;

	if (HX_RV89COMBO_ID == pHeader->ulSubMOFTag){
		U32  minor = HX_GET_MINOR_VERSION(pInitParams.ulStreamVersion);//huwei 20110429 add for raw date

		if (RAW_BITSTREAM_MINOR_VERSION == minor){
			t->m_decoding_thread_count = 1;
		}		
	}

	if(t->m_decoding_thread_count > 1){
		result = decoder_create_threads(t);
                voVLog("decoder_create_threads return %x\n", result);
		if (result != VO_ERR_NONE){
			return result;
		}
	}

	/* Call the decoder backend init function */
	result = pDecode->fpInit((void*)&pInitParams, (void*)&pDecode->pDecodeState);

	if (result != VO_ERR_NONE)
		return result;	

	/* Send version-specific init messages, if required */
	if ((ulMajorBitstreamVersion == RV20_MAJOR_BITSTREAM_VERSION ||
		ulMajorBitstreamVersion == RV30_MAJOR_BITSTREAM_VERSION) &&
		(ulNumResampledImageSizes != 0)){

		//TBD PRP
		struct Decoder *t = (struct Decoder *)(pDecode->pDecodeState);
		U32 num_sizes = ulNumResampledImageSizes + 1;/* includes native size */
		pDimensions[0] = pInitParams.pels;    /* native width */
		pDimensions[1] = pInitParams.lines;   /* native height */
		t->m_options.Num_RPR_Sizes = num_sizes;

		for (i = 0; i < 2 * num_sizes; i++){
			t->m_options.RPR_Sizes[i] = pDimensions[i];
		}
	}

	voVLog("Initilizaiton done!\n");
	return result;
}

