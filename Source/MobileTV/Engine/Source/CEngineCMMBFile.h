#ifndef _CENGINECMMBFILE_H_
#define _CENGINECMMBFILE_H_

//#include "voMTVEngine.h"
#include "voLiveSource.h"
#include "CEngineCMMBBase.h"
#include "voThread.h"

class CEngineCMMBFile : public CEngineCMMBBase
{
public:
	CEngineCMMBFile(void);
	~CEngineCMMBFile(void);

	virtual int		OpenDevice(void) { return 0; }
	virtual int		CloseDevice(void) { Stop(); return CEngineCMMBBase::CloseDevice(); }

	virtual int		Scan (VO_LIVESRC_SCAN_PARAMEPTERS* pParam, bool nQuickScan = false);
	virtual int		SetChannel (int nNewChannelID);

	virtual int		DecryptData (int nCAT, void * pDataPtr) { return 0; }

	virtual int		Start (void);
	virtual int		Stop (void);

protected:

	virtual void	ReadData (void);
	virtual void	WriteData (void);

	VO_PBYTE		ParseCMMBBuffer (VO_PBYTE pBuffer, int & nBufSize);

	void			ReleaseData (void);

protected:

	VO_TCHAR			m_szFileFolder[MAX_PATH];
	int					m_nPacketSize;
	int					m_nBitrate;
	int					m_nFileType;

// 	int					m_nCurChannel;
// 	VO_LIVESRC_CHANNELINFO *	m_pChannelInfo;
	VO_TCHAR **			m_ppFilesName;

// 	int	*				m_pChannelESGNum;
// 	VO_LIVESRC_ESGINFO **		m_ppESGInfo;

protected:
	static VO_U32		PushDataEntry(VO_PTR pParam);
	VO_U32				PushData();
	void				BitrateControl(void);

protected:
	VO_HANDLE	m_hPushThread;
// 	bool		m_bStop;
	int			m_nStartTime;

	VO_PTR		m_hFile;
	int			m_nSyncOffset;
	bool		m_bMTPDumpFile;

	VO_PBYTE	m_pBuffer;
	int			m_nBufSize;
	int			m_nBufRest;
	VO_U32		m_dwCMMBID;

	int			m_nSent;
	int			m_nCMMBCount;

// 	bool		m_bPause;
	bool		m_bReading;

// 	bool		m_bCancelScan;
};
#endif // _CENGINECMMBFILE_H_
