
#ifndef __VOCHANNELTIEMTHREADISS_H__

#define __VOCHANNELTIEMTHREADISS_H__

#include "voChannelItemThread.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
class voChannelItemThreadISS :
	public voChannelItemThread
{
public:
	voChannelItemThreadISS(void);
	virtual ~voChannelItemThreadISS(void);

protected:
	//VO_U32 CreateFileParser( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item , VO_PBYTE pBuffer , VO_U32 size );
	VO_U32 GetHeadData(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE Type);
protected:
	_TRACK_INFO*	pTrackInfo;

};
#ifdef _VONAMESPACE
}
#endif
#endif
