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
VO_U32 Role_Tag::Init(TiXmlNode* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	  const char* attriValue = pNode->ToElement()->Attribute(GROUP_STREAMING_MIMETYPE);
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
				pNode = pNode->NextSibling();
				//continue;
			}
	  }
	  attriValue = pNode->ToElement()->Attribute(GROUP_LANG);
	  if(attriValue)
		  memcpy(m_chLang, attriValue, strlen(attriValue));
	  attriValue = pNode->ToElement()->Attribute(TAG_CODECS);
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

	return VO_RET_SOURCE2_OK;
}
