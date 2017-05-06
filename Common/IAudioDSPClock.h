#ifndef __IAUDIO_DSP_CLOCK_H
#define __IAUDIO_DSP_CLOCK_H
struct IAudioDSPClock
{
	virtual long getDSPTimeStamp() const = 0;
};
#endif // __IAUDIO_DSP_CLOCK_H


