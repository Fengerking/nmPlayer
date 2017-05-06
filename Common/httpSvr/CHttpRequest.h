	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CHttpRequest.h

	Contains:	CHttpRequest header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#ifndef __CHttpRequest_H__
#define __CHttpRequest_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CHttpRequest
{
public:
    enum HTTP_REQUEST_TYPE
    {
        hrqUnknown = 0,
        hrqTimeout,
        hrqHead,
        hrqGet, 
        hrqGetModified
    };

public:
	CHttpRequest (void);
	virtual ~CHttpRequest (void);

	virtual bool	Parse (const char* pBuffer, int nLen);
	virtual bool	ParseLine (char* pLine, int nSize);
	virtual bool	TimeOut (void);
	virtual void	Reset (void);

protected:
	bool	GetLine (char ** pBuffer, int* nLen, char** pLine, int* nLineSize);
	bool	GetWord (char ** ppLine, int* nLineSize, char** pWord, int* nWordSize);
	void	strlwr (char* pText);
	

public:
	HTTP_REQUEST_TYPE	m_nType;
	char				m_szTarget[256];
	char				m_szHost[32];
	unsigned short		m_nVerMajor;
	unsigned short		m_nVerMinor;
	unsigned long		m_lPosBegin;
	unsigned long		m_lPosEnd;
	unsigned long		m_lLength;
	bool				m_bTooLarge;
	bool				m_bURLTooLong;
	bool				m_bKeepAlive;
	unsigned long		m_lProcessed;
    bool                m_bGzip;
};
    
#ifdef _VONAMESPACE
}
#endif // End define _VONAMESPACE

#endif // __CHttpRequest_H__
