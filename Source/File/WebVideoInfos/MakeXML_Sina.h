#ifndef __CMakeXML_Sina__
#define __CMakeXML_Sina__

#include "MakeXML.h"

class CMakeXML_Sina :
	public CMakeXML
{
public:
	CMakeXML_Sina(void);
	~CMakeXML_Sina(void);

	VO_U32 MakeXML(VO_PCHAR pSource, const VO_PCHAR szPath);

protected:
	VO_U32 parseVideoItem(VO_PCHAR pInfo);
	VO_U32 parseClassVideo(VO_PCHAR pInfo); //for search

private:
	VO_PCHAR m_pBufXML;
};

#endif //__CMakeXML_Sina__
