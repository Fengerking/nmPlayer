#ifndef _UTIL_MEASURE_H_
#define _UTIL_MEASURE_H_

#include "voOMXOSFun.h"

const int FR_SAMPLES = 128;
const int FR_ST_PERIOD = 1000; //ms, default short term period for measure
const int FR_LT_PERIOD = 30000; //ms, default long term period for measure
const int FR_OUTPUT_INTERVAL = 1000; //ms, default output interval for frame rate
const int FR_MAX_VIDEO_FRAME_RATE = 40; //fps

// move average of frame rate
class UnitRator
{
public:
	UnitRator(int period, int step=1);

	void Start();
	void AddFrame();

	OMX_U32 GetFrameCount() { return tail; }
	double GetFrameRate() { return (tail - head < (OMX_U32)step) ? 0 : (double)(tail - head) * 1000.0 / ( Item(tail) - Item(head) ); }

	bool ShouldOutput(int interval=FR_OUTPUT_INTERVAL);
	void DebugInfo();

protected:
	inline OMX_U32& Item(OMX_U32 index) { return time[(index / step) % FR_SAMPLES]; }

private:
	OMX_U32 head;
	OMX_U32 tail;
	OMX_U32 time[FR_SAMPLES];
	int period;
	OMX_U32 step;
};

class FrameRator
{
public:
	FrameRator(const char* title, int maxfrate); 
	void Start();
	void AddFrame();

private:
	const char* title;
	UnitRator sta; //short term average
	UnitRator lta; //long term average
};


#endif //_UTIL_MEASURE_H_