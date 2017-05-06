#include "sdp_parse.h"
#include "sdp_session.h"
#include "sdp_media.h"
#include "sdp_utility.h"

#if defined(LINUX)
#   include "vocrsapis.h"
#endif

CSDPParse::CSDPParse()
: m_pSDPSession(NULL)
{
}

CSDPParse::~CSDPParse()
{
	list_T<CSDPMedia *>::iterator iter;
	for(iter=m_listSDPMedia.begin(); iter!=m_listSDPMedia.end(); ++iter)
	{
		delete *iter;
	}
	m_listSDPMedia.clear();

	SAFE_DELETE(m_pSDPSession);
}

bool CSDPParse::Open(const TCHAR * _sdpFilePath)
{
	FILE * pSDPFile = _tfopen(_sdpFilePath, _T("rb"));
	if(pSDPFile == NULL)
		return false;

	fseek(pSDPFile, 0, SEEK_END);
	long sdpDataSize = ftell(pSDPFile);
	fseek(pSDPFile, 0, SEEK_SET);

	char * _sdpData = new char[(sdpDataSize + 4) / 4 * 4];
	if(_sdpData == NULL)
	{
		fclose(pSDPFile);
		return false;
	}
	fread(_sdpData, 1, sdpDataSize, pSDPFile);
	_sdpData[sdpDataSize] = '\0';

	bool rc = OpenSDP(_sdpData);
	delete[] _sdpData;
	return rc;
}

bool CSDPParse::OpenSDP(const char * _sdpText)
{
	const char * _line = _sdpText;
	const char * _nextLine = NULL;

	m_pSDPSession = new CSDPSession();
	if(m_pSDPSession == NULL)
		return false;
	while(true)
	{
		m_pSDPSession->AddSessionDescriptionLine(_line);

		_nextLine = SDP_GetNextLine(_line);
		_line = _nextLine;
		if(_line == NULL || *_line == '\0'|| _strnicmp(_line, "m=", 2) == 0)
			break;
	}

	int index = 0;
	while(true)
	{
		if(_line == NULL || *_line == '\0')
			break;

		if(_strnicmp(_line, "m=", 2) != 0)
			break;

		CSDPMedia * pSDPMedia = new CSDPMedia(m_pSDPSession, index++);
		if(pSDPMedia == NULL)
			break;
		m_listSDPMedia.push_back(pSDPMedia);
		while(true)
		{
			pSDPMedia->AddMediaDescriptionLine(_line);

			_nextLine = SDP_GetNextLine(_line);
			_line = _nextLine;
			if(_line == NULL || *_line == '\0'|| _strnicmp(_line, "m=", 2) == 0)
				break;
		}
	}
	
	return true;
}

CSDPSession * CSDPParse::GetSDPSession()
{
	return m_pSDPSession;
}

CSDPMedia * CSDPParse::GetSDPMedia(int index)
{
	list_T<CSDPMedia *>::iterator iter;
	for(iter=m_listSDPMedia.begin(); iter!=m_listSDPMedia.end(); ++iter)
	{
		CSDPMedia * pSDPMedia = *iter;
		if(pSDPMedia->GetIndex() == index)
			return pSDPMedia;
	}

	return NULL;
}

int CSDPParse::GetSDPMediaCount()
{
	return m_listSDPMedia.size();
}
