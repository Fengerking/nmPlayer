#include <stdio.h>
#include "voPDPort.h"
#include "voPD.h"
#include "voPDEngineSDK.h"

#include "CPDSessionWrapper2.h"
#include "CPDTrackWrapper2.h"
static int const MAX_TRACK_COUNT = 4;
class CVOPDSource2;
static CVOPDSource2*		  spNewAPI=NULL;
static 	VO_PTR VO_API  OpenPDFileReader(VO_U32 uID, VO_PTCHAR pName, VO_FILE_MODE uMode)			/*!< File open operation, return the file IO handle. failed return NULL*/;
static 	VO_S32 VO_API  ReadPDFileReader(VO_U32 uID, VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)	/*!< File read operation. return read number, failed return -1, retry return -2*/;
static 	VO_S32 VO_API  WritePDFileReader(VO_U32 uID, VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);	/*!< File write operation. return write number, failed return -1*/
static 	VO_S32 VO_API  FlushPDFileReader(VO_U32 uID, VO_PTR pFile);									/*!< File flush operation. return 0*/
static 	VO_S64 VO_API  SeekPDFileReader(VO_U32 uID, VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag);	/*!< File seek operation. return current file position, failed return -1*/
static 	VO_S64 VO_API  SizePDFileReader(VO_U32 uID, VO_PTR pFile);									/*!< File get size operation. return file size, failed return -1*/
static 	VO_S64 VO_API  SavePDFileReader(VO_U32 uID, VO_PTR pFile);									/*!< File get saved operation. return file saved size, failed return -1*/
static 	VO_S32 VO_API  ClosePDFileReader(VO_U32 uID, VO_PTR pFile);									/*!< File close operation, failed return -1*/

long VOFILEAPI  VOFILEOPEN_PD(HVOFILEREAD *ppPDSource, const char *pSourceFile, VOFILEOP *pFileOp);
long VOFILEAPI  VOFILECLOSE_PD(HVOFILEREAD pPDSource);
long VOFILEAPI  VOFILESETPARAMETER_PD(HVOFILEREAD pPDSource, LONG nID, LONG lValue);
long VOFILEAPI  VOFILEGETPARAMETER_PD(HVOFILEREAD pPDSource, LONG nID, LONG *plValue);
long VOFILEAPI  VOFILEFP2MT_PD(HVOFILEREAD pPDSource, int nFilePos, int* pnMediaTime, bool bStart);
long VOFILEAPI  VOFILEMT2FP_PD(HVOFILEREAD pPDSource, int nMediaTime, int* pnFilePos, bool bStart);

// PD Track Function
long VOFILEAPI  VOTRACKOPEN_PD(HVOFILETRACK * ppPDTrack, HVOFILEREAD pPDSource, int nIndex);
long VOFILEAPI  VOTRACKCLOSE_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack);
long VOFILEAPI  VOTRACKSETPARAMETER_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, LONG nID, LONG lValue);
long VOFILEAPI  VOTRACKGETPARAMETER_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, LONG nID, LONG *plValue);
long VOFILEAPI  VOTRACKGETINFO_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOTRACKINFO *pTrackInfo);
long VOFILEAPI  VOTRACKGETSAMPLEBYINDEX_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOSAMPLEINFO *pSampleInfo);
long VOFILEAPI  VOTRACKGETSAMPLEBYTIME_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOSAMPLEINFO *pSampleInfo);
long VOFILEAPI  VOTRACKGETNEXTKEYFRAME_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, int nIndex, int nDirectionFlag);
long VOFILEAPI  VOTRACKGETFPBYINDEX_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, int nIndex);


class CVOPDTrack2
{
public:
	CVOPDTrack2(HVOFILEREAD session,int index):
	  m_session(session),
	  m_headData(NULL)
	{
		voPDTrackOpen(&m_track, m_session, index) ;
		memset(&m_sampleInfo,0,sizeof(m_sampleInfo));
		memset(&m_nextKeyFrame,0,sizeof(m_nextKeyFrame));
	}
	~CVOPDTrack2()
	{
		SAFE_DELETE_ARRAY(m_headData);
		voPDTrackClose(m_session, m_track);
	}
	void	Init()
	{
		VO_SOURCE_TRACKINFO * pTrackInfo=&m_trackInfo;
		VOTRACKINFO	oldTrackInfo;
		voPDTrackInfo(m_session, m_track, &oldTrackInfo);
		pTrackInfo->Type			= (VO_SOURCE_TRACKTYPE)(oldTrackInfo.uType+1);//damn!
		pTrackInfo->Duration		= oldTrackInfo.uDuration;
		pTrackInfo->Start			= 0;//TODO
		//Get head data
		VOCODECDATABUFFER	buffer;
		buffer.buffer = NULL;
		buffer.length = 0;

		int dwRC = voPDTrackGetParameter (m_session, m_track,VOID_COM_HEAD_DATA, (LONG *)&buffer);
		if (dwRC == VORC_FILEREAD_OK && buffer.length > 0)
		{
			if (m_headData != NULL)
			{
				SAFE_DELETE_ARRAY(m_headData);
			}
			pTrackInfo->HeadSize=buffer.length;
			m_headData = new VO_BYTE[buffer.length];
			buffer.buffer = m_headData;
			dwRC = voPDTrackGetParameter (m_session, m_track,VOID_COM_HEAD_DATA, (LONG *)&buffer);
			if(dwRC==VORC_FILEREAD_OK)
				pTrackInfo->HeadData = m_headData;
		}

		switch(oldTrackInfo.uCodec)
		{
		case VC_H264:
		case VC_AVC:
			pTrackInfo->Codec=VO_VIDEO_CodingH264;
			break;
		case VC_H263:
			pTrackInfo->Codec=VO_VIDEO_CodingH263;
			break;
		case VC_MPEG4:
			pTrackInfo->Codec=VO_VIDEO_CodingMPEG4;
			break;
		case VC_WMV:
			pTrackInfo->Codec=VO_VIDEO_CodingWMV;
			break;
		case VC_RV:
			pTrackInfo->Codec=VO_VIDEO_CodingRV;
			break;
		case VC_DIVX:
			pTrackInfo->Codec=VO_VIDEO_CodingDIVX;
			break;
		case VC_MPEG2:
			pTrackInfo->Codec=VO_VIDEO_CodingMPEG2;
			break;
		
		case AC_AAC:
			pTrackInfo->Codec=VO_AUDIO_CodingAAC;

			break;
		case AC_AMR:
			pTrackInfo->Codec=VO_AUDIO_CodingAMRNB;

			break;
		case AC_AWB:
			pTrackInfo->Codec=VO_AUDIO_CodingAMRWB;

			break;
		case AC_AWBP:
			pTrackInfo->Codec=VO_AUDIO_CodingAMRWBP;

			break;
		case AC_QCELP:
			pTrackInfo->Codec=VO_AUDIO_CodingQCELP13;

			break;
		case AC_WMA:
			pTrackInfo->Codec=VO_AUDIO_CodingWMA;

			break;
		case AC_RA:
			pTrackInfo->Codec=VO_AUDIO_CodingRA;
		case AC_MP3:
			pTrackInfo->Codec=VO_AUDIO_CodingMP3;
		case AC_EVRC:
			pTrackInfo->Codec=VO_AUDIO_CodingEVRC;
		case AC_OGG:
			pTrackInfo->Codec=VO_AUDIO_CodingOGG;
			break;
		case AC_AC3	:
			pTrackInfo->Codec=VO_AUDIO_CodingAC3;
		case AC_FLAC:
			pTrackInfo->Codec=VO_AUDIO_CodingFLAC;
		case AC_PCM:
			pTrackInfo->Codec=VO_AUDIO_CodingPCM;
		case AC_MIDI	:
			pTrackInfo->Codec=VO_AUDIO_CodingMIDI;
			break;
		default:
			break;
		}
	}
	VO_U32 SetTrackParam(VO_U32 uID, VO_PTR pParam)
	{
		return voPDTrackSetParameter(m_session, m_track, uID, (LONG) pParam);
	}
	VO_U32 GetTrackParam (VO_U32 uID, VO_PTR pParam)
	{
		return voPDTrackGetParameter(m_session, m_track, uID, (LONG*) pParam);
	}
	VO_U32	GetTrackInfo (VO_SOURCE_TRACKINFO * pTrackInfo)
	{
			memcpy(pTrackInfo,&m_trackInfo,sizeof(m_trackInfo));
			return 0;
	}
	VO_U32 GetSample (VO_SOURCE_SAMPLE * pSample)
	{
		
		m_sampleInfo.pBuffer=pSample->Buffer;
		VO_U32 inputTime=pSample->Time;
		if(m_trackInfo.Type==VO_SOURCE_TT_VIDEO)
		{
			//update the next key frame
			if(m_nextKeyFrame.uTime<inputTime)
			{
				long nextKey=voPDTrackGetNextKeyFrame(m_session, m_track, m_sampleInfo.uIndex, 1);
				if(nextKey>m_sampleInfo.uIndex)
				{
					m_nextKeyFrame.uIndex = nextKey;
					int ret=voPDTrackGetSampleByIndex(m_session, m_track,  &m_nextKeyFrame);
					if(ret)//TODO
					{
						//m_nextKeyFrame.uIndex = 0;	
					}
				}
#define MIN_FORWARD_TIME 1000
				if (inputTime>m_sampleInfo.uTime+MIN_FORWARD_TIME)
				{
					m_sampleInfo.uIndex = m_nextKeyFrame.uIndex;
				}
			}
			
			
		}
		int ret=voPDTrackGetSampleByIndex(m_session, m_track,  &m_sampleInfo);
		
		if(ret==0)
		{
			m_sampleInfo.uIndex++;
			pSample->Size				= m_sampleInfo.uSize;
			pSample->Time				= m_sampleInfo.uTime;
			pSample->Duration		= m_sampleInfo.uDuration;
		}
		return ret;
	}
	VO_U32 SetPos (VO_S32 * pPos)
	{
		m_sampleInfo.uIndex=0;
		return 0;
	}
private:
	HVOFILETRACK								m_track;
	VO_PBYTE										m_headData;
	HVOFILEREAD									m_session;
	VOSAMPLEINFO								m_sampleInfo;
	VO_SOURCE_TRACKINFO			m_trackInfo;
	VOSAMPLEINFO								m_nextKeyFrame;
};
class CVOPDSource2
{
public:
		CVOPDSource2(VO_SOURCE_OPENPARAM * pParam)
		{
			memset(&m_tracks,0,sizeof(m_tracks));
			m_sourceInfo.Tracks=0;
			if(voPDSessionCreate(&m_session,NULL))
				m_session=NULL;//error
			else
			{
				//set param first
				memcpy(&m_param,pParam,sizeof(VO_SOURCE_OPENPARAM ));
				memcpy(&m_newReaderAPI,pParam->pSourceOP,sizeof(VO_SOURCE_READAPI));
				long version=1;
				voPDSessionSetParameter(m_session, VOID_STREAMING_SDK_VERSION,(LONG)&version);
				voPDSessionSetParameter(m_session, VOID_STREAMING_INIT_PARAM, (LONG)m_param.nReserve);
				m_oldReaderAPI.close				= VOFILECLOSE_PD;
				m_oldReaderAPI.closeTrack	=	VOTRACKCLOSE_PD;
				m_oldReaderAPI.getNextKeyFrame	= VOTRACKGETNEXTKEYFRAME_PD;
				m_oldReaderAPI.getParam					=	VOFILEGETPARAMETER_PD;
				m_oldReaderAPI.getPosByIndex		=	VOTRACKGETFPBYINDEX_PD;
				m_oldReaderAPI.getSampleByTime	= VOTRACKGETSAMPLEBYTIME_PD;
				m_oldReaderAPI.getTrackInfo			= VOTRACKGETINFO_PD;
				m_oldReaderAPI.getTrackParam		= VOTRACKGETPARAMETER_PD;
				m_oldReaderAPI.open							= VOFILEOPEN_PD;
				m_oldReaderAPI.openTrack				= VOTRACKOPEN_PD;
				m_oldReaderAPI.pos2time					= VOFILEFP2MT_PD;
				m_oldReaderAPI.setParam					= VOFILESETPARAMETER_PD;
				m_oldReaderAPI.setTrackParam		= VOTRACKSETPARAMETER_PD;
				m_oldReaderAPI.time2pos					= VOFILEMT2FP_PD;

				m_newFI
			}
		}
		~CVOPDSource2()
		{
			for (int i=0;i<m_sourceInfo.Tracks;i++)
			{
				SAFE_DELETE(m_tracks[i]);
			}
			if(m_session)
				voPDSessionClose(m_session);
		}
		VO_U32	OpenURL()
		{
			int ret=1;
			if(m_param.pSource)
			{
				
					ret=voPDSessionOpenURL(m_session, (TCHAR *) m_param.pSource,&m_oldReaderAPI);
					if(ret==0)
					{
						voPDSessionGetParameter (m_session,VOID_FILEREAD_TRACKS, (LONG *)&(m_sourceInfo.Tracks));
						voPDSessionGetParameter(m_session,VOID_COM_DURATION,(LONG *)&(m_sourceInfo.Duration));
						if(m_sourceInfo.Tracks<0||m_sourceInfo.Tracks>=MAX_TRACK_COUNT)
						{
							voLog(LL_TRACE,"error.txt","m_sourceInfo.Tracks<0||m_sourceInfo.Tracks>=MAX_TRACK_COUNT\n");
							//break;
						}
						for (int i=0;i<m_sourceInfo.Tracks;i++)
						{
							m_tracks[i] = new CVOPDTrack2(m_session,i);
							m_tracks[i]->Init();
						}
					}

			
				
			}
			return ret;
		}
		VO_U32	GetSourceInfo(VO_SOURCE_INFO * pSourceInfo)
		{
			memcpy(pSourceInfo,&m_sourceInfo,sizeof(VO_SOURCE_INFO));
			return 0;
		}
		VO_U32 GetTrackInfo (VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
		{
			int ret=-1;
			if(m_tracks[nTrack])
			{
				ret = m_tracks[nTrack]->GetTrackInfo(pTrackInfo);
			}
			return ret;
		}
		VO_U32 GetSample (VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
		{
			int ret=-1;
			if(m_tracks[nTrack])
			{
				ret = m_tracks[nTrack]->GetSample(pSample);
			}
			return ret;
		}
		VO_U32 SetPos (VO_U32 nTrack, VO_S32 * pPos)
		{
			if(nTrack==0)//only seek once
			{
				VO_S32 newPos=*pPos;
				voPDSessionSetParameter(m_session,  VOID_FILEREAD_ACTION_SEEK, (LONG)&newPos);
			}
			int ret=-1;
			if(m_tracks[nTrack])
			{
				ret = m_tracks[nTrack]->SetPos(pPos);
			}
			return ret;
		}
		VO_U32 SetSourceParam ( VO_U32 uID, VO_PTR pParam)
		{
			return voPDSessionSetParameter(m_session, uID, (LONG) pParam );
		}
		VO_U32 GetSourceParam (VO_U32 uID, VO_PTR pParam)
		{
			return voPDSessionGetParameter(m_session, uID, (LONG* )pParam);
		}
		VO_U32 SetTrackParam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
		{
			int ret=-1;
			if(m_tracks[nTrack])
			{
				ret = m_tracks[nTrack]->SetTrackParam(uID,pParam);
			}
			return ret;
		}
		VO_U32 GetTrackParam (VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
		{
			int ret=-1;
			if(m_tracks[nTrack])
			{
				ret = m_tracks[nTrack]->GetTrackParam(uID,pParam);
			}
			return ret;
		}
		VO_SOURCE_READAPI*	GetNewFileReaderOP(){return &m_newReaderAPI;}
		VOFILEOP*							GetOldFileOP(){return &m_oldFileOP;}
private:
	VO_SOURCE_OPENPARAM		m_param;
	VO_SOURCE_INFO						m_sourceInfo;
	HVOFILEREAD								m_session;
	CVOPDTrack2*								m_tracks[MAX_TRACK_COUNT];
	int														m_sourceInfo.Tracks;
	TFileReaderAPI								m_oldReaderAPI;
	VOFILEOP										m_oldFileOP;
	VO_SOURCE_READAPI				m_newReaderAPI;
};
static 	VO_PTR VO_API  OpenPDFileReader(VO_U32 uID, VO_PTCHAR pName, VO_FILE_MODE uMode)			/*!< File open operation, return the file IO handle. failed return NULL*/
{
	VO_PTR result = NULL;
	if(spNewAPI)
	{
		VOFILEOP * oldOP=spNewAPI->GetOldFileOP();
		if(oldOP)
		{
			result =  oldOP->voOpenFile((const char*)pName,FO_READ_ONLY);
		}
	}
	return result;
}
static 	VO_S32 VO_API  ReadPDFileReader(VO_U32 uID, VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)	/*!< File read operation. return read number, failed return -1, retry return -2*/
{
	int resultSize=-1;
	if(spNewAPI)
	{
		VOFILEOP * oldOP=spNewAPI->GetOldFileOP();
		if(oldOP)
		{

			oldOP->voReadFile(pFile,pBuffer,uSize,&resultSize);
		}
	}
	return resultSize;
}
static 	VO_S32 VO_API  WritePDFileReader(VO_U32 uID, VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);	/*!< File write operation. return write number, failed return -1*/
static 	VO_S32 VO_API  FlushPDFileReader(VO_U32 uID, VO_PTR pFile);									/*!< File flush operation. return 0*/
static 	VO_S64 VO_API  SeekPDFileReader(VO_U32 uID, VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag);	/*!< File seek operation. return current file position, failed return -1*/
static 	VO_S64 VO_API  SizePDFileReader(VO_U32 uID, VO_PTR pFile);									/*!< File get size operation. return file size, failed return -1*/
static 	VO_S64 VO_API  SavePDFileReader(VO_U32 uID, VO_PTR pFile);									/*!< File get saved operation. return file saved size, failed return -1*/
static 	VO_S32 VO_API  ClosePDFileReader(VO_U32 uID, VO_PTR pFile);									/*!< File close operation, failed return -1*/

long VOFILEAPI  VOFILEOPEN_PD(HVOFILEREAD *ppPDSource, const char *pSourceFile, VOFILEOP *pFileOp);
long VOFILEAPI  VOFILECLOSE_PD(HVOFILEREAD pPDSource);
long VOFILEAPI  VOFILESETPARAMETER_PD(HVOFILEREAD pPDSource, LONG nID, LONG lValue);
long VOFILEAPI  VOFILEGETPARAMETER_PD(HVOFILEREAD pPDSource, LONG nID, LONG *plValue);
long VOFILEAPI  VOFILEFP2MT_PD(HVOFILEREAD pPDSource, int nFilePos, int* pnMediaTime, bool bStart);
long VOFILEAPI  VOFILEMT2FP_PD(HVOFILEREAD pPDSource, int nMediaTime, int* pnFilePos, bool bStart);

// PD Track Function
long VOFILEAPI  VOTRACKOPEN_PD(HVOFILETRACK * ppPDTrack, HVOFILEREAD pPDSource, int nIndex);
long VOFILEAPI  VOTRACKCLOSE_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack);
long VOFILEAPI  VOTRACKSETPARAMETER_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, LONG nID, LONG lValue);
long VOFILEAPI  VOTRACKGETPARAMETER_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, LONG nID, LONG *plValue);
long VOFILEAPI  VOTRACKGETINFO_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOTRACKINFO *pTrackInfo);
long VOFILEAPI  VOTRACKGETSAMPLEBYINDEX_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOSAMPLEINFO *pSampleInfo);
long VOFILEAPI  VOTRACKGETSAMPLEBYTIME_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOSAMPLEINFO *pSampleInfo);
long VOFILEAPI  VOTRACKGETNEXTKEYFRAME_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, int nIndex, int nDirectionFlag);
long VOFILEAPI  VOTRACKGETFPBYINDEX_PD(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, int nIndex);
/**
* Open the source and return source handle
* \param ppHandle [OUT] Return the source operator handle
* \param pName	[IN] The source name
* \param pParam [IN] The source open param
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDOpen2(VO_PTR * ppHandle, VO_SOURCE_OPENPARAM * pParam)
{
	*ppHandle = NULL;
	CVOPDSource2* source=new CVOPDSource2(pParam);
	int ret=source->OpenURL();
	if(ret==0)
	{
		*ppHandle=source;
		spNewAPI=source;
	}
	return ret;
}

/**
* Close the opened source.
* \param pHandle [IN] The handle which was create by open function.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDClose2(VO_PTR pHandle)
{
		CVOPDSource2* source = (CVOPDSource2*)pHandle;
		SAFE_DELETE(source);
		return 0;
}

/**
* Get the source information
* \param pHandle [IN] The handle which was create by open function.
* \param pSourceInfo [OUT] The structure of source info to filled.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetSourceInfo2(VO_PTR pHandle, VO_SOURCE_INFO * pSourceInfo)
{
	CVOPDSource2* source = (CVOPDSource2*)pHandle;
	return source->GetSourceInfo(pSourceInfo);
}

/**
* Get the track information
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pTrackInfo [OUT] The track info to filled..
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetTrackInfo2(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	CVOPDSource2* source = (CVOPDSource2*)pHandle;
	return source->GetTrackInfo(nTrack,pTrackInfo);
}

/**
* Get the track buffer
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pSample [OUT] The sample info was filled.
*		  Audio. It will fill the next frame audio buffer automatically.
*		  Video  It will fill the frame data depend on the sample time. if the next key frmae time
*				 was less than the time, it will fill the next key frame data, other, it will fill
*				 the next frame data.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetSample2(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	CVOPDSource2* source = (CVOPDSource2*)pHandle;
	return source->GetSample(nTrack,pSample);
}

/**
* Set the track read position.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pPos [IN/OUT] The new pos will be set, and it will reset with previous key frame time if it is video track.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDSetPos2(VO_PTR pHandle, VO_U32 nTrack, VO_S64 * pPos)
{
	CVOPDSource2* source = (CVOPDSource2*)pHandle;
	VO_S32 pos=(VO_S32)*pPos;
	VO_U32 ret = source->SetPos(nTrack,&pos);
	if(ret==0)
		*pPos=pos;
	return ret;
}

/**
* Set source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDSetSourceParam2(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CVOPDSource2* source = (CVOPDSource2*)pHandle;
	return source->SetSourceParam(uID,pParam);
}

/**
* Get source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [Out] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetSourceParam2(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CVOPDSource2* source = (CVOPDSource2*)pHandle;
	return source->GetSourceParam(uID,pParam);
}

/**
* Set track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDSetTrackParam2(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	CVOPDSource2* source = (CVOPDSource2*)pHandle;
	return source->SetTrackParam(nTrack,uID,pParam);
}

/**
* Get track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetTrackParam2(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	CVOPDSource2* source = (CVOPDSource2*)pHandle;
	return source->GetTrackParam(nTrack,uID,pParam);
}

VO_S32 VO_API voGetPDReadAPI (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag)
{
	if(pReadHandle)
	{
		pReadHandle->Close				 = voPDClose2;
		pReadHandle->GetSample		 =	voPDGetSample2;
		pReadHandle->GetSourceInfo =	voPDGetSourceInfo2;
		pReadHandle->GetSourceParam	=	voPDGetSourceParam2;
		pReadHandle->GetTrackInfo		=	voPDGetTrackInfo2;
		pReadHandle->GetTrackParam	=	voPDGetTrackParam2;
		pReadHandle->Open				 =		voPDOpen2;
		pReadHandle->SetPos				=		voPDSetPos2;
		pReadHandle->SetSourceParam	=	voPDSetSourceParam2;
		pReadHandle->SetTrackParam	=	voPDSetTrackParam2;

	}
	return 0;
}




