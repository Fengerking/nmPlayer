#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "voCOMXCfgOne.h"

#undef  LOG_TAG
#define LOG_TAG "voCOMXCfgOne"
#ifdef _WIN32
#pragma warning(disable: 4996) //Use strxxx_s to replace strxxx
#endif

voCOMXCfgOne::voCOMXCfgOne():voCOMXBaseConfig()
{
	if(m_pObjName)
		strcpy(m_pObjName, __FILE__);
}

voCOMXCfgOne::~voCOMXCfgOne()
{

}

int voCOMXCfgOne::getCompCount()
{
	return m_nSectNum;
}
OMX_STRING voCOMXCfgOne::getCompNameByIndex(const int index)
{
	COMXCfgSect* pTemp = NULL;
	pTemp = m_pFirstSect;
	for(int i = 0; i< index; i++)
	{
		pTemp = pTemp->m_pNext;
	}
	if(pTemp)
		return pTemp->m_pName;
	return NULL;

}
int voCOMXCfgOne::getCompRolesCount(OMX_STRING& inComName)
{
	COMXCfgSect* pSect = FindSect(inComName);
	COMXCfgItem* pTemp = m_pFirstItem;
	OMX_U32 nNum = 0;
	for(int i = 0; i <= m_nItemNum && pTemp; i++)
	{
		if(pTemp->m_pSection == pSect)
			nNum ++;
		pTemp = pTemp->m_pNext;
	}
	return nNum;
}
OMX_STRING voCOMXCfgOne::getCompRoleByIndex(OMX_STRING& inComName, const int index)
{	
	char nBuffer[16];
	memset(nBuffer, 0, 16);
	sprintf(nBuffer, "%d", (index+1));
	OMX_STRING pRole = GetItemText(inComName, nBuffer);
	return pRole;
}
