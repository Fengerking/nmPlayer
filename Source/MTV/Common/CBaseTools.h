#pragma once
#include "voType.h"

class CBaseTools
{
private:
	CBaseTools(void);
	~CBaseTools(void);

public:
	static VO_BOOL GetHeadData(VO_U32 inCodecType, VO_BYTE* inBuf, VO_U32 inLen, VO_BOOL& outVideo, VO_BYTE** outHeadData, VO_U32& outHeadDataLen);
	static VO_BOOL IsKeyFrame(VO_U32 inCodecType, VO_BYTE* inBuf, VO_U32 inLen);
};
