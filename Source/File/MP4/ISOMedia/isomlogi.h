/**
 * ISO Media logical objects
 * @created 12/17/2004
 */

#ifndef _ISOM_LOGI_H
#define _ISOM_LOGI_H

#include "../MP4Base/mpxobj.h"
#include "../MP4Base/mpxio.h"
#include "isombox.h"

//#define _MIN_MEM //Min memory usage, only for parse/reader
#pragma pack(push, 4)

//namespace ISOM {

const int TIME_DIVISOR = 90000;
#ifdef _XMLDUMP
const double TIME_UNIT = 1.0 / TIME_DIVISOR;
#endif //_XMLDUMP


class Sample;
class Chunk;
class Track;

//====================================================================

class Sample
#ifdef _XMLDUMP
	: public XmlDump
#endif //_XMLDUMP
{
public:
	Sample();
	~Sample();

protected:
#ifndef _MIN_MEM
	uint32 address;
#endif //_MIN_MEM
	//uint32 size;
	//uint8 attrib;
	uint32 size_attrib; // merge these two fields to save memory
	//float timestamp;
	uint32 timestamp; //unit ms
	//double duration;

protected:
	enum
	{
		ATT_SYNC = 0x80000000,
		ATT_NOT_SYNC = 0x7fffffff,
		SIZE_MASK = 0x7fffffff,
		NONSIZE_MASK = 0x80000000
	};

public:
#ifdef _MIN_MEM
	uint32 GetAddress() const; // { return address; }
	void SetAddress(uint32 a) {} //{ address = a; }
#else //_MIN_MEM
	uint32 GetAddress() const { return address; }
	void SetAddress(uint32 a) { address = a; }
#endif //_MIN_MEM
	//uint32 GetSize() const { return size; }
	//void SetSize(uint32 s) { size = s;	}

	void SetTime(uint32 t) 
	{ 
#ifdef _MY_LOG
		//fprintf(MyLogFile, "time: %f\n", t);
#endif //_MY_LOG
		timestamp = t; 
	}

	uint32 GetTime() const { return timestamp; }
	//void SetDuration(double dur) { duration = dur; }
	//double GetDuration() const { return duration; }

public:
	Sample* NextSample() { return this + 1; } //!!! must store samples in array
	const Sample* NextSample() const { return this + 1; } //!!! must store samples in array
	void SetDuration(uint32 dur) 
	{ 
#ifdef _MY_LOG
		//fprintf(MyLogFile, " dur: %f   \t", dur);
#endif //_MY_LOG
		NextSample()->SetTime(GetTime() + dur);
	}
	uint32 GetDuration() const 
	{ 
		return NextSample()->GetTime() - GetTime();
	}

public:
	uint32 GetSize() const
	{
		return size_attrib & SIZE_MASK;
	}
	void SetSize(uint32 s)
	{
		size_attrib &= NONSIZE_MASK;
		size_attrib |= s;  //!!! s <= SIZE_MASK
	}
	bool IsSync() const { return (size_attrib & ATT_SYNC) > 0 ? true : false ; }
	void SetSync(bool b=true) 
	{ 
		if (b) 
			size_attrib |= ATT_SYNC;
		else 
			size_attrib &= ATT_NOT_SYNC; 
	}

protected:
	Chunk* chunk;
	//int chunkindex; //chunk index in track

public:
	Chunk* GetChunk() const { return chunk; }
	void SetChunk(Chunk* c) { chunk = c; }
	//inline Chunk* GetChunk() const;
	//void SetChunk(int cindex) { chunkindex = cindex; }
	inline Track* GetTrack() const;
	inline uint32 GetTrackID() const;

#ifdef _DEBUG
public:
	static unsigned int AliveObjects;
	static unsigned int MaxObjects;
#endif //_DEBUG

#ifdef _XMLDUMP
public:
	virtual const char* GetXmlTag() const { return "sample"; }
	virtual bool DumpAttributes(XmlWriter& w) const;
#endif //_XMLDUMP

public:
	void Reset() { chunk = 0; }  // 7/14/2006
};


//====================================================================

class Chunk
#ifdef _XMLDUMP
	: public XmlDump
#endif //_XMLDUMP
{
public:
	Chunk();
	~Chunk();

protected:
	uint32 address;

public:
	uint32 GetAddress() const { return address; }
	void SetAddress(uint32 a) { address = a; }

#ifdef _MIN_MEM
	uint32 GetSize() const { return 0; }
	void SetSize(uint32 s) { ;	}
#else //_MIN_MEM
protected:
	uint32 size;
public:
	uint32 GetSize() const { return size; }
	void SetSize(uint32 s) { size = s;	}
#endif //_MIN_MEM

protected:
	Track* track;

public:
	Track* GetTrack() { return track; }
	void SetTrack(Track* c) { track = c; }

#ifdef _DEBUG
public:
	static unsigned int AliveObjects;
	static unsigned int MaxObjects;
#endif //_DEBUG


// Compatibility
//------------------------------------------------

protected:
	//Sample* firstSample;
	int firstSampleIndex;

public:
	//void SetFirstSample(Sample* s) { firstSample = s; }
	//Sample* FirstSample() const { return firstSample; }
	void SetFirstSample(int index) { firstSampleIndex = index; }
	inline Sample* FirstSample() const;

	Sample* NextSample(Sample* s) const //!!! must stored in array
	{ 
		++s; 
		return s->GetChunk() == this ? s : 0;
	} 
	int ChildrenCount() const;
	uint32 ChildrenSize() const;

#ifdef _XMLDUMP
public:
	virtual const char* GetXmlTag() const { return "chunk"; }
	virtual bool DumpAttributes(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP

public:
	uint32 SampleDescriptionIndex() { return 1; }

public:
	bool DumpAsMemory(Writer& w) const;

public:
	void Reset() { track = 0; }  // 7/14/2006
};

//====================================================================

class Track : public Object
{
public:
	enum TrackType
	{
		ttVideo,
		ttAudio,
		ttHint,
		ttNull
	};

	struct VideoInfo
	{
		uint16 width;
		uint16 height;
	};

	struct AudioInfo
	{
		uint32 samplerate;
		uint16 channelcount;
		uint16 samplebits;
	};

public:
	Track(uint32 id=0, bool audio=false);
	virtual ~Track(); //must be virtual for free
	void ClearData(); //7/21/2006, remove all samples/chunks

private:
	uint32 track_ID;
	uint32 duration;
	uint32 timescale;

	int tracktype;

	union
	{
		VideoInfo vi;
		AudioInfo ai;
	};

	uint32 handler;

public:
	uint32 GetID() const {	return track_ID; }
	void SetID(uint32 id) 	{ track_ID = id; }

	uint32 GetDuration() const { return duration; }
	void SetDuration(uint32 dur) { duration = dur; }
	uint32 GetTimeScale() const { return timescale; }
	void SetTimeScale(uint32 scale) { timescale = scale; }

	bool IsAudio() const { return tracktype == ttAudio; }
	bool IsVideo() const { return tracktype == ttVideo; }
	int GetType() const { return tracktype; }
	void SetType(int tt);

	uint16 GetWidth() const { return vi.width; }
	uint16 GetHeight() const { return vi.height; }
	void SetWidth(uint16 w) { vi.width = w; }
	void SetHeight(uint16 h) { vi.height = h; }

	uint32 GetSampleRate() const { return ai.samplerate; }
	void SetSampleRate(uint32 sr) { ai.samplerate = sr; }
	uint16 GetChannelCount() const { return ai.channelcount; }
	void SetChannelCount(uint16 ch) { ai.channelcount = ch; }
	uint16 GetSampleBits() const { return ai.samplebits; }
	void SetSampleBits(uint16 sb) { ai.samplebits = sb; }

	void SetHandlerType(uint32 t) { handler = t; }
	uint32 GetHandlerType() const { return handler; }

// Descriptor
//------------------------------------------------
protected:
	uint32 descriptor_type;
	uint8* descriptor_data;
	uint32 descriptor_size;

public:
	void SetDescriptor(uint32 dtype, uint8* ddata, uint32 dsize);
	uint32 GetDescriptorType() const { return descriptor_type; }
	uint32 GetDescriptorSize() const { return descriptor_size; }
	uint8* GetDescriptorData() const { return descriptor_data; }

// Sample Type
//------------------------------------------------
protected:
	uint32 sample_type;

public:
	void SetSampleType(uint32 stype) { sample_type = stype; }
	uint32 GetSampleType() const { return sample_type; }

// Samples
//------------------------------------------------
protected:
	uint32 sample_count;
	Sample* samples;

public:
	bool CreateSamples(uint32 count);
	uint32 GetSampleCount() const { return sample_count; }
	Sample* GetSamples() { return samples; }
	Sample* GetSample(uint32 index) const { return index < sample_count ? &samples[index] : 0; }

	uint32 GetSyncSampleCount() const; //5/9/2005

// Chunks
//------------------------------------------------
protected:
	uint32 chunk_count;
	Chunk* chunks;

public:
	bool CreateChunks(uint32 count);
	uint32 GetChunkCount() const { return chunk_count; }
	Chunk* GetChunks() { return chunks; }
	Chunk* GetChunk(uint32 index) const { return index < chunk_count ? &chunks[index] : 0; }

// Tools
//------------------------------------------------
public:
	Chunk* FirstChunk() { return chunks ? &chunks[0] : 0; }
	Chunk* LastChunk() { return chunks ? &chunks[chunk_count - 1] : 0; }
	Sample* FirstSample() { return samples ? &samples[0] : 0; }
	Sample* LastSample() { return samples ? &samples[sample_count - 1] : 0; }
	uint32 GetMaxSampleSize() const;
	uint32 GetMaxChunkSize() const;
	uint32 GetSampleIndex(Sample* s) const;
	uint32 FindSampleByTime(uint32 t) const;

#ifdef _DEBUG
public:
	static unsigned int AliveObjects;
	static unsigned int MaxObjects;
#endif //_DEBUG

// Compatibility
//------------------------------------------------
public:
	int ChildrenCount() const { return GetChunkCount(); }
	Chunk* NextChunk(Chunk* c) //!!! must stored in array
	{ 
		++c; 
		return c->GetTrack() ? c : 0;
	} 

#ifdef _XMLDUMP
public:
	virtual const char* GetXmlTag() const { return "track"; }
	virtual bool DumpAttributes(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP


// Create, 3/22/2006
//------------------------------------------------
public:
	Sample* AddNewSample();
	Chunk* AddNewChunk();

protected:
	uint32 max_sample;
	uint32 max_chunk;

protected:
	void AdjustSamplesChunk(int offset);

protected:
	uint16 samples_per_chunk;

public:
	void SetSamplesPerChunk(uint16 spc)
	{
		samples_per_chunk = spc;
	}

	// add samples to chunk, auto controlled by samples per chunk
	Chunk* NewSampleAutoAddChunk(Sample*& sample);
	Chunk* AddSampleEnd();

	// another interface to add samples to chunk, manual controlled
	//Chunk* AddNewChunk();
	Chunk* NewSampleAddToChunk(Sample*& sample);
	Chunk* EndChunk();

	void UpdateDuration();
	void UpdateSize();

public:
	//uint32 FindSampleByTime(double timestamp);  //6/30/2006
};

//====================================================================

class Movie : public Container
{
public:
	Movie();
	~Movie();

	void ClearData();

	WRAP_CHILDREN_FUNCTIONS(Track)

protected:
	uint32 duration;

public:
	void SetDuration(uint32 dur) { duration = dur; }
	uint32 GetDuration() const { return duration; }

// Descriptor
//------------------------------------------------
protected:
	uint32 descriptor_type;
	uint8* descriptor_data;
	uint32 descriptor_size;

public:
	void SetDescriptor(uint32 dtype, uint8* ddata, uint32 dsize);
	uint32 GetDescriptorType() const { return descriptor_type; }
	uint32 GetDescriptorSize() const { return descriptor_size; }
	uint8* GetDescriptorData() const { return descriptor_data; }

public:
	//uint32 GetAudioTrackCount() const;
	//uint32 GetVideoTrackCount() const;
	Track* GetAudioTrack(int i=0) const;
	Track* GetVideoTrack(int i=0) const;

public:
	Track* GetTrackByID(uint32 id) const;
	Track* GetTrackByIndex(int index) const;
	uint32 GetSampleCount() const;

#ifdef _XMLDUMP
public:
	virtual const char* GetXmlTag() const { return "movie"; }
	virtual bool DumpAttributes(XmlWriter& w) const;
#endif //_XMLDUMP

public:
	void UpdateDuration();
	void UpdateSize();

};

//====================================================================

inline Track* Sample::GetTrack() const
{ 
	return GetChunk()->GetTrack(); 
}

inline uint32 Sample::GetTrackID() const
{ 
	return GetTrack()->GetID(); 
}


inline Sample* Chunk::FirstSample() const
{ 
	return firstSampleIndex >= 0 ? 
		track->GetSample(firstSampleIndex) :
		0; 
}

#if 0
inline Chunk* Sample::GetChunk() const
{
	return chunkindex >= 0 ?
		GetTrack()->GetChunk(chunkindex) :
		0;
}
#endif

//} //namespace ISOM

#pragma pack(pop)

#endif // _ISOM_LOGI_H
