
#include "vo_mpd_reader.h"
#include "voString.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "math.h"


#define COUNTS_NUMBER 10240

#ifdef WIN32 
#define _ATO64(a) _atoi64(a) 
#elif defined LINUX 
#define _ATO64(a) atoll(a) 
#endif 



#define be16toh(x) 	(( x & 0xff00) >> 8 | ( x & 0xff) << 8)

const int AAC_SampRateTab[12] = {
	96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};

vo_mpd_reader::vo_mpd_reader(void)
:m_manifest(0)
,m_file_content(NULL)
,m_is_live(VO_FALSE)
,m_is_need_update(VO_FALSE)
,m_seg_temlate(VO_FALSE)
,m_seg_count(0)
,m_ptr_FI_head(0)
,m_ptr_FI_tail(0)
,video_flag(0)
,audio_flag(0)
{	
	InitAVList();
	CreateAVList(COUNTS_NUMBER);
	FlushAVList();
	video_scale = 0;
	audio_scale = 0;
	m_count_audio = 0;
	m_count_video = 0;
	real_count = 0;
	unknown_flag = 0;
	//mpd_tag = new Mpd_tag();
	m_grouptag = new Group_Tag();
	m_template_tag = new SegmentTemplate_Tag();
	m_seg_base = new SegmentBase_Tag();
	m_seg_info =  new SegInfo_Tag();
	m_rpe_tag =  new Repre_Tag();
//	memset (&m_pXmlLoad, 0, sizeof (VO_XML_PARSER_API));
}

vo_mpd_reader::~vo_mpd_reader(void)
{
    destroy();
	DestroyAVList();
}



int vo_mpd_reader::vowcslen(const vowchar_t* str) {   
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	return len;
} 

int vo_mpd_reader::VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen) 
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
				pd[id++] = (unsigned char)(ps[is] >> 18 | 0xf0);
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

 
VO_CHAR *  vo_mpd_reader::getRepresentationUrl(int stream_index,int representation_index){
	       Representation * temp = m_manifest->adaptionSet[stream_index].m_ptr_QL_head;
		   char * temp_url = NULL;
		   while(temp){
			   if(temp->index_QL == representation_index){
				   temp_url = temp->initalUrl;
				   strcat(m_manifest->baseurl,temp_url);
				   return m_manifest->baseurl;
			   }
			   temp = temp->ptr_next;
		   }
		   return NULL;
}

VO_BOOL vo_mpd_reader::get_segmentItem(Seg_item ** ptr_item,  VO_U64 start_time ){
	Seg_item * temp = m_ptr_FI_head;
	while(temp){
		if(temp->starttime == start_time){
			*ptr_item = temp;
			return VO_TRUE;
		}
		temp = temp->ptr_next;

	}
	*ptr_item = NULL;
	return VO_FALSE;
}

VO_BOOL vo_mpd_reader::get_segmentItem(Seg_item ** ptr_item,  int ini_chunk ){
	Seg_item * temp = m_ptr_FI_head;
	int j = 0;
	while(j<ini_chunk && temp){
		j++;
		temp = temp->ptr_next;
	}
	if(j ==ini_chunk){
			*ptr_item = temp;
			return VO_TRUE;
	}
	return VO_FALSE;
}

VO_BOOL vo_mpd_reader::get_mpd( VO_CHAR * url )
{
	if( VO_FALSE == m_stream.open( url , DOWNLOAD2MEM ) )
		return VO_FALSE;
	m_read_time = voOS_GetSysTime();
	VO_S64 content_size = m_stream.get_content_length();
	if(m_file_content) 
	{
		delete []m_file_content;
		m_file_content = 0;
	}
	m_file_content = new VO_CHAR[ content_size + 1 ];
	m_file_content_size = content_size;
	memset( m_file_content , 0 , content_size + 1 );
    //FILE*   pf = fopen( "c:\\log.txt ", "wb");
	VO_CHAR * ptr = m_file_content;
	while( content_size != 0 )
	{
		VO_S64 size = m_stream.read( (VO_PBYTE)ptr , content_size ); 

		if( size == -2 )
			continue;

		if( size == -1 )
		{
			m_stream.close();
			return VO_FALSE;
		}

		content_size -= size;
		ptr = ptr + size;
	}
	//VOLOGR();
//utf-16 xml  to utf-8  utf-16 xml FF  fe
   // fwrite(m_file_content,1,m_file_content_size,pf);
   
	if( m_file_content[0] == -1 && m_file_content[1] == -2 )
	{

#if defined _WIN32

		int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_file_content, -1, NULL, NULL, NULL, NULL );

		VO_CHAR *content_temp = NULL;
		content_temp = new VO_CHAR[ size + 1 ];
		memset( content_temp , 0 , size + 1 );

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_file_content, -1, content_temp, size, NULL, NULL );
		m_file_content_size = size;	
#elif defined LINUX

		VO_CHAR *content_temp = new VO_CHAR[ m_file_content_size + 1 ];
		memset( content_temp , 0 , m_file_content_size + 1 );

		int SrcLen = vowcslen((vowchar_t *)m_file_content);

		VO_UnicodeToUTF8((vowchar_t *)m_file_content, SrcLen, content_temp, m_file_content_size);
#endif
		delete []m_file_content;
		m_file_content = content_temp;
		//delete  []content_temp;

	}
  
	m_stream.closedownload();
	return VO_TRUE;
}

VO_VOID vo_mpd_reader::get_relative_directory( VO_CHAR * url )
{
	memset( m_relative_directory , 0 , sizeof( m_relative_directory ) );
	strcpy( m_relative_directory , url );
	VO_CHAR * ptr = strrchr( m_relative_directory , '/' );
	ptr++;
	*ptr = '\0';
}

VO_S32 vo_mpd_reader::get_samplerate_index( VO_S32 samplerate ) 
{
	int idx = 0;

	for (idx = 0; idx < 12; idx++) 
	{
		if (samplerate == AAC_SampRateTab[idx])
			return idx;
	}

	return 4; //Default 44100
} 


#if 0
VO_BOOL  vo_mpd_reader::analyze_video(VO_S64 Duration_video[1024] ,VO_S64 Timeline_video[1024],void *pLevel_repre,int flag,int group_index)
{
		    int i  = 0;
		    m_count_video = 0;
		    m_count_audio = 0;
	       void  *pChildNode = NULL;
		//   pChildNode = pLevel_repre->FirstChild();
		  m_pXmlLoad->GetFirstChild(pLevel_repre,&pChildNode);
		   //int nVal				= 0; //the number value
		    char * pTagName = NULL;
		   VO_S64  duration  = 0;
		   int nSize;
		
		

		   while(pChildNode){ 
			   Seg_item * item = new Seg_item;
		       memset( item , 0 , sizeof(Seg_item));
			//  pTagName = pChildNode->ToElement()->Attribute(TAG_R);
			   if(m_pXmlLoad->GetAttributeValue(pChildNode,(char*)(TAG_R),&pTagName,nSize) == VO_ERR_NONE)
			  {
				  	 item->group_index = group_index;

#if defined _WIN32
					m_seg_count= _atoi64(pTagName);
#else 
				    m_seg_count = atoll(pTagName);
#endif
			  }
          //    pTagName = pChildNode->ToElement()->Attribute(TAG_D);
			    if(m_pXmlLoad->GetAttributeValue(pChildNode,(char*)(TAG_D),&pTagName,nSize) == VO_ERR_NONE)
			{
					#if defined _WIN32
					item->duration = _atoi64(pTagName);
                    #else 
					item->duration = atoll(pTagName);
#endif
					duration = item->duration;

			  }

			
		 //    pTagName = pChildNode->ToElement()->Attribute(TAG_T);
			   if(m_pXmlLoad->GetAttributeValue(pChildNode,(char*)(TAG_T),&pTagName,nSize) == VO_ERR_NONE)
			{
				 item->group_index = group_index;

#if defined _WIN32
				  item->starttime = _atoi64(pTagName);
#else 
				 item->starttime = atoll(pTagName);
				
#endif     
					  Timeline_video[i] = item->starttime;
					  Duration_video[i] = item->duration;
					  i++;

			  }
			
                      for(VO_U32 j = 0;j<m_seg_count;j++){
						Seg_item * item = new Seg_item;
						memset( item , 0 , sizeof(Seg_item));
						item->starttime = duration*j;
						item->startnmber = j+1;
						Timeline_video[j] = item->starttime;
						Duration_video[j] = item->duration;

					}


			
			 
			  
			  if(flag)
				  m_count_video++;
			  else
				  m_count_audio++;

			  item->nIsVideo = flag;
			//  pChildNode = pChildNode->NextSibling();
//
			  m_pXmlLoad->GetNextSibling(pChildNode,&pChildNode);
	          add_fragmentItem(item);
			 // m_seg_count++;

		  }
		


		  return VO_TRUE;

}
#endif
#if 1
VO_VOID vo_mpd_reader::analyze_video(VO_S64 Duration_video[1024] ,VO_S64 Timeline_video[1024],TiXmlNode *pLevel_segmentTimeLine,int flag,int group_index)
{
	VO_S64 i  = 0;
	m_count_video = 0;
	m_count_audio = 0;
	TiXmlNode  *pChildNode_temp = NULL;
	pChildNode_temp = pLevel_segmentTimeLine->FirstChild();
	const char * pTagName = NULL;
	VO_U64  d  = 0;
	VO_U64 nRCount = 0;
	VO_U64 nDuration = 0;
	VO_U64 t_M  = 0;//modify
	VO_U64 t_O  = 0;//original


	while(pChildNode_temp)
	{
		nRCount = 0;
		pTagName = pChildNode_temp->ToElement()->Attribute(TAG_D);
		if(pTagName)
		{
			d = _ATO64(pTagName);
		}
		else
			d = 0;
	
		pTagName = pChildNode_temp->ToElement()->Attribute(TAG_T);
		if(pTagName)
		{
			t_M = _ATO64(pTagName);
		}
		else
			t_M = nDuration;
		t_O = t_M;

		pTagName = pChildNode_temp->ToElement()->Attribute(TAG_R);
		if(pTagName)
		{
			nRCount= _ATO64(pTagName);
		}
		if(nRCount==0)
			nRCount =1;
	
		real_count += nRCount;
		if(real_count>1024){
			real_count = nRCount;
		}
		else
			real_count = 0;

		if(nRCount> COUNTS_NUMBER/4)
		{
			nRCount = COUNTS_NUMBER/10;
		}
		int *countX; 
		if(flag)
			countX = &m_count_video;
		else
			countX = &m_count_audio;
		while(nRCount --)
		{
			Seg_item * item_temp= new Seg_item;
			VOLOGR("new Seg_item3 = %p",item_temp);
			memset( item_temp , 0 , sizeof(item_temp));
			item_temp->duration = d;
			item_temp->nIsVideo = flag;
			item_temp->group_index = group_index;
			item_temp->starttime = t_M;
			item_temp->startnmber = *countX+1;
			Timeline_video[*countX] = item_temp->starttime;
			Duration_video[*countX] = item_temp->duration;
			add_fragmentItem(item_temp);
			(*countX) ++;
			m_seg_count++;

			t_M = t_O + (nDuration += d);
		}
		pChildNode_temp = pChildNode_temp->NextSibling();
	}
	
}
#else
VO_VOID vo_mpd_reader::analyze_video(VO_S64 Duration_video[1024] ,VO_S64 Timeline_video[1024],TiXmlNode *pLevel_segmentTimeLine,int flag,int group_index)
{
	VO_S64 i  = 0;
	m_count_video = 0;
	m_count_audio = 0;
	VO_S64  start  = 0;
	TiXmlNode  *pChildNode_temp = NULL;
	pChildNode_temp = pLevel_segmentTimeLine->FirstChild();
	const char * pTagName = NULL;
	VO_S64  duration  = 0;
	if(pChildNode_temp)
	{
		pTagName = pChildNode_temp->ToElement()->Attribute(TAG_R);
		if(pTagName)
		{
#if defined _WIN32
			m_seg_count= _atoi64(pTagName);
#else 
			m_seg_count = atoll(pTagName);
#endif
		}
	}
	if(m_seg_count>1024)
	{
		real_count = m_seg_count;
		m_seg_count =1024;
	}
	if(m_seg_count!=0)
	{
		pTagName = pChildNode_temp->ToElement()->Attribute(TAG_D);
		if(pTagName)
		{
#if defined _WIN32
			duration = _atoi64(pTagName);
#else 
			duration = atoll(pTagName);
#endif
		}
		pTagName = pChildNode_temp->ToElement()->Attribute(TAG_T);
		if(pTagName)
		{
#if defined _WIN32
			start = _atoi64(pTagName);
#else 
			start = atoll(pTagName);
#endif
		}
		for(VO_U32 j = 0;j<m_seg_count;j++)
		{
			Seg_item * item_temp= new Seg_item;
			memset( item_temp , 0 , sizeof(item_temp));
			item_temp->duration = duration;
			item_temp->nIsVideo = flag;
			item_temp->group_index = group_index;
			item_temp->starttime = start+item_temp->duration*j;
			item_temp->startnmber = j+1;
			Timeline_video[j] = item_temp->starttime;
			Duration_video[j] = item_temp->duration;
			add_fragmentItem(item_temp);
		}
		m_count_video = m_seg_count;
		m_count_audio = m_seg_count;
	}
	else
	{
		while(pChildNode_temp)
		{ 
			Seg_item * temp = new Seg_item;
			memset( temp , 0 , sizeof(Seg_item));
			pTagName = pChildNode_temp->ToElement()->Attribute(TAG_D);
			if(pTagName)
			{
#if defined _WIN32
				temp->duration = _atoi64(pTagName);
#else 
				temp->duration = atoll(pTagName);
#endif
				duration = temp->duration;
			}
			pTagName = pChildNode_temp->ToElement()->Attribute(TAG_T);
			 if(pTagName)
			 {
				 temp->group_index = group_index;

#if defined _WIN32
				 temp->starttime = _atoi64(pTagName);
#else 
				 temp->starttime = atoll(pTagName);
				
#endif     
				 Timeline_video[i] = temp->starttime;
				 Duration_video[i] = temp->duration;
				 i++;
			  }
              pTagName = pChildNode_temp->ToElement()->Attribute(TAG_R);
			  if(pTagName)
			  {
				  temp->group_index = group_index;
#if defined _WIN32
					m_seg_count= _atoi64(pTagName);
#else 
				    m_seg_count = atoll(pTagName);
#endif
					for(VO_U32 j = 0;j<m_seg_count;j++)
					{
						Seg_item * item = new Seg_item;
						memset( item , 0 , sizeof(Seg_item));
						item->starttime =  duration*j;
						item->startnmber = j+1;
						add_fragmentItem(item);
						Timeline_video[j] = item->starttime;
						Duration_video[j] = item->duration;
					}
			  }
			  if(flag)
				  m_count_video++;
			  else
				  m_count_audio++;
			  temp->nIsVideo = flag;
			  if(m_seg_count==0)
			  {
				  add_fragmentItem(temp);
			  }
			  else
			  {
				  delete temp;
				  temp = NULL;
			  }
			  pChildNode_temp = pChildNode_temp->NextSibling();
		}
}
}
#endif

VO_VOID vo_mpd_reader::add_fragmentItem( Seg_item * ptr_item )
{
	if(video_scale ==0||audio_scale==0)
	{
       video_scale = 1;
	   audio_scale = 1;
	}
	VO_S64 temp_time = -1 ;
	VO_S64 head_time = -1 ;
	VO_S64 tail_time = -1;
	if(ptr_item->nIsVideo)
		temp_time = ptr_item->starttime*1000/video_scale;
	else
		temp_time = ptr_item->starttime*1000/audio_scale;
	ptr_item->ptr_next = NULL;
	if( m_ptr_FI_head == NULL && m_ptr_FI_tail == NULL )
	{
		m_ptr_FI_head = m_ptr_FI_tail = ptr_item;
	}
	else
	{
		if(m_ptr_FI_tail->nIsVideo)
			tail_time = m_ptr_FI_tail->starttime*1000/video_scale;
		else
			tail_time =  m_ptr_FI_tail->starttime*1000/audio_scale;
		if(m_ptr_FI_head->nIsVideo)
			head_time = m_ptr_FI_head->starttime*1000/video_scale;
		else
			head_time =  m_ptr_FI_head->starttime*1000/audio_scale;
		if((ptr_item->group_index == 0 &&temp_time == tail_time))
		{
			m_ptr_FI_tail->ptr_next = ptr_item;
			m_ptr_FI_tail = ptr_item;
		}
		else if( temp_time <head_time)
		{
			ptr_item->ptr_next = m_ptr_FI_head;
			m_ptr_FI_head = ptr_item;
		}
		else if (temp_time>= tail_time)
		{
			m_ptr_FI_tail->ptr_next = ptr_item;
			m_ptr_FI_tail = ptr_item;
		}
		else
		{
			Seg_item * ptr_cur = m_ptr_FI_head;
			VO_S64 cur_time = -1;
			VO_S64 cur_time_next = -1;

			while(ptr_cur->ptr_next)
			{
				if(ptr_cur->ptr_next->nIsVideo)
					cur_time_next = ptr_cur->ptr_next->starttime*1000/video_scale;
				else
					cur_time_next = ptr_cur->ptr_next->starttime*1000/audio_scale;
				if(ptr_cur->nIsVideo)
					cur_time = ptr_cur->starttime*1000/video_scale;
				else
					cur_time = ptr_cur->starttime*1000/audio_scale;
				if(temp_time >= cur_time && temp_time < cur_time_next)
				{
					ptr_item->ptr_next = ptr_cur->ptr_next;
					ptr_cur->ptr_next = ptr_item;
					break;
				}
				ptr_cur = ptr_cur->ptr_next;
			}
		 }
	}
}

Seg_item* vo_mpd_reader::GetFirstChunk(){
	return m_ptr_FI_head;
}

//#ifndef __USE_TINYXML_1111
#if 0
VO_BOOL  vo_mpd_reader::analyze_2(char * url){

	//TiXmlDocument doc;
	memset(m_manifest, 0, sizeof(MpdStreamingMedia));
	m_manifest->group_count = 0;
	VO_S32 *group_count       = &m_manifest->group_count;
	ApaptionSet adaption_set[10];
	memset(adaption_set,0x00,sizeof(ApaptionSet) * 10);
	if(m_pXmlLoad->LoadXML((char*)m_file_content, int(m_file_content_size),voXML_FLAG_SOURCE_BUFFER) != VO_ERR_NONE)
	{
		VOLOGR("Cannot Load manifest file on parsing use voXMLParser!" );
		return VO_FALSE;
	}
	
   void* pNode = NULL;
    m_pXmlLoad->GetFirstChild(NULL,&pNode);
	if (!pNode) 
		return VO_FALSE;	
	
	 char* attriValue;
    VO_CHAR	m_base_url[256];
	int nSize				= 0; //the number value
	

	
	VO_S64 period_index = 0;
    VO_S64 pre_duration = 0;
	
if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(TAG_DASH_STREAMING_TYPE),&attriValue,nSize) == VO_ERR_NONE)
	
	{
	if (0 == StrCompare(attriValue, "OnDemand")){
		m_manifest->islive = VO_FALSE;
		m_is_live = VO_FALSE;
	}
	else if(0 == StrCompare(attriValue, "IsLive"))
	{
		m_manifest->islive = VO_TRUE; 
		m_is_live = VO_TRUE;
	}
	else if(0 == StrCompare(attriValue, "static")){
		m_manifest->islive = VO_FALSE;
		m_is_live = VO_FALSE;
	}
	else if(0 == StrCompare(attriValue, "dynamic")){
		m_manifest->islive = VO_TRUE; 
		m_is_live = VO_TRUE;

	}
	}
if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(TAG_DASH_UPDATE_PERIOD),&attriValue,nSize) == VO_ERR_NONE)
{
		VO_CHAR	duration[512];
		memcpy(duration,attriValue,strlen(attriValue));
		replace(duration,"PT","");
		replace(duration,"S","");
		#if defined _WIN32
	    m_update_period = _atoi64(duration)*1000;
#elif defined LINUX
	    m_update_period = atoll(duration)*1000;
		#endif
	}


	int hour = 0;
	int minute = 0;
	int second  = 0;
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(TAG_DASH_STREAMING_MPDURATION),&attriValue,nSize) == VO_ERR_NONE)
	{

		VO_CHAR	duration[512];
		memcpy(duration,attriValue,strlen(attriValue));
		
		replace(duration,"S","");
		replace(duration,"PT","");
		replace(duration,"0H0M","");
		replace(duration,"0H","");
		const char * find_url_h = "H";
		int h_index = lastIndex(duration,find_url_h);
		if(h_index!=-1){
			 hour = duration[h_index-1]-48;
			 duration[h_index-1]= '0';
            replace(duration,"0H","");
	    }
		  replace(duration,"H","");
	    const char * find_url_m = "M";
	   int m_index = lastIndex(duration,find_url_m);
		if(m_index>0)
		{
			minute = (duration[m_index-1])-48;
			
		   duration[m_index-1]= '0';
		   replace(duration,"0M","");
		}
		else if(m_index ==0){
			minute = (duration[m_index+1])-48;
			replace(duration,"M","");
			duration[m_index] = '0';

		}


         VO_CHAR s_dd[512];
		const char * find_url_dot = ".";
		int s_index = lastIndex(duration,find_url_dot);
		if(s_index!=-1){
		for(int j = 0;j<strlen(attriValue);j++)
		s_dd[j] = duration[s_index+j+1];
		}
		#if defined _WIN32
		if( _atoi64(duration)>0)
		second = _atoi64(s_dd);
		else
		second = _atoi64(s_dd)*1000;


		#elif defined LINUX
		if( atoll(duration)>0)
		second = atoll(s_dd);
		else
		second = atoll(s_dd)*1000;
	
        #endif
		
#if defined _WIN32
		m_manifest->mediaPresentationDuration = _atoi64(duration)*1000+ (minute)*60*1000+second;
#elif defined LINUX
		m_manifest->mediaPresentationDuration = atoll(duration)*1000+minute*60*1000+second;
		
#elif defined _IOS
		m_manifest->mediaPresentationDuration = atoll(duration)*1000+minute*60*1000+second;
#elif defined _MAC_OS
		m_manifest->mediaPresentationDuration = atoll(duration)*1000+minute*60*1000+second;
#endif
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(TAG_DASH_BufferDepth),&attriValue,nSize) == VO_ERR_NONE){
	
		VO_CHAR	duration[512];
		memcpy(duration,attriValue,strlen(attriValue));
		replace(duration,"PT","");
		replace(duration,"PTM","");
		replace(duration,"S","");
		replace(duration,"0H0M","");
		replace(duration,"0H","");
		const char * find_url_h = "H";
		int h_index = lastIndex(duration,find_url_h);
		if(h_index!=-1){
			 hour = duration[h_index-1]-48;
			 duration[h_index-1]= '0';
            replace(duration,"0H","");
	    }
	    const char * find_url_m = "M";
	   int m_index = lastIndex(duration,find_url_m);
		if(m_index!=-1)
		{
			minute = (duration[m_index-1])-48;
		   duration[m_index-1]= '0';
		   replace(duration,"0M","");
		}
          VO_CHAR s_dd[512];
		const char * find_url_dot = ".";
		int s_index = lastIndex(duration,find_url_dot);
		if(s_index!=-1){
		for(int j = 0;j<strlen(attriValue);j++)
		s_dd[j] = duration[s_index+j+1];
		}
		#if defined _WIN32
		second = _atoi64(s_dd);
		#elif defined LINUX
		second = atoll(s_dd);
        #endif
		
#if defined _WIN32
		m_manifest->mediaPresentationDuration = _atoi64(duration)*1000+ (minute)*60*1000+second;
#elif defined LINUX
		m_manifest->mediaPresentationDuration = atoll(duration)*1000+minute*60*1000+second;
		
#elif defined _IOS
		m_manifest->mediaPresentationDuration = atoll(duration)*1000+minute*60*1000+second;
#elif defined _MAC_OS
		m_manifest->mediaPresentationDuration = atoll(duration)*1000+minute*60*1000+second;
#endif
	}
	//duration
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(TAG_DASH_STREAMING_MINBUFFERTIMER),&attriValue,nSize) == VO_ERR_NONE)
	{
#if defined _WIN32
		m_manifest->minBufferTime = _atoi64(attriValue)*1000;
#elif defined LINUX
		m_manifest->minBufferTime = atoll(attriValue)*1000;
#elif defined _IOS
		m_manifest->minBufferTime = atoll(attriValue)*1000;

#elif defined _MAC_OS
		m_manifest->minBufferTime = atoll(attriValue)*1000;	
#endif 
	}
	void *pChildNode;
	m_pXmlLoad->GetFirstChild(pNode,&pChildNode);
	 char* pName_1;
  
while(pChildNode){
	   m_pXmlLoad->GetTagName(pChildNode,&pName_1,nSize);

		if(StrCompare(pName_1,TAG_DASH_BASEURL_2)==0)
		{
			
		  /*  attriValue = pChildNode->FirstChild()->Value();

		    memcpy(m_base_url,attriValue,strlen(attriValue));
			m_base_url[strlen(attriValue)] = '\0';
			pChildNode = pChildNode->NextSibling();*/

			
		}
		if(StrCompare(pName_1,"ProgramInformation")==0)
		{
		//	pChildNode = pChildNode->NextSibling();
			m_pXmlLoad->GetNextSibling(pChildNode,&pChildNode);
		//	pName_1 = pChildNode->Value();


		}
		if(StrCompare(pName_1,TAG_DASH_STREAMING_Period)==0||StrCompare(pName_1,TAG_DASH_STREAMING_MEDIA_G1)==0)
		//if(StrCompare(pName_1,TAG_DASH_STREAMING_Period)>=0)
		{
			 period_index++;
		}
	
	    
		 void *pLevel_group;
	     m_pXmlLoad->GetFirstChild(pLevel_group,&pLevel_group);

		 while(pLevel_group){
			 pre_duration = 0;

			 VO_S64 Timeline_video[1024];
			 VO_S64 Timeline_audio[1024];
			 VO_S64 Duration_video[1024];
			 VO_S64 Duration_audio[1024];

			  char* pGroup_Name ;
			  m_pXmlLoad->GetTagName(pLevel_group,&pGroup_Name,nSize);
		
			 ApaptionSet *pAdaptionSet= &adaption_set[(*group_count)];
			 pAdaptionSet->m_ptr_QL_head = pAdaptionSet->m_ptr_QL_tail = NULL;
			 if(0==StrCompare(pGroup_Name,"Title")){
				// attriValue =  pLevel_group->ToElement()->Attribute("moreInformationURL");
			  m_pXmlLoad->GetNextSibling(pLevel_group,&pLevel_group);
			 
				  
			  }

		  if(0==StrCompare(pGroup_Name,TAG_DASH_STREAMING_Group)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET_G1)||0==StrCompare(pGroup_Name, TAG_DASH_STREAMING_Group_G1)){
		//		  if(0<=StrCompare(pGroup_Name,TAG_DASH_STREAMING_Group)||0<=StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET)){
					if(m_pXmlLoad->GetAttributeValue(pLevel_group,(char*)(TAG_DASH_STREAMING_MIMETYPE),&attriValue,nSize) == VO_ERR_NONE)
					{
				    memcpy(pAdaptionSet->type,attriValue,strlen(attriValue));
					if(0==StrCompare(attriValue,"video/mp4")){
						pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
					}
					else if(0==StrCompare(attriValue,"video/mp2t")){
						pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;

					}
					else if(0==StrCompare(attriValue,"audio/mp4")){
						pAdaptionSet->nTrackType = VO_SOURCE_TT_AUDIO;

					}
					else if(0==StrCompare(attriValue,"text")){
					//	pLevel_group = pLevel_group->NextSibling();
						m_pXmlLoad->GetNextSibling(pLevel_group,&pLevel_group);
						 continue;
					
					}
					}
					else
						pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;

		}
			
				 if(m_pXmlLoad->GetAttributeValue(pLevel_group,(char*)(TAG_CODECS),&attriValue,nSize) == VO_ERR_NONE)
			
			    {

				  if (StrCompare(attriValue, "H264")== 0)
					  pAdaptionSet->nCodecType = VO_VIDEO_CodingH264;
				  else if(StrCompare(attriValue,"avc1"))
					  pAdaptionSet->nCodecType = VO_VIDEO_CodingH264;
				  else if (StrCompare(attriValue, "WVC1")== 0)
					  pAdaptionSet->nCodecType = VO_VIDEO_CodingVC1;//VOMP_VIDEO_CodingVC1;
				  else if (StrCompare(attriValue, "WmaPro")== 0 || StrCompare(attriValue, "WMAP") == 0)
					  pAdaptionSet->nCodecType = VO_AUDIO_CodingWMA;
				  else if (StrCompare(attriValue, "AACL")== 0)
					  pAdaptionSet->nCodecType = VO_AUDIO_CodingAAC;

			  }
				 pAdaptionSet->group_index = *group_count;
				(*group_count) ++;
            	void *pLevel_repre;
	            m_pXmlLoad->GetFirstChild(pLevel_group,&pLevel_repre);
			   
				   VO_CHAR   pval_media[256];
				   VO_CHAR   pval_inia[256];
				 while(pLevel_repre){ 
				 char * rpe_name ;
		         m_pXmlLoad->GetTagName(pLevel_repre,&rpe_name,nSize);
				 if(StrCompare(rpe_name, TAG_DASH_ContentComponent)==0)
				 {
					  
					   	if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_CONTENTYPE),&attriValue,nSize) == VO_ERR_NONE)
					{
						   if(StrCompare(attriValue,"audio")){
							//   attriValue = pLevel_repre->ToElement()->Attribute(TAG_LANG);
							   if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_LANG),&attriValue,nSize) == VO_ERR_NONE)
							{
								   memcpy(pAdaptionSet->audio_QL.lang,attriValue,strlen(attriValue));

							   }

						   }
					   }

					   //pLevel_repre = pLevel_repre->NextSibling();
					   m_pXmlLoad->GetNextSibling(pLevel_repre,&pLevel_repre);



				 }
				
		    if(StrCompare(rpe_name, TAG_DASH_TEMPLATE)==0||StrCompare(rpe_name, TAG_DASH_TEMPLATE_G1)==0){
			//	  if(StrCompare(rpe_name, TAG_DASH_TEMPLATE)>=0){
						
				if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_MEDIABASEURL),&attriValue,nSize) == VO_ERR_NONE)
						{
							   memcpy(pval_media ,url,strlen(attriValue)+strlen(url));
							   strcat(pval_media,attriValue);
						   }
						 if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_BASEURL_3),&attriValue,nSize) == VO_ERR_NONE)
						 {
							   memcpy(pval_inia ,url,strlen(attriValue)+strlen(url));
							   strcat(pval_inia,attriValue);
						   }
						if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_BASEURL_4),&attriValue,nSize) == VO_ERR_NONE)
						{
							   memcpy(pval_inia ,url,strlen(attriValue)+strlen(url));
							   strcat(pval_inia,attriValue);
						   }

						// attriValue = pLevel_repre->ToElement()->Attribute(TAG_SIMPLE_TIMESCALE);
						 if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_SIMPLE_TIMESCALE),&attriValue,nSize) == VO_ERR_NONE)
					{
#if defined _WIN32
							   m_manifest->timeScale = _atoi64(attriValue);
#elif defined LINUX
							   m_manifest->timeScale = atoll(attriValue);
#elif defined _IOS
							   m_manifest->timeScale = atoll(attriValue);

#elif defined _MAC_OS
							   m_manifest->timeScale = atoll(attriValue);		
#endif 
						   }
					
				
						 void * pLevel_segmentTimeLine = NULL;
						 int group_index = *group_count-1;
						 m_pXmlLoad->GetFirstChild(pLevel_repre,&pLevel_segmentTimeLine);
						 if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO){
							
						if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_STREAMING_DURATION),&attriValue,nSize) == VO_ERR_NONE)
					{
							 VO_CHAR	duration[512];
							 memcpy(duration,attriValue,strlen(attriValue));
							 replace(duration,"PT","");
							 replace(duration,"S","");
							 #if defined _WIN32
							 m_manifest->duration_video=  _atoi64(duration)*1000;
#elif defined LINUX
							 m_manifest->duration_video = atoll(duration)*1000;
#endif
 						  }
						
					if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_START_NUMBER),&attriValue,nSize) == VO_ERR_NONE)
					{
				
							 m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
						 
						}
						if(pLevel_segmentTimeLine==NULL){
							for(VO_U32 j = 0;j<m_seg_count;j++){
						     Seg_item * item = new Seg_item;
						     memset( item , 0 , sizeof(Seg_item));
						     item->starttime =  m_manifest->duration_video*j;
						     item->startnmber = j+1;
							 item->duration =  m_manifest->duration_video;
						     Timeline_video[j] = item->starttime;
						     Duration_video[j] = m_manifest->duration_video;
							 item->nIsVideo =  1;
							 item->group_index = group_index;
						     add_fragmentItem(item);
							 m_count_video++;

					}
					}

						else
							 analyze_video(Duration_video,Timeline_video,pLevel_segmentTimeLine,1,group_index);
						 }

						 else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO){
					
						if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_STREAMING_DURATION),&attriValue,nSize) == VO_ERR_NONE)
						{
							 VO_CHAR	duration[512];
							 memcpy(duration,attriValue,strlen(attriValue));
							 replace(duration,"PT","");
							 replace(duration,"S","");
							 #if defined _WIN32
							 m_manifest->duration_audio=  _atoi64(duration)*1000;
#elif defined LINUX
							   m_manifest->duration_audio = atoll(duration)*1000;
#endif
						  }
						
						 if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_START_NUMBER),&attriValue,nSize) == VO_ERR_NONE)
						{
							  m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_audio;
						 
						}

                       if(pLevel_segmentTimeLine==NULL){
						   	 for(VO_U32 j = 0;j<m_seg_count;j++){
						     Seg_item * item = new Seg_item;
						     memset( item , 0 , sizeof(Seg_item));
						     item->starttime =  m_manifest->duration_audio*j;
						     item->startnmber = j+1;
							 item->nIsVideo =  0;
							 item->duration =  m_manifest->duration_audio;
						     Timeline_audio[j] = item->starttime;
						     Duration_audio[j] = item->duration;
							 item->group_index = group_index;
						     add_fragmentItem(item);
							 m_count_audio++;

					}
						 }
					     else
							 analyze_video(Duration_audio,Timeline_audio,pLevel_segmentTimeLine,0,group_index);

						 }
						 m_seg_temlate = VO_TRUE;
						 m_pXmlLoad->GetNextSibling(pLevel_repre,&pLevel_repre);
						// pLevel_repre = pLevel_repre->NextSibling();
				 }
		if(StrCompare(rpe_name, "ok")==0){
		//	pLevel_repre = pLevel_repre->NextSibling();
			 m_pXmlLoad->GetNextSibling(pLevel_repre,&pLevel_repre);

				 }
		if(StrCompare(rpe_name, TAG_DASH_STREAMING_Representation)==0||StrCompare(rpe_name, TAG_DASH_STREAMING_Representation_G1)==0){
	//	if(StrCompare(rpe_name, TAG_DASH_STREAMING_Representation)>=0){
			    Representation *pRepresentation=new Representation;
		        memset( pRepresentation , 0 , sizeof(Representation));

				VO_CHAR	m_base_url_temp[256];
		         pRepresentation->group_index = *group_count-1;
			     memcpy(pRepresentation->mediaUrl,pval_media,strlen(pval_media));
				 memcpy(pRepresentation->initalUrl,pval_inia,strlen(pval_inia));
			 if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_STREAMING_MEDIA_ID),&attriValue,nSize) == VO_ERR_NONE)
				  {
					    memcpy(pRepresentation->id,attriValue,strlen(attriValue));

				   }

					     
					   //  attriValue = pLevel_repre->ToElement()->Attribute(TAG_DASH_STREAMING_BIRATE);
					 if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_STREAMING_BIRATE),&attriValue,nSize) == VO_ERR_NONE) 
						{
#if defined _WIN32
						   pRepresentation->bitrate = _atoi64(attriValue);
#else
						   pRepresentation->bitrate= atoll(attriValue);
#endif
					   }
					 
						//  attriValue = pLevel_repre->ToElement()->Attribute(TAG_DASH_STREAMING_MINBUFFERTIMER);
				if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_STREAMING_MINBUFFERTIMER),&attriValue,nSize) == VO_ERR_NONE) 
						    {
#if defined _WIN32
						   pRepresentation->minbufferTime = _atoi64(attriValue);
#else
						   pRepresentation->minbufferTime= atoll(attriValue);
#endif
					   }
				 if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO){

						//   attriValue = pLevel_repre->ToElement()->Attribute(TAG_DASH_STREAMING_WIDTH);
						  if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_STREAMING_WIDTH),&attriValue,nSize) == VO_ERR_NONE) 
						   {
							
						 
#if defined _WIN32
							   pRepresentation->video_QL.Width  = _atoi64(attriValue);
#else
							   pRepresentation->video_QL.Width = atoll(attriValue);
#endif
						   }
					//   if(m_pXmlLoad->GetAttributeValue(pLevel_repre,TAG_DASH_STREAMING_HEIGHT,&attriValue,nSize) == VO_ERR_NONE)
						   
					 if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_STREAMING_HEIGHT),&attriValue,nSize) == VO_ERR_NONE) 
							{
								
#if defined _WIN32
							   pRepresentation->video_QL.Height  = _atoi64(attriValue);
#else
							   pRepresentation->video_QL.Height = atoll(attriValue);
#endif
						   
							}
				        addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);
							if(m_seg_temlate){
								int i = 0;
								while(i<(m_count_video)){

									Seg_item * item = new Seg_item;
									memset(item,0,sizeof(Seg_item));
									item->starttime = Timeline_video[i];
									item->duration =  Duration_video[i];
									item->representation_index = pAdaptionSet->rpe_number;
									add_segmentItem(pRepresentation,item);
									i++;
								}	
							}
					 //   addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);

				 } //video
			     else if(pAdaptionSet->nTrackType == VO_SOURCE_TT_AUDIO)
					   {
							
								if(m_pXmlLoad->GetAttributeValue(pLevel_repre,(char*)(TAG_DASH_STREAMING_HEIGHT),&attriValue,nSize) == VO_ERR_NONE) {
							#if defined _WIN32
						   pAdaptionSet->audio_QL.bitrate  = _atoi64(attriValue);
#else
						   pAdaptionSet->audio_QL.bitrate = atoll(attriValue);
#endif
			}


							    addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);
								if(m_seg_temlate){
									int i = 0;
									while(i<m_count_audio){
										Seg_item * item = new Seg_item;
										memset(item,0,sizeof(Seg_item));
										item->starttime = Timeline_audio[i];
										item->duration = Duration_audio[i];
										item->representation_index = pAdaptionSet->rpe_number;
										add_segmentItem(pRepresentation,item);	
										i++;
									}
								}
								//addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);

					}
					 
					 void*  pLevel_segment = NULL;
					 m_pXmlLoad->GetFirstChild(pLevel_repre,&pLevel_segment);
					
	                 int  seg_index = 0;
		 while(pLevel_segment){ 
					
						 char * pTagName = NULL;
						m_pXmlLoad->GetTagName(pLevel_segment,&pTagName,nSize);
					 if(StrCompare(pTagName,TAG_DASH_SEGMENTBASE)==0){
							 void * temp = NULL;
							 m_pXmlLoad->GetFirstChild(pLevel_segment,&temp);
							 if(m_pXmlLoad->GetAttributeValue(temp,(char*)(TAG_DASH_STREAMING_SOURCEURL),&attriValue,nSize) == VO_ERR_NONE)
							 {
								 memcpy(pRepresentation->initalUrl,attriValue,strlen(attriValue));
							 }
							 if(m_pXmlLoad->GetAttributeValue(temp,(char*)(TAG_DASH_RANGE),&attriValue,nSize) == VO_ERR_NONE)
							 {
								  memcpy(pRepresentation->ini_range,attriValue,strlen(attriValue));
							 }
						//	 attriValue =  temp->ToElement()->Attribute(TAG_DASH_RANGE);
						//	 pLevel_segment =pLevel_segment->NextSibling();
							m_pXmlLoad->GetNextSibling(pLevel_segment,&pLevel_segment);
						 }//include the baseUrl  the first

					  if(StrCompare(pTagName,TAG_DASH_BASEURL_2)==0){
						   void * temp = NULL;
						   m_pXmlLoad->GetFirstChild(pLevel_segment,&temp);
						   m_pXmlLoad->GetTagName(temp,&attriValue,nSize);

						  // attriValue = pLevel_segment->FirstChild()->Value();
						  if(m_base_url[0]!='\0'){
						//	   memset(m_base_url,0,strlen(m_base_url)+strlen(attriValue));
							   memcpy(m_base_url_temp ,m_base_url,strlen(attriValue)+strlen(m_base_url));
							   strcat(m_base_url_temp,attriValue);
							  m_base_url_temp[strlen(attriValue)+strlen(m_base_url)] = '\0';
							
						  }
                         m_pXmlLoad->GetNextSibling(pLevel_segment,&pLevel_segment);

					  }

					
					
				  
				
					if(StrCompare(pTagName,TAG_DASH_STREAMING_SegmentInfo)==0||StrCompare(pTagName,TAG_DASH_SEGMENTLIST)==0){
						
					if(m_pXmlLoad->GetAttributeValue(pLevel_segment,(char*)(TAG_DASH_STREAMING_DURATION),&attriValue,nSize) == VO_ERR_NONE)
							{
								
								 VO_CHAR	dur[512];
								 memcpy(dur,attriValue,strlen(attriValue));
								
								 replace(dur,"PT","");
								 replace(dur,"S","");

								 //pszVal = pLevel_segment->ToElement()->Attribute(TAG_DASH_STREAMING_DURATION);
#if defined _WIN32
								 pRepresentation->duration = _atoi64(dur)*1000;
#elif defined LINUX
								 pRepresentation->duration =  atoll(dur)*1000;
#elif defined _IOS
								 pRepresentation->duration = atoll(dur)*1000;
#elif defined _MAC_OS
								 pRepresentation->duration = atoll(dur)*1000;
#endif
							 }
					 

					    	void * pLevel_segment_1 = NULL;
							m_pXmlLoad->GetFirstChild(pLevel_segment,&pLevel_segment_1);
							 char * pTagName_1 = NULL;
							int segment_Index  = 0;
						//    char * base_url = NULL;
							VO_CHAR	base_url[512];
						    memcpy(base_url ,url,strlen(url));

							while(pLevel_segment_1){
							
	                   
							    m_pXmlLoad->GetTagName(pLevel_segment_1,&pTagName_1,nSize);
						
							 if(StrCompare(pTagName_1,TAG_DASH_BASEURL_2)==0){
								  
						if(m_pXmlLoad->GetAttributeValue(pLevel_segment_1,(char*)(TAG_DASH_BASEURL_2),&attriValue,nSize) == VO_ERR_NONE)
								{
										 if(strstr(attriValue , ("http://"))){
											  memcpy(base_url ,attriValue,strlen(attriValue));

										 }
										 else{
											 if(m_base_url[0]!='\0'){
                                            memcpy(m_base_url_temp ,url,strlen(attriValue)+strlen(url));

											 }
											 else
											 {
	                                       memcpy(base_url ,url,strlen(attriValue)+strlen(url));
											 }
									
										strcat(base_url,attriValue);
										replace(base_url,"Manifest.mpd","");
										replace(base_url,"vlcManifestVideo.mpd","");
										replace(base_url,"vlcManifestAudio.mpd","");
										replace(base_url,"vlc","");
										 
										 }
								
								 }
									  // pLevel_segment_1 = pLevel_segment_1->NextSibling();
									  m_pXmlLoad->GetNextSibling(pLevel_segment_1,&pLevel_segment_1);

							 }


							 if(StrCompare(pTagName_1,TAG_DASH_STREAMING_InitialisationSegmentURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL)==0){

									
						if(m_pXmlLoad->GetAttributeValue(pLevel_segment_1,(char*)(TAG_DASH_STREAMING_SOURCEURL),&attriValue,nSize) == VO_ERR_NONE)

									{
										 if(strstr(attriValue , ("http://")))
										 {
											 VOLOGR( "It is HTTP url Streaming!" );
											 memcpy(pRepresentation->initalUrl ,attriValue,strlen(attriValue));

										 }
										 else{ 
											 if(m_base_url_temp[0]!='\0'){
												// memcpy(m_base_url ,url,strlen(attriValue)+strlen(url));  
												 memcpy(pRepresentation->initalUrl ,m_base_url_temp,strlen(attriValue)+strlen(m_base_url));

											 }
											 else
											 {
												 memcpy(pRepresentation->initalUrl ,url,strlen(attriValue)+strlen(url));
											 }
									   
										   strcat(pRepresentation->initalUrl,attriValue);
										   replace(pRepresentation->initalUrl,"Manifest.mpd","");
										   replace(pRepresentation->initalUrl,"vlcManifestVideo.mpd","");
										   replace(pRepresentation->initalUrl,"vlcManifestAudio.mpd","");
										   replace(pRepresentation->initalUrl,"vlc","");
										 }
									 }
							//	pLevel_segment_1 =   pLevel_segment_1->NextSibling();
							   m_pXmlLoad->GetNextSibling(pLevel_segment_1,&pLevel_segment_1);

							 }

						 if(StrCompare(pTagName_1,TAG_DASH_STREAMING_URL)==0||StrCompare(pTagName_1,TAG_DASH_SEGURL)==0){

							 Seg_item * ptr_item = new Seg_item;
							 memset( ptr_item , 0 , sizeof(Seg_item));
							 int group_index = *group_count-1;
							
							 ptr_item->group_index = group_index;
							// attriValue = pLevel_segment_1->ToElement()->Attribute(TAG_DASH_RANGE);
							if(m_pXmlLoad->GetAttributeValue(pLevel_segment_1,(char*)(TAG_DASH_RANGE),&attriValue,nSize) == VO_ERR_NONE)

						
							 { 	
								 memcpy(ptr_item->range,attriValue,strlen(attriValue));
							 }
							 if(m_pXmlLoad->GetAttributeValue(pLevel_segment_1,TAG_DASH_MEDIABASEURL,&attriValue,nSize) == VO_ERR_NONE){
							
								 
								 if(strstr(attriValue , ("http://"))){
									 VOLOGR( "It is HTTP url Streaming!" );
									 memcpy(ptr_item->item_url ,attriValue,strlen(attriValue));
									 ptr_item->starttime = pRepresentation->duration*seg_index;
									 seg_index++;

								 }
								 else if(strstr(attriValue , ("."))){
									 if(m_base_url[0]!='\0'){
										 memcpy(ptr_item->item_url ,m_base_url_temp,strlen(attriValue)+strlen(base_url));
									 }
									 else
										  memcpy(ptr_item->item_url ,base_url,strlen(attriValue)+strlen(base_url));

									 strcat(ptr_item->item_url,attriValue);
									 ptr_item->starttime = pRepresentation->duration*seg_index;
									 seg_index++;


								 }
								 else
								 {

									 int le= strlen(attriValue);
									 const char * length = NULL;
									 const char * find_url = "/";
									 int index = lastIndex(attriValue,find_url);
									 le = le-index-1;
									 length = attriValue+index+1;

#if defined _WIN32
									 ptr_item->starttime= _atoi64(length);
#elif defined LINUX
									 ptr_item->starttime=  atoll(length);
#elif defined _IOS
									 ptr_item->starttime = atoll(length);
#elif defined _MAC_OS
									 ptr_item->starttime = atoll(length);
#endif		 

									 memcpy(ptr_item->item_url,url,strlen(url)+strlen(attriValue));
									 strcat(ptr_item->item_url,attriValue);
									  replace(ptr_item->item_url,"vlcManifestVideo.mpd","");
									  replace(ptr_item->item_url,"vlcManifestAudio.mpd","");
									 replace(ptr_item->item_url,"Manifest.mpd","");
									 replace(ptr_item->item_url,"vlc","");

								 }

							
							 }
							 if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
							 {
								 ptr_item->nIsVideo = 1;
							 }
							 else
								 ptr_item->nIsVideo = 0;
							
							// attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_STREAMING_SOURCEURL);
							  if(m_pXmlLoad->GetAttributeValue(pLevel_segment_1,TAG_DASH_STREAMING_SOURCEURL,&attriValue,nSize) == VO_ERR_NONE)
							  {
							
							
							 if(strstr(attriValue , ("http://"))){
										 VOLOGR( "It is HTTP url Streaming!" );
										 memcpy(ptr_item->item_url ,attriValue,strlen(attriValue));
										 ptr_item->starttime = pRepresentation->duration*seg_index;
										 seg_index++;

									 }
						
							 else if(strstr(attriValue , ("."))){
								 memcpy(ptr_item->item_url ,base_url,strlen(attriValue)+strlen(base_url));
								 strcat(ptr_item->item_url,attriValue);
								 ptr_item->starttime = pRepresentation->duration*seg_index;
								 seg_index++;

							  }
							 else
							 {
								 int le= strlen(attriValue);
								 const char * length = NULL;
								 const char * find_url = "/";
								 int index = lastIndex(attriValue,find_url);
								 le = le-index-1;
								 length = attriValue+index+1;
#if defined _WIN32
								 ptr_item->starttime= _atoi64(length);
#elif defined LINUX
							     ptr_item->starttime=  atoll(length);
#elif defined _IOS
								 ptr_item->starttime = atoll(length);
#elif defined _MAC_OS
								  ptr_item->starttime = atoll(length);
#endif		 

								  memcpy(ptr_item->item_url,url,strlen(url)+strlen(attriValue));
								  strcat(ptr_item->item_url,attriValue);
								  replace(ptr_item->item_url,"vlcManifestVideo.mpd","");
								  replace(ptr_item->item_url,"vlcManifestAudio.mpd","");
								  replace(ptr_item->item_url,"vlcManifest.mpd","");
								  replace(ptr_item->item_url,"vlc","");
								}

							 }  
							 ptr_item->duration = pRepresentation->duration;
                             
               				 ptr_item->segment_index = segment_Index;//start from
			            	
							 if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO ){
								 if(video_flag ==0&&pRepresentation->rpe_item_index ==0){
                                  add_fragmentItem(ptr_item); //add one time
								 }
								
							 }
							 else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO&&pRepresentation->rpe_item_index ==0){
								 if(audio_flag==0){
									add_fragmentItem(ptr_item);//add one time
									
								 }
								
							 }

					        
							 add_segmentItem(pRepresentation,ptr_item);
							 segment_Index++;
						 //    pLevel_segment_1 =  pLevel_segment_1->NextSibling();
							 m_pXmlLoad->GetNextSibling(pLevel_segment_1,&pLevel_segment_1);
					 }   
							
			}		
			          // pLevel_segment= pLevel_segment->NextSibling();
					   m_pXmlLoad->GetNextSibling(pLevel_segment,&pLevel_segment);

 }//segment_end	
			
}           

//add by new

//added by new

  m_pXmlLoad->GetNextSibling(pLevel_repre,&pLevel_repre);

}
     
 
} //representation end
        if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO)video_flag++;
		else
		if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO) audio_flag++;
		if(pLevel_group){
    
		  m_pXmlLoad->GetNextSibling(pLevel_group,&pLevel_group);
		}

}
       //pChildNode = pChildNode->NextSibling();
	    m_pXmlLoad->GetNextSibling(pChildNode,&pChildNode);

}

    ApaptionSet *streamIndexc = new ApaptionSet[m_manifest->group_count];
	memcpy(streamIndexc,adaption_set,sizeof(ApaptionSet)*m_manifest->group_count);
	m_manifest->adaptionSet = streamIndexc;
	
	return VO_TRUE;

}

#endif



VO_BOOL  vo_mpd_reader::analyze_2(VO_CHAR * url )
{

	real_count = 0;
	m_seg_count = 0;
	TiXmlDocument doc;
	memset(m_manifest, 0, sizeof(MpdStreamingMedia));
	m_manifest->group_count = 0;
	VO_S32 *group_count       = &m_manifest->group_count;
	ApaptionSet adaption_set[10];
	memset(adaption_set,0x00,sizeof(ApaptionSet) * 10);
	if (!doc.LoadFile((char*)m_file_content, m_file_content_size, TIXML_DEFAULT_ENCODING))
	{
		VOLOGR("Cannot Load manifest file on parsing!" );
		return VO_FALSE;
	}
     TiXmlNode* pNode = NULL;
	if(strstr(url,"/verizon.mpd")){
		 pNode = doc.FirstChild(TAG_DASH_STREAMING_MEDIA_G1);
	}
	else
	    pNode = doc.FirstChild(TAG_DASH_STREAMING_MEDIA);
	
	char * url_temp   = strrchr(url,'/');
	int len = strlen(url)- strlen(url_temp);
	url[len+1] = '\0';
	VOLOGR("Analyze %s",url);
	if (!pNode) 
		return VO_FALSE;
	const char* attriValue		= NULL;
	int nVal				= 0; 
	TiXmlNode* pChildNode	= NULL;
    TiXmlNode* pLevel_repre = NULL;
	VO_S64 period_index = 0;
    VO_S64 pre_duration = 0;
	
	unknown_flag = 0;
	
	mpd_tag.Init(pNode);
	m_is_need_update = mpd_tag.GetLiveType();
	if(mpd_tag.GetProfile() == 0||mpd_tag.GetProfile() ==1)
	{
		m_is_live = VO_FALSE;
		m_manifest->islive = VO_FALSE;
	}
	else if(mpd_tag.GetProfile()==2)
	{
		m_is_live = VO_TRUE;
		m_manifest->islive = VO_TRUE;

	}
	else if(mpd_tag.GetProfile()==3)
	{
		unknown_flag = 1;

	}
	m_is_need_update = mpd_tag.GetLiveType();
	m_update_period =  mpd_tag.GetUpdatePeriod();
	m_manifest->mediaPresentationDuration = mpd_tag.GetDuration();

    pChildNode = pNode->FirstChild();
	while(pChildNode)
	{
		const char* pName_1 = pChildNode->Value();
		if(StrCompare(pName_1,TAG_DASH_BASEURL_2)==0)
		{
			m_base_url_el.Init(pChildNode);

			;//pChildNode = pChildNode->NextSibling();
		}
		else if(StrCompare(pName_1,"ProgramInformation")==0)
		{
			m_program.Init(pChildNode);
			pChildNode = pChildNode->NextSibling();
			pName_1 = pChildNode->Value();
		}
		else if(StrCompare(pName_1,"Location")==0)
		{
			pChildNode = pChildNode->NextSibling();
		}
		else if(StrCompare(pName_1,"Metrics")==0)
		{
			m_metrics.Init(pChildNode);
			pChildNode = pChildNode->NextSibling();
			pName_1 = pChildNode->Value();

		}
		else if(StrCompare(pName_1,TAG_DASH_STREAMING_Period)==0||StrCompare(pName_1,TAG_DASH_STREAMING_MEDIA_G1)==0)
		{
			m_period_tag.Init(pChildNode);
			if(m_period_tag.GetDuration())
			{
				m_manifest->mediaPresentationDuration = m_period_tag.GetDuration();

			}
			 period_index++;
		}
		TiXmlNode * pLevel_group = pChildNode->FirstChild();
		while(pLevel_group)
		{
			pre_duration = 0;

			const char* pGroup_Name = pLevel_group->Value();
			ApaptionSet *pAdaptionSet= &adaption_set[(*group_count)];
			if(0==StrCompare(pGroup_Name,"Title"))
			{
				attriValue =  pLevel_group->ToElement()->Attribute("moreInformationURL");
				;//pLevel_group = pLevel_group->NextSibling();
			}
			else if(0==StrCompare(pGroup_Name,"Video AdaptationSet"))
			{
				pLevel_group = pLevel_group->FirstChild();
				break;
			}
			else if(0==StrCompare(pGroup_Name,"Audio AdaptationSet"))
			{
				 pLevel_group = pLevel_group->FirstChild();
				 pGroup_Name = pLevel_group->Value();
				 break;
			}
			else if(0==StrCompare(pGroup_Name,TAG_DASH_STREAMING_Group)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET_G1)||0==StrCompare(pGroup_Name, TAG_DASH_STREAMING_Group_G1)||0==StrCompare(pGroup_Name, "ContentComponent"))
			 {
				 VO_CHAR	  m_base_url[256];
				 m_base_url[0]='\0';
				 m_grouptag = new Group_Tag();
				 m_grouptag->Init(pLevel_group);
				 pAdaptionSet->nTrackType = m_grouptag->GetTrackType();
				 memcpy(pAdaptionSet->lang ,m_grouptag->GetLang(),strlen(m_grouptag->GetLang()));
				 pAdaptionSet->nCodecType  = m_grouptag->GetCodecs();
				 int ts_component = 0;
				 pAdaptionSet->group_index = *group_count;
				(*group_count) ++;
				 group_number = *group_count;
			     pLevel_repre= pLevel_group->FirstChild();// 
				 VO_CHAR   pval_media[256];
				 VO_CHAR   pval_inia[256];
				 while(pLevel_repre)
				 { 
					const char * rpe_name = NULL;
					rpe_name = pLevel_repre->Value();
					if(StrCompare(rpe_name,TAG_DASH_BASEURL_2)==0)
					{
					 attriValue = pLevel_repre->FirstChild()->Value();
					 memcpy(m_base_url,attriValue,strlen(attriValue));
					 m_base_url[strlen(attriValue)] = '\0';
					 //pLevel_repre = pLevel_repre->NextSibling();
					}
					else if(0==StrCompare(rpe_name, "ContentComponent"))
					{
                        ts_component++;
						if( pAdaptionSet->group_index ==1&&ts_component==2)
						{
							is_TS = VO_TRUE;
						}

						 com_tag =  new contentComponent_Tag();
						 com_tag->Init(pLevel_repre);

						 if(StrCompare(com_tag->GetContentType(),"video/mp4")==0)
						 {
							 pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;

						 }
						 else if(StrCompare(com_tag->GetContentType(),"video")==0)
						 {
							 pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
						 }
						 else if(StrCompare(com_tag->GetContentType(),"video/mp2t")==0)
						 {
							 pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
						 }
						 else if(StrCompare(com_tag->GetContentType(),"audio/mp4")==0||0==StrCompare(com_tag->GetContentType(),"audio"))
						 {
							 pAdaptionSet->nTrackType = VO_SOURCE_TT_AUDIO;
						 }
						 else if(StrCompare(com_tag->GetContentType(),"text"))
						 {
							 pLevel_group = pLevel_group->NextSibling();
							 continue;
						 }
					}
					
					else if(StrCompare(rpe_name, TAG_DASH_TEMPLATE)==0||StrCompare(rpe_name, TAG_DASH_TEMPLATE_G1)==0)
					{
						 int ini_number = 1;
						 m_template_tag = new SegmentTemplate_Tag();
						 m_template_tag->Init(pLevel_repre);
						 if(m_template_tag->GetMediaUrl())
						 {
							 if(m_base_url[0]!='\0')
							 {
								int len = strlen(m_base_url)+strlen(url)+strlen(m_template_tag->GetMediaUrl());
								memcpy(pval_media ,url,strlen(m_base_url)+strlen(url)+strlen(m_template_tag->GetMediaUrl()));
								strcat(pval_media,m_base_url);
								strcat(pval_media,m_template_tag->GetMediaUrl());
								pval_media[len]= '\0';
							 }
							 else
							 {
								 int len = strlen(m_base_url)+strlen(url)+strlen(m_template_tag->GetMediaUrl());
								 memcpy(pval_media ,url,strlen(m_template_tag->GetMediaUrl())+strlen(url));
								 strcat(pval_media,m_template_tag->GetMediaUrl());
								 pval_media[len]= '\0';

							 }
						 }
						 
						 if(m_template_tag->GetIniaUrl())
						 {
							if(m_base_url[0]!='\0')
							{
								memcpy(pval_inia ,url,strlen(m_base_url)+strlen(url)+strlen(m_template_tag->GetIniaUrl()));
								strcat(pval_inia,m_base_url);
								strcat(pval_inia,m_template_tag->GetIniaUrl());
							}
							else
							{
								int len = strlen(m_template_tag->GetIniaUrl())+strlen(url);
								memcpy(pval_inia ,url,strlen(m_template_tag->GetIniaUrl())+strlen(url));
								strcat(pval_inia,m_template_tag->GetIniaUrl());
								pval_inia[len]= '\0';
							}
						 }

						 TiXmlNode *pLevel_segmentTimeLine = NULL;
						 int group_index = *group_count-1;
						 pLevel_segmentTimeLine= pLevel_repre->FirstChild();
						 if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
						 {
							 m_manifest->timeScale  = m_template_tag->GetTimeScale();
							 video_scale = m_manifest->timeScale;
							 audio_scale = video_scale;
							 m_manifest->duration_video = m_template_tag->GetDuration();
							 if(m_template_tag->GetStart_number())
							 {
								 if(m_manifest->duration_video==0)
									m_seg_count = 0;
								 else
								 {
									 m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
									 if(m_seg_count==0)
									{
										m_seg_count =200;

									}
								 }
								 ini_number  = m_template_tag->GetStart_number();

							 }
						

							 if(pLevel_segmentTimeLine==NULL)
							 {
								 for(VO_U32 j = 0;j<m_seg_count;j++)
								 {
									 Seg_item * item = new Seg_item;
									 memset( item , 0 , sizeof(Seg_item));
									 item->starttime =  m_manifest->duration_video*j;
									 item->startnmber = ini_number+j;
									 item->duration =  m_manifest->duration_video;
									 Timeline_video[j] = item->starttime;
									 Duration_video[j] = m_manifest->duration_video;
									 item->nIsVideo =  1;
									 item->group_index = group_index;
									 m_count_video++;
									 add_fragmentItem(item);
								 }
							 }
							 else
								analyze_video(Duration_video,Timeline_audio,pLevel_segmentTimeLine,1,group_index);
						 }
						 else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO)
						 {
							  m_manifest->timeScale  = m_template_tag->GetTimeScale();
							  audio_scale = m_manifest->timeScale;
							  m_seg_count = 0;
							  m_manifest->duration_audio = m_template_tag->GetDuration();
							  if(m_template_tag->GetStart_number())
							  {
								if(m_manifest->duration_audio==0)
									m_seg_count = 0;
								else
								{
									m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
									if(m_seg_count==0)
									{
										m_seg_count =200;
									}

								}
								ini_number = m_template_tag->GetStart_number();
							}
							  else
								  m_seg_count = 0;


							if(pLevel_segmentTimeLine==NULL)
							{
								 for(VO_U32 j = 0;j<m_seg_count;j++)
								 {
									 Seg_item * item = new Seg_item;
									 memset( item , 0 , sizeof(Seg_item));
									 item->starttime =  m_manifest->duration_video*j; 
									 item->startnmber = ini_number+j;
									 item->nIsVideo =  0;
									 item->duration =  m_manifest->duration_video;
									 Timeline_audio[j] = item->starttime;
									 Duration_audio[j] = item->duration;
									 item->group_index = group_index;
									 m_count_audio++;
									 add_fragmentItem(item);
								 }
							}
							else
								analyze_video(Duration_audio,Timeline_audio,pLevel_segmentTimeLine,0,group_index);
						}
						m_seg_temlate = VO_TRUE;
						//pLevel_repre = pLevel_repre->NextSibling();
					}
					else if(StrCompare(rpe_name, "ok")==0)
					{
					;//pLevel_repre = pLevel_repre->NextSibling();
					}
					else if(StrCompare(rpe_name, TAG_DASH_STREAMING_Representation)==0||StrCompare(rpe_name, TAG_DASH_STREAMING_Representation_G1)==0)
					{
						Representation *pRepresentation=new Representation;
						memset( pRepresentation , 0 , sizeof(Representation));
						m_rpe_tag->Init(pLevel_repre);
						VO_CHAR	m_base_url_temp[256];
						pRepresentation->track_count = m_rpe_tag->GetTrackCount();
						pRepresentation->group_index = *group_count-1;
					//	if(!m_template_tag->GetMediaUrl())
						memcpy(pRepresentation->mediaUrl,pval_media,strlen(pval_media));
						memcpy(pRepresentation->initalUrl,pval_inia,strlen(pval_inia));
						if(m_rpe_tag->GetTrackType() !=VO_SOURCE_TT_MAX)
						pAdaptionSet->nTrackType = m_rpe_tag->GetTrackType();
						if(m_rpe_tag->GetID())
						memcpy(pRepresentation->id,m_rpe_tag->GetID(),strlen(m_rpe_tag->GetID()));
						pRepresentation->bitrate = m_rpe_tag->GetBandWidth();
						if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
						{
							pRepresentation->video_QL.Width  = m_rpe_tag->GetWidth();
							pRepresentation->video_QL.Height  = m_rpe_tag->GetHeight();
							addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);
							if(m_seg_temlate)
							{
								int i = 0;
								while(i<(m_count_video))
								{
									Seg_item * item = new Seg_item;
									VOLOGR("new Seg_item = %p",item);
									memset(item,0,sizeof(Seg_item));
									item->starttime = Timeline_video[i];
									item->duration =  Duration_video[i];
									item->representation_index = pAdaptionSet->rpe_number;
									add_segmentItem(pRepresentation,item);				
									i++;
								}
							}
						} //video
						else if(pAdaptionSet->nTrackType == VO_SOURCE_TT_AUDIO)
						{
							pAdaptionSet->audio_QL.bitrate =  m_rpe_tag->GetBandWidth();
							addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);
							if(m_seg_temlate)
							{
								int i = 0;
								while(i<m_count_audio)
								{
									Seg_item * item = new Seg_item;
									VOLOGR("new Seg_item2 = %p",item);
									memset(item,0,sizeof(Seg_item));
									item->starttime = Timeline_audio[i];
									item->duration = Duration_audio[i];
									item->representation_index = pAdaptionSet->rpe_number;
									add_segmentItem(pRepresentation,item);
									i++;
								}
							}
						}//endof if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)

						TiXmlNode*  pLevel_segment = NULL;
						pLevel_segment = pLevel_repre->FirstChild();
						int  seg_index = 0;
						while(pLevel_segment)
						{
							const char * pTagName = NULL;
							pTagName = pLevel_segment->Value();
							if(StrCompare(pTagName,TAG_DASH_TEMPLATE)==0)
							{ 
								int ini_number = 1;
								m_template_tag->Init(pLevel_segment);
								if(m_template_tag->GetMediaUrl())
								{
									if(m_base_url[0]!='\0')
									{
										int len = strlen(m_base_url)+strlen(url)+strlen(m_template_tag->GetMediaUrl());
										memcpy(pval_media ,url,strlen(m_base_url)+strlen(url)+strlen(m_template_tag->GetMediaUrl()));
										strcat(pval_media,m_base_url);
										strcat(pval_media,m_template_tag->GetMediaUrl());
										pval_media[len]= '\0';
									}
									else
									{
										memcpy(pval_media ,url,strlen(m_template_tag->GetMediaUrl())+strlen(url));
										int len = strlen(m_template_tag->GetMediaUrl())+strlen(url);
										strcat(pval_media,m_template_tag->GetMediaUrl());
										pval_media[len] = '\0';
									}

								}
								if(m_template_tag->GetIniaUrl())
								{
									 if(m_base_url[0]!='\0')
									{
										int len = strlen(m_base_url)+strlen(url)+strlen(m_template_tag->GetIniaUrl());
										memcpy(pval_inia ,url,len);
										strcat(pval_inia,m_base_url);
										strcat(pval_inia,m_template_tag->GetIniaUrl());
										pval_inia[len]= '\0';
									}
									else
									{
										memcpy(pval_inia ,url,strlen(m_template_tag->GetIniaUrl())+strlen(url));
										int len = strlen(m_template_tag->GetIniaUrl())+strlen(url);
										strcat(pval_inia,m_template_tag->GetIniaUrl());
										pval_inia[len] = '\0';
									}

								}
								//	memcpy(pRepresentation->mediaUrl,pval_media,strlen(pval_media));
								  
						             memcpy(pRepresentation->initalUrl,pval_inia,strlen(pval_inia));
                                     int length_ini = strlen(pRepresentation->initalUrl);
									 pRepresentation->mediaUrl[length_ini] = '\0';
									 int length = strlen(pRepresentation->mediaUrl);
									 memcpy(pRepresentation->mediaUrl,pval_media,strlen(pval_media));
									 length = strlen(pval_media);
									 pRepresentation->mediaUrl[length] = '\0';
								TiXmlNode *p_child = NULL;
								int group_index = *group_count-1;
								p_child= pLevel_segment->FirstChild();

								if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
								 {
									 m_template_tag->Init(pLevel_segment);
									 m_manifest->timeScale = m_template_tag->GetTimeScale();
									 video_scale = m_manifest->timeScale;
									 audio_scale = video_scale;
									  m_manifest->duration_video = m_template_tag->GetDuration();
									 if(m_template_tag->GetStart_number())
									 {
										 if(m_manifest->duration_video==0)
											 m_seg_count = 0;
										 else
										 {
											 m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
											 if(m_seg_count==0)
											 {
												 m_seg_count =300;
											 }
										 }
										 ini_number  = m_template_tag->GetStart_number();
									 }
									 if(p_child==NULL)
									 {
										 for(VO_U32 j = 0;j<m_seg_count;j++)
										 {
										 Seg_item * item = new Seg_item;
										 memset( item , 0 , sizeof(Seg_item));
										item->starttime =  m_manifest->duration_video*j;
										item->startnmber = ini_number+j;
										 item->duration =  m_manifest->duration_video;
										 Timeline_video[j] = item->starttime;
										 Duration_video[j] = m_manifest->duration_video;
										 item->nIsVideo =  1;
										 item->group_index = group_index;
										 m_count_video++;
										 if(pRepresentation->track_count!=2)
										 add_fragmentItem(item);
										 Seg_item * p_item = new Seg_item;
										 memset( p_item , 0 , sizeof(Seg_item));
										 p_item->starttime =  m_manifest->duration_video*j; 
										 p_item->startnmber = ini_number+j;
										 p_item->nIsVideo =  1;
										 p_item->duration =  m_manifest->duration_video;
										 Timeline_audio[j] = p_item->starttime;
										 Duration_audio[j] = p_item->duration;
										 p_item->group_index = group_index;
										 add_segmentItem(pRepresentation,p_item);
								 }
							 }
							 else
								analyze_video(Duration_video,Timeline_audio,p_child,1,group_index);
								//add for the representation
							
					}
						    else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO)
						 {
							  m_manifest->timeScale  = m_template_tag->GetTimeScale();
							  audio_scale = m_manifest->timeScale;
							  m_manifest->duration_audio = m_template_tag->GetDuration();
							  if(m_template_tag->GetStart_number())
							  {
								if(m_manifest->duration_audio==0)
									m_seg_count = 0;
								else
								{
									m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
									if(m_seg_count==0)
									{
										m_seg_count =200;
									}

								}
								ini_number = m_template_tag->GetStart_number();
							}

							if(p_child==NULL)
							{
								 for(VO_U32 j = 0;j<m_seg_count;j++)
								 {
									 Seg_item * item = new Seg_item;
									 memset( item , 0 , sizeof(Seg_item));
									 item->starttime =  m_manifest->duration_video*j; 
									 item->startnmber = ini_number+j;
									 item->nIsVideo =  0;
									 item->duration =  m_manifest->duration_video;
									 Timeline_audio[j] = item->starttime;
									 Duration_audio[j] = item->duration;
									 item->group_index = group_index;
									 m_count_audio++;
									 if(pRepresentation->track_count!=2)
									 add_fragmentItem(item);
									 Seg_item * p_item = new Seg_item;
									 memset( p_item, 0 , sizeof(Seg_item));
									 p_item->starttime =  m_manifest->duration_video*j; 
									 p_item->startnmber = ini_number+j;
									 p_item->nIsVideo =  0;
									 p_item->duration =  m_manifest->duration_video;
									 Timeline_audio[j] = p_item->starttime;
									 Duration_audio[j] = p_item->duration;
									 p_item->group_index = group_index;
									 add_segmentItem(pRepresentation,p_item);
								 }
							}
							else
								analyze_video(Duration_audio,Timeline_audio,p_child,0,group_index);
						}
							}
							if(StrCompare(pTagName,TAG_DASH_HOLDER)==0)
							{
							

							}
							else if(StrCompare(pTagName,TAG_DASH_SEGMENTBASE)==0)
							{
								TiXmlNode * temp = NULL;
								temp=  pLevel_segment->FirstChild();
								m_seg_base->Init(temp);
								m_seg_base->GetTimeScale();
								if(m_seg_base->GetIniUrl())
								{
									 if(m_base_url[0]!='\0')
									{
										int len = strlen(m_base_url)+strlen(url)+strlen(m_seg_base->GetIniUrl());
										memcpy(pval_inia ,url,len);
										strcat(pval_inia,m_base_url);
										strcat(pval_inia,attriValue);
										pval_media[len]= '\0';
									}
									else
									{
										memcpy(pval_inia ,url,strlen(m_seg_base->GetIniUrl())+strlen(url));
										int len = strlen(m_seg_base->GetIniUrl())+strlen(url);
										strcat(pval_inia,m_seg_base->GetIniUrl());
										pval_inia[len] = '\0';
									}
									 memcpy(pRepresentation->initalUrl,pval_inia,strlen(pval_inia));

								}
								int range_len = strlen(m_seg_base->GetIni_range());
								if(range_len!=0)
									memcpy(pRepresentation->ini_range,attriValue,strlen(attriValue));
								//pLevel_segment =pLevel_segment->NextSibling();
							}//include the baseUrl  the first
							else if(StrCompare(pTagName,TAG_DASH_BASEURL_2)==0)
							{
								attriValue = pLevel_segment->FirstChild()->Value();
								if(m_base_url[0]!='\0')
								{
								//	   memset(m_base_url,0,strlen(m_base_url)+strlen(attriValue));
									memcpy(m_base_url_temp ,m_base_url,strlen(attriValue)+strlen(m_base_url));
									strcat(m_base_url_temp,attriValue);
									m_base_url_temp[strlen(attriValue)+strlen(m_base_url)] = '\0';

								}
								//pLevel_segment =pLevel_segment->NextSibling();
							}
							else if(StrCompare(pTagName,TAG_DASH_STREAMING_SegmentInfo)==0||StrCompare(pTagName,TAG_DASH_SEGMENTLIST)==0)
							{
								m_seg_info->Init(pLevel_segment);
								pRepresentation->duration = m_seg_info->GetDuration()*1000;
								if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
								{
									m_manifest->timeScale = m_seg_info->GetTimeScale();
									video_scale = m_manifest->timeScale;
									audio_scale = video_scale;
								}
								else if(pAdaptionSet->nTrackType == VO_SOURCE_TT_AUDIO)
								{
									m_manifest->timeScale = m_seg_info->GetTimeScale();
									audio_scale = video_scale;
								}

								TiXmlNode * pLevel_segment_1 = NULL;
								pLevel_segment_1 = pLevel_segment->FirstChild();
								const char * pTagName_1 = NULL;
								int segment_Index  = 0;
								VO_CHAR	base_url[512];
								memcpy(base_url ,url,strlen(url));
								while(pLevel_segment_1)
								{
									pTagName_1 = pLevel_segment_1->Value();
									if(StrCompare(pTagName_1,TAG_DASH_BASEURL_2)==0)
									{
										 attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_BASEURL_2);
										 if(attriValue)
										 {
											 if(strstr(attriValue , ("http://")))
											 {
												  memcpy(base_url ,attriValue,strlen(attriValue));
											 }
											 else
											 {
												if(m_base_url[0]!='\0')
												{
													memcpy(m_base_url_temp ,url,strlen(attriValue)+strlen(url));
												}
												else
												{
													memcpy(base_url ,url,strlen(attriValue)+strlen(url));
												}
									
												strcat(base_url,attriValue);
												replace(base_url,"Manifest.mpd","");
												replace(base_url,"vlcManifestVideo.mpd","");
												replace(base_url,"vlcManifestAudio.mpd","");
												replace(base_url,"vlc",""); 
											}
										}//endof  if(attriValue)
										//pLevel_segment_1 = pLevel_segment_1->NextSibling();
									}//endof if(StrCompare(pTagName_1,TAG_DASH_BASEURL_2)==0)
									else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_InitialisationSegmentURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL_3)==0)
									{
										 attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_STREAMING_SOURCEURL);
										 if(attriValue)
										 {
											 if(strstr(attriValue , ("http://")))
											 {
												 VOLOGR( "It is HTTP url Streaming!" );
												 memcpy(pRepresentation->initalUrl ,attriValue,strlen(attriValue));
											 }
											 else
											 { 
												 if(m_base_url[0]!='\0')
												 {
													memcpy(pRepresentation->initalUrl ,m_base_url_temp,strlen(attriValue)+strlen(m_base_url));
												 }
												 else
												 {
													 memcpy(pRepresentation->initalUrl ,url,strlen(attriValue)+strlen(url));
												 }
										   
												strcat(pRepresentation->initalUrl,attriValue);
												replace(pRepresentation->initalUrl,"Manifest.mpd","");
												replace(pRepresentation->initalUrl,"vlcManifestVideo.mpd","");
												replace(pRepresentation->initalUrl,"vlcManifestAudio.mpd","");
												replace(pRepresentation->initalUrl,"vlc","");
											}
										}
										//pLevel_segment_1 =   pLevel_segment_1->NextSibling();
									}//enfof 	else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_InitialisationSegmentURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL_3)==0)
									else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_URL)==0||StrCompare(pTagName_1,TAG_DASH_SEGURL)==0)
									{
										 Seg_item * ptr_item = new Seg_item;
										 memset( ptr_item , 0 , sizeof(Seg_item));
										 int group_index = *group_count-1;
										 ptr_item->group_index = group_index;
										 attriValue = pLevel_segment_1->ToElement()->Attribute(TAG_DASH_RANGE);
										 if(attriValue)
										 { 	
											 memcpy(ptr_item->range,attriValue,strlen(attriValue));
										 }

										 attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_MEDIABASEURL);
										 if(attriValue)
										 {
											if(strstr(attriValue , ("http://")))
											{
												 VOLOGR( "It is HTTP url Streaming!" );
												 memcpy(ptr_item->item_url ,attriValue,strlen(attriValue));
												 ptr_item->starttime = pRepresentation->duration*seg_index;
												 seg_index++;

											}
											else if(strstr(attriValue , (".")))
											{
												 if(m_base_url[0]!='\0')
												 {
													 memcpy(ptr_item->item_url ,m_base_url_temp,strlen(attriValue)+strlen(base_url));
												 }
												else
													memcpy(ptr_item->item_url ,url,strlen(attriValue)+strlen(base_url));

												 strcat(ptr_item->item_url,attriValue);
												 ptr_item->starttime = pRepresentation->duration*seg_index;
												 seg_index++;
											}
											else
											{
												int le= strlen(attriValue);
												const char * length = NULL;
												const char * find_url = "/";
												int index = m_comtag->lastIndex(attriValue,find_url);
												le = le-index-1;
												length = attriValue+index+1;
						#if defined _WIN32
												ptr_item->starttime= _atoi64(length);
						#elif defined LINUX
												ptr_item->starttime=  atoll(length);
						#endif		 

												memcpy(ptr_item->item_url,url,strlen(url)+strlen(attriValue));
												strcat(ptr_item->item_url,attriValue);
											}
										}

										 if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
										 {
											 ptr_item->nIsVideo = 1;
										 }
										 else
											 ptr_item->nIsVideo = 0;
										 attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_STREAMING_SOURCEURL);
										 if(attriValue)
										 {
											if(strstr(attriValue , ("http://")))
											{
												 VOLOGR( "It is HTTP url Streaming!" );
												 memcpy(ptr_item->item_url ,attriValue,strlen(attriValue));
												 ptr_item->starttime = pRepresentation->duration*seg_index;
												 seg_index++;
											}
											else if(strstr(attriValue , (".")))
											{
												memcpy(ptr_item->item_url ,base_url,strlen(attriValue)+strlen(base_url));
												strcat(ptr_item->item_url,attriValue);
												ptr_item->starttime = pRepresentation->duration*seg_index;
												seg_index++;
											}
											else
											{
												int le= strlen(attriValue);
												const char * length = NULL;
												const char * find_url = "/";
												int index = m_comtag->lastIndex(attriValue,find_url);
												le = le-index-1;
												length = attriValue+index+1;

												ptr_item->starttime= _ATO64(length); 

												memcpy(ptr_item->item_url,url,strlen(url)+strlen(attriValue));
												strcat(ptr_item->item_url,attriValue);
											}
										}  
										ptr_item->duration = pRepresentation->duration;
										ptr_item->segment_index = segment_Index;//start from
							
										if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO )
										{
											 if(video_flag ==0&&pRepresentation->rpe_item_index ==0)
											 {
												 Seg_item * temp = new Seg_item;
												 memset( temp , 0 , sizeof(Seg_item));
												 temp->duration =ptr_item->duration ;
												 temp->starttime = ptr_item->starttime;
												 temp->nIsVideo = 1;
												 temp->group_index =ptr_item->group_index; 
												 //temp->group_index = 6;
												 memcpy(temp->item_url,ptr_item->item_url,strlen(ptr_item->item_url));
												 add_fragmentItem(temp); //add one time
											 }
										}
										else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO&&pRepresentation->rpe_item_index ==0)
										{
											if(audio_flag==0)
											{
												Seg_item * temp = new Seg_item;
												memset( temp , 0 , sizeof(Seg_item));
												temp->duration =ptr_item->duration ;
												temp->starttime = ptr_item->starttime;
												temp->nIsVideo = 0;
												temp->group_index =ptr_item->group_index; 
												memcpy(temp->item_url,ptr_item->item_url,strlen(ptr_item->item_url));
												add_fragmentItem(temp); 
											}
										}
										 add_segmentItem(pRepresentation,ptr_item);
										 segment_Index++;
									}//	endof else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_URL)==0||StrCompare(pTagName_1,TAG_DASH_SEGURL)==0)

									pLevel_segment_1 =  pLevel_segment_1->NextSibling();
								}//endof while(pLevel_segment_1) 
							}//endof if(StrCompare(pTagName,TAG_DASH_STREAMING_SegmentInfo)==0||StrCompare(pTagName,TAG_DASH_SEGMENTLIST)==0)
							pLevel_segment= pLevel_segment->NextSibling();
						}//endof  while(pLevel_segment)  
					//pLevel_repre = pLevel_repre->NextSibling();
					}//endof if(StrCompare(rpe_name, TAG_DASH_STREAMING_Representation)==0||StrCompare(rpe_name, TAG_DASH_STREAMING_Representation_G1)==0) 
					pLevel_repre = pLevel_repre->NextSibling();				
				}
					//endof while(pLevel_repre)  
			}//endof if(0==StrCompare(pGroup_Name,TAG_DASH_STREAMING_Group)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET_G1)||0==StrCompare(pGroup_Name, TAG_DASH_STREAMING_Group_G1))  

			if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO)
				video_flag++;
			else
			{
				if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO) 
					audio_flag++;
			}	
			//if(pLevel_group)
			pLevel_group = pLevel_group->NextSibling();	
		}//endof while(pLevel_group) 
		pChildNode = pChildNode->NextSibling();
	}//endof while(pChildNode) 
    VOLOGR("Analyze %d",video_scale);
	m_manifest->audio_scale = audio_scale;
	m_manifest->video_scale = video_scale;
    ApaptionSet *streamIndexc = new ApaptionSet[m_manifest->group_count];
	memcpy(streamIndexc,adaption_set,sizeof(ApaptionSet)*m_manifest->group_count);
	m_manifest->adaptionSet = streamIndexc;
	return VO_TRUE;

}




VO_VOID vo_mpd_reader::replace(char *source,const char *sub,const char *rep )
{

	int in,out;
	in = out = 0;
	int lsub = strlen(sub);
	int lsour = strlen(source);
	char tmp[155];
	char *p = source;
	char sourceclone[1024];
	memset(sourceclone,0x00,1024);
	memcpy(sourceclone,source,lsour);
	while(lsour >= lsub)
	{
		memset(tmp,0x00,155);
		memcpy(tmp,p,lsub);
		if(strcmp(tmp,sub)==0)
		{
			out = in +lsub;
			break;
		}
		in ++;
		p++;
		lsour -- ;
	}
	if(out >0 && in != out)
	{
		memset(source, 0 ,strlen(source));
		memcpy(source, sourceclone,in);
		strcat(source,rep);
		strcat(source, &sourceclone[out]);
	}
}


int vo_mpd_reader::StrCompare(const char* p1, const char* p2)
{
#if defined _WIN32
	return stricmp(p1, p2);
#elif defined LINUX
	return strcasecmp(p1, p2);
#elif defined _IOS
	return strcasecmp(p1, p2);
#elif defined _MAC_OS
	return strcasecmp(p1, p2);
#endif
}


#if 0

VO_VOID vo_mpd_reader::addRepresentationLevel(ApaptionSet * pAdaptionSet,int group_index, Representation *pRepresentation){
 	
	pRepresentation->rpe_item_index = pAdaptionSet->rpe_number;

	if( pAdaptionSet->m_ptr_QL_head == NULL && pAdaptionSet->m_ptr_QL_tail == NULL )
	{  pAdaptionSet->m_ptr_QL_head = pAdaptionSet->m_ptr_QL_tail = pRepresentation;
	//   pAdaptionSet->m_ptr_QL_head->m_ptr_SG_head = pAdaptionSet->m_ptr_QL_tail->m_ptr_SG_tail = pRepresentation->m_ptr_SG_head = pRepresentation->m_ptr_SG_tail;

	}
	else
	{
		pAdaptionSet->m_ptr_QL_tail->ptr_next = pRepresentation;
		pRepresentation->ptr_pre = pAdaptionSet->m_ptr_QL_tail;
		pAdaptionSet->m_ptr_QL_tail = pRepresentation;
	}
	pAdaptionSet->rpe_number++;


}
#endif

VO_VOID vo_mpd_reader::addRepresentationLevel(ApaptionSet * pAdaptionSet,int group_index, Representation *pRepresentation){
 	
    pRepresentation->rpe_item_index = pAdaptionSet->rpe_number;
	if( pAdaptionSet->m_ptr_QL_head == NULL && pAdaptionSet->m_ptr_QL_tail == NULL )
	{ 
		pAdaptionSet->m_ptr_QL_head = pAdaptionSet->m_ptr_QL_tail = pRepresentation;
	}
	else if(pAdaptionSet->m_ptr_QL_tail->bitrate<pRepresentation->bitrate)
	{
		pAdaptionSet->m_ptr_QL_tail->ptr_next = pRepresentation;
		pRepresentation->ptr_pre = pAdaptionSet->m_ptr_QL_tail;
		pAdaptionSet->m_ptr_QL_tail = pRepresentation;
	}
	else if(pAdaptionSet->m_ptr_QL_head->bitrate>pRepresentation->bitrate)
	{
		pRepresentation->ptr_next = pAdaptionSet->m_ptr_QL_head;
		pAdaptionSet->m_ptr_QL_head = pRepresentation;
	}
	else
	{
		Representation *ptr_cur = pAdaptionSet->m_ptr_QL_head;
		while(ptr_cur)
		{
			if(pRepresentation->bitrate>ptr_cur->bitrate)
			{
				pRepresentation->ptr_next = ptr_cur->ptr_next;
				ptr_cur->ptr_next = pRepresentation;
				pRepresentation->ptr_pre =ptr_cur;
				break;
			}
			 ptr_cur = ptr_cur->ptr_next;
		}
	}
	pAdaptionSet->rpe_number++;
}


VO_S64 vo_mpd_reader::find_fragment_starttime(VO_S64 pos, int nIsVideo ){
	
	Seg_item * ptr_temp = m_ptr_FI_head;
	/*add by leon*/
	if(!ptr_temp )
		return 0;
	VO_S64 start_time = m_ptr_FI_head->starttime;
	Seg_item * tmpPtr = NULL;
	Seg_item * tmpPtrPre = NULL;
	while(ptr_temp)
	{
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
	
	 return tmpPtrPre ? tmpPtrPre->starttime :start_time;
}

Representation * vo_mpd_reader::get_qualityLevel( int stream_index,int index_item )
{
Representation * ptr_item = NULL;
ptr_item = m_manifest->adaptionSet[stream_index].m_ptr_QL_head;
while(ptr_item)
{
	if( ptr_item->rpe_item_index == index_item )
		return ptr_item;
	ptr_item = ptr_item->ptr_next;
}
return NULL;
}


VO_VOID  vo_mpd_reader::add_segmentItem(Representation * rpe,Seg_item * ptr_item)
{
	VO_S64 temp_time = -1 ;
	VO_S64 head_time = -1 ;
	VO_S64 tail_time = -1;
	if(ptr_item->nIsVideo)
		temp_time = ptr_item->starttime*1000/video_scale;
	else
		temp_time = ptr_item->starttime*1000/audio_scale;
	ptr_item->ptr_next = NULL;
	if( rpe->m_ptr_SG_head == NULL &&  rpe->m_ptr_SG_tail == NULL )
	{
		rpe->m_ptr_SG_head =  rpe->m_ptr_SG_tail = ptr_item;
	}
	else
	{
		if( rpe->m_ptr_SG_head->nIsVideo)
			head_time =  rpe->m_ptr_SG_head->starttime*1000/video_scale;
		else
			head_time =   rpe->m_ptr_SG_head->starttime*1000/audio_scale;
		if(m_ptr_FI_head->nIsVideo)
			tail_time = rpe->m_ptr_SG_tail->starttime*1000/video_scale;
		else
			tail_time =  rpe->m_ptr_SG_tail->starttime*1000/audio_scale;
		if( temp_time <  head_time
			|| (ptr_item->group_index == 0 &&temp_time == head_time))
		{
			ptr_item->ptr_next =  rpe->m_ptr_SG_head;
			rpe->m_ptr_SG_head = ptr_item;
		}
		else if (temp_time>= tail_time)
		{
			 rpe->m_ptr_SG_tail->ptr_next = ptr_item;
			 rpe->m_ptr_SG_tail = ptr_item;
		}
		else
		{
			Seg_item * ptr_cur =  rpe->m_ptr_SG_head;
			VO_S64 cur_time = -1;
			VO_S64 cur_time_next = -1;
			while(ptr_cur->ptr_next)
			{
				if(ptr_cur->ptr_next->nIsVideo)
					cur_time = ptr_cur->ptr_next->starttime*1000/video_scale;
				else
					cur_time_next = ptr_cur->ptr_next->starttime*1000/audio_scale;
				if(ptr_cur->nIsVideo)
					cur_time = ptr_cur->starttime*1000/video_scale;
				else
					cur_time = ptr_cur->starttime*1000/audio_scale;
				if(temp_time >= cur_time && temp_time < cur_time_next)
				{
					ptr_item->ptr_next = ptr_cur->ptr_next;

					ptr_cur->ptr_next = ptr_item;

					break;
				}
				ptr_cur = ptr_cur->ptr_next;
			}

		}
	}

    	rpe->segmentsNumber++;//the number segments in  the representation 

}

VO_BOOL vo_mpd_reader::Parse(vo_http_stream *ptr_fragment,VO_CHAR * url,MpdStreamingMedia *manifest )
{
	destroy();
	VOLOGR(" url %s", url);
	memset( m_url,0,sizeof(m_url));
	strcpy( m_url , url );
	get_relative_directory( url );  
	m_manifest= manifest;
	if( VO_FALSE == get_mpd( url ) )
		return VO_FALSE;
	 analyze_2(m_url);
	return VO_TRUE;
}

VO_VOID vo_mpd_reader::destroy()
{
	VOLOGR("Reader destroy1");
	if( m_file_content )
	{
		delete []m_file_content;
		m_file_content = 0;
	}
	Seg_item * ptr_item = m_ptr_FI_head;
	
	int i =0;
	while( ptr_item)//!= m_ptr_FI_tail )
	{
		Seg_item * ptr_temp = ptr_item;
		VOLOGR("1");
		ptr_item = ptr_item->ptr_next;
		i++;
		
		VOLOGR("delete Seg_item3 = %p,%p",ptr_temp,ptr_item);
		delete ptr_temp;
		VOLOGR("2");
		ptr_temp = NULL;
	}

     VOLOGR("Reader destroy2");
	m_ptr_FI_head = m_ptr_FI_tail = 0;
//Flush list
	FlushAVList();
}

VO_VOID vo_mpd_reader::hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex, VO_S32 length )
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


void vo_mpd_reader::CreateAVList(int counts)
{
	DestroyAVList();

	Timeline_video = new VO_S64[counts];
	Timeline_audio = new VO_S64[counts];
	Duration_video = new VO_S64[counts];
	Duration_audio = new VO_S64[counts];
}
void vo_mpd_reader::InitAVList()
{
	Timeline_video = NULL;
	Timeline_audio = NULL;
	Duration_video = NULL;
	Duration_audio = NULL;
}
void vo_mpd_reader::FlushAVList()
{
	memset(Timeline_video, 0x00, sizeof(Timeline_video));
	memset(Timeline_audio, 0x00, sizeof(Timeline_audio));
	memset(Duration_video, 0x00, sizeof(Duration_video));
	memset(Duration_audio, 0x00, sizeof(Duration_audio));
}
void vo_mpd_reader::DestroyAVList()
{
	if(Timeline_video) 
		delete []Timeline_video;
	if(Timeline_audio) 
		delete []Timeline_audio;
	if(Duration_video) 
		delete []Duration_video;
	if(Duration_audio) 
		delete []Duration_audio;

	InitAVList();
}