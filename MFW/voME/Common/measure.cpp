#include "measure.h"
#include "voLog.h"
#undef LOG_TAG
#define LOG_TAG "VOUTIL"

UnitRator::UnitRator(int p, int s)
: period(p)
, step(s)
{
    Start();
}

void UnitRator::Start()
{
	head = tail = 0;
	Item(head) = voOMXOS_GetSysTime();
    //VOLOGI("at %lu", Item(head));
}

void UnitRator::AddFrame()
{
	++tail;
	if ((tail % step) != 0)
		return;
	int now = Item(tail) = voOMXOS_GetSysTime();
	while (now - Item(head) > (OMX_U32)period)
		head += step;
	//DebugInfo();
}

void UnitRator::DebugInfo()
{
	VOLOGI("(%lu - %lu) / %lu => %.3ffps", tail, head, Item(tail) - Item(head), GetFrameRate());
}

bool UnitRator::ShouldOutput(int interval)
{
	OMX_U32 now = Item(tail);
	OMX_U32 prev = Item(tail - step);
	OMX_U32 check = now / interval * interval;
	return (now >= check) && (prev < check);
}


FrameRator::FrameRator(const char* t, int maxfr)
: title(t)
, sta(FR_ST_PERIOD, 1)
, lta(FR_LT_PERIOD, maxfr * FR_LT_PERIOD / (FR_SAMPLES * 1000))
{
}

void FrameRator::Start()
{
	sta.Start();
	lta.Start();
}

void FrameRator::AddFrame()
{
	sta.AddFrame();
	lta.AddFrame();
	if (sta.ShouldOutput(FR_OUTPUT_INTERVAL))
	{
		VOLOGI("%s frame rate: %.2f/%.2f", title, sta.GetFrameRate(), lta.GetFrameRate());
        //sta.DebugInfo();
        //lta.DebugInfo();
	}
}
