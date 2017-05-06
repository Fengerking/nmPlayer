#ifndef _FORMAT_INITIALIZER_H_
#define _FORMAT_INITIALIZER_H_

#include "voYYDef_TS.h"
#include "BaseParser.h"
#include "tsparse.h"
#include "voSource.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

using namespace TS;

class CHeadParser
{
public:
	CHeadParser();
	virtual ~CHeadParser();
	uint8* GetHeadData() { return headdata; }
	int GetHeadSize() { return headsize; }
	virtual bool Process(uint8* data, int size) = 0; //return true if got head data

protected:
	uint8* headdata;
	int headsize;
	uint8* m_pPrivate1;
	uint8* m_pPrivate2;
	int    m_iPrivate1Len;
	int    m_iPrivate2Len;
};

class CFormatInitializer 
	: public CBaseTSParser
	, public IPESListener
{
public:
	CFormatInitializer();
	~CFormatInitializer();

//Implement IPESListener
//----------------------------------------
public:
	virtual void OnElementInfo(int total, int playbackable);
	virtual void OnElementStream(ESConfig* escfg);
	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet);
	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData);

private:
	int status;
	CHeadParser* videohead;
	CHeadParser* audiohead;
	int playbackableES;
	uint8  m_uFormatState;
	uint8* m_ppPrivateDataForFormat[0x2000]; //Max PID Count
	uint32 m_pulPrivateDataForFormatLen[0x2000]; //Max PID Count
    uint32 m_ulReadyStreamCount;
    uint32 m_ulOpenFlag;
	



public:
	bool IsStreamReady() 
	{
        int   iCount = 0;
		// tag: 20100428
		//return (status & 0x30);
		// end
		if((m_ulOpenFlag & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL) == VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)
        {
            return (status & 0x20) == 0x20;
		}

		if (playbackableES > 1)
		{
            return m_ulReadyStreamCount == playbackableES;
		}
		else
		{
			return ((status & 0x30) != 0); //audio or video
		}
	}

	bool IsAudioReady() { return (status & 0x10) == 0x10; }
	bool IsVideoReady() { return (status & 0x20) == 0x20; }

	void SetOpenFlag(uint32 ulOpenFlag);

protected:
	void SetAudioReady() { status |= 0x10; }
	void SetVideoReady() { status |= 0x20; }

	void InitStreamFromDescriptor(ESConfig* escfg);
	void InitStreamFromConfig(ESConfig* escfg);
	void InitStreamFromPES(ESConfig* escfg, PESPacket* packet);

	void InitMPEG2(ESConfig* escfg, uint8* pData, uint32 cbData);
	void InitMPEG4(ESConfig* escfg, uint8* pData, uint32 cbData);
	void InitMP3(ESConfig* escfg, uint8* pData, uint32 cbData,uint32 syncword);///<0 means 11172,1 means 13818
	void InitAC3(ESConfig* escfg, uint8* pData, uint32 cbData);
	void InitEAC3(ESConfig* escfg, uint8* pData, uint32 cbData);
    
	//Add for DTS
	void InitDTS(ESConfig* escfg, uint8* pData, uint32 cbData);
    //Add for DTS

	void InitM2TSLPCM(ESConfig* escfg, uint8* pData, uint32 cbData);
	void InitTeletext(ESConfig* escfg, uint8* pData, uint32 cbData);
	void InitH264(ESConfig* escfg, uint8* pData, uint32 cbData);
	void InitAdtsAAC(ESConfig* escfg, uint8* pData, uint32 cbData);
	void InitLatmAAC(ESConfig* escfg, uint8* pData, uint32 cbData);

	void NewStreamAVC(int streamid, void* dsidata, int dsisize);
	void NewStreamAAC(int streamid, void* dsidata, int dsisize);
	void InitMetaDataID3(ESConfig* escfg, uint8* pData, uint32 cbData);
	void InitDvbSubTitle(ESConfig* escfg, uint8* pData, uint32 cbData);
    void InitHEVC(ESConfig* escfg, uint8* pData, uint32 cbData);    
};


#endif //_FORMAT_INITIALIZER_H_