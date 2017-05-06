#include "adtshead.h"

#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const uint8 NUM_SAMPLE_RATES = 0xc;
bool ADTSHeader::FindHead(void* data, int len, void** ppResult,bool beStrict)
{
	uint8* p = (uint8*)data;
	while (len--)
	{
		if (*p == 0xff)
		{
			uint8 t = *(p+1);
			if (((t & 0xf0) == 0xf0) && (t != 0xff))
			{
				*ppResult = p;
				if ((beStrict == true) && (len>= 9))
				{
					uint8 sampleIndex = (p[2] >> 2) &0xF;
					uint8 channel_configuration = ((p[2]&0x1)<<2) | ((p[3]>>6)&0x3);
					if((sampleIndex > NUM_SAMPLE_RATES) ||(channel_configuration>0?(channel_configuration>0x7):0))
					{
						continue;
					}
					return true;
				}
				else
					return true;
			}
		}
		++p;
	}
	return false;
}

bool ADTSHeader::FindHead(void* data, int len, void** ppResult, uint8 f1, uint8 f2, uint8 f3)
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

bool ADTSHeader::Parse(uint8* data)
{
	uint8* p = _head = data;
	
	// is a valid head?
	if (IsNotSyncWord(p))
		return false;

	// check protection_absent
	++p;
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
	return true;
}

bool ADTSHeader::IsMPEG2() const
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
uint8 ADTSHeader::GetProfile() const
{
	return *(_head + 2) >> 6;
}

uint8 ADTSHeader::GetSampleRateIndex() const
{
	return (*(_head + 2) >> 2) & 0x0f;
}

uint8 ADTSHeader::GetChannelConfig() const
{
	return ( (*(_head + 2) << 2) | (*(_head + 3) >> 6) ) & 0x07;
}

uint8 ADTSHeader::GetAACObjectType() const
{
	return GetProfile() + 1;
}

uint32 ADTSHeader::GetSampleRate() const
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

uint16 ADTSHeader::GetChannelCount() const
{
	uint8 i = GetChannelConfig();
	return channel_counts[i];
}



#ifdef _HEAD_DECODE

bool ADTSHeader::Decode(uint8* data, uint32 size)
{
	return Parse(data);
}

#endif //_HEAD_DECODE


#ifdef _HEAD_MTYPE

#include "voiuuids.h"

HRESULT ADTSHeader::BuildMediaType(CMediaType* pmt)
{
	pmt->ResetFormatBuffer ();

    pmt->majortype = MEDIATYPE_Audio;
    pmt->subtype = MEDIASUBTYPE_ADTSAAC;
    pmt->SetFormatType(&FORMAT_WaveFormatEx);
	pmt->SetTemporalCompression(FALSE);

	WORD ch = GetChannelCount();
	int sri = GetSampleRateIndex();
	int sr = GetSampleRate();
	int ot = GetAACObjectType();

	WORD cExtra = 2;
	int cSize = sizeof (WAVEFORMATEX) + cExtra;
	WAVEFORMATEX *audioInfo =
        (WAVEFORMATEX*)pmt->AllocFormatBuffer(cSize);
    if (audioInfo == NULL) 
	{
        return E_OUTOFMEMORY;
    }
	memset(audioInfo, 0, cSize);

	audioInfo->wFormatTag = WAVE_FORMAT_ADTSAAC;
	audioInfo->nChannels = ch;
	audioInfo->nSamplesPerSec = sr;
	audioInfo->nBlockAlign = 4;
	audioInfo->nAvgBytesPerSec = audioInfo->nSamplesPerSec * audioInfo->nBlockAlign;
	audioInfo->wBitsPerSample = 16;
	audioInfo->cbSize = cExtra;

	BYTE* p = (BYTE*) (audioInfo + 1);
	*p++ = (BYTE) ((ot << 3) | (sri >> 1));
	*p = (BYTE) ((sri << 7) | (ch << 3));

    return S_OK;
}

#endif //_HEAD_MTYPE


void ADTSHeader::ToDSI(uint8 dsi[2]) const
{
	int ch = GetChannelCount();
	int sri = GetSampleRateIndex();
	int ot = GetAACObjectType();
	dsi[0] = (uint8) ((ot << 3) | (sri >> 1));
	dsi[1] = (uint8) ((sri << 7) | (ch << 3));
}