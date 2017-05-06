
#include "CMKVTrack.h"
#include "CMKVFileReader.h"
#include "volog.h"
#include "cmnFile.h"
#include "ASSERT.h"
//#define MAX_READ_BUFFER_RETRY_COUNT 3 ///< can be changed due to the peformance,3 for pentech project

SampleList::SampleList()
:m_nMaxSampleSize(0x0000ffff)
,m_nMaxSampleNumber(10)
{
	;
}

SampleList::~SampleList()
{
	Flush();
}

VO_BOOL SampleList::CalcTheTimestamp(VO_SOURCE_SAMPLE_STRUCT* pSampleStruct)
{
	list_T<VO_SOURCE_SAMPLE_STRUCT *>::iterator iter;
	VO_SOURCE_SAMPLE_STRUCT* pTmp_Sample_struct = NULL;
	VO_U32	number = m_nUnusedSampleNumber;

	iter--;
	pTmp_Sample_struct = (VO_SOURCE_SAMPLE_STRUCT *)(*iter);

	VO_S64	duration = pSampleStruct->Sample->Time - pTmp_Sample_struct->Sample->Time;

	iter= m_pSampleList.end();
	do
	{
		m_nUnusedSampleNumber--;
		iter--;
		pTmp_Sample_struct = (VO_SOURCE_SAMPLE_STRUCT *)(*iter);
		
		if(pTmp_Sample_struct->InUse){
			break;
		}

		if(0xffffffff == m_nUnusedSampleNumber){
//			MK_ASSERT(0);
			VOLOGE("CalcTheTimestamp---error");
		}

		pTmp_Sample_struct->Sample->Time = (VO_S64)(m_nUnusedSampleNumber/number)*duration;
		pTmp_Sample_struct->InUse = VO_TRUE;
		
	}while(iter!=m_pSampleList.begin());


// 	MK_ASSERT(0 == m_nUnusedSampleNumber);	
	VOLOGE("CalcTheTimestamp---m_nUnusedSampleNumber=%lu",m_nUnusedSampleNumber);
	return VO_TRUE;
}

VO_BOOL SampleList::InsertSampleAndSortbyTime(VO_SOURCE_SAMPLE_STRUCT* pSampleStruct)
{
	list_T<VO_SOURCE_SAMPLE_STRUCT *>::iterator iter;
	VO_SOURCE_SAMPLE_STRUCT* pTmp_Sample_struct = (VO_SOURCE_SAMPLE_STRUCT *)(*iter);

	iter= --m_pSampleList.end();
	pTmp_Sample_struct = (VO_SOURCE_SAMPLE_STRUCT *)(*iter);

	// normal 
	if(pSampleStruct->Sample->Time > pTmp_Sample_struct->Sample->Time)
	{
		CalcTheTimestamp(pSampleStruct);
		m_pSampleList.push_back(pSampleStruct);	
	}
	// same timestamp
	else if(pSampleStruct->Sample->Time == pTmp_Sample_struct->Sample->Time)
	{
		pSampleStruct->InUse = VO_FALSE;
		pTmp_Sample_struct->InUse = VO_FALSE;		
		m_nUnusedSampleNumber++;
		m_pSampleList.push_back(pSampleStruct);	
	}
	// need to adjust the position by timestamp
	else
	{
	
		iter= m_pSampleList.end();
		do
		{
			iter--;
			pTmp_Sample_struct = (VO_SOURCE_SAMPLE_STRUCT *)(*iter);

			if(pSampleStruct->Sample->Time < pTmp_Sample_struct->Sample->Time){
					
				m_pSampleList.insert(iter, pSampleStruct);
				break;
			}
			
		}while(iter!=m_pSampleList.begin());


	}

	
	
	return VO_TRUE;
}

VO_BOOL SampleList::AddSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_SOURCE_SAMPLE_STRUCT* pSampleStruct = new VO_SOURCE_SAMPLE_STRUCT;
	pSampleStruct->Sample = pSample;
	pSampleStruct->InUse = VO_TRUE;

	if(m_pSampleList.empty())
	{
		m_pSampleList.push_back(pSampleStruct);		
	}
	else
	{
		InsertSampleAndSortbyTime(pSampleStruct);
	}

	return VO_TRUE;
}

VO_BOOL SampleList::IsSpaceLeft(VO_U64 nSpace)
{	
	VO_U64	totalSize = 0;
	
	list_T<VO_SOURCE_SAMPLE_STRUCT *>::iterator iter;
	for(iter=m_pSampleList.begin(); iter!=m_pSampleList.end(); ++iter)
	{
		VO_SOURCE_SAMPLE_STRUCT* pSamplestruct = (VO_SOURCE_SAMPLE_STRUCT *)(*iter);
		totalSize += (VO_U64)(pSamplestruct->Sample->Size & 0x7fffffff);
	}	

	VOLOGI("IsSpaceLeft---totalSize=%llu, m_nMaxSampleSize = %llu,nSpace=%llu", totalSize, m_nMaxSampleSize,nSpace);
	
	if((totalSize + nSpace) >= m_nMaxSampleSize){
		return VO_FALSE;
	}
	
	return VO_TRUE;
}

VO_BOOL SampleList::IsReachTheMaxFrameNumber()
{
	VO_U32 totalNumber = m_pSampleList.size();
	VO_BOOL bResult = VO_FALSE;
	
	if(totalNumber >= m_nMaxSampleNumber){
		bResult = VO_TRUE;
	}

	VOLOGI("IsReachTheMaxFrameNumber---totalNumber = %lu, m_nMaxSampleNumber = %lu, bResult=%d", totalNumber, m_nMaxSampleNumber,bResult);
	
	return bResult;
}

VO_U32	SampleList::GetSampleByTime(VO_S64 Time, VO_SOURCE_SAMPLE* pSample)
{
	VO_U32		nResult = VO_ERR_SOURCE_END;
	while (0 < m_pSampleList.size()) 
	{	
		VO_SOURCE_SAMPLE_STRUCT *pElement = m_pSampleList.front();
		if(!pElement->InUse){
			nResult = VO_ERR_SOURCE_ERRORDATA;
			break;
		}
		
		if(pElement->Sample->Time >= Time){
			nResult = VO_ERR_SOURCE_OK;
			pSample->Buffer = pElement->Sample->Buffer;
			pSample->Size = pElement->Sample->Size;
			pSample->Time = pElement->Sample->Time;
			pSample->Duration = pElement->Sample->Duration;
			pSample->Flag = pElement->Sample->Flag;
			break;
		}
		
		delete pElement->Sample->Buffer;
		m_pSampleList.pop_front();
	}

	return nResult;
}

VO_U32	SampleList::GetSampleByIndex(VO_U32 Index, VO_SOURCE_SAMPLE* pSample)
{
	VO_U32		nResult = VO_ERR_SOURCE_END;
	VO_U32 		nCount = 0;
	
	while (0 < m_pSampleList.size()) 
	{		
		VO_SOURCE_SAMPLE_STRUCT *pElement = m_pSampleList.front();
		
		if(!pElement->InUse){
			nResult = VO_ERR_SOURCE_ERRORDATA;
			break;
		}
		
		if(nCount >= Index){
			nResult = VO_ERR_SOURCE_OK;
			pSample->Buffer = pElement->Sample->Buffer;
			pSample->Size = pElement->Sample->Size;
			pSample->Time = pElement->Sample->Time;
			pSample->Duration = pElement->Sample->Duration;
			pSample->Flag = pElement->Sample->Flag;
			break;
		}
		nCount++;
		delete pElement->Sample->Buffer;
		m_pSampleList.pop_front();
	}

	return nResult;
		
}
/*
VO_U32	SampleList::GetBackSampleCount()
{
	VO_U32	nCount = 0;
	nCount = m_pBackSampleList.size();
	return nCount;

}

VO_U32	SampleList::MoveBackSampleToSampleList(VO_PTR pFileIO)
{
	VO_U32		nResult = VO_ERR_SOURCE_ERRORDATA;
	
	if(NULL == pFileIO){
		return nResult;
	}
	CGFileChunk* pFileChunk = (CGFileChunk*)pFileIO;
	VO_U64	nPos = pFileChunk->FGetFilePos();

	while(!m_pBackSampleList.empty()) 
	{	
		if(IsNeedToStop()){
			break;
		}

		VO_SOURCE_BACKSAMPLE *pTempSample= m_pBackSampleList.front();
		VO_SOURCE_SAMPLE*	pSample = NULL;

		pSample = pTempSample->Sample;
		pSample->Buffer= (VO_PBYTE)new VO_CHAR[pSample->Size];
		
		//get the frame data.
		pFileChunk->FLocate(pTempSample->FilePos);
		if(!pFileChunk->FRead(pSample->Buffer, pSample->Size)){
			delete pSample->Buffer;
			return nResult;
		}

		nResult = VO_ERR_SOURCE_OK;

		m_pSampleList.push_back(pSample);		
		m_pBackSampleList.pop_front();
	}

	pFileChunk->FLocate(nPos);

	return nResult;
		
}
*/
VO_U32	SampleList::GetSampleCount()
{
	VO_U32	nCount = 0;
	nCount = m_pSampleList.size();
	return nCount;
}

VO_VOID SampleList::Flush()
{
	VOLOGI("SampleList----Flush" );

	list_T<VO_SOURCE_SAMPLE_STRUCT *>::iterator iter;
	for(iter=m_pSampleList.begin(); iter!=m_pSampleList.end(); ++iter)
	{
		VO_SOURCE_SAMPLE_STRUCT* pElement = (VO_SOURCE_SAMPLE_STRUCT *)(*iter);
		delete pElement->Sample->Buffer;
		delete *iter;
	}
	m_pSampleList.clear();
/*
	list_T<VO_SOURCE_BACKSAMPLE *>::iterator backiter;
	for(backiter=m_pBackSampleList.begin(); backiter!=m_pBackSampleList.end(); ++backiter)
	{
		VO_SOURCE_BACKSAMPLE* pSample = (VO_SOURCE_BACKSAMPLE *)(*backiter);
		delete pSample->Sample->Buffer;
		delete *backiter;
	}
	m_pBackSampleList.clear();
*/	
}


CMKVTrack::CMKVTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, TracksTrackEntryStruct * pTrackInfo , VO_MEM_OPERATOR* pMemOp)
: CBaseTrack( nType , btStreamNum , dwDuration , pMemOp )
,m_pTrackInfo( pTrackInfo )
,m_pSampleList(NULL)
,m_bParseForSelf(VO_FALSE)
,m_pParser(NULL)
,m_hMKV(NULL)
,m_hMKVBuffer(NULL)
{
	m_pSampleList = new SampleList();

}

CMKVTrack::~CMKVTrack()
{
	if(m_pSampleList){
		delete m_pSampleList;
		m_pSampleList= NULL;
	}		

	if(m_hMKV){
		cmnFileClose(m_hMKV);
		m_hMKV = NULL;
	}

	
	if(m_hMKVBuffer){
		cmnFileClose(m_hMKVBuffer);
		m_hMKVBuffer = NULL;
	}
	
}

VO_BOOL CMKVTrack::SetDataParser(Parser* pParser)
{
	if(!pParser){
		return VO_FALSE;
	}
	VOLOGI("SetDataParser---TrackNumber=%lu, TrackType=%lu", m_pTrackInfo->TrackNumber, m_pTrackInfo->TrackType);
	
	m_pParser = pParser;

	m_pParser->AddSampleNotifyCallback(m_pTrackInfo->TrackNumber, (VO_PTR)this, (pOnNotify)SampleNotify);
	return VO_TRUE;
}

VO_BOOL CMKVTrack::RemoveDataParser()
{
	m_pParser = NULL;

	m_pParser->RemoveSampleNotifyCallback((TrackType)m_pTrackInfo->TrackType);

	return VO_TRUE;
}

VO_BOOL CMKVTrack::SetNotifyFuc(VO_PTR pOnNotifyUser, pOnNotify OnNotify)
{
	m_nIsNeedNewParserNotify.OnNotify = OnNotify;
	m_nIsNeedNewParserNotify.pOnNotifyUser = pOnNotifyUser;
	return VO_TRUE;
}

VO_VOID CMKVTrack::Flush()
{
	VOLOGI("Flush");

	m_pSampleList->Flush();
}

//we can do something to cut down the time still
VO_U32 CMKVTrack::SetPosN(VO_S64* pPos)
{
	VO_U32 result = VO_ERR_SOURCE_OK;

	VO_SOURCE_SAMPLE tempSample;
	VOLOGI("SetPosN---start---pPos=%lld, tracknumber = %lu",*pPos, m_pTrackInfo->TrackNumber);

	while(1)
	{
		result = m_pSampleList->GetSampleByTime(*pPos, &tempSample);
/*		
		//push the back sample to samplelist if needed
		if(!IsNeedToStop()){
			m_pSampleList->MoveBackSampleToSampleList(m_pParser->GetFileIO());
		}
*/
		//generate the sample if needed
		if(CanGenerateMoreSample()){
			TrackGenerateIndex();
		}

		//if we haven't get the sample we wanted
		if(VO_ERR_SOURCE_OK != result){ 
			//and there is no more sample left,
			if(!m_pSampleList->GetSampleCount()){
				result = VO_ERR_SOURCE_END;
			}
		}
	}
	
	VOLOGI("SetPosN---end---pPos=%lld, tracknumber = %lu",*pPos, m_pTrackInfo->TrackNumber);
	return result;
}


VO_U32 CMKVTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 result = VO_ERR_SOURCE_OK;
	VOLOGI("GetSampleN--Start--time=%lld, TrackType=%lu",pSample->Time, m_pTrackInfo->TrackType);

	//get the sample which you wanted	
	static VO_U32 index = 0;
	if(m_pTrackInfo->TrackType == Audio_Type){
		result = m_pSampleList->GetSampleByIndex(index, pSample);
		index = 1;
	}else{
		result = m_pSampleList->GetSampleByTime(pSample->Time, pSample);
	}
/*
	//push the back sample to samplelist if needed
	if(!IsNeedToStop()){
		m_pSampleList->MoveBackSampleToSampleList(m_pParser->GetFileIO());
	}
*/
	//generate the sample if needed
	if(CanGenerateMoreSample()){
		TrackGenerateIndex();
	}

	//if we haven't get the sample we wanted
	if(VO_ERR_SOURCE_OK != result){ 
		//and there is no more sample left,
		if(!m_pSampleList->GetSampleCount()){
			result = VO_ERR_SOURCE_END;
		}
	}

	

	//dump the info and data.
	PrintMKVTrackSample(pSample);
	PrintMKVTrackBuffer(pSample);

	VOLOGI("GetSampleN--End--time=%lld, TrackType=%lu,result=%llu",pSample->Time, m_pTrackInfo->TrackType, result);
	
	return result;
}

VO_U32 CMKVTrack::Start()
{
	VO_U32 result = 0;
	VOLOGI("Start----m_nType=%lu",m_nType);
	
	((CMKVAnalyze*)m_pParser)->Start();
	return result;
}

VO_U32 CMKVTrack::MoveTo(VO_S64 llTimeStamp)
{
	VO_U32 result = 0;
	VOLOGI("MoveTo----llTimeStamp=%lld, m_nType=%lu",llTimeStamp,m_nType );

	Flush();

	result = ((CMKVAnalyze*)m_pParser)->MoveTo(llTimeStamp);
	return result;
}

VO_U32 CMKVTrack::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	VO_U32 result = 0;
	VOLOGI("GetNearKeyFrame---llTime=%lld, pllPreviousKeyframeTime=%lld,pllNextKeyframeTime=%lld",llTime,*pllPreviousKeyframeTime,*pllNextKeyframeTime);
	
	result = m_pParser->GetNearKeyFrame(llTime, pllPreviousKeyframeTime, pllNextKeyframeTime);

	return result;
}

VO_BOOL CMKVTrack::SampleNotify(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam )
{
	VO_BOOL bResult = VO_TRUE;
	
	if(NULL == pUser){
		return VO_FALSE;
	}

	if(NULL == wParam){
		return VO_FALSE;
	}
	
	if(NULL == lParam){
		return VO_FALSE;
	}


	CMKVTrack* pTrack = (CMKVTrack*)pUser;
	list_T<SampleElementStruct *>* pSampleElementList = (list_T<SampleElementStruct *> *)wParam;
	CGFileChunk* pFileChunk = (CGFileChunk*)(lParam);
	VO_U64	nPos = pFileChunk->FGetFilePos();

		
	list_T<SampleElementStruct *>::iterator SampleElementIter;

	VO_U64	totalSize = 0;
	for(SampleElementIter=pSampleElementList->begin(); SampleElementIter!=pSampleElementList->end(); ++SampleElementIter)
	{
		SampleElementStruct* pSampleElement = (SampleElementStruct *)(*SampleElementIter);
		totalSize += (VO_U64)(pSampleElement->framesize & 0x7fffffff);
	}	

	
	//check whether there is space to generate sample
	if(!pTrack->CanGenerateMoreSample(totalSize)){
		//check weather we need to create a new parser.
		if(!pTrack->GetParseForSelf() &&
			pTrack->m_nIsNeedNewParserNotify.OnNotify(pTrack->m_nIsNeedNewParserNotify.pOnNotifyUser, NULL, NULL))
		{

			pTrack->m_pParser->RemoveSampleNotifyCallback((TrackType)pTrack->m_pTrackInfo->TrackNumber);
			pFileChunk->SetShareFileHandle(VO_TRUE);
			
			CMKVAnalyze* pAnalyze = (CMKVAnalyze*)pTrack->m_pParser;
			CMKVAnalyze* pDataParser = new CMKVAnalyze(*pAnalyze);
			pTrack->SetDataParser((Parser*)pDataParser);
			
			pTrack->SetParseForSelf(VO_TRUE);
		}

		
		return VO_FALSE;
	}

	//generate the sample
	while(!pSampleElementList->empty())
	{
		SampleElementIter = pSampleElementList->begin();
		SampleElementStruct* pMKVFrame = (SampleElementStruct *)(*SampleElementIter);


		VO_SOURCE_SAMPLE* pSample = new VO_SOURCE_SAMPLE;
		memset(pSample, 0x0, sizeof(VO_SOURCE_SAMPLE));
		VO_U32	nSize = (VO_U32)pMKVFrame->framesize;
		VO_PBYTE pBuffer = NULL;

		pBuffer = (VO_PBYTE)new VO_CHAR[(int)pMKVFrame->framesize];
		
		//get the frame data.
		pFileChunk->FLocate(pMKVFrame->filepos);
		if(!pFileChunk->FRead(pBuffer, nSize)){
			delete pSample;
			delete pBuffer;
			bResult = VO_FALSE;
			break;
		}
	
		pSample->Buffer = pBuffer;
		pSample->Size = (VO_U32)(pMKVFrame->bIsKeyFrame? (pMKVFrame->framesize|0x80000000) : pMKVFrame->framesize);
		pSample->Time = pMKVFrame->RelativeTimeCode + pMKVFrame->TimeCode;
		pSample->Duration = (VO_U32)pMKVFrame->duration;
		pSample->Flag = 0;
	
		VOLOGI("SampleNotify---Size=%lu, Time=%lld, filepos=%llu", pSample->Size & 0x7fffffff, pSample->Time, pMKVFrame->filepos);
		VOLOGI("SampleNotify---TrackNumber=%lu, TrackType=%lu", pTrack->m_pTrackInfo->TrackNumber, pTrack->m_pTrackInfo->TrackType);
		pTrack->m_pSampleList->AddSample(pSample);
		
		pSampleElementList->remove(SampleElementIter);

		//	pTrack->PrintMKVTrackSample(pSample);
		//	pTrack->PrintMKVTrackBuffer(pSample);
		
	}

	pFileChunk->FLocate(nPos);



	return bResult;
}

VO_BOOL CMKVTrack::CanGenerateMoreSample(VO_U64 nSpace)
{
	VO_BOOL bResult = VO_FALSE;
	bResult = m_pSampleList->IsSpaceLeft(nSpace);
	
	VOLOGI("CanGenerateMoreSample---bResult=%ld",bResult);

	return bResult;
}

VO_BOOL CMKVTrack::IsNeedToCreateNewDataParser()
{
	VO_BOOL bResult = VO_FALSE;
	
	if(m_bParseForSelf){
		return bResult;
	}

	if(0 != m_pSampleList->GetSampleCount()){
		bResult = VO_TRUE;
	}
		
	VOLOGI("IsNeedToCreateNewDataParser---bResult=%ld",bResult);
	return bResult;
}

VO_BOOL CMKVTrack::TrackGenerateIndex()
{
/*
	CvoFileDataParser* pDataParser = m_pDataParser ? m_pDataParser : m_pStreamFileReader->GetFileDataParserPtr();
	VO_BOOL bNoEnd = TrackGenerateIndexB(pDataParser);
	if(m_pStreamFileIndex->IsNull() && bNoEnd)	//track buffer null but file parse not end, so need new data parser to parse
	{
		VOLOGI("tracks force to use each data parser!!");

		if(!m_pDataParser)
			m_pStreamFileReader->OnNewDataParser(this, &m_pDataParser);

		bNoEnd = TrackGenerateIndexB(m_pDataParser);
	}
*/	
	VO_U32	nResult = ERROR_NONE;
	VOLOGI("TrackGenerateIndex---start");

	m_pParser->SetMode(PROCESS_PARSE_MODE);

	while(ERROR_NONE == nResult || ERROR_SKIP == nResult)
	{
		nResult = m_pParser->DataParse();
	}
	VOLOGI("TrackGenerateIndex---end---nResult=%lu",nResult);
	return (ERROR_EOS == nResult)?VO_FALSE:VO_TRUE;

}


VO_VOID CMKVTrack::PrintMKVTrackSample(VO_SOURCE_SAMPLE* pSample)
{
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

		if(Video_Type == m_pTrackInfo->TrackType){

		}else if(Audio_Type == m_pTrackInfo->TrackType){

		}

#ifdef _WIN32
		if(Video_Type == m_pTrackInfo->TrackType){
			fileSource.pSource = (VO_PTR)_T("G:\\Debug\\dumper_MKVTrackSample_Video.txt");

		}else if(Audio_Type == m_pTrackInfo->TrackType){
			fileSource.pSource = (VO_PTR)_T("G:\\Debug\\dumper_MKVTrackSample_Audio.txt");
		}

#else
		if(Video_Type == m_pTrackInfo->TrackType){
			fileSource.pSource = (VO_PTR)_T("/data/local/voOMXPlayer/dumper_MKVTrackSample_Video.txt");

		}else if(Audio_Type == m_pTrackInfo->TrackType){
			fileSource.pSource = (VO_PTR)_T("/data/local/voOMXPlayer/dumper_MKVTrackSample_Audio.txt");
		}
#endif
		m_hMKV = cmnFileOpen(&fileSource);	

		if(m_hMKV == NULL)
		{
			return;
		}

//		strcpy(common_info, "Idx		Real		TS			Size		Sync(0:I frame)\r\n");
//		cmnFileWrite(m_hVideoLogFile, common_info, strlen(common_info));

		memset(common_info, 0x0, len);
		sprintf(common_info, "Duration	%lu, ", pSample->Duration);
		cmnFileWrite(m_hMKV, common_info, strlen(common_info));
		
		memset(common_info, 0x0, len);
		sprintf(common_info, "TrackNumber	%lu, \r\n", m_pTrackInfo->TrackNumber);
		cmnFileWrite(m_hMKV, common_info, strlen(common_info));

	}
	
	char* frame = (pSample->Size & 0x80000000)?"I": "P";

	memset(common_info, 0x0, len);
	sprintf(common_info, "%s frame, ", frame);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));


	memset(common_info, 0x0, len);
	sprintf(common_info, "timecode  %lld, ", pSample->Time);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "size  %lu \r\n", (pSample->Size & 0x7fffffff));
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
//	cmnFileClose(m_hMKV);
//	m_hMKV = NULL;
}



VO_VOID CMKVTrack::PrintMKVTrackBuffer(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 len = 0;
	char	common_info[256];
	len = sizeof(common_info);
	memset(common_info, 0x0, len);
	

	if (!m_hMKVBuffer)
	{
		VO_FILE_SOURCE		fileSource;
		fileSource.nFlag	= VO_FILE_TYPE_NAME;
		fileSource.nOffset	= 0;
		fileSource.nLength	= 0xFFFFFFFF;
		fileSource.nMode	= VO_FILE_READ_WRITE;

		if(Video_Type == m_pTrackInfo->TrackType){

		}else if(Audio_Type == m_pTrackInfo->TrackType){

		}

#ifdef _WIN32
		if(Video_Type == m_pTrackInfo->TrackType){
			fileSource.pSource = (VO_PTR)_T("G:\\Debug\\dumper_MKVTrackBuffer_Video.txt");

		}else if(Audio_Type == m_pTrackInfo->TrackType){
			fileSource.pSource = (VO_PTR)_T("G:\\Debug\\dumper_MKVTrackBuffer_Audio.txt");
		}

#else
		if(Video_Type == m_pTrackInfo->TrackType){
			fileSource.pSource = (VO_PTR)_T("/data/local/voOMXPlayer/dumper_MKVTrackBuffer_Video.txt");

		}else if(Audio_Type == m_pTrackInfo->TrackType){
			fileSource.pSource = (VO_PTR)_T("/data/local/voOMXPlayer/dumper_MKVTrackBuffer_Audio.txt");
		}
#endif
		m_hMKVBuffer = cmnFileOpen(&fileSource);	

		if(m_hMKVBuffer == NULL)
		{
			return;
		}
		VO_CODECBUFFER pHeadData;
		GetHeadData(&pHeadData);
		if(0 != pHeadData.Length){
			cmnFileWrite(m_hMKVBuffer, pHeadData.Buffer, pHeadData.Length);
		}

	}
	
	cmnFileWrite(m_hMKVBuffer, pSample->Buffer, pSample->Size & 0x7fffffff);
	
}

