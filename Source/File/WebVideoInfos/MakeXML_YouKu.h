#ifndef __CMakeXML_YouKu_H__
#define __CMakeXML_YouKu_H__

#include "MakeXML.h"

class CMakeXML_YouKu : public CMakeXML
{
public:
	CMakeXML_YouKu(void);
	~CMakeXML_YouKu(void);

	VO_U32 MakeXML(VO_PCHAR pSource, const VO_PCHAR szPath);

protected:
	VO_U32 parseVideoItem(VO_PCHAR pInfo);
	VO_U32 parseClassVideo(VO_PCHAR pInfo); //for search

private:
	VO_PCHAR m_pBufXML;
};

#endif //__CMakeXML_YouKu_H__
