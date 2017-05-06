#ifndef _CCASBASE_H_
#define _CCASBASE_H_

#include "voType.h"
#include "voString.h"

#define CAS_PARAM_ADDPROGRAM			2001
#define CAS_PARAM_REMOVEPROGRAM			2002
#define CAS_PARAM_GETCASID				2003
#define CAS_PARAM_GETKDAVERSION			2004

class CCASBase
{
public:
	CCASBase(void);
	~CCASBase(void);

	virtual int InitializeCAS(void * inParam);
	virtual int TerminateCAS(void);
	virtual int GetCASInfo(void * pInfo);
	virtual int SetCASParam(void * inCAID, int nSize, int nServiceID);
	virtual int SendExMData(VO_BYTE * pExMData, int nSize, int nType, int nServiceID);
	virtual int DescrambleData(VO_BYTE * inData, int inSize, VO_BYTE * outData, unsigned int * outSize, int nType);
	virtual int SetParam(int nID, int nParam);
	virtual int GetParam(int nID, int * nParam);

protected:
	VO_TCHAR	m_szDebugString[256];

protected:
private:
};

#endif	// _CCASBASE_H_
