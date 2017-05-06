	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCfgComp.cpp

	Contains:	voCOMXCfgComp class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#define LOG_TAG "voCOMXCfgComp"


#include <stdlib.h>
#include "voOMX_Index.h"
#include "OMX_Component.h"
#include "voCOMXCfgComp.h"
#include "voLog.h"

#define VOCOMP_AUDIO_DEC    (OMX_STRING)"OMX.VisualOn.Audio.Decoder.XXX"
#define VOCOMP_VIDEO_DEC    (OMX_STRING)"OMX.VisualOn.Video.Decoder.XXX"
#define VOCOMP_AUDIO_ENC    (OMX_STRING)"OMX.VisualOn.Audio.Encoder.XXX"
#define VOCOMP_VIDEO_ENC    (OMX_STRING)"OMX.VisualOn.Video.Encoder.XXX"

voCOMXCfgComp::voCOMXCfgComp(void)
	: voCOMXBaseConfig ()
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCfgComp::~voCOMXCfgComp(void)
{
}

OMX_BOOL voCOMXCfgComp::Open (OMX_STRING pFile)
{
	voCOMXBaseConfig::Open (pFile);

	return OMX_TRUE;
}

OMX_STRING voCOMXCfgComp::GetCodecCompNameByCoding (OMX_U32 nDomain, OMX_U32 nCodec, OMX_U32 nCoding, OMX_U32 nIndex, OMX_U32 nParam1 /* = 0 */, OMX_U32 nParam2 /* = 0 */, OMX_U32 nParam3 /* = 0 */)
{
	OMX_STRING pCompName = NULL;

	char szSectName[128];
	if (GetSectName (szSectName, nDomain, nCodec, nCoding, nParam1, nParam2, nParam3))
		pCompName = GetCompName (szSectName, 0 , nIndex);

	if (pCompName == NULL)
	{
		if (nDomain == OMX_PortDomainAudio)
		{
			if (nCodec == 0)
				return VOCOMP_AUDIO_DEC;
			else
				return VOCOMP_AUDIO_ENC;
		}
		else 
		{
			if (nCodec == 0)
				return VOCOMP_VIDEO_DEC;
			else
				return VOCOMP_VIDEO_ENC;
		}
	}

	return pCompName;
}

OMX_STRING voCOMXCfgComp::GetCodecCompNameByMIME (OMX_U32 nDomain, OMX_U32 nCodec, OMX_STRING pMIME, OMX_U32 nFourCC, OMX_U32 nIndex, OMX_U32 nParam1 /* = 0 */, OMX_U32 nParam2 /* = 0 */, OMX_U32 nParam3 /* = 0 */)
{
	OMX_STRING pCompName = NULL;
	char szSectName[128];
	if (nDomain == OMX_PortDomainAudio)
	{
		strcpy (szSectName, "Audio_");

		if (nCodec == 0)
			strcat (szSectName, "Dec_");
		else
			strcat (szSectName, "Enc_");
	}
	else if (nDomain == OMX_PortDomainVideo)
	{
		OMX_U32 n720PWidth = GetItemValue((OMX_STRING)"Video_Dec_720P",(OMX_STRING)"Width", 10240);
		OMX_U32 n720PHeight = GetItemValue((OMX_STRING)"Video_Dec_720P", (OMX_STRING)"Height", 10240);
		OMX_U32 n720PBitrate = GetItemValue((OMX_STRING)"Video_Dec_720P", (OMX_STRING)"Bitrate", 20480);	// 20K kbps

		OMX_BOOL b720P = OMX_FALSE;
		if((nParam1 >= n720PWidth && nParam2 >= n720PHeight) || (nParam3 / 1000) > n720PBitrate)
			b720P = OMX_TRUE;

		strcpy (szSectName, "Video_");

		if (nCodec == 0)
			strcat (szSectName, "Dec_");
		else
			strcat (szSectName, "Enc_");

		if(b720P == OMX_TRUE)
			strcat (szSectName, "720P_");
	}
	else
	{
		return false;
	}

	strcat (szSectName, pMIME);
	pCompName = GetCompName (szSectName, nFourCC ,nIndex);

/*
	if (pCompName == NULL && strcmp (pMIME, "Effect"))
	{
		if (nDomain == OMX_PortDomainAudio)
		{
			if (nCodec == 0)
				return VOCOMP_AUDIO_DEC;
			else
				return VOCOMP_AUDIO_ENC;
		}
		else 
		{
			if (nCodec == 0)
				return VOCOMP_VIDEO_DEC;
			else
				return VOCOMP_VIDEO_ENC;
		}
	}
*/
	return pCompName;
}

OMX_STRING voCOMXCfgComp::GetCompName (OMX_STRING pSectName, OMX_U32 nFourCC , OMX_U32 nIndex)
{
	OMX_U32 nCount = 0;

	COMXCfgSect * pSect = FindSect (pSectName);
	if (pSect == NULL)
		return NULL;

	COMXCfgItem * pItem = GetFirstItem ();
	while (pItem != NULL)
	{
		if (pItem->m_pSection == pSect)
			nCount++;
		pItem = pItem->m_pNext;
	}
	if (nCount <= 0 || nIndex >= nCount)
		return NULL;

	COMXCfgItem ** ppItems = (COMXCfgItem **)voOMXMemAlloc (nCount * sizeof (COMXCfgItem *));
	if (ppItems == NULL)
		return NULL;
	nCount = 0;

	pItem = GetFirstItem ();
	while (pItem != NULL)
	{
		if (pItem->m_pSection == pSect)
		{
			ppItems[nCount] = pItem;
			nCount++;
		}
		pItem = pItem->m_pNext;
	}

	if(nFourCC != 0)
	{
		for (int i = 0 ; i < (int)nCount ; i++)
		{
			if(!voOMXMemCompare(ppItems[i]->m_pName , &nFourCC , 4))
			{
				pItem = ppItems[i];

				voOMXMemFree (ppItems);
				return pItem->m_pValue;
			}
		}
		
	}

	OMX_U32 i = 0;
	OMX_U32 j = 0;

	OMX_U32 nPriority = 0XFFFF;
	OMX_U32 nPrioItem = 0XFFFF;

	OMX_U32	nFound = 0;

	for (j = 0; j < nIndex; j++)
	{
		nPriority = 0XFFFF;
		nFound = 0;
		for (i = 0; i < nCount; i++)
		{
			if (ppItems[i] != NULL)
			{
				nPrioItem = atol (ppItems[i]->m_pName);
				if (nPrioItem < nPriority)
				{
					nPriority = nPrioItem;
					nFound = i;
				}
			}
		}

		ppItems[nFound] = NULL;
	}

	nPriority = 0XFFFF;
	nFound = 0;
	for (OMX_U32 i = 0; i < nCount; i++)
	{
		if (ppItems[i] != NULL)
		{
			nPrioItem = atol (ppItems[i]->m_pName);
			if (nPrioItem < nPriority && nPrioItem > 0)
			{
				nPriority = nPrioItem;
				nFound = i;
			}
		}
	}

	pItem = ppItems[nFound];
	voOMXMemFree (ppItems);

	return pItem->m_pValue;
}

OMX_BOOL voCOMXCfgComp::GetSectName (OMX_STRING pSectName, OMX_U32 nDomain, OMX_U32 nCodec, OMX_U32 nCoding, OMX_U32 nParam1 /* = 0 */, OMX_U32 nParam2 /* = 0 */, OMX_U32 nParam3 /* = 0 */)
{
	if (nDomain == OMX_PortDomainAudio)
	{
		strcpy (pSectName, "Audio");

		if (nCodec == 0)
			strcat (pSectName, "_Dec");
		else
			strcat (pSectName, "_Enc");

		if (nCoding == OMX_AUDIO_CodingPCM)
			strcat (pSectName, "_PCM");
		else if (nCoding == OMX_AUDIO_CodingADPCM)
			strcat (pSectName, "_ADPCM");
		else if (nCoding == OMX_AUDIO_CodingAAC)
			strcat (pSectName, "_AAC");
		else if (nCoding == OMX_AUDIO_CodingMP3)
			strcat (pSectName, "_MP3");
		else if (nCoding == OMX_AUDIO_CodingAMR)
			strcat (pSectName, "_AMR");
		else if (nCoding == OMX_AUDIO_CodingWMA)
			strcat (pSectName, "_WMA");
		else if (nCoding == OMX_AUDIO_CodingRA)
			strcat (pSectName, "_RA");
		else if (nCoding == OMX_AUDIO_CodingQCELP8)
			strcat (pSectName, "_QCELP8");
		else if (nCoding == OMX_AUDIO_CodingQCELP13)
			strcat (pSectName, "_QCELP13");
		else if (nCoding == OMX_AUDIO_CodingEVRC)
			strcat (pSectName, "_EVRC");
		else if (nCoding == OMX_AUDIO_CodingGSMFR)
			strcat (pSectName, "_GSMFR");
		else if (nCoding == OMX_AUDIO_CodingGSMEFR)
			strcat (pSectName, "_GSMEFR");
		else if (nCoding == OMX_AUDIO_CodingGSMHR)
			strcat (pSectName, "GSMHR");
		else if (nCoding == OMX_AUDIO_CodingTDMAFR)
			strcat (pSectName, "_TDMAFR");
		else if (nCoding == OMX_AUDIO_CodingTDMAFR)
			strcat (pSectName, "_TDMAFR");
		else if (nCoding == OMX_AUDIO_CodingTDMAEFR)
			strcat (pSectName, "_TDMAEFR");
		else if (nCoding == OMX_AUDIO_CodingSMV)
			strcat (pSectName, "_SMV");
		else if (nCoding == OMX_AUDIO_CodingG711)
			strcat (pSectName, "_G711");
		else if (nCoding == OMX_AUDIO_CodingG723)
			strcat (pSectName, "_G723");
		else if (nCoding == OMX_AUDIO_CodingG726)
			strcat (pSectName, "_G726");
		else if (nCoding == OMX_AUDIO_CodingG729)
			strcat (pSectName, "_G729");
		else if (nCoding == OMX_AUDIO_CodingSBC)
			strcat (pSectName, "_SBC");
		else if (nCoding == OMX_AUDIO_CodingVORBIS)
			strcat (pSectName, "_VORBIS");
		else if (nCoding == OMX_AUDIO_CodingMIDI)
			strcat (pSectName, "_MIDI");
		else if (nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAC3)
			strcat (pSectName, "_AC3");
		else if (nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingEAC3)
			strcat (pSectName, "_EAC3");
		else if (nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAMRWBP)
			strcat (pSectName, "_AMRWBP");
		else if (nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingDTS)
			strcat (pSectName, "_DTS");
		else
			return OMX_FALSE;
	}
	else if (nDomain == OMX_PortDomainVideo)
	{
		OMX_U32 n720PWidth = GetItemValue((OMX_STRING)"Video_Dec_720P", (OMX_STRING)"Width", 10240);
		OMX_U32 n720PHeight = GetItemValue((OMX_STRING)"Video_Dec_720P", (OMX_STRING)"Height", 10240);
		OMX_U32 n720PBitrate = GetItemValue((OMX_STRING)"Video_Dec_720P", (OMX_STRING)"Bitrate", 20480);	// 20K kbps

		OMX_BOOL b720P = OMX_FALSE;
		if((nParam1 >= n720PWidth && nParam2 >= n720PHeight) || (nParam3 / 1000) > n720PBitrate)
			b720P = OMX_TRUE;

		strcpy (pSectName, "Video");

		if (nCodec == 0)
			strcat (pSectName, "_Dec");
		else
			strcat (pSectName, "_Enc");

		if(b720P == OMX_TRUE)
			strcat (pSectName, "_720P");

		if (nCoding == OMX_VIDEO_CodingMPEG2)
			strcat (pSectName, "_MPEG2");
		else if (nCoding == OMX_VIDEO_CodingH263)
			strcat (pSectName, "_H263");
		else if (nCoding == OMX_VIDEO_CodingMPEG4)
			strcat (pSectName, "_MPEG4");
		else if (nCoding == OMX_VIDEO_CodingWMV)
			strcat (pSectName, "_WMV");
		else if (nCoding == OMX_VIDEO_CodingRV)
			strcat (pSectName, "_RV");
		else if (nCoding == OMX_VIDEO_CodingAVC)
			strcat (pSectName, "_AVC");
		else if (nCoding == OMX_VIDEO_CodingMJPEG)
			strcat (pSectName, "_MJPEG");
		else if (nCoding == OMX_VO_VIDEO_CodingS263)
			strcat (pSectName, "_S263");
		else if (nCoding == OMX_VO_VIDEO_CodingVP6)
			strcat (pSectName, "_VP6");
		else if (nCoding == OMX_VO_VIDEO_CodingDIV3)
			strcat (pSectName, "_DIV3");
		else if (nCoding == OMX_VO_VIDEO_CodingVP8)
			strcat (pSectName, "_VP8");
		else if (nCoding == OMX_VO_VIDEO_CodingVP7)
			strcat (pSectName, "_VP7");
		else
			return OMX_FALSE;
	}
	else
		return OMX_FALSE;

	return OMX_TRUE;
}
