#pragma once

#include "ConfigFile.h"
#include "voLiveSource.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CChannelInfo : public VO_LIVESRC_CHANNELINFO
{
public:
	CChannelInfo(void);
	~CChannelInfo(void);

	bool Load(CConfigFile& cfg);
	bool Save(CConfigFile& cfg);
};
#ifdef _VONAMESPACE
}
#endif