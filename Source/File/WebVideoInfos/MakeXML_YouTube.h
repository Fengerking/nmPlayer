#ifndef __MakeXML_YouTube_H__
#define __MakeXML_YouTube_H__

#include "MakeXML.h"

class CMakeXML_YouTube : public CMakeXML
{
public:
	CMakeXML_YouTube(void);
	~CMakeXML_YouTube(void);

	VO_U32 MakeXML(VO_PCHAR pSource, const VO_PCHAR szPath);

protected:
	VO_U32 SplitVideoInfo(VO_PCHAR pSource);

	VO_U32 parseVideoItem(VO_PCHAR pInfo);
	VO_U32 parseVideoEntry(VO_PCHAR pInfo);

private:
	VO_PCHAR m_pBufXML;
};

#endif //__MakeXML_YouTube_H__