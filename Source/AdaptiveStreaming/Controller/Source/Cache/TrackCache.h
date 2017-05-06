#ifndef __TRACKCACHE_H__
#define __TRACKCACHE_H__

#include <list>
#include "shared_ptr.hpp"
#include "voProgramInfo.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class TrackCache
{
public:
	explicit TrackCache(VO_U32 uMaxDuration);
	virtual ~TrackCache(void);

	VO_U32	PutSample(const shared_ptr<_SAMPLE> pSample);
	VO_U32	GetSample(shared_ptr<_SAMPLE>* pSample);
	VO_U32	GetSamples(std::list< shared_ptr<_SAMPLE> > **plist);

	//VO_U32	Duration() /*const*/ { return m_uCurDuration; };

	VO_U32	ReCapacity(VO_U32 uMaxDuration) { m_uMaxDuration = uMaxDuration; return VO_RET_SOURCE2_OK; }

	VO_U32	SetPos(std::list< shared_ptr<_SAMPLE> >::iterator iterP);

	VO_U32	Flush();

private:

	std::list< shared_ptr<_SAMPLE> >	m_listSamples;

	VO_U32		m_uMaxDuration;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__TRACKCACHE_H__