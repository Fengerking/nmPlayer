/**
 * MPEG4 Config Record 
 * @created 4/29/2006
 */

#ifndef _MP4_CFG_H
#define _MP4_CFG_H

#include "vobstype.h"
#include "isomio.h"

namespace MP4 {

using namespace ISOM;

struct AudioSpecificConfig
{
public:
	bit8 audioObjectType;
	bit4 samplingFrequencyIndex;
	uint32 samplingFrequency;
	bit4 channelConfiguration;

	bool Load(Reader& r);
	bool Load(void* pData, uint32 cbSize)
	{
		MemStream ms((uint8*)pData, cbSize);
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

	void Init(const ParameterSet& another);
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
	bool Load(void* pData, uint32 cbSize)
	{
		MemStream ms((uint8*)pData, cbSize);
		ReaderMSB r(&ms);
		return Load(r);
	}

	uint32 ToH264Header(void* pBuf, uint32 cbBufSize);
};


} //namespace MP4

#endif //.h