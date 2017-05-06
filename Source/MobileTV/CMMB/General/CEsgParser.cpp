#include "CEsgParser.h"
#include "fMacros.h"


CEsgParser::CEsgParser(void)
{
}

CEsgParser::~CEsgParser(void)
{
}


bool CEsgParser::ParseContentInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
	TiXmlDocument doc;

	if (!doc.LoadFile((char*)pXML, nLen, TIXML_DEFAULT_ENCODING)) 
		return false;

	TiXmlNode* pNode = doc.FirstChild(ESG_BASE_TAG);

	if (!pNode) 
		return false;

	int nVal				= 0;
	const char* pszVal		= VO_NULL;
	TiXmlNode* pChildNode	= VO_NULL;
	TiXmlNode* pCurrNode	= VO_NULL;
	TiXmlElement* pElement	= VO_NULL;

	pChildNode = pNode->FirstChild();
	while (pChildNode)
	{
		if (!IsTheSameTag(pChildNode, TAG_CONTENT))
		{
			pChildNode = pChildNode->NextSibling();
			continue;
		}

		EsgContentInfo info;

		pChildNode->ToElement()->Attribute(TAG_CONTENT_ID, &nVal);
		info.content_id = nVal;

		TiXmlNode* pLevel1 = pChildNode->FirstChild();

		while (pLevel1)
		{
			const char* pName = pLevel1->Value();

			if (!StrCompare(TAG_CONTENT_CLASS, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					MultiByte2WideChar(info.content_class, pCurrNode->Value(), strlen(pCurrNode->Value()));
				}
			}
			else if (!StrCompare(TAG_CONTENT_GENRE, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					info.content_genre = atoi(pCurrNode->Value());
				}
			}
			else if (!StrCompare(TAG_DURATION, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pszVal			= pCurrNode->Value();
					info.duration	= atol(pszVal);
				}
			}
			else if (!StrCompare(TAG_DIGEST_INFO, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pCurrNode = pCurrNode->FirstChild();
					if (pCurrNode)
					{
						MultiByte2WideChar(info.digest_info, pCurrNode->Value(), strlen(pCurrNode->Value()));
					}
				}
			}
			else if (!StrCompare(TAG_CONTENT_SERVICE_PARA_ID, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pCurrNode = pCurrNode->FirstChild();
					if (pCurrNode)
					{
						info.service_para_id = atol(pCurrNode->Value());
					}
				}
			}
			else if (!StrCompare(TAG_CONTENT_LANG_SPECIAL, pName))
			{
				pszVal = pLevel1->ToElement()->Attribute(TAG_LANG);
				MultiByte2WideChar(info.content_lang_special, pszVal, strlen(pszVal));

				pCurrNode = pLevel1->FirstChild(TAG_CONTENT_TITLE);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						MultiByte2WideChar(info.title, pNode->Value(), strlen(pNode->Value()));
					}
				}
			}

			pLevel1 = pLevel1->NextSibling();
		}

		if (pRecver)
			pRecver->WhenParsedEsgContentInfo(&info);

		pChildNode = pChildNode->NextSibling();
	}

	return true;
}

bool CEsgParser::ParseScheduleInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
	TiXmlDocument doc;
	
	if (!doc.LoadFile((char*)pXML, nLen, TIXML_DEFAULT_ENCODING)) 
		return false;

	TiXmlNode* pNode = doc.FirstChild(ESG_BASE_TAG);
	
	if (!pNode) 
		return false;

	int nVal				= 0;
	const char* pszVal		= VO_NULL;
	const char* pAtt		= VO_NULL;
	TiXmlNode* pChildNode	= VO_NULL;
	TiXmlNode* pCurrNode	= VO_NULL;
	TiXmlElement* pElement	= VO_NULL;
	
	pChildNode=pNode->FirstChild();
	while (pChildNode)
	{
		if (!IsTheSameTag(pChildNode, TAG_SCHEDULE))
		{
			pChildNode = pChildNode->NextSibling();
			continue;
		}

		EsgScheduleInfo info;

		pElement	= pChildNode->ToElement();
		pElement->Attribute(TAG_SCHEDULE_ID, &nVal);
		info.schedule_id = nVal;

		TiXmlNode* pLevel1 = pChildNode->FirstChild();

		while (pLevel1)
		{
			const char* pName = pLevel1->Value();

			if (!StrCompare(TAG_SCHEDULE_SERVICE_ID, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					info.service_id = atoi(pCurrNode->Value());
				}
			}
			else if (!StrCompare(TAG_SCHEDULE_CONTENT_ID, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pszVal = pCurrNode->Value();
					info.content_id = atol(pszVal);
				}
			}
			else if (!StrCompare(TAG_SCHEDULE_TITLE, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pCurrNode = pCurrNode->FirstChild();
					if (pCurrNode)
					{
						MultiByte2WideChar(info.title, pCurrNode->Value(), strlen(pCurrNode->Value()));
					}
				}
			}
			else if (!StrCompare(TAG_STARTTIME, pName))
			{
				pCurrNode = pLevel1->FirstChild(TAG_DATE);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						MultiByte2WideChar(info.date, pNode->Value(), strlen(pNode->Value()));
					}
				}
				pCurrNode = pLevel1->FirstChild(TAG_TIME);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						MultiByte2WideChar(info.time, pNode->Value(), strlen(pNode->Value()));
					}
				}
			}
			else if (!StrCompare(TAG_FOR_FREE, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pszVal			= pCurrNode->Value();
					info.for_free	= Str2Bool(pszVal);
				}
			}
			else if (!StrCompare(TAG_LIVE, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pszVal		= pCurrNode->Value();
					info.live	= Str2Bool(pszVal);
				}
			}
			else if (!StrCompare(TAG_REPEAT, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pszVal		= pCurrNode->Value();
					info.repeat = Str2Bool(pszVal);
				}
			}

			pLevel1 = pLevel1->NextSibling();
		}

		if (pRecver)
			pRecver->WhenParsedEsgScheduleInfo(&info);

		pChildNode = pChildNode->NextSibling();
	}

	return true;
}

bool CEsgParser::ParseServiceAuxInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
	TiXmlDocument doc;

	if (!doc.LoadFile((char*)pXML, nLen, TIXML_DEFAULT_ENCODING)) 
		return false;

	TiXmlNode* pNode = doc.FirstChild(ESG_BASE_TAG);

	if (!pNode) 
		return false;

	int nVal				= 0;
	const char* pszVal		= VO_NULL;
	TiXmlNode* pChildNode	= VO_NULL;
	TiXmlNode* pCurrNode	= VO_NULL;
	TiXmlElement* pElement	= VO_NULL;

	pChildNode = pNode->FirstChild();
	while (pChildNode)
	{
		if (!IsTheSameTag(pChildNode, TAG_SERVICE_AUX))
		{
			pChildNode = pChildNode->NextSibling();
			continue;
		}

		EsgServiceAuxInfo info;

		pChildNode->ToElement()->Attribute(TAG_AUX_SERVICE_ID, &nVal);
		info.service_id = nVal;

		TiXmlNode* pLevel1 = pChildNode->FirstChild();

		while (pLevel1)
		{
			const char* pName = pLevel1->Value();

			if (!StrCompare(TAG_SERVICE_LANG_SPC, pName))
			{
				pszVal = pLevel1->ToElement()->Attribute(TAG_LANG);
				MultiByte2WideChar(info.service_lang_special, pszVal, strlen(pszVal));

				pCurrNode = pLevel1->FirstChild(TAG_SERVICE_DESC);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						MultiByte2WideChar(info.service_desc, pNode->Value(), strlen(pNode->Value()));
					}
				}

				pCurrNode = pLevel1->FirstChild(TAG_SERVICE_PROVIDER);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						MultiByte2WideChar(info.service_provider, pNode->Value(), strlen(pNode->Value()));
					}
				}

				pCurrNode = pLevel1->FirstChild(TAG_SERVICE_LANGUAGE);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						MultiByte2WideChar(info.service_lang, pNode->Value(), strlen(pNode->Value()));
					}
				}

			}
			else if (!StrCompare(TAG_MEDIA, pName))
			{
				pCurrNode = pLevel1->FirstChild(TAG_MEDIA_USAGE);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						info.media_type.usage = atoi(pNode->Value());
					}
				}

				pCurrNode = pLevel1->FirstChild(TAG_MEDIA_CONTENT_CLASS);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						MultiByte2WideChar(info.media_type.content_class, pNode->Value(), strlen(pNode->Value()));
					}
				}

				pCurrNode = pLevel1->FirstChild(TAG_MEDIA_URI);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						MultiByte2WideChar(info.media_type.content_class, pNode->Value(), strlen(pNode->Value()));
					}
				}
			}

			pLevel1 = pLevel1->NextSibling();
		}

		if (pRecver)
			pRecver->WhenParsedEsgServiceAuxInfo(&info);

		pChildNode = pChildNode->NextSibling();
	}

	return true;
}

bool CEsgParser::ParseServiceInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
	TiXmlDocument doc;

	if (!doc.LoadFile((char*)pXML, nLen, TIXML_DEFAULT_ENCODING)) 
		return false;

	TiXmlNode* pNode = doc.FirstChild(ESG_BASE_TAG);

	if (!pNode) 
		return false;

	int nVal				= 0;
	const char* pszVal		= VO_NULL;
	TiXmlNode* pChildNode	= VO_NULL;
	TiXmlNode* pCurrNode	= VO_NULL;
	TiXmlElement* pElement	= VO_NULL;

	pChildNode = pNode->FirstChild();
	while (pChildNode)
	{
		if (!IsTheSameTag(pChildNode, TAG_SERVICE))
		{
			pChildNode = pChildNode->NextSibling();
			continue;
		}

		EsgServiceInfo info;

		pChildNode->ToElement()->Attribute(TAG_SERVICE_SERVICE_ID, &nVal);
		info.service_id = nVal;

		TiXmlNode* pLevel1 = pChildNode->FirstChild();

		while (pLevel1)
		{
			const char* pName = pLevel1->Value();

			if (!StrCompare(TAG_SERVICE_CLASS, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pszVal				= pCurrNode->Value();
					info.service_class	= atoi(pszVal);
				}
			}
			else if (!StrCompare(TAG_SERVICE_GENRE, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pszVal				= pCurrNode->Value();
					info.service_genre	= atoi(pszVal);
				}
			}
			else if (!StrCompare(TAG_SERVICE_PARA_ID, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pszVal					= pCurrNode->Value();
					info.service_param_id	= atoi(pszVal);
				}
			}
			else if (!StrCompare(TAG_SERVICE_FOR_FREE, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{
					pszVal			= pCurrNode->Value();
					info.for_free	= Str2Bool(pszVal);
				}
			}
			else if (!StrCompare(TAG_SERVICE_NAME, pName))
			{
				pszVal = pLevel1->ToElement()->Attribute(TAG_LANG);
				MultiByte2WideChar(info.service_name_lang, pszVal, strlen(pszVal));

				pCurrNode = pLevel1->FirstChild(TAG_SERVICE_NAME_STR);
				if (pCurrNode)
				{
					TiXmlNode* pNode = pCurrNode->FirstChild();
					if (pNode)
					{
						MultiByte2WideChar(info.service_name_str, pNode->Value(), strlen(pNode->Value()));
					}
				}
			}

			pLevel1 = pLevel1->NextSibling();
		}

		if (pRecver)
			pRecver->WhenParsedEsgServiceInfo(&info);

		pChildNode = pChildNode->NextSibling();
	}

	return true;
}

bool CEsgParser::ParseServiceParam(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
	TiXmlDocument doc;

	if (!doc.LoadFile((char*)pXML, nLen, TIXML_DEFAULT_ENCODING)) 
		return false;

	TiXmlNode* pNode = doc.FirstChild(ESG_BASE_TAG);

	if (!pNode) 
		return false;

	int nVal				= 0;
	const char* pszVal		= VO_NULL;
	TiXmlNode* pChildNode	= VO_NULL;
	TiXmlNode* pCurrNode	= VO_NULL;
	TiXmlElement* pElement	= VO_NULL;

	pChildNode = pNode->FirstChild();
	while (pChildNode)
	{
		if (!IsTheSameTag(pChildNode, TAG_SERVICE_PARA))
		{
			pChildNode = pChildNode->NextSibling();
			continue;
		}

		EsgServiceParamInfo info;

		pChildNode->ToElement()->Attribute(TAG_SERVICE_PARA_ID, &nVal);
		info.service_param_id = nVal;

		TiXmlNode* pLevel1 = pChildNode->FirstChild();

		while (pLevel1)
		{
			const char* pName = pLevel1->Value();

			if (!StrCompare(TAG_SERVICE_PARA_DATA, pName))
			{
				pCurrNode = pLevel1->FirstChild();
				if (pCurrNode)
				{

				}
			}
			pLevel1 = pLevel1->NextSibling();
		}

		if (pRecver)
			pRecver->WhenParsedEsgServiceParamInfo(&info);

		pChildNode = pChildNode->NextSibling();
	}

	return true;
}

bool CEsgParser::IsTheSameTag(TiXmlNode* pNode, char* pszTag)
{
	return !StrCompare(pNode->Value(), pszTag);
}

int  CEsgParser::StrCompare(const char* p1, const char* p2)
{
#ifdef WIN32
	return stricmp(p1, p2);
#else
	return strcasecmp(p1, p2);
#endif
}

void CEsgParser::MultiByte2WideChar(VO_TCHAR* pWideChar, const char* pMultiByte, int nLen)
{
#ifdef _WIN32
	MultiByteToWideChar(CP_UTF8, 0, pMultiByte, -1, pWideChar, nLen);
#else
	memcpy(pWideChar, pMultiByte, nLen);
#endif
}

bool CEsgParser::Str2Bool(const char* pStr)
{
	return !StrCompare(ESG_VALUE_TRUE, pStr);
}

