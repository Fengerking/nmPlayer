#ifndef __PARSEUTIL_H__
#define __PARSEUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

static const unsigned int BytesOfStartCode = 4;
static const unsigned char _NALUnitStartCode[4] = {0x00, 0x00, 0x00, 0x01};

bool IsSequenceParameterSet(unsigned char * paramSet, int len);
bool IsPictureParameterSet(unsigned char * paramSet, int len);
bool ParseSpropParameterSets(const char * _spropParamSets, unsigned char ** __paramSets, int * _paramSetsLength);

bool ParseMp4aLatmConfig(const char * _mp4aLatmConfig, unsigned char ** __configData, int * _configSize);

#ifdef __cplusplus
}
#endif

#endif //__PARSEUTIL_H__