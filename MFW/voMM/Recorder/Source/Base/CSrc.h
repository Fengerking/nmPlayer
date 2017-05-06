#ifndef _CSRC_H
#define _CSRC_H

// #include "recvideosrc.h"
// #include "recaudiosrc.h"
#include "voMMRecord.h"

#define DROP_SRC

class CSourceSink;

class CSrc
{
public:
	static void  AudioSrcCallback(VO_MMR_ASBUFFER* pBuffer);
	static void  PreviewVideoSrcCallback(VO_MMR_VSBUFFER* pBuffer);
	static void  RecordVideoSrcCallback(VO_MMR_VSBUFFER* pBuffer);

public:
	CSrc(void);
public:
	virtual ~CSrc(void);

public:
	void SetSrcSink(CSourceSink* pSink){m_pSrcSink = pSink;};
	static void SetSendStatus(bool bClose);
	static void SetAutoDrop(bool bDrop);
	static void Pause(bool bPause);
	static void SetHasPreview(bool bHas);

protected:
	static bool m_bPause;
	static bool m_bClose;
	static bool m_bAutoDrop;
	static CSourceSink* m_pSrcSink;

#ifdef DROP_SRC
	static unsigned long m_dwLastFrmTime;
	static bool m_bNeedDrop;
	static bool m_bHasPrevieweData;
#endif
};
#endif // _CSRC_H

