#include "CBaseFileParser.h"
#include "CFileFormatCheck.h"
#include "CBaseTrack.h"

typedef VO_S32 (VO_API * VOGETFILEREADAPI) (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag);

CBaseFileParser::CBaseFileParser(VO_FILE_OPERATOR * pFileOpt , VO_MEM_OPERATOR *pMemOpt , VO_LIB_OPERATOR *pLibOpt )
: CMemoryOpt(pMemOpt , VO_INDEX_FILTER_SOURCE)
, mpFileOpt(pFileOpt)
, mpMemOpt(pMemOpt)
, mpLibOpt(pLibOpt)
, mnFileOffset(0)
, mnFileLength(0)
, mFileFormat(VO_FILE_FFUNKNOWN)
, mhFileHandle(NULL)
, mnTrackCount(0)
, mppTrackArray(NULL)
{
	ZeroMemory(mpFileName , sizeof(TCHAR) * MAX_PATH);
	SetLibOperator(mpLibOpt);
}

CBaseFileParser::~CBaseFileParser(void)
{
	Release();
}

HRESULT CBaseFileParser::Load(TCHAR *pFileName)
{
	if(_tcscmp(pFileName , mpFileName))
	{
		_tcscpy(mpFileName, pFileName);
	}
	else
	{
		return S_FALSE;
	}

	Release();

	mFileFormat = VO_FILE_FFUNKNOWN;
	
	VO_TCHAR szURL[1024];
	vostrcpy (szURL, (VO_PTCHAR)pFileName);
	//vostrupr (szURL);

	if (!vostrncmp (szURL, _T("RTSP://"), 6))
		mFileFormat = VO_FILE_FFSTREAMING_RTSP;
	else if (!vostrncmp (szURL, _T("rtsp://"), 6))
		mFileFormat = VO_FILE_FFSTREAMING_RTSP;
	else if (!vostrncmp (szURL, _T("MMS://"), 5))
		mFileFormat = VO_FILE_FFSTREAMING_RTSP;
	else if (!vostrncmp (szURL, _T("mms://"), 5))
		mFileFormat = VO_FILE_FFSTREAMING_RTSP;
	else if (!vostrncmp (szURL, _T("HTTP://"), 6))
		mFileFormat = VO_FILE_FFSTREAMING_HTTPPD;
	else if (!vostrncmp (szURL, _T("http://"), 6))
		mFileFormat = VO_FILE_FFSTREAMING_HTTPPD;

	if(mFileFormat == VO_FILE_FFUNKNOWN)
	{
		CFileFormatCheck fmtCheck(mpFileOpt , mpMemOpt);

		VO_FILE_SOURCE filSource;
		memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
		filSource.nMode = VO_FILE_READ_ONLY;
		filSource.pSource = pFileName;
		filSource.nFlag = VO_FILE_TYPE_NAME;
		filSource.nOffset = mnFileOffset;
		filSource.nLength = mnFileLength;

		mFileFormat = fmtCheck.GetFileFormat(pFileName , ~FLAG_CHECK_AUDIOREADER);
		if(mFileFormat == VO_FILE_FFUNKNOWN)
		{
			mFileFormat = CFileFormatCheck::GetFileFormat((VO_PTCHAR)pFileName, FLAG_CHECK_ALL);
		}
	}

	if(mFileFormat == VO_FILE_FFUNKNOWN)
		return E_FAIL;

	HRESULT hr = LoadDll();
	if(hr != S_OK)
		return hr;

	return OpenSource();
}

HRESULT CBaseFileParser::OpenSource()
{
	VO_SOURCE_OPENPARAM sourceOpen;
	Set(&sourceOpen , 0 , sizeof(sourceOpen));

	sourceOpen.pLibOP = mpLibOpt;
	sourceOpen.pMemOP = mpMemOpt;
	sourceOpen.pSourceOP = mpFileOpt;

	VO_FILE_SOURCE fileSource;
	Set(&fileSource , 0 , sizeof(fileSource));

	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nMode = VO_FILE_READ_ONLY;
	fileSource.pSource = mpFileName;
	fileSource.nLength = mnFileLength;
	fileSource.nOffset = mnFileOffset;

	sourceOpen.pSource = &fileSource;
	sourceOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR | VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE;

	VO_U32 nRC = mfunFileRead.Open(&mhFileHandle , &sourceOpen);
	if(nRC != VO_ERR_SOURCE_OK || mhFileHandle == NULL)
		return E_FAIL;

	VO_SOURCE_INFO srcInfo;
	Set(&srcInfo , 0 , sizeof(srcInfo));
	nRC = mfunFileRead.GetSourceInfo(mhFileHandle , &srcInfo);
	if(nRC != VO_ERR_SOURCE_OK)
		return E_FAIL;

	mnTrackCount = srcInfo.Tracks;
	mppTrackArray = (CBaseTrack **)Alloc(sizeof(CBaseTrack *) * mnTrackCount);
	if(mppTrackArray == NULL)
		return E_OUTOFMEMORY;

	Set(mppTrackArray , 0 , sizeof(CBaseTrack *) * mnTrackCount);

	VO_SOURCE_TRACKINFO trackInfo;
	for (int i = 0 ; i < mnTrackCount ; i++)
	{
		//nRC = mfunFileRead.GetTrackInfo(mhFileHandle , i , &trackInfo);
		//if(nRC != VO_ERR_SOURCE_OK)
		//	return E_FAIL;

		mppTrackArray[i] = new CBaseTrack(mpMemOpt , mhFileHandle , &mfunFileRead , i);
		mppTrackArray[i]->InitTrack();
	}

	return S_OK;
}

HRESULT CBaseFileParser::Release()
{
	if(mppTrackArray != NULL)
	{
		for (int i = 0 ; i < mnTrackCount ; i++)
		{
			delete mppTrackArray[i];
		}

		Free(mppTrackArray);
	}
	return S_OK;
}


HRESULT	CBaseFileParser::Stop()
{
	if(mppTrackArray != NULL)
	{
		for (int i = 0 ; i < mnTrackCount ; i++)
		{
			mppTrackArray[i]->Stop();
		}
	}

	return S_OK;
}

HRESULT	CBaseFileParser::Pause()
{
	if(mppTrackArray != NULL)
	{
		for (int i = 0 ; i < mnTrackCount ; i++)
		{
			mppTrackArray[i]->Pause();
		}
	}

	return S_OK;
}

HRESULT CBaseFileParser::Run()
{
	if(mppTrackArray != NULL)
	{
		for (int i = 0 ; i < mnTrackCount ; i++)
		{
			mppTrackArray[i]->Run();
		}
	}

	return S_OK;
}

int CBaseFileParser::GetTrackCount()
{
	return mnTrackCount;
}

CBaseTrack *CBaseFileParser::GetTrack(int nIndex)
{
	if(mppTrackArray == NULL)
		return NULL;

	if(nIndex < 0 || nIndex > mnTrackCount - 1)
		return NULL;

	return mppTrackArray[nIndex];
}

HRESULT CBaseFileParser::SetParam(int nID , void *pParam)
{
	if(mhFileHandle != NULL)
	{
		return mfunFileRead.SetSourceParam(mhFileHandle , nID , pParam);
	}

	return E_NOTIMPL;
}

HRESULT	CBaseFileParser::GetParam(int nID , void *pParam)
{
	return E_NOTIMPL;
}

HRESULT CBaseFileParser::LoadDll()
{
	if (mFileFormat == VO_FILE_FFMOVIE_AVI)
	{
		vostrcpy (m_szDllFile, _T("voAVIFR"));
		vostrcpy (m_szAPIName, _T("voGetAVIReadAPI"));
		
	}
	else if (mFileFormat == VO_FILE_FFMOVIE_MP4 || mFileFormat == VO_FILE_FFMOVIE_MOV)
	{
		vostrcpy (m_szDllFile, _T("voMP4FR"));
		vostrcpy (m_szAPIName, _T("voGetMP4ReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFMOVIE_ASF)
	{
		vostrcpy (m_szDllFile, _T("voASFFR"));
		vostrcpy (m_szAPIName, _T("voGetASFReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFMOVIE_MPG)
	{
		vostrcpy (m_szDllFile, _T("voMPGFR"));
		vostrcpy (m_szAPIName, _T("voGetMPGReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFMOVIE_TS)
	{
		vostrcpy (m_szDllFile, _T("voTSFR"));
		vostrcpy (m_szAPIName, _T("voGetTSReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFMOVIE_FLV)
	{
		vostrcpy (m_szDllFile, _T("voFLVFR"));
		vostrcpy (m_szAPIName, _T("voGetFLVReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFMOVIE_REAL)
	{
		vostrcpy (m_szDllFile, _T("voRealFR"));
		vostrcpy (m_szAPIName, _T("voGetRealReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFAUDIO_OGG)
	{
		vostrcpy (m_szDllFile, _T("voOGGFR"));
		vostrcpy (m_szAPIName, _T("voGetOGGReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFAUDIO_MIDI)
	{
		vostrcpy (m_szDllFile, _T("voMidiFR"));
		vostrcpy (m_szAPIName, _T("voGetMidiReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFSTREAMING_RTSP || mFileFormat == VO_FILE_FFAPPLICATION_SDP)
	{
		vostrcpy (m_szDllFile, _T("voSrcRTSP"));
		vostrcpy (m_szAPIName, _T("voGetRTSPReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFSTREAMING_HTTPPD)
	{
		vostrcpy (m_szDllFile, _T("voSrcPD"));
		vostrcpy (m_szAPIName, _T("voGetPDReadAPI"));
	}
	else if (mFileFormat == VO_FILE_FFMOVIE_CMMB)
	{
		vostrcpy (m_szDllFile, _T("voCMMBParser"));
		vostrcpy (m_szAPIName, _T("voGetMTVReadAPI"));
	}
	else if( mFileFormat == VO_FILE_FFMOVIE_MKV )
	{
		vostrcpy( m_szDllFile , _T("voMKVFR") );
		vostrcpy( m_szAPIName , _T("voGetMKVReadAPI") );
	}
	else
	{
		vostrcpy (m_szDllFile, _T("voAudioFR"));
		vostrcpy (m_szAPIName, _T("voGetAudioReadAPI"));
	}


	vostrcat (m_szDllFile, _T(".Dll"));

	if (LoadLib (g_hInst) == 0)
	{
		return E_NOTIMPL;
	}

	VOGETFILEREADAPI pAPI = (VOGETFILEREADAPI) m_pAPIEntry;
	pAPI (&mfunFileRead, 0);

	return S_OK;
}

HRESULT CBaseFileParser::UnloadDll()
{
	FreeLib();
	return S_OK;
}

