#ifndef __CMakeXML_TuDou_H__
#define __CMakeXML_TuDou_H__

#include "MakeXML.h"

class CMakeXML_TuDou : public CMakeXML
{
public:
	CMakeXML_TuDou(void);
	~CMakeXML_TuDou(void);

	VO_U32 MakeXML(VO_PCHAR pSource, const VO_PCHAR szPath);

protected:
	VO_U32 parseVideoItem(VO_PCHAR pInfo);

private:
	VO_PCHAR m_pBufXML;
};

#endif //__CMakeXML_TuDou_H__
