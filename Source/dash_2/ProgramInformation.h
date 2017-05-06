
#ifndef _ProgramInformation_H
#define _ProgramInformation_H

#include "voType.h"
#include "voSource2.h"
#include "tinyxml.h"
#include  "Common_Tag.h"




#define PRO_LANG				"lang"
#define PRO_MORE_URL			"moreInformationURL"
#define TITLE_TAG			     "Title"
#define SOURCE_TAG			     "Source"
#define COPY_TAG			     "Copyright"





class ProgramInformation:public Common_Tag
	{
	public:
		ProgramInformation(void);
		~ProgramInformation(void);
	public:
		virtual VO_U32 Init(TiXmlNode* pNode);
	public:
		
	    VO_CHAR *GetLang(){ return m_chLang;}
		VO_CHAR *GeMoreUrl(){ return m_more_Url;}
		VO_CHAR *GeTitle(){ return m_title;}
		VO_CHAR *GeSource(){ return m_source;}
		VO_CHAR *GeCopyRight(){ return m_copy_right;}
	private:
	
		VO_CHAR m_chLang[256];
		VO_CHAR m_more_Url[256];
		VO_CHAR m_title[256];
		VO_CHAR m_source[256];
		VO_CHAR m_copy_right[256];
		Common_Tag * m_comtag;
};
#endif