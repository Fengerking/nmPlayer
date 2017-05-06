#pragma once
#include "voYYDef_filcmn.h"
#include "voSink.h"
#include "CBaseAssembler.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif




class CBaseWriter
{
public:
	CBaseWriter(void);
	virtual ~CBaseWriter(void);

public:
	virtual VO_U32	Open(VO_FILE_SOURCE* pFileSource , VO_SINK_OPENPARAM * pParam);
	virtual VO_U32	Close();
	virtual VO_U32	AddSample( VO_SINK_SAMPLE * pSample);
	virtual VO_U32	SetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32	GetParam(VO_U32 uID, VO_PTR pParam);

private:
	virtual CBaseAssembler* doCreateAssembler()=0;
	virtual CBaseDumper*	doCreateDumper(VO_FILE_OPERATOR* op);

protected:
	VO_BOOL			m_bIsFirstVideo;
	VO_BOOL			m_bIsFirstAudio;
	VO_S64			m_nTimeOffset;

	VO_PTR			m_hFile;
	VO_U64			m_ullFileSize;
	CBaseDumper*	m_pDumper;
	CBaseAssembler*	m_pAssembler;
	ASSEMBLERINITINFO* m_pAssemblerInitInfo;

	voCMutex		m_Mutex;
};

#ifdef _VONAMESPACE
}
#endif
