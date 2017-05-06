/**
 * MPEG4 Config Record 
 * @created 4/29/2006
 */

#ifndef _MP4_CFG_H
#define _MP4_CFG_H

#include "mpxtype.h"
#include "mpxio.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
//namespace MP4 {

//using namespace MPx;

struct AudioSpecificConfig
{
public:
	bit8 audioObjectType;
	bit4 samplingFrequencyIndex;
	uint32 samplingFrequency;
	bit4 channelConfiguration;

	bool Load(Reader& r);
	bool Load(uint8* pData, uint32 cbSize)
	{
		MemStream ms(pData, cbSize);
		ReaderMSB r(&ms);
		return Load(r);
	}
};


struct ParameterSet
{
public:
	uint16 len;
	uint8* data;

	ParameterSet();
	~ParameterSet();

	bool Load(Reader& r);
	bool Dump(Writer& w);

	int GetSize() const { return 2 + len; }

	void Init(const ParameterSet& another);
	void Init(uint8* d, uint16 l);
};

struct AVCDecoderConfigurationRecord
{
	uint8 AVCProfileIndication;
	uint8 profile_compatibility;
	uint8 AVCLevelIndication;
	uint8 lengthSizeMinusOne;
	uint8 numOfSequenceParameterSets;
	ParameterSet* sequenceParameterSet;
	uint8 numOfPictureParameterSets;
	ParameterSet* pictureParameterSet;

	AVCDecoderConfigurationRecord();
	~AVCDecoderConfigurationRecord();

	void Init(const AVCDecoderConfigurationRecord& another);

	bool Load(Reader& r);
	bool Load(uint8* pData, uint32 cbSize)
	{
		MemStream ms(pData, cbSize);
		ReaderMSB r(&ms);
		return Load(r);
	}

	bool Dump(Writer& w);

	bool ToH264Stream(Writer* w);
	int ToAVCHeaderSize();
	bool ToAVCHeader(uint8* buf, int bufsize);

	bool InitParameterSetsFromH264Stream(uint8* stream, int streamsize);
	bool InitParameterSetsFromAVCHeader(uint8* header, int headersize);

	int GetSize();
};

struct NALUArray {
	bit1 array_completeness;
	bit6 NAL_unit_type;
	uint16 numNalus;
	ParameterSet* nalu;

	NALUArray();
	~NALUArray();

	bool Load(Reader& r);
	bool Dump(Writer& w);
	int GetSize();

	void Init(const NALUArray& another);
	void Init(uint8* d, uint16 l);
	void Create(uint16 countNalus);
};

struct HEVCDecoderConfigurationRecord
{
	//uint8 configurationVersion = 1;
	bit3 profile_space;
	bit5 profile_idc;
	uint16 constraint_indicator_flags;
	uint8 level_idc;
	bit32 profile_compatibility_indications;
	//bit6 reserved = ¡®111111¡¯b;
	bit2 chromaFormat;
	//bit5 reserved = ¡®11111¡¯b;
	bit3 bitDepthLumaMinus8;
	//bit5 reserved = ¡®11111¡¯b;
	bit3 bitDepthChromaMinus8;
	bit16 avgFrameRate;
	bit2 constantFrameRate;
	bit3 numTemporalLayers;
	//bit1 reserved = ¡®1¡¯b;
	bit2 lengthSizeMinusOne; 
	uint8 numOfArrays;
	NALUArray* naluArray;

	HEVCDecoderConfigurationRecord();
	~HEVCDecoderConfigurationRecord();

	void Init(const HEVCDecoderConfigurationRecord& another);

	bool Load(Reader& r);
	bool Load(uint8* pData, uint32 cbSize)
	{
		MemStream ms(pData, cbSize);
		ReaderMSB r(&ms);
		return Load(r);
	}

	int GetSize();
	bool Dump(Writer& w);

	bool InitArrayFromVOHeadData(uint8* header, int headersize);

	/*
	bool ToH264Stream(Writer* w);
	int ToAVCHeaderSize();
	bool ToAVCHeader(uint8* buf, int bufsize);

	bool InitParameterSetsFromH264Stream(uint8* stream, int streamsize);

	int GetSize();
	*/
};


#pragma pack(push, 1)

struct H263DecSpecStruc
{
	uint32 vendor;
	uint8 decoder_version;
	uint8 H263_Level;
	uint8 H263_Profile;
};

struct AMRDecSpecStruc
{
	uint32 vendor;
	uint32 decoder_version     : 8;
	uint32 mode_set            :16;
	uint32 mode_change_period  : 8;
	uint8 frames_per_sample;
};

struct QCELPDecSpecStruc
{
	uint32 vendor;
	uint8 decoder_version;
	uint8 frames_per_sample;
};

struct SMVDecSpecStruc
{
	uint32 vendor;
	uint8 decoder_version;
	uint8 frames_per_sample;
};

struct EVRCDecSpecStruc
{
	uint32 vendor;
	uint8 decoder_version;
	uint8 frames_per_sample;
};




#pragma pack(pop)

//} //namespace MP4
#ifdef _VONAMESPACE
}
#endif
#endif //_MP4_CFG_H
