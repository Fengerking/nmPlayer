#include "mp4cfg.h"

using namespace MP4;

bool AudioSpecificConfig::Load(Reader& r)
{
	const uint32 SampleRates[] = 
	{
		96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000
	};

	r.ReadBits(&audioObjectType, 5);
	if (audioObjectType == 31)
	{
		r.ReadBits(&audioObjectType, 6);
		audioObjectType += 32;
	}

	r.ReadBits(&samplingFrequencyIndex, 4);
	if (samplingFrequencyIndex == 0xf)
		r.ReadBits(&samplingFrequency, 24);
	else
		samplingFrequency = SampleRates[samplingFrequencyIndex];

	r.ReadBits(&channelConfiguration, 4);

	return true;
}


ParameterSet::ParameterSet()
: data(0)
{
}

ParameterSet::~ParameterSet()
{
	if (data)
	{
		delete[] data;
		data = 0;
	}
}

bool ParameterSet::Load(Reader& r)
{
	bool b = r.Read(&len);
	data = new uint8[len];
	b &= r.Read(data, len);
	return b;
}

void ParameterSet::Init(const ParameterSet& another)
{
	len = another.len;
	data = new uint8[len];
	memcpy(data, another.data, len);
}



AVCDecoderConfigurationRecord::AVCDecoderConfigurationRecord()
: sequenceParameterSet(0),
  pictureParameterSet(0)
{
}

AVCDecoderConfigurationRecord::~AVCDecoderConfigurationRecord()
{
	if (sequenceParameterSet)
	{
		delete[] sequenceParameterSet;
		sequenceParameterSet = 0;
	}
	if (pictureParameterSet)
	{
		delete[] pictureParameterSet;
		pictureParameterSet = 0;
	}
}


void AVCDecoderConfigurationRecord::Init(const AVCDecoderConfigurationRecord& another)
{
	AVCProfileIndication = another.AVCProfileIndication;
	profile_compatibility = another.profile_compatibility;
	AVCLevelIndication = another.AVCLevelIndication;
	lengthSizeMinusOne = another.lengthSizeMinusOne;
	numOfSequenceParameterSets = another.numOfSequenceParameterSets;
	if (numOfSequenceParameterSets)
	{
		sequenceParameterSet = new ParameterSet[numOfSequenceParameterSets];
		for (uint8 i = 0; i < numOfSequenceParameterSets; i++)
		{
			sequenceParameterSet[i].Init(another.sequenceParameterSet[i]);
		}
	}
	numOfPictureParameterSets = another.numOfPictureParameterSets;
	if (numOfPictureParameterSets)
	{
		pictureParameterSet = new ParameterSet[numOfPictureParameterSets];
		for (uint8 i = 0; i < numOfPictureParameterSets; i++)
		{
			pictureParameterSet[i].Init(another.pictureParameterSet[i]);
		}
	}
}


bool AVCDecoderConfigurationRecord::Load(Reader& r)
{
	r.Move(1); //uint8 configurationVersion = 1
	r.Read(&AVCProfileIndication);
	r.Read(&profile_compatibility);
	r.Read(&AVCLevelIndication);
	r.Read(&lengthSizeMinusOne);
	lengthSizeMinusOne &= 0x03;
	
	r.Read(&numOfSequenceParameterSets);
	numOfSequenceParameterSets &= 0x1f;
	if (numOfSequenceParameterSets)
	{
		sequenceParameterSet = new ParameterSet[numOfSequenceParameterSets];
		for (int i = 0; i < numOfSequenceParameterSets; i++)
		{
			ParameterSet& param = sequenceParameterSet[i];
			param.Load(r);
		}
	}

	r.Read(&numOfPictureParameterSets);
	if (numOfPictureParameterSets)
	{
		pictureParameterSet = new ParameterSet[numOfPictureParameterSets];
		for (int i = 0; i < numOfPictureParameterSets; i++)
		{
			ParameterSet& param = pictureParameterSet[i];
			param.Load(r);
		}
	}

	return true;
}

uint32 AVCDecoderConfigurationRecord::ToH264Header(void* pBuf, uint32 cbBufSize)
{
	int num1 = numOfSequenceParameterSets;
	int num2 = numOfPictureParameterSets;
	int seqhSize = (num1 + num2) * 4;
	int i;
	for (i = 0; i < num1; i++)
		seqhSize += sequenceParameterSet[i].len;
	for (i = 0; i < num2; i++)
		seqhSize += pictureParameterSet[i].len;
	if (pBuf == 0)
		return seqhSize;
	if (cbBufSize < (uint32)seqhSize)
		return 0;

	uint8* p = (uint8*)pBuf;
	for (i = 0; i < num1; i++)
	{
		memcpy(p, "\x00\x00\x00\x01", 4);
		p += 4;
		memcpy(p, sequenceParameterSet[i].data, sequenceParameterSet[i].len);
		p += sequenceParameterSet[i].len;
	}
	for (i = 0; i < num2; i++)
	{
		memcpy(p, "\x00\x00\x00\x01", 4);
		p += 4;
		memcpy(p, pictureParameterSet[i].data, pictureParameterSet[i].len);
		p += pictureParameterSet[i].len;
	}
	return seqhSize;
}
