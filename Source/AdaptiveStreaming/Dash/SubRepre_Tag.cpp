#include "SubRepre_Tag.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

SubRepre_Tag::SubRepre_Tag(void)
{
	Delete();
}

SubRepre_Tag::~SubRepre_Tag(void)
{
	
	
}
VO_VOID SubRepre_Tag::Delete(void)
{
   m_codectype  = VO_VIDEO_CodingH264;
	m_track_type = VO_SOURCE_TT_MAX;
	memset(m_uID, 0x00, sizeof(m_uID));
	
}
VO_U32 SubRepre_Tag::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	   char* attriValue = NULL;
	  int nSize = 0;
	  if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(RPE_TYPE),&attriValue,nSize) == VO_ERR_NONE)
	  {
	  if(attriValue)
	  {
		  	if(0==StrCompare(attriValue,"video/mp4"))
			{
				m_track_type = VO_SOURCE_TT_VIDEO;
			}
			else if(0==StrCompare(attriValue,"video/mp2t"))
			{
				m_track_type = VO_SOURCE_TT_VIDEO;
			}
			else if(0==StrCompare(attriValue,"audio/mp4"))
			{
				m_track_type = VO_SOURCE_TT_AUDIO;
			}
			else if(0==StrCompare(attriValue,"text"))
			{
				m_track_type = VO_SOURCE_TT_SUBTITLE;
				//continue;
			}
	  }
	  }
	  if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(RPE_WIDTH),&attriValue,nSize) == VO_ERR_NONE)
	  {
	  if(attriValue)
	  {
		  m_width = _ATO64(attriValue);
	  }
	  }
	  if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(RPE_HEIGHT),&attriValue,nSize) == VO_ERR_NONE)
	  {
	  if(attriValue)
	  {
		  m_height = _ATO64(attriValue);
	  }
	  }
	   if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(RPE_BAND_WIDTH),&attriValue,nSize) == VO_ERR_NONE)
	   {
	  if(attriValue)
	  {
		  m_bandwidth = _ATO64(attriValue);
	  }
	   }
	 if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(RPE_CODEC),&attriValue,nSize) == VO_ERR_NONE)
	 {
	  if(attriValue)
	  {
		   if (StrCompare(attriValue, "H264")== 0)
			  m_codectype = VO_VIDEO_CodingH264;
		  else if(StrCompare(attriValue,"avc1"))
			  m_codectype = VO_VIDEO_CodingH264;
		  else if (StrCompare(attriValue, "WVC1")== 0)
			 m_codectype = VO_VIDEO_CodingVC1;//VOMP_VIDEO_CodingVC1;
		  else if (StrCompare(attriValue, "WmaPro")== 0 || StrCompare(attriValue, "WMAP") == 0)
			  m_codectype = VO_AUDIO_CodingWMA;
		  else if (StrCompare(attriValue, "AACL")== 0)
			  m_codectype = VO_AUDIO_CodingAAC;
	  }
	 }
	 if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(RPE_ID),&attriValue,nSize) == VO_ERR_NONE)
	 {
		 if(attriValue)
		{
			 memcpy(m_uID, attriValue, strlen(attriValue));
			 m_uID[strlen(attriValue)]= '\0';

		}
	 }



	return VO_RET_SOURCE2_OK;
}
