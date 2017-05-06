#pragma once
#define MAX_FILE_LEN2 512
#include <stdio.h>
#include <stdlib.h>
#include "vompType.h"

class CTestDumpFile
{
	bool	m_bEnabled;
	FILE*	m_fAudio;
	FILE*	m_fVideo;
	VOMP_BUFFERTYPE			m_videoBuf;
	VOMP_BUFFERTYPE			m_audioBuf;
	int		ReadFrame(FILE* file,VOMP_BUFFERTYPE * pBuffer,VOMP_BUFFERTYPE * target);
public:
	CTestDumpFile(void);
	virtual ~CTestDumpFile(void);
	bool		IsEnabled(){return m_bEnabled;}
	void		Enable(bool enabled){m_bEnabled=enabled;};
	int			ReadAudio (VOMP_BUFFERTYPE * pBuffer);
	int			ReadVideo (VOMP_BUFFERTYPE * pBuffer);
	void		SetMediaFile(char* audioFile,char* videoFile);
	void		Reset();
};
