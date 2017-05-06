#pragma once

#include "ConfigFile.h"
//#include "voMTVEngine.h"
#include "voLiveSource.h"

class CChannelInfo : public VO_LIVESRC_CHANNELINFO
{
public:
	CChannelInfo(void);
	~CChannelInfo(void);

	bool Load(CConfigFile& cfg);
	bool Save(CConfigFile& cfg);
};
