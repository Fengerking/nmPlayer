#ifndef __LATMPARSER_H__
#define __LATMPARSER_H__

#include "voYYDef_TS.h"
#include "readutil.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#define  MAX_LATM_AAC_FRAME_COUNT    127
class CLATMParser
{
private:

	// internals
	int			  streamId[16][8];	

	uint8		  progSIndex[128];
	uint8   	  laySIndex[128];

	uint8		  frameLengthTypes[128];
	uint8		  latmBufferFullness[128];
	uint16		  frameLength[128];
	uint16		  muxSlotLengthBytes[128];
	uint8		  muxSlotLengthCoded[128];
	uint8		  _numLayer[16];
	uint8		  progCIndx[128];
	uint8		  layCIndx[128];
	uint8		  AuEndFlag[128];
	uint8		  objTypes[8];
	uint8		  audio_mux_version;
	uint8		  audio_mux_version_A;
	uint8		  all_same_framing;
	int			  taraFullness;
	uint8		  config_crc;
	int32		  other_data_bits;
	bit4		  numProgram;
	bit6		  numSubFrames;
	int			  streamCnt;
	bit4		  numChunk;
	bit24		  _samplerate;
	bit4		  _channel;
	uint16		  muxlength;
	uint8         _audiocfg[4];
	BitStream	  _bs;


public:
	CLATMParser();
	virtual ~CLATMParser();
	int	Parse(uint8 *buf, int size);

	uint8 GetAACObjectType() const { return objTypes[0]; } //???
	uint32 GetSampleRate() const { return _samplerate; }
	uint16 GetChannelCount() const { return _channel; }

	uint8* GetAudioSpecificConfig() { return _audiocfg; }

	// Extracts LATM payload
	int GetFramesInfo(int* sizes, uint8*& pStart);

	int GetSubFrameNum();

protected:
	// only reads Config information
	int	ReadMUXConfig();


	int GetMuxlength();

private:
	int LatmGetValue();
	int	ReadAudioSpecConfig(int layer);
	int GASpecificConfig(int layers);
	int PayloadLengthInfo();

	//add by qichaoshen @ 2011-11-11
	int iFlagGetMuxInfo;
    //add by qichaoshen @ 2011-11-11
	int lastError;
};


#ifdef _VONAMESPACE
}
#endif

#endif //__LATMPARSER_H__