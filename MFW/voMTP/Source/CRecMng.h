#pragma once
#include "voString.h"
#include "voType.h"

typedef enum
{
	REC_MP4		= 0,
}RecFileFormat;

class CRecMng
{
public:
	CRecMng(void);
	virtual ~CRecMng(void);

public:
	VO_U32	RecStart(TCHAR* pszFileName, VO_U32 nFileFormat=REC_MP4);
	VO_U32	RecStop();

	VO_BOOL	IsRecording();
private:
	VO_BOOL		m_bRecording;
};
