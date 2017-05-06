//-----------------------------------------------------------------------------
//
//	Monogram Multimedia AAC Decoder
//
//	Author : Igor Janos
//
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LatmParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


const int AAC_Sample_Rates[] = {
		96000, 88200, 64000, 48000,
		44100, 32000, 24000, 22050, 
		16000, 12000, 11025, 8000,
		7350, 0, 0, 0
};



//-----------------------------------------------------------------------------
//
//	CLATMReader class
//
//-----------------------------------------------------------------------------

CLATMParser::CLATMParser()
{
	// zero internals
	memset(frameLengthTypes, 0, sizeof(frameLengthTypes));
	memset(latmBufferFullness, 0, sizeof(latmBufferFullness));
	memset(frameLength, 0, sizeof(frameLength));
	memset(muxSlotLengthBytes, 0, sizeof(muxSlotLengthBytes));
	memset(muxSlotLengthBytes, 0, sizeof(muxSlotLengthBytes));
	memset(_numLayer, 0, sizeof(_numLayer));
	memset(progCIndx, 0, sizeof(progCIndx));
	memset(layCIndx, 0, sizeof(layCIndx));
	memset(AuEndFlag, 0, sizeof(AuEndFlag));
	numSubFrames = -1;
	lastError = -1; //not initialized
}

CLATMParser::~CLATMParser()
{
}


int	CLATMParser::Parse(uint8 *buf, int size)
{
	muxlength = 0;
	
	_bs.Init(buf);

	bit11 sync;
	_bs.ReadBits(11, sync);
	if (sync != 0x2b7) 
		return -1;		// just support AudioSyncStream() now

	_bs.ReadBits(13, muxlength);

#if 0
	if (3 + muxlength > size) 
		return -2;			// not enough data
#endif

	lastError = ReadMUXConfig();
	return lastError;
}

int CLATMParser::ReadMUXConfig()
{
	bit1 use_same_mux;
	_bs.ReadBits(1, use_same_mux);
	if (use_same_mux && (lastError == 0))
		return 0;  //treat it as OK

	streamCnt = 0;
	numSubFrames = 0;
	
	audio_mux_version_A = 0;
	_bs.ReadBits(1, audio_mux_version);
	if (audio_mux_version == 1) // audioMuxVersion
	{				
		_bs.ReadBits(1, audio_mux_version_A);
	}
	
	if (audio_mux_version_A == 1) 
		return -16;

	if (audio_mux_version == 1) 			
		taraFullness = LatmGetValue();
	_bs.ReadBits(1, all_same_framing);
	_bs.ReadBits(6, numSubFrames);
	_bs.ReadBits(4, numProgram);
	if(numProgram > 0) 
	{
		numSubFrames = -1;
		return -11;
	}
	for (int prog=0; prog<=numProgram; prog++) 
	{
		bit3 numLayer;
		_bs.ReadBits(3, numLayer);
		_numLayer[prog] = numLayer;

		if(numLayer > 0) 
		{
			numSubFrames = -1;
			return -12;
		}
		
		for (int lay=0; lay<=numLayer; lay++) {
			streamId[prog][lay]=streamCnt;
			streamCnt++;
			
			uint8 use_same_config;
			if (prog == 0 && lay == 0) 
			{
				use_same_config = 0;
			} 
			else 
			{
				_bs.ReadBits(1, use_same_config);
				
				if(use_same_config)
					return -13;
			}
			
			if (!use_same_config) 
			{
				bit32 init;
				if (audio_mux_version == 0) 
				{
					// audio specific config.
					_bs.ReadBits(32, init);
					_bs.SkipBits(-32); //???
					_audiocfg[0] = (uint8)((init&0xff000000) >> 24);
					_audiocfg[1] = (uint8)((init&0x00ff0000) >> 16);
					_audiocfg[2] = (uint8)((init&0x0000ff00) >> 8);
					_audiocfg[3] = (uint8)(init&0x000000ff);
					if (ReadAudioSpecConfig(lay) < 0)
						return -14;
				} 
				else 
				{
					int ascLen = LatmGetValue();
					_bs.ReadBits(32, init);
					_bs.SkipBits(-32); //???
					_audiocfg[0] = (uint8)((init&0xff000000) >> 24);
					_audiocfg[1] = (uint8)((init&0x00ff0000) >> 16);
					_audiocfg[2] = (uint8)((init&0x0000ff00) >> 8);
					_audiocfg[3] = (uint8)(init&0x000000ff);

					int conlen = ReadAudioSpecConfig(lay);
					if(conlen < 0) return -15;

					ascLen -= conlen;							
					// fill bits
					_bs.SkipBits(ascLen);
				}
			} 
			// these are not needed... perhaps
			bit3 frame_length_type;
			_bs.ReadBits(3, frame_length_type);
			frameLengthTypes[streamId[prog][lay]] = frame_length_type;
			if (frame_length_type == 0) 
			{
				_bs.ReadBits(8, latmBufferFullness[streamId[prog][lay]]);
				if (!all_same_framing) 
				{
					if ((objTypes[lay] == 6 ||
						objTypes[lay] == 20) &&
						(objTypes[lay-1] == 8 ||
						objTypes[lay-1] == 24)) 
					{
						bit6 core_frame_offset;
						_bs.ReadBits(6, core_frame_offset);
					}
				}
			} 
			else
				if (frame_length_type == 1) 
				{
					_bs.ReadBits(9, frameLength[streamId[prog][lay]]);
				} 
				else
					if (frame_length_type == 3 ||
						frame_length_type == 4 ||
						frame_length_type == 5) 
					{
						bit6 celp_table_index;
						_bs.ReadBits(6, celp_table_index);
					} 
					else
						if (frame_length_type == 6 ||
							frame_length_type == 7) 
						{
							bit1 hvxc_table_index;
							_bs.ReadBits(1, hvxc_table_index);
						}
						
		}
	}
	
	// other data
	other_data_bits = 0;
	bit1 b1;
	_bs.ReadBits(1, b1);
	if (b1) 
	{
		// other data present
		if (audio_mux_version == 1) 
		{
			other_data_bits = LatmGetValue();
		} 
		else 
		{
			// other data not present
			other_data_bits = 0;
			bit1 esc;
			bit8 tmp;
			do 
			{
				other_data_bits <<= 8;
				_bs.ReadBits(1, esc);
				_bs.ReadBits(8, tmp);
				other_data_bits |= tmp;
			} 
			while (esc);
		}
	}

	bit1 CRC;
	_bs.ReadBits(1,  CRC);
	if (CRC) 
	{
		_bs.ReadBits(8, config_crc);
	}

	return 0;
}

int	CLATMParser::GASpecificConfig(int layer)
{
	int ret=0;

	bit1 framelen_flag;
	bit1 dependsOnCoder;
	bit1 ext_flag;
	bit14 delay;
	bit3 layerNr;
	_bs.ReadBits(1, framelen_flag);
	ret += 1;
	_bs.ReadBits(1, dependsOnCoder);
	ret += 1;

	if (dependsOnCoder) 
	{
		_bs.ReadBits(14, delay);
		ret += 14;
	}

	_bs.ReadBits(1, ext_flag);
	ret += 1;

	if (objTypes[layer] == 6 || objTypes[layer] == 20) 
	{
		_bs.ReadBits(3, layerNr);
		ret += 3;
	}

	if (ext_flag) 
	{
		if (objTypes[layer] == 22) 
		{
			//BitStreamGetBits(&_bs, 5);				ret += 5;	// numOfSubFrame
			//BitStreamGetBits(&_bs, 11);				ret += 11;	// layer_length
			_bs.SkipBits(16);
			ret += 16;
		}
		if (objTypes[layer] == 17 ||
			objTypes[layer] == 19 ||
			objTypes[layer] == 20 ||
			objTypes[layer] == 23) 
		{
			//BitStreamGetBits(&_bs, 3);				ret += 3;	// stuff
			_bs.SkipBits(3);
			ret += 3;
		}

		//BitStreamGetBit(&_bs);						ret += 1;	// extflag3
		_bs.SkipBits(1);
		ret += 3;
	}
	return ret;
}

int	CLATMParser::ReadAudioSpecConfig(int layer)
{
	int ret = 0;
	int sbr_present = -1;
	
	_bs.ReadBits(5, objTypes[layer]);
	ret += 5;
	if (objTypes[layer] == 31) 
	{
		bit6 n;
		_bs.ReadBits(6, n);
		objTypes[layer] = 32 + n;						
		ret += 6;
	}

	bit4 samplingFrequencyIndex;
	_bs.ReadBits(4, samplingFrequencyIndex);
	ret += 4;
	_samplerate = AAC_Sample_Rates[samplingFrequencyIndex];
	if (samplingFrequencyIndex == 0x0f) 
	{
		_bs.ReadBits(24, _samplerate);
		ret += 24;	
	}

	_bs.ReadBits(4, _channel);
	ret += 4;

	if (objTypes[layer] == 5) 
	{
		sbr_present = 1;
		_bs.ReadBits(4, samplingFrequencyIndex);
		ret += 4;
		if (samplingFrequencyIndex == 0x0f) 
		{
			_bs.ReadBits(24, _samplerate);
			ret += 24;	
		}
		_bs.ReadBits(5, objTypes[layer]);
		ret += 5;
		if (objTypes[layer] == 31) 
		{
			bit6 n;
			_bs.ReadBits(6, n);
			objTypes[layer] = 32 + n;						
			ret += 6;
		}
	}

	switch (objTypes[layer]) 
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 6:
	case 7:
	case 17:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
		ret += GASpecificConfig(layer);
		break;
	}

	return ret;	
}

int CLATMParser::GetFramesInfo(int* sizes, uint8*& pStart)
{
	if (audio_mux_version_A == 1)
		return -1;

	PayloadLengthInfo();
	pStart = _bs.Position();

	int total = 0;
	for (int i = 0; i <= numSubFrames; i++)
	{
		sizes[i] = muxSlotLengthBytes[i]; //???
		total += sizes[i];
	}

	_bs.ShiftAlign(pStart, total);

	return numSubFrames + 1;
}

int CLATMParser::PayloadLengthInfo()
{
	uint8 tmp;
	if (all_same_framing) 
	{
		for (int prog=0; prog<=numProgram; prog++) 
		{
			for (int lay=0; lay <= _numLayer[prog]; lay++) 
			{
				if (frameLengthTypes[streamId[prog][lay]] == 0) {
					muxSlotLengthBytes[streamId[prog][lay]] = 0;
					do 
					{
						_bs.ReadBits(8, tmp);
						muxSlotLengthBytes[streamId[prog][lay]] += tmp;
					} 
					while (tmp == 255);
				} 
				else 
				{
					if (frameLengthTypes[streamId[prog][lay]] == 5 ||
						frameLengthTypes[streamId[prog][lay]] == 7 ||
						frameLengthTypes[streamId[prog][lay]] == 3) 
					{
						_bs.ReadBits(2, muxSlotLengthCoded[streamId[prog][lay]]);
					}
				}
			}
		}
	} else 
	{
		_bs.ReadBits(4, numChunk);
		for (int chunkCnt=0; chunkCnt <= numChunk; chunkCnt++) {
			bit4 streamIndex;
			_bs.ReadBits(4, streamIndex);
			int prog = progCIndx[chunkCnt] = progSIndex[streamIndex];
			int lay  = layCIndx[chunkCnt]  = laySIndex[streamIndex];
			if (frameLengthTypes[streamId[prog][lay]] == 0) 
			{
				muxSlotLengthBytes[streamId[prog][lay]] = 0;
				do 
				{
					_bs.ReadBits(8, tmp);
					muxSlotLengthBytes[streamId[prog][lay]] += tmp;
				} 
				while (tmp == 255);
				_bs.ReadBits(1, AuEndFlag[streamId[prog][lay]]);
			} 
			else 
			{
				if (frameLengthTypes[streamId[prog][lay]] == 5 ||
					frameLengthTypes[streamId[prog][lay]] == 7 ||
					frameLengthTypes[streamId[prog][lay]] == 3) 
				{
					_bs.ReadBits(2, muxSlotLengthCoded[streamId[prog][lay]]);
				}
			}
		}
	}
	return 0;
}

int CLATMParser::LatmGetValue()
{
	bit2 bytesForValue;
	_bs.ReadBits(2, bytesForValue);
	int value = 0;
	uint8 u8;
	for (int i=0; i<=bytesForValue; i++) 
	{
		_bs.ReadBits(8, u8);
		value <<= 8;
		value |= u8;
	}

	return value;
}

int CLATMParser::GetSubFrameNum()
{
	return numSubFrames;
}

int CLATMParser::GetMuxlength()
{
	return muxlength + 3;
}
