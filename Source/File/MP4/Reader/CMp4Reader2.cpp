#include "CMp4Reader2.h"
#ifdef _SUPPORT_PARSER_METADATA
#include "CMP4MetaData.h"
#endif	//_SUPPORT_PARSER_METADATA

#include "voLog.h"
#include "voOSFunc.h"
#include "voSource2.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


const double TIME_UNIT = 1.0;	//all time multiply this value is MS

CMp4Reader2::CMp4Reader2(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_pReader(VO_NULL)
	, m_pStream(VO_NULL)
	, m_pMovie(VO_NULL)
	, m_bPushPlay(VO_FALSE)
	, m_pReaderPushPlay(VO_NULL)
	, m_pStreamPushPlay(VO_NULL)
{
}

CMp4Reader2::~CMp4Reader2()
{
	Close();
}
//2011/11/21,by leon ,fix 9158
CBaseTrack* CMp4Reader2::GetTrackByIndex(VO_U32 nIndex)
{
	CBaseTrack* track = CBaseReader::GetTrackByIndex(nIndex);
	if(track && track->IsInUsed())
		return track;
	
	return VO_NULL;
}

VO_U32 CMp4Reader2::SelectDefaultTracks()
{
	VO_U32 nFlags = 0;	//0x1 - audio selected, 0x2 - video selected

	if (!m_nTracks)
	{
		return VO_ERR_SOURCE_TRACKNOTFOUND;
	}

	for(VO_S32 i = 0; i < (VO_S32)m_nTracks; i++)
	{
		if(m_ppTracks[i])
		{
			//add by leon #6335
			VO_U32 pCodec = 0;
			if(m_ppTracks[i]->GetCodec(&pCodec) != VO_ERR_SOURCE_OK || pCodec <=0 )
				continue;
	
			m_ppTracks[i]->SetInUsed(VO_TRUE);
		}
	}
	return VO_ERR_SOURCE_OK;
}
VO_U32 CMp4Reader2::SetSelectTrack(VO_U32 nIndex, VO_BOOL bInUsed,VO_U64 llTs)
{

	CBaseTrack* pTrack = GetTrackByIndex(nIndex);
	if(!pTrack)
		return VO_ERR_SOURCE_TRACKNOTFOUND;
	
	VO_S64 sPos = (VO_S64)llTs;
 	if(pTrack->SetPos(&sPos) == VO_ERR_SOURCE_OK)
 		return VO_ERR_SOURCE_OK;
 	else
		return VO_ERR_SOURCE_ERRORDATA;
}

VO_U32 CMp4Reader2::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	Close();
	VOLOGI("+CMp4Reader2::Load, File op: %d",(int)m_pFileOp);
	m_pStream = new CMp4FileStream(m_pFileOp);

	if(!m_pStream)
		return VO_ERR_OUTOF_MEMORY;
	VOLOGI("file name:%s", pFileSource->pSource);
	if(!m_pStream->Open(pFileSource))
		return VO_ERR_SOURCE_OPENFAIL;

	m_ullFileSize = m_pStream->Size();

	m_pReader = new ReaderMSB(m_pStream, m_ullFileSize);
	m_pMovie = new Movie;

	m_pMovie->SetDrmReader(this);
#ifdef H265_DASH
	 m_pMovie->DisableBoxType(FOURCC2_mfra);
#else
	if (VO_SOURCE_OPENPARAM_FLAG_PUSHPLAY & nSourceOpenFlags)
	{
		m_bPushPlay = VO_TRUE;
		///<disable the mfra because this box is end of file,we can not parse it when pushplay
		m_pMovie->DisableBoxType(FOURCC2_mfra);
	}
#endif
	uint32 g = voOS_GetSysTime();

	if((VO_SOURCE_OPENPARAM_FLAG_OPENPD == (nSourceOpenFlags & 0xFF) || VO_SOURCE_OPENPARAM_FLAG_PUSHPLAY & nSourceOpenFlags )
		? m_pMovie->ProgressiveOpen(m_pReader) : m_pMovie->Open(m_pReader,VO_FALSE))
	{
		VOLOGE("Mp4Parser:Open File Failed+++++++++++++++++++++");
		return VO_ERR_SOURCE_OPENFAIL;
	}
	VOLOGR("Load1 Cost:%d", voOS_GetSysTime() - g);
	g = voOS_GetSysTime();


	rc = InitTracks(nSourceOpenFlags);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;
	VOLOGR("Load2 Cost:%d", voOS_GetSysTime() - g);
	g = voOS_GetSysTime();
//	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	{
		rc = SelectDefaultTracks();
		if (VO_ERR_SOURCE_OK != rc)
		{
			return rc;
		}
		rc = PrepareTracks();	
	}
	VOLOGR("Load3 Cost:%d", voOS_GetSysTime() - g);
#if 0 //test
	video_matrix vm;
	GetParameter(VO_PID_FILE_GETVIDEOMATRIX, &vm);

#pragma pack(push, 1)
	//see SVN:\Numen\Source\File\MP4\MP4Base\mp4cfg.h for other decoder specific structures
	struct H263DecSpecStruc
	{
		uint32 vendor;
		uint8 decoder_version;
		uint8 H263_Level;
		uint8 H263_Profile;
	}
#ifdef __GNUC__
	__attribute__((packed)) 
#endif //__GNUC__
	h263ds;
#pragma pack(pop, 1)

	dec_spec_data dsd = { sizeof(h263ds), &h263ds };
	rc = m_ppTracks[0]->GetParameter(VO_PID_FILE_GETDECSPECDATA, &dsd);
	rc = m_ppTracks[1]->GetParameter(VO_PID_FILE_GETDECSPECDATA, &dsd);
#endif
	VOLOGI("-CMp4Reader2::Load");
	return rc;
}

VO_U32 CMp4Reader2::Close()
{
	VOLOGR("+Close");
	UnprepareTracks();
	UninitTracks();

	VOLOGR("-1");
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		SAFE_DELETE(m_ppTracks[i]);
	}
	VOLOGR("-12");

	if(m_pMovie)
		m_pMovie->Close();

	SAFE_DELETE(m_pReader);
	SAFE_DELETE(m_pStream);
	SAFE_DELETE(m_pMovie);
	SAFE_DELETE(m_pReaderPushPlay);
	SAFE_DELETE(m_pStreamPushPlay);
VOLOGR("-Close");
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Reader2::InitTracks(VO_U32 nSourceOpenFlags)
{
	VO_S32 nTracks = m_pMovie->GetTrackCount();
	if(nTracks > 0)
	{
		Track** ppTracks = NEW_OBJS(Track*, nTracks);
		if(ppTracks)
		{
			if(VO_ERR_SOURCE_OK == TracksCreate(nTracks))
			{
				MP4RRC rc = MP4RRC_OK;
				for(VO_S32 i = 0; i < nTracks; i++)
				{
					ppTracks[i] = VO_NULL;
					rc = m_pMovie->OpenTrack(i, ppTracks[i]);

					if(MP4RRC_OK == rc)
					{
						if (ppTracks[i]->IsIsml() && !m_bPushPlay)
						{
							VOLOGI("Use PD ISml");
							ppTracks[i]->SetPDIsmv(VO_TRUE);
						}
						CMp4Track2* pNewTrack = NULL;
#if 0 ///<disable the double Io design because the drm crash
						if(m_bPushPlay && ppTracks[i]->IsIsml())
						{
							pNewTrack = new CMp4TrackPushPlay(ppTracks[i], m_pReader, m_pMemOp,this);
						}
						else
#endif
						{
							pNewTrack = new CMp4Track2(ppTracks[i], m_pReader, m_pMemOp);
						}
						
						if(!pNewTrack)
							break;
					
						TracksAdd(pNewTrack);
					}
					else
					{
						if (ppTracks[i])
						{
							ppTracks[i]->Close();

							delete ppTracks[i];
							ppTracks[i] = VO_NULL;
						}
					}
				}
			}

			MemFree(ppTracks);
		}
	}

	return CBaseReader::InitTracks(nSourceOpenFlags);
}

VO_U32 CMp4Reader2::GetFileHeadSize(VO_U32* pdwFileHeadSize)
{
	*pdwFileHeadSize = (VO_U32)m_pMovie->GetHeaderSize();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Reader2::GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	if(!pParam)
		return VO_ERR_INVALID_ARG;

	VO_S64 llMediaTime = VO_MAXU32;
	VO_S64 llTmp = VO_MAXU32;
	VO_U32 dwTrackDuration = 0;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(!m_ppTracks[i] && !m_ppTracks[i]->IsInUsed())
			continue;

		dwTrackDuration = m_ppTracks[i]->GetDuration();

		VO_U32 rc = ((CMp4Track2*)m_ppTracks[i])->GetMediaTimeByFilePos(pParam->llFilePos, &llTmp, (pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START) ? VO_TRUE : VO_FALSE);
		//if reach this file position, the track is end
		//we will use the file duration value
		if(VO_ERR_SOURCE_OK != rc || llTmp >= dwTrackDuration)
			llTmp =VO_S64 (m_pMovie->GetDuration() * TIME_UNIT);

		//we will get the min value of the tracks
		if(llMediaTime > llTmp)
			llMediaTime = llTmp;
	}

	pParam->llMediaTime = llMediaTime;

	return VO_RET_SOURCE2_NOIMPLEMENT;//VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Reader2::GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	if(!pParam)
		return VO_ERR_INVALID_ARG;

	VO_S64 llFilePos = (pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START) ? VO_MAXU32 : 0;
	VO_S64 llTmp = (pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START) ? VO_MAXU32 : 0;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(!m_ppTracks[i] && !m_ppTracks[i]->IsInUsed())
			continue;

		((CMp4Track2*)m_ppTracks[i])->GetFilePosByMediaTime(pParam->llMediaTime, &llTmp, (pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START) ? VO_TRUE : VO_FALSE);
		if(pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START)
		{
			if(llFilePos > llTmp)
				llFilePos = llTmp;
		}
		else
		{
			if(llFilePos < llTmp)
				llFilePos = llTmp;
		}
	}

	pParam->llFilePos = llFilePos;

	return VO_ERR_SOURCE_OK;
}

#ifdef _SUPPORT_PARSER_METADATA
VO_U32 CMp4Reader2::InitMetaDataParser()
{
	m_pMetaDataParser = new CMP4MetaData(m_pFileOp, m_pMemOp);
	if(!m_pMetaDataParser)
		return VO_ERR_OUTOF_MEMORY;

	MetaDataInitInfo initInfo;
	initInfo.eType = VO_METADATA_iTunes;
	initInfo.ullFilePositions[0] = VO_MAXU64;
	initInfo.ullFilePositions[1] = VO_MAXU64;
	initInfo.ullFileSize = 0;
	initInfo.dwReserved = 0;
	initInfo.hFile = m_pStream->GetFileHandle();

	return m_pMetaDataParser->Load(&initInfo);
}

#endif //_SUPPORT_PARSER_METADATA

VO_U32 CMp4Reader2::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 nRC = MP4RRC_OK;
	switch ( uID )
	{
	case VO_PID_SOURCE_GETSEEKTIMEBYPOS:
		GetSeekTimeByPos((VO_FILE_MEDIATIMEFILEPOS*)pParam);
		break;
	case VO_PID_FILE_HEADDATAINFO:
		{
			headerdata_info * ptr_info = (headerdata_info *)pParam;

			if( ptr_info->ptr_array )
			{
				ptr_info->ptr_array[0].physical_pos = m_pMovie->GetHeaderPos();
				ptr_info->ptr_array[0].size = m_pMovie->GetHeaderSize();
			}

			ptr_info->arraysize = 1;
		}
		break;

	case VO_PID_FILE_GETAVSTARTPOS: //Jason 10/21/2010
		{
			av_startpos* avsp = (av_startpos*) pParam;
			avsp->video_startpos = avsp->audio_startpos = -1;
			for( VO_U32 i = 0 ; i < m_nTracks ; i++ )
			{
				CMp4Track2* track = (CMp4Track2*) m_ppTracks[i];
				if( track && track->IsInUsed() )
				{
					if (track->GetType() == VOTT_VIDEO)
						avsp->video_startpos = track->GetDataStartPos();
					else if (track->GetType() == VOTT_AUDIO)
						avsp->audio_startpos = track->GetDataStartPos();
				}
			}
		}
		break;

	case VO_PID_FILE_GETVIDEOMATRIX: //Jason 4/21/2011
		{
			video_matrix* vm = (video_matrix*) pParam;
			memset(vm, 0, sizeof(video_matrix));
			m_pMovie->GetMatrix(vm->global);
			for( VO_U32 i = 0 ; i < m_nTracks ; i++ )
			{
				CMp4Track2* track = (CMp4Track2*) m_ppTracks[i];
				if( track && track->IsInUsed() )
				{
					if (track->GetType() == VOTT_VIDEO)
						track->GetMatrix(vm->track);
				}
			}
			int c1 = vm->global[3] >> 16;
			int c2 = vm->track[3] >> 16;
			int c3 = vm->global[4] >> 16;
			int c4 = vm->track[4] >> 16;

			if((c1 | c2) == 1)
				vm->rotation = 270;
			else if((c1 | c2) == -1)
				vm->rotation = 90;
			else if((c3 | c4) == -1)
				vm->rotation = 180;
			else
				vm->rotation = 0;
		}
		break;
	case VO_PID_SOURCE_DATE:
		{
			char * date = (char*)pParam;
			char *time = m_pMovie->GetCreateTime();
			int size = strlen(time);
			memcpy(date, time,size);
			date[strlen(time)] = '\0';
			//VOLOGR("GetSourceDate: %s",date);
		}
		break;
	case VO_PID_SOURCE_NUM_TRACKS:
		{
			int *num = (int*)pParam;
			*num = m_pMovie->GetTrackCount();
			//VOLOGR("GetTrackNum: %d", *num);
		}
		break;
	case VO_PID_SOURCE_LOCATION:
		{
			char * gps = (char*)pParam;
			char *gg =m_pMovie->GetGPS();
			memcpy(gps, gg, strlen(gg));
			gps[strlen(gg)] = '\0';
		}
		break;
	case VO_PID_SOURCE_MINORVERSION:
		{
			VO_U32 * ptr = (VO_U32*)pParam;
			*ptr =m_pMovie->GetMinorVersion();
		}
		break;
	case VO_PID_SOURCE_LGE_VIDEO_CONTENT:
		{
			VO_U32 * ptr = (VO_U32*)pParam;
			*ptr =m_pMovie->GetEVideoContent();
		}
		break;
	default:
		return CBaseReader::GetParameter( uID , pParam );
	}
	return nRC;
}

VO_U32 CMp4Reader2::GetSeekPosByTime(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	VO_S64 mediatime = pParam->llMediaTime;

	for( VO_U32 i = 0 ; i < m_nTracks ; i++ )
	{
		if( m_ppTracks[i] && m_ppTracks[i]->IsInUsed() )
		{
			VO_S64 time = mediatime;
			if( 0 == m_ppTracks[i]->SetPos( &time ) )
			{
				if( time < mediatime )
					mediatime = time;
			}
		}
	}

	pParam->llMediaTime = mediatime;
	return GetFilePosByMediaTime( pParam );
}

VO_U32 CMp4Reader2::GetSeekTimeByPos(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	VO_S64 mediatime = 0x7fffffffffffffffll;

	for( VO_U32 i = 0 ; i < m_nTracks ; i++ )
	{
		if( m_ppTracks[i] && m_ppTracks[i]->IsInUsed() )
		{
			VO_S64 time =0;
			time = m_ppTracks[i]->GetRealDurationByFilePos(pParam->llFilePos );
			if( time < mediatime )
					mediatime = time;
		}
	}

	pParam->llMediaTime = mediatime;
	return VO_ERR_NONE;
}

VO_U32 CMp4Reader2::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	if(uID == VO_PID_SOURCE_SECOND_IO)
	{
		VO_FILE_SOURCE* pFileSource = (VO_FILE_SOURCE*)pParam;
		if (pFileSource == NULL)
		{
			return VO_ERR_NOT_IMPLEMENT;
		}
		///<new a IO for reading data
		m_pStreamPushPlay = new CMp4FileStream(m_pFileOp);

		if(!m_pStreamPushPlay)
			return VO_ERR_OUTOF_MEMORY;

		if(!m_pStreamPushPlay->Open(pFileSource))
			return VO_ERR_SOURCE_OPENFAIL;

		m_pReaderPushPlay = new ReaderMSB(m_pStreamPushPlay, m_ullFileSize);
		return VO_ERR_SOURCE_OK;
	}
	return CBaseReader::SetParameter(uID,pParam);
}