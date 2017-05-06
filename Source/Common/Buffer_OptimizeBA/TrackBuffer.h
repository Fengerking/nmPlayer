#ifndef __TRACKBUFFER_H__
#define __TRACKBUFFER_H__

#include <list>
#include "shared_ptr.hpp"
#include "voProgramInfo.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class TrackBuffer
{
public:
	explicit TrackBuffer(VO_U32 uMaxDuration);
	virtual ~TrackBuffer(void);

	virtual VO_U32	PutSample(const shared_ptr<_SAMPLE> pSample);
	VO_U32			PutSamples(std::list< shared_ptr<_SAMPLE> >* plist, VO_BOOL bForceInput);
	virtual VO_U32	GetSample(_SAMPLE* pSample);

	virtual VO_U32	Duration() /*const*/ { return m_uCurDuration; };

	VO_U32			ReCapacity(VO_U32 uMaxDuration) { m_uMaxDuration = uMaxDuration; return VO_RET_SOURCE2_OK; }
	//VO_U32		Capacity() const;

	VO_U32			Flush();

	///**
	//* @return	VO_RET_SOURCE2_FAIL			前端数据不足; nohting
	//*			VO_RET_SOURCE2_NEEDRETRY	尾端数据不足; nohting
	//*/
	virtual VO_U32	SetPos(VO_U64* pullTimePoint);

	const shared_ptr<_SAMPLE>	Peek() const;

	VO_BOOL			WillAvailable() const { return m_bWillUsable; };

protected:
	VO_U32 _SetPos(std::list< shared_ptr<_SAMPLE> >::iterator iterP);

	std::list< shared_ptr<_SAMPLE> >	m_listSamples;

	voCMutex	m_lock;

private:
	virtual VO_U32	CalculateDuration();

	VO_U32		m_uMaxDuration;
	VO_U32		m_uCurDuration;

	VO_BOOL		m_bWillUsable;
	
	shared_ptr<_SAMPLE>	m_pCacheSample;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__TRACKBUFFER_H__