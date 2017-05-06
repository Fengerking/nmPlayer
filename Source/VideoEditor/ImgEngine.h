
#ifndef __ImgEngine_H__
#define __ImgEngine_H__

#ifdef _WIN32
	#include <windows.h>
	#include <tchar.h>
#endif

#include "CBaseNode.h"
#include "voImage.h"
#include "voJPEG.h"
#include "voPng.h"
#include "voString.h"
#include "videoEditorType.h"
#include "SampleTransformer.h"

/*VO_S32 VO_API voGetJpegDecAPI (VO_IMAGE_DECAPI * pDecHandle);*/
typedef VO_S32 (VO_API * VOGETIMGDECAPI) (VO_IMAGE_DECAPI * pDecHandle);

class ImgEngine : public CBaseNode
{
public:
	// Used to control the image drawing
	ImgEngine (VO_PTR hInst,VO_MEM_OPERATOR * pMemOP);
	virtual ~ImgEngine (void);

	virtual VO_U32		Init ();
	virtual VO_U32		Uninit (void);

	virtual int 		SetDataSource (void * pSource, int nFlag);
	virtual void	SetLibOperator (VO_LIB_OPERATOR * pLibOP) {m_pLibOP = pLibOP;CDllLoad::SetLibOperator(m_pLibOP);}

	virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	VO_VOID RelaeseBuffer();
	VO_U32	Process();
	int			LoadDll (void);
	VO_U32 GetJPEGSample(VO_VIDEO_BUFFER *pImgBuffer);
	VO_U32 GetPNGSample(VO_VIDEO_BUFFER *pImgBuffer);

	VO_IMAGE_DECAPI	m_ImgDecApi;
	VO_IMAGE_CODINGTYPE m_nFormat;
	VO_TCHAR       m_cTitle[255];
	VO_HANDLE	m_phCodec;
	VOMP_VIDEO_FORMAT m_srcFormat;

	VO_VIDEO_BUFFER ImgBuffer;
	VOMP_BUFFERTYPE m_sourceImgBuffer;
	VOMP_BUFFERTYPE m_curImgBuffer;
	VOMP_BUFFERTYPE m_tmpImgBuffer;
	VOMP_VIDEO_FORMAT m_outFormat;
	
};

#endif // __vompCEngine_H__