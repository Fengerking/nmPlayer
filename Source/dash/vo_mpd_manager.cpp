//#include "stdafx.h"
//#define _USE_NEW_BITRATEADAPTATION
#include "vo_mpd_manager.h"
#include "voOSFunc.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
#define LEON_ADD_2

vo_mpd_manager::vo_mpd_manager(void)
:max_bitrate(0)
,min_bitrate(0)
,m_video_index(-1)
,m_audio_index(-1)
,cur_bitrate_index(0)
,last_QL_bitrate(0)
,m_bNeedReduceQualityLevel(VO_TRUE) // vo_true set the better-qualitylevel to highest level in the URL
,m_nReduceNum(0)
,bitrate_changed_Video(VO_FALSE)
,bitrate_changed_Audio(VO_FALSE)
,m_nCurBitrateCounts(0)
{
	m_bStartBitrate = true;
	last_QL_bitrate = -1;
	m_bStartBitrate = true;
  	num = 0;
	memset( m_url , 0 , sizeof(m_url) );
//	memset( request_video , 0 , sizeof(request_video) );
//	memset( request_audio , 0 , sizeof(request_audio) );
	m_better_QualityLevel.bitrate = 999999999;
	//m_manifest.piff.data = NULL;
	m_manifest.adaptionSet = NULL; 
	m_manifest.group_count = 0;
	m_nVideoChunk_id = 0;
	m_nAudioChunk_id = 0;
	m_playlist_item_count = 0;

}

vo_mpd_manager::~vo_mpd_manager(void)
{
    destroy();
}

VO_VOID vo_mpd_manager::TooHightQualityLevel()
{
	m_nReduceNum ++;
	if(m_nReduceNum > 2)
	{
		m_bNeedReduceQualityLevel = VO_TRUE;
		m_nReduceNum =0;
	//	VOLOGI("TooHightqualitylevel for player ,set down a level");
	}
}

VO_VOID vo_mpd_manager::replace(char *source,const char *sub,const char *rep )
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

Representation * vo_mpd_manager::get_qualityLevel( int stream_index,int index_item ){

	Representation * ptr_item = NULL;

	//if(m_video_index == -1) return NULL;

	ptr_item = m_manifest.adaptionSet[stream_index].m_ptr_QL_head;

	while(ptr_item)
	{
		if( ptr_item->rpe_item_index == index_item )
			return ptr_item;

		ptr_item = ptr_item->ptr_next;
	}
	return NULL;
}
VO_S64 vo_mpd_manager::get_cur_bitrate()
{

      return get_qualityLevel( m_video_index,cur_bitrate_index )->bitrate;

}


VO_U32 vo_mpd_manager::get_videoCodecType(){

	return m_manifest.adaptionSet[m_video_index].nCodecType;
		
		
}

VO_S32 vo_mpd_manager::get_duration()
{
	//VOLOGI("vo_mpd_manager::get_duration %d",m_playlist_duration);

	if( is_live()&&is_needupdate() )
	{
	//	VOLOGR( "get duration 0" );
		return 0;
	}
	
	return m_playlist_duration; 

}

VO_VOID vo_mpd_manager::reduce_QualityLevel()
{
	
	Representation * ptr_item = NULL;
	//ptr_item = m_manifest.adaptionSet[m_video_index].m_ptr_QL_head; //the birate of the index
	ptr_item = m_manifest.adaptionSet[m_video_index].m_ptr_QL_tail;
	//int index = m_video_index;

	while(ptr_item)
	{
		if(m_better_QualityLevel.bitrate > ptr_item->bitrate)
		{
			m_nReduceNum =0;
			memcpy(&m_better_QualityLevel,ptr_item,sizeof(Representation));
			break;
		}
		ptr_item = ptr_item->ptr_pre;
	}
//	VOLOGR("reduce_QualityLevel bitrate(%lld),index(%d)",m_better_QualityLevel.bitrate,m_better_QualityLevel.rpe_item_index);
}
#ifdef LEON_ADD_2
VO_S64 vo_mpd_manager::determine_bitrate( VO_S64 last_network_bitrate ,VO_S64 duration ){

	if( last_network_bitrate == -1 )
		return 0;
	gbitrate[num] = last_network_bitrate;
	gduration[num++] = duration;
	Representation * ptr_item = NULL;
	if(num>2)
	{
		num = 0;
		VO_S64 bbb = (gbitrate[0] + gbitrate[1] + gbitrate[2])/3;

		ptr_item =  m_manifest.adaptionSet[m_video_index].m_ptr_QL_head;
		Representation *temp = ptr_item;
		while(ptr_item )
		{
			if(bbb > ptr_item->bitrate*1.5)
			{
				temp = ptr_item;
				ptr_item = ptr_item->ptr_next;
			}
			else
			{
				break;
			}
		}
		ptr_item = temp;
		VOLOGI("BA:curbitrate:%lld bitrate:%lld",bbb,ptr_item->bitrate)
	}
	if(!ptr_item)
		ptr_item = get_qualityLevel( m_video_index,cur_bitrate_index); //the first ; //the birate of the index
#ifdef _USE_NEW_BITRATEADAPTATION
	CheckBetterQL();
#else
	if( m_bNeedReduceQualityLevel)
		reduce_QualityLevel();
#endif
	m_bNeedReduceQualityLevel = VO_FALSE;
	VO_S64 url_bitrate = ptr_item->bitrate;
	if(url_bitrate>m_better_QualityLevel.bitrate){
		url_bitrate = m_better_QualityLevel.bitrate;
		cur_bitrate_index = m_better_QualityLevel.rpe_item_index;
	}
	else
		cur_bitrate_index = ptr_item->rpe_item_index;

	VOLOGI("BA: Now netspeed %lld,  better_QualityLevel %lld",url_bitrate,m_better_QualityLevel.bitrate );

	last_bitrate_index = cur_bitrate_index;
	return url_bitrate;
}
#else
VO_S64 vo_mpd_manager::determine_bitrate( VO_S64 last_network_bitrate ,VO_S64 duration ){

if( last_network_bitrate == -1 )
		return 0;
	gbitrate[num] = last_network_bitrate;
	gduration[num++] = duration;
	Representation * ptr_item = NULL;
	if(num>2){
		num = 0;
		double bbb = (gbitrate[0] + gbitrate[1] + gbitrate[2]) /1000.0;
		double ddd = (gduration[0] + gduration[1] + gduration[2]) /10000000.0;
		if(bbb<= ddd *0.8){
			VO_S64 bt = get_cur_bitrate();
			VOLOGR("determine_bitrate1 %d",bt);
			Representation *tmp_ptr_item =  m_manifest.adaptionSet[m_video_index].m_ptr_QL_head;
			int upgrade = 1;
			if(bbb <= ddd * 0.2)	upgrade = 2;
			int index = 1;
		while(tmp_ptr_item && upgrade){
			if(bt<tmp_ptr_item->bitrate){
					upgrade --;
					ptr_item = tmp_ptr_item;
					index ++;
			 }
			tmp_ptr_item = tmp_ptr_item->ptr_next;
			}
		}
		else if(bbb>=ddd*0.8)
		{
			  VO_S64 bt = get_cur_bitrate();
			  VOLOGR("determine_bitrate2 %d",bt);
			  Representation* tmp_ptr_item = m_manifest.adaptionSet[m_video_index].m_ptr_QL_tail;
			  int index = 0;
			  int upgrade = 1;
			  while(tmp_ptr_item && upgrade)
			{
				if(bt > tmp_ptr_item->bitrate)
				{
					upgrade --;
					index++;
					ptr_item = tmp_ptr_item;
				}
				tmp_ptr_item =tmp_ptr_item->ptr_pre;
			}
		}
	
		else
			   ptr_item = get_qualityLevel( m_video_index,cur_bitrate_index); //the first 
         }

	 if(!ptr_item)
      ptr_item = get_qualityLevel( m_video_index,cur_bitrate_index); //the first ; //the birate of the index
#ifdef _USE_NEW_BITRATEADAPTATION
	CheckBetterQL();
#else
	 if( m_bNeedReduceQualityLevel)
		reduce_QualityLevel();
#endif
	   m_bNeedReduceQualityLevel = VO_FALSE;
	   VO_S64 url_bitrate = ptr_item->bitrate;
	  if(url_bitrate>m_better_QualityLevel.bitrate){
     	url_bitrate = m_better_QualityLevel.bitrate;
		cur_bitrate_index = m_better_QualityLevel.rpe_item_index;
	  }
	  else
		 cur_bitrate_index = ptr_item->rpe_item_index;
	
	  VOLOGR("last speed %lld now netspeed %lld,  better_QualityLevel %lld",last_network_bitrate,url_bitrate,m_better_QualityLevel.bitrate );

	  last_bitrate_index = cur_bitrate_index;
	  return url_bitrate;
}
#endif
VO_VOID vo_mpd_manager::hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex, VO_S32 length )
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
VO_BOOL vo_mpd_manager::get_fragmentItem( Seg_item ** ptr_item,  VO_S64 start_time,Representation * pp ){
	               Seg_item * temp= pp->m_ptr_SG_head;
				   while(temp!= pp->m_ptr_SG_tail){
					   if(temp->starttime == start_time){
						   *ptr_item = temp;
						   return VO_TRUE;
					   }
					   temp = temp->ptr_next;


				   }
				   return VO_FALSE;

}



VO_S32 vo_mpd_manager::pop_fragment( Seg_item * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate,VO_S64 &last_fragment_duration ){
	//last network birate means the last downloadbirate
	VO_S64 chunkid = 0;
	VO_S64 url_bitrate = 0;
	if(!(ptr_item->group_index >= 0))
		return -1;
	if(ptr_item->group_index == m_video_index){//the first value is 0

		if(m_bStartBitrate){
			url_bitrate = min_bitrate;// max_bitrate;//
			cur_bitrate_index =  m_manifest.adaptionSet[m_video_index].m_ptr_QL_head->rpe_item_index;//
			ptr_item->representation_index = cur_bitrate_index;
			if(!m_reader.m_seg_temlate){
				Representation * pRepresentation = get_qualityLevel(m_video_index,cur_bitrate_index);
				VO_S64 start_time = ptr_item->starttime;
				get_fragmentItem(&ptr_item,start_time,pRepresentation) ;
				memcpy(ptr_path,ptr_item->item_url,strlen(ptr_item->item_url));
		 }
				//the first birate
			last_bitrate_index = cur_bitrate_index;
			m_bStartBitrate = false;
		}//the first time use the default birate
		else
		{
#if 1
			url_bitrate = determine_bitrate(last_network_bitrate,last_fragment_duration);  //not first time 

#else
			cur_bitrate_index =  m_manifest.adaptionSet[m_video_index].m_ptr_QL_head->rpe_item_index;
			url_bitrate = m_manifest.adaptionSet[m_video_index].m_ptr_QL_head->bitrate;	

#endif


		/*	if(!is_live())
			url_bitrate = determine_bitrate(last_network_bitrate,last_fragment_duration);  //not first time 

			else
			{
				cur_bitrate_index =  m_manifest.adaptionSet[m_video_index].m_ptr_QL_head->rpe_item_index;
				url_bitrate = m_manifest.adaptionSet[m_video_index].m_ptr_QL_head->bitrate;	
			}*/

			ptr_item->representation_index = cur_bitrate_index;
			if(!m_reader.m_seg_temlate){
				Representation * pRepresentation = get_qualityLevel(m_video_index,cur_bitrate_index);
				VO_S64 start_time = ptr_item->starttime;
				get_fragmentItem(&ptr_item,start_time,pRepresentation) ;
				memcpy(ptr_path,ptr_item->item_url,strlen(ptr_item->item_url));
		 }

		}

	        if(last_QL_bitrate!=url_bitrate)
			 bitrate_changed_Video = VO_TRUE;
		    last_QL_bitrate = url_bitrate;
			chunkid = m_nVideoChunk_id++;
			last_fragment_duration = ptr_item->duration; //get the video url 
			ptr_item->representation_index = cur_bitrate_index;
	}
	else{
		    url_bitrate = m_manifest.adaptionSet[m_audio_index].audio_QL.bitrate;
		   if(!m_reader.m_seg_temlate){
			   	memcpy(ptr_path,ptr_item->item_url,strlen(ptr_item->item_url));
		   }
		 //  cur_bitrate_index = 0;
		   chunkid = m_nAudioChunk_id++;
		   ptr_item->representation_index = 0;
	}
    
	  VOLOGR( "pop_fragment url_bitrate %lld" , url_bitrate);
	if(m_reader.m_seg_temlate)
	{ 
		int  stream_index = ptr_item->group_index;
		//ptr_item->representation_index = cur_bitrate_index;
		int  represen_index = ptr_item->representation_index;
		Representation * pRepresentation = get_qualityLevel(stream_index,represen_index);
		//int index = 0;	
		VO_CHAR c[10];
		memset(c,0x00,sizeof(c));
#if defined _WIN32
		strcat(c,"%I64d");
		//	strcat(base_url, "/QualityLevels(%I64d)/Fragments(");
#elif defined LINUX
		strcat(c,"%lld");
		//	strcat(base_url, "/QualityLevels(%lld)/Fragments(");
#elif defined _IOS
		strcat(c,"%lld");
#elif defined _MAC_OS
		strcat(c,"%lld");
#endif
	
		    memcpy(ptr_item->item_url,pRepresentation->mediaUrl,strlen(pRepresentation->mediaUrl));
			strcpy(ptr_item->item_url,pRepresentation->mediaUrl);
			//	char * url = "http://usa.visualon.com/streaming/VerizonDash/sourceA/Dash/h264baseline-1000/video/QL_749952/0";
			char str[255];
			memset(str,0x00,sizeof(str));
			sprintf(str,c,pRepresentation->bitrate);
			replace(ptr_item->item_url,"$Bandwidth$",str);
			memset(str,0x00,sizeof(str));
			sprintf(str,c, ptr_item->starttime);
			replace(ptr_item->item_url,"$Time$",str);
			replace(ptr_item->item_url,"$RepesendationID$",pRepresentation->id);
			replace(ptr_item->item_url,"$RepresentationID$",pRepresentation->id);
			memset(str,0x00,sizeof(str));
			sprintf(str,c,ptr_item->startnmber);
			replace(ptr_item->item_url,"$Number$",str);
			memset(str,0x00,sizeof(str));
			sprintf(str,c,ptr_item->startnmber);
			replace(ptr_item->item_url,"$Index$",str);
			memset(str,0x00,sizeof(str));
			replace(ptr_item->item_url,"Manifest.mpd","");
			replace(ptr_item->item_url,"verizon.mpd","");
			replace(ptr_item->item_url,"generic.mpd","");
			replace(ptr_item->item_url,"vlcManifestVideo.mpd","");
			replace(ptr_item->item_url,"vlcManifestAudio.mpd","");
			replace(ptr_item->item_url,"x264_base_2seg_2buf.mpd","");
			memcpy(ptr_path,ptr_item->item_url,strlen(ptr_item->item_url));
		

		}
	return 1;

}


/*
VO_BOOL vo_mpd_manager::get_segmentItem(Representation * last_rpe,Seg_item ** ptr_item,  VO_S64 start_time ){
	
  
	   return m_reader.get_segmentItem(last_rpe,ptr_item,start_time); //don't know the index
}
*/
VO_BOOL vo_mpd_manager::get_segmentItem(Seg_item ** ptr_item,  int ini_chunk,int type)
{
  return m_reader.get_segmentItem(ptr_item,ini_chunk,type); //don't know the index
}
VO_BOOL vo_mpd_manager::get_segmentItem(Seg_item ** ptr_item,  VO_U64 start_time ){
	
  
	   return m_reader.get_segmentItem(ptr_item,start_time); //don't know the index
}
VO_BOOL vo_mpd_manager::get_segmentItem(Seg_item ** ptr_item,  int ini_chunk ){
	
  
	   return m_reader.get_segmentItem(ptr_item,ini_chunk); //don't know the index
}

VO_CHAR* vo_mpd_manager::getRepresentationUrl(int stream_index,int representation_index){
	return m_reader.getRepresentationUrl(stream_index,representation_index);

}



Seg_item* vo_mpd_manager::GetFirstChunk()
{
     return m_reader.m_ptr_FI_head;
	/*if(m_reader.m_ptr_FI_head.group_index == m_video_index||m_reader.m_ptr_FI_head.group_index ==m_audio_index)
		return m_reader.m_ptr_FI_head;
	else
		return m_reader.m_manifest.adaptionSet[m_video_index]->m_ptr_QL_head.m_ptr_SG_head;
	*/
   
}
Seg_item* vo_mpd_manager::GetNextChunk(Seg_item *ptr_item){
//	Seg_item * last_seg = ptr_item;
   if(ptr_item ==  m_reader.m_ptr_FI_tail)
			return NULL;
	while(ptr_item!=m_reader.m_ptr_FI_tail)
	{
	
		ptr_item = ptr_item->ptr_next;
		
		if(!ptr_item) return NULL;
		if(ptr_item->group_index == m_video_index || ptr_item->group_index == m_audio_index)
			break;
	}
	return ptr_item;
}

Seg_item* vo_mpd_manager::GetNextAudio_Chunk(Seg_item *ptr_item){
   if(ptr_item ==  m_reader.m_ptr_FI_tail)
			return NULL;
   Seg_item * temp   = ptr_item->ptr_next;
	while(temp!=m_reader.m_ptr_FI_tail)
	{
		if(temp->nIsVideo !=0)
		{
			temp = temp->ptr_next;

		}
		else
		{
			ptr_item = temp;
		    break;
		}
		
		if(!ptr_item) return NULL;
	}
	return ptr_item;
}
VO_S64 vo_mpd_manager::get_audiostarttime(){
	Seg_item * ptr_temp = m_reader.m_ptr_FI_head;

	while(ptr_temp)
	{
		if(ptr_temp->group_index == m_audio_index)
		{
			return (ptr_temp->starttime-m_reader.GetStartTime())/GetTimeScaleMs(); //????
		}

		ptr_temp = ptr_temp->ptr_next;
	}
	return 0;

}


VO_VOID vo_mpd_manager::make_requesturl(VO_CHAR *requestUrl, VO_S32 streamIndex,VO_S32 representation_index)
{
	VO_CHAR *base_url =requestUrl;//[1024];
	memset( base_url , 0 , strlen(base_url) );
	Representation * rpe = NULL;// new Representation;
	rpe = m_manifest.adaptionSet[streamIndex].m_ptr_QL_head;
	while(rpe){
		if(rpe->index_QL == representation_index){
          strcat(base_url,rpe->initalUrl);
			break;
		}
		else
			rpe = rpe->ptr_next;

	}
}

VO_VOID vo_mpd_manager::destroy()
{
	if(!m_manifest.adaptionSet)
		return;

	int i = 0;
	while(i<m_reader.group_number)
	{
		Representation * ptr_item = m_manifest.adaptionSet[i].m_ptr_QL_head;
		while( ptr_item )//!= m_manifest.adaptionSet[i].m_ptr_QL_tail )
		{
			Representation * ptr_temp = ptr_item;
			Seg_item* item = ptr_temp->m_ptr_SG_head;
			while(item)//!=ptr_temp->m_ptr_SG_tail){
			{
				Seg_item* item_temp= item;
				item = item->ptr_next;
				//m_reader.find((int)item_temp);
				delete item_temp;
				item_temp = NULL;
			}
			//m_reader.find((int)item);
			ptr_item = ptr_item->ptr_next;
			delete ptr_temp;
			ptr_temp = NULL;
		}
		//delete ptr_item;

		m_manifest.adaptionSet[i].m_ptr_QL_head = m_manifest.adaptionSet[i].m_ptr_QL_tail = NULL;
		i++;

	}
	  if(m_manifest.adaptionSet){delete []m_manifest.adaptionSet; m_manifest.adaptionSet = NULL; m_manifest.group_count = 0;}
	  
/*
	if( m_video_index != -1)
	{
		Representation * ptr_item = m_manifest.adaptionSet[m_video_index].m_ptr_QL_head;

		while( ptr_item != m_manifest.adaptionSet[m_video_index].m_ptr_QL_tail )
		{
			Representation * ptr_temp = ptr_item;
			Seg_item* item = ptr_temp->m_ptr_SG_head;
			while(item !=ptr_temp->m_ptr_SG_tail){
				
				Seg_item* item_temp= item;
				item = item->ptr_next;
				delete item_temp;
				}
			if(item)
			   delete item;
			ptr_item = ptr_item->ptr_next;

			delete ptr_temp;
		}
		  delete ptr_item;

		m_manifest.adaptionSet[m_video_index].m_ptr_QL_head = m_manifest.adaptionSet[m_video_index].m_ptr_QL_tail = NULL;
	}
	if(m_audio_index!=-1){
		Representation * ptr_item = m_manifest.adaptionSet[m_audio_index].m_ptr_QL_head;
		if(ptr_item == m_manifest.adaptionSet[m_audio_index].m_ptr_QL_tail){
		    Representation * ptr_temp = ptr_item;
			Seg_item* item = ptr_temp->m_ptr_SG_head;

			while(item !=ptr_temp->m_ptr_SG_tail){	
				Seg_item* item_temp= item;
				item = item->ptr_next;
				delete item_temp;
				
				}
			if(item)
				delete  item;

			    delete ptr_temp;
		}
	
		m_manifest.adaptionSet[m_audio_index].m_ptr_QL_head = m_manifest.adaptionSet[m_audio_index].m_ptr_QL_tail = NULL;
	}

	//if(m_manifest.piff.data) {delete []m_manifest.piff.data; m_manifest.piff.data = NULL;}
	if(m_manifest.adaptionSet){delete []m_manifest.adaptionSet; m_manifest.adaptionSet = NULL; m_manifest.group_count = 0;}*/
}
	
VO_VOID vo_mpd_manager::close()
{
	destroy();
//	m_stop_thread = VO_FALSE;

}


VO_BOOL vo_mpd_manager::SetManifest( vo_http_stream *ptr_fragment,VO_CHAR * url )
{
	//  url = "http://usa.visualon.com/streaming/VerizonDash/sourceA/Dash/h264baseline-5000/vlcManifestVideo.mpd";
	 // url ="http://usa.visualon.com/streaming/VerizonDash/sourceA/Dash/h264baseline-1000/vlcManifestAudio.mpd";
     // url ="http://10.2.64.44/dash/demo/test.mpd";
	//url = "http://usa.visualon.com/streaming/VerizonDash/sourceA/Dash/h264baseline-1000/Manifest.mpd";
  	//url = "http://10.2.64.44/dash/verizon/mp4/h264b1s/Manifest.mpd";
	//url = "http://10.2.64.44/dash/demo2/final.mpd";
	//url = "http://10.2.64.44/dash/verizon/mp4/h264b1s_d/Manifest.mpd";
	// url = "http://10.2.64.44/dash/verizon/mp4/h264b1s_d/Manifest.mpd";
	//url = "http://usa.visualon.com/streaming/VerizonDash/sourceB/dash-1sec/baseline.mpd";
  //  url = "http://10.2.64.42/dash/mp4/h264m5000/Manifest.mpd";
	// url = "http://10.2.64.42/dash/mp4/h264m2000/Manifest.mpd";
   //	url = "http://10.2.64.42/dash/mp4/h264b1000/Manifest.mpd";
  //   url = "http://usa.visualon.com/streaming/MediaExcel/dash_samples/OneSecGop/BL_dash/verizon.mpd";
    //  url = "http://10.2.64.44/dash/verizon/envivio/NAB-600Kbps/Manifest.mpd";
	 //  url = "http://10.2.64.44/dash/verizon/envivio/NAB-1Mbps/Manifest.mpd";
	//   url = "http://10.2.64.44/dash/verizon/thomson/manifest_live.mpd";
	 //  url = "http://10.2.64.44/dash/verizon/thomson/manifest.mpd"
     // url = "http://usa.visualon.com/streaming/VerizonDashCisco/VerizonDASH/x264_base_2seg_2buf/x264_base_2seg_2buf.mpd";
    //    url = "http://usa.visualon.com/streaming/MediaExcel/dash_samples/OneSecGop/BL_dash/generic.mpd";
	// url = "http://83.206.103.249/dash/stream1/Manifest.mpd";
	 //url = "http://10.2.64.44/dash/verizon/cisco/x264_base_2seg_2buf.mpd";
	// url = "http://10.2.64.44/dash/verizon/cisco/x264_base_1seg_1buf/x264_base_1seg_1buf.mpd";
	 //url = " http://10.2.64.42/dash/MediaExcel_dash_samples/OneSecGop/BL_dash/generic.mpd";
  //    url = "http://10.2.64.42/dash/MediaExcel_dash_samples/OneSecGop/BL_dash/verizon.mpd";
   //  url = "http://83.206.103.249/dash/stream1/Manifest.mpd";
	// url = "http://10.2.64.44/dash/verizon/verizon/h264b10s/Manifest.mpd";
	//url = "http://83.206.103.249/dash/stream1/Manifest.mpd";
	//url = "http://10.2.65.12/DASH/MediaExcel/OneSecGop/BL_dash/verizon.mpd";
   // url = "http://10.2.64.44/dash/verizon/cisco/x264_base_2seg_2buf.mpd";
	//url = "http://10.2.65.12/DASH/cisco/x264_base_1seg_1buf/x264_base_1seg_1buf.mpd";
	// url = "http://10.2.64.42/dash/mp4/h264b5000/Manifest.mpd";
	//url = "http://usa.visualon.com/streaming/VerizonDash/sourceA/Dash/h264baseline-5000/Manifest.mpd";
	//url= "http://10.2.64.44/ dash/verizon/cisco/x264_main_10seg_2buf/x264_main_10seg_2buf.mpd";
	//url = "http://usa.visualon.com/streaming/MediaExcel/dash_samples/OneSecGop/BL_dash/verizon.mpd";
	//url = "http://usa.visualon.com/streaming/VerizonDash/sourceA/Dash/h264main-1000/vlcManifest.mpd";
	//  url = "http://10.2.64.44/dash/verizon/thomson/manifest_0.mpd";
	   // url = "http://10.2.64.44/dash/verizon/thomson/MP4_baseline_2/manifest.mpd";
	  //  url = "http://10.2.64.44/dash/verizon/thomson/MP4_main_1/manifest.mpd";
	// url = " http://10.2.64.44/dash/verizon/thomson/manifest_o.mpd";
	 // url = "http://10.2.64.95/Dash/verizon.mpd";
     // url = "http://10.2.64.44/dash/verizon/thomson/manifest_live_o.mpd";
	  //url = "http://10.2.64.44/dash/verizon/thomson/manifest_live.mpd";
	// url = "http://usa.visualon.com/streaming/VerizonDashCisco/VerizonDASH/x264_main_1seg_1buf/x264_main_1seg_1buf.mpd";
	//  url = "http://10.2.64.44/dash/verizon/mediaexcel/1.2/verizon.mpd";
	 //url = "http://usa.visualon.com/streaming/ThomsonDash/MP4_baseline_1/manifest.mpd";
	// url = "http://10.2.64.44/dash/verizon/verizon/h264b10s/Manifest.mpd";
	//url = "http://usa.visualon.com/streaming/ThomsonDash/MP4_baseline_1/manifest_live.mpd";
	//url = "http://10.2.64.44/dash/verizon/thomson/manifest_o.mpd";
	//url = "http://10.2.64.44/dash/verizon/verizon/h264b10s/Manifest.mpd";
   //	url = "http://10.2.65.12/DASH/Thomson/MP4_Main_1/manifest.mpd";
	 // url = "http://10.2.64.96/Thomson/ISO/manifest_live.mpd";
     // url = "http://10.2.64.44:8080/project/verizon/thomson-multibitrate/manifest_live.mpd";
	 // url = "http://10.2.64.44:8080/project/verizon/thomson-multibitrate/manifest.mpd";
	 // url = "http://10.2.64.44:8080/project/verizon/thomson-multibitrate/manifest.mpd";
	//url = "http://10.2.65.231/dash/Thomson/dash-iso/MP4_baseline_10/manifest_live.mpd";
    //url = "http://10.2.65.231/dash/Thomson/dash-iso/MP4_baseline_10/manifest_live.mpd";
	//url = "http://10.2.64.44/project/verizon/elemental/dashmp4.mpd";
//	url = "http://10.2.64.44/project/verizon/thomson-multibitrate/manifest.mpd";
	//url = "http://10.2.64.44/project/verizon/thomson-multibitrate/manifest_live.mpd";
//	url = "http://10.2.64.44/project/verizon/envivio-nab-1mbps/manifest.mpd";
	//url = "http://10.2.64.44/project/verizon/mediaexcel-v1.2/verizon.mpd";
//	url = "http://10.2.64.44/project/verizon/dolby/live/multi_lang/ChID_voices_DualTrack_321_200_ddp/ChID_voices_DualTrack_321_200_ddp.mpd";
//	url = "http://10.2.64.44/project/verizon/dolby/ondemand/ChID/ChID_voices_321_ddp/ChID_voices_321_ddp.mpd";
	//url = "http://10.2.64.44/project/verizon/cisco/gen3/stream1.mpd";
	//url = "http://10.2.64.44/project/verizon/source-a/h264b1000/manifest.mpd";
	//url = "http://10.2.64.44/project/verizon/dashlivesample/dash.mpd";
	//https://sh.visualon.com/sites/default/files/task/2012/05/249/stream1.mpd
//	url = "http://10.2.64.44/project/verizon/dashlivesample/dash.mpd";
	//url = "http://10.2.64.44/project/verizon/envivio-nab-600kbps/manifest.mpd";
	//url = "http://10.2.64.44/project/verizon/dolby/live/dual_decoding/ChID_voices_AD_321_100_ddp/ChID_voices_AD_321_100_ddp.mpd";
	//url = "http://10.2.64.44/project/verizon/cisco/x264_base_1seg_1buf/x264_base_1seg_1buf_m.mpd";
//	url = "http://10.2.64.44/project/verizon/thomson-multibitrate/manifest.mpd";
//	url = "http://10.2.64.44/project/verizon/source-a/h264b1000/manifest.mpd";
//	url = "http://10.2.64.44/project/verizon/elemental/dashmp4.mpd";
	//url = "http://10.2.64.44/project/verizon/source-a/h264b1000/manifest.mpd";
//	url = "http://10.2.64.44/project/verizon/envivio-nab-600kbps/manifest.mpd";
//	url = "http://10.2.64.44/project/verizon/mediaexcel-live/dash.mpd";
	//url = "http://www.mediaexcel.com/temp/dash/akamai.mpd";
	//url  = "http://10.2.64.44/project/verizon/dashlivesample/dash.mpd";
	//url = "http://10.2.64.44/project/verizon/cisco/gen3/stream1.mpd";
//	url = "http://10.2.64.44/project/verizon/elemental/dashmp4.mpd";
//	url = "http://10.2.64.44/project/verizon/source-a/h264b1000/manifest.mpd";
	//url = "http://10.2.64.44/project/verizon/dolby/live/ChID/ChID_voices_200_ddp/ChID_voices_200_ddp.mpd";
	//url = "http://10.2.64.44/project/verizon/harmonic/bp/1s/live/set1.mpd";
	//url = "http://10.2.64.44/project/verizon/thomson-multibitrate/manifest_live.mpd";
   //  url = "http://83.206.103.249/dash/stream1/Manifest.mpd";
	//url = "http://10.2.64.44/project/verizon/mediaexcel-v1.2/verizon.mpd";
	//url = "http://10.2.64.44/project/verizon/cisco/x264_base_1seg_1buf/x264_base_1seg_1buf_m.mpd";
 	//url = "http://www.mediaexcel.com/temp/dash/akamai.mpd";
	//url = "http://10.2.64.44/project/verizon/harmonic/bp/1s/live/set1.mpd";
	//url = "http://mktlive1-remote.elementaltechnologies.com/dash/dashmp4/live.mpd";
	//url = "http://10.2.64.44/project/verizon/cisco/stream3min/stream1.mpd";
	//url = "http://10.2.64.44/project/verizon/ericsson/spr1200p/streams.mpd";
	//url = "http://10.2.64.44/project/verizon/source-a/h264b1000/manifest.mpd";
	//url = "http://10.2.64.44/project/verizon/cisco/stream3min/stream1.mpd";
	//url = "http://10.2.65.206/MyDash/cisco/stream3min/stream1.mpd";
	//url = "http://10.2.64.44/project/verizon/thomson/thomson-multibitrate/manifest_live.mpd";
    // url ="http://10.2.64.44/project/verizon/thomson/thomson-multibitrate/manifest.mpd";
	// url = "http://10.2.64.44/project/verizon/mediaexcel/mediaexcel-v1.2/verizon.mpd";
	// url = "http://10.2.64.44:9999/project/verizon/cisco/stream3min/stream1.mpd";
	// url = "http://usa.visualon.com:8080/hera/videos/ch1/dash.mpd";
	 // url = "http://10.2.64.221/MyDash/cisco/stream3min/stream1.mpd";
    //url = "http://10.2.65.37/MyDash/cisco/stream3min/stream1.mpd";
   // url = "http://10.2.64.44/project/verizon/dolby/live/dual_decoding/ChID_voices_AD_321_100_ddp/ChID_voices_AD_321_100_ddp.mpd";
 //   url = "http://66.0.13.217/http/directory1/vlc.streams.mpd";
    // url = "http://66.0.13.217/http/directory1/vlc.streams.mpd";
    //  url = "http://10.2.64.44/project/verizon/ericsson/spr1200p/streams.mpd";
	memset( m_url,0,sizeof(m_url));
//	int len = strlen(url)- strlen(url_temp);
	strcpy( m_url , url );
	//m_url[len] = '\0';
	
	for(int i = 0;i<20;i++)
		replace(m_url," ","%20");
	
	if( !Parse_mpd(ptr_fragment) )
		return VO_FALSE;

	    return VO_TRUE;

}

VO_S32 vo_mpd_manager::setLang(VO_CHAR * lang){

	if(m_audio_index!=-1){
		int temp = m_audio_index;
		while(m_manifest.adaptionSet[temp].nTrackType == VO_SOURCE_TT_AUDIO){
		if(m_reader.StrCompare(m_manifest.adaptionSet[temp].lang,lang)==0){
           m_audio_index = temp;
			return temp;
			break;
		}
		else
		    temp++;
	
}
		return 0;
 }
	else return -1;

}
VO_BOOL vo_mpd_manager::Parse_mpd(vo_http_stream *ptr_fragment){
	m_video_index = -1; m_audio_index = -1;
	destroy();
//	if(m_manifest.adaptionSet){delete []m_manifest.adaptionSet; m_manifest.adaptionSet = NULL; m_manifest.group_count = 0;}
	if( !m_reader.Parse(ptr_fragment,m_url, &m_manifest))
	return VO_FALSE;
	m_playlist_item_count++;
	int kid = 0;
	int video_count= 0;
	int count = 0;
	for(int i =0 ;i< m_manifest.group_count; i++){
		if(m_manifest.adaptionSet[i].nTrackType==VO_SOURCE_TT_VIDEO){
		  video_count++; 
		}
	}
	for(int i =0 ;i< m_manifest.group_count; i++){
		//if(m_reader.StrCompare(	pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO,"video/mp4")==0 &&  !(kid &0x01)){
		if(m_manifest.adaptionSet[i].nTrackType==VO_SOURCE_TT_VIDEO&&!(kid &0x01)){
		    m_video_index = i;//the first video 
			kid |= 0x01;

#ifdef _USE_NEW_BITRATEADAPTATION
			Representation * ql = m_manifest.adaptionSet[i].m_ptr_QL_head;
			for(int x = 0; x < m_manifest.adaptionSet[i].rpe_number; x++)
			{
				m_BitrateMap[count++].SetBitrate(x, ql->bitrate, ql->video_QL.Width,ql->video_QL.Height);			
				ql = ql->ptr_next;
			}
#endif
		}
		else if( m_manifest.adaptionSet[i].nTrackType==VO_SOURCE_TT_AUDIO && !(kid & 0x02)){
			m_audio_index = i;//the first audio
			kid |= 0x02;

		}
		if( m_video_index != -1){
		
				min_bitrate = m_manifest.adaptionSet[m_video_index].m_ptr_QL_head->bitrate;
				max_bitrate = m_manifest.adaptionSet[m_video_index].m_ptr_QL_tail->bitrate;
		}
	}
	
	     m_video_index =(video_count-1)/2;
		 if(video_count ==2)
		 {
			 m_video_index = 1;
		 }
		// m_video_index = 1;

		 if(video_count>1){
			 Seg_item * item = m_reader.m_ptr_FI_head;
			 while(item != NULL)
			 {
				if(item->nIsVideo==1)
				 item->group_index = m_video_index;
				 item = item->ptr_next;

			 }
		 }
		 m_playlist_duration = (VO_S64)(m_manifest.mediaPresentationDuration);
		 m_manifest.m_start_time = m_reader.GetStartTime();
	    // VOLOGR("m_playlist_duration2  %lld",m_playlist_duration);
	     ptr_fragment->closedownload();

	 if(m_video_index >= 0)
		 m_nCurBitrateCounts = m_manifest.adaptionSet[m_video_index].rpe_number;
	 else
		 m_nCurBitrateCounts = 0;
	// m_manifest.m_start_time =  m_manifest.adaptionSet[m_video_index].m_ptr_QL_head->m_ptr_SG_head->starttime;
	 if(m_manifest.audio_scale!=m_manifest.video_scale)
	 m_manifest.m_start_time_audio = m_manifest.adaptionSet[m_audio_index].m_ptr_QL_head->m_ptr_SG_head->starttime;
	 else
		m_manifest.m_start_time_audio = m_reader.GetStartTime();

         return VO_TRUE;

}
VO_S64  vo_mpd_manager::set_pos( VO_S64 pos )
{
	
	VO_S64 start_time = 0;
	if(pos == 0) return 0;
	start_time = m_reader.find_fragment_starttime(pos, m_video_index ==-1?0 : 1) ;//0 audio, 1 video
    VOLOGR("manager start_time :%lld",start_time);
	return start_time;

}

#ifdef _USE_NEW_BITRATEADAPTATION
void vo_mpd_manager::AddtimeStamp2Map( VO_U64 timeStamp, int nFlag/* = 0*/)
{
	if(m_video_index >= 0)
	{
		/*int birate_number = m_manifest.adaptionSet[m_video_index].rpe_number-1;*/
		m_BitrateMap[(m_nCurBitrateCounts -1 )-cur_bitrate_index].AddtimeStamp2Map(timeStamp,nFlag);
	}
}

int vo_mpd_manager::CheckDelayTimeStamp(int delayTime, VO_U64 timeStamp)
{
	/*for(int i = 0; i< m_manifest.adaptionSet[m_video_index].rpe_number; i++)*/
	for(int i = 0; i< m_nCurBitrateCounts; i++)
	{
		if(m_BitrateMap[i].CheckDelayTimeStamp(delayTime,timeStamp) == 0)
			break;
	}
	return 0;
}

VO_BOOL vo_mpd_manager::GetIsSmoothPlayed(int index)
{
	return m_BitrateMap[index].GetIsSmoothPlayed();
}

void vo_mpd_manager::FlushMap()
{

	for(int i = 0; i< m_nCurBitrateCounts; i++)
	{
		m_BitrateMap[i].FlushMap();
	}
}
VO_VOID vo_mpd_manager::CheckBetterQL()
{

	//	VOLOGR("+reduce_QualityLevel bitrate(%lld),index(%d) ",m_better_QualityLevel.bitrate,m_better_QualityLevel.index_QL);
	Representation * ptr_item = NULL;
	ptr_item = m_manifest.adaptionSet[m_video_index].m_ptr_QL_head;
	int birate_number = 0;
	memcpy(&m_better_QualityLevel,ptr_item,sizeof(Representation));
	
	while(ptr_item)
	{
		if(!GetIsSmoothPlayed(birate_number++))//ptr_item->index_QL))
			break;

		memcpy(&m_better_QualityLevel,ptr_item,sizeof(Representation));
		ptr_item = ptr_item->ptr_next;
	}
	VOLOGR("better_QualityLevel %lld",m_better_QualityLevel.bitrate );

}
#endif

int vo_mpd_manager::GetSegmentCounts()
{
	int count = m_reader.m_count_audio+m_reader.m_count_video;
	//return m_reader.m_seg_count;
	return count;
}

int vo_mpd_manager::FindIndexByStartTime(VO_U64 startTime)
{
	Seg_item *seg = m_reader.m_ptr_FI_head;
	int i =0;
	while(seg)
	{
		if(seg->starttime == startTime)
			return i;
		seg = seg->ptr_next;
		i++;
	}
	return -1;
}

VO_S64  vo_mpd_manager::GetLastStartTime(VO_U8 isVideo)
{
	Seg_item *seg = m_reader.m_ptr_FI_head;
	VO_S64 time = seg->starttime;
	while(seg)
	{
		if(seg->nIsVideo == isVideo)
			time = seg->starttime;
		seg = seg->ptr_next;
	}
	return time;
}

VO_U32 vo_mpd_manager::SetBACAP(VO_SOURCE2_CAP_DATA* pCap)
{
	if(pCap == NULL)
		return VO_ERR_INVALID_ARG;
	VOLOGI("BAInfo*** Max Cap %d , Width %d , Height %d" , pCap->nBitRate,pCap->nVideoWidth,pCap->nVideoHeight );

	for(int i= 0 ;i< 20;i++)
	{
		m_BitrateMap[i].SetBACAP(pCap);
	}
	return VO_ERR_NONE;
}