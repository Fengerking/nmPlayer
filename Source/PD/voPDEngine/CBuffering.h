#pragma once
#include "voPDPort.h"
typedef enum _BUFFERINGTYPE
{
	BT_NONE					= 0,	/*!< no do buffering */
	BT_SEEK					= 1,	/*!< set position thread activate buffering */
	BT_PLAY					= 2,	/*!< play thread activate buffering */
} BUFFERINGTYPE;

class CNetGet2;
class CBuffering:MEM_MANAGER
{
public:
	CBuffering(CNetGet2* pNetGet);
	virtual ~CBuffering();

	bool			Start(int nStart, int nEnd, bool bForceSend, BUFFERINGTYPE btType, int nCurrFilePos);
	void			Stop(bool bSendBufferingEnd = false);
	int				GetPercent();
	bool			SetCurrentDownloadFilePos(int nFilePos);

	BUFFERINGTYPE	GetBufferingType();

protected:
	CNetGet2*		m_pNetGet;

	BUFFERINGTYPE	m_Type;
	int				m_nStart;
	int				m_nEnd;
	int				m_nPercent;	//0-100
	IVOMutex*		m_csLock;
};
