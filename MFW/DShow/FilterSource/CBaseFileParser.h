#pragma once

#include "streams.h"
#include "voFile.h"
#include "voMem.h"
#include "CDllLoad.h"
#include "CMemoryOpt.h"

class CBaseTrack; 
class CBaseFileParser : public CDllLoad , public CMemoryOpt
{
public:
	CBaseFileParser(VO_FILE_OPERATOR * pFileOpt , VO_MEM_OPERATOR *pMemOpt , VO_LIB_OPERATOR *pLibOpt );
	virtual ~CBaseFileParser(void);

	virtual HRESULT	Stop();
	virtual HRESULT	Pause();
	virtual HRESULT Run();

	virtual HRESULT Load(TCHAR *pFileName);
	virtual HRESULT Release();

	virtual int GetTrackCount();
	virtual CBaseTrack *GetTrack(int nIndex);

	virtual HRESULT SetParam(int nID , void *pParam);
	virtual HRESULT	GetParam(int nID , void *pParam);

protected:
	virtual HRESULT OpenSource();

	HRESULT LoadDll();
	HRESULT UnloadDll();

protected:
	VO_SOURCE_READAPI	mfunFileRead;
	TCHAR				mpFileName[MAX_PATH];
	VO_FILE_FORMAT		mFileFormat;
	int					mnFileOffset;
	int					mnFileLength;

	VO_FILE_OPERATOR *	mpFileOpt;
	VO_MEM_OPERATOR	*	mpMemOpt;
	VO_LIB_OPERATOR *	mpLibOpt;

	VO_PTR				mhFileHandle;

	int					mnTrackCount;
	CBaseTrack **		mppTrackArray;
};
