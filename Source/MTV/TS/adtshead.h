
#ifndef _ADTS_HEADER_H
#define _ADTS_HEADER_H

#include "voYYDef_TS.h"
#include "abshead.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class ADTSHeader : public AbstractHeader
{
public:

#ifdef _HEAD_DECODE
	virtual bool Decode(uint8* data, uint32 size);
#endif //_HEAD_DECODE

#ifdef _HEAD_ENCODE
	virtual bool Encode(uint8* data, uint32& size);
#endif //_HEAD_ENCODE
	
#ifdef _HEAD_MTYPE
	virtual HRESULT BuildMediaType(CMediaType* pmt);
#endif //_HEAD_MTYPE


public:
	enum
	{
		ADTS_MAX_SIZE = 9,
		FRAME_SAMPLES = 1024  //raw samples per frame
	};

public:
	bool Parse(uint8* data);

private:
	uint8* _head;

	uint16 _frame_len; //whole frame len, include head
	uint16 _data_len;  //actual data len, exclue head
	uint8* _frame_data;


public:
	uint8* GetFrameData() const { return _frame_data; }

	uint16 GetFrameLen() const { return _frame_len; }
	uint16 GetDataLen() const { return _data_len; }
	uint16 GetHeadLen() const { return _frame_len - _data_len; }

	bool IsMPEG2() const;
	uint8 GetProfile() const;
	uint8 GetSampleRateIndex() const;
	uint8 GetChannelConfig() const;

	uint8 GetAACObjectType() const;
	uint32 GetSampleRate() const;
	uint16 GetChannelCount() const;

	void ToDSI(uint8 dsi[2]) const;

public:
	static inline bool IsSyncWord(uint8* p);
	static inline bool IsNotSyncWord(uint8* p);
	static bool FindHead(void* data, int len, void** ppResult,bool beStrict = false);
	static bool FindHead(void* data, int len, void** ppResult, uint8 f1, uint8 f2, uint8 f3);

};


//====================================================================

inline bool ADTSHeader::IsSyncWord(uint8* p)
{
	//return ( *(uint16*)p & 0xf0ff ) == 0xf0ff;
	//data alignment
	return ( (*p == 0xff) && ((*(p+1) & 0xf0) == 0xf0) );
}

inline bool ADTSHeader::IsNotSyncWord(uint8* p)
{
	//return ( *(uint16*)p & 0xf0ff ) != 0xf0ff;
	return ( (*p != 0xff) || ((*(p+1) & 0xf0) != 0xf0) );
}

#ifdef _VONAMESPACE
}
#endif

#endif // _ADTS_HEADER_H
