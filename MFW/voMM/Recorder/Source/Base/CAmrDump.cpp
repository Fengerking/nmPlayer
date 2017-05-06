#include "CAmrDump.h"
#include "vcamerabase.h"
#include "cmnFile.h"

CAmrDump::CAmrDump(void)
{
	m_pFile = NULL;
}

CAmrDump::~CAmrDump(void)
{
}

long long CAmrDump::CreateVideoTrack (int nWidth, int nHeight)
{
	return I_VORC_REC_NO_ERRORS;
}

long long CAmrDump::CreateAudioTrack (int nSampleRate, int nChannels, int nBits, int nCodec)
{
	return I_VORC_REC_NO_ERRORS;
}

bool CAmrDump::SetHeaderInfo(unsigned char* pInfo, int nInfoLen)
{
	return true;
}

TCHAR* CAmrDump::GetFileExtName()
{
	return _T("Amr");
}


bool CAmrDump::doInit()
{
	return true;
}

bool CAmrDump::doStart()
{
	VO_FILE_SOURCE filSource;
	filSource.pSource = m_szName;
	filSource.nFlag = VO_SOURCE_OPENPARAM_FLAG_SOURCENAME;

	//m_pFile = _tfopen(m_szName, _T("wb"));
	m_pFile = cmnFileOpen(0, &filSource, VO_FILE_WRITE_ONLY);

	if(!m_pFile)
		return false;

	//fwrite("#!AMR\n", 1, strlen("#!AMR\n"), m_pFile);
	cmnFileWrite(0, m_pFile, (VO_PTR)"#!AMR\n", strlen("#!AMR\n"));

	return true;
}

bool CAmrDump::doStop()
{
	if(m_pFile)
	{
		//fclose(m_pFile);
		cmnFileClose(0, m_pFile);
		m_pFile = NULL;
		return true;
	}

	return false;
}


int CAmrDump::DumpVideoSample(CSampleData* pSample)
{
	return I_VORC_REC_NO_ERRORS;
}

int CAmrDump::DumpAudioSample(CSampleData* pSample)
{
	if(m_pFile)
	{
		//bool bRet = (pSample->GetDataLen() == fwrite(pSample->GetDataPoint(), 1, pSample->GetDataLen(), m_pFile));

		bool bRet = (pSample->GetDataLen() == cmnFileWrite(0, m_pFile, pSample->GetDataPoint(), pSample->GetDataLen()));

		return bRet?I_VORC_REC_NO_ERRORS:I_VORC_REC_WRITE_FAILED;
	}

	return I_VORC_REC_NO_ERRORS;
}
