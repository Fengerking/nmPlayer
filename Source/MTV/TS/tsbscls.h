#ifndef _TS_BASE_CLASS_H_
#define _TS_BASE_CLASS_H_

#include "voYYDef_TS.h"
#include "vobstype.h"
#include "readutil.h"
//#include "logger.h"
#include "mp4desc.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

namespace TS {

class Object
{
public:
	virtual ~Object() {}
	virtual bool Load(BitStream& is, void* pEnd) = 0;
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os) = 0;
#endif //ENABLE_LOG

};


class Item : public Object
{
public:
	typedef Item* (*CREATOR)(BitStream& is, void* pEnd);
	static Item* LoadAll(BitStream& is, void* pEnd, CREATOR creat);
#ifdef ENABLE_LOG
	void DumpAll(Logger& os);
#endif //ENABLE_LOG

public:
	Item();
	virtual ~Item();

	int GetCount() const;

public:
	Item*  more;
};

class descriptor : public Item
{
// Properties
public:
	bit8 tag;
	bit8 length;

public:
	descriptor* FindFirst(bit8 tag);

	static Item* Create(BitStream& is, void* pEnd);
	static descriptor* LoadAll(BitStream& is, void* pEnd) { return (descriptor*) Item::LoadAll(is, pEnd, Create); }

private:
	static descriptor* CreateDescriptorByTag(bit8 tag);
};


class unknown_descriptor
: public descriptor
{
public:
	unknown_descriptor();
	virtual ~unknown_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	uint8* body;
};

struct SLPacketHeader
{
	bit1 accessUnitStartFlag;
	bit1 accessUnitEndFlag;
	bit1 OCRflag;
	bit1 idleFlag;
	bit1 paddingFlag;
	bit3 paddingBits;

	bit32 packetSequenceNumber;
	bit1 DegPrioflag;
	bit32 degradationPriority;
	bit64 objectClockReference;
	bit1 randomAccessPointFlag;
	bit32 AU_sequenceNumber;
	bit1 decodingTimeStampFlag;
	bit1 compositionTimeStampFlag;
	bit1 instantBitrateFlag;
	bit64 decodingTimeStamp;
	bit64 compositionTimeStamp;
	bit32 accessUnitLength;
	bit32 instantBitrate;

	SLPacketHeader();
	uint8* Parse(uint8* data, uint32 size, MP4::SLConfigDescriptor* SLCD);

};

/*
IOD_descriptor () {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
Scope_of_IOD_label 8 uimsbf
IOD_label 8 uimsbf
InitialObjectDescriptor ()
}
*/

class IOD_descriptor
: public descriptor
{
public:
	IOD_descriptor();
	virtual ~IOD_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit8 Scope_of_IOD_label;
	bit8 IOD_label;
	MP4::InitialObjectDescriptor* IOD;
};


/*
AC3_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
component_type_flag 1 bslbf
bsid_flag 1 bslbf
mainid_flag 1 bslbf
asvc_flag 1 bslbf
reserved_flags 4 bslbf
if (component_type_flag == "1"){
component_type 8 uimsbf
}
if (bsid_flag == "1"){
bsid 8 uimsbf
}
if (mainid_flag == "1"){
mainid 8 uimsbf
}
if (asvc_flag == "1"){
asvc 8 uimsbf
}
for(i=0;i<N;i++){
additional_info 8 uimsbf length-??
}
}
*/

class AC3_descriptor
: public descriptor
{
public:
	AC3_descriptor();
	virtual ~AC3_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit1 component_type_flag;
	bit1 bsid_flag;
	bit1 mainid_flag;
	bit1 asvc_flag;
	bit8 component_type;
	bit8 bsid;
	bit8 mainid;
	bit8 asvc;
	bit8* additional_info;
	int additional_info_len;
};


} //namespace TS

#ifdef _VONAMESPACE
}
#endif
#endif // _TS_BASE_CLASS_H_