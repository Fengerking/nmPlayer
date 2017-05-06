
#ifndef __VOCHANNELTIEMTHREADHLS_H__

#define __VOCHANNELTIEMTHREADHLS_H__

#include "voChannelItemThread.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
class voChannelItemThreadHLS :
	public voChannelItemThread
{
public:
	voChannelItemThreadHLS(void);
	virtual ~voChannelItemThreadHLS(void);

protected:
	virtual VO_U32 ToGetItem(VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item, VO_U32 nErrCode);

	virtual VO_U32 ToProcessChunk(VO_HANDLE hIO, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 *nReadsize);
	virtual VO_U32 SendCustomEvent(VO_U16 nOutputType, VO_PTR pData) ;
	virtual VO_U32 CheckCustomTag(VO_U64 nTimestamp);
	virtual VO_U32 ResetCustomerTagMap();

	virtual VO_U32 GetHeadData( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE  Type ); 
	virtual VO_VOID DoDrop(VO_U32 &nFlag);
protected:
	VO_U64	m_uCustomerTagChunkID;
	VO_U64* m_pCustomerTagMap;
	VO_U32	m_nCustomerTagMapSize;
	VO_U32   m_nCustomerTagCount;
};
#ifdef _VONAMESPACE
}
#endif
#endif
