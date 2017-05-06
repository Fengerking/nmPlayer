
#ifndef __VOCLIVEDATASOURCE_H__

#define __VOCLIVEDATASOURCE_H__

#include "voCDataSource.h"
#include "CBaseLiveSource.h"

class voCLiveDataSource :
	public voCDataSource
{
public:
	voCLiveDataSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB);
	~voCLiveDataSource(void);

	virtual VO_U32			LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32			CloseSource (void);

	virtual VO_U32			Start(void);
	virtual VO_U32			Pause(void);
	virtual VO_U32			Stop(void);

	virtual VO_U32			SetTrackPos (VO_U32 nTrack, VO_S64 * pPos);
	virtual VO_VOID			NotifyStatus ( VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2 );
	virtual VO_VOID			NotifyData ( VO_U16 nOutputType, VO_PTR pData );

    virtual void		SetLibOperator (VO_LIB_OPERATOR * pLibOP);

protected:
	virtual VO_LIVESRC_FORMATTYPE GetLiveType( VO_U32 nType );

protected:
	CBaseLiveSource*		m_pLiveSource;
	VO_CHAR					m_url[1024];

	VO_BOOL					m_should_stop;
	VO_BOOL					m_is_started;
	VO_U64					m_nLiveSourceDuration;
};


#endif
