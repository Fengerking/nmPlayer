#include "Role_Tag.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

Role_Tag::Role_Tag(void)
{
	Delete();
}

Role_Tag::~Role_Tag(void)
{
	
}
VO_VOID Role_Tag::Delete(void)
{
	memset(m_chLang, 0x00, sizeof(m_chLang));
	m_codectype  = VO_VIDEO_CodingH264;
	m_track_type = VO_SOURCE_TT_VIDEO;
	
}
VO_U32 Role_Tag::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	  char* attriValue =  NULL;
	  int nSize = 0;
	  if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(GROUP_STREAMING_MIMETYPE),&attriValue,nSize) == VO_ERR_NONE)
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
				m_pXmlLoad->GetNextSibling(pNode,&pNode);
			//	pNode = pNode->NextSibling();
				//continue;
			}
	  }
	  }
	  if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(GROUP_LANG),&attriValue,nSize) == VO_ERR_NONE)
	  {
		  if(attriValue)
		  memcpy(m_chLang, attriValue, strlen(attriValue));
	  }
	  if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(TAG_CODECS),&attriValue,nSize) == VO_ERR_NONE)
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

	return VO_RET_SOURCE2_OK;
}
