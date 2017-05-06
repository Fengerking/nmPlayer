#ifndef __CMDHDL_H__
#define __CMDHDL_H__
#include "voPDPort.h"
class CCmdHandle:MEM_MANAGER
{
public:
	CCmdHandle();
	virtual ~CCmdHandle();

public:
	virtual bool Init();
public:
	virtual char * GetNextResponseLine(char * _line);

protected:
	char * m_cmdBuffer;
	char * m_responseBuffer;
};


#endif //__CMDHDL_H__