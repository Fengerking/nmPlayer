#include "CEncode.h" 
#include "vcamerabase.h"

CEncode::CEncode(void)
	: m_pDump (NULL)
	, m_bStop (true)
	, m_pEncodeData(NULL)
	, m_dwVideoWidth(0)
	, m_dwVideoHeight(0)
	, m_hCodec(NULL)
{
}

CEncode::~CEncode(void)
{
}

bool CEncode::SetDump (CDump * pDump)
{
	m_pDump = pDump;

	return true;
}

long long CEncode::ReceiveAudioSample(CSampleData * pSample)
{
	return I_VORC_REC_NO_ERRORS;
}

bool CEncode::Start (void)
{
	m_bStop = false;
	return true;
}

bool CEncode::Stop (void)
{
	m_bStop = true;

#ifdef _TEST_263ENCODE_TIME
	Dump();
#endif

#ifdef _TEST_ENCODE_TIME
	Dump();
#endif	

	return true;
}

