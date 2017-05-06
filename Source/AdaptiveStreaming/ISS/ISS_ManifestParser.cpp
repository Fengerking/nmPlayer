/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		ISS_ManifestParser.cpp

	Contains:	ISS_ManifestParser class file

	Written by:	Aiven

	Change History (most recent first):
	2012-08-14		Aiven			Create file

*******************************************************************************/
#include "ISS_ManifestParser.h"

#include "voOSFunc.h"
#include "voLog.h"
#include "CCodeCC.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#define be16toh(x) 	(( x & 0xff00) >> 8 | ( x & 0xff) << 8)
const int AAC_SampRateTab[12] = {
	96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};

/*
//void _ATOU64(VO_CHAR *a,VO_U64 &r)
#define _ATOU64(a,r)  \
{\
	int size = strlen(a);\
	VO_U64 ix = 1; \
	r = 0; \
	while(size) \
	{ \
		char x = a[size -1];\
		r += atoi(&x) * ix;\
		ix *= 10; \
		size --;\
	}\
}
*/
void _ATOU64(VO_CHAR *a,VO_U64 &r)
//#define _ATOU64(a,r)
{\
 VO_U64 time = 0 ;\
 char * b = a; \
 b = a;\
 while(*b != '\0')\
 {\
  char c = *b;\
  time *= 10; \
  int x = atoi(&c);\
  time += x;\
  b++;\
 }\
 r = time;\
}

#ifdef WIN32 
#define _ATO64(a) _atoi64(a) 
#elif defined LINUX || defined _IOS
#define _ATO64(a) atoll(a) 

#endif 

char* SearchLastString(char* Source, char* Desitiation) 
{
	if(!Source || !Desitiation){
		return NULL;
	}

	int i, d_len = strlen(Desitiation), end = strlen(Source) - d_len;
	char* last_index = NULL;
	char* pTmpStart = Source+end;

	for (i = 0; i <= end; i++)
	{
		if (!strncmp(Desitiation, pTmpStart - i, d_len)){
			last_index = pTmpStart - i;
			break;
		}
	}

    return last_index;
}


ISS_ManifestParser::ISS_ManifestParser()
:m_pXmlLoad(NULL)
,m_pBuffer(NULL)
{
	VOLOGI("ISS_ManifestParser");
	m_pXmlLoad = new CXMLLoad;
}

ISS_ManifestParser::~ISS_ManifestParser()
{
	VOLOGI("~ISS_ManifestParser");

	Uninit();

	if(m_pXmlLoad){
		delete m_pXmlLoad;
		m_pXmlLoad = NULL;
	}

	if(m_pBuffer){
		delete[] m_pBuffer;
		m_pBuffer = NULL;

	}
}

VO_U32 ISS_ManifestParser::Init()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	m_pXmlLoad->OpenParser();

	return nResult;
}

VO_U32 ISS_ManifestParser::Uninit()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	m_pXmlLoad->CloseParser();

	return nResult;
}

VO_VOID ISS_ManifestParser::ConvertUnicodeToUTF8( VO_PBYTE pBuffer,  VO_U32 uSize)
{
	m_uSize =uSize; 

	if(m_pBuffer){
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
	
	m_pBuffer = new VO_CHAR[m_uSize+1];
	memcpy(m_pBuffer, (VO_CHAR*)pBuffer, m_uSize);
	m_pBuffer[m_uSize] = '\0';

	VO_BOOL neetoconvert = VO_FALSE;
/*
	if( m_pBuffer[0] == (VO_CHAR)0xff && m_pBuffer[1] == (VO_CHAR)0xfe){
		neetoconvert = VO_TRUE;
	}else
*/
	{
		VO_PBYTE tmpStartBuffer = NULL;
		VO_CHAR* Start = (VO_CHAR*)"<";
		tmpStartBuffer = (VO_PBYTE)strstr((char*)pBuffer, (char*)Start);
		if(tmpStartBuffer[0] == (VO_CHAR)0x3C && tmpStartBuffer[1] == (VO_CHAR)0x00){
			neetoconvert = VO_TRUE;
		}

	}

	if(neetoconvert)
	{
#if defined _WIN32

		int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_pBuffer, -1, NULL, NULL, NULL, NULL );

		VO_CHAR *content_temp = NULL;
		content_temp = new VO_CHAR[ size + 1 ];
		memset( content_temp , 0 , size + 1 );

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_pBuffer, -1, content_temp, size, NULL, NULL );
		m_uSize = size; 

#else
		VOLOGR("haha");
		VO_CHAR *content_temp = new VO_CHAR[ m_uSize + 1 ];
		memset( content_temp , 0 , m_uSize + 1 );

		int SrcLen = vowcslen((vowchar_t *)m_pBuffer);

		VO_UnicodeToUTF8((vowchar_t *)m_pBuffer, SrcLen, content_temp, m_uSize);
#endif
		delete []m_pBuffer;
		m_pBuffer = content_temp;

	}

}

VO_U32 ISS_ManifestParser::CheckTheManifest( VO_PBYTE* ppBuffer,  VO_U32* pSize)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VO_PBYTE tmpStartBuffer = NULL;
	VO_PBYTE tmpEndBuffer = NULL;

	tmpStartBuffer = (VO_PBYTE)strstr((char*)m_pBuffer, (char*)"<SmoothStreamingMedia");
	tmpEndBuffer = (VO_PBYTE)SearchLastString((char*)tmpStartBuffer, (char*)"</SmoothStreamingMedia>");

	if(tmpEndBuffer && tmpStartBuffer){
		*pSize = tmpEndBuffer - tmpStartBuffer + strlen("</SmoothStreamingMedia>");
		*ppBuffer = tmpStartBuffer;
		nResult = VO_RET_SOURCE2_OK;
	}
	VOLOGI("CheckTheManifest---size=%lu, result=%lu", *pSize, nResult);

	return nResult;
}

VO_U32 ISS_ManifestParser::Parse( VO_PBYTE pBuffer,  VO_U32 uSize, SmoothStreamingMedia *manifest)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	VOLOGI("parser---begin---1");
	VO_U32 nBegin = voOS_GetSysTime();

	ProtectionHeader *piff = &manifest->piff;

	ConvertUnicodeToUTF8(pBuffer,  uSize);
	VOLOGI("parser---begin---2");

	VO_PBYTE pTmpBuffer = NULL;
	VO_U32 nTmpSize = 0;

	if(VO_RET_SOURCE2_OK != CheckTheManifest(&pTmpBuffer, &nTmpSize)){
		VOLOGE("Dirty data!!!");
		return VO_RET_SOURCE2_FAIL;
	}

	if(m_pXmlLoad->LoadXML((char*)pTmpBuffer, int(nTmpSize),voXML_FLAG_SOURCE_BUFFER) != VO_ERR_NONE)
	{
		VOLOGR("Cannot Load manifest file on parsing use voXMLParser!" );
		return VO_RET_SOURCE2_FAIL;
	}
	VOLOGI("parser XML Cost1:%d", voOS_GetSysTime() - nBegin);
	void *pRoot = NULL;
	m_pXmlLoad->GetFirstChild(NULL,&pRoot);
	if (!pRoot) 
		return VO_RET_SOURCE2_FAIL;	
	char *pTagName;
	int size;
	m_pXmlLoad->GetTagName(pRoot,&pTagName,size);

//	VO_U32 *streamCount 	  = &manifest->streamCount;
//	StreamIndex streamIndex[20];
//	memset(streamIndex,0x00,sizeof(StreamIndex) * 20);
//	FragmentItem * pFragmentHead[20];
//	FragmentItem * pFragmentTail[20];
	int 		   nCurFragmentID = 0;
//	memset(pFragmentHead,0x00, sizeof(FragmentItem *) * 20);
//	memset(pFragmentTail,0x00, sizeof(FragmentItem *) * 20);

	
//	int _audioNumber =0;
//	int qualityLevel_index	= 0;
//	int chunk_index 		= 1;

	VO_BOOL is_first		= VO_TRUE;
	VO_U64 duration 		= 0;
	VO_U64 pre_duration 	= 0;

	VO_CHAR audioPrivateData[256];
	memset( audioPrivateData, 0, 256 );

	char *attriValue;
	int nSize = 0;
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_MAJOR_VERSION),&attriValue,nSize) == VO_ERR_NONE)
		manifest->majorVersion = atoi(attriValue);
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_MINOR_VERSION),&attriValue,nSize) == VO_ERR_NONE)
		manifest->minorVersion = atoi(attriValue);
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_DURATION),&attriValue,nSize) == VO_ERR_NONE)
		manifest->duration = _ATO64(attriValue);

	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_TIME_SCALE),&attriValue,nSize) == VO_ERR_NONE)
		manifest->timeScale = _ATO64(attriValue);
	else
		manifest->timeScale = DEFAULT_TIME_SCALE;
	
	manifest->islive = VO_FALSE;
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_ISLIVE),&attriValue,nSize) == VO_ERR_NONE)
	{
		if (strcmp(attriValue, "TRUE") ==0 || 0 == strcmp(attriValue, "true"))
		{
			manifest->islive = VO_TRUE;
		}
	}

	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_LOOK_AHEAD_FRAGMENT_COUNT),&attriValue,nSize) == VO_ERR_NONE){
		manifest->lookaheadfragmentcount = atoi(attriValue);		
	}
	else{
		manifest->lookaheadfragmentcount = 0;
	}

	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_DVR_WINDOW_LENGTH),&attriValue,nSize) == VO_ERR_NONE){
		manifest->dvrwindowlength = VO_U64( _ATO64(attriValue));
	}else{
		manifest->dvrwindowlength = 0;
	}
	
	void *pvoChildNode;
	m_pXmlLoad->GetFirstChild(pRoot,&pvoChildNode);

	VO_U32	trackindex = 0;

	while (pvoChildNode) //&& stream_index < STREAM_NUMBER)
	{
		duration = 0;
		pre_duration = 0;
//		qualityLevel_index = 0;
//		chunk_index = 0;
		m_pXmlLoad->GetTagName(pvoChildNode,&pTagName,size);
		//PIFF
		if(strcmp(pTagName, TAG_PIFF_PROTECTION) == 0 )
		{
			piff->isUsePiff = VO_TRUE;
			void *pvoLevel1;
			m_pXmlLoad->GetFirstChild(pvoChildNode,&pvoLevel1);
			if(pvoLevel1)
			{
				m_pXmlLoad->GetTagName(pvoLevel1,&pTagName,nSize);
				
				if (strcmp((char*)(TAG_PIFF_PROTECTIONHEADER), pTagName) == 0)
				{
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_PIFF_SYSTEMID),&attriValue,nSize) == VO_ERR_NONE)
					{
						VO_CHAR systemid[50];
						int sssize	=0;
						for(int i = 0;i<nSize;i++)
						{	if(attriValue[i] != '-' ) 
								systemid[sssize++] = attriValue[i];
						}
						hexstr2byte(piff->systemID, (VO_CHAR *)systemid, sssize/2 );
						if(m_pXmlLoad->GetTagValue(pvoLevel1,&attriValue,nSize) == VO_ERR_NONE)
						{
							/*data must be changed to Base64
							*......................*
							*/
/*
							if( m_drmCustomerType != VOSMTH_DRM_Discretix_PlayReady )
							{
								VO_BYTE *c = new VO_BYTE[nSize];
								memcpy(c, attriValue,nSize);
								piff->data = c;
								piff->dataSize = nSize;
							}
*/
						}
					}
				}
			}
		}
		//end of PIFF

		//skip if the tagname is not stream index
		if (strcmp(pTagName, TAG_STREAM_INDEX) != 0)
		{
			m_pXmlLoad->GetNextSibling(pvoChildNode,&pvoChildNode);
			continue;
		}

		VO_CHAR type[8] = {0};	

		if(m_pXmlLoad->GetAttributeValue(pvoChildNode,(char*)(TAG_TYPE),&attriValue,nSize) == VO_ERR_NONE){
			memcpy(type, attriValue, nSize);
			strlwr(type);
		}

		//if the stream type is not video or audio, skip it derictly.
		if((strcmp(type, "video") != 0) && (strcmp(type, "audio") != 0) && (strcmp(type, "text") != 0)){
			m_pXmlLoad->GetNextSibling(pvoChildNode,&pvoChildNode);
			continue;
		}

		StreamIndex *pStreamIndex = new StreamIndex;
		memset(pStreamIndex->type, 0x0, sizeof(pStreamIndex->type));
		memset(pStreamIndex->name, 0x0, sizeof(pStreamIndex->name));
		memset(pStreamIndex->subtype, 0x0, sizeof(pStreamIndex->subtype));
		memset(pStreamIndex->url, 0x0, sizeof(pStreamIndex->url));
		memset(pStreamIndex->language, 0x0, sizeof(pStreamIndex->language));
		memset(&pStreamIndex->CustomAttribute, 0x0, sizeof(pStreamIndex->CustomAttribute));
		pStreamIndex->streamindex = manifest->streamIndexlist.size();
		manifest->streamIndexlist.push_back(pStreamIndex);

		memcpy(pStreamIndex->type, type, sizeof(type));
		
		if (strcmp(pStreamIndex->type, "video") == 0)
			pStreamIndex->nTrackType = VO_SOURCE2_TT_VIDEO;
		else if (strcmp(pStreamIndex->type, "audio") == 0)
			pStreamIndex->nTrackType = VO_SOURCE2_TT_AUDIO;
		else if (strcmp(pStreamIndex->type, "text") == 0)
			pStreamIndex->nTrackType = VO_SOURCE2_TT_SUBTITLE;

//		(*streamCount) ++;
		
		if(m_pXmlLoad->GetAttributeValue(pvoChildNode,(char*)(TAG_NAME),&attriValue,nSize) == VO_ERR_NONE)
			memcpy(pStreamIndex->name, attriValue, nSize);
		if(m_pXmlLoad->GetAttributeValue(pvoChildNode,(char*)(TAG_SUBTYPE),&attriValue,nSize) == VO_ERR_NONE)
		{

			memcpy(pStreamIndex->subtype, attriValue, nSize);
			pStreamIndex->nCodecType = CCodeCC::GetCodecByCC(pStreamIndex->nTrackType, CCodeCC::GetCC((char*)pStreamIndex->subtype));
			/*if (strcmp(attriValue, "H264")== 0 || strcmp(attriValue, "AVC1") == 0)
				pStreamIndex->nCodecType = VO_VIDEO_CodingH264;
			else if (strcmp(attriValue, "WVC1")== 0)
				pStreamIndex->nCodecType = VO_VIDEO_CodingVC1;//VOMP_VIDEO_CodingVC1;
			else if (strcmp(attriValue, "WmaPro")== 0 || strcmp(attriValue, "WMAP") == 0)
				pStreamIndex->nCodecType = VO_AUDIO_CodingWMA;
			else if (strcmp(attriValue, "AACL")== 0)
				pStreamIndex->nCodecType = VO_AUDIO_CodingAAC;*/
		}
		if(m_pXmlLoad->GetAttributeValue(pvoChildNode,(char*)(TAG_URL),&attriValue,nSize) == VO_ERR_NONE)
			memcpy(pStreamIndex->url, attriValue, nSize);
		if(m_pXmlLoad->GetAttributeValue(pvoChildNode,(char*)(TAG_LANGUAGE),&attriValue,nSize) == VO_ERR_NONE)
			memcpy(pStreamIndex->language, attriValue, nSize);

		if(!strlen(pStreamIndex->language)){
			if (VO_SOURCE2_TT_AUDIO == pStreamIndex->nTrackType)
				memcpy(pStreamIndex->language, "Audio",5);
			else if (VO_SOURCE2_TT_SUBTITLE == pStreamIndex->nTrackType)
				memcpy(pStreamIndex->language, "Text",4);
		}
		
		void *pvoLevel1;
		m_pXmlLoad->GetFirstChild(pvoChildNode,&pvoLevel1);
//		FragmentItem **tmppHead = &(pFragmentHead[nCurFragmentID] = NULL);
//		FragmentItem **tmppTail = &(pFragmentTail[nCurFragmentID++] = NULL);


		while(pvoLevel1)
		{
			m_pXmlLoad->GetTagName(pvoLevel1,&pTagName,nSize);
			if ( 0 == strcmp(pTagName, TAG_C))
			{	
				FragmentItem * ptr_item = new FragmentItem;
				memset( ptr_item , 0 , sizeof( FragmentItem ) );
				
				if( pStreamIndex->nTrackType == VO_SOURCE2_TT_VIDEO)
					ptr_item->nIsVideo = 1;
				else if( pStreamIndex->nTrackType == VO_SOURCE2_TT_AUDIO)
					ptr_item->nIsVideo = 0;
				else if( pStreamIndex->nTrackType == VO_SOURCE2_TT_SUBTITLE)
					ptr_item->nIsVideo = 2;
				
				ptr_item->streamindex = manifest->streamIndexlist.size() - 1;//*streamCount -1;
				ptr_item->index = 0;//chunk_index;

				if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_T),&attriValue,nSize) == VO_ERR_NONE)
				{
					ptr_item->starttime  =VO_U64( _ATO64(attriValue));
					//_ATOU64(attriValue, ptr_item->starttime);
				//	VOLOGI("StartTime:%llu, %s", ptr_item->starttime,attriValue);

					//sscanf(attriValue,"%I64u",&ptr_item->starttime);
					//ptr_item->starttime = _ATO64(attriValue);
				}
				else
					ptr_item->starttime = pre_duration + duration;

				//VOLOGR("StartTime:%llu, %s", ptr_item->starttime,attriValue);
		
			
				if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_D),&attriValue,nSize) == VO_ERR_NONE)
					pre_duration = _ATO64(attriValue);

				if( is_first )
				{
					is_first = VO_FALSE;
					manifest->fragment_duration = pre_duration;
				}
				ptr_item->duration = pre_duration;
				duration = ptr_item->starttime;
				//VOLOGR("chunk starttime %lld",ptr_item->starttime);
				add_fragmentItem(pStreamIndex, ptr_item );
//				pStreamIndex->chunksNumber++;
//				chunk_index++;
			}
			else if (strcmp((char*)(TAG_QUALITY_LEVEL), pTagName) == 0)
			{
				if (pStreamIndex->nTrackType == VO_SOURCE2_TT_VIDEO)
				{
					
					QualityLevel * ptr_item = new QualityLevel;
					memset( ptr_item , 0 , sizeof( QualityLevel ) );

					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_BITRATE),&attriValue,nSize) == VO_ERR_NONE)
					{
						ptr_item->bitrate = _ATO64(attriValue);
					}
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_FOUR_CC),&attriValue,nSize) == VO_ERR_NONE)
					{
						memcpy(ptr_item->fourCC, attriValue, nSize);	
						pStreamIndex->nCodecType = CCodeCC::GetCodecByCC(pStreamIndex->nTrackType, CCodeCC::GetCC(ptr_item->fourCC));
						/*if (strcmp(attriValue, "H264") ==0 || strcmp(attriValue, "AVC1")== 0)
							pStreamIndex->nCodecType = VO_VIDEO_CodingH264;
						else if (strcmp(attriValue, "WVC1")==0)
							pStreamIndex->nCodecType = VO_VIDEO_CodingVC1;//VO_VIDEO_CodingWMV;*/

					}
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_WIDTH_V2),&attriValue,nSize) == VO_ERR_NONE)
						ptr_item->video_info.Width = atoi(attriValue);
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_WIDTH_V1),&attriValue,nSize) == VO_ERR_NONE)
						ptr_item->video_info.Width = atoi(attriValue);
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_HEIGHT_V2),&attriValue,nSize) == VO_ERR_NONE)
						ptr_item->video_info.Height = atoi(attriValue);
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_HEIGHT_V1),&attriValue,nSize) == VO_ERR_NONE)
						ptr_item->video_info.Height = atoi(attriValue);
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_CODEC_PRIVATE_DATA),&attriValue,nSize) == VO_ERR_NONE)
						ptr_item->length_CPD = nSize / 2;
					hexstr2byte( ptr_item->codecPrivateData, (VO_CHAR *)attriValue, ptr_item->length_CPD );
					add_qualityLevel(pStreamIndex, ptr_item, trackindex);
					trackindex++;
				}
				//audio
				else if(pStreamIndex->nTrackType == VO_SOURCE2_TT_AUDIO)
				{
					QualityLevel * pAudio_QL = new QualityLevel;
					memset( pAudio_QL , 0 , sizeof( QualityLevel ) );

//					QualityLevel_Audio *pAudio_QL= &pStreamIndex->audio_QL[_audioNumber];
//					_audioNumber++;
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_BITRATE),&attriValue,nSize) == VO_ERR_NONE)
						pAudio_QL->bitrate = _ATO64(attriValue);

					m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_FOUR_CC),&attriValue,nSize);
					memcpy(pAudio_QL->fourCC, attriValue, nSize);	
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_SIMPLE_RATE),&attriValue,nSize) == VO_ERR_NONE)
						pAudio_QL->audio_info.samplerate_index = get_samplerate_index(atoi(attriValue));

					if(pStreamIndex->nCodecType == 0)
						pStreamIndex->nCodecType = CCodeCC::GetCodecByCC(pStreamIndex->nTrackType, CCodeCC::GetCC(pAudio_QL->fourCC));

					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_WAVE_FORMAT_EX),&attriValue,nSize) == VO_ERR_NONE)
					{
						VO_BYTE	tmpData[256] = {0};
						VO_U32	length = nSize / 2;
						hexstr2byte( tmpData, (VO_CHAR *)attriValue, length );
						VO_PBYTE p = tmpData;
						VO_WAVEFORMATEX waveex;
						waveex.wFormatTag			=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;
						waveex.nChannels			=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;
						waveex.nSamplesPerSec		=	VO_U32(MAKEFOURCC2(*(p+3),*(p+ 2),*(p+1),*p )); p+=4;	
						waveex.nAvgBytesPerSec		=	VO_U32(MAKEFOURCC2(*(p+3),*(p+ 2),*(p+1),*p )); p+=4;	
						waveex.nBlockAlign			=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;
						waveex.wBitsPerSample		=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;
						waveex.cbSize				=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;

					//	(VO_WAVEFORMATEX* )pAudio_QL->codecPrivateData;
						pAudio_QL->audio_info.audio_format.Channels = waveex.nChannels;
						pAudio_QL->audio_info.audio_format.SampleRate = waveex.nSamplesPerSec;
						pAudio_QL->audio_info.audio_format.SampleBits = waveex.wBitsPerSample;
						pAudio_QL->audio_info.samplerate_index = get_samplerate_index(pAudio_QL->audio_info.audio_format.SampleRate);

						if(strcmp(pAudio_QL->fourCC, "mp4a") == 0 && waveex.wFormatTag == 0x00FF){
							pStreamIndex->nCodecType = VO_AUDIO_CodingAAC;
						}else if( strcmp(pStreamIndex->subtype, "WmaPro") ==0|| strcmp(pAudio_QL->fourCC,"WMAP") == 0){
							pStreamIndex->nCodecType = VO_AUDIO_CodingWMA;
							memcpy(pAudio_QL->codecPrivateData,tmpData,length);
							pAudio_QL->length_CPD = length;
						}else{
							VOLOGE("Parse---No CodecType!");
						}
					}
					else	
					{
						m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_CODEC_PRIVATE_DATA),&attriValue,nSize);
						VO_U16 size = nSize/ 2;
						char *privateData = attriValue;
	
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_CHANNELS),&attriValue,nSize) == VO_ERR_NONE)
							pAudio_QL->audio_info.audio_format.Channels = atoi(attriValue);
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_SIMPLE_RATE),&attriValue,nSize) == VO_ERR_NONE)
							pAudio_QL->audio_info.audio_format.SampleRate = atoi(attriValue);
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_BIT_PER_SAMPLE),&attriValue,nSize) == VO_ERR_NONE)
							pAudio_QL->audio_info.audio_format.SampleBits = atoi(attriValue); 
						VO_U16 wFormatTag = 0;
						VO_U16 nBlockAlign = 0;
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_AUDIO_TAG),&attriValue,nSize) == VO_ERR_NONE)
							wFormatTag = atoi(attriValue);
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_PACKET_SIZE),&attriValue,nSize) == VO_ERR_NONE)
							nBlockAlign = atoi(attriValue);


						if( strcmp(pStreamIndex->subtype, "WmaPro") ==0|| strcmp(pAudio_QL->fourCC,"WMAP") == 0 ||
							AUDIO_TAG_WMA == wFormatTag)
						{
							pStreamIndex->nCodecType = VO_AUDIO_CodingWMA;

							VO_WAVEFORMATEX waveex;
							waveex.wFormatTag = wFormatTag;
							waveex.nChannels = (VO_U16)pAudio_QL->audio_info.audio_format.Channels;
							waveex.nSamplesPerSec = pAudio_QL->audio_info.audio_format.SampleRate;
							waveex.nAvgBytesPerSec = VO_U32(pAudio_QL->bitrate);
							waveex.nBlockAlign = nBlockAlign;
							waveex.wBitsPerSample = (VO_U16)pAudio_QL->audio_info.audio_format.SampleBits;
							waveex.cbSize = size;
							memcpy(pAudio_QL->codecPrivateData,&waveex,VO_WAVEFORMATEX_STRUCTLEN);
							hexstr2byte( &pAudio_QL->codecPrivateData[VO_WAVEFORMATEX_STRUCTLEN], (VO_CHAR *)privateData, waveex.cbSize );
							pAudio_QL->length_CPD = VO_WAVEFORMATEX_STRUCTLEN + waveex.cbSize;
						}
						else if(strcmp(pStreamIndex->subtype, "AACL")==0 || strcmp(pAudio_QL->fourCC,"AACL") == 0 || 
							strcmp(pStreamIndex->subtype, "AACH")==0 || strcmp(pAudio_QL->fourCC,"AACH") ==0 ||
							AUDIO_TAG_AAC == wFormatTag)
						{
							pStreamIndex->nCodecType = VO_AUDIO_CodingAAC;
							hexstr2byte(pAudio_QL->codecPrivateData, (VO_CHAR *)privateData, size);
							pAudio_QL->length_CPD = size;
						}
						else if(strcmp(pAudio_QL->fourCC, "EC-3")==0)
						{
							pStreamIndex->nCodecType = VO_AUDIO_CodingEAC3;
							hexstr2byte(pAudio_QL->codecPrivateData, (VO_CHAR *)privateData, size);
							pAudio_QL->length_CPD = size;
						}
						else if(strcmp(pAudio_QL->fourCC, "AC-3")==0)
						{
							pStreamIndex->nCodecType = VO_AUDIO_CodingAC3;
							hexstr2byte(pAudio_QL->codecPrivateData, (VO_CHAR *)privateData, size);
							pAudio_QL->length_CPD = size;
						}
						
						else{
							VOLOGE("Parse---No CodecType!");
						}
						
					}
					
					add_qualityLevel(pStreamIndex, pAudio_QL, trackindex);
					trackindex++;
				}
				else if(pStreamIndex->nTrackType == VO_SOURCE2_TT_SUBTITLE)
				{
					QualityLevel * ptr_item = new QualityLevel;
					memset( ptr_item , 0 , sizeof( QualityLevel ) );

					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_BITRATE),&attriValue,nSize) == VO_ERR_NONE)
					{
						ptr_item->bitrate = _ATO64(attriValue);
					}
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_FOUR_CC),&attriValue,nSize) == VO_ERR_NONE)
					{
						memcpy(ptr_item->fourCC, attriValue, nSize);	
						pStreamIndex->nCodecType = CCodeCC::GetCodecByCC(pStreamIndex->nTrackType, CCodeCC::GetCC(ptr_item->fourCC));

					}
					add_qualityLevel(pStreamIndex, ptr_item, trackindex);
					trackindex++;
				}
//				qualityLevel_index++;
			}
			m_pXmlLoad->GetNextSibling(pvoLevel1,&pvoLevel1);
		//	VOLOGR("parser XML Cost1-1:%d", voOS_GetSysTime() - nBegin);

		}
		m_pXmlLoad->GetNextSibling(pvoChildNode,&pvoChildNode);
		//VOLOGR("parser XML Cost1-2:%d", voOS_GetSysTime() - nBegin);

	}
/*	
	for(int i =0; i<20;i++)
	{
		MergeFragmentList(&m_ptr_FI_head,&m_ptr_FI_tail,pFragmentHead[i],pFragmentTail[i]);
	}
	StreamIndex *streamIndexc = new StreamIndex[manifest->streamCount];
	memcpy(streamIndexc,streamIndex,sizeof(StreamIndex)*manifest->streamCount);
	manifest->streamIndex = streamIndexc;
*/	

/*
	list_T<StreamIndex *>::iterator streamIndexiter;
	for(streamIndexiter = manifest->streamIndexlist.begin(); streamIndexiter != manifest->streamIndexlist.end(); ++streamIndexiter)
	{
		StreamIndex* tmp = (StreamIndex*)(*streamIndexiter);
		VOLOGI("parser XML streamindex=%lu, Chunk_List_size=%lu, QL_List_size=%lu", tmp->streamindex, tmp->Chunk_List.size(), tmp->QL_List.size());

		list_T<FragmentItem *>::iterator ChunkIndexiter;
		for(ChunkIndexiter = tmp->Chunk_List.begin(); ChunkIndexiter != tmp->Chunk_List.end(); ++ChunkIndexiter){
			FragmentItem* pItem = (FragmentItem*)(*ChunkIndexiter);
			VOLOGI("parser XML streamindex=%ld, index=%ld, starttime=%llu, duration=%llu, nIsVideo=%lu", pItem->streamindex, pItem->index, pItem->starttime, pItem->duration, pItem->nIsVideo);
		}
	}
*/
	VOLOGI("parser XML Cost2:%d", voOS_GetSysTime() - nBegin);
	return nResult;

}

VO_U32 ISS_ManifestParser::SetLibOp(VO_LIB_OPERATOR* libop)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VOLOGI("SetLibOp");

	if(!libop){
		return nResult;
	}

	if(m_pXmlLoad){
		m_pXmlLoad->SetLibOperator(libop);
		nResult = VO_RET_SOURCE2_OK;
	}

	return nResult;
}

VO_U32 ISS_ManifestParser::SetWorkPath(VO_TCHAR * pWorkPath)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VOLOGI("SetWorkPath---pWorkPath=%s", pWorkPath);

	if(!pWorkPath){
		return nResult;
	}

	if(m_pXmlLoad){
		m_pXmlLoad->SetWorkPath(pWorkPath);
		nResult = VO_RET_SOURCE2_OK;
	}

	return nResult;
}


VO_U32 ISS_ManifestParser::ReleaseManifestInfo(SmoothStreamingMedia *manifest)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	if(!manifest){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGR("ReleaseManifestInfo---start");

	list_T<StreamIndex *>::iterator streamIndexiter;
	for(streamIndexiter = manifest->streamIndexlist.begin(); streamIndexiter != manifest->streamIndexlist.end(); ++streamIndexiter)
	{
		StreamIndex* tmp = (StreamIndex*)(*streamIndexiter);

		list_T<QualityLevel *>::iterator qualityleveliter;
		for(qualityleveliter = tmp->QL_List.begin(); qualityleveliter != tmp->QL_List.end(); ++qualityleveliter)
		{
			QualityLevel* tmpql = (QualityLevel*)(*qualityleveliter);
			delete tmpql;
		}
		tmp->QL_List.clear();

		list_T<FragmentItem *>::iterator fragmentiter;
		for(fragmentiter = tmp->Chunk_List.begin(); fragmentiter != tmp->Chunk_List.end(); ++fragmentiter)
		{
			FragmentItem* tmpfragment = (FragmentItem*)(*fragmentiter);
			delete tmpfragment;
		}
		tmp->Chunk_List.clear();

		delete tmp;
	}

	manifest->streamIndexlist.clear();

	if(manifest->piff.data){
		delete[] manifest->piff.data;
	}

	VOLOGR("ReleaseManifestInfo---end");
	return nResult;
}


VO_VOID ISS_ManifestParser::add_fragmentItem(StreamIndex *pStreamIndex, FragmentItem * ptr_item)
{
	VO_U32 count = pStreamIndex->Chunk_List.size();
	ptr_item->index = count;

	//from little to big
	if(0 == count){
		pStreamIndex->Chunk_List.push_back(ptr_item);
		return;
	}

	list_T<FragmentItem *>::iterator iter = --pStreamIndex->Chunk_List.end();
	FragmentItem* pTmp = (FragmentItem *)(*iter);

	if(ptr_item->starttime >=pTmp->starttime){
		pStreamIndex->Chunk_List.push_back(ptr_item);
	}else{
		iter=pStreamIndex->Chunk_List.begin();
		while(iter != pStreamIndex->Chunk_List.end())
		{
			pTmp = (FragmentItem *)(*iter);
			if( ptr_item->starttime < pTmp->starttime ){
//				pStreamIndex->Chunk_List.push_front(ptr_item);
				pStreamIndex->Chunk_List.insert(iter, ptr_item);
				
				break;
			}
		
			if(0 == (--count)){
				pStreamIndex->Chunk_List.push_back(ptr_item);			
				break;
			}
			iter++;
		}	

	}

}


VO_VOID ISS_ManifestParser::add_qualityLevel( StreamIndex *pStreamIndex, QualityLevel * ptr_item, VO_U32 index)
{
	VO_U32 count = pStreamIndex->QL_List.size();
	ptr_item->index_QL = index;

#ifdef _AUTO_SORT_
	if(0 == count){
		pStreamIndex->QL_List.push_back(ptr_item);
		return;
	}

	list_T<QualityLevel *>::iterator iter = --pStreamIndex->QL_List.end();
	QualityLevel* pTmp = (QualityLevel *)(*iter);

	if(ptr_item->bitrate >=pTmp->bitrate){
		pStreamIndex->QL_List.push_back(ptr_item);
	}else{
	
		iter=pStreamIndex->QL_List.begin();
		while(iter != pStreamIndex->QL_List.end())
		{
			pTmp = (QualityLevel *)(*iter);
			if( ptr_item->bitrate < pTmp->bitrate){
				pStreamIndex->QL_List.insert(iter, ptr_item);
				break;
			}
		
			if(0 == (--count)){
				pStreamIndex->QL_List.push_back(ptr_item);			
				break;
			}
			iter++;
		}	

	}
#else
	pStreamIndex->QL_List.push_back(ptr_item);
#endif
	
	VOLOGR("add_qualityLevel---index_QL=%ld, bitrate=%lld",ptr_item->index_QL, ptr_item->bitrate);
}

VO_VOID ISS_ManifestParser::hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex, VO_S32 length )
{
	for( VO_S32 i = 0 ; i < length ; i++ )
	{
		VO_CHAR temp[3];

		temp[0] = str_hex[ 2*i ];
		temp[1] = str_hex[ 2*i + 1 ];
		temp[2] = '\0';

		int value;

		sscanf( temp , "%x" , &value );

		ptr_hex[i] = value;
	}
}

int ISS_ManifestParser::vowcslen(const vowchar_t* str) {   
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	return len;
} 

VO_S32 ISS_ManifestParser::get_samplerate_index( VO_S32 samplerate ) 
{
	int idx = 0;

	for (idx = 0; idx < 12; idx++) 
	{
		if (samplerate == AAC_SampRateTab[idx])
			return idx;
	}

	return 4; //Default 44100
} 


VO_BOOL ISS_ManifestParser::ParseSmoothStreamingMedia()
{
	return VO_TRUE;
}

VO_BOOL ISS_ManifestParser::ParseStreamIndex()
{
	return VO_TRUE;
}

VO_BOOL ISS_ManifestParser::ParseQualityLevel_Audio()
{
	return VO_TRUE;
}

VO_BOOL ISS_ManifestParser::ParseQualityLevel()
{
	return VO_TRUE;
}

VO_BOOL ISS_ManifestParser::ParseChunk()
{
	return VO_TRUE;
}

int ISS_ManifestParser::VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen) 
{
	
	VOLOGR("Enter to VO_UnicodeToUTF8 file size: %d ", nDstLen);

	int is = 0, id = 0;
	const vowchar_t *ps = szSrc;
	unsigned char *pd = (unsigned char*)strDst;

	if (nDstLen <= 0)
		return 0;

	for (is = 0; is < nSrcLen; is++) {
		if (BYTE_1_REP > ps[is]) { /* 1 byte utf8 representation */
			if (id + 1 < nDstLen) {
				pd[id++] = (unsigned char)ps[is];
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_2_REP > ps[is]) {
			if (id + 2 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 6 | 0xc0);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_3_REP > ps[is]) { /* 3 byte utf8 representation */
			if (id + 3 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 12 | 0xe0);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_4_REP > ps[is]) { /* 4 byte utf8 representation */
			if (id + 4 < nDstLen) {
				pd[id++] = (unsigned char)(VO_U32(ps[is]) >> 18 | 0xf0);
				pd[id++] = (unsigned char)(((ps[is] >> 12)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} /* @todo Handle surrogate pairs */
	}

	pd[id] = '\0'; /* Terminate string */

	return id; /* This value is in bytes */
}

