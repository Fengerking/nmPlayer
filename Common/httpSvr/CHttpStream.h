	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CHttpStream.h

	Contains:	CHttpStream header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#ifndef __CHttpStream_H__
#define __CHttpStream_H__

#include "voFile.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
    
class CHttpBaseStream
{
public:
	CHttpBaseStream (void * hUserData){m_hUserData = hUserData, m_llStartPos = 0; m_llEndPos = 0;m_bStreamAvailable=false;memset(m_szSourceURL, 0, 1024);m_nDuration=0;m_bNeedCompressBuffer=false;}
	virtual ~CHttpBaseStream (void){}

	virtual bool		Open (void * pSource, int nFlag)
    {
        strcpy(m_szSourceURL, (char*)pSource);
        return true;
    }
	virtual bool		Close (void) {m_bStreamAvailable=false;return true;}
	virtual bool		Start (void) {return true;}
	virtual bool		Stop (void) {return true;}
	virtual int			Read (unsigned char * pBuff, int nSize) {return 0;}
	virtual bool		Seek (long long llPos, int nFlag) {return false;}
	virtual long long	Size (void) {return 0;}

	virtual long long	GetStartPos (void) {return m_llStartPos;}
	virtual void		SetStartPos (long long llPos) {m_llStartPos = llPos;}
	virtual long long	GetEndPos (void) {return m_llEndPos;}
	virtual void		SetEndPos (long long llPos) {m_llEndPos = llPos;}
    virtual void        SetAvailable(bool bAvailble){m_bStreamAvailable = bAvailble;};
    virtual int         IsAvailable(){return m_bStreamAvailable;};
    virtual char*       GetSourceURL(){return m_szSourceURL;};
    virtual void        SetSourceURL(char* pszURL){if(pszURL){memset(m_szSourceURL, 0, 1024);strcpy(m_szSourceURL, pszURL);}};
    virtual void        SetDuration(int nDuration){m_nDuration = nDuration;};
    virtual int         GetDuration(){return m_nDuration;};
    virtual void        UpdateStream(unsigned char* pBuf, int nSize){};
    virtual unsigned char* AllocBuffer(int nSize){return NULL;};
    virtual void        SetCompressBuffer(bool bCompress){m_bNeedCompressBuffer = bCompress;};

protected:
	long long			m_llStartPos;
	long long			m_llEndPos;
	void *				m_hUserData;
    bool                m_bStreamAvailable;
    char                m_szSourceURL[1024];
    int                 m_nDuration;
    bool                m_bNeedCompressBuffer;
};

class CHttpFileStream : public CHttpBaseStream
{
public:
	CHttpFileStream (void * hUserData);
	virtual ~CHttpFileStream (void);

	virtual bool		Open (void * pSource, int nFlag);
	virtual bool		Close (void);
	virtual int			Read (unsigned char * pBuff, int nSize);
	virtual bool		Seek (long long llPos, int nFlag);
	virtual long long	Size (void);

protected:
	VO_FILE_SOURCE		m_sSource;
	VO_TCHAR			m_szFileName[1024];
	VO_PTR				m_hFile;
};

class CHttpSinkStream : public CHttpBaseStream
{
public:
	CHttpSinkStream (void * hUserData);
	virtual ~CHttpSinkStream (void);

	virtual bool            Open (void * pSource, int nFlag);
	virtual bool            Close (void);
	virtual int             Read (unsigned char * pBuff, int nSize);
	virtual bool            Seek (long long llPos, int nFlag);
	virtual long long       Size (void);
    virtual void            UpdateStream(unsigned char* pBuf, int nSize);
    virtual unsigned char*  AllocBuffer(int nSize);
    virtual bool            IsEOS();
    virtual unsigned char*  GetBuffer();
protected:
    unsigned char*      m_pBuffer;
    long long           m_nSize;
    long long           m_nCurrPos;
    long long           m_nMemSize;
};
    
    // End define
#ifdef _VONAMESPACE
}
#endif

#endif // __CHttpStream_H__
