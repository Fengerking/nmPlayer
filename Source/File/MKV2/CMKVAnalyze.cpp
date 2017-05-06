#include "CMKVAnalyze.h"
#include "MKVID.h"
#include "EBML.h"
#include "MKVInfoStructureFunc.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "cmnFile.h"


NotifyCenter::NotifyCenter()
{
	memset(&m_pNotify, 0x0, sizeof(m_pNotify));

}

NotifyCenter::~NotifyCenter()
{
	;
}

VO_VOID NotifyCenter::Reset()
{
	memset(&m_pNotify, 0x0, sizeof(m_pNotify));
}

VO_BOOL NotifyCenter::RegistNotify(NotifyElement ElementType,VO_PTR pOnNotifyUser, pOnNotify OnNotify)
{
	if(ElementType >= ELEMENT_MAX){
		return VO_FALSE;
	}

	m_pNotify[ElementType].OnNotify = OnNotify;
	m_pNotify[ElementType].pOnNotifyUser = pOnNotifyUser;

	return VO_TRUE;
}

VO_BOOL NotifyCenter::IsNotifyRegist(NotifyElement ElementType)
{
	if(ElementType >= ELEMENT_MAX){
		return VO_FALSE;
	}

	if(m_pNotify[ElementType].OnNotify && m_pNotify[ElementType].pOnNotifyUser){
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_VOID NotifyCenter::EventNotify(NotifyElement element, VO_PTR wParam , VO_PTR lParam)
{
	if(element >= ELEMENT_MAX){
		return;
	}

	if(m_pNotify[element].OnNotify){
		m_pNotify[element].OnNotify(m_pNotify[element].pOnNotifyUser, wParam , lParam);
	}
}

VO_BOOL NotifyCenter::CancelNotify(NotifyElement ElementType)
{
	if(ElementType >= ELEMENT_MAX){
		return VO_FALSE;
	}

	memset(&m_pNotify[ElementType], 0x0, sizeof(m_pNotify[ElementType]));
	return VO_TRUE;
}


SampleNotifyCenter::SampleNotifyCenter()
{
	memset(&m_pSampleNotify[0], 0x0, sizeof(m_pSampleNotify[0]) * MAX_TRACK_NUMBER);
}
SampleNotifyCenter::~SampleNotifyCenter()
{
	;
}

VO_VOID SampleNotifyCenter::Reset()
{
	memset(&m_pSampleNotify[0], 0x0, sizeof(m_pSampleNotify[0]) * MAX_TRACK_NUMBER);
}

VO_BOOL SampleNotifyCenter::Notify(VO_U32 nTrackNumber, VO_PTR wParam , VO_PTR lParam)
{
	for(int i = 0; i < MAX_TRACK_NUMBER; i++){
		if(m_pSampleNotify[i].pOnNotifyUser == NULL){
			continue;
		}

		if(m_pSampleNotify[i].nTrackNumber == nTrackNumber){
			if (!m_pSampleNotify[i].OnNotify(m_pSampleNotify[i].pOnNotifyUser, wParam, lParam)){
				return VO_FALSE;
			}
			break;
		}
	}

	return VO_TRUE;
}

VO_BOOL SampleNotifyCenter::RegistSampleNotify(VO_U32 nTrackNumber,VO_PTR pOnNotifyUser, pOnNotify OnNotify)
{
	VO_BOOL result = VO_FALSE;

	for(int i = 0; i < MAX_TRACK_NUMBER; i++){
		if(m_pSampleNotify[i].pOnNotifyUser == NULL){
			m_pSampleNotify[i].nTrackNumber = nTrackNumber;
			m_pSampleNotify[i].pOnNotifyUser = pOnNotifyUser;
			m_pSampleNotify[i].OnNotify = OnNotify;
			
			result = VO_TRUE;
			break;
		}		
	}

	return result;
}

VO_BOOL SampleNotifyCenter::IsTrackRegist(VO_U32 nTrackNumber)
{
	VO_BOOL	result = VO_FALSE;
	
	for(int i = 0; i < MAX_TRACK_NUMBER; i++)
	{
		if(m_pSampleNotify[i].pOnNotifyUser == NULL){
			continue;
		}

		
		if(m_pSampleNotify[i].nTrackNumber == nTrackNumber){
			result = VO_TRUE;
			break;
		}
	}

	return result;
}

VO_BOOL SampleNotifyCenter::CancelSampleNotify(pOnNotify notify)
{
	VO_BOOL	result = VO_FALSE;
	
	for(int i = 0; i < MAX_TRACK_NUMBER; i++)
	{		
		if(m_pSampleNotify[i].pOnNotifyUser == notify){
			memset(&m_pSampleNotify[i], 0x0, sizeof(m_pSampleNotify[i]));
			result = VO_TRUE;
		}
	}

	return result;
}

VO_BOOL SampleNotifyCenter::CancelSampleNotify(VO_U32 nTrackNumber)
{
	VO_BOOL	result = VO_FALSE;
	
	for(int i = 0; i < MAX_TRACK_NUMBER; i++)
	{
		if(m_pSampleNotify[i].pOnNotifyUser == NULL){
			continue;
		}
		
		if(m_pSampleNotify[i].nTrackNumber == nTrackNumber){
			memset(&m_pSampleNotify[i], 0x0, sizeof(m_pSampleNotify[i]));
			result = VO_TRUE;
		}
	}

	return result;

}

Parser::Parser()
:m_nAnalyzeMode(0)
,m_bParseEnd(VO_FALSE)
,m_nCurrectClusterTime(-1)
,m_nStatus(IDLE_STATE)
{
	m_pSampleNotifyCenter = new SampleNotifyCenter;
}

Parser::Parser(Parser& parser)
{
	m_nAnalyzeMode	= parser.m_nAnalyzeMode;
	m_bParseEnd = parser.m_bParseEnd;
	m_nCurrectClusterTime = parser.m_nCurrectClusterTime;
	m_nStatus = parser.m_nStatus;

	m_pSampleNotifyCenter = new SampleNotifyCenter;
	
}


Parser::~Parser()
{
	if(m_pSampleNotifyCenter){
		delete m_pSampleNotifyCenter;
	}
}

VO_BOOL Parser::CopyParser(Parser* pParser)
{
	pParser->m_bParseEnd = m_bParseEnd;
	pParser->m_nAnalyzeMode = m_nAnalyzeMode;

	return VO_TRUE;
}

VO_VOID Parser::SetMode(VO_U64 mode)
{
	m_nAnalyzeMode =mode;
}

VO_BOOL Parser::SetFileIO( VO_PTR pFileIO)
{

	return VO_TRUE;
}

VO_PTR Parser::GetFileIO()
{
	return NULL;
}

VO_U32 Parser::DataParse()
{
	return 0;
}

VO_U32 Parser::MoveTo( VO_S64 llTimeStamp)
{
	return 0;
}

VO_VOID Parser::Start()
{
	SetStatus(READY_STATE);
}


VO_VOID Parser::Stop()
{
	SetStatus(IDLE_STATE);
}

VO_VOID Parser::Flush()
{
	m_nCurrectClusterTime = -1; 

	m_bParseEnd = VO_FALSE;
}

VO_U32 Parser::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	return 0;
}

VO_VOID Parser::AddSampleNotifyCallback(VO_U32 nTrackNumber,VO_PTR pOnNotifyUser, pOnNotify OnNotify)
{
	m_pSampleNotifyCenter->RegistSampleNotify(nTrackNumber, pOnNotifyUser, OnNotify);

}

VO_VOID Parser::RemoveSampleNotifyCallback(VO_U32 nTrackNumber)
{
	m_pSampleNotifyCenter->CancelSampleNotify(nTrackNumber);
}

PARSER_STATUS Parser::GetStatus()
{
	return m_nStatus;
}

VO_BOOL Parser::SetStatus(PARSER_STATUS nStatus)
{
	m_nStatus = nStatus;
	return VO_TRUE;
}


HeadParser::HeadParser()
{
	m_pNotifyCenter = new NotifyCenter;

}

HeadParser::~HeadParser()
{
	if(m_pNotifyCenter){
		delete m_pNotifyCenter;
	}
}

VO_VOID HeadParser::AnalyzeFile( VO_S64 filesize)
{
	;
}

VO_BOOL HeadParser::RegistNotify(NotifyElement ElementType,VO_PTR pOnNotifyUser, pOnNotify OnNotify)
{
	return m_pNotifyCenter->RegistNotify(ElementType,pOnNotifyUser, OnNotify);
}

VO_BOOL HeadParser::CancelNotify(NotifyElement ElementType)
{
	return m_pNotifyCenter->CancelNotify(ElementType);
}

CMKVAnalyze::CMKVAnalyze( CGFileChunk* m_pFileChunk)
//:CvoBaseMemOpr(pMemOp)
:m_pFileChunk( m_pFileChunk )
//,m_lastfilepos(0)
{
//	InitializeMKVInfo( &m_Info );
	m_nAnalyzeMode = PROCESS_ANALYZE_MODE;
}

CMKVAnalyze::CMKVAnalyze(CMKVAnalyze& pCMKVAnalyze )
:HeadParser()
,Parser(((Parser)pCMKVAnalyze))
{
	m_pFileChunk = new CGFileChunk(*(pCMKVAnalyze.m_pFileChunk));
	m_Info = pCMKVAnalyze.m_Info;	
}


CMKVAnalyze::~CMKVAnalyze()
{
	UnInitializeMKVInfo( &m_Info );
}


VO_BOOL CMKVAnalyze::GetSampleData(VO_PTR pFrame, VO_PBYTE pFrameData, VO_U32* pFrameLength)
{
	VOLOGI("GetSampleData");
	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::SetFileIO( VO_PTR pFileIO)
{
	if(!pFileIO)
		return VO_FALSE;

	VOLOGI("SetFileIO");
	m_pFileChunk = (CGFileChunk *)pFileIO;

	return VO_TRUE;
}

VO_PTR CMKVAnalyze::GetFileIO()
{
	return (VO_PTR)m_pFileChunk;
}

//headdata
VO_VOID CMKVAnalyze::AnalyzeFile( VO_S64 filesize )
{
	m_pFileChunk->FLocate( 0 );
	ReadSection( &CMKVAnalyze::ProcessMainSection , filesize , &m_Info );
	
	//Addedb by Aiven, dump the MKV HeadData for debug. Open this function only if you want to debug.	
	MKVSegmentStruct * ptr_seg = CMKVAnalyze::GetSegmentInfo(0);
	PrintMKVSegmentStruct(ptr_seg);	
}

//data
VO_U32 CMKVAnalyze::DataParse()
{
	VO_U32	nResult = ERROR_NONE;
	
	if(READY_STATE != GetStatus()){
		return ERROR_STATUS_WRONG;
	}

	SetStatus(RUNNING_STATE);

	nResult = ParseCluster();
	
	SetStatus(READY_STATE);

	return nResult;
}

VO_VOID CMKVAnalyze::Start()
{
	MKVSegmentStruct * pSegInfo = GetSegmentInfo(0);

	VOLOGI("Start");	
	m_pFileChunk->FLocate((VO_U64)pSegInfo->ClusterPos);
	Parser::Start();
}


VO_U32 CMKVAnalyze::MoveTo( VO_S64 llTimeStamp)
{
	VO_S64 filepos;
	MKVSegmentStruct * pSegInfo = GetSegmentInfo(0);

	VOLOGI("MoveTo----timestamp=%lld", llTimeStamp);	

	if(READY_STATE != GetStatus()){
		return VO_FALSE;
	}

	Flush();

	if( pSegInfo->CuesCuePointEntryListHeader.empty())
	{
		//I have to guess a place
		filepos = GuessPositionByTime( llTimeStamp , pSegInfo );//55564561;
		//filepos = GetPositionByTravelCluster( llTimeStamp , pSegInfo );
	}
	else
	{
		//use cues info to get a right place
		filepos = GetPositionWithCuesInfoByTime( llTimeStamp , pSegInfo );
	}
	//

	if (filepos >= 0){
//		SetStartFilePos(filepos);	
		m_pFileChunk->FLocate(filepos);
	}
	else{
//		SetParseEnd(VO_TRUE);
		m_bParseEnd = VO_TRUE;
	}


	return VO_ERR_SOURCE_OK;
}

VO_VOID CMKVAnalyze::Flush()
{
	VOLOGI("Flush");

	MKVSegmentStruct * ptr_seg = GetSegmentInfo(0);
	Parser::Flush();	
}
/*
VO_U32 CMKVAnalyze::PostAllSampleInList(MKVSegmentStruct * ptr_seg)
{
	VO_U32 nResult = ERROR_NONE;

	SampleListStruct *pSampleList;
	list_T<SampleListStruct *>::iterator	SampleListIter;
	VO_U32	IsTrackFull = 0;
	for(SampleListIter=ptr_seg->SampleListPointEntryListHeader.begin(); SampleListIter!=ptr_seg->SampleListPointEntryListHeader.end(); ++SampleListIter)
	{
		IsTrackFull++;
		pSampleList = (SampleListStruct *)(*SampleListIter);	
		if(!m_pSampleNotifyCenter->IsTrackRegist(pSampleList->TrackNumber)){
			IsTrackFull--;
			continue;
		}
			
		if(ERROR_STOP == PostTheSampleInList(pSampleList)){
			IsTrackFull--;
		}
	}
	
	if(!IsTrackFull){
		nResult = ERROR_STOP;
	}
	VOLOGI("PostAllSampleInList---nResult=%lu", nResult);

	return nResult;
}
*/
VO_U32 CMKVAnalyze::ParseCluster()
{
	VO_U32 nResult = ERROR_SKIP;
	MKVSegmentStruct * ptr_seg = GetSegmentInfo(0);	
	VO_U64 FilePos = m_pFileChunk->FGetFilePos();
	VOLOGI("ParseCluster---FilePos=%llu", FilePos);

/*
	if(ERROR_STOP == PostAllSampleInList(ptr_seg)){
		return ERROR_STOP;
	}
*/
	IDAndSizeInfo  Info;
	
	//check the file position
	if(FilePos>=(VO_U64)ptr_seg->SegmentEndPos){
		return ERROR_EOS;
	}
	
	if( !ReadIDAndSize( &Info ) ){
		m_pFileChunk->FLocate(FilePos);
		return nResult;
	}

	if((FilePos+(VO_U64)Info.size)>=(VO_U64)ptr_seg->SegmentEndPos){
		Info.size = ptr_seg->SegmentEndPos - FilePos;
	}

	//start to parse the data
	if( memcmp( Info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , Info.s_ID ) == 0 )
	{
		nResult = ProcessSegmentSection(&Info, ptr_seg);
	}
	else if(( memcmp( Info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_TIMECODE , Info.s_ID ) == 0 ) 
		||( memcmp( Info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_SIMPLEBLOCK , Info.s_ID ) == 0 )
		||( memcmp( Info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP , Info.s_ID ) == 0 ))
	{
		nResult = ProcessClusterSection(&Info, ptr_seg);
	}

	//skip the unknown data
	if(ERROR_SKIP == nResult){
		VO_U64 pos = m_pFileChunk->FGetFilePos();
		m_pFileChunk->FLocate(pos+Info.size);
	}
	
	return nResult;
}

MKVSegmentStruct * CMKVAnalyze::GetSegmentInfo( VO_U32 segmentindex )
{
	MKVSegmentStruct * pSeg = NULL;
	VO_U32 i = 0;

	list_T<MKVSegmentStruct*>::iterator iter;
	iter= m_Info.begin();

	while(iter!=m_Info.end())
	{
		if(i == segmentindex)
		{
			pSeg = (MKVSegmentStruct *)(*iter);
			break;
		}
		
		i++;		
		iter++;
	}

	return pSeg;
}

double	CMKVAnalyze::GetLastClusterTime()
{
	MKVSegmentStruct * ptr_seg = GetSegmentInfo(0);

	m_pFileChunk->FLocate( ptr_seg->ClusterPos );

	VO_U64 last_clusterpos = ptr_seg->ClusterPos;
	VO_BOOL find = VO_FALSE;
	
	VOLOGI("GetLastClusterTime----last_clusterpos=%llu", last_clusterpos);	

	do 
	{
		while( 1 )
		{
			VO_U64 pos = m_pFileChunk->FGetFilePos();
			IDAndSizeInfo info;

			if( ReadIDAndSize( &info ) == VO_FALSE )
			{
				find = VO_TRUE;
				break;
			}

			if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , info.s_ID ) == 0 )
			{
				last_clusterpos = pos;
			}

			if( !m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size ))
			{
				delete( info.pID );
				break;
			}

			delete( info.pID );
		}
	} while ( !find );

	m_pFileChunk->FLocate( last_clusterpos , VO_TRUE );

	VO_S64 CurrentTimeCode = 0;
	VO_S64 maxreftimecode = 0;

	while( 1 )
	{

		IDAndSizeInfo info;

		if( ReadIDAndSize( &info ) == VO_FALSE )
		{
			break;
		}

		if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , info.s_ID ) == 0 )
		{
			;
		}
		else if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_TIMECODE , info.s_ID ) == 0 )
		{
			if (info.size <= 4 )
			{
				Read_uint( (VO_S32)info.size , &CurrentTimeCode );
				CurrentTimeCode = (VO_S64)(ptr_seg->TimecodeScale / 1000000. * CurrentTimeCode);
			}
			else///<TIMECODE should be uint,so if it > 4,skip info.size bytes and the reset the value
			{
				info.size = 0;
				CurrentTimeCode = 0;
			}
		}
		else if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP , info.s_ID ) == 0 )
		{
		}
		else if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_BLOCK , info.s_ID ) == 0 )
		{
			VO_U64 end = m_pFileChunk->FGetFilePos() + info.size;
			VO_S64 rtime = 0;
			
			Read_vint( &rtime );
			
			Read_sint( 2 , &rtime );
			
			rtime =(VO_S64)( ptr_seg->TimecodeScale / (VO_S64)1000000. * rtime);
			
			if( rtime > maxreftimecode )
				maxreftimecode = rtime;

			if( !m_pFileChunk->FLocate( end ) )
			{
				delete( info.pID );
				break;
			}
		}
		else if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_SIMPLEBLOCK , info.s_ID ) == 0 )
		{
			VO_U64 end = m_pFileChunk->FGetFilePos() + info.size;
			VO_S64 rtime = 0;
			
			Read_vint( &rtime );
			
			Read_sint( 2 , &rtime );
			
			rtime =(VO_S64)( ptr_seg->TimecodeScale / (VO_S64)1000000. * rtime);

			if( rtime > maxreftimecode )
				maxreftimecode = rtime;

			if( !m_pFileChunk->FLocate( end ) )
			{
				delete( info.pID );
				break;
			}
		}
		else
		{
			if( !m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size ))
			{
				delete( info.pID );
				break;
			}
		}

		delete( info.pID );
	}

	return (double)(CurrentTimeCode + maxreftimecode);
}

VO_U32 CMKVAnalyze::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	*pllPreviousKeyframeTime = 0xffffffffffffffffll;
	*pllNextKeyframeTime = 0xffffffffffffffffll;
	MKVSegmentStruct * pSegInfo = GetSegmentInfo(0);

VOLOGI("GetNearKeyFrame----llTime=%lld", llTime);	
//	if (m_nType != VOTT_VIDEO/* || pSegInfo == NULL*/)
	if(pSegInfo == NULL)
	{
		return VO_ERR_NOT_IMPLEMENT;
	}


	if( !pSegInfo->CuesCuePointEntryListHeader.empty() )
	{
		list_T<CuesCuePointStruct *>::iterator iter;
		for(iter=pSegInfo->CuesCuePointEntryListHeader.begin(); iter!=pSegInfo->CuesCuePointEntryListHeader.end(); ++iter)
		{
			CuesCuePointStruct* pCue = (CuesCuePointStruct *)(*iter);
			
			if( pCue->CueTime * pSegInfo->TimecodeScale / 1000000. >= llTime )
			{
				if( pCue->CueTime * pSegInfo->TimecodeScale / 1000000. > llTime )
				{
					break;
				}
				*pllPreviousKeyframeTime = llTime;
			}
		}

		if( iter!=pSegInfo->CuesCuePointEntryListHeader.end()/*  && llTimeStamp < MaxCueTime*/)
		{
			CuesCuePointStruct* pCue = (CuesCuePointStruct *)(*iter);
			*pllNextKeyframeTime = (VO_S64)(pCue->CueTime * pSegInfo->TimecodeScale / 1000000.);

			if ((*pllPreviousKeyframeTime == (VO_S64)0xffffffffffffffffll) && iter!=pSegInfo->CuesCuePointEntryListHeader.begin())
			{
				CuesCuePointStruct* pCue = (CuesCuePointStruct *)(*iter);
				*pllPreviousKeyframeTime = (VO_S64)(pCue->CueTime * pSegInfo->TimecodeScale / 1000000.);
			}
			return VO_ERR_NONE;

		}
		
		if(*pllPreviousKeyframeTime == (VO_S64)0xffffffffffffffffll)
		{
			return VO_ERR_FAILED;
		}
		else
		{
			return VO_ERR_NONE;
		}
		
	}

	return VO_ERR_NOT_IMPLEMENT;
}


VO_S64 CMKVAnalyze::GuessPositionByTime( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo )
{
	double duration = pSegInfo->Duration * pSegInfo->TimecodeScale / 1000000.;
//	VO_U64 guessfilepos = (VO_U64)( (((llTimeStamp - 5000)<0)?0:(llTimeStamp - 5000)) / duration ) * ( pSegInfo->SegmentEndPos - pSegInfo->ClusterPos );
	double temp = ((llTimeStamp - 5000)<0)?0:((double)llTimeStamp - 5000.);
	double percent =   temp / duration;
	VO_U64 guessfilepos = (VO_U64)(percent * ( pSegInfo->SegmentEndPos - pSegInfo->ClusterPos ));

	VOLOGI("GuessPositionByTime----llTime=%lld", llTimeStamp);	

	if( guessfilepos <= (VO_U64)pSegInfo->ClusterPos )
		return GetPositionPrecisely( llTimeStamp , pSegInfo->ClusterPos );

	VO_U64 startpos = guessfilepos;
	VO_U64 endpos = pSegInfo->SegmentEndPos;

	VO_S64 filepos;

	while( 1 )
	{
		filepos = FindCluster( llTimeStamp , startpos , endpos );

		if( filepos >= 0 )
		{
			if( filepos < pSegInfo->ClusterPos )
			{
				filepos = pSegInfo->ClusterPos;
			}
			break;
		}
		else if(filepos == -2)
		{
			VO_S64 pos = (VO_U64)(startpos - ( 5000 / duration ) * ( pSegInfo->SegmentEndPos - pSegInfo->ClusterPos ));

			if( pos < pSegInfo->ClusterPos )
			{
				filepos = pSegInfo->ClusterPos;
				break;
			}
			else
			{
				startpos = pos;
			}
		}
		else
		{
			return -1;
		}
	}

	return GetPositionPrecisely( llTimeStamp , filepos );
}

VO_U64 CMKVAnalyze::GetPositionByTravelCluster( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo )
{
	return GetPositionPrecisely( llTimeStamp , pSegInfo->ClusterPos );
}

VO_S64 CMKVAnalyze::FindCluster( VO_S64 llTimeStamp , VO_U64 startpos , VO_U64 endpos, VO_BOOL direction)
{
/*
	VO_U64 findpos = -1;
	m_pFileChunk->FLocate( startpos );
*/
	VO_U32 dwInterSizeCluster = 4*1024*1024;

	VO_BYTE Buffer[1024];
	VO_U32 dwRead;
	VO_U32 offset = 0;
	VO_U32 count = 0;

	VO_S64 tempPos = (VO_S64)startpos;
	MKVSegmentStruct * pSegInfo = GetSegmentInfo(0);
	
	VOLOGI("FindCluster----llTime=%lld", llTimeStamp);	

	while( 1 )
	{
		//check whether there are clusters left
		if (count++ >= dwInterSizeCluster/1024)
		{
			return -1;
		}

		//calc the file position should be set.
		if(direction){
			if(tempPos <= pSegInfo->ClusterPos){
				return -1;
			}	
			
			tempPos = tempPos + offset - 1024;

			if(tempPos < pSegInfo->ClusterPos){
				tempPos = pSegInfo->ClusterPos;
			}
		}
		else
		{
			if(tempPos >= (VO_S64)endpos){
				return -1;
			}
			
			tempPos = m_pFileChunk->FGetFilePos() - offset;
		}

		if(m_pFileChunk->FLocate( tempPos )){
			return -1;
		}

		if( m_pFileChunk->FRead3( Buffer , 1024 , &dwRead ) )
		{
			for ( VO_U32 i = 0 ; i < dwRead - 3 ; i++ )
			{
				if( memcmp( Buffer + i , (VO_PTR)MKVID_SEGMENT_CLUSTER , 4 ) == 0 )
				{
					VO_U64 filepos = m_pFileChunk->FGetFilePos() - dwRead + i;
					//check if it is a cluster
					if( VO_TRUE == CheckIsCluster( filepos , endpos ) )
					{
						//get cluster time to compare
						VO_U64 timecode = GetClusterFirstKeyFrameTimeCodeDirectly( filepos );

						if( timecode <= (VO_U64)llTimeStamp )
							return filepos;		//find the cluster which conain the sample
						else
							return -2;			//haven't find the cluster which contain the sample
					}
				}
			}
			offset = 3;
/*
			Buffer[0] = Buffer[dwRead-3];
			Buffer[1] = Buffer[dwRead-2];
			Buffer[2] = Buffer[dwRead-1];
*/			
		}
		else
		{
			return -1;
		}
		
//		m_pFileChunk->FLocate( startpos );
	}
	m_pFileChunk->FLocate( startpos );
}

VO_BOOL CMKVAnalyze::CheckIsCluster( VO_U64 filepos , VO_U64 endpos )
{
	VO_U64 filepos_backup = m_pFileChunk->FGetFilePos();

	m_pFileChunk->FLocate( filepos );

	IDAndSizeInfo info;
	
	VOLOGI("CheckIsCluster----filepos=%llu, endpos=%llu", filepos, endpos);	

	if( ReadIDAndSize( &info ) == VO_FALSE )
	{
		m_pFileChunk->FLocate( filepos_backup );
		return VO_FALSE;
	}

	if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , info.s_ID ) != 0 )
	{
		m_pFileChunk->FLocate( filepos_backup );
		delete( info.pID );
		return VO_FALSE;
	}

	if( m_pFileChunk->FGetFilePos() + info.size > endpos - 5 )
	{
		m_pFileChunk->FLocate( filepos_backup );
		delete( info.pID );
		return VO_FALSE;
	}

	m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size );

	delete( info.pID );

	if( ReadIDAndSize( &info ) == VO_FALSE )
	{
		m_pFileChunk->FLocate( filepos_backup );
		return VO_FALSE;
	}

	if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , info.s_ID ) != 0 )
	{
		m_pFileChunk->FLocate( filepos_backup );
		delete( info.pID );
		return VO_FALSE;
	}

	delete( info.pID );

	m_pFileChunk->FLocate( filepos_backup );
	return VO_TRUE;
}

VO_U64 CMKVAnalyze::GetClusterFirstKeyFrameTimeCodeDirectly( VO_U64 filepos )
{
	VO_U64 filepos_backup = m_pFileChunk->FGetFilePos();
	VO_U64 CurFilePos = filepos;
	VO_U64 EndFilePos;
	VO_S64 clustertimecode = 0;
	VO_S64 mostsmallrelativetimecode = 0x7fffffffffffffffLL;
	MKVSegmentStruct * pSegInfo = GetSegmentInfo(0);

	m_pFileChunk->FLocate( filepos );

	IDAndSizeInfo info;

	VOLOGI("GetClusterFirstKeyFrameTimeCodeDirectly---filepos=%llu",filepos);		
	if( !ReadIDAndSize( &info ) )
	{
		m_pFileChunk->FLocate( filepos_backup );
		return 0xffffffffffffffffLL;
	}

	delete( info.pID );

	EndFilePos = CurFilePos + info.size;

	while ( 1 )
	{
		IDAndSizeInfo subinfo;

		if( !ReadIDAndSize( &subinfo ) )
			break;

		if( memcmp( subinfo.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_TIMECODE , subinfo.s_ID ) == 0 )
		{
			if (subinfo.size <= 4 )
			{
				Read_uint( (VO_U32)subinfo.size , &clustertimecode );
				clustertimecode = (VO_S64)(pSegInfo->TimecodeScale / 1000000. * clustertimecode);
			}
			else///<TIMECODE should be uint,so if it > 4,skip info.size bytes and the reset the value
			{
				break;
			}
		}
		else if( memcmp( subinfo.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP , subinfo.s_ID ) == 0 )
		{
			VO_U64 endpos = m_pFileChunk->FGetFilePos() + subinfo.size;

			VO_S64 time = GetFirstKeyFrameRelativeTime( m_pFileChunk->FGetFilePos() , subinfo.size );

			if( time != 0x7fffffffffffffffLL )
			{
				if( time < mostsmallrelativetimecode )
					mostsmallrelativetimecode = time;
			}

			m_pFileChunk->FLocate(endpos);
		}
		else if( memcmp( subinfo.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_SIMPLEBLOCK , subinfo.s_ID ) == 0 )
		{
			VO_U64 endpos = m_pFileChunk->FGetFilePos() + subinfo.size;

			VO_S64 id;
			Read_vint( &id );

			VO_S64 relativetimecode;
			Read_sint( 2 , &relativetimecode );
			relativetimecode = (VO_S64)(pSegInfo->TimecodeScale / 1000000. * relativetimecode);

			VO_U8 flag;
			m_pFileChunk->FRead( &flag , sizeof( VO_U8 ) );

			if( (flag & 0x80) && id == Video_Type )
			{
				if( relativetimecode < mostsmallrelativetimecode )
					mostsmallrelativetimecode = relativetimecode;
			}

			m_pFileChunk->FLocate( endpos );
		}
		else
		{
			VO_U64 pos = m_pFileChunk->FGetFilePos();
			m_pFileChunk->FLocate( pos + subinfo.size );
		}

		delete(subinfo.pID);

		CurFilePos = m_pFileChunk->FGetFilePos();

		if( CurFilePos >= EndFilePos )
			break;
	}

	m_pFileChunk->FLocate( filepos_backup );

	if( mostsmallrelativetimecode == 0x7fffffffffffffffLL )
		return 0xffffffffffffffffLL;
	else
		return mostsmallrelativetimecode + clustertimecode;
}

VO_S64 CMKVAnalyze::GetFirstKeyFrameRelativeTime( VO_U64 filepos , VO_U64 blockgroupsize )
{
	VO_U64 filepos_backup = filepos;
	VO_U64 endpos = filepos + blockgroupsize;

	VO_BOOL isKeyFrame = VO_TRUE;
	VO_S64 submostsmallframerelativecode = 0x7fffffffffffffffLL;
	MKVSegmentStruct * pSegInfo = GetSegmentInfo(0);

	m_pFileChunk->FLocate( filepos );

	VOLOGI("GetFirstKeyFrameRelativeTime---filepos=%llu",filepos);		

	while( 1 )
	{
		IDAndSizeInfo info;
		if(!ReadIDAndSize( &info ))
		{
			break;
		}
		if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_BLOCK , info.s_ID ) == 0 )
		{
			VO_U64 subendpos = m_pFileChunk->FGetFilePos() + info.size;
			VO_S64 id;
			Read_vint( &id );

			VO_S64 relativetimecode;
			Read_sint( 2 , &relativetimecode );
			relativetimecode = (VO_S64)(pSegInfo->TimecodeScale / 1000000. * relativetimecode);

//			if( m_simpleblock_kf_way || m_groupblock_kf_way)
			if(1)
			{
				VO_U8 flag;
				m_pFileChunk->FRead( &flag , sizeof( VO_U8 ) );

				if( (flag & 0x80) && id == Video_Type )
				{
					if( relativetimecode < submostsmallframerelativecode )
						submostsmallframerelativecode = relativetimecode;
				}
			}
			else
			{
				if( (id == Video_Type) && (relativetimecode < submostsmallframerelativecode) )
					submostsmallframerelativecode = relativetimecode;

				if( id != Video_Type )
					isKeyFrame = VO_FALSE;
			}

			m_pFileChunk->FLocate( subendpos );
		}
		else if( memcmp( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_REFERENCEBLOCK , info.s_ID ) == 0 )
		{
			isKeyFrame = VO_FALSE;
			delete( info.pID );
			break;
		}
		else
		{
			m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size );
		}

		delete( info.pID );

		if( m_pFileChunk->FGetFilePos() >= endpos )
			break;
	}

	m_pFileChunk->FLocate( filepos_backup );

	if( isKeyFrame )
		return submostsmallframerelativecode;
	else
		return 0x7fffffffffffffffLL;
}

VO_U64 CMKVAnalyze::GetPositionWithCuesInfoByCount()
{
	MKVSegmentStruct * pSegInfo = GetSegmentInfo(0);

	VO_U64 nCurPos = m_pFileChunk->FGetFilePos();
	CuesCuePointTrackPositionsStruct * pPointer = NULL;
	
	VOLOGI("GetPositionWithCuesInfoByCount");		

	list_T<CuesCuePointStruct *>::iterator iter;
	for(iter=pSegInfo->CuesCuePointEntryListHeader.begin(); iter!=pSegInfo->CuesCuePointEntryListHeader.end(); ++iter)
	{
		CuesCuePointStruct* pCue = (CuesCuePointStruct *)(*iter);
		
		list_T<CuesCuePointTrackPositionsStruct *>::iterator tempiter = pCue->TrackPositionsHeader.begin();
		pPointer = (CuesCuePointTrackPositionsStruct*)(*tempiter);

		if (pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos > (VO_S64)nCurPos)
		{
			break;
		}
	}
	
	return pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos;
	
}
VO_U64 CMKVAnalyze::GetPositionWithCuesInfoByTime( VO_S64 llTimeStamp , MKVSegmentStruct * pSegInfo )
{
//	voLIST_ENTRY * pEntry = pSegInfo->CuesCuePointEntryListHeader.Flink;
	VO_S64 MaxCueTime = 0;
	VO_S64 TempTimeStamp = llTimeStamp;
	VO_S64 n_Pos = 0;

	VOLOGI("GetPositionWithCuesInfoByTime---llTimeStamp=%lld",llTimeStamp);		

	if(pSegInfo->CuesCuePointEntryListHeader.empty()){
		return GetPositionByTravelCluster( TempTimeStamp , pSegInfo );
	}

	list_T<CuesCuePointStruct *>::iterator iter;
	iter=pSegInfo->CuesCuePointEntryListHeader.begin();
	for(iter=pSegInfo->CuesCuePointEntryListHeader.begin(); iter!=pSegInfo->CuesCuePointEntryListHeader.end(); ++iter)
	{
		CuesCuePointStruct* pCue = (CuesCuePointStruct *)(*iter);
		
		if( pCue->CueTime * pSegInfo->TimecodeScale / 1000000. >= TempTimeStamp )
		{
			if (pCue->CueTime * pSegInfo->TimecodeScale / 1000000. == TempTimeStamp)
			{
				CuesCuePointTrackPositionsStruct * pPointer = NULL;
				list_T<CuesCuePointTrackPositionsStruct *>::iterator tempiter = pCue->TrackPositionsHeader.begin();
				pPointer = (CuesCuePointTrackPositionsStruct*)(*tempiter);

				return pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos;
			}
			break;
		}

		MaxCueTime = (VO_S64)(pCue->CueTime* pSegInfo->TimecodeScale / 1000000.);
	}
	//can't understand
/*	
	if( pEntry == &(pSegInfo->CuesCuePointEntryListHeader))
	{
		voLIST_ENTRY * pEntry = pSegInfo->CuesCuePointEntryListHeader.Blink;
		if (pEntry == &(pSegInfo->CuesCuePointEntryListHeader))
		{
			///<There is no list in Cue segment,but i don't think it make sence,just in case
			return GetPositionByTravelCluster( TempTimeStamp , pSegInfo );
		}
		CuesCuePointStruct * pCue = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );
		if (MaxCueTime != pCue->CueTime* pSegInfo->TimecodeScale / 1000000.)
		{
			///< the cue segment is not ordered by cue time,there must be something wrong 
			return GetPositionByTravelCluster( TempTimeStamp , pSegInfo );
		}
		CuesCuePointTrackPositionsStruct * pPointer = GET_OBJECTPOINTER(pCue->TrackPositionsHeader.Flink , CuesCuePointTrackPositionsStruct , List );
		 
		VO_U64 ret= GetPositionPrecisely( TempTimeStamp , pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos);
		return ret;
	}
*/
	//we find the cluster
	if((--iter) != pSegInfo->CuesCuePointEntryListHeader.begin()){
		CuesCuePointStruct* pCue = (CuesCuePointStruct *)(*iter);
		CuesCuePointTrackPositionsStruct * pPointer = NULL;
		list_T<CuesCuePointTrackPositionsStruct *>::iterator tempiter = pCue->TrackPositionsHeader.begin();
		pPointer = (CuesCuePointTrackPositionsStruct *)(*tempiter);
		n_Pos = pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos;
		VOLOGI("GetPositionWithCuesInfoByTime---CueTime=%lld",pCue->CueTime); 	
		
	}else{
		n_Pos = GuessPositionByTime( llTimeStamp , pSegInfo );
	}

	VOLOGI("GetPositionWithCuesInfoByTime---n_Pos=%lld",n_Pos);		

	return (VO_U64)n_Pos;
/*	
	if( pEntry->Blink != &(pSegInfo->CuesCuePointEntryListHeader) )
		pEntry = pEntry->Blink;
	else
		return GuessPositionByTime( llTimeStamp , pSegInfo );

	CuesCuePointStruct * pCue = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );
	CuesCuePointTrackPositionsStruct * pPointer = GET_OBJECTPOINTER(pCue->TrackPositionsHeader.Flink , CuesCuePointTrackPositionsStruct , List );
	return pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos;
*/	
}

VO_U64 CMKVAnalyze::GetPositionPrecisely( VO_S64 llTimeStamp , VO_U64 posnear )
{
	VO_U64 clusterpre = posnear;

	m_pFileChunk->FLocate( posnear );

	IDAndSizeInfo info;
	if( !ReadIDAndSize( &info ) )
		return clusterpre;

	delete( info.pID );

	m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size );
	
	VOLOGI("GetPositionPrecisely---llTimeStamp=%lld",llTimeStamp);		

	while( 1 )
	{
		VO_U64 backup = m_pFileChunk->FGetFilePos();

		IDAndSizeInfo infonext;
		if( !ReadIDAndSize( &infonext ) )
			return clusterpre;

		if( memcmp( infonext.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , infonext.s_ID ) != 0 )
		{
			delete( infonext.pID );
			return clusterpre;
		}

		delete( infonext.pID );

		VO_U64 timecode = GetClusterFirstKeyFrameTimeCodeDirectly( backup );

		if( timecode == 0xffffffffffffffffLL )
		{
			;
		}
		else if( timecode > (VO_U64)llTimeStamp )
			return clusterpre;
		else if( timecode == (VO_U64)llTimeStamp )
			return backup;
		else
			clusterpre = backup;

		m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + infonext.size );
	}

	return posnear;
}


/********************************************************************************************/
VO_U32 CMKVAnalyze::ReadSection( pProcessSection ProcessSection , VO_S64 SectionSize , VO_VOID * pParam )
{
	VO_U32	nResult = ERROR_NONE;
	
	VO_S64 CurFilePos , SectionEndFilePos;

	CurFilePos = m_pFileChunk->FGetFilePos();

	SectionEndFilePos = CurFilePos + SectionSize;

	while ( 1 )
	{
		IDAndSizeInfo info;

//		m_lastfilepos = m_pFileChunk->FGetFilePos();

		if( !ReadIDAndSize( &info ) )
			break;
		CurFilePos = m_pFileChunk->FGetFilePos();

		if(info.size > SectionEndFilePos - CurFilePos)
			info.size = SectionEndFilePos - CurFilePos;

		switch((this->*ProcessSection)( &info , pParam ) )
		{
			case ERROR_SKIP:
			{
				VO_U64 pos = m_pFileChunk->FGetFilePos();
				m_pFileChunk->FLocate( pos + info.size );		
			}
				break;		
			case ERROR_STOP:
			{				
				delete(info.pID);
				nResult = ERROR_STOP;
				return nResult;
			}
				break;
			case ERROR_NONE:
			{
				//call the notify function if it's registed before.
				EventNotify(&info);			
			}
		}

		delete(info.pID);

		CurFilePos = m_pFileChunk->FGetFilePos();

		if( CurFilePos >= SectionEndFilePos )
			break;	
	}

	return nResult;
}

VO_U32 CMKVAnalyze::ProcessMainSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	VO_U32	nResult = ERROR_SKIP;
	list_T<MKVSegmentStruct *>* pMKVInfo = ( list_T<MKVSegmentStruct *>*)pParam;

	if( memcmp( pInfo->pID , (VO_PTR)(VO_PTR)MKVID_SEGMENT , pInfo->s_ID ) == 0 )
	{
		MKVSegmentStruct * pSegInfo = (MKVSegmentStruct*)new MKVSegmentStruct;
		InitializeMKVSegmentStruct( pSegInfo );

		pSegInfo->SegmentBeginPos = m_pFileChunk->FGetFilePos();
		pSegInfo->SegmentEndPos = pSegInfo->SegmentBeginPos + pInfo->size;
		pMKVInfo->push_back(pSegInfo);
		
		nResult = ReadSection( &CMKVAnalyze::ProcessSegmentSection , pInfo->size , pSegInfo );
	}
	else
	{
		nResult = ERROR_SKIP;
	}

	return nResult;
}

VO_U32 CMKVAnalyze::ProcessSegmentSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;
	VO_U32	nResult = ERROR_SKIP;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD , pInfo->s_ID ) == 0 )
	{
		VOLOGI( "+CMKVAnalyze::ProcessSeekHeadSection %lu" , voOS_GetSysTime() );
		nResult = ReadSection( &CMKVAnalyze::ProcessSeekHeadSection , pInfo->size , pSegInfo );
		VOLOGI( "-CMKVAnalyze::ProcessSeekHeadSection %lu" , voOS_GetSysTime() );
	}
	else if(  memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEGMENTINFO , pInfo->s_ID ) == 0  )
	{
		VOLOGI( "+CMKVAnalyze::ProcessSegmentInfoSection %lu" , voOS_GetSysTime() );
		nResult = ReadSection( &CMKVAnalyze::ProcessSegmentInfoSection , pInfo->size , pSegInfo );
		VOLOGI( "-CMKVAnalyze::ProcessSegmentInfoSection %lu" , voOS_GetSysTime() );
	}
	else if(  memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS , pInfo->s_ID ) == 0  )
	{
		VOLOGI( "+CMKVAnalyze::ProcessTracksSection %lu" , voOS_GetSysTime() );
		nResult = ReadSection( &CMKVAnalyze::ProcessTracksSection , pInfo->size , pSegInfo );
		VOLOGI( "-CMKVAnalyze::ProcessTracksSection %lu" , voOS_GetSysTime() );
	}
	else if(  memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA , pInfo->s_ID ) == 0  )
	{
		VOLOGI( "+CMKVAnalyze::ProcessCuesSection %lu" , voOS_GetSysTime() );
		nResult = ReadSection( &CMKVAnalyze::ProcessCuesSection , pInfo->size , pSegInfo );
		VOLOGI( "-CMKVAnalyze::ProcessCuesSection %lu" , voOS_GetSysTime() );
	}
	else if(  memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , pInfo->s_ID ) == 0  )
	{
		VOLOGI( "+CMKVAnalyze::ProcessClusterSection %lu" , voOS_GetSysTime() );
		VO_U64 CurFilePos = m_pFileChunk->FGetFilePos();		
		if(0 ==pSegInfo->ClusterPos){
			pSegInfo->ClusterPos = CurFilePos;
		}
//		pSegInfo->ClusterPos = (0 ==pSegInfo->ClusterPos)?m_pFileChunk->FGetFilePos():pSegInfo->ClusterPos;
		nResult = ReadSection( &CMKVAnalyze::ProcessClusterSection , pInfo->size , pSegInfo );

		//skip the cluster and don't parse other cluster again.
		if(nResult == ERROR_STOP && m_nAnalyzeMode & PROCESS_ANALYZE_MODE_FLAG)
		{
			nResult = ERROR_NONE;
			m_pFileChunk->FLocate(CurFilePos + pInfo->size);				
			m_nAnalyzeMode = m_nAnalyzeMode & (~PROCESS_CLUSTER_FLAG);
		}
		//rollback to the position where the block start.
		else if(nResult == ERROR_STOP){			
			nResult = ERROR_NONE;
			m_pFileChunk->FLocate(CurFilePos);				
		}		
		
		VOLOGI( "-CMKVAnalyze::ProcessClusterSection %lu" , voOS_GetSysTime() );
	}
	else
	{
		nResult = ERROR_SKIP;
	}

	return nResult;
}

VO_U32 CMKVAnalyze::ProcessSeekHeadSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD_SEEK , pInfo->s_ID ) == 0 )
	{
		SeekHeadElementStruct * pElement = ( SeekHeadElementStruct*)new SeekHeadElementStruct;
		memset( pElement , 0 , sizeof( SeekHeadElementStruct ) );

		ReadSection( &CMKVAnalyze::ProcessSeekHeadSeekSection , pInfo->size , pElement );
		pSegInfo->SeekHeadElementEntryListHeader.push_back(pElement);
//		InsertASCSortedList<SeekHeadElementStruct>( &(pSegInfo->SeekHeadElementEntryListHeader) , &(pElement->List) , SeekHeadElementStructComp() );
	}
	else
	{
		return ERROR_SKIP;
	}

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessSeekHeadSeekSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	SeekHeadElementStruct * pElement = ( SeekHeadElementStruct * )pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD_SEEK_SEEKID , pInfo->s_ID ) == 0 )
	{
		VO_PBYTE pID = (VO_PBYTE)new VO_PBYTE[(VO_U32)(pInfo->size * sizeof( VO_BYTE ))];
		memset( pID , 0 , (VO_U32)(pInfo->size * sizeof( VO_BYTE )) );

		m_pFileChunk->FRead( pID , (VO_U32)pInfo->size );

		if( memcmp( pID , (VO_PTR)MKVID_SEGMENT_SEGMENTINFO , 4 ) == 0 )
		{
			pElement->SeekID = SegmentInfo;
		}
		else if( memcmp( pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD , 4 ) == 0 )
		{
			pElement->SeekID = SeekHead;
		}
		else if( memcmp( pID , (VO_PTR)MKVID_SEGMENT_TRACKS , 4 ) == 0 )
		{
			pElement->SeekID = Tracks;
		}
		else if( memcmp( pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , 4 ) == 0 )
		{
			pElement->SeekID = Cluster;
		}
		else if( memcmp( pID , (VO_PTR)MKVID_SEGMENT_CUEDATA , 4 ) == 0 )
		{
			pElement->SeekID = Cues;
		}

		delete(pID);
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD_SEEK_SEEKPOSITION , pInfo->s_ID ) == 0 )
	{
		VO_S64 pos;

		Read_uint( (VO_S32)pInfo->size , &pos );

		pElement->SeekPos = pos;
	}
	else
	{
		return ERROR_SKIP;
	}

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessSegmentInfoSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEGMENTINFO_DURATION , pInfo->s_ID ) == 0 )
	{
		if( pInfo->size == 4 )
		{
			float duration;

			m_pFileChunk->FRead( &duration , sizeof( float ) );
			BigEndianLittleEndianExchange( &duration , sizeof(float) );

			pSegInfo->Duration = duration;
		}
		else if( pInfo->size == 8 )
		{
			double duration;

			m_pFileChunk->FRead( &duration , sizeof( double ) );
			BigEndianLittleEndianExchange( &duration , sizeof(double) );

			pSegInfo->Duration = duration;
		}
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEGMENTINFO_TIMECODESCALE , pInfo->s_ID ) == 0 )
	{
		VO_S64 codescale = 0;

		Read_uint( (VO_S32)pInfo->size , &codescale );

		pSegInfo->TimecodeScale = codescale;
	}
	else
	{
		return ERROR_SKIP;
	}
	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessTracksSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY , pInfo->s_ID ) == 0 )
	{
		TracksTrackEntryStruct * pElement = (TracksTrackEntryStruct*)new TracksTrackEntryStruct;
		memset( pElement , 0 , sizeof( TracksTrackEntryStruct ) );
		InitializeTracksTrackEntryStruct( pElement );

		ReadSection( &CMKVAnalyze::ProcessTrackEntrySection , pInfo->size , pElement );

		//check the tracktype, if we don't want to parse it ,return  ERROR_NONE dirctly.
		switch(pElement->TrackType)
		{
			case Video_Type:
				if(!(m_nAnalyzeMode & PROCESS_VIDEOTRACK_FLAG)){
					delete(pElement);
					return ERROR_NONE;
				}
				break;
			case Audio_Type:
				if(!(m_nAnalyzeMode & PROCESS_AUDIOTRACK_FLAG)){
					delete(pElement);
					return ERROR_NONE;
				}
				break;
			case Complex_Type:
				if(!(m_nAnalyzeMode & PROCESS_COMPLEXTRACK_FLAG)){
					delete(pElement);
					return ERROR_NONE;
				}
				break;
			case Logo_Type:
				if(!(m_nAnalyzeMode & PROCESS_LOGOTRACK_FLAG)){
					delete(pElement);
					return ERROR_NONE;
				}
				break;
			case Subtitle_Type:
				if(!(m_nAnalyzeMode & PROCESS_SUBTITLETRACK_FLAG)){
					delete(pElement);
					return ERROR_NONE;
				}
				break;
			case Buttons_Type:
				if(!(m_nAnalyzeMode & PROCESS_BUTTONSTACK_FLAG)){
					delete(pElement);
					return ERROR_NONE;
				}
				break;
			case Control_Type:
				if(!(m_nAnalyzeMode & PROCESS_CONTROLTRACK_FLAG)){
					delete(pElement);
					return ERROR_NONE;
				}
				break;			
			default:
				delete(pElement);
				return ERROR_NONE;
		}

		pSegInfo->TracksTrackEntryListHeader.push_back(pElement);
		
	}
	else
	{
		return ERROR_SKIP;
	}

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessTrackEntrySection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_TRACKNUMBER , pInfo->s_ID ) == 0 )
	{
		VO_S64 tracknum;

		Read_uint( (VO_S32)pInfo->size , &tracknum );

		pElement->TrackNumber = (VO_U32)tracknum;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_TRACKTYPE , pInfo->s_ID ) == 0 )
	{
		VO_S64 tracktype;

		Read_uint( (VO_S32)pInfo->size , &tracktype );

		pElement->TrackType = (VO_U32)tracktype;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_FLAGENABLED , pInfo->s_ID ) == 0 )
	{
		VO_S64 enable;

		Read_uint( (VO_S32)pInfo->size , &enable );

		pElement->IsEnabled = (enable == 1) ? VO_TRUE : VO_FALSE;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_FLAGDEFAULT , pInfo->s_ID ) == 0 )
	{
		VO_S64 defaultflag;

		Read_uint( (VO_S32)pInfo->size , &defaultflag );

		pElement->IsDefault = (defaultflag == 1) ? VO_TRUE : VO_FALSE;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_FLAGFORCED , pInfo->s_ID ) == 0 )
	{
		VO_S64 forced;

		Read_uint( (VO_S32)pInfo->size , &forced );

		pElement->IsForced = (forced == 1) ? VO_TRUE : VO_FALSE;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_DEFAULTDURATION , pInfo->s_ID ) == 0 )
	{
		VO_S64 defaultduration;

		Read_uint( (VO_S32)pInfo->size , &defaultduration );

		pElement->DefaultDuration = defaultduration;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_TRACKTIMECODESCALE , pInfo->s_ID ) == 0 )
	{
		if( pInfo->size == 4 )
		{
			float duration;

			m_pFileChunk->FRead( &duration , sizeof(float) );
			BigEndianLittleEndianExchange( &duration , sizeof(float) );

			pElement->TrackTimecodeScale = duration;
		}
		else if( pInfo->size == 8 )
		{
			double duration;

			m_pFileChunk->FRead( &duration , sizeof( double ) );
			BigEndianLittleEndianExchange( &duration , sizeof(double) );

			pElement->TrackTimecodeScale = duration;
		}
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_LANGUAGE , pInfo->s_ID ) == 0 )
	{
		VO_U8 languae[10];
		memset( languae , 0 , 10 );

		m_pFileChunk->FRead( languae , (VO_U32)pInfo->size );
		memcpy( pElement->str_Language , languae , (VO_U32)pInfo->size );
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CODECID , pInfo->s_ID ) == 0 )
	{
		VO_U8 codecid[50];
		memset( codecid , 0 , 50 );

		m_pFileChunk->FRead( codecid , (VO_U32)pInfo->size );
		memcpy( pElement->str_CodecID , codecid , (VO_U32)pInfo->size );
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CODECPRIVATE , pInfo->s_ID ) == 0 )
	{
		if( pInfo->size )
		{
			pElement->CodecPrivateSize = (VO_U32)pInfo->size;
			pElement->pCodecPrivate = ( VO_S8* )new VO_S8[(VO_U32)pInfo->size];

			m_pFileChunk->FRead( pElement->pCodecPrivate , (VO_U32)pInfo->size );
		}
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_VIDEO , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessTrackEntryVideoSection , pInfo->size , pElement );
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_AUDIO , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessTrackEntryAudioSection , pInfo->size , pElement );
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTENCODEINGS , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessContentEncodings , pInfo->size , pElement );
	}
	else
	{
		return ERROR_SKIP;
	}

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessTrackEntryVideoSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_VIDEO_PIXELWIDTH , pInfo->s_ID ) == 0 )
	{
		VO_S64 width;

		Read_uint( (VO_S32)pInfo->size , &width );

		pElement->PixelWidth = (VO_U32)width;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_VIDEO_PIXELHEIGHT , pInfo->s_ID ) == 0 )
	{
		VO_S64 height;

		Read_uint( (VO_S32)pInfo->size , &height );

		pElement->PixelHeight = (VO_U32)height;
	}
	else
	{
		return ERROR_SKIP;
	}

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessTrackEntryAudioSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_AUDIO_SAMPLEFREQ , pInfo->s_ID ) == 0 )
	{
		if( (VO_S32)pInfo->size == 4 )
		{
			float samplefreq;

			m_pFileChunk->FRead( &samplefreq , sizeof( float ) );
			BigEndianLittleEndianExchange( &samplefreq , sizeof(float) );

			pElement->SampleFreq = samplefreq;
		}
		else if( (VO_S32)pInfo->size == 8 )
		{
			double samplefreq;

			m_pFileChunk->FRead( &samplefreq , sizeof( double ) );
			BigEndianLittleEndianExchange( &samplefreq , sizeof(double) );

			pElement->SampleFreq = samplefreq;
		}
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_AUDIO_CHANNELS , pInfo->s_ID ) == 0 )
	{
		VO_S64 channels;

		Read_uint( (VO_S32)pInfo->size , &channels );

		pElement->Channels = (VO_U32)channels;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_AUDIO_BITDEPTH , pInfo->s_ID ) == 0 )
	{
		VO_S64 bitdepth;

		Read_uint( (VO_S32)pInfo->size , &bitdepth );

		pElement->BitDepth = (VO_U32)bitdepth;
	}
	else
	{
		return ERROR_SKIP;
	}

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessBlockSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	if(-1 == m_nCurrectClusterTime){
		VOLOGE("ProcessBlockSection---m_nCurrectClusterTime=-1");
		return ERROR_SKIP;
	}

	return (m_nAnalyzeMode | PROCESS_BLOCK_FLAG) ? ParseBlockSection(pInfo , pParam) : SkipBlockSection(pInfo , pParam);
}

VO_U32 CMKVAnalyze::SkipBlockSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
//	VO_U64 BlockEndPos = m_pFileChunk->FGetFilePos() + pInfo->size;
//	m_pFileChunk->FLocate(BlockEndPos);

	return ERROR_SKIP;
}

VO_U32 CMKVAnalyze::ParseBlockSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	VO_U64 LastPos = m_pFileChunk->FGetFilePos();
	VO_U64 BlockEndPos = LastPos + pInfo->size;
	VO_U8 m_framecount = 0;
	VO_U64 m_framesizearray[256] = {0};
	VO_S64 tracknumber = 0;
	VO_S64 timecode = 0;

	if(NULL == pInfo && NULL == pParam){
		return ERROR_SKIP;
	}


	Read_vint( &tracknumber);
	TracksTrackEntryStruct* trackinfo = GetTrackInfoByNumber((VO_U32)tracknumber);

	//skip the unneeded data.
	if(!m_pSampleNotifyCenter->IsTrackRegist((VO_U32)tracknumber)){
		m_pFileChunk->FLocate(LastPos);
		return ERROR_SKIP;
	}

	Read_uint( 2 , &timecode );
	MKVSegmentStruct * ptr_seg = GetSegmentInfo(0);
	timecode = (VO_S64)(ptr_seg->TimecodeScale / 1000000. * timecode);

	VO_U8 flag;
	m_pFileChunk->FRead( &flag , sizeof( VO_U8 ) );

	switch( (flag & 0x6)>>1 )
	{
	case 0x0:// no lacing
		{
			m_framecount = 1;
			m_framesizearray[0] = BlockEndPos - m_pFileChunk->FGetFilePos();			
		}
		break;
	case 0x1:
	case 0x2:
	case 0x3:
		{
			m_pFileChunk->FRead( &m_framecount , sizeof( VO_U8 ) );
			m_framecount++;

			memset( m_framesizearray , 0 ,sizeof(VO_U32)*m_framecount );

			if( (flag & 0x6) == 0x2 )//xiph lacing
			{
				VO_U64 totalsize, sumsize = 0;
				for( VO_U8 i = 0 ; i < m_framecount - 1 ; i++ )
				{
					VO_U8 value;
					do
					{
						m_pFileChunk->FRead( &value , sizeof( VO_U8 ) );
						m_framesizearray[i] = (VO_U64)value;
					} while ( value == 0xff );

					sumsize += m_framesizearray[i];
				}

				totalsize = BlockEndPos - m_pFileChunk->FGetFilePos();

				m_framesizearray[ m_framecount - 1 ] = (VO_U64)(totalsize - sumsize);
			}
			else if( (flag & 0x6) == 0x4 )//fix size lacing
			{
				VO_U64 leftsize = BlockEndPos - m_pFileChunk->FGetFilePos();
				VO_U64 average = leftsize / (VO_U64)m_framecount;

				for( VO_U8 i = 0 ; i < m_framecount ; i++ )
				{
					m_framesizearray[i] = average;
				}
			}
			else if( (flag & 0x6) == 0x6 )//ebml lacing
			{
				VO_S64 result;
				VO_U64 totalsize, sumsize;

				Read_vint( &result );
				m_framesizearray[0] = (VO_U64)result;

				sumsize = (VO_U64)result;

				for( VO_U8 i = 1 ; i < m_framecount - 1 ; i++ )
				{
					Read_svint( &result );
					m_framesizearray[i] = (VO_U32)(result + m_framesizearray[i-1]);

					sumsize = sumsize + m_framesizearray[i];
				}

				totalsize = BlockEndPos - m_pFileChunk->FGetFilePos();

				m_framesizearray[ m_framecount - 1 ] = (VO_U64)(totalsize - sumsize);
			}

		}		
		break;
	default:
		break;
	}

	VO_U64 framesize = 0;
	VO_S64	duration = 0;
	
	list_T<SampleElementStruct *>* pTempSamples = (list_T<SampleElementStruct *>*)pParam;

	duration = (0 != trackinfo->DefaultDuration)?trackinfo->DefaultDuration/1000000:10;//30 is the default value

	for(int i = 0; i < m_framecount; i++){
		SampleElementStruct * Sample = (SampleElementStruct*)new SampleElementStruct;
		memset(Sample, 0x0, sizeof(SampleElementStruct));
		
		Sample->bIsKeyFrame = ( flag & 0x80 )? VO_TRUE : VO_FALSE;

		Sample->RelativeTimeCode = timecode + duration*i;
		Sample->TimeCode = 0;
		
		Sample->framesize = m_framesizearray[i];
		Sample->TrackNumber = (VO_U8)tracknumber;
		Sample->duration = duration;
		
		Sample->filepos = m_pFileChunk->FGetFilePos() + framesize;	
		framesize += Sample->framesize;
		pTempSamples->push_back(Sample);
	}

	m_pFileChunk->FLocate(BlockEndPos);
	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessBlockGroupSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{	
	BlockGroupElementStruct * BlockGroup = (BlockGroupElementStruct *)pParam;	
	VO_U32	nResult = ERROR_NONE;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_BLOCK , pInfo->s_ID ) == 0 )
	{		
		VOLOGI( "+CMKVAnalyze::ProcessBlockSection--ProcessBlockSection %lu" , voOS_GetSysTime() );
		nResult= ProcessBlockSection(pInfo,&BlockGroup->SampleElementList);
		VOLOGI( "+CMKVAnalyze::ProcessBlockSection--ProcessBlockSection %lu" , voOS_GetSysTime() );
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_BLOCKDURATION, pInfo->s_ID ) == 0 )
	{
		Read_uint( (VO_S32)pInfo->size , &BlockGroup->BlockDuration );
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_REFERENCEBLOCK, pInfo->s_ID ) == 0 )
	{
		Read_uint( (VO_S32)pInfo->size , &BlockGroup->ReferenceTimecodec );
	}
	else
	{
		nResult = ERROR_SKIP;
	}
	
	return nResult;

}

VO_U32 CMKVAnalyze::ProcessCuesSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT , pInfo->s_ID ) == 0 )
	{
		CuesCuePointStruct * pElement = (CuesCuePointStruct*)new CuesCuePointStruct;
//		InitializeCuesCuePointStruct( pElement );

		ReadSection( &CMKVAnalyze::ProcessCuePointSection , (VO_U32)pInfo->size , pElement );

//		InsertASCSortedList< CuesCuePointStruct >( &(pSegInfo->CuesCuePointEntryListHeader) , &(pElement->List) , CuesCuePointStructComp() );
		pSegInfo->CuesCuePointEntryListHeader.push_back(pElement);
	}
	else
	{
		return ERROR_SKIP;
	}

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessCuePointSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	CuesCuePointStruct * pElement = ( CuesCuePointStruct * )pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETIME , pInfo->s_ID ) == 0 )
	{
		VO_S64 cuetime;

		Read_uint( (VO_S32)pInfo->size , &cuetime );

		pElement->CueTime = cuetime;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETRACKPOSITIONS, pInfo->s_ID ) == 0 )
	{
		CuesCuePointTrackPositionsStruct * pPosition = (CuesCuePointTrackPositionsStruct*)new CuesCuePointTrackPositionsStruct;
		memset( pPosition , 0 , sizeof( CuesCuePointTrackPositionsStruct ) );

		ReadSection( &CMKVAnalyze::ProcessCuePointTrackPositionsSection , (VO_U32)pInfo->size , pPosition );

//		InsertTailList( &( pElement->TrackPositionsHeader ) , &(pPosition->List) );
		pElement->TrackPositionsHeader.push_back(pPosition);
	}
	else
	{
		return ERROR_SKIP;
	}

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessCuePointTrackPositionsSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	CuesCuePointTrackPositionsStruct * pElement = ( CuesCuePointTrackPositionsStruct * )pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETRACKPOSITIONS_CUETRACK , pInfo->s_ID ) == 0 )
	{
		VO_S64 tracknumber;

		Read_uint( (VO_U32)pInfo->size , &tracknumber );

		pElement->CueTrack = (VO_U32)tracknumber;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETRACKPOSITIONS_CUECLUSTERPOSITION , pInfo->s_ID ) == 0 )
	{
		VO_S64 pos;

		Read_uint( (VO_S32)pInfo->size , &pos );

		pElement->CueClusterPosition = pos;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETRACKPOSITIONS_CUEBLOCKNUMBER , pInfo->s_ID ) == 0 )
	{
		VO_S64 blocknumber;

		Read_uint( (VO_S32)pInfo->size , &blocknumber );

		pElement->CueBlockNumber = (VO_U32)blocknumber;
	}
	else
	{
		return ERROR_SKIP;
	}

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::SkipClusterSection(IDAndSizeInfo * pInfo , VO_VOID * pParam)
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;	

	list_T<SeekHeadElementStruct *>::iterator iter;
	for(iter=pSegInfo->SeekHeadElementEntryListHeader.begin(); iter!=pSegInfo->SeekHeadElementEntryListHeader.end(); ++iter)
	{
		SeekHeadElementStruct * pSeekHead = (SeekHeadElementStruct *)(*iter);
		
		if( pSegInfo->CuesCuePointEntryListHeader.empty() && pSeekHead->SeekID == Cues )
		{
			if(m_pFileChunk->FLocate( pSeekHead->SeekPos + pSegInfo->SegmentBeginPos ))
				return ERROR_NONE;
			continue;
		}
		else if( pSegInfo->SeekHeadElementEntryListHeader.empty() && pSeekHead->SeekID == SeekHead )
		{
			if(m_pFileChunk->FLocate( pSeekHead->SeekPos + pSegInfo->SegmentBeginPos ))
				return ERROR_NONE;
			continue;
		}
		else if( pSegInfo->TracksTrackEntryListHeader.empty() && pSeekHead->SeekID == Tracks )
		{
			if(m_pFileChunk->FLocate( pSeekHead->SeekPos + pSegInfo->SegmentBeginPos ))
				return ERROR_NONE;
			continue;
		}
	}

	m_pFileChunk->FLocate( pSegInfo->SegmentEndPos );

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ParseClusterSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;
	VO_U32	nResult = ERROR_NONE;
	
	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_TIMECODE , pInfo->s_ID ) == 0 )
	{
		Read_uint( (VO_S32)pInfo->size , &m_nCurrectClusterTime );
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_SIMPLEBLOCK , pInfo->s_ID ) == 0 )
	{
		list_T<SampleElementStruct *> TempSampleElement;
		VOLOGI( "+CMKVAnalyze::ParseClusterSection--ProcessBlockSection %lu" , voOS_GetSysTime() );
		nResult = ProcessBlockSection(pInfo, &TempSampleElement);
		if(ERROR_NONE == nResult){
			nResult = ReconstructSampleElement(&TempSampleElement);
		}
		VOLOGI( "-CMKVAnalyze::ParseClusterSection--ProcessBlockSection %lu" , voOS_GetSysTime() );

	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP , pInfo->s_ID ) == 0 )
	{
		BlockGroupElementStruct BlockGroupElement;
		nResult = ReadSection(&CMKVAnalyze::ProcessBlockGroupSection, pInfo->size, &BlockGroupElement);
		VOLOGI( "+CMKVAnalyze::ProcessBlockGroupSection %lu" , voOS_GetSysTime() );
		if(ERROR_NONE == nResult){
			nResult = ReconstructBlockGroupElement(&BlockGroupElement);				
		}
		VOLOGI( "-CMKVAnalyze::ProcessBlockGroupSection %lu" , voOS_GetSysTime() );
		
	}
	else
	{
		nResult = ERROR_SKIP;
	}
	
	VOLOGI( "-CMKVAnalyze::MKVID_SEGMENT_CLUSTER %lu" , voOS_GetSysTime() );
	
	return nResult;
}

VO_U32 CMKVAnalyze::ReconstructSampleElement(list_T<SampleElementStruct *>* pSampleElement)
{
	MKVSegmentStruct * pSegInfo = GetSegmentInfo(0);
	SampleElementStruct* pSample = NULL;
	VO_U32 nResult = ERROR_NONE;
	
	if(pSampleElement->empty()){
		return nResult;
	}
	
	list_T<SampleElementStruct *>::iterator SampleElementIter;
	for(SampleElementIter=pSampleElement->begin(); SampleElementIter!=pSampleElement->end(); ++SampleElementIter)
	{
		pSample = (SampleElementStruct *)(*SampleElementIter);
		pSample->TimeCode = m_nCurrectClusterTime;
	}
	
	//if the track is rigested, call the notify function.
	if(!m_pSampleNotifyCenter->Notify(pSample->TrackNumber, (VO_PTR)pSampleElement, (VO_PTR)m_pFileChunk)){
		nResult = ERROR_STOP;
	}

	list_T<SampleElementStruct *>::iterator iter;
	for(iter=pSampleElement->begin(); iter!=pSampleElement->end(); ++iter)
	{
		SampleElementStruct* pSample = (SampleElementStruct *)(*iter);
		delete *iter;
	}
	pSampleElement->clear();

	VOLOGI( "ReconstructSampleList---size=%lu, tracknumber=%d, nResult=%d" , pSampleElement->size(), pSample->TrackNumber,nResult );
	return nResult;
}

VO_U32 CMKVAnalyze::ReconstructBlockGroupElement(BlockGroupElementStruct* pBlockGroupElement)
{
	MKVSegmentStruct * pSegInfo = GetSegmentInfo(0);
	VO_U32 nResult = ERROR_NONE;
	SampleElementStruct* pSample = NULL;
	
	VO_S64 index = 0;

	if(pBlockGroupElement->SampleElementList.empty()){
		return nResult;
	}
	
	list_T<SampleElementStruct *>::iterator SampleElementIter;
	for(SampleElementIter=pBlockGroupElement->SampleElementList.begin(); SampleElementIter!=pBlockGroupElement->SampleElementList.end(); ++SampleElementIter)
	{
		pSample = (SampleElementStruct *)(*SampleElementIter);
		pSample->bIsKeyFrame = ( !pBlockGroupElement->ReferenceTimecodec )? VO_TRUE : VO_FALSE;
		pSample->TimeCode = m_nCurrectClusterTime;
		pSample->RelativeTimeCode += pBlockGroupElement->BlockDuration * (VO_S64)index;
		pSample->duration = pBlockGroupElement->BlockDuration;
		
		index++;	
	}
	
	//if the track is rigested, call the notify function.
	if(!m_pSampleNotifyCenter->Notify(pSample->TrackNumber, (VO_PTR)&pBlockGroupElement->SampleElementList, (VO_PTR)m_pFileChunk)){
		nResult = ERROR_STOP;
	}
	
	VOLOGI( "ReconstructSampleList---size=%lu, tracknumber=%d, nResult=%d" , pBlockGroupElement->SampleElementList.size(), pSample->TrackNumber, nResult);
	return nResult;
}
/*
VO_U32 CMKVAnalyze::PostTheSampleInList(SampleListStruct*  pSampleList)
{
	VO_U32 nResult = ERROR_NONE;
	VO_U64 CusPos= m_pFileChunk->FGetFilePos();
	list_T<SampleElementStruct *>::iterator SampleElementIter;

	if(NULL == pSampleList){
		return nResult;
	}
	
	while(!pSampleList->SampleElementList.empty())
	{
		SampleElementIter = pSampleList->SampleElementList.begin();
		SampleElementStruct* pSample = (SampleElementStruct *)(*SampleElementIter);
		
		//if the track is rigested, call the notify function.
		if(m_pSampleNotifyCenter->IsTrackRegist(pSample->TrackNumber)){
			if(!m_pSampleNotifyCenter->Notify(pSample->TrackNumber, (VO_PTR)pSample, (VO_PTR)m_pFileChunk)){
				VOLOGE("Notify failed!---size=%d",pSampleList->SampleElementList.size());
				nResult = ERROR_STOP;
				break;
			}
		}
		pSampleList->SampleElementList.remove(SampleElementIter);
	}
	
	VOLOGI( "PostTheSampleInList---CusPos=%llu" , CusPos,pSampleList->TrackNumber);
	m_pFileChunk->FLocate(CusPos);

	return nResult;
}
*/
VO_U32 CMKVAnalyze::ProcessClusterSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	return (m_nAnalyzeMode & PROCESS_CLUSTER_FLAG) ? ParseClusterSection(pInfo , pParam) : SkipClusterSection(pInfo , pParam);
}


//get the size and data by embl
VO_BOOL CMKVAnalyze::Read_vint( VO_S64 * pResult )
{
	VO_BYTE sizeFlag;
	VO_U8 size = 1;
	VO_U8 retval;

	m_pFileChunk->FRead( &sizeFlag , 1 );

	retval = TranslateVINT( &sizeFlag , &size , pResult );

	if( retval )
	{
		VO_BYTE * pByte = (VO_BYTE*)new VO_BYTE[size];
		pByte[0] = sizeFlag;

		m_pFileChunk->FRead( pByte + 1 , size - 1 );

		TranslateVINT( pByte , &size , pResult );

		delete(pByte);
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::Read_svint( VO_S64 * pResult )
{
	VO_BYTE sizeFlag;
	VO_U8 size = 1;
	VO_U8 retval;

	m_pFileChunk->FRead( &sizeFlag , 1 );

	retval = TranslateVSINT( &sizeFlag , &size , pResult );

	if( retval )
	{
		VO_BYTE * pByte = (VO_BYTE*)new VO_BYTE[size];
		pByte[0] = sizeFlag;

		m_pFileChunk->FRead( pByte + 1 , size - 1 );

		TranslateVSINT( pByte , &size , pResult );

		delete(pByte);
	}

	return VO_TRUE;
}

//read unsigned unit
VO_BOOL CMKVAnalyze::Read_uint( VO_S32 readsize , VO_S64 * pResult )
{
	VO_PBYTE pData = (VO_BYTE*)new VO_BYTE[readsize];
	VO_PBYTE pPos = ( VO_PBYTE )pResult;
	*pResult = 0;

	m_pFileChunk->FRead( pData , readsize );

	for( VO_S32 i = 0 ; i < readsize ; i++ )
	{
		pPos[i] = pData[ readsize - i - 1 ];
	}

	delete(pData);

	return VO_TRUE;
}

//read singed unit
VO_BOOL CMKVAnalyze::Read_sint( VO_S32 readsize , VO_S64 * pResult )
{
	VO_PBYTE pData = (VO_BYTE*)new VO_BYTE[readsize];
	VO_PBYTE pPos = ( VO_PBYTE )pResult;
	*pResult = 0;

	m_pFileChunk->FRead( pData , readsize );

	if( pData[0] & 0x80 )
	{
		for( VO_U32 i = 0 ; i < sizeof( VO_S64 ) ; i++ )
		{
			if( i < (VO_U32)readsize )
				pPos[i] = pData[ readsize - i - 1 ];
			else
				pPos[i] = 0xff;
		}
	}
	else
	{
		for( VO_S32 i = 0 ; i < readsize ; i++ )
		{
			if( i < readsize )
				pPos[i] = pData[ readsize - i - 1 ];
		}
	}

	delete(pData);

	return VO_TRUE;
}



VO_BOOL CMKVAnalyze::ReadIDAndSize( IDAndSizeInfo * pInfo )
{
	VO_BYTE sizeFlag;
	VO_U8 size = 1;
	VO_S64 result;
	VO_U8 retval;
	VO_BOOL retread;

	//first read id
	retread = m_pFileChunk->FRead( &sizeFlag , 1 );
	if( retread == VO_FALSE || sizeFlag == 0 )
	{
		return VO_FALSE;
	}

	retval = TranslateVINT( &sizeFlag , &size , &result );

	pInfo->pID = (VO_BYTE*)new VO_BYTE[size];
	pInfo->s_ID = size;
	pInfo->pID[0] = sizeFlag;

	if( retval )
	{
		retread = m_pFileChunk->FRead( &(pInfo->pID[1]) , size - 1 );
		if( retread == VO_FALSE )
		{
			delete(pInfo->pID);
			pInfo->pID = NULL;
			return VO_FALSE;
		}
	}
	//

	//second read size
	retread = m_pFileChunk->FRead( &sizeFlag , 1 );
	if( retread == VO_FALSE || sizeFlag == 0xFF)
	{
		if (sizeFlag == 0xFF)
		{
			VOLOGE("MKV Error:not support undefined element size");
		}
		delete(pInfo->pID);
		pInfo->pID = NULL;
		return VO_FALSE;
	}

	size = 1;
	retval = TranslateVINT( &sizeFlag , &size , &result );

	if( retval )
	{
		VO_BYTE * pByte = (VO_BYTE*)new VO_BYTE[size];
		pByte[0] = sizeFlag;

		retread = m_pFileChunk->FRead( pByte + 1 , size - 1 );
		if( retread == VO_FALSE )
		{
			delete(pByte);
			delete(pInfo->pID);
			pInfo->pID = NULL;
			return VO_FALSE;
		}

		TranslateVINT( pByte , &size , &result );

		delete(pByte);
	}

	pInfo->size = result;
	//

	if( pInfo->s_ID == 2 && pInfo->pID[0] == 0 && pInfo->pID[1] == 0 && (VO_S32)pInfo->size == 0 )
	{
		delete(pInfo->pID);
		pInfo->pID = NULL;
		return VO_FALSE;
	}

	return VO_TRUE;

}

VO_U32 CMKVAnalyze::ProcessContentEncodings( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTENCODEING , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessContentEncoding , pInfo->size , pElement );
	}
	else
		return ERROR_SKIP;

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessContentEncoding( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTCOMPRESSION , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessContentCompression , pInfo->size , pElement );
	}
	else
		return ERROR_SKIP;

	return ERROR_NONE;
}

VO_U32 CMKVAnalyze::ProcessContentCompression( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTCOMPALGO , pInfo->s_ID ) == 0 )
	{
		VO_S64 algo;
		Read_uint( (VO_S32)pInfo->size , &algo );

		pElement->ContentCompAlgo = (VO_U32)algo;
	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTCOMPSETTINGS , pInfo->s_ID ) == 0 )
	{
		if( pInfo->size )
		{
			pElement->ContentCompSettingsSize = (VO_S32)pInfo->size;
			pElement->ptr_ContentCompSettings = (VO_PBYTE)new VO_PBYTE[(VO_U32)pInfo->size];
			m_pFileChunk->FRead( pElement->ptr_ContentCompSettings , (VO_U32)pInfo->size );
		}
	}
	else
		return ERROR_SKIP;

	return ERROR_NONE;
}


VO_VOID CMKVAnalyze::PrintMKVSegmentStruct(MKVSegmentStruct* info)
{
	VO_PTR	m_hMKV = NULL;
	VO_U32 len = 0;
	char	common_info[256];
	len = sizeof(common_info);
	memset(common_info, 0x0, len);
	

	if (!m_hMKV)
	{
		VO_FILE_SOURCE		fileSource;
		fileSource.nFlag	= VO_FILE_TYPE_NAME;
		fileSource.nOffset	= 0;
		fileSource.nLength	= 0xFFFFFFFF;
		fileSource.nMode	= VO_FILE_READ_WRITE;

#ifdef _WIN32
		fileSource.pSource = (VO_PTR)_T("G:\\Debug\\dumper_MKVSegmentStruct_Pc.txt");
#else
		fileSource.pSource = (VO_PTR)_T("/data/local/voOMXPlayer/dumper_MKVSegmentStruct_Dev.txt");
#endif
		m_hMKV = cmnFileOpen(&fileSource);	

		if(m_hMKV == NULL)
		{
			return;
		}

//		strcpy(common_info, "Idx		Real		TS			Size		Sync(0:I frame)\r\n");
//		cmnFileWrite(m_hVideoLogFile, common_info, strlen(common_info));

	}
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "Duration  %f\r\n", info->Duration);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "TimecodeScale  %lld\r\n", info->TimecodeScale);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "ClusterPos  %lld\r\n", info->ClusterPos);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "SegmentEndPos  %lld\r\n", info->SegmentEndPos);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "SegmentBeginPos  %lld	\r\n", info->SegmentBeginPos);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));


	list_T<SeekHeadElementStruct *>::iterator SeekHeadIter;
	for(SeekHeadIter=info->SeekHeadElementEntryListHeader.begin(); SeekHeadIter!=info->SeekHeadElementEntryListHeader.end(); ++SeekHeadIter)
	{
		SeekHeadElementStruct* SeekHead = (SeekHeadElementStruct *)(*SeekHeadIter);
		
		memset(common_info, 0x0, len);
		sprintf(common_info, "SeekID  %d	, SeekPos  %lld\r\n", SeekHead->SeekID, SeekHead->SeekPos);
		cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	}
	
	list_T<TracksTrackEntryStruct *>::iterator TrackIter;
	for(TrackIter=info->TracksTrackEntryListHeader.begin(); TrackIter!=info->TracksTrackEntryListHeader.end(); ++TrackIter)
	{
		TracksTrackEntryStruct* TrackInfo = (TracksTrackEntryStruct *)(*TrackIter);
		
		memset(common_info, 0x0, len);
		sprintf(common_info, "TrackNumber  %lu	, TrackType  %lu   ,DefaultDuration  %lld	, TrackTimecodeScale  %f\r\n", TrackInfo->TrackNumber, TrackInfo->TrackType, TrackInfo->DefaultDuration, TrackInfo->TrackTimecodeScale);
		cmnFileWrite(m_hMKV, common_info, strlen(common_info));

		if(TrackInfo->TrackType == 1)//video
		{
			memset(common_info, 0x0, len);		
			sprintf(common_info, "PixelWidth  %lu , PixelHeight  %lu	\r\n", TrackInfo->PixelWidth, TrackInfo->PixelHeight);		
			cmnFileWrite(m_hMKV, common_info, strlen(common_info));
		}
		else if(TrackInfo->TrackType == 2)//audio
		{
			memset(common_info, 0x0, len);				
			sprintf(common_info, "SampleFreq  %f , Channels  %lu	,BitDepth  %lu	\r\n", TrackInfo->SampleFreq, TrackInfo->Channels, TrackInfo->BitDepth);		
			cmnFileWrite(m_hMKV, common_info, strlen(common_info));
		}
	}
	
	list_T<CuesCuePointStruct *>::iterator CuesIter;
	for(CuesIter=info->CuesCuePointEntryListHeader.begin(); CuesIter!=info->CuesCuePointEntryListHeader.end(); ++CuesIter)
	{
		CuesCuePointStruct* CuesInfo = (CuesCuePointStruct *)(*CuesIter);
		
		list_T<CuesCuePointTrackPositionsStruct *>::iterator iter = CuesInfo->TrackPositionsHeader.begin();
		CuesCuePointTrackPositionsStruct* CuesCue = (CuesCuePointTrackPositionsStruct*)(*iter);
		memset(common_info, 0x0, len);
		sprintf(common_info, "CueTime  %lld	, CueClusterPosition  %lld   ,CueBlockNumber  %lu	, CueTrack  %lu\r\n", CuesInfo->CueTime, CuesCue->CueClusterPosition, CuesCue->CueBlockNumber,CuesCue->CueTrack);
		cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	}

	cmnFileClose(m_hMKV);
	m_hMKV = NULL;
}

VO_VOID CMKVAnalyze::EventNotify(IDAndSizeInfo * pInfo)
{
	if(!m_pNotifyCenter){
		return;
	}
		
	if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS , pInfo->s_ID ) == 0 )
	{		
		MKVSegmentStruct * ptr_seg = GetSegmentInfo(0);

		if(ptr_seg){
			m_pNotifyCenter->EventNotify(TRACK, (VO_PTR)&ptr_seg->TracksTrackEntryListHeader, NULL);
		}

	}
	else if( memcmp( pInfo->pID , (VO_PTR)MKVID_SEGMENT , pInfo->s_ID ) == 0  )
	{
		MKVSegmentStruct * ptr_seg = GetSegmentInfo(0);
		
		if(!ptr_seg){
			return;
		}

		//generate the duration if we can't can't get it from file.
		if( ptr_seg->Duration == 0 )
		{
			ptr_seg->Duration = GetLastClusterTime();
		}

		m_pNotifyCenter->EventNotify(SEGMENT, (VO_PTR)ptr_seg, NULL);
	}
}



TracksTrackEntryStruct * CMKVAnalyze::GetBestTrackInfo( VO_U32 tracktype )
{
	MKVSegmentStruct * ptr_seg = GetSegmentInfo(0);
	TracksTrackEntryStruct * pTempSelect = NULL;
	TracksTrackEntryStruct*  pTrackEntry;
	list_T<TracksTrackEntryStruct *>::iterator iter;
	
	for(iter=ptr_seg->TracksTrackEntryListHeader.begin(); iter!=ptr_seg->TracksTrackEntryListHeader.end(); ++iter)
	{
		pTrackEntry = (TracksTrackEntryStruct *)(*iter);
		
		if( pTrackEntry->TrackType == tracktype )// 1 means audio
		{
			if( pTempSelect == NULL )
			{
				if( pTrackEntry->IsEnabled )
				{
					pTempSelect = pTrackEntry;

					if( pTempSelect->IsDefault || pTempSelect->IsForced )
						return pTempSelect;
				}
			}
			else
			{
				if( pTrackEntry->IsDefault || pTrackEntry->IsForced )
				{
					pTempSelect = pTrackEntry;
					return pTempSelect;
				}
			}
		}
	
	}


	return pTempSelect;

}

TracksTrackEntryStruct * CMKVAnalyze::GetTrackInfoByNumber( VO_U32 TrackNumber )
{
	MKVSegmentStruct * ptr_seg = GetSegmentInfo(0);
	TracksTrackEntryStruct * pTempSelect = NULL;
	TracksTrackEntryStruct*  pTrackEntry;
	list_T<TracksTrackEntryStruct *>::iterator iter;
	
	for(iter=ptr_seg->TracksTrackEntryListHeader.begin(); iter!=ptr_seg->TracksTrackEntryListHeader.end(); ++iter)
	{
		pTrackEntry = (TracksTrackEntryStruct *)(*iter);
		
		if( pTrackEntry->TrackNumber == TrackNumber )// 1 means audio
		{
			if( pTempSelect == NULL )
			{
				if( pTrackEntry->IsEnabled )
				{
					pTempSelect = pTrackEntry;

					if( pTempSelect->IsDefault || pTempSelect->IsForced )
						return pTempSelect;
				}
			}
			else
			{
				if( pTrackEntry->IsDefault || pTrackEntry->IsForced )
				{
					pTempSelect = pTrackEntry;
					return pTempSelect;
				}
			}
		}
	
	}


	return pTempSelect;
}


