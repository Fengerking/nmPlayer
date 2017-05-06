/**
 * ISO Media Boxes
 * @created 12/8/2004
 */

#ifndef _ISOM_BOX_H
#define _ISOM_BOX_H

#include <stdio.h>
#ifdef _XMLDUMP
#include "../MP4Base/xmlwr.h"
#endif //_XMLDUMP


#include "../MP4Base/mpxobj.h"
#include "../MP4Base/mpxio.h"

#include "voType.h"
#include "../Common/fCC2.h"

//using namespace MPx;

//====================================================================

//namespace ISOM {

//class TrackFromBoxBuilder;
//class MovieFromBoxBuilder;

class Box : public Container
{
public:
	enum BoxType
	{
		btUUID = FOURCC2_uuid,
		btFILE = FOURCC2_file, //a virtual box
		btFTYP = FOURCC2_ftyp,
		btMOOV = FOURCC2_moov,
		btMVHD = FOURCC2_mvhd,
		btTRAK = FOURCC2_trak,
		btTKHD = FOURCC2_tkhd,
		btTREF = FOURCC2_tref,
		btEDTS = FOURCC2_edts,
		btELST = FOURCC2_elst,
		btMDIA = FOURCC2_mdia,
		btMDHD = FOURCC2_mdhd,
		btHDLR = FOURCC2_hdlr,
		btMINF = FOURCC2_minf,
		btVMHD = FOURCC2_vmhd,
		btSMHD = FOURCC2_smhd,
		btHMHD = FOURCC2_hmhd,
		btNMHD = FOURCC2_nmhd,
		btDINF = FOURCC2_dinf,
		btDREF = FOURCC2_dref,
		btURL  = FOURCC2_url ,
		btURN  = FOURCC2_urn ,
		btSTBL = FOURCC2_stbl,
		btSTSD = FOURCC2_stsd,
		btSTTS = FOURCC2_stts,
		btCTTS = FOURCC2_ctts,
		btSTSC = FOURCC2_stsc,
		btSTSZ = FOURCC2_stsz,
		btSTZ2 = FOURCC2_stz2,
		btSTCO = FOURCC2_stco,
		btCO64 = FOURCC2_co64,
		btSTSS = FOURCC2_stss,
		btSTSH = FOURCC2_stsh,
		btPADB = FOURCC2_padb,
		btSTDP = FOURCC2_stdp,
		btMVEX = FOURCC2_mvex,
		btMEHD = FOURCC2_mehd,
		btTREX = FOURCC2_trex,
		btMOOF = FOURCC2_moof,
		btMFHD = FOURCC2_mfhd,
		btTRAF = FOURCC2_traf,
		btTFHD = FOURCC2_tfhd,
		btTRUN = FOURCC2_trun,
		btMFRA = FOURCC2_mfra,
		btTFRA = FOURCC2_tfra,
		btMFRO = FOURCC2_mfro,
		btMDAT = FOURCC2_mdat,
		btFREE = FOURCC2_free,
		btSKIP = FOURCC2_skip,
		btUDTA = FOURCC2_udta,
		btCPRT = FOURCC2_cprt,
		btAVCC = FOURCC2_avcC,
	};

private:
	static Box* CreateBoxByType(uint32 type);

public:
	static Box* Load(Reader& r);

public:
	Box(uint32 boxtype=0);
	virtual ~Box();
	void Init(const Box& b);

	uint32 GetType() const;

protected:
	uint32 type;
	//uint64 largesize;
	//Uuid uuid;

	uint32 selfsize;  //for LoadBase, LoadExt

// Load
//------------------------------------------------
public:
	virtual bool LoadChildren(Reader& r);
	bool LoadBase(Reader& r); //! Cannot overload
	virtual bool LoadExt(Reader& r);

protected:
	bool SkipLoadExt(Reader& r)
	{
		bool b = r.Move(GetSize() - selfsize);
		selfsize = GetSize();
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

#ifdef _XMLDUMP
// Dump XML
//------------------------------------------------
public:
	bool Dump(XmlWriter& w) const;
	virtual bool DumpBase(XmlWriter& w) const;
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP


// Tree
//------------------------------------------------
public:
	Box* FirstChild() 
	{ 
		return (Box*)Container::FirstChild(); 
	}

	Box* NextChild(Box* current)
	{
		return (Box*)Container::NextChild(current);
	}

	Box* LastChild()
	{
		return (Box*)Container::LastChild();
	}

	virtual void AddChild(Box* b)
	{
		Container::AddChild(b);
	}


// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

};


class FullBox : public Box
{
public:
	FullBox(uint32 boxtype=0, uint8 v=0, uint32 f=0);

	virtual bool LoadExt(Reader& r);

#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint8 version;
	uint32 flags;

//Note!!
// DumpBase is not a counterpart to LoadBase
// DumpExt is not a counterpart to LoadExt
// because of Box::Load & Box::CreateBoxByType, LoadBase not overloaded

// Dump
//------------------------------------------------
public:
	virtual bool DumpBase(Writer& w) const;
	//virtual bool DumpExt(Writer& w) const;
	virtual uint32 CalcBaseSize() const;
	//virtual uint32 CalcExtSize() const;
};


class BoxWithoutChild : public Box
{
public:
	virtual bool LoadExt(Reader& r);
};


class FullBoxWithoutChild : public FullBox
{
public:
	virtual bool LoadExt(Reader& r);
};


class MdatBox : public Box
{
public:
	virtual bool LoadExt(Reader& r);
};

//====================================================================

class FileBox : public Box
{
public:
	FileBox();
	virtual ~FileBox();

	bool Load(Reader& r, uint32 fsize);

protected:
	Box* mdat;

public:
	virtual void AddChild(Box* b)
	{
		if (b->GetType() == btMDAT)
			mdat = b;
		Box::AddChild(b);
	}

public:
	Box* GetMdatBox() { return mdat; }
};

//====================================================================

#define compatible_brands_max		16
class FileTypeBox : public Box
{
public:
	FileTypeBox();
	virtual ~FileTypeBox();

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint32 major_brand;
	uint32 minor_version;
	uint32 compatible_brands[compatible_brands_max];
	uint32 compatible_brands_count;

public:
	void SetMajorBrand(uint32 b) { major_brand = b; }
	void ClearCompatibleBrands() { compatible_brands_count = 0; }
	bool AddCompatibleBrand(uint32 b)
	{
		if (compatible_brands_count >= compatible_brands_max)
			return false;
		else
			compatible_brands[compatible_brands_count++] = b;
		return true;
	}


// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};


//====================================================================

class MovieHeaderBox : public FullBox
{
public:
	MovieHeaderBox(uint8 v=0, uint32 f=0);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	union
	{
		struct
		{
			uint64 creation_time;
			uint64 modification_time;
			uint64 duration;
		} v1;
		struct
		{
			uint32 creation_time;
			uint32 modification_time;
			uint32 duration;
		} v0;
	};
	uint32 timescale;
	int32 rate;
	int16 volume;
	int32 matrix[9];
	uint32 pre_defined[6];
	uint32 next_track_ID;

public:
	void SetNextTrackID(uint32 id) { next_track_ID = id; }
	void UpdateTimeStamp();
	void SetDuration(uint64 dur, uint32 scale);

	uint32 GetTimeScale() const { return timescale; }
	uint64 GetDuration() const { return version == 1 ? v1.duration : v0.duration; }

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

	//friend class TrackFromBoxBuilder;

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};

//====================================================================

class TrackHeaderBox : public FullBox
{
public:
	TrackHeaderBox(uint8 v=0, uint32 f=0x000001);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	union
	{
		struct
		{
			uint64 creation_time;
			uint64 modification_time;
			//uint32 track_ID;
			uint64 duration;
		} v1;
		struct
		{
			uint32 creation_time;
			uint32 modification_time;
			//uint32 track_ID;
			uint32 duration;
		} v0;
	};
	uint32 track_ID;
	int16 layer;
	int16 alternate_group;
	int16 volume;
	int32 matrix[9];
	uint32 width;
	uint32 height;
	
public:
	void SetTrackID(uint32 id) { track_ID = id; }
	void SetVolume(int16 v) { volume = v; }
	void SetVideoSize(double w, double h);
	void UpdateTimeStamp();

	void SetDuration(uint64 dur)
	{
		if (version == 1)
			v1.duration = dur;
		else
			v0.duration = (uint32)dur;
	}

	uint64 GetDuration() const
	{
		if (version == 1)
			return v1.duration;
		else
			return v0.duration;
	}

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

	//friend class TrackFromBoxBuilder;

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};



//====================================================================

class MediaHeaderBox : public FullBox
{
public:
	MediaHeaderBox(uint8 v=0, uint32 f=0);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	union
	{
		struct
		{
			uint64 creation_time;
			uint64 modification_time;
			uint64 duration;
		} v1;
		struct
		{
			uint32 creation_time;
			uint32 modification_time;
			uint32 duration;
		} v0;
	};
	uint32 timescale;
	int16 language;
	int16 pre_defined;

public:
	void UpdateTimeStamp();
	void SetDuration(uint64 dur, uint32 scale);
	uint64 GetDuration() const { return version == 1 ? v1.duration : v0.duration; }
	uint32 GetTimeScale() const { return timescale; }

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};


//====================================================================

class HandlerBox : public FullBox
{
public:
	HandlerBox(uint8 v=0, uint32 f=0);
	virtual ~HandlerBox();

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint32 pre_defined;
	uint32 handler_type;
	char* name;

public:
	uint32 GetHandlerType() const { return handler_type; }
	void SetHandlerType(uint32 t) { handler_type = t; }

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};


//====================================================================

class EditListBox : public FullBox
{
public:
	EditListBox(uint8 v=0, uint32 f=0);
	virtual ~EditListBox();

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

public:
	void SetEntryCount(int count);
	bool SetEntry(int index, uint32 dur, int32 time);

public:
	struct V1
	{
		uint64 segment_duration;
		int64 media_time;
	};
	struct V0
	{
		uint32 segment_duration;
		int32 media_time;
	};

protected:
	uint32 entry_count;
	union
	{
		V1* v1;
		V0* v0;
	};
	int16 media_rate_integer;
	int16 media_rate_fraction;

	void Release();
};

//====================================================================

class VideoMediaHeaderBox : public FullBox
{
public:
	VideoMediaHeaderBox(uint8 v=0, uint32 f=1);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint16 graphicsmode;
	uint16 opcolor[3];

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};

//====================================================================

class SoundMediaHeaderBox : public FullBox
{
public:
	SoundMediaHeaderBox(uint8 v=0, uint32 f=0);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	int16 balance;

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};

//====================================================================

class HintMediaHeaderBox : public FullBox
{
public:
	HintMediaHeaderBox(uint8 v=0, uint32 f=0);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint16 maxPDUsize;
	uint16 avgPDUsize;
	uint32 maxbitrate;
	uint32 avgbitrate;
// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};

//====================================================================

class NullMediaHeaderBox : public FullBox
{
public:
	NullMediaHeaderBox(uint8 v=0, uint32 f=0);

	//virtual bool LoadExt(Reader& r);
	//virtual bool DumpExt(XmlWriter& w) const;

protected:

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	//virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	//virtual uint32 CalcExtSize() const;

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};

//====================================================================

class DataReferenceBox : public FullBox
{
public:
	DataReferenceBox(uint8 v=0, uint32 f=0);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint32 entry_count;

public:
	void SetEntryCount(uint32 c) { entry_count = c; }

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

};

//====================================================================

class SampleDescriptionBox : public FullBox
{
public:
	SampleDescriptionBox(uint8 v=0, uint32 f=0);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint32 entry_count;

public:
	void SetEntryCount(uint32 c) { entry_count = c; }

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

};

//====================================================================

class SampleEntry : public Box
{
public:
	SampleEntry(uint32 fmt=0);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint16 data_reference_index;

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};


//====================================================================

class HintSampleEntry : public SampleEntry
{
public:
	virtual bool LoadExt(Reader& r)
	{
		if (!SampleEntry::LoadExt(r))
			return false;
		return SkipLoadExt(r);
	}

};

//====================================================================

class VisualSampleEntry : public SampleEntry
{
public:
	VisualSampleEntry(uint32 codingname=0);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint16 width;
	uint16 height;
	uint32 horizresolution;
	uint32 vertresolution;
	uint16 frame_count;
	char compressorname[32];
	uint16 depth;
	int16 pre_defined;

public:
	uint16 GetWidth() const { return width; }
	uint16 GetHeight() const { return height; }
	void SetWidth(uint16 w) { width = w; }
	void SetHeight(uint16 h) { height = h; }

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};

//====================================================================

class AudioSampleEntry : public SampleEntry
{
public:
	AudioSampleEntry(uint32 codingname=0);

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint16 channelcount;
	uint16 samplesize;
	//uint32 samplerate;
	uint16 samplerate; //should be 16.16 fix point number, 11/29/2005

public:
	uint16 GetSampleRate() const { return samplerate; }
	void SetSampleRate(uint16 r) { samplerate = r; }
	uint16 GetChannelCount() const { return channelcount; }
	void SetChannelCount(uint16 ch) { channelcount = ch; }
	uint16 GetSampleSize() const { return samplesize; }
	void SetSampleSize(uint16 ss) { samplesize = ss; }

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};


//====================================================================

class SyncSampleBox : public FullBox
{
public:
	SyncSampleBox(uint8 v=0, uint32 f=0);
	virtual ~SyncSampleBox();

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint32 entry_count;
	uint32* sample_number;

	void Release();

public:
	uint32 EntryCount() const { return entry_count; }
	uint32 SampleNumber(uint32 i) const { return sample_number[i]; }

	void SetMaxEntryCount(uint32 c);

	void AddSyncSample(uint32 i) { sample_number[entry_count++] = i; }


// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb); //5/9/2005

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};

//====================================================================

class SampleToChunkBox : public FullBox
{
public:
	SampleToChunkBox(uint8 v=0, uint32 f=0);
	virtual ~SampleToChunkBox();
	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP

public:
	void SetMaxEntryCount(uint32 c);
	bool AddEntry(uint32 chunk, uint32 spc, uint32 sdi);

protected:
	uint32 entry_count;
	struct Entry
	{
		uint32 first_chunk;
		uint32 samples_per_chunk;
		uint32 sample_description_index;
	};

	Entry* entries;

	void Release();

public:
	uint32 FirstChunk(uint32 i) { return entries[i].first_chunk; }
	uint32 SamplesPerChunk(uint32 i) { return entries[i].samples_per_chunk; }
	uint32 SampleDescriptionIndex(uint32 i) { return entries[i].sample_description_index; }

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

	//friend class TrackFromBoxBuilder;

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};

//====================================================================

class SampleSizeBox : public FullBox
{
public:
	SampleSizeBox(uint8 v=0, uint32 f=0);
	virtual ~SampleSizeBox();

	void SetSampleCount(uint32 count);
	void SetEntry(uint32 i, uint32 size)
	{
		entry_size[i] = size;
	}

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint32 sample_size;
	uint32 sample_count;
	uint32* entry_size;

	void Release();

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

	//friend class TrackFromBoxBuilder;

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};

//====================================================================

class ChunkOffsetBox : public FullBox
{
public:
	ChunkOffsetBox(uint8 v=0, uint32 f=0);
	virtual ~ChunkOffsetBox();
	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP

	void SetEntryCount(uint32 c);
	void SetEntry(uint32 i, uint32 offset)
	{
		chunk_offset[i] = offset;
	}

protected:
	uint32 entry_count;
	uint32* chunk_offset;

	void Release();

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

	//friend class TrackFromBoxBuilder;

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};

//====================================================================

class CopyrightBox : public FullBox
{
public:
	CopyrightBox();
	virtual ~CopyrightBox();
	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	int16 language;
	char* notice;
};

//UnknownBox
//5/18/2005
//====================================================================

class UnknownBox : public Box
{
public:
	UnknownBox(uint32 boxtype=0);
	virtual ~UnknownBox();

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
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

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};

//====================================================================

class UnknownFullBox : public FullBox
{
public:
	UnknownFullBox(uint32 boxtype=0, uint8 v=0, uint32 f=0);
	virtual ~UnknownFullBox();

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
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

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};

//====================================================================

class TimeToSampleBox : public FullBox
{
public:
	TimeToSampleBox(uint8 v=0, uint32 f=0);
	virtual ~TimeToSampleBox();
	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP

public:
	void SetMaxEntryCount(uint32 c);
	bool AddEntry(uint32 delta);
	bool AddEntries(uint32 delta, uint32 count);

protected:
	uint32 entry_count;
	struct Entry
	{
		uint32 sample_count;
		uint32 sample_delta;
	};

	Entry* entries;

	void Release();

public:
	uint32 EntryCount() { return entry_count; }
	uint32 SampleCount(uint32 i) { return entries[i].sample_count; }
	uint32 SampleDelta(uint32 i) { return entries[i].sample_delta; }

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

	//friend class TrackFromBoxBuilder;

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};

//DescriptorBox
//5/18/2005
//====================================================================

class DescriptorBox : public UnknownBox
{
public:
	DescriptorBox(uint32 boxtype=0);

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};


//====================================================================

class ESDBox : public UnknownFullBox
{
public:
	ESDBox(uint8 v=0, uint32 f=0);

#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);
};


//====================================================================

class MovieBox : public Box
{
public:
	virtual bool LoadChildren(Reader& r);
};


#if 0 //DataBox Removed

//DataBox
//4/27/2006
//====================================================================

class DataBox : public Box
{
public:
	DataBox(uint32 boxtype=0);
	virtual ~DataBox();

	virtual bool LoadExt(Reader& r);
#ifdef _XMLDUMP
	virtual bool DumpExt(XmlWriter& w) const;
#endif //_XMLDUMP

protected:
	uint8 version;
	uint32 flags;
	uint32 datasize;
	uint8* data;

	void Release();

public:
	uint32 GetDataSize() const { return datasize; }
	uint8* GetData() const { return data; }
	void SetData(uint8* d, uint32 s);

	uint32 GetDataInt() const;
	char* GetDataText(char* buf, int bufsize) const;

// Visit
//------------------------------------------------
public:
	//virtual void Visit(MovieFromBoxBuilder& mb);

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	virtual uint32 CalcExtSize() const;
};

#endif //DataBox Removed

//====================================================================


class FreeBox : public Box
{
public:
	FreeBox();

// Dump
//------------------------------------------------
public:
	//virtual bool DumpBase(Writer& w) const;
	virtual bool DumpExt(Writer& w) const;
	//virtual uint32 CalcBaseSize() const;
	//virtual uint32 CalcExtSize() const;
};


//====================================================================

//} //namespace ISOM


#endif // _ISOM_BOX_H
