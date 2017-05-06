#ifndef _TS_BASIC_PARSER_H_
#define _TS_BASIC_PARSER_H_

#include "voYYDef_TS.h"
#include "tsstruct.h"
#include "PacketBuffer.h"
#include "tsconst.h"

//#define _SUPPORT_CACHE
#ifdef _WIN32_WCE
#define CACHE_SIZE (6 << 20)
#else //_WIN32_WCE
#define CACHE_SIZE (16 << 20)
#endif //_WIN32_WCE


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


namespace TS {

template<class LISTENER> class ListenerNode
{
public:
	LISTENER* object;
	ListenerNode* next;

	ListenerNode(LISTENER* obj=NULL)
		: object(obj)
		, next(NULL)
	{
	}
};

template<class LISTENER> class ListenerList
{
public:
	ListenerNode<LISTENER>* head;

public:
	ListenerList()
		: head(NULL)
	{
	}

	~ListenerList()
	{
		Release();
	}

	void Set(LISTENER* obj)
	{
		Release();
		Add(obj);
	}

	void Add(LISTENER* obj)
	{
		ListenerNode<LISTENER>* target = new ListenerNode<LISTENER>(obj);
		// tag: 20100423
		target->next = NULL;
		// end
		ListenerNode<LISTENER>* prev = NULL;
		ListenerNode<LISTENER>* node = head;
		while (node)
		{
			if (node->object == obj) //already in the list
				return;
			prev = node;
			node = node->next;
		}
		if (prev)
			prev->next = target;
		else
			head = target;
	}

	bool Remove(LISTENER* obj)
	{
		ListenerNode<LISTENER>* prev = NULL;
		ListenerNode<LISTENER>* node = head;
		while (node)
		{
			if (node->object == obj)
			{
				if (prev)
					prev->next = node->next;
				else
					head = node->next;
				delete node;
				return true;
			}
			prev = node;
			node = node->next;
		}
		return false;
	}

	void Clear()
	{
		Release();
	}

protected:
	void Release()
	{
		ListenerNode<LISTENER>* node = head;
		while (node)
		{
			ListenerNode<LISTENER>* temp = node;
			node = node->next;
			delete temp;
		}

		head = NULL;
	}

};




/**
 * Handle error
 */
class IErrorListener
{
public:
	//lose one or next packets before this packet
	virtual void OnLosePacket(RawPacket* packet,int nErrorCode) = 0;
};


/**
 * Handle raw packet
 */
class IRawListener
{
public:
	virtual void OnRawPacket(RawPacket* packet) = 0;
};

class IRawListeners 
	: public ListenerList<IRawListener>
	, public IRawListener
{
public:
	virtual void OnRawPacket(RawPacket* packet)
	{
		ListenerNode<IRawListener>* node = head;
		while (node)
		{
			IRawListener* obj = node->object;
			node = node->next;  //! current node may be deleted after On...
			obj->OnRawPacket(packet);
		}
	}
};


class IPSIListener
{
public:
	virtual void OnPSI(PSI* psi) = 0;
};

class IPSIListeners 
	: public ListenerList<IPSIListener>
	, public IPSIListener
{
public:
	virtual void OnPSI(PSI* psi)
	{
		ListenerNode<IPSIListener>* node = head;
		while (node)
		{
			IPSIListener* obj = node->object;
			node = node->next;
			obj->OnPSI(psi);
		}
	}
};


class IPATListener
{
public:
	virtual void OnPAT(PAT* pat) = 0;
};

class IPATListeners 
	: public ListenerList<IPATListener>
	, public IPATListener
{
public:
	virtual void OnPAT(PAT* pat)
	{
		ListenerNode<IPATListener>* node = head;
		while (node)
		{
			IPATListener* obj = node->object;
			node = node->next;
			obj->OnPAT(pat);
		}
	}
};

class IMGTListener
{
public:
	virtual void OnMGT(MGT* pMGT) = 0;
};

class IMGTListeners 
	: public ListenerList<IMGTListener>
	, public IMGTListener
{
public:
	virtual void OnMGT(MGT* pMGT)
	{
		ListenerNode<IMGTListener>* node = head;
		while (node)
		{
			IMGTListener* obj = node->object;
			node = node->next;
			obj->OnMGT(pMGT);
		}
	}
};


class IPMTListener
{
public:
	virtual void OnPMT(PMT* pmt) = 0;
};

class IPMTListeners 
	: public ListenerList<IPMTListener>
	, public IPMTListener
{
public:
	virtual void OnPMT(PMT* pmt)
	{
		ListenerNode<IPMTListener>* node = head;
		while (node)
		{
			IPMTListener* obj = node->object;
			node = node->next;
			obj->OnPMT(pmt);
		}
	}
};


class ISDTListener
{
public:
	virtual void OnSDT(SDT* sdt) = 0;
};

class ISDTListeners 
	: public ListenerList<ISDTListener>
	, public ISDTListener
{
public:
	virtual void OnSDT(SDT* sdt)
	{
		ListenerNode<ISDTListener>* node = head;
		while (node)
		{
			ISDTListener* obj = node->object;
			node = node->next;
			obj->OnSDT(sdt);
		}
	}
};


class INITListener
{
public:
	virtual void OnNIT(NIT* nit) = 0;
};

class INITListeners 
	: public ListenerList<INITListener>
	, public INITListener
{
public:
	virtual void OnNIT(NIT* nit)
	{
		ListenerNode<INITListener>* node = head;
		while (node)
		{
			INITListener* obj = node->object;
			node = node->next;
			obj->OnNIT(nit);
		}
	}
};


class IEITListener
{
public:
	virtual void OnEIT(EIT* eit) = 0;
};

class IEITListeners 
	: public ListenerList<IEITListener>
	, public IEITListener
{
public:
	virtual void OnEIT(EIT* eit)
	{
		ListenerNode<IEITListener>* node = head;
		while (node)
		{
			IEITListener* obj = node->object;
			node = node->next;
			obj->OnEIT(eit);
		}
	}
};


class ITDTListener
{
public:
	virtual void OnTDT(TDT* tdt) = 0;
};

class ITDTListeners 
	: public ListenerList<ITDTListener>
	, public ITDTListener
{
public:
	virtual void OnTDT(TDT* tdt)
	{
		ListenerNode<ITDTListener>* node = head;
		while (node)
		{
			ITDTListener* obj = node->object;
			node = node->next;
			obj->OnTDT(tdt);
		}
	}
};


class IPESListener
{
public:
	virtual void OnElementInfo(int total, int playbackable) {}
	virtual void OnElementStream(ESConfig* escfg) = 0;
	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet) = 0;
	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData) = 0;
	//virtual void OnRawData(ESConfig* escfg, uint8* pData, uint32 cbData) {;}
};


class IPESListeners 
	: public ListenerList<IPESListener>
	, public IPESListener
{
public:
	virtual void OnElementInfo(int total, int playbackable)
	{
		ListenerNode<IPESListener>* node = head;
		while (node)
		{
			IPESListener* obj = node->object;
			node = node->next;
			obj->OnElementInfo(total, playbackable);
		}
	}


	virtual void OnElementStream(ESConfig* escfg)
	{
		ListenerNode<IPESListener>* node = head;
		while (node)
		{
			IPESListener* obj = node->object;
			node = node->next;
			obj->OnElementStream(escfg);
		}
	}

	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet)
	{
		ListenerNode<IPESListener>* node = head;
		while (node)
		{
			IPESListener* obj = node->object;
			node = node->next;
			obj->OnPESHead(escfg, packet);
		}
	}

	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData)
	{
		ListenerNode<IPESListener>* node = head;
		while (node)
		{
			IPESListener* obj = node->object;
			node = node->next;
			obj->OnPESData(escfg, pData, cbData);
		}
	}

};


/** 
 * Parse TS raw data
 * Input: TS raw data
 * Output: Transport packet
 */
class RawParser
{
public:
	RawParser();
	~RawParser();
	bool Process(uint8* pData, int32 cbData, int32* pcbProcessed);
	void Reset(); //reset internal status

	void SetRawListener(IRawListener* listener) { _rawListener = listener; }
	void SetErrorListener(IErrorListener* listener) { _errListener = listener; }
	void ProcessCache();

public:
	bool SetPIDFilter(int count, bit13* PIDs=NULL);

	PIDInfo* GetPIDInfo(bit13 PID) { return _PIDsInfo.GetPIDInfo(PID); }
	PIDsInfo* GetPIDsInfo() { return &_PIDsInfo; }

private:
	bit13* _filterPIDs;
	int _filterCount;

	IRawListener* _rawListener;
	IErrorListener* _errListener;
	uint32 _packetSize;
	uint32 _iPacket; //packet index
	//RawPacket _packet; //current packet

	PIDsInfo _PIDsInfo;
	PIDInfo* _PIDInfo; //current PID Info
	int _countSyncErr;
	uint8 *m_pPacketBak;
	uint16 m_nLenPacketBak;


protected:
	uint8* ParseAdaptationField(uint8* pData);
	bool ParseOnePacket(uint8* pData, uint32 cbData);
	bool CheckContinuityCounter(RawPacket* packet);

protected:
	uint8* FindPacketHeader(uint8* pData, int cbData, int packetSize=TransportPacketSize);
	uint8* FindPacketHeader2(uint8* pData, int cbData, int packetSize=TransportPacketSize);
	int CheckPacketSize(uint8* pData, int cbData);

#ifdef _SUPPORT_CACHE

public:
	void EnableCache(bool b) { _cacheStatus = b ? 1 : 0; }
	void SendCache() { _cacheStatus = 2; }
	void SetCacheSize(int maxSize) { _cache.SetMaxSize(maxSize); }
	void FlushCache() { _cacheStatus = 3; }

protected:
	bool ShouldSendCache() { return _cacheStatus == 2; }
	bool ShouldDoCache() { return _cacheStatus >= 1; }
	bool ShouldFlushCache() { return _cacheStatus == 3; }

private:
	CPacketBuffer _cache;
	int _cacheStatus;
#endif //_SUPPORT_CACHE

public:
	void BreakParse(bool b=true) { _breakParse = b; }
protected:
	bool ShouldBreak() { return _breakParse; }
private:
	bool _breakParse;

};

class PSIParser : public IRawListener
{
public:
	virtual void OnRawPacket(RawPacket* packet);
	void SetListener(IPSIListener* listener) { _listener = listener; }
private:
	void ParseSection(uint8* data, int data_len);
	int  PeekSectionLength(uint8* data);

private:
	bool			_parsingSection;
	IPSIListener* _listener;
	PSI _PSI;
	CPacketBuffer _bufferPSI;
};

class PATParser	
	: public IRawListener
	, public IPSIListener
{
public:
	PATParser();
	virtual void OnRawPacket(RawPacket* packet);
	virtual void OnPSI(PSI* psi);
	void SetListener(IPATListener* listener) { _listener = listener; }

protected:
	void InitForISDBT(RawPacket* packet);

private:
	PSIParser _PSIParser;
	IPATListener* _listener;
	PAT _PAT;
};


class MGTParser	
	: public IRawListener
	, public IPSIListener
{
public:
	MGTParser();
	virtual void OnRawPacket(RawPacket* packet);
	virtual void OnPSI(PSI* psi);
	void SetListener(IMGTListener* listener) { _listener = listener; }

private:
	PSIParser _PSIParser;
	IMGTListener* _listener;
	MGT _MGT;
};

class PMTParser	
	: public IRawListener
	, public IPATListener
	, public IPSIListener
{
public:
	PMTParser();

	virtual void OnRawPacket(RawPacket* packet);
	virtual void OnPSI(PSI* psi);
	virtual void OnPAT(PAT* PAT) { _PAT = PAT; }
	void SetListener(IPMTListener* listener) { _listener = listener; }
	void Reset(); //reset internal status


private:
	PSIParser _PSIParser;
	IPMTListener* _listener;
	PMT _PMT;
	PAT* _PAT;
};



class SDTParser	
	: public IRawListener
	, public IPSIListener
{
public:
	SDTParser();
	virtual void OnRawPacket(RawPacket* packet);
	virtual void OnPSI(PSI* psi);
	void SetListener(ISDTListener* listener) { _listener = listener; }

private:
	ISDTListener* _listener;
	PSIParser _PSIParser;
};


class NITParser	
	: public IRawListener
	, public IPATListener
	, public IPSIListener
{
public:
	NITParser();
	virtual void OnRawPacket(RawPacket* packet);
	virtual void OnPSI(PSI* psi);
	virtual void OnPAT(PAT* pat);
	void SetListener(INITListener* listener) { _listener = listener; }

private:
	INITListener* _listener;
	PSIParser _PSIParser;
	int _network_PID;
};


typedef void  ( *EITDescDataCallback)(uint8*  pData , uint32  ulDataLength);

class EITParser	
	: public IRawListener
	, public IMGTListener
	, public IPSIListener
{
public:
	EITParser();
	virtual void OnRawPacket(RawPacket* packet);
	virtual void OnPSI(PSI* psi);
    virtual void OnMGT(MGT*  pMGT);
	void SetListener(IEITListener* listener) { _listener = listener; }
	void IsReady();
	void SetEITCallbackFun(void*  pFunc) {m_pEITCallbackFunc=pFunc; }

private:
	IEITListener* _listener;
	PSIParser _PSIParser;
	MGT*      m_pMGT;
    void*     m_pEITCallbackFunc;
};


class PESParser	
	: public IRawListener
{
public:
	PESParser();
	void SetListener(IPESListener* listener) { _listener = listener; }
	void SetProgram(ProgramInfo* pi);
	void Reset(){_pes_header_parsed = false;}
	void ResetValidTimeStamp(){m_llLastValidTimeStamp = 0xFFFFFFFFFFFFFFFFLL;}

	virtual void OnRawPacket(RawPacket* packet);

private:
	IPESListener* _listener;
	ProgramInfo* _pi;
	unsigned long long     m_llLastValidTimeStamp;
	bool		 _pes_header_parsed;

protected:
	void ParsePESPacket(RawPacket* packet, ESConfig* escfg);
	void Parse14496Section(RawPacket* packet, ESConfig* escfg);
	void ParseODStream(uint8* data, uint32 len);
};

class PESDataHandler : public ElementExtension
{
public:
	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet) = 0;
	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData) = 0;
};


class PESDataDispatcher : public IPESListener
{
public:
	virtual void OnElementStream(ESConfig* escfg) { }
	
	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet)
	{
		if (escfg->extension)
			((PESDataHandler*)escfg->extension)->OnPESHead(escfg, packet);

	}

	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData)
	{
		//if (IsBadReadPtr(escfg, sizeof(ESConfig))) return;
		//if (IsBadReadPtr(escfg->extension, sizeof(ElementExtension))) return;
		if (escfg->extension)
		{
			//if (IsBadCodePtr(((PESDataHandler*)escfg->extension)->OnPESData)) return;
			((PESDataHandler*)escfg->extension)->OnPESData(escfg, pData, cbData);
		}
	}
};


class TDTParser	
	: public IRawListener
	, public IPSIListener
{
public:
	TDTParser();
	virtual void OnRawPacket(RawPacket* packet);
	virtual void OnPSI(PSI* psi);
	void SetListener(ITDTListener* listener) { _listener = listener; }

private:
	ITDTListener* _listener;
	PSIParser _PSIParser;
};


class PSIPParser	
	: public IRawListener
	, public IPSIListener
{
public:
	PSIPParser();
	virtual void OnRawPacket(RawPacket* packet);
	virtual void OnPSI(PSI* psi);
	void SetListener(ISDTListener* listener) { _listener = listener; }

private:
	ISDTListener* _listener;
	PSIParser _PSIParser;
};


} //namespace TS

#ifdef _VONAMESPACE
}
#endif


#endif //_TS_BASIC_PARSER_H_
