/**
 * MP4 Descriptors
 * @created 1/5/2005
 */

#ifndef _MP4_DESC_H
#define _MP4_DESC_H

#include <stdio.h>
#ifdef _XMLDUMP
#include "xmlwr.h"
#endif //_XMLDUMP


#include "mpxobj.h"
#include "mpxio.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
//using namespace MPx;

//====================================================================

//namespace MP4 {

//ISO/IEC 14496-1:2004(E), p19
//0x00 Forbidden
const uint8 ObjectDescrTag                      = 0x01;
const uint8 InitialObjectDescrTag               = 0x02;
const uint8 ES_DescrTag                         = 0x03;
const uint8 DecoderConfigDescrTag               = 0x04;
const uint8 DecSpecificInfoTag                  = 0x05;
const uint8 SLConfigDescrTag                    = 0x06;
const uint8 ContentIdentDescrTag                = 0x07;
const uint8 SupplContentIdentDescrTag           = 0x08;
const uint8 IPI_DescrPointerTag                 = 0x09;
const uint8 IPMP_DescrPointerTag                = 0x0A;
const uint8 IPMP_DescrTag                       = 0x0B;
const uint8 QoS_DescrTag                        = 0x0C;
const uint8 RegistrationDescrTag                = 0x0D;
const uint8 ES_ID_IncTag                        = 0x0E;
const uint8 ES_ID_RefTag                        = 0x0F;
const uint8 MP4_IOD_Tag                         = 0x10;
const uint8 MP4_OD_Tag                          = 0x11;
const uint8 IPL_DescrPointerRefTag              = 0x12;
const uint8 ExtensionProfileLevelDescrTag       = 0x13;
const uint8 profileLevelIndicationIndexDescrTag = 0x14;
//0x15-0x3F Reserved for ISO use
const uint8 ContentClassificationDescrTag       = 0x40;
const uint8 KeyWordDescrTag                     = 0x41;
const uint8 RatingDescrTag                      = 0x42;
const uint8 LanguageDescrTag                    = 0x43;
const uint8 ShortTextualDescrTag                = 0x44;
const uint8 ExpandedTextualDescrTag             = 0x45;
const uint8 ContentCreatorNameDescrTag          = 0x46;
const uint8 ContentCreationDateDescrTag         = 0x47;
const uint8 OCICreatorNameDescrTag              = 0x48;
const uint8 OCICreationDateDescrTag             = 0x49;
const uint8 SmpteCameraPositionDescrTag         = 0x4A;
const uint8 SegmentDescrTag                     = 0x4B;
const uint8 MediaTimeDescrTag                   = 0x4C;
//0x4D-0x5F Reserved for ISO use (OCI extensions)
const uint8 IPMP_ToolsListDescrTag              = 0x60;
const uint8 IPMP_ToolTag                        = 0x61;
const uint8 M4MuxTimingDescrTag                 = 0x62;
const uint8 M4MuxCodeTableDescrTag              = 0x63;
const uint8 ExtSLConfigDescrTag                 = 0x64;
const uint8 M4MuxBufferSizeDescrTag             = 0x65;
const uint8 M4MuxIdentDescrTag                  = 0x66;
const uint8 DependencyPointerTag                = 0x67;
const uint8 DependencyMarkerTag                 = 0x68;
const uint8 M4MuxChannelDescrTag                = 0x69;
//0x6A-0xBF Reserved for ISO use
//0xC0-0xFE User private
//0xFF Forbidden


class Descriptor : public Container
{
private:
	static Descriptor* CreateDescriptorByTag(uint8 aTag);

public:
	static Descriptor* Load(Reader& r);
	static Descriptor* Load(uint8* data, uint32 size);

public:
	Descriptor(uint8 aTag=0);
	virtual ~Descriptor();
	void Init(const Descriptor& b);

	uint8 GetTag() const { return tag; }

protected:
	uint8 tag;

	uint32 loadedsize;  //for LoadBase, LoadExt

// Load
//------------------------------------------------
public:
	int LoadChildren(Reader& r);
	bool LoadBase(Reader& r); //! Cannot overload
	virtual bool LoadExt(Reader& r);

protected:
	bool SkipLoad(Reader& r)
	{
		bool b = r.Move(GetSize() - loadedsize);
		loadedsize = GetSize();
		return b;
	}


// Dump
//------------------------------------------------
public:
	virtual bool Dump(Writer& w) const;
	virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;

	void UpdateSize();
	virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

// Dump XML
//------------------------------------------------
#ifdef _XMLDUMP
public:
	virtual const char* GetXmlTag() const { return "Descriptor"; }
	bool Dump(XmlWriter& w) const;
	virtual bool DumpBase(XmlWriter& w) const;
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP


// Tree
//------------------------------------------------
public:
	Descriptor* FirstChild() 
	{ 
		return (Descriptor*)Container::FirstChild(); 
	}

	Descriptor* NextChild(Descriptor* current)
	{
		return (Descriptor*)Container::NextChild(current);
	}

	Descriptor* LastChild()
	{
		return (Descriptor*)Container::LastChild();
	}

	virtual void AddChild(Descriptor* b)
	{
		Container::AddChild(b);
	}

	Descriptor* GetChildByTag(uint8 t);
	Descriptor* GetChildByIndex(int i);

// Util
//------------------------------------------------
public:
	static bool ReadMpegSize(Reader& r, uint32& s);
	static bool WriteMpegSize(Writer& w, uint32 s);
};


//====================================================================

class DescriptorWithoutChild : public Descriptor
{
public:
	virtual bool LoadExt(Reader& r)
	{
		return SkipLoad(r);
	}

#ifdef _XMLDUMP
public:
	virtual const char* GetXmlTag() const { return "DescriptorWithoutChild"; }
#endif //_XMLDUMP
};


//====================================================================

class UnknownDescriptor : public Descriptor
{
public:
	UnknownDescriptor(uint8 aTag=0);
	virtual ~UnknownDescriptor();

public:
	virtual bool LoadExt(Reader& r);

#ifdef _XMLDUMP
	virtual const char* GetXmlTag() const { return "UnknownDescriptor"; }
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint32 bodysize;
	uint8* bodydata;

	void Release();

public:
	uint32 GetBodySize() const { return bodysize; }
	uint8* GetBodyData() const { return bodydata; }
	void SetBody(uint8* d, uint32 s);

	virtual uint32 CalcExtSize() const;
	virtual bool DumpExt(Writer& w) const;

};

//====================================================================

class ESDescriptor : public Descriptor
{
public:
	ESDescriptor();
	virtual ~ESDescriptor();

public:
	uint16 ES_ID;
	bit1 streamDependenceFlag;
	bit1 URL_Flag;
	bit1 OCRstreamFlag;
	bit5 streamPriority;

	uint16 dependsOn_ES_ID;
	uint8 URLlength;
	char* URLstring;
	uint16 OCR_ES_Id;

	bool GetDecoderSpecificData(uint8* buf, int* bufsize);

public:
	virtual bool LoadExt(Reader& r);

#ifdef _XMLDUMP
	virtual const char* GetXmlTag() const { return "ESDescriptor"; }
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

	virtual uint32 CalcExtSize() const;
	virtual bool DumpExt(Writer& w) const;
};


//====================================================================

class DecoderConfigDescriptor : public Descriptor
{
public:
	DecoderConfigDescriptor();
	virtual ~DecoderConfigDescriptor();

public:
	uint8 objectTypeIndication;
	bit6 streamType;
	bit1 upStream;
	uint32 bufferSizeDB;
	uint32 maxBitrate;
	uint32 avgBitrate;
	//DecoderSpecificInfo decSpecificInfo[0 .. 1];
	//profileLevelIndicationIndexDescriptor profileLevelIndicationIndexDescr [0..255];

public:
	virtual bool LoadExt(Reader& r);

#ifdef _XMLDUMP
	virtual const char* GetXmlTag() const { return "DecoderConfigDescriptor"; }
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

	virtual uint32 CalcExtSize() const;
	virtual bool DumpExt(Writer& w) const;
};

//====================================================================

class DecoderSpecificInfo : public UnknownDescriptor
{
public:
	DecoderSpecificInfo() : UnknownDescriptor(DecSpecificInfoTag) {}

#ifdef _XMLDUMP
	virtual const char* GetXmlTag() const { return "DecoderSpecificInfo"; }
#endif //_XMLDUMP
};

//====================================================================

class SLConfigDescriptor : public Descriptor
{
public:
	SLConfigDescriptor();
	virtual ~SLConfigDescriptor();

public:
	uint8 predefined;
	uint8 flags;
	uint32 timeStampResolution;
	uint32 OCRResolution;
	uint8 timeStampLength;
	uint8 OCRLength;
	uint8 AU_Length;
	uint8 instantBitrateLength;
	bit4 degradationPriorityLength;
	bit5 AU_seqNumLength;
	bit5 Packet_SeqNumLength;

public:
	bool useAccessUnitStartFlag() const { return (flags & 0x80)? true : false; }
	bool useAccessUnitEndFlag() const { return (flags & 0x40)? true : false; }
	bool useRandomAccessPointFlag() const { return (flags & 0x20)? true : false; }
	bool hasRandomAccessUnitsOnlyFlag() const { return (flags & 0x10)? true : false; }
	bool usePaddingFlag() const { return (flags & 0x08)? true : false; }
	bool useTimeStampsFlag() const { return (flags & 0x04)? true : false; }
	bool useIdleFlag() const { return (flags & 0x02)? true : false; }
	bool durationFlag() const { return (flags & 0x01)? true : false; }

public:
	virtual bool LoadExt(Reader& r);

#ifdef _XMLDUMP
	virtual const char* GetXmlTag() const { return "SLConfigDescriptor"; }
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

	virtual uint32 CalcExtSize() const;
	virtual bool DumpExt(Writer& w) const;
};

//====================================================================

class InitialObjectDescriptor : public Descriptor
{
public:
	InitialObjectDescriptor();
	virtual ~InitialObjectDescriptor();

public:
	uint16 ObjectDescriptorID;
	uint8 ODProfileLevelIndication;
	uint8 sceneProfileLevelIndication;
	uint8 audioProfileLevelIndication;
	uint8 visualProfileLevelIndication;
	uint8 graphicsProfileLevelIndication;

public:
	virtual bool LoadExt(Reader& r);

#ifdef _XMLDUMP
	virtual const char* GetXmlTag() const { return "InitialObjectDescriptor"; }
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

	virtual uint32 CalcExtSize() const;
	virtual bool DumpExt(Writer& w) const;

public:
	ESDescriptor* FindESD(uint16 esid);
};


//====================================================================

class ObjectDescriptor : public Descriptor
{
public:
	ObjectDescriptor();
	virtual ~ObjectDescriptor();

public:
	uint16 ObjectDescriptorID;

public:
	virtual bool LoadExt(Reader& r);

#ifdef _XMLDUMP
	virtual const char* GetXmlTag() const { return "ObjectDescriptor"; }
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

	virtual uint32 CalcExtSize() const;
	virtual bool DumpExt(Writer& w) const;

public:
	ESDescriptor* FindESD(uint16 esid);
};


//====================================================================

//} //namespace MP4
#ifdef _VONAMESPACE
}
#endif
#endif // _MP4_DESC_H
