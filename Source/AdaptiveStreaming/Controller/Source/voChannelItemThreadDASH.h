
#ifndef __VOCHANNELTIEMTHREADDASH_H__

#define __VOCHANNELTIEMTHREADDASH_H__

#include "voAdaptiveStreamingFileParser.h"
#include "voChannelItemThread.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voChannelItemThreadDASH :
	public voChannelItemThread
{
public:
	voChannelItemThreadDASH(void);
	virtual ~voChannelItemThreadDASH(void);

	virtual VO_VOID Stop();

protected:
	/* .........ToDealItem_II instead of
	virtual VO_U32 ToDealItem(VO_ADAPTIVESTREAMPARSER_CHUNK *pChunk, VO_BOOL *pReset);
	..........*/
	//virtual void thread_function();
	VO_U32 GetHeadData(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE Type);
	VO_U32 ToProcessChunk(VO_HANDLE hIO, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 *nReadsize);

	
};


#ifdef _VONAMESPACE
}
#endif
#endif
