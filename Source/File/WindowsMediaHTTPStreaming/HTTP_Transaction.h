#ifndef __CHTTP_Transaction_H__
#define __CHTTP_Transaction_H__

#include "TCPClientSocket.h"
#include "Thread.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

enum HTTPMethod {
	HTTP_GET, HTTP_OPTIONS, HTTP_HEAD, HTTP_POST, HTTP_PUT, HTTP_DELETE, HTTP_TRACE, HTTP_CONNECT
};

struct RequestInfo
{
	HTTPMethod method;
	VO_CHAR *szURL;
	VO_CHAR *szExtendHeaders;	//Optional
	VO_CHAR *szBody;			//Optional
};


class CHTTP_Transaction : public CThread
{
public:
	class Delegate {
	public:
		~Delegate() {}
		virtual void Some(VO_CHAR *, VO_S32) = 0;
	};

	CHTTP_Transaction(CHTTP_Transaction::Delegate *);
	CHTTP_Transaction(void);
	~CHTTP_Transaction(void);

	virtual VO_S32 Start(RequestInfo *);//
	VO_S32 Reset();

	virtual VO_VOID ThreadMain();

	VO_S32 getResposeInfo(VO_PCHAR *);

	VO_S32 ReadData(VO_CHAR *, VO_S32);//

protected:
	VO_S32 doGetResposeInfo(VO_CHAR *, VO_S32);
	VO_S32 AnalyseRespose(VO_CHAR *);

	VO_S32 doReadNormal(VO_CHAR *, VO_S32);
	VO_S32 doReadChunked(VO_CHAR *, VO_S32);

	CTCPClientSocket* m_pTCPClient;

	VO_CHAR m_szURL[1024];
	VO_CHAR m_szHost[1024];
	VO_CHAR m_szPath[1024];
	VO_S32 m_iPort;

	VO_BOOL m_bKeepAlive;

	VO_BOOL m_bChunked;
	VO_S64 m_llSize; //Chunked or ContentLen

	CHTTP_Transaction::Delegate *m_pDelegate;
private:
	VO_CHAR* m_szRespose;

	static const VO_S32 CacheSize = 64 * 1024;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CHTTP_Transaction_H__