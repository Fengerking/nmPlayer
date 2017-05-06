#include "CBaseAssembler.h"
#include "CBaseDumper.h"
#include "cmnMemory.h"
#include "CAnalyseMPEG4.h"
#include "CAnalyseAAC.h"
#include "CAnalysePCM.h"
#include "voLog.h"
#include "CAnalyseH264.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseAssembler::CBaseAssembler(void)
: m_pDumper(VO_NULL)
, m_pAssembleInitInfo(VO_NULL)
{
	for (VO_BYTE n=0; n<MAX_AV_STREAM_COUNT; n++)
	{
		m_pExtData[n]			= VO_NULL;
		m_bWaitFirstFrame[n]	= VO_TRUE;
	}
}

CBaseAssembler::~CBaseAssembler(void)
{
	ReleaseExtData();
}

VO_VOID	CBaseAssembler::Open(ASSEMBLERINITINFO* pInit)
{
	m_pAssembleInitInfo = pInit;
	m_pDumper			= pInit->dumper;
}

VO_VOID	CBaseAssembler::Close()
{
	m_HeadDataList.Release();
}

VO_VOID	CBaseAssembler::AddSample(VO_BOOL bSync, VO_SINK_SAMPLE * pSample)
{
	// handle first frame, insert head data at the begin of frame
	if (m_bWaitFirstFrame[pSample->nAV])
	{
		if(!m_pExtData[pSample->nAV])
		{
			VO_SINK_EXTENSION* pExt = CheckExtData(m_pAssembleInitInfo->codec_list[pSample->nAV], pSample->nAV, pSample->Buffer, pSample->Size);

			if (pExt)
			{
				AddExtDataEx(pSample->nAV, pExt);

				// add to head data
				if (VO_AUDIO_CodingAAC == m_pAssembleInitInfo->codec_list[pSample->nAV])
				{
					// detect adts header from input stream
					m_HeadDataList.Append(pSample->nAV, VO_NULL, 0);
				}
				else
				{
					VO_MEM_INFO vmi = {0};
					vmi.Size		= pExt->Size;
					cmnMemAlloc(0, &vmi);
					cmnMemCopy(0, vmi.VBuffer, pExt->Buffer, pExt->Size);
					m_HeadDataList.Append(pSample->nAV, (VO_BYTE*)vmi.VBuffer, pExt->Size);
				}
				// end
			}
			else
				return doAddSample(bSync, pSample);
		}

		m_bWaitFirstFrame[pSample->nAV] = VO_FALSE;

		VO_BOOL bAddHead = VO_FALSE;
		VO_U16 head_len = 0;
		VO_U16 codec = m_pAssembleInitInfo->codec_list[pSample->nAV];
		if (codec == VO_VIDEO_CodingMPEG4 || codec == VO_VIDEO_CodingH264)
		{
			head_len = m_pExtData[pSample->nAV]->Size;
		}
		else if (codec == VO_AUDIO_CodingAAC)
		{
			bAddHead = VO_TRUE;
			head_len = m_pExtData[pSample->nAV]->Size;
		}

		// KMP
		if (bAddHead)
		{
			VO_SINK_SAMPLE mock;
			mock.nAV		= pSample->nAV;
			mock.Duration	= pSample->Duration;
			mock.Size		= pSample->Size + head_len;
			mock.Time		= pSample->Time;

			VO_MEM_INFO vmi = {0};
			vmi.Size		= mock.Size;
			cmnMemAlloc(0, &vmi);
			mock.Buffer		= (VO_BYTE*)vmi.VBuffer;

			cmnMemCopy(0, mock.Buffer, m_pExtData[pSample->nAV]->Buffer, head_len);
			cmnMemCopy(0, mock.Buffer+head_len, pSample->Buffer, pSample->Size);

			doAddSample(bSync, &mock);

			cmnMemFree(0, mock.Buffer);
		}
		else
			doAddSample(bSync, pSample);
	}
	else
		doAddSample(bSync, pSample);
}

VO_U32 CBaseAssembler::Write(VO_BYTE* pData, VO_U32 nLen)
{
	if (m_pDumper)
	{
		return m_pDumper->Write(pData, nLen);
	}

	return VO_ERR_NONE;
}

// from set head data
VO_VOID	CBaseAssembler::AddExtData(VO_U16 nStreamID, VO_SINK_EXTENSION* pExtData)
{
	// here has iisue, need ID identify ext data, now mux support only 2 stream
	m_pExtData[nStreamID] = CloneExtData(pExtData);

	if (m_pAssembleInitInfo->codec_list[pExtData->nAV] == VO_AUDIO_CodingAAC)
	{
		VO_BYTE* pData = new VO_BYTE[7];
		AddHeadData(nStreamID, pData, 7);
	}
	else
	{
		VO_BYTE* pData = new VO_BYTE[pExtData->Size];
		cmnMemCopy(0, pData, pExtData->Buffer, pExtData->Size);

		AddHeadData(nStreamID, pData, pExtData->Size);
	}
		
}

VO_VOID	CBaseAssembler::AddExtDataEx(VO_U16 nStreamID, VO_SINK_EXTENSION* pExtData)
{
	m_pExtData[nStreamID] = pExtData;
}

VO_VOID CBaseAssembler::ReleaseExtData()
{
	for (VO_BYTE n=0; n<MAX_AV_STREAM_COUNT; n++)
	{
		VO_SINK_EXTENSION* pExt = m_pExtData[n];
		if(pExt)
		{
			if (pExt->Buffer)
			{
				cmnMemFree(0, pExt->Buffer);
				pExt->Buffer = VO_NULL;
			}

			pExt->Size = 0;

			delete pExt;
		}

		m_pExtData[n] = VO_NULL;
	}

}

VO_SINK_EXTENSION*	CBaseAssembler::CloneExtData(VO_SINK_EXTENSION* pExtData)
{
	VO_SINK_EXTENSION* pNew = new VO_SINK_EXTENSION;

	pNew->nAV		= pExtData->nAV;
	pNew->Size		= pExtData->Size;

	VO_MEM_INFO vmi = {0};
	vmi.Size		= pExtData->Size;
	cmnMemAlloc(0, &vmi);
	if(vmi.VBuffer)
	{
		cmnMemCopy(0, vmi.VBuffer, pExtData->Buffer, pExtData->Size);
		pNew->Buffer	= (VO_BYTE*)vmi.VBuffer;

		return pNew;
	}
	return VO_NULL;
}

VO_SINK_EXTENSION*	CBaseAssembler::GetExtDataByID(VO_U16 nID)
{
	if (m_pExtData[nID])
	{
		return m_pExtData[nID];
	}

	return VO_NULL;
}

VO_U16 CBaseAssembler::GetExtDataLenByID(VO_U16 nID)
{
	if (nID == 0) // adts header
	{
		return 7;
	}
	else
	{
		VO_SINK_EXTENSION* pExt = GetExtDataByID(nID);
		if (pExt)
		{
			return pExt->Size;
		}
	}

	return 0;
}


VO_SINK_EXTENSION*  CBaseAssembler::CheckExtData(VO_U16 nCodec, VO_U16 nStreamID, VO_BYTE* pFrameStart, VO_U32 nFrameLen)
{
	VO_SINK_EXTENSION* pExt = GetExtDataByID(nStreamID);
	if (pExt)
	{
		return pExt;
	}

	CBaseAnalyseData* analyser	= VO_NULL;
	VO_BOOL bRet				= VO_FALSE;
	VO_U32 headsize				= 0;
	VO_BYTE* headdata			= VO_NULL;

	if (nStreamID == 0)
	{
		switch (nCodec)
		{
		case VO_AUDIO_CodingAAC:
			{
				analyser = new CAnalyseAAC;
			}
			break;
		case VO_AUDIO_CodingPCM:
			{
				analyser = new CAnalysePCM;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch(nCodec)
		{
		case VO_VIDEO_CodingMPEG4:
			{
				analyser = new CAnalyseMPEG4;
			}
			break;
		case VO_VIDEO_CodingH264:
			{
				analyser = new CAnalyseH264;
			}
			break;
		default:
			break;
		}
	}
	if (analyser)
	{
		bRet = analyser->AnalyseData(pFrameStart, nFrameLen);
		if (bRet)
		{
			headdata = analyser->GetSequenceHeader(headsize);
			if (headdata && headsize>0)
			{
				pExt			= new VO_SINK_EXTENSION;
				pExt->nAV		= nStreamID;
				pExt->Size		= headsize;

				VO_MEM_INFO vmi = {0};
				vmi.Size		= headsize;
				cmnMemAlloc(0, &vmi);
				cmnMemCopy(0, vmi.VBuffer, headdata, headsize);
				pExt->Buffer	= (VO_BYTE*)vmi.VBuffer;
			}
		}

		delete analyser;
	}

	return pExt;
}

VO_BOOL	CBaseAssembler::GetHeadData(VO_U16 nStreamID, VO_BYTE** pData, VO_U32* pDataLen)
{
	CItem* pItem = m_HeadDataList.GetItemByID(nStreamID);

	if(!pItem)
		return VO_FALSE;
	
	*pData		= (VO_BYTE*)pItem->Data();
	*pDataLen	= pItem->DataLen();

	return VO_TRUE;
}


VO_BOOL	CBaseAssembler::AddHeadData(VO_U16 nStreamID, VO_BYTE*  pData, VO_U32 pDataLen)
{
	m_HeadDataList.Append(nStreamID, pData, pDataLen);
	return VO_TRUE;
}

