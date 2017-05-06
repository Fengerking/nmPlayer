#pragma once
#include "voType.h"
#include <string.h>

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define LOG_TAG "ASXParser"

struct REFList//reference
{
	REFList() : pNextREF(NULL) { memset(szURL, 0, 1024); }

	~REFList() {
		if(pNextREF)
			delete pNextREF;
	};

	VO_CHAR szURL[1024];	
	
	VO_S32 nFlag;

	REFList *pNextREF;
};

class CASXParser
{
public:
	CASXParser(void);
	~CASXParser(void);

	REFList * Parse(VO_CHAR *);

	VO_S32 getCount();

	VO_VOID Close();


private:
	REFList *m_pREFsHead;
	REFList *m_pREFsTail;
};

#ifdef _VONAMESPACE
}
#endif