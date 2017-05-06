
#ifndef _contentComponent_Tag_H
#define _contentComponent_Tag_H

#include "voType.h"
#include "voSource2.h"
#include "tinyxml.h"
#include  "Common_Tag.h"

#define MPD_ID					"id"
#define MPD_LANG				"lang"
#define MPD_CONTENTTYPE			"contentType"
#define MPD_PAR					"par"




class contentComponent_Tag:public Common_Tag
{
public:
	contentComponent_Tag();

public:
	virtual VO_U32 Init(TiXmlNode* pNode);

public:
	VO_U32 GetID(){return m_uID;}
	VO_CHAR *GetLang(){ return m_chLang;}
	VO_CHAR *GetContentType(){ return m_chContentType;}
	VO_CHAR *GetPar(){ return m_chPar;}
private:
	VO_U32	m_uID;
	VO_CHAR m_chLang[20];
	VO_CHAR m_chContentType[10];
	VO_CHAR m_chPar[20];
};



#endif

