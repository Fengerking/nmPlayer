#include "commonheader.h"
#include "base64.h"
#include "list_T.h"
#include "parseutil.h"

struct PARAMSET
{
	unsigned char * m_paramSet;
	int m_paramSetLen;
};


bool IsSequenceParameterSet(unsigned char * paramSet, int len)
{
#if 1
	if(len < 1)
		return false;

	if((paramSet[0] & 0x07) == 7)
	{
	   return true;
	}
#endif

#if 1
	if(len < 5)
		return false;

	if(paramSet[0] == 0x00 && 
	   paramSet[1] == 0x00 && 
	   paramSet[2] == 0x00 && 
	   paramSet[3] == 0x01 && 
	   (paramSet[4] & 0x07) == 7)
	{
	   return true;
	}
#endif

	return false;
}

bool IsPictureParameterSet(unsigned char * paramSet, int len)
{
#if 1
	if(len < 1)
		return false;

	if((paramSet[0] & 0x08) == 8)
	{
	   return true;
	}
#endif

#if 1
	if(len < 5)
		return false;

	if(paramSet[0] == 0x00 && 
	   paramSet[1] == 0x00 && 
	   paramSet[2] == 0x00 && 
	   paramSet[3] == 0x01 && 
	   (paramSet[4] & 0x08) == 8)
	{
	   return true;
	}
#endif

	return false;
}

bool ParseSpropParameterSets(const char * _spropParamSets, unsigned char ** __paramSets, int * _paramSetsLength)
{
	char * _fmtpSpropParameterSets = StringDup(_spropParamSets);
	if(_fmtpSpropParameterSets == NULL)
		return false;
	size_t fmtpSpropParameterSetsLen = strlen(_fmtpSpropParameterSets);
	if(fmtpSpropParameterSetsLen == 0)
		return false;

	*_paramSetsLength = 0;
	*__paramSets = new unsigned char[fmtpSpropParameterSetsLen * 4];
	if(*__paramSets == NULL)
		return false;

	int spropParamSetCount = 1;
	for(char * _c=_fmtpSpropParameterSets; *_c!='\0'; ++_c) 
	{
		if(*_c == ',')
		{
			++spropParamSetCount;
			*_c = '\0';
		}
	}

	list_T<PARAMSET> listParamSet;

	// Parse parameter sets
	char * _paramSet = _fmtpSpropParameterSets;
	for(int i=0; i<spropParamSetCount; ++i)
	{
		size_t paramSetLen = 0; 
		unsigned char * _param = Base64Decode(_paramSet, paramSetLen);
		if(_param == NULL)
		{
			_paramSet += (strlen(_paramSet) + 1);
			continue;
		}

		PARAMSET paramSet;
		paramSet.m_paramSet = _param;
		paramSet.m_paramSetLen = paramSetLen;
		listParamSet.push_back(paramSet);

		_paramSet += (strlen(_paramSet) + 1);
	}
	SAFE_DELETE_ARRAY(_fmtpSpropParameterSets);

	unsigned char * _writePos = *__paramSets;
	int paramSetsLength = 0;

	// Copy the SPS first
	list_T<PARAMSET>::iterator iter;
	for(iter=listParamSet.begin(); iter!=listParamSet.end(); iter++)
	{
		PARAMSET paramSet = (PARAMSET)*iter;
		if(paramSet.m_paramSet == NULL)
			continue;

		if(IsSequenceParameterSet(paramSet.m_paramSet, paramSet.m_paramSetLen))
		{
			memcpy(_writePos, _NALUnitStartCode, BytesOfStartCode);
			_writePos += BytesOfStartCode;
			paramSetsLength += BytesOfStartCode;

			memcpy(_writePos, paramSet.m_paramSet, paramSet.m_paramSetLen);
			_writePos += paramSet.m_paramSetLen;
			paramSetsLength += paramSet.m_paramSetLen;

			SAFE_DELETE_ARRAY(iter->m_paramSet);
			listParamSet.remove(iter);
			break;
		}
	}

	// Copy other parameter set
	for(iter=listParamSet.begin(); iter!=listParamSet.end(); iter++)
	{
		PARAMSET paramSet = (PARAMSET)*iter;
		if(paramSet.m_paramSet == NULL)
			continue;

		memcpy(_writePos, _NALUnitStartCode, BytesOfStartCode);
		_writePos += BytesOfStartCode;
		paramSetsLength += BytesOfStartCode;

		memcpy(_writePos, paramSet.m_paramSet, paramSet.m_paramSetLen);
		_writePos += paramSet.m_paramSetLen;
		paramSetsLength += paramSet.m_paramSetLen;

		SAFE_DELETE_ARRAY(iter->m_paramSet);
	}

	listParamSet.clear();

	*_paramSetsLength = paramSetsLength;

	return true;
}

bool ParseMp4aLatmConfig(const char * _mp4aLatmConfig, unsigned char ** __configData, int * _configSize)
{
	bool audioMuxVersion = false;
	bool allStreamsSameTimeFraming = true;
	unsigned int subFrames = 0;
	unsigned int program = 0;
	unsigned int layer = 0;

	*__configData = NULL;
	*_configSize = 0;

	do 
	{
		const char * from = _mp4aLatmConfig;
		unsigned char uc = 0;
		uc = (Value(*from++) << 4) | Value(*from++);

		audioMuxVersion = ((uc & 0x80) >> 7) != 0;
		if(audioMuxVersion != 0) 
			break;

		allStreamsSameTimeFraming = ((uc & 0x40) >> 6) != 0;
		subFrames = (uc & 0x3F);

		uc = (Value(*from++) << 4) | Value(*from++);
		program = (uc & 0xF0) >> 4;

		layer = (uc & 0x0E) >> 1;

		unsigned char lastBit = uc & 1;

		*_configSize = (strlen(from) + 1) / 2 + 1;

		*__configData = new unsigned char[*_configSize];
		if(*__configData == NULL)
			break;
		memset(*__configData, 0, *_configSize);

		unsigned char * to = *__configData;
		do 
		{
			uc = Value(*from++) << 4; 
			uc |= Value(*from++);

			*to++ = (lastBit << 7) | ((uc & 0xFE) >> 1);
			lastBit = uc & 1;
		} while(from < _mp4aLatmConfig + strlen(_mp4aLatmConfig));

//		*_configSize = 2;

		return true;

	} while(0);
	
	return false;
}
