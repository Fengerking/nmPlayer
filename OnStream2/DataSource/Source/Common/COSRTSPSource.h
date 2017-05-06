#ifndef __COSRTSPSource_H__
#define __COSRTSPSource_H__

#include "COSBaseSource.h"

class COSRTSPSource : public COSBaseSource
{
public:
	COSRTSPSource (VO_SOURCE2_LIB_FUNC *pLibop);
	virtual ~COSRTSPSource (void);

	virtual int				Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag);
	virtual int				Uninit(void);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);

	virtual int				doReadAudio(VOOSMP_BUFFERTYPE * pBuffer);
	virtual int				doReadVideo(VOOSMP_BUFFERTYPE * pBuffer);


	virtual int				HandleEvent (int nID, int nParam1, int nParam2);

	virtual int				GetMediaCount(int ssType);
	virtual int				SelectMediaIndex(int ssType, int nIndex);
	virtual int				IsMediaIndexAvailable(int ssType, int nIndex);
	virtual int				GetMediaProperty(int ssType, int nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty);
	virtual int				CommetSelection(int bSelect);
	virtual int				GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);

protected:
	virtual int				LoadDll();

	VO_SOURCE2_RTSP_INIT_PARAM	m_sInitParam;
};

#endif // __COSBaseSource_H__
