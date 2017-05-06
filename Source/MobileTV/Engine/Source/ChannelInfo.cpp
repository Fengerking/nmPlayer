#include "ChannelInfo.h"

CChannelInfo::CChannelInfo(void)
{
}

CChannelInfo::~CChannelInfo(void)
{
}

bool CChannelInfo::Load(CConfigFile& cfg)
{
	return cfg.ReadData(this, sizeof(CChannelInfo));
}

bool CChannelInfo::Save(CConfigFile& cfg)
{
	return cfg.WriteData(this, sizeof(CChannelInfo));
}
