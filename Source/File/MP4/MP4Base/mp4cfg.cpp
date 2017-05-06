#include "mp4cfg.h"
#include "mpxutil.h"

//using namespace MP4;
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
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

bool ParameterSet::Dump(Writer& w)
{
	bool b = w.Write(len);
	b &= w.Write(data, len);
	return b;
}

void ParameterSet::Init(const ParameterSet& another)
{
	len = another.len;
	data = new uint8[len];
	memcpy(data, another.data, len);
}

void ParameterSet::Init(uint8* d, uint16 l)
{
	len = l;
	data = new uint8[l];
	memcpy(data, d, l);
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

bool AVCDecoderConfigurationRecord::Dump(Writer& w)
{
	uint8 configurationVersion = 1;
	w.Write(configurationVersion);
	w.Write(AVCProfileIndication);
	w.Write(profile_compatibility);
	w.Write(AVCLevelIndication);
	w.Write(lengthSizeMinusOne);
	
	int i;
	w.Write(numOfSequenceParameterSets);
	for (i = 0; i < numOfSequenceParameterSets; i++)
	{
		ParameterSet& param = sequenceParameterSet[i];
		param.Dump(w);
	}

	w.Write(numOfPictureParameterSets);
	for (i = 0; i < numOfPictureParameterSets; i++)
	{
		ParameterSet& param = pictureParameterSet[i];
		param.Dump(w);
	}

	return true;
}

bool AVCDecoderConfigurationRecord::InitParameterSetsFromH264Stream(uint8* stream, int streamsize)
{
	uint8* pos[32];
	bool seq[32];
	int count = 0;
	numOfSequenceParameterSets = 0;
	numOfPictureParameterSets = 0;
	uint8 *p1, *p2, *q;
	p1 = stream;
	p2 = p1 + streamsize;
	while ((q = Util::MemFind(p1, p2 - p1, (void*)"\x00\x00\x01", 3)))
	{
		p1 = q + 3;
		pos[count] = p1;
		if ((*p1 & 0x0f) == 0x07)
		{
			++numOfSequenceParameterSets;
			seq[count] = true;
		}
		else
		{
			++numOfPictureParameterSets;
			seq[count] = false;
		}
		++count;
		++p1;
	}
	pos[count] = p2 + 3;

	if (numOfSequenceParameterSets)
		sequenceParameterSet = new ParameterSet[numOfSequenceParameterSets];
	if (numOfPictureParameterSets)
		pictureParameterSet = new ParameterSet[numOfPictureParameterSets];

	int i1 = 0;
	int i2 = 0;
	for (int i = 0; i < count; i++)
	{
		if (seq[i])
			sequenceParameterSet[i1++].Init(pos[i], pos[i+1] - pos[i] - 3);
		else
			pictureParameterSet[i2++].Init(pos[i], pos[i+1] - pos[i] - 3);
	}

	return true;
}

int AVCDecoderConfigurationRecord::GetSize()
{
	int i = 0;
	int size = 7 + (numOfSequenceParameterSets + numOfPictureParameterSets) * 2;
	for (i = 0; i < numOfSequenceParameterSets; i++)
		size += sequenceParameterSet[i].len;
	for (i = 0; i < numOfPictureParameterSets; i++)
		size += pictureParameterSet[i].len;
	return size;
}

bool AVCDecoderConfigurationRecord::ToH264Stream(Writer* w)
{
	int num1 = numOfSequenceParameterSets;
	int num2 = numOfPictureParameterSets;
	int i;
	for (i = 0; i < num1; i++)
	{
		if (!w->Write("\x00\x00\x00\x01", 4))
			return false;
		if (!w->Write(sequenceParameterSet[i].data, sequenceParameterSet[i].len))
			return false;
	}
	for (i = 0; i < num2; i++)
	{
		if (!w->Write("\x00\x00\x00\x01", 4))
			return false;
		if (!w->Write(pictureParameterSet[i].data, pictureParameterSet[i].len))
			return false;
	}
	return true;
}


int AVCDecoderConfigurationRecord::ToAVCHeaderSize()
{
	int num1 = numOfSequenceParameterSets;
	int num2 = numOfPictureParameterSets;
	int seqhsize = (num1 + num2) * 2;
	int i;
	for (i = 0; i < num1; i++)
		seqhsize += sequenceParameterSet[i].len;
	for (i = 0; i < num2; i++)
		seqhsize += pictureParameterSet[i].len;
	return seqhsize;
}

bool AVCDecoderConfigurationRecord::ToAVCHeader(uint8* buf, int bufsize)
{
	int num1 = numOfSequenceParameterSets;
	int num2 = numOfPictureParameterSets;
	uint8* p = buf;
	int i;
	for (i = 0; i < num1; i++)
	{
		int len = sequenceParameterSet[i].len;
		*p++ = (len >> 8) & 0xff;
		*p++ = len & 0xff;
		memcpy(p, sequenceParameterSet[i].data, len);
		p += len;
	}
	for (i = 0; i < num2; i++)
	{
		int len = pictureParameterSet[i].len;
		*p++ = (len >> 8) & 0xff;
		*p++ = len & 0xff;
		memcpy(p, pictureParameterSet[i].data, len);
		p += len;
	}
	return (p - buf <= bufsize);
}

bool AVCDecoderConfigurationRecord::InitParameterSetsFromAVCHeader(uint8* header, int headersize)
{
	uint8* pos[32];
	bool seq[32];
	int count = 0;
	numOfSequenceParameterSets = 0;
	numOfPictureParameterSets = 0;
	uint8 *p1, *p2;
	p1 = header;
	p2 = p1 + headersize;
	while (p1 < p2)
	{
		int len = (*p1++) << 8;
		len += *p1++;
		pos[count] = p1;
		if ((*p1 & 0x0f) == 0x07)
		{
			++numOfSequenceParameterSets;
			seq[count] = true;
		}
		else
		{
			++numOfPictureParameterSets;
			seq[count] = false;
		}
		++count;
		p1 += len;
	}
	pos[count] = p2 + 2;

	if (numOfSequenceParameterSets)
		sequenceParameterSet = new ParameterSet[numOfSequenceParameterSets];
	if (numOfPictureParameterSets)
		pictureParameterSet = new ParameterSet[numOfPictureParameterSets];

	int i1 = 0;
	int i2 = 0;
	for (int i = 0; i < count; i++)
	{
		if (seq[i])
			sequenceParameterSet[i1++].Init(pos[i], pos[i+1] - pos[i] - 2);
		else
			pictureParameterSet[i2++].Init(pos[i], pos[i+1] - pos[i] - 2);
	}

	return true;
}


NALUArray::NALUArray()
: nalu(0)
{
}

NALUArray::~NALUArray()
{
	if (nalu)
	{
		delete[] nalu;
		nalu = 0;
	}
}

void NALUArray::Create(uint16 countNalus)
{
	numNalus = countNalus;
	if (numNalus)
		nalu = new ParameterSet[numNalus];
}

void NALUArray::Init(const NALUArray& another)
{
	array_completeness = another.array_completeness;
	NAL_unit_type = another.NAL_unit_type;
	numNalus = another.numNalus;
	if (numNalus)
	{
		nalu = new ParameterSet[numNalus];
		for (uint8 i = 0; i < numNalus; i++)
		{
			nalu[i].Init(another.nalu[i]);
		}
	}
}


bool NALUArray::Load(Reader& r)
{
	r.ReadBits(&array_completeness, 1);
	bit1 reserved;
	r.ReadBits(&reserved, 1);
	r.ReadBits(&NAL_unit_type, 6);
	
	r.Read(&numNalus);
	if (numNalus)
	{
		nalu = new ParameterSet[numNalus];
		for (int i = 0; i < numNalus; i++)
		{
			ParameterSet& param = nalu[i];
			param.Load(r);
		}
	}

	return true;
}

bool NALUArray::Dump(Writer& w)
{
	w.WriteBits(array_completeness, 1);
	bit1 reserved = 0;
	w.WriteBits(reserved, 1);
	w.WriteBits(NAL_unit_type, 6);

	int i;
	w.Write(numNalus);
	for (i = 0; i < numNalus; i++)
	{
		ParameterSet& param = nalu[i];
		param.Dump(w);
	}

	return true;
}

int NALUArray::GetSize()
{
	int size = 3;
	for (int i = 0; i < numNalus; i++)
	{
		ParameterSet& param = nalu[i];
		size += param.GetSize();
	}

	return size;
}

HEVCDecoderConfigurationRecord::HEVCDecoderConfigurationRecord()
: naluArray(0)
{
}

HEVCDecoderConfigurationRecord::~HEVCDecoderConfigurationRecord()
{
	if (naluArray)
	{
		delete[] naluArray;
		naluArray = 0;
	}
}


void HEVCDecoderConfigurationRecord::Init(const HEVCDecoderConfigurationRecord& another)
{
	profile_space = another.profile_space;
	profile_idc = another.profile_idc;
	constraint_indicator_flags = another.constraint_indicator_flags;
	level_idc = another.level_idc;
	profile_compatibility_indications = another.profile_compatibility_indications;
	chromaFormat = another.chromaFormat;
	bitDepthLumaMinus8 = another.bitDepthLumaMinus8;
	bitDepthChromaMinus8 = another.bitDepthChromaMinus8;
	avgFrameRate = another.avgFrameRate;
	constantFrameRate = another.constantFrameRate;
	numTemporalLayers = another.numTemporalLayers;
	lengthSizeMinusOne = another.lengthSizeMinusOne; 
	numOfArrays = another.numOfArrays;
	if (numOfArrays)
	{
		naluArray = new NALUArray[numOfArrays];
		for (uint8 i = 0; i < numOfArrays; i++)
		{
			naluArray[i].Init(another.naluArray[i]);
		}
	}
}

bool HEVCDecoderConfigurationRecord::Load(Reader& r)
{
#ifdef LIEYUN_DEBUG
	r.Move(1); //uint8 configurationVersion = 1;
	r.ReadBits(&profile_space, 3);
	r.ReadBits(&profile_idc, 5);
	r.Read(&constraint_indicator_flags);
	r.Read(&level_idc);
	r.Read(&profile_compatibility_indications);
	uint8 reserved;
	//bit6 reserved = ¡®111111¡¯b;
	r.ReadBits(&reserved, 6);
	r.ReadBits(&chromaFormat, 2);
	//bit5 reserved = ¡®11111¡¯b;
	r.ReadBits(&reserved, 5);
	r.ReadBits(&bitDepthLumaMinus8, 3);
	//bit5 reserved = ¡®11111¡¯b;
	r.ReadBits(&reserved, 5);
	r.ReadBits(&bitDepthChromaMinus8, 3);
	r.Read(&avgFrameRate);
	r.ReadBits(&constantFrameRate, 2);
	r.ReadBits(&numTemporalLayers, 3);
	//bit1 reserved = ¡®1¡¯b;
	r.ReadBits(&reserved, 1);
	r.ReadBits(&lengthSizeMinusOne, 2);
	
	r.Read(&numOfArrays);
	if (numOfArrays)
	{
		naluArray = new NALUArray[numOfArrays];
		for (int i = 0; i < numOfArrays; i++)
		{
			NALUArray& param = naluArray[i];
			param.Load(r);
		}
	}
#endif
	return true;
}

bool HEVCDecoderConfigurationRecord::Dump(Writer& w)
{
	uint8 configurationVersion = 1;
	w.Write(configurationVersion);
	w.WriteBits(profile_space, 3);
	w.WriteBits(profile_idc, 5);
	w.Write(constraint_indicator_flags);
	w.Write(level_idc);
	w.Write(profile_compatibility_indications);
	uint8 reserved = 0x3f;
	w.WriteBits(reserved, 6);
	w.WriteBits(chromaFormat, 2);
	reserved = 0x1f;
	w.WriteBits(reserved, 5);
	w.WriteBits(bitDepthLumaMinus8, 3);
	reserved = 0x1f;
	w.WriteBits(reserved, 5);
	w.WriteBits(bitDepthChromaMinus8, 3);
	w.Write(avgFrameRate);
	w.WriteBits(constantFrameRate, 2);
	w.WriteBits(numTemporalLayers, 3);
	reserved = 1;
	w.WriteBits(reserved, 1);
	w.WriteBits(lengthSizeMinusOne, 2); 
	w.Write(numOfArrays);

	int i;
	bool b = true;
	for (i = 0; i < numOfArrays; i++)
	{
		NALUArray& param = naluArray[i];
		b = b && param.Dump(w);
	}

	return b;
}

bool HEVCDecoderConfigurationRecord::InitArrayFromVOHeadData(uint8* header, int headersize)
{
	const uint8 FLAG_SPS = 0x21; // Tab 7-1 ISO/IEC 23008-2
	const uint8 FLAG_PPS = 0x22; // Tab 7-1 ISO/IEC 23008-2
	const uint8 FLAG_APS = 20; //??

	uint8* pos[32];
	uint8 type[32];
	int count = 0;
	int numOfSPS = 0;
	int numOfPPS = 0;
	int numOfAPS = 0;
	uint8 *p1, *p2, *q;
	p1 = header;
	p2 = p1 + headersize;
	while ((q = Util::MemFind(p1, p2 - p1, (void*)"\x00\x00\x01", 3)))
	{
		p1 = q + 3;
		pos[count] = p1;
		uint8 nalutype = (*p1 >> 1) & 0x3f;
		type[count] = nalutype;
		switch (nalutype)
		{
		case FLAG_SPS:
			++numOfSPS;
			break;

		case FLAG_PPS:
			++numOfPPS;
			break;

		case FLAG_APS:
			++numOfAPS;
			break;

		default:
			break;
		}
		++count;
		++p1;
	}
	pos[count] = p2 + 3;

	numOfArrays = 0;
	if (numOfSPS) numOfArrays++;
	if (numOfPPS) numOfArrays++;
	if (numOfAPS) numOfArrays++;

	NALUArray* arraySPS = 0;
	NALUArray* arrayPPS = 0;
	NALUArray* arrayAPS = 0;
	naluArray = new NALUArray[numOfArrays];
	NALUArray* p = naluArray;
	if (numOfSPS) 
	{
		p->array_completeness = 0;
		p->NAL_unit_type = FLAG_SPS;
		p->Create(numOfSPS);
		arraySPS = p++;
	}
	if (numOfPPS) 
	{
		p->array_completeness = 0;
		p->NAL_unit_type = FLAG_PPS;
		p->Create(numOfPPS);
		arrayPPS = p++;
	}
	if (numOfAPS) 
	{
		p->array_completeness = 0;
		p->NAL_unit_type = FLAG_APS;
		p->Create(numOfAPS);
		arrayAPS = p++;
	}

	int i1 = 0;
	int i2 = 0;
	int i3 = 0;
	for (int i = 0; i < count; i++)
	{
		uint8* np = pos[i];
		int ns = pos[i+1] - np - 3;
		switch (type[i])
		{
		case FLAG_SPS:
			arraySPS->nalu[i1++].Init(np, ns);
			break;

		case FLAG_PPS:
			arrayPPS->nalu[i2++].Init(np, ns);
			break;

		case FLAG_APS:
			arrayAPS->nalu[i3++].Init(np, ns);
			break;

		default:
			break;
		}
	}

	return true;
}

int HEVCDecoderConfigurationRecord::GetSize()
{
	int size = 16;
	for (int i = 0; i < numOfArrays; i++)
	{
		NALUArray& param = naluArray[i];
		size += param.GetSize();
	}
	return size;
}
