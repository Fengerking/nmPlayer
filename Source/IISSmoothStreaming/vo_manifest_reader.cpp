#include "vo_manifest_reader.h"
#include "voString.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "math.h"
#include "CCodeCC.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef _IOS
#ifdef _ISS_SOURCE_
using namespace _ISS;
#endif
#endif

#define be16toh(x) 	(( x & 0xff00) >> 8 | ( x & 0xff) << 8)


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

#ifdef WIN32 
#define _ATO64(a) _atoi64(a) 
#elif defined LINUX || defined _IOS
#define _ATO64(a) atoll(a) 

#endif 

const int AAC_SampRateTab[12] = {
	96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};

vo_manifest_reader::vo_manifest_reader(void)
{
	//memset (&m_xmlLoad, 0, sizeof (VO_XML_PARSER_API));
	m_file_content = NULL;
	m_manifest = 0;
	m_ptr_FI_head = 0;
	m_ptr_FI_tail = 0;
	m_is_live = VO_FALSE;
	m_item_count = 0;
	m_pXmlLoad = NULL;
	m_drmCustomerType = VOSMTH_DRM_COMMON_PlayReady;

}

vo_manifest_reader::~vo_manifest_reader(void)
{
	destroy();
}
VO_BOOL vo_manifest_reader::Parse( VO_PBYTE pBuffer, VO_U32 uSize ,SmoothStreamingMedia *manifest )
{
	m_manifest = manifest;
	m_file_content_size =uSize;	
	m_file_content = new VO_CHAR[m_file_content_size+1];
	memcpy(m_file_content, (VO_CHAR*)pBuffer, m_file_content_size);
	m_file_content[m_file_content_size] = '\0';

	if( m_file_content[0] == (VO_CHAR)0xff && m_file_content[1] == (VO_CHAR)0xfe)
	{
#if defined _WIN32

		int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_file_content, -1, NULL, NULL, NULL, NULL );

		VO_CHAR *content_temp = NULL;
		content_temp = new VO_CHAR[ size + 1 ];
		memset( content_temp , 0 , size + 1 );

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_file_content, -1, content_temp, size, NULL, NULL );
		m_file_content_size = size;	

#else
		VOLOGR("haha");
		VO_CHAR *content_temp = new VO_CHAR[ m_file_content_size + 1 ];
		memset( content_temp , 0 , m_file_content_size + 1 );

		int SrcLen = vowcslen((vowchar_t *)m_file_content);

		VO_UnicodeToUTF8((vowchar_t *)m_file_content, SrcLen, content_temp, m_file_content_size);
#endif
		delete []m_file_content;
		m_file_content = content_temp;

	}

	parse_manifest2();

	return VO_TRUE;
}
VO_BOOL vo_manifest_reader::Parse(vo_http_stream *ptr_fragment, VO_CHAR * url, SmoothStreamingMedia *manifest )
{
	destroy();

	get_relative_directory( url );

	m_manifest = manifest;

	//VOLOGR("get_manifest start");
	if( VO_FALSE == get_manifest(ptr_fragment, url ) )
	{
		//VOLOGR("get_manifest failed!");
		return VO_FALSE;
	}
	//VOLOGR("get_manifest end");

	parse_manifest2();
	return VO_TRUE;
}

VO_VOID vo_manifest_reader::get_relative_directory( VO_CHAR * url )
{
	memset( m_relative_directory , 0 , sizeof( m_relative_directory ) );
	strcpy( m_relative_directory , url );

	VO_CHAR * ptr = strrchr( m_relative_directory , '/' );
	ptr++;
	*ptr = '\0';
}

VO_BOOL vo_manifest_reader::get_manifest(vo_http_stream *pHttpStream, VO_CHAR * url )
{
	//VOLOGR("begin download manifest");
	if( VO_FALSE == pHttpStream->startdownload( url , DOWNLOAD2MEM ) )
		return VO_FALSE;
	//VOLOGR("end download manifest");

	m_read_time = voOS_GetSysTime();

	VO_S64 content_size = pHttpStream->get_content_length();
	m_file_content_size = (VO_S32)content_size;
	VOLOGR("Content Size:%lld", m_file_content_size);

	m_file_content = new VO_CHAR[ VO_U32(content_size + 1) ];
	memset( m_file_content , 0 , VO_U32(content_size + 1) );
	
	VO_CHAR * ptr = m_file_content;

	while( content_size != 0 )
	{
		VO_S64 size = pHttpStream->read( (VO_PBYTE)ptr , content_size );

		if( size == -2 )
			continue;

		if( size == -1 )
		{
			pHttpStream->close();
			return VO_FALSE;
		}
		content_size -= size;
		//VOLOGR("ptr[0]:0x%x,ptr[1]:0x%x", ptr[0],ptr[1]);
		ptr = ptr + size;
	}
	VOLOGR("left Size:%lld", content_size);

	//Add convert UTF-16 to UTF-8
	//UTF-16 XML Head: FF FE
	VOLOGR("[0]:0x%x,[1]:0x%x", m_file_content[0], m_file_content[1]);
	//01/09/2012, leon, in NDKbuild this is wrong.
//	if( m_file_content[0] == -1 && m_file_content[1] == -2 )

	memset(m_manifest, 0, sizeof(SmoothStreamingMedia));
	if( m_drmCustomerType == VOSMTH_DRM_Discretix_PlayReady )
	{
		m_manifest->piff.data = new VO_BYTE[m_file_content_size+1];
		memcpy(m_manifest->piff.data, m_file_content, m_file_content_size);
		m_manifest->piff.dataSize = m_file_content_size;
	}	

	if( m_file_content[0] == (VO_CHAR)0xff && m_file_content[1] == (VO_CHAR)0xfe)
	{
#if defined _WIN32

		int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_file_content, -1, NULL, NULL, NULL, NULL );

		VO_CHAR *content_temp = NULL;
		content_temp = new VO_CHAR[ size + 1 ];
		memset( content_temp , 0 , size + 1 );

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_file_content, -1, content_temp, size, NULL, NULL );
		m_file_content_size = size;	

#else
		VOLOGR("haha");
		VO_CHAR *content_temp = new VO_CHAR[ m_file_content_size + 1 ];
		memset( content_temp , 0 , m_file_content_size + 1 );

		int SrcLen = vowcslen((vowchar_t *)m_file_content);

		VO_UnicodeToUTF8((vowchar_t *)m_file_content, SrcLen, content_temp, m_file_content_size);
#endif
		delete []m_file_content;
		m_file_content = content_temp;

	}

	pHttpStream->closedownload();

	return VO_TRUE;
}

VO_VOID vo_manifest_reader::destroy()
{
	if( m_file_content )
	{
		delete []m_file_content;
		m_file_content = 0;
	}

	FragmentItem * ptr_item = m_ptr_FI_head;

	while( ptr_item )
	{
		FragmentItem * ptr_temp = ptr_item;
		ptr_item = ptr_item->ptr_next;

		delete ptr_temp;
	}

	m_ptr_FI_head = m_ptr_FI_tail = 0;

}


VO_VOID vo_manifest_reader::hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex, VO_S32 length )
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

VO_S64 vo_manifest_reader::find_fragment_starttime( VO_S64 pos,int nIsVideo )
{

	FragmentItem * ptr_temp = m_ptr_FI_head;
	FragmentItem *tmpPtr = NULL;
	FragmentItem *tmpPtrPre = NULL;
	while(ptr_temp )
	{
		//VOLOGR("seektime %lld,now %lld,next %lld,bVideo:%d(%d)",pos,ptr_temp->starttime,ptr_temp->ptr_next->starttime, ptr_temp->nIsVideo, nIsVideo);
		if( ptr_temp->nIsVideo == nIsVideo)
		{
			if(pos >= ptr_temp->starttime)
			{
				tmpPtrPre = tmpPtr;
				tmpPtr = ptr_temp;
			}
			else if(pos < ptr_temp->starttime )
			{
				break;
			}
		}
			
		ptr_temp = ptr_temp->ptr_next;
	}

	return tmpPtrPre ? tmpPtrPre->starttime :0;
}

VO_BOOL vo_manifest_reader::get_FirstfragmentItem( FragmentItem ** ppItem )
{
	*ppItem = m_ptr_FI_head;
	return VO_TRUE;
}
VO_BOOL vo_manifest_reader::get_fragmentItem( FragmentItem ** ptr_item,  VO_S64 start_time )
{
	FragmentItem * ptr_temp = m_ptr_FI_head;

	while(ptr_temp)
	{
		if(ptr_temp->starttime == start_time )
		{
			*ptr_item = ptr_temp;
			return VO_TRUE;
		}

		ptr_temp = ptr_temp->ptr_next;
	}
	*ptr_item = NULL;
	return VO_FALSE;
}

VO_VOID vo_manifest_reader::MergeFragmentList(FragmentItem **head, FragmentItem **tail,FragmentItem *head1, FragmentItem *tail1)
{
	FragmentItem *pFragmentItem = head1;
	FragmentItem *pCurItem = *head;
	while( pFragmentItem )
	{
		
		head1= head1->ptr_next;
		pCurItem = add_fragmentItem(head, tail, pFragmentItem,pCurItem);
		pFragmentItem = head1;
		m_item_count++;
	}
}
FragmentItem *vo_manifest_reader::add_fragmentItem(FragmentItem ** head , FragmentItem **tail, FragmentItem * ptr_item,FragmentItem *pCurItem  )
{
	//if(nFlag)
	ptr_item->ptr_next = NULL;

	if( *head == NULL && *tail == NULL )
		*head = *tail = ptr_item;
	else
	{
		if (ptr_item->starttime >= (*tail)->starttime)
		{
			(*tail)->ptr_next = ptr_item;
			*tail = ptr_item;
		}
		else if( ptr_item->starttime < (*head)->starttime 
			|| (ptr_item->streamindex == 0 &&ptr_item->starttime == (*head)->starttime))
		{
			ptr_item->ptr_next = *head;
			*head = ptr_item;
		}
		else
		{
			FragmentItem * ptr_cur = NULL;
			if(pCurItem ->starttime <= ptr_item->starttime)
				ptr_cur = pCurItem;
			else
				ptr_cur = *head;
			
			while(ptr_cur->ptr_next)
			{
				if(ptr_item->starttime >= ptr_cur->starttime && ptr_item->starttime < ptr_cur->ptr_next->starttime)
				{
					ptr_item->ptr_next = ptr_cur->ptr_next;
					ptr_cur->ptr_next = ptr_item;
					break;

				}
				ptr_cur = ptr_cur->ptr_next;
			}
			return ptr_cur;
		}
	}
//	streamIndex->chunksNumber++;
//	m_item_count++;
}
VO_VOID vo_manifest_reader::add_fragmentItem( FragmentItem * ptr_item )
{
	ptr_item->ptr_next = NULL;
	if( m_ptr_FI_head == NULL && m_ptr_FI_tail == NULL )
		m_ptr_FI_head = m_ptr_FI_tail = ptr_item;
	else
	{
		if( ptr_item->starttime < m_ptr_FI_head->starttime 
			|| (ptr_item->streamindex == 0 &&ptr_item->starttime == m_ptr_FI_head->starttime))
		{
			ptr_item->ptr_next = m_ptr_FI_head;
			m_ptr_FI_head = ptr_item;
		}
		else if (ptr_item->starttime >= m_ptr_FI_tail->starttime)
		{
			m_ptr_FI_tail->ptr_next = ptr_item;
			m_ptr_FI_tail = ptr_item;
		}
		else
		{
			FragmentItem * ptr_cur = m_ptr_FI_head;

			while(ptr_cur->ptr_next)
			{
		
				if(ptr_item->starttime >= ptr_cur->starttime && ptr_item->starttime < ptr_cur->ptr_next->starttime)
				{
					ptr_item->ptr_next = ptr_cur->ptr_next;

					ptr_cur->ptr_next = ptr_item;

					break;
				}
				ptr_cur = ptr_cur->ptr_next;
			}
			
		}

	}
	m_manifest->streamIndex[ptr_item->streamindex].chunksNumber++;
	m_item_count++;
}


VO_VOID vo_manifest_reader::add_qualityLevel( StreamIndex *pStreamIndex,VO_S32 index_stream, QualityLevel * ptr_item )
{
	ptr_item->index_QL = pStreamIndex->QL_Number;

	if( pStreamIndex->m_ptr_QL_head == NULL && pStreamIndex->m_ptr_QL_tail == NULL )
		pStreamIndex->m_ptr_QL_head = pStreamIndex->m_ptr_QL_tail = ptr_item;
	else
	{
		QualityLevel * p1 = pStreamIndex->m_ptr_QL_head;
		QualityLevel * p2 = p1->ptr_next;	
		QualityLevel * tail = pStreamIndex->m_ptr_QL_tail;
		QualityLevel *head = pStreamIndex->m_ptr_QL_head;
		VO_S32 counts =  pStreamIndex->QL_Number;
		while(counts--)
		{
			if( p1->bitrate < ptr_item->bitrate )
			{
				if(p1 == head)
				{
					pStreamIndex->m_ptr_QL_head = ptr_item;
					p1->ptr_pre = ptr_item;
					ptr_item->ptr_next = p1;
					//pStreamIndex->m_ptr_QL_tail = p1;
				}
				else
				{
					p1->ptr_next = ptr_item;
					ptr_item->ptr_next = p2;
					ptr_item->ptr_pre = p1;
					p2->ptr_pre = ptr_item;
				}

				break;
			}
			if(counts ==0 && p1 == tail )
			{
				p1->ptr_next = ptr_item;
				ptr_item->ptr_pre = p1;
				pStreamIndex->m_ptr_QL_tail = ptr_item;
				break;
			}
			
			p1 = p1->ptr_next;
		}

// 		pStreamIndex->m_ptr_QL_tail->ptr_next = ptr_item;
// 		ptr_item->ptr_pre = pStreamIndex->m_ptr_QL_tail;
// 		pStreamIndex->m_ptr_QL_tail = ptr_item;
	}
	pStreamIndex->QL_Number++;
}


VO_BOOL vo_manifest_reader::parse_manifest2()
{
	
	VO_U32 nBegin = voOS_GetSysTime();
	m_item_count = 0;

	ProtectionHeader *piff = &m_manifest->piff;


	if(m_pXmlLoad->LoadXML((char*)m_file_content, int(m_file_content_size),voXML_FLAG_SOURCE_BUFFER) != VO_ERR_NONE)
	{
		VOLOGR("Cannot Load manifest file on parsing use voXMLParser!" );
		return VO_FALSE;
	}
	VOLOGI("parser XML Cost1:%d", voOS_GetSysTime() - nBegin);
	void *pRoot = NULL;
	m_pXmlLoad->GetFirstChild(NULL,&pRoot);
	if (!pRoot) 
		return VO_FALSE;	
	char *pTagName;
	int size;
	m_pXmlLoad->GetTagName(pRoot,&pTagName,size);

	VO_U32 *streamCount       = &m_manifest->streamCount;
	StreamIndex streamIndex[20];
	memset(streamIndex,0x00,sizeof(StreamIndex) * 20);
	FragmentItem * pFragmentHead[20];
	FragmentItem * pFragmentTail[20];
	int            nCurFragmentID = 0;
	memset(pFragmentHead,0x00, sizeof(FragmentItem *) * 20);
	memset(pFragmentTail,0x00, sizeof(FragmentItem *) * 20);

	
	int _audioNumber =0;
	int qualityLevel_index  = 0;
	int chunk_index         = 1;

	VO_BOOL is_first        = VO_TRUE;
	VO_U64 duration			= 0;
	VO_U64 pre_duration		= 0;

	VO_CHAR	audioPrivateData[256];
	memset( audioPrivateData, 0, 256 );

	char *attriValue;
	int nSize = 0;
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_MAJOR_VERSION),&attriValue,nSize) == VO_ERR_NONE)
		m_manifest->majorVersion = atoi(attriValue);
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_MINOR_VERSION),&attriValue,nSize) == VO_ERR_NONE)
		m_manifest->minorVersion = atoi(attriValue);
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_DURATION),&attriValue,nSize) == VO_ERR_NONE)
		m_manifest->duration = _ATO64(attriValue);

	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_TIME_SCALE),&attriValue,nSize) == VO_ERR_NONE)
		m_manifest->timeScale = _ATO64(attriValue);
	else
		m_manifest->timeScale = DEFAULT_TIME_SCALE;
	
	m_manifest->islive = VO_FALSE;
	m_is_live = VO_FALSE;
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(TAG_ISLIVE),&attriValue,nSize) == VO_ERR_NONE)
	{
		if (StrCompare(attriValue, "TRUE") ==0)
		{
			m_manifest->islive = VO_TRUE;
			m_is_live = VO_TRUE;
		}
	}
	void *pvoChildNode;
	m_pXmlLoad->GetFirstChild(pRoot,&pvoChildNode);


	while (pvoChildNode) //&& stream_index < STREAM_NUMBER)
	{
		duration = 0;
		pre_duration = 0;
		qualityLevel_index = 0;
		chunk_index = 0;
		m_pXmlLoad->GetTagName(pvoChildNode,&pTagName,size);
		//PIFF
		if(StrCompare(pTagName, TAG_PIFF_PROTECTION) == 0 )
		{
			piff->isUsePiff = VO_TRUE;
			void *pvoLevel1;
			m_pXmlLoad->GetFirstChild(pvoChildNode,&pvoLevel1);
			if(pvoLevel1)
			{
				m_pXmlLoad->GetTagName(pvoLevel1,&pTagName,nSize);
				
				if (StrCompare((char*)(TAG_PIFF_PROTECTIONHEADER), pTagName) == 0)
				{
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_PIFF_SYSTEMID),&attriValue,nSize) == VO_ERR_NONE)
					{
						VO_CHAR systemid[50];
						int sssize  =0;
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
							if( m_drmCustomerType != VOSMTH_DRM_Discretix_PlayReady )
							{
								VO_BYTE *c = new VO_BYTE[nSize];
								memcpy(c, attriValue,nSize);
								piff->data = c;
								piff->dataSize = nSize;
							}
						}
					}
				}
			}
		}
		//end of PIFF

		if (StrCompare(pTagName, TAG_STREAM_INDEX) != 0)
		{
			m_pXmlLoad->GetNextSibling(pvoChildNode,&pvoChildNode);
			continue;
		}

		StreamIndex *pStreamIndex = &streamIndex[(*streamCount)];

		if(m_pXmlLoad->GetAttributeValue(pvoChildNode,(char*)(TAG_TYPE),&attriValue,nSize) == VO_ERR_NONE)
			memcpy(pStreamIndex->type, attriValue, nSize);
		if (StrCompare(pStreamIndex->type, "video") == 0)
			pStreamIndex->nTrackType = VO_SOURCE_TT_VIDEO;
		else if (StrCompare(pStreamIndex->type, "audio") == 0)
			pStreamIndex->nTrackType = VO_SOURCE_TT_AUDIO;
		else if(StrCompare(pStreamIndex->type, "text") == 0)
		{
			m_pXmlLoad->GetNextSibling(pvoChildNode,&pvoChildNode);
			continue;
		}
		(*streamCount) ++;
		
		if(m_pXmlLoad->GetAttributeValue(pvoChildNode,(char*)(TAG_NAME),&attriValue,nSize) == VO_ERR_NONE)
			memcpy(pStreamIndex->name, attriValue, nSize);
		if(m_pXmlLoad->GetAttributeValue(pvoChildNode,(char*)(TAG_SUBTYPE),&attriValue,nSize) == VO_ERR_NONE)
		{

			memcpy(pStreamIndex->subtype, attriValue, nSize);
			pStreamIndex->nCodecType = CCodeCC::GetCodecByCC(pStreamIndex->nTrackType, CCodeCC::GetCC((char*)pStreamIndex->subtype));
			/*if (StrCompare(attriValue, "H264")== 0 || StrCompare(attriValue, "AVC1") == 0)
				pStreamIndex->nCodecType = VO_VIDEO_CodingH264;
			else if (StrCompare(attriValue, "WVC1")== 0)
				pStreamIndex->nCodecType = VO_VIDEO_CodingVC1;//VOMP_VIDEO_CodingVC1;
			else if (StrCompare(attriValue, "WmaPro")== 0 || StrCompare(attriValue, "WMAP") == 0)
				pStreamIndex->nCodecType = VO_AUDIO_CodingWMA;
			else if (StrCompare(attriValue, "AACL")== 0)
				pStreamIndex->nCodecType = VO_AUDIO_CodingAAC;*/
		}
		if(m_pXmlLoad->GetAttributeValue(pvoChildNode,(char*)(TAG_URL),&attriValue,nSize) == VO_ERR_NONE)
			memcpy(pStreamIndex->url, attriValue, nSize);
		
		void *pvoLevel1;
		m_pXmlLoad->GetFirstChild(pvoChildNode,&pvoLevel1);
		FragmentItem **tmppHead = &(pFragmentHead[nCurFragmentID] = NULL);
		FragmentItem **tmppTail = &(pFragmentTail[nCurFragmentID++] = NULL);

		while(pvoLevel1)
		{
			m_pXmlLoad->GetTagName(pvoLevel1,&pTagName,nSize);
			if ( 0 == StrCompare(pTagName, TAG_C))
			{	
				FragmentItem * ptr_item = new FragmentItem;
				memset( ptr_item , 0 , sizeof( FragmentItem ) );
				
				if(	pStreamIndex->nTrackType == VO_SOURCE_TT_VIDEO)
					ptr_item->nIsVideo = 1;
				else
					ptr_item->nIsVideo = 0;
				ptr_item->streamindex = *streamCount -1;
				ptr_item->index = chunk_index;

				if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_T),&attriValue,nSize) == VO_ERR_NONE)
				{
				//	ptr_item->starttime  =VO_U64( _ATO64(attriValue));
					_ATOU64(attriValue, ptr_item->starttime);
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
					m_manifest->fragment_duration = pre_duration;
				}
				ptr_item->duration = pre_duration;
				duration = ptr_item->starttime;
				//VOLOGR("chunk starttime %lld",ptr_item->starttime);
				add_fragmentItem(tmppHead,tmppTail, ptr_item );
				pStreamIndex->chunksNumber++;
				chunk_index++;
			}
			else if (StrCompare((char*)(TAG_QUALITY_LEVEL), pTagName) == 0)
			{
				if (pStreamIndex->nTrackType == VO_SOURCE_TT_VIDEO)
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
						/*if (StrCompare(attriValue, "H264") ==0 || StrCompare(attriValue, "AVC1")== 0)
							pStreamIndex->nCodecType = VO_VIDEO_CodingH264;
						else if (StrCompare(attriValue, "WVC1")==0)
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
					add_qualityLevel(pStreamIndex, *streamCount -1, ptr_item);
				}
				//audio
				else if(pStreamIndex->nTrackType == VO_SOURCE_TT_AUDIO)
				{

					QualityLevel_Audio *pAudio_QL= &pStreamIndex->audio_QL[_audioNumber];
					_audioNumber++;
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_BITRATE),&attriValue,nSize) == VO_ERR_NONE)
						pAudio_QL->bitrate = _ATO64(attriValue);

					m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_FOUR_CC),&attriValue,nSize);
					memcpy(pAudio_QL->fourCC, attriValue, nSize);	
					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_SIMPLE_RATE),&attriValue,nSize) == VO_ERR_NONE)
						pAudio_QL->samplerate_index = get_samplerate_index(atoi(attriValue));

					if(pStreamIndex->nCodecType == 0)
						pStreamIndex->nCodecType = CCodeCC::GetCodecByCC(pStreamIndex->nTrackType, CCodeCC::GetCC(pAudio_QL->fourCC));

					if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_WAVE_FORMAT_EX),&attriValue,nSize) == VO_ERR_NONE)
					{
						pAudio_QL->length_CPD = nSize / 2;
						hexstr2byte( pAudio_QL->codecPrivateData, (VO_CHAR *)attriValue, nSize/ 2 );
						VO_PBYTE p = pAudio_QL->codecPrivateData;
						VO_WAVEFORMATEX waveex;
						waveex.wFormatTag			=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;
						waveex.nChannels			=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;
						waveex.nSamplesPerSec		=	VO_U32(MAKEFOURCC2(*(p+3),*(p+ 2),*(p+1),*p ));	p+=4;	
						waveex.nAvgBytesPerSec		=	VO_U32(MAKEFOURCC2(*(p+3),*(p+ 2),*(p+1),*p ));	p+=4;	
						waveex.nBlockAlign			=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;
						waveex.wBitsPerSample		=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;
						waveex.cbSize				=	VO_U16(MAKEFOURCC2(0,0,*(p+1),*p ));				p+=2;

					//	(VO_WAVEFORMATEX* )pAudio_QL->codecPrivateData;
						pAudio_QL->audio_info.Channels = waveex.nChannels;
						pAudio_QL->audio_info.SampleRate = waveex.nSamplesPerSec;
						pAudio_QL->audio_info.SampleBits = waveex.wBitsPerSample;
						pAudio_QL->samplerate_index = get_samplerate_index(pAudio_QL->audio_info.SampleRate);

						if(StrCompare(pAudio_QL->fourCC, "mp4a") == 0 && waveex.wFormatTag == 0x00FF)
							pStreamIndex->nCodecType = VO_AUDIO_CodingAAC;
					}
					else	
					{
						m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_CODEC_PRIVATE_DATA),&attriValue,nSize);
						VO_U16 size = nSize/ 2;
						char *privateData = attriValue;
	
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_CHANNELS),&attriValue,nSize) == VO_ERR_NONE)
							pAudio_QL->audio_info.Channels = atoi(attriValue);
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_SIMPLE_RATE),&attriValue,nSize) == VO_ERR_NONE)
							pAudio_QL->audio_info.SampleRate = atoi(attriValue);
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_BIT_PER_SAMPLE),&attriValue,nSize) == VO_ERR_NONE)
							pAudio_QL->audio_info.SampleBits = atoi(attriValue); 
						VO_U16 wFormatTag = 0;
						VO_U16 nBlockAlign = 0;
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_AUDIO_TAG),&attriValue,nSize) == VO_ERR_NONE)
							wFormatTag = atoi(attriValue);
						if(m_pXmlLoad->GetAttributeValue(pvoLevel1,(char*)(TAG_PACKET_SIZE),&attriValue,nSize) == VO_ERR_NONE)
							nBlockAlign = atoi(attriValue);


						if( StrCompare(pStreamIndex->subtype, "WmaPro") ==0|| StrCompare(pAudio_QL->fourCC,"WMAP") == 0)
						{
							pStreamIndex->nCodecType = VO_AUDIO_CodingWMA;

							VO_WAVEFORMATEX waveex;
							waveex.wFormatTag = wFormatTag;
							waveex.nChannels = pAudio_QL->audio_info.Channels;
							waveex.nSamplesPerSec = pAudio_QL->audio_info.SampleRate;
							waveex.nAvgBytesPerSec = VO_U32(pAudio_QL->bitrate);
							waveex.nBlockAlign = nBlockAlign;
							waveex.wBitsPerSample = pAudio_QL->audio_info.SampleBits;
							waveex.cbSize = size;
							memcpy(pAudio_QL->codecPrivateData,&waveex,VO_WAVEFORMATEX_STRUCTLEN);
							hexstr2byte( &pAudio_QL->codecPrivateData[VO_WAVEFORMATEX_STRUCTLEN], (VO_CHAR *)privateData, waveex.cbSize );
							pAudio_QL->length_CPD = VO_WAVEFORMATEX_STRUCTLEN + waveex.cbSize;

						}
						else if(StrCompare(pStreamIndex->subtype, "AACL")==0 || StrCompare(pAudio_QL->fourCC,"AACL") ==0)
						{
							pStreamIndex->nCodecType = VO_AUDIO_CodingAAC;
							hexstr2byte(pAudio_QL->codecPrivateData, (VO_CHAR *)privateData, size);
							pAudio_QL->length_CPD = size;
						}
					}
				}
				qualityLevel_index++;
			}
			m_pXmlLoad->GetNextSibling(pvoLevel1,&pvoLevel1);
		//	VOLOGI("parser XML Cost1-1:%d", voOS_GetSysTime() - nBegin);

		}
		m_pXmlLoad->GetNextSibling(pvoChildNode,&pvoChildNode);
		//VOLOGI("parser XML Cost1-2:%d", voOS_GetSysTime() - nBegin);

	}
	
	for(int i =0; i<20;i++)
	{
		MergeFragmentList(&m_ptr_FI_head,&m_ptr_FI_tail,pFragmentHead[i],pFragmentTail[i]);
	}
	StreamIndex *streamIndexc = new StreamIndex[m_manifest->streamCount];
	memcpy(streamIndexc,streamIndex,sizeof(StreamIndex)*m_manifest->streamCount);
	m_manifest->streamIndex = streamIndexc;

	VOLOGI("parser XML Cost2:%d", voOS_GetSysTime() - nBegin);
	return VO_TRUE;

}

int  vo_manifest_reader::StrCompare(const char* p1, const char* p2)
{
#ifndef __USE_TINYXML
	if(!p1 || !p2) return -1;
#endif

#if defined WINCE
	return strcmp(p1, p2);
#elif defined _WIN32
	return stricmp(p1, p2);
#elif defined LINUX
	return strcasecmp(p1, p2);
#elif defined _IOS
	return strcasecmp(p1, p2);
#elif defined _MAC_OS
	return strcasecmp(p1, p2);
#endif
}

int vo_manifest_reader::vowcslen(const vowchar_t* str) {   
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	return len;
} 

int vo_manifest_reader::VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen) 
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

VO_S32 vo_manifest_reader::get_samplerate_index( VO_S32 samplerate ) 
{
	int idx = 0;

	for (idx = 0; idx < 12; idx++) 
	{
		if (samplerate == AAC_SampRateTab[idx])
			return idx;
	}

	return 4; //Default 44100
} 

VO_U32 vo_manifest_reader::ParserAudioHeadData(VO_AUDIO_FORMAT &info)
{
	//do it, codes from Jackey
	return VO_ERR_NONE;
}
