#include "adtsparser.h"

#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

bool CADTSParser::FindHead(void* data, int len, void** ppResult)
{
	uint8* p = (uint8*)data;
	for (; len--; p++)
	{
		if (*p == 0xff)
		{
			uint8 t = *(p+1);
			if (((t & 0xf0) == 0xf0) && (t != 0xff))
			{
				// do more check
				CADTSParser parser;
				if (!parser.Parse(p))
					continue;
				if (parser.GetFrameLen() > 1024)
					continue;
				if (parser.GetSampleRateIndex() > 11)
					continue;
				//if (parser.GetChannelCount() > 2)
				//	continue;

				//if (t != 0xf9)
				//	t = t ; //test;
				*ppResult = p;
				return true;
			}
		}
	}
	return false;
}

bool CADTSParser::FindHead(void* data, int len, void** ppResult, uint8 f1, uint8 f2, uint8 f3)
{
	uint8* p = (uint8*)data;
	while (len--)
	{
		if (*p == 0xff)
		{
			if ( (*(p+1) == f1) && (*(p+2) == f2) )
			{
				*ppResult = p;
				return true;
			}
		}
		++p;
	}
	return false;
}

//====================================================================

bool CADTSParser::Parse(uint8* data)
{
	uint8* p = _head = data;
	
	// is a valid head?
	if (IsNotSyncWord(p))
		return false;

	// check protection_absent
	++p;

	// check the ID and Layer
//	if (*p != 0XF8 && *p != 0XF9)
//		return false;

	// check the profile 
	if (((*(p+1)) & 0xC0) != 0X80 && ((*(p+1)) & 0XC0) != 0X40)
		return false;

	uint8 protection_absent = *p & 1;

	p += 2;
	_frame_len = *p & 0x03;
	_frame_len <<= 8;
	_frame_len |= *++p;
	_frame_len <<= 3;
	_frame_len |= *++p >> 5;

	_frame_data = p + 2;

	// CRC check
	if (protection_absent == 0)
		_frame_data += 2;

	_data_len = _frame_len - (_frame_data - data);

	if (_frame_len > 1024)
		return false;

	if (GetSampleRateIndex() > 11)
		return false;

	return true;
}

bool CADTSParser::IsMPEG2() const
{
	return (bool)(*(_head + 1) & 0x08);
}

/**
profile 		
bits 	ID 1 (MPEG-2 profile)                  ID 0 (MPEG-4 Object type)
00 (0) 	Main profile                           AAC MAIN
01 (1) 	Low Complexity profile (LC)            AAC LC
10 (2) 	Scalable Sample Rate profile (SSR)     AAC SSR
11 (3) 	(reserved)                             AAC LTP
*/
uint8 CADTSParser::GetProfile() const
{
	return *(_head + 2) >> 6;
}

uint8 CADTSParser::GetSampleRateIndex() const
{
	return (*(_head + 2) >> 2) & 0x0f;
}

uint8 CADTSParser::GetChannelConfig() const
{
	return ( (*(_head + 2) << 2) | (*(_head + 3) >> 6) ) & 0x07;
}

uint8 CADTSParser::GetAACObjectType() const
{
	return GetProfile() + 1;
}

uint32 CADTSParser::GetSampleRate() const
{
	const uint32 sample_rates[] =
    {
        96000, 88200, 64000, 48000, 
		44100, 32000,24000, 22050, 
		16000, 12000, 11025, 8000,
		0, 0, 0, 0
    };
	uint8 i = GetSampleRateIndex();
	return sample_rates[i];
}
	const uint16 channel_counts[] =
    {
		2, 1, 2, 3, 4, 5, 6, 7
    };

uint16 CADTSParser::GetChannelCount() const
{
	uint8 i = GetChannelConfig();
	return channel_counts[i];
}
