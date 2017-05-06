#include "commonheader.h"
#include "cmdhdl.h"

CCmdHandle::CCmdHandle()
: m_cmdBuffer(NULL)
, m_responseBuffer(NULL)
{
}

CCmdHandle::~CCmdHandle()
{
	VO_SAFE_DELETE(m_cmdBuffer);
	VO_SAFE_DELETE(m_responseBuffer);
}

bool CCmdHandle::Init()
{
	return true;
}

char * CCmdHandle::GetNextResponseLine(char * _line)
{
	if(_line == NULL || *_line == '\0')
		return NULL;

	for(char * _c=_line; *_c != '\0'; ++_c)
	{
		if(*_c == '\n')
		{
			++_c; //skip '\n' to next newline
			return _c;
		}
	}
	return NULL;
}
