#ifndef __CMakeXML_H__
#define __CMakeXML_H__
#include "voType.h"
#include "voLog.h"

#define Size_BufXML 1024*70

class CMakeXML
{
public:
	CMakeXML(void);
	virtual ~CMakeXML(void);

	virtual VO_U32 MakeXML(VO_PCHAR pSource, const VO_PCHAR szPath) = 0;

protected:
	VO_U32 SpecialCharacters(VO_PCHAR pSource);
};

#endif //__CMakeXML_H__
