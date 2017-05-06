
#include "CMp4PushReader2.h"
#ifdef _SUPPORT_PARSER_METADATA
#include "CMP4MetaData.h"
#endif	//_SUPPORT_PARSER_METADATA

#include "voLog.h"
#include "voLiveSource.h"
#include "fragmentTrack.h"
#include "voDRM2.h"
#include "isomscan.h"

#ifdef MULITI_TRACK

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const double TIME_UNIT = 1.0;	//all time multiply this value is MS
#define ListNumber  10
const VO_U32 ONEBLOCKSIZE = 1024*20;

CMp4PushReader2::CMp4PushReader2()
	: CBaseReader(NULL, NULL, NULL, NULL)
	, m_pReader(VO_NULL)
	, m_pStream(VO_NULL)
	, m_pMovie(VO_NULL)
	,m_pPlayready_Drmcallback(NULL)
	,m_trackInfo(NULL)
	,m_nDefaultSize_Video(0)
	,m_nDefaultDuration_Video(0)
	,m_nDefaultSize_Audio(0)
	,m_nDefaultDuration_Audio(0)
	,m_u32TimeScale(1)
	,m_pBuf_x(NULL)
	,m_bBufNew(VO_FALSE)
	,m_bExit(VO_FALSE)
	,m_pSidxinfo(NULL)
	,m_pCencDrmCallbackProc(NULL)
	,m_u64PreChunkEndtime(0)
{
	memset(&m_InitDataStructure,0,sizeof(INITDATASTRUCTURE));
	m_InitDataStructure.pDataBuffer = new VO_BYTE[ONEBLOCKSIZE + 1];
	m_InitDataStructure.uBuffersize = ONEBLOCKSIZE + 1;
	memset(m_InitDataStructure.pDataBuffer,0,ONEBLOCKSIZE + 1);
	m_InitDataStructure.uUsedBuffersize = 0;

	memset(&m_InitHeadDataInfo,0,sizeof(VODS_INITDATA_INFO));
}

CMp4PushReader2::~CMp4PushReader2()
{
	if(m_pPlayready_Drmcallback ) delete m_pPlayready_Drmcallback;
	m_pPlayready_Drmcallback = NULL;
	FreeSegmentIndexInfo();
	Close();
	if(0 < m_InitDataStructure.uBuffersize)
		delete []m_InitDataStructure.pDataBuffer;
	VOLOGUNINIT();
}

VO_U32 CMp4PushReader2::Open(PARSERPROC pProc, VO_VOID* pUserData,VO_TCHAR* pWorkingPath)
{
	m_pProc		= pProc;
	m_pUserData	= pUserData;
	VOLOGINIT(pWorkingPath);
	return VO_ERR_NONE;
}
VO_U32 CMp4PushReader2::CreateStream(VO_U32 nFlag,VO_PBYTE pBuf, VO_U32 nSize)
{	
	m_pBuf_x = pBuf;
	m_bBufNew = VO_FALSE;
	VO_U32 size = nSize;
	if(nFlag == VO_PARSER_FLAG_STREAM_DASHHEAD)
	{

		if(pBuf[4] == 'f'&&pBuf[5] == 't'&&pBuf[6] == 'y'&&pBuf[7] == 'p')
		{	
			VO_U32 ftypSize = pBuf[0]<<24 | pBuf[1]<<16 | pBuf[2]<<9 |pBuf[3];
			if(ftypSize >24)
			{
				pBuf[20] = 'd';
				pBuf[21] = 'a';
				pBuf[22] = 's';
				pBuf[23] = 'h';
			}
			else
			{
				pBuf[8] = 'd';
				pBuf[9] = 'a';
				pBuf[10] = 's';
				pBuf[11] = 'h';
			}
		}
		else
		{
			//for Hamonic, no ftyp box
			size +=16;
			m_pBuf_x = new VO_BYTE[size+1];
			m_pBuf_x[0] = 0x00;
			m_pBuf_x[1] = 0x00;
			m_pBuf_x[2] = 0x00;
			m_pBuf_x[3] = 0x10;
			m_pBuf_x[4] = 0x66;
			m_pBuf_x[5] = 0x74;
			m_pBuf_x[6] = 0x79;
			m_pBuf_x[7] = 0x70;
			m_pBuf_x[8] = 0x64;
			m_pBuf_x[9] = 0x61;
			m_pBuf_x[10] = 0x73;
			m_pBuf_x[11] = 0x68;
			m_pBuf_x[12] = 0x00;
			m_pBuf_x[13] = 0x00;
			m_pBuf_x[14] = 0x00;
			m_pBuf_x[15] = 0x00;
			memcpy(m_pBuf_x+ 16, pBuf,nSize);
			m_bBufNew =VO_TRUE;
		}
	}

	m_pStream = new CMp4PushFileStream(NULL);
	if(!m_pStream)
		return VO_ERR_OUTOF_MEMORY;
	if(!m_pStream->Open(m_pBuf_x, size))
		return VO_ERR_SOURCE_OPENFAIL;
	m_ullFileSize = m_pStream->Size();
	m_pReader = new ReaderMSB(m_pStream, m_ullFileSize);
	return VO_ERR_SOURCE_OK;
}
VO_U32 CMp4PushReader2::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	m_u32TimeScale = 0;
	m_u64Starttime = 0;
	m_u64Starttime = *(VO_S64*)pBuffer->pReserved;
	
	VO_U32 rc = VO_ERR_SOURCE_OK;
#ifdef LIEYUN_DEBUG
	TestDashIf();
#endif
	if(pBuffer->nFlag == VO_PARSER_FLAG_STREAM_DASHINITDATA)//dash-if mode,so need to parse the initial data like PD first
	{
		if(!ProcessDashIfInitdata(pBuffer))
		{
			//DASHIF_INITDATASTATUS_TYPE uRetrunValue = DASHIF_NEEDMOREDATA;
			//IssueParseResult(VO_PARSER_OT_INITDATA_ISENOUGH, NULL , &uRetrunValue);
			return VO_RET_SOURCE2_INPUTDATASMALL;
		}
		else
		{
			//DASHIF_INITDATASTATUS_TYPE uRetrunValue = DASHIF_DATAENOUGH;
			//IssueParseResult(VO_PARSER_OT_INITDATA_ISENOUGH, NULL , &uRetrunValue);
			ParserDashIfInitalHead(m_InitDataStructure.pDataBuffer,m_InitDataStructure.uUsedBuffersize);
			ParserDashIfSidx(m_InitDataStructure.pDataBuffer,m_InitDataStructure.uUsedBuffersize);
			return VO_RET_SOURCE2_OK;
		}
		

	}
	else
	{
		CreateStream(pBuffer->nFlag, pBuffer->pBuf, pBuffer->nBufLen);

		
		if(pBuffer->nFlag == VO_PARSER_FLAG_STREAM_DASHHEAD)
		{
			VOLOGR("+Parser Head.")
			rc = ParserHead(pBuffer->pBuf,pBuffer->nBufLen);	
			VOLOGR("-Parser Head.")
		}
		else if(pBuffer->nFlag == VO_PARSER_FLAG_STREAM_DASHDATA)
		{
			VOLOGR("+Parser data.")
			rc = ParserData(pBuffer->pBuf,pBuffer->nBufLen);
			VOLOGR("-Parser data.")
		}
		else if(pBuffer->nFlag == VO_PARSER_FLAG_STREAM_DASHINDEX)
		{
			VOLOGR("+Parser sidx data.")
			rc = ParserSidx(pBuffer->pBuf,pBuffer->nBufLen);
			VOLOGR("-Parser sidx data.")
		}
	}
	CloseData();
	return rc;
}

CBaseTrack* CMp4PushReader2::GetTrackByIndex(VO_U32 nIndex)
{
	CBaseTrack* track = CBaseReader::GetTrackByIndex(nIndex);
	if(track && track->IsInUsed())
		return track;
	
	return VO_NULL;
}

VO_U32 CMp4PushReader2::ParserData(VO_PBYTE pBuf,VO_U64 bufsize)
{
	VOLOGR("+ParserData.Track Count:%d,Buffer:%p,Size:%lld",m_nTracks,pBuf,bufsize);

	VO_U32 rc = VO_ERR_SOURCE_OK;
	TrackOp     trackOp[ListNumber];
	SegmentsBox sb[ListNumber];
	VO_U32		nType[ListNumber];
	VO_U32		nTimeScale[ListNumber];
	VO_U64 nDataOffset = 0;

	for(int i =0;i< m_nTracks;i++)
	{

		sb[i].SetTrackPos(i);
		if(sb[i].Init(m_pReader, -2) != MP4RRC_OK)
		{
			VOLOGI("Failed to initial the segment and begin to dump the input data");
			FILE *fp = fopen("/sdcard/Huludrmdata.dump","wb");
			if(fp)
			{
				fwrite(pBuf,1,bufsize,fp);
				fclose(fp);
			}
			return VO_ERR_SOURCE_OPENFAIL;
		}
		
		nTimeScale[i] = sb[i].GetTimeScale();
		for(int n =0;n < m_nTracks;n ++)
		{
			CMp4Track2 *pTrack = (CMp4Track2*)m_ppTracks[n];
			if(!pTrack)
			{
				VOLOGI("Track handle is NULL,track number: %d",n);
				return VO_ERR_SOURCE_OPENFAIL;
			}
			if( pTrack->GetTrackId() == sb[i].GetTrackID() )
			{
				VO_TRACKTYPE ttype =  pTrack->GetType();
#ifndef LIEYUN_DEBUG
				nType[i] = (ttype == VOTT_AUDIO?VO_PARSER_OT_AUDIO :(ttype == VOTT_VIDEO? VO_PARSER_OT_VIDEO : (ttype == VOTT_TEXT?VO_PARSER_OT_TEXT:0X7FFFFFFF)));
#else
				nType[i] = (ttype == VOTT_AUDIO?VO_PARSER_OT_AUDIO :(ttype == VOTT_VIDEO? VO_PARSER_OT_VIDEO : 0X7FFFFFFF));
#endif
				//TrackSegment *pSegTrack = (TrackSegment *)m_ppTracks[n];
				sb[i].SetCencDrmFlag(pTrack->IsCencDrm());
				break;
			}
		}
		uint32 nDefaultDuration = m_pMovie->GetDefaultDuration( sb[i].GetTrackID());
		uint32 nDefaultSize = m_pMovie->GetDefaultSize( sb[i].GetTrackID() );
		if(nType[i] == VO_PARSER_OT_AUDIO)
		{
#ifdef LIEYUN_DEBUG
			if(m_u64PreChunkEndtime > m_u64Starttime)
				trackOp[i].Init(m_pReader, m_pStream, &sb[i],
								m_u64PreChunkEndtime, nDefaultDuration, nDefaultSize, nDataOffset);
			else
#endif
				trackOp[i].Init(m_pReader, m_pStream, &sb[i],
								m_u64Starttime, nDefaultDuration, nDefaultSize, nDataOffset);
			if(m_pCencDrmCallbackProc)
				trackOp[i].SetStreamDrmCallback(m_pCencDrmCallbackProc);
		}
		else if(nType[i] == VO_PARSER_OT_VIDEO)
		{
#ifdef LIEYUN_DEBUG
			if(m_u64PreChunkEndtime > m_u64Starttime)
				trackOp[i].Init(m_pReader, m_pStream, &sb[i],
								m_u64PreChunkEndtime, nDefaultDuration, nDefaultSize, nDataOffset);
			else
#endif
				trackOp[i].Init(m_pReader, m_pStream, &sb[i],
								m_u64Starttime, nDefaultDuration, nDefaultSize, nDataOffset);
			if(m_pCencDrmCallbackProc)
				trackOp[i].SetStreamDrmCallback(m_pCencDrmCallbackProc);
		}
#ifndef LIEYUN_DEBUG
		else if(nType[i] == VO_PARSER_OT_TEXT)
		{
			trackOp[i].Init(m_pReader, m_pStream, &sb[i],
							m_u64Starttime, nDefaultDuration, nDefaultSize, nDataOffset);
			if(m_pCencDrmCallbackProc)
				trackOp[i].SetStreamDrmCallback(m_pCencDrmCallbackProc);
		}
#endif
		nDataOffset = sb[i].GetAllSampleSize();
	}

	VO_SOURCE_SAMPLE sample;
	memset(&sample, 0x00, sizeof(VO_SOURCE_SAMPLE));
	VO_BOOL bEos = VO_FALSE;
	VO_U64	u64TempSampletime = 0;
	VO_U64  u64AudioChunkduration = 0;
	VO_U32  u32AudioSampleNumPerChunk = 0;
	VO_U64  u64VideoChunkduration = 0;
	VO_U32  u32VideoSampleNumPerChunk = 0;
	while(!m_bExit)
	{
		bEos = VO_TRUE;
		for(int i =0;i< m_nTracks;i++)
		{
			if( m_bExit )
				rc = VO_ERR_SOURCE_OK;
			if(!trackOp[i].IsEOS())
				bEos = VO_FALSE;
			else
				continue;

			if(trackOp[i].GetSample(nType[i], &sample) == VO_ERR_SOURCE_OK)
			{
#ifdef LIEYUN_DEBUG
				if(nType[i] == VO_PARSER_OT_VIDEO)
				{
					u64VideoChunkduration += sample.Duration;
					u32VideoSampleNumPerChunk += 1;
					VOLOGR("PushParser: video sample index: %d, video sample time: %lld",u32VideoSampleNumPerChunk,sample.Time);
				}
				else if(nType[i] == VO_PARSER_OT_AUDIO)
				{
					u64AudioChunkduration += sample.Duration;
					u32AudioSampleNumPerChunk += 1;
					VOLOGR("PushParser: audio sample index: %d, audio sample time: %lld",u32AudioSampleNumPerChunk,sample.Time);
				}
#endif
				VOLOGR("PushParser:ParserData TrackID:%d,Post out of Sample[%d],Time:%lld,Size:%x:TrackID==%d",i,nType[i],sample.Time, sample.Size,sb[i].GetTrackID());
				IssueParseResult(nType[i], sb[i].GetTrackID(), &sample, &nTimeScale[i]);	
			}

		}
		if(bEos == VO_TRUE)
			break;
	}
#ifndef LIEYUN_DEBUG
	 m_u64PreChunkEndtime = u64TempSampletime;
#endif
#ifdef LIEYUN_DEBUG
	 VOLOGR("PushParser: Video chunk duration: %d,video sample number per-chunk: %d,audio chunk duration: %d,audio sample number per-chunk: %d",(VO_U32)u64VideoChunkduration,u32VideoSampleNumPerChunk,(VO_U32)u64AudioChunkduration, u32AudioSampleNumPerChunk);
#endif
	 VOLOGR("-ParserData");
#ifdef H265_DUMPTIME
	//fclose(fpdumptime);
#endif
	return rc;
}
VO_U32  CMp4PushReader2::FreeSegmentIndexInfo()
{
	if(!m_pSidxinfo)
		return VO_ERR_SOURCE_OK;

	if(0 >= m_pSidxinfo->uCount)
		return VO_ERR_SOURCE_OK;

	delete []m_pSidxinfo->pSegInfo;
	delete m_pSidxinfo;
	m_pSidxinfo = NULL;
	
}
VO_BOOL  CMp4PushReader2::ProcessDashIfInitdata(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	Initialdatastream(pBuffer->pBuf,pBuffer->nBufLen);
	if(IsInitdataEnough(m_pReader,m_InitDataStructure.uUsedBuffersize))
		return VO_TRUE;
	else
		return VO_FALSE;

}
#ifndef LIEYUN_DEBUG
VO_VOID TestFun(void *pNum)
{
	//DASHIF_INITDATASTATUS_TYPE *pType = (DASHIF_INITDATASTATUS_TYPE *)pNum;
	//if(DASHIF_NEEDMOREDATA == *pType)
	//	int value = *pType;
}
VO_BOOL CMp4PushReader2::TestDashIf()
{
	//DASHIF_INITDATASTATUS_TYPE numvalue = DASHIF_NEEDMOREDATA;
	//TestFun(&numvalue);
	const char * pFilepath = "D:\\Downloads\\sony_multi_video\\SNE_DASH_SD_CASE1A_video_Track3.m4v"; //SNE_DASH_CASE4B_SD_audio_Track1.m4a; SNE_DASH_SD_CASE1A_video_Track3.m4v
	//const char * pFilepath = "D:\\Medias\\dash\\dash-if\\Netflix\\Netflix\\ElephantsDream_AAC48K_064.mp4.dash"; //SNE_DASH_CASE4B_SD_audio_Track1.m4a; SNE_DASH_SD_CASE1A_video_Track3.m4v							
	FILE *pfile = NULL;
	const int READLEN = 1024;
	VO_BYTE inbuffer[READLEN + 1];
	memset(inbuffer,0,READLEN+1);
	pfile = fopen(pFilepath,"rb");
	if(!pfile)
		return VO_FALSE;
	int readlen = 0;
	while(readlen = fread(inbuffer,1,READLEN,pfile))
	{
		Initialdatastream(inbuffer,readlen);
		if(IsInitdataEnough(m_pReader,m_InitDataStructure.uUsedBuffersize))
			break;
	}
	ParserDashIfInitalHead(m_InitDataStructure.pDataBuffer,m_InitDataStructure.uUsedBuffersize);
	ParserDashIfSidx(m_InitDataStructure.pDataBuffer,m_InitDataStructure.uUsedBuffersize);
	return VO_TRUE;
}
#endif

VO_BOOL CMp4PushReader2::IsInitdataEnough(Reader* r,VO_U32 uBuflen)
{
	Scanner scanner(r);
	r->SetPosition(0);
	uint64 moovsize = scanner.FindTag(FOURCC2_moov);
	if((0 == moovsize)||(r->Position() + moovsize > uBuflen))
		return VO_FALSE;
	uint64 si_size = 0;
	uint64 pr_si_size = si_size;
	ABSPOS pos = 0;
	while(si_size = scanner.FindTag(FOURCC2_sidx))
	{
		pos = r->Position();
		if(pos + si_size > uBuflen)
			return VO_FALSE;
		pr_si_size = si_size;
		r->Move(si_size);
		pos += si_size;
	}
	r->SetPosition(pos);
	si_size = scanner.FindTag(FOURCC2_ssix);
	if(si_size > 0)
	{
		if((si_size + r->Position() + 8) > uBuflen)
			return VO_FALSE;
		else
			return VO_TRUE;
	}
	else
	{
		if(pos + 8 > uBuflen)//size 8 equal tag size +box size parameter.
			return VO_FALSE;
		else
			return VO_TRUE;
	}

	return VO_TRUE;
}
VO_BOOL CMp4PushReader2::InitialDashHeard(INITDATASTRUCTURE &initdatastructure,VO_PBYTE pBuf,VO_U32 uBuflen)
{
	if(pBuf[4] == 'f'&&pBuf[5] == 't'&&pBuf[6] == 'y'&&pBuf[7] == 'p')
	{	
		VO_U32 ftypSize = pBuf[0]<<24 | pBuf[1]<<16 | pBuf[2]<<9 |pBuf[3];
		if(ftypSize >24)
		{
			pBuf[20] = 'd';
			pBuf[21] = 'a';
			pBuf[22] = 's';
			pBuf[23] = 'h';
		}
		else
		{
			pBuf[8] = 'd';
			pBuf[9] = 'a';
			pBuf[10] = 's';
			pBuf[11] = 'h';
		}
	}
	else
	{
		m_InitDataStructure.pDataBuffer[0] = 0x00;
		m_InitDataStructure.pDataBuffer[1] = 0x00;
		m_InitDataStructure.pDataBuffer[2] = 0x00;
		m_InitDataStructure.pDataBuffer[3] = 0x10;
		m_InitDataStructure.pDataBuffer[4] = 0x66;
		m_InitDataStructure.pDataBuffer[5] = 0x74;
		m_InitDataStructure.pDataBuffer[6] = 0x79;
		m_InitDataStructure.pDataBuffer[7] = 0x70;
		m_InitDataStructure.pDataBuffer[8] = 0x64;
		m_InitDataStructure.pDataBuffer[9] = 0x61;
		m_InitDataStructure.pDataBuffer[10] = 0x73;
		m_InitDataStructure.pDataBuffer[11] = 0x68;
		m_InitDataStructure.pDataBuffer[12] = 0x00;
		m_InitDataStructure.pDataBuffer[13] = 0x00;
		m_InitDataStructure.pDataBuffer[14] = 0x00;
		m_InitDataStructure.pDataBuffer[15] = 0x00;
		m_InitDataStructure.uUsedBuffersize += 16;
	}
	return VO_TRUE;
}
VO_BOOL CMp4PushReader2::Initialdatastream(VO_PBYTE pBuf,VO_U32 uBuflen)
{


	if((m_InitDataStructure.uUsedBuffersize + uBuflen) >= m_InitDataStructure.uBuffersize)
		ReAllocInitialDataBuffer();
	if(0 == m_InitDataStructure.uUsedBuffersize)
		InitialDashHeard(m_InitDataStructure,pBuf,uBuflen);
	memcpy(m_InitDataStructure.pDataBuffer + m_InitDataStructure.uUsedBuffersize,pBuf,uBuflen);
	m_InitDataStructure.uUsedBuffersize += uBuflen;
	UnInitialdatastream();
	m_pStream = new CMp4PushFileStream(NULL);
	if(!m_pStream)
		return VO_FALSE;
	if(!m_pStream->Open(m_InitDataStructure.pDataBuffer, m_InitDataStructure.uUsedBuffersize))
	{
		VOLOGI("SCMp4PushReader2::Initialdatastream,failed to Open stream");
		return VO_FALSE;
	}
	m_ullFileSize = m_pStream->Size();
	m_pReader = new ReaderMSB(m_pStream, m_ullFileSize);
	
	return VO_TRUE;
}

VO_BOOL CMp4PushReader2::UnInitialdatastream()
{
	SAFE_DELETE(m_pReader);
	SAFE_DELETE(m_pStream);
	return VO_TRUE;
}

VO_BOOL CMp4PushReader2::ReAllocInitialDataBuffer()
{
	int prebufferlen = m_InitDataStructure.uBuffersize;
	VO_BYTE *pNewBuffer = new VO_BYTE[prebufferlen + ONEBLOCKSIZE + 1];
	if(!pNewBuffer)
		return VO_FALSE;
	memset(pNewBuffer,0,(prebufferlen + ONEBLOCKSIZE + 1));
	if(m_InitDataStructure.pDataBuffer)
	{
		if(m_InitDataStructure.uUsedBuffersize > 0)
			memcpy(pNewBuffer,m_InitDataStructure.pDataBuffer,m_InitDataStructure.uUsedBuffersize);
		delete m_InitDataStructure.pDataBuffer;
		m_InitDataStructure.pDataBuffer = NULL;
	}
	m_InitDataStructure.pDataBuffer = pNewBuffer;
	m_InitDataStructure.uBuffersize = prebufferlen + ONEBLOCKSIZE + 1;
	return VO_TRUE;
}
VO_U32  CMp4PushReader2::ParserSidx(VO_PBYTE pBuf,VO_U64 bufsize)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	SidxBox sidx;
	sidx.InitSidx(m_pReader,bufsize);
	//sidx.Init(m_pReader->GetStream(),bufsize);

	FreeSegmentIndexInfo();
	VO_SegmentIndexStructure sidxstructure = sidx.GetSementIndexStructure();


	m_pSidxinfo = new VODS_SEG_INDEX_INFO;

	memset(m_pSidxinfo,0,sizeof(VODS_SEG_INDEX_INFO));
	m_pSidxinfo->pSegInfo = new VODS_SUBSEGMENT_INFO[sidxstructure.ref_count];
	memset(m_pSidxinfo->pSegInfo,0,sizeof(VODS_SUBSEGMENT_INFO)*sidxstructure.ref_count);
	m_pSidxinfo->uStreamID = sidxstructure.reference_ID;
	m_pSidxinfo->uCount = sidxstructure.ref_count;
	VO_BYTE INDEXBOXFLAG = 1;
	VO_U32 uoffset = 0;
	for(int ref_count = 0; ref_count <sidxstructure.ref_count; ref_count++)
	{
		m_pSidxinfo->pSegInfo[ref_count].bFirstKeyFrame = VO_TRUE;
		m_pSidxinfo->pSegInfo[ref_count].uDuration = sidxstructure.pSidxrefs[ref_count].nsubsegment_duration;
		m_pSidxinfo->pSegInfo[ref_count].uOffset = uoffset;
		m_pSidxinfo->pSegInfo[ref_count].uSegIndex = ref_count;
		if(INDEXBOXFLAG != sidxstructure.pSidxrefs[ref_count].breference_type)
			m_pSidxinfo->pSegInfo[ref_count].uType = VODS_SUBSEGMENT_TYPE_SEGMENT;
		else
			m_pSidxinfo->pSegInfo[ref_count].uType = VODS_SUBSEGMENT_TYPE_INDEX;
		m_pSidxinfo->pSegInfo[ref_count].uSize = sidxstructure.pSidxrefs[ref_count].nreference_size;
		m_pSidxinfo->pSegInfo[ref_count].uStartTime = sidxstructure.pSidxrefs[ref_count].nSAP_delta_time;
		m_pSidxinfo->pSegInfo[ref_count].uTimeScale = sidxstructure.timescale;
		uoffset += sidxstructure.pSidxrefs[ref_count].nreference_size;
		 
	}
	IssueParseResult(VO_PARSER_OT_SEGMENTINDEX_INFO, NULL , m_pSidxinfo);

	return rc;
}

VO_U32  CMp4PushReader2::ParserDashIfInitalHead(VO_PBYTE pBuf,VO_U64 bufsize)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	m_pReader->SetPosition(0);
	Scanner scanner(m_pReader);
	uint64 moovsize = scanner.FindTag(FOURCC2_moov);
	if(moovsize > 0)
	{
		memset(&m_InitHeadDataInfo,0,sizeof(VODS_INITDATA_INFO));
		m_InitHeadDataInfo.sHeader.uOffset = 0;
		m_InitHeadDataInfo.sHeader.uSize = m_pReader->Position() + moovsize;
		IssueParseResult(VO_PARSER_OT_INITDATA_INFO, NULL , &m_InitHeadDataInfo);
	}
	m_pReader->SetPosition(0);
	rc = ParserDashIfHead(pBuf,bufsize);
	return rc;
}

VO_U32  CMp4PushReader2::ParserDashIfSidx(VO_PBYTE pBuf,VO_U64 bufsize)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	SidxBox sidx;
	rc = sidx.InitSidx2(m_pReader,bufsize);	

	FreeSegmentIndexInfo();
	VO_SegmentIndexStructure sidxstructure = sidx.GetSementIndexStructure();
	m_pSidxinfo = new VODS_SEG_INDEX_INFO;
	memset(m_pSidxinfo,0,sizeof(VODS_SEG_INDEX_INFO));
	m_pSidxinfo->pSegInfo = new VODS_SUBSEGMENT_INFO[sidxstructure.ref_count];
	memset(m_pSidxinfo->pSegInfo,0,sizeof(VODS_SUBSEGMENT_INFO)*sidxstructure.ref_count);
	m_pSidxinfo->uStreamID = sidxstructure.reference_ID;
	m_pSidxinfo->uCount = sidxstructure.ref_count;
	VO_BYTE INDEXBOXFLAG = 1;
	for(int ref_count = 0; ref_count <sidxstructure.ref_count; ref_count++)
	{
		m_pSidxinfo->pSegInfo[ref_count].bFirstKeyFrame = VO_TRUE;
		m_pSidxinfo->pSegInfo[ref_count].uDuration = sidxstructure.pSidxrefs[ref_count].nsubsegment_duration;
		m_pSidxinfo->pSegInfo[ref_count].uOffset = sidxstructure.pSidxrefs[ref_count].nreference_data_offset;
		m_pSidxinfo->pSegInfo[ref_count].uSegIndex = ref_count;
		if(INDEXBOXFLAG != sidxstructure.pSidxrefs[ref_count].breference_type)
			m_pSidxinfo->pSegInfo[ref_count].uType = VODS_SUBSEGMENT_TYPE_SEGMENT;
		else
			m_pSidxinfo->pSegInfo[ref_count].uType = VODS_SUBSEGMENT_TYPE_INDEX;
		m_pSidxinfo->pSegInfo[ref_count].uSize = sidxstructure.pSidxrefs[ref_count].nreference_size;
		m_pSidxinfo->pSegInfo[ref_count].uStartTime = sidxstructure.pSidxrefs[ref_count].nSAP_delta_time;
		m_pSidxinfo->pSegInfo[ref_count].uTimeScale = sidxstructure.timescale;
		//uoffset += sidxstructure.pSidxrefs[ref_count].nreference_size;
		 
	}
	IssueParseResult(VO_PARSER_OT_SEGMENTINDEX_INFO, NULL , m_pSidxinfo);
	CloseData();
	return rc;
}
VO_U32	CMp4PushReader2::ParserDashIfHead(VO_PBYTE pBuf,VO_U64 bufsize)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	
	CloseHead();

	m_pMovie = new Movie;
#ifndef CENC
	if(m_pCencDrmCallbackProc)
		m_pMovie->SetCENCDrmCallback(m_pCencDrmCallbackProc);
#endif
	m_pMovie->Open(m_pReader,VO_TRUE);
	
	VO_U32 nFlag =0;
	rc = InitTracks(nFlag);

	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	for(int i=0; i< m_nTracks; i++)
	{
		CreateTrackInfo(i);

		CMp4Track2* pTrack = (CMp4Track2*)m_ppTracks[i];
		VOLOGR("PushParser:ParserHead pTrack==%d,TrackID==%d",pTrack,pTrack->GetTrackId());
		IssueParseResult(VO_PARSER_OT_TRACKINFO, pTrack == NULL ? 0 : pTrack->GetTrackId(), m_trackInfo, &m_u32TimeScale);

		SAFE_DELETE(m_trackInfo);
	}

	//CloseData();

	return rc;
}
VO_U32 CMp4PushReader2::ParserHead(VO_PBYTE pBuf,VO_U64 bufsize)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	
	CloseHead();

	m_pMovie = new Movie;
#ifndef CENC
	if(m_pCencDrmCallbackProc)
		m_pMovie->SetCENCDrmCallback(m_pCencDrmCallbackProc);
#endif

	m_pMovie->Open(m_pReader,VO_TRUE);
	
	VO_U32 nFlag =0;
	rc = InitTracks(nFlag);

	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	for(int i=0; i< m_nTracks; i++)
	{
		CreateTrackInfo(i);

		CMp4Track2* pTrack = (CMp4Track2*)m_ppTracks[i];
		VOLOGI("PushParser:ParserHead pTrack==%d,TrackID==%d",pTrack,pTrack->GetTrackId());
		IssueParseResult(VO_PARSER_OT_TRACKINFO, pTrack == NULL ? 0 : pTrack->GetTrackId(), m_trackInfo, &m_u32TimeScale);

		SAFE_DELETE(m_trackInfo);
	}

	CloseData();

	return rc;
}
VO_VOID	CMp4PushReader2::IssueParseResult(VO_U32 nType, VO_U64 ullTrackId, VO_VOID* pValue, VO_VOID* pReserved /*= NULL*/)
{	
	VOLOGR("+ IssueParserResult. Type:0x%08x", nType);
	VO_PARSER_OUTPUT_BUFFER output;
	output.nStreamID = ullTrackId;
	output.nType = nType;
	output.pOutputData = (VO_VOID*)pValue;
	output.pReserved = pReserved;
	output.pUserData = m_pUserData;

	m_pProc(&output);
	VOLOGR("- IssueParserResult. ");
}

VO_U32 CMp4PushReader2::CreateTrackInfo(int id)
{
	SAFE_DELETE(m_trackInfo);
	VO_CODECBUFFER codecbuffer;
	memset(&codecbuffer,0,sizeof(VO_CODECBUFFER));
	CMp4Track2* pNewTrack = (CMp4Track2*)m_ppTracks[id];
	pNewTrack->GetHeadData(&codecbuffer);
	m_u32TimeScale = VO_U32(pNewTrack->GetTimeScale());
	if(m_u32TimeScale == 0)
		m_u32TimeScale =1;

	m_trackInfo = ( VO_LIVESRC_TRACK_INFOEX * )new VO_BYTE[ sizeof( VO_LIVESRC_TRACK_INFOEX ) + codecbuffer.Length ];

	VO_LIVESRC_TRACK_INFOEX *trackinfoex = m_trackInfo;
	memset( trackinfoex , 0 , sizeof( VO_LIVESRC_TRACK_INFOEX ) );

	trackinfoex->Type = pNewTrack->GetType()== VOTT_AUDIO?VO_SOURCE_TT_AUDIO: (pNewTrack->GetType() == VOTT_VIDEO?VO_SOURCE_TT_VIDEO:(pNewTrack->GetType() ==VOTT_TEXT?VO_SOURCE_TT_SUBTITLE:VO_SOURCE_TT_MAX));
	pNewTrack->GetCodec(&trackinfoex->Codec);
	if(trackinfoex->Type == VO_SOURCE_TT_AUDIO)
	{
		pNewTrack->GetAudioFormat(&trackinfoex->audio_info);
	}
	else if(trackinfoex->Type == VO_SOURCE_TT_VIDEO)
	{
		pNewTrack->GetParameter(VO_PID_VIDEO_FORMAT, &trackinfoex->video_info);
	}
	else if(trackinfoex->Type == VO_SOURCE_TT_SUBTITLE)
	{
		return VO_ERR_SOURCE_OK;
	}
	trackinfoex->HeadData = trackinfoex->Padding;
	memcpy(trackinfoex->HeadData, codecbuffer.Buffer, codecbuffer.Length);
	trackinfoex->HeadSize = codecbuffer.Length;

	return VO_ERR_SOURCE_OK;
}
VO_U32 CMp4PushReader2::Close()
{
	m_bExit = VO_TRUE;
	CloseHead();
	CloseData();
	return VO_ERR_SOURCE_OK;
}
VO_U32 CMp4PushReader2::CloseData()
{
	VOLOGR("+CloseData");

	SAFE_DELETE(m_pReader);
	SAFE_DELETE(m_pStream);

	if(m_bBufNew)
	{
		delete []m_pBuf_x;
		m_pBuf_x = NULL;
		m_bBufNew = VO_FALSE;
	}
	VOLOGR("-CloseData");
	return VO_ERR_SOURCE_OK;
}
VO_U32 CMp4PushReader2::CloseHead()
{
	VOLOGR("+Close");
	UnprepareTracks();
	UninitTracks();

	VOLOGR("-1");
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		SAFE_DELETE(m_ppTracks[i]);
	}
	m_nTracks = 0;
	VOLOGR("-12");

	if(m_pMovie)
		m_pMovie->Close();

	SAFE_DELETE(m_pMovie);

	VOLOGR("-Close");
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4PushReader2::InitTracks(VO_U32 nSourceOpenFlags)
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
						CMp4Track2* pNewTrack = new CMp4Track2(ppTracks[i], m_pReader, m_pMemOp);
						pNewTrack->SetIsStreaming(VO_TRUE);
						pNewTrack->SetCencDrmFlag(ppTracks[i]->IsCencdrm());
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


VO_U32 CMp4PushReader2::SetParam(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 nRc = VO_ERR_NOT_IMPLEMENT;
   if(VO_DATASOURCE_PID_DRM_FP_CB == uID)
	{
		m_pCencDrmCallbackProc = (VO_DATASOURCE_FILEPARSER_DRMCB *)pParam;
	}
	return nRc;
}

VO_U32 CMp4PushReader2::GetParam(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 nRc = VO_ERR_NONE;
	return nRc;
}

TrackOp::TrackOp()
{
	m_pSegmentsBox = NULL;
	m_pReader = NULL;
	m_pStream = NULL;
	m_pBuffer = NULL;
	m_nCurSampleSize = 0;
	m_nCurSampleIndex = 0;
	m_pCencDrmCallbackProc = NULL;
	memset(&m_CencDrmSampleInfo,0,sizeof(CENCDRMSAMPLEINFO));
}
TrackOp::~TrackOp()
{
	SAFE_DELETE(m_pBuffer);
	if(m_CencDrmSampleInfo.uBuffersize)
	{
		delete []m_CencDrmSampleInfo.pDataBuffer;
		m_CencDrmSampleInfo.pDataBuffer = NULL;
	}
}
VO_U32 TrackOp::Init(ReaderMSB *pReader, CMp4PushFileStream* pStream, SegmentsBox *pSb,
					 VO_U64 nStartTime,VO_U64 nDefaultDuration, VO_U64 nDefaultSize, VO_U64 nOffset)
{
	m_pReader = pReader;
	m_pStream = pStream;
	m_pSegmentsBox = pSb;
	m_nDefaultDuration = nDefaultDuration;
	m_nDefaultSize = nDefaultSize;
	m_nCount = m_pSegmentsBox->GetSampleCount();
	m_nStartTime = nStartTime;
	m_nDataOffset = nOffset;
	m_pSegmentsBox->SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_SIZE, &m_nDefaultSize);
	m_pSegmentsBox->SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_DURATION, &m_nDefaultDuration);
	return VO_ERR_SOURCE_OK;
}
MP4RRC TrackOp::GetNextMoofInfo()
{
	MP4RRC rt = m_pSegmentsBox->GetFragmentInfo(-2);
	if(MP4RRC_OK != rt)
	{
		VOLOGI("TrackOp::GetNextMoofInfo,call m_pSegmentsBox->GetFragmentInfo(-2) failed");
		return rt;
	}
	m_nCount = m_pSegmentsBox->GetSampleCount();//set current fragment sample count.
	m_nCurSampleIndex = 0; //reset the sample index for the new fragment;
	m_pSegmentsBox->SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_SIZE, &m_nDefaultSize);
	m_pSegmentsBox->SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_DURATION, &m_nDefaultDuration);
	return MP4RRC_OK;
}
CENCDRMSAMPLEINFO * TrackOp::GetCencDrmInfo(VO_U32 index)
{
	uint32 offset = m_pSegmentsBox->m_SaizBox.GetOffsetByIndex(index);
	if(offset < 0)
	{
		VOLOGI("TrackOp::GetCencDrmInfo,call m_pSegmentsBox->m_SaizBox.GetOffsetByIndex(index) failed and index = %d",index);
		return NULL;
	}
	if(m_pSegmentsBox->m_SaioBox.m_enrycount = 1)
	{
		offset += m_pSegmentsBox->m_SaioBox.m_pOffsetArr[0]; 
	}
	if(m_CencDrmSampleInfo.uBuffersize > 0)
	{
		delete m_CencDrmSampleInfo.pDataBuffer;
		m_CencDrmSampleInfo.pDataBuffer = NULL;
	}
	m_CencDrmSampleInfo.uBuffersize = m_pSegmentsBox->m_SaizBox.m_pSamplesizeArr[index];
	m_CencDrmSampleInfo.pDataBuffer = new VO_BYTE[m_CencDrmSampleInfo.uBuffersize + 1];
	memset(m_CencDrmSampleInfo.pDataBuffer,0,(m_CencDrmSampleInfo.uBuffersize + 1));
	m_pReader->SetPosition(offset);
	m_pStream->Read(m_CencDrmSampleInfo.pDataBuffer,m_CencDrmSampleInfo.uBuffersize);
	return &m_CencDrmSampleInfo;
}
VO_BOOL TrackOp::IsEOS()
{
	if( m_nCount <= m_nCurSampleIndex)
	{
		if((MP4RRC_OK != GetNextMoofInfo())||(m_nCount <= m_nCurSampleIndex))
			return VO_TRUE;
		else 
			return VO_FALSE;
	}
	else
		return VO_FALSE;
}
VO_U32 TrackOp::GetSample(VO_U32 nType, VO_SOURCE_SAMPLE *pSample)
{
	if(!(m_pSegmentsBox && m_pReader && m_pStream))
		return VO_ERR_FAILED;

	if( m_nCount <= m_nCurSampleIndex)
		return VO_ERR_SOURCE_END;

	ABSPOS pos = m_pSegmentsBox->GetSampleAddress(m_nCurSampleIndex) + m_nDataOffset;
	uint32 nSize = m_pSegmentsBox->GetSampleSize(m_nCurSampleIndex);
	uint64 timeScale = m_pSegmentsBox->GetTimeScale();

	if(m_nCurSampleSize < nSize + 1024) //FOR drm so add 1024
	{
		SAFE_DELETE(m_pBuffer);
		
		m_nCurSampleSize = nSize;
#ifndef USEOLD
		if(m_pSegmentsBox->m_bIsCencDrm)
		{
			m_pBuffer = new unsigned char[nSize + 1024];
			memset(m_pBuffer,0,(nSize + 1024));
		}
		else
		{
			m_pBuffer = new unsigned char[m_nCurSampleSize];
			memset(m_pBuffer,0,m_nCurSampleSize);
		}
#else
		if(m_pSegmentsBox->m_pSencInfo)
		{
			m_pBuffer = new unsigned char[nSize + 1024];
			memset(m_pBuffer,0,(nSize + 1024));
		}
		else
		{
			m_pBuffer = new unsigned char[m_nCurSampleSize];
			memset(m_pBuffer,0,m_nCurSampleSize);
		}
#endif
	}

	m_pReader->SetPosition(pos);
	m_pStream->Read(m_pBuffer, nSize);
#ifndef CENC
#ifndef USEOLD
	if(m_pSegmentsBox->m_bIsCencDrm)
	{
		CENCDRMSAMPLEINFO *tmpCencDrmInfo = GetCencDrmInfo(m_nCurSampleIndex);
		if(NULL == tmpCencDrmInfo)
			return VO_ERR_SOURCE_ERRORDATA;
		if(tmpCencDrmInfo->uBuffersize > 0)
		{
			VO_DRM2_SAMPLEDECYPTION_INFO sampleinfo;
			sampleinfo.pSampleEncryptionBox = tmpCencDrmInfo->pDataBuffer;
			sampleinfo.uSampleEncryptionBoxSize = tmpCencDrmInfo->uBuffersize;
			sampleinfo.uSampleIndex = m_nCurSampleIndex;
			sampleinfo.uTrackId = m_pSegmentsBox->GetTrackID();
			if(m_pCencDrmCallbackProc)
			{
				VO_U32 uReturnValue = nSize + 1024;
				m_pCencDrmCallbackProc->FRCB(m_pCencDrmCallbackProc->pUserData,(VO_PBYTE)m_pBuffer,nSize,(VO_PBYTE *)m_pBuffer,&uReturnValue,VO_DRM2DATATYPE_MEDIASAMPLE,VO_DRM2_DATAINFO_SINGLE_SAMPLEDECYPTION,&sampleinfo);
				nSize = uReturnValue;
			}
		}
	}
#else
	if(m_pSegmentsBox->m_pSencInfo)
	{
		if(m_pSegmentsBox->m_pSencInfo->size > 0)
		{
			VO_DRM2_SAMPLEDECYPTION_INFO sampleinfo;
			sampleinfo.pSampleEncryptionBox = m_pSegmentsBox->m_pSencInfo->pSenc;
			sampleinfo.uSampleEncryptionBoxSize = m_pSegmentsBox->m_pSencInfo->size;
			sampleinfo.uSampleIndex = m_nCurSampleIndex;
			sampleinfo.uTrackId = m_pSegmentsBox->GetTrackID();
			if(m_pCencDrmCallbackProc)
			{
				VO_U32 uReturnValue = nSize + 1024;
				m_pCencDrmCallbackProc->FRCB(m_pCencDrmCallbackProc->pUserData,(VO_PBYTE)m_pBuffer,nSize,(VO_PBYTE *)m_pBuffer,&uReturnValue,VO_DRM2DATATYPE_MEDIASAMPLE,VO_DRM2_DATAINFO_SINGLE_SAMPLEDECYPTION,&sampleinfo);
				nSize = uReturnValue;
			}
		}
	}
#endif
#endif

	if(nType == VO_PARSER_OT_AUDIO && m_pSegmentsBox->IsNeedGather21Chunk())
	{
		pSample->Buffer = m_pBuffer;
		pSample->Size = nSize;
		pSample->Time = m_pSegmentsBox->m_nBaseMediaDecodetime;
	}
	else
	{
		pSample->Buffer = m_pBuffer;
		pSample->Size = nSize;
		pSample->Time = m_pSegmentsBox->GetSampleTime2(m_nCurSampleIndex, 1); 
		pSample->Duration = (VO_U32)m_pSegmentsBox->GetDurationByIndex(m_nCurSampleIndex);
		m_pSegmentsBox->GetDurationByIndex(m_nCurSampleIndex);
		if(m_pSegmentsBox->m_nBaseMediaDecodetime == 0)
			pSample->Time += m_nStartTime;
	}
	VOLOGI("TrackOp::GetSample sample time: %d,sample duration: %d,sample size: %d",(int)pSample->Time,(int)pSample->Duration,(int)pSample->Size);
	if(nType == VO_PARSER_OT_VIDEO && m_nCurSampleIndex == 0)
		pSample->Size |= 0x80000000;

	m_nCurSampleIndex ++;

	return VO_ERR_SOURCE_OK;
}

#endif