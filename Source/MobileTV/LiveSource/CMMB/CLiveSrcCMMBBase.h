#ifndef _ENGINECMMBBASE_H_
#define _ENGINECMMBBASE_H_

#include "voLiveSourceCMMB.h"
#include "CLiveSrcBase.h"
#include "voCMMBParser.h"
#include "CCASbase.h"

#include "voString.h"
#include "voCSemaphore.h"
#include "voLog.h"
#include "voString.h"

#pragma warning (disable : 4996)


#ifdef LINUX
#define MAX_PATH 260
#endif

#define	STREAM_TYPE_NULL		0
#define	STREAM_TYPE_TS0			1
#define	STREAM_TYPE_ESG			2
#define	STREAM_TYPE_SERVICE		3
#define STREAM_TYPE_EB			4

#define MAX_CHANNEL_COUNT	100
#define MAX_ESG_ENTRY_NUM	64000

#define FREQSTART_DEFAULT	13
#define FREQEND_DEFAULT		48

#define	MAKE_CHANNELID(freqIndex, serviceID)	(freqIndex << 16 | serviceID)
#define GET_FREQINDEX(channelID)				(channelID >> 16)
#define GET_SERVICEID(channelID)				(channelID & 0x0000FFFF)

#define MAX_CAID_LEN		32
#define MAX_KDAVERSION_LEN	MAX_PATH

#define DECRYPT_BUFFER_MAXSIZE	10000
#define CAS_BASE


#ifdef CAS_BASE
#include "CCASbase.h"
#define CAS_CLASS	CCASBase
#define CAS_TYPE	0
#endif

/*
typedef struct  
{
	VO_U16			service_id;
	VO_U16			service_param_id;
	VO_TCHAR		service_name[128];
	VOSERVICECLASS	service_class;
	VO_BOOL			service_free;
	VOCAINFO		ca_info;
}VOSERVICEINFO;
*/

class CLiveSrcCMMBBase : public CLiveSrcBase
{
public:
	CLiveSrcCMMBBase(void);
	virtual ~CLiveSrcCMMBBase(void);

	virtual VO_U32	Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);
	virtual	VO_U32	Close();
	virtual VO_U32	Scan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam);
	virtual VO_U32	Cancel(VO_U32 nID);
	virtual VO_U32	GetChannel(VO_U32 * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo);
	virtual VO_U32	SetChannel(VO_S32 nChannelID);
	virtual VO_U32	GetESG(VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** pESGInfo);

protected:
	virtual	VO_U32 doOpenDevice();
	virtual VO_U32 doCloseDevice();
	virtual VO_U32 doStartRecieve();
	virtual VO_U32 doStopRecieve();
	virtual VO_U32 doInitParser();
	virtual VO_U32 doUninitParser();
protected:
	VO_U32	EBQuery();


protected:
	virtual VO_BOOL	doOnParsedData(VO_PARSER_OUTPUT_BUFFER * pData);

protected:
	int				ReadLine(VO_PBYTE pData, int nDataLen, VO_PTR hFile);

	void			ReleaseData (void);
	void			InitData(void);

	virtual void	ReadData (void);
	virtual void	WriteData (void);
	virtual void	WriteCurChannel(void);
	virtual void	WriteExtraData (VO_HANDLE hFile);

	int				FindServiceIndex(int nChannelID);
	int				AddNewService(VOCONTINUESERVICEINFO * pContService);

	int				FindServiceInfoIndex(int nChannelID);
	int				AddNewServiceInfo(VOSERVICEINFO * pServiceInfo);

	int				AddNewSchedule(VOSCHEDULEINFO * pSchedule);
	int				UpdateSchedule(void);

	int				FindChannelIndex(int nChannelID);
	int				UpdateChannel(void);

	VO_U32			GetFrequencyValue(int id);
	
	virtual int		PreScan (void);
	virtual int		PostScan (void);
	virtual int		DeviceScan (void);
	virtual int		ReceiveESGInfo(void);

	virtual int		SetDeviceChannel (int nNewChannelID);
	virtual int		SetTS0Channel (void) { return VO_ERR_LIVESRC_OK; }

	int				DecryptData (int nCAT, void * pDataPtr);

	//void			SendData(int packetType, unsigned char * pData, int nDataLen, VO_U64 nTimeStamp, int nCodecType, int nStreamId, bool bKeyFrame);


protected:
	CCASBase*				m_pCAS;
	int						m_nContServiceCount;
	VOCONTINUESERVICEINFO*	m_pContService;

	int						m_nServiceInfoCount;
	VOSERVICEINFO*			m_pServiceInfo;

	VO_TCHAR				m_szAppPath[MAX_PATH];	
	VO_TCHAR				m_szChannelFilePath[MAX_PATH];
	VO_TCHAR				m_szCAID[MAX_CAID_LEN];
	VO_TCHAR				m_szKDAversion[MAX_KDAVERSION_LEN];

	unsigned char		m_CATData[256];
	unsigned int		m_nCATSize;

	VO_TCHAR				m_szDebugString[256]; // For debug output

	int					m_nFreqNum;
	int					m_nFreqStart;
	int					m_nFreqEnd;
	int					m_nCurStreamType;

	bool				m_bCancelScan;
	bool				m_bStop;
	bool				m_bServiceSelected;
	bool				m_bWaitForGetServiceDone;
	bool				m_bPause;
	bool				m_bPauseConfirmed;

	int					m_nLastChannel;	// Selected channel when program exited last time.
	int					m_nCurChannel;
// 	int					m_nChannelNum;
	VO_LIVESRC_CHANNELINFO *	m_pChannelInfo;

	int					m_nChannelCount;

	voCSemaphore		m_hESGParsed;
	int	*				m_pChannelESGNum;
	VO_CMMB_ESG_ITEM **		m_ppESGInfo;
	VO_CMMB_ESG_ITEM **		m_pESGTemp;
	int					m_nESGEntryNum;
	bool				m_fESGReceived;

	int					m_nCurNetworkID;
	int					m_nEMMService;

	bool				m_bScanning;

	VO_PBYTE			m_pDecryptBuffer;
	unsigned int		m_nDecryptBufferSize;

	VO_CMMB_ESGINFO		m_CmmbEsgInfo;
	VO_LIVESRC_ESG_INFO	m_TotalEsgInfo;

	VO_SOURCE_SAMPLE	m_AVSample;
	VO_LIVESRC_SAMPLE	m_LiveSrcSample;

	VO_BYTE*			m_pAudioBuffer;

	// To be confirmed : Telepath SDK needs to get service info before select any service.
	bool				m_bGotServiceInfo;

public:
	int				GetCAID (VO_TCHAR * szCAID) { vostrcpy(szCAID, m_szCAID); return VO_ERR_LIVESRC_OK; }
	int				GetKDAVersion (VO_TCHAR * szKDA) { vostrcpy(szKDA, m_szKDAversion); return VO_ERR_LIVESRC_OK; }
};

#endif //_ENGINECMMBBASE_H_
