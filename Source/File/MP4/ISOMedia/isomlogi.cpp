#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "isomlogi.h"

//using namespace ISOM;

#ifdef _DEBUG
unsigned int Sample::AliveObjects = 0;
unsigned int Sample::MaxObjects = 0;
unsigned int Chunk::AliveObjects = 0;
unsigned int Chunk::MaxObjects = 0;
unsigned int Track::AliveObjects = 0;
unsigned int Track::MaxObjects = 0;
#endif //_DEBUG


// Sample
//====================================================================

Sample::Sample()
: size_attrib(0),
  timestamp(0)
//  , chunk(0) // 7/14/2006
{
#ifdef _DEBUG
	++AliveObjects;
	if (MaxObjects < AliveObjects)
		MaxObjects = AliveObjects;
#endif //_DEBUG
}

Sample::~Sample()
{
#ifdef _DEBUG
	--AliveObjects;
#endif //_DEBUG
}

#ifdef _MIN_MEM
uint32 Sample::GetAddress() const
{
	if (GetChunk() == 0)
		return 0;
	uint32 addr = GetChunk()->GetAddress();
	Sample* s = GetChunk()->FirstSample();
	while (s != this)
	{
		addr += s->GetSize();
		s = s->NextSample();
	}
	return addr;
}
#endif //_MIN_MEM

// Chunk
//====================================================================

Chunk::Chunk()
: firstSampleIndex(-1)
//, track(0)  // 7/14/2006
{
#ifdef _DEBUG
	++AliveObjects;
	if (MaxObjects < AliveObjects)
		MaxObjects = AliveObjects;
#endif //_DEBUG
}

Chunk::~Chunk()
{
#ifdef _DEBUG
	--AliveObjects;
#endif //_DEBUG
}

int Chunk::ChildrenCount() const
{
	Sample* s = FirstSample();
	if (s == 0)
		return 0;
	int c = 0;
	do
	{
		++c;
		++s;
	}
	while (s->GetChunk() == this);
	return c;
}

uint32 Chunk::ChildrenSize() const
{
	Sample* s = FirstSample();
	if (s == 0)
		return 0;
	int c = 0;
	do
	{
		c += s->GetSize();
		++s;
	}
	while (s->GetChunk() == this);
	return c;
}

// Track
//====================================================================

Track::Track(uint32 id, bool audio)
{
	track_ID = id;
	sample_type = FOURCC2_null;
	descriptor_type = 0;
	descriptor_size= 0;
	sample_count = 0;
	samples = 0;
	chunk_count = 0;
	chunks = 0;
	max_sample = 0;
	max_chunk = 0;
#ifdef _DEBUG
	++AliveObjects;
	if (MaxObjects < AliveObjects)
		MaxObjects = AliveObjects;
#endif //_DEBUG
}

Track::~Track()
{
	if (descriptor_size)
	{
		delete[] descriptor_data;
		descriptor_size = 0;
	}
	if (samples)
	{
		delete[] samples;
		samples = 0;
	}
	if (chunks)
	{
		delete[] chunks;
		chunks = 0;
	}
#ifdef _DEBUG
	--AliveObjects;
#endif //_DEBUG
}

void Track::ClearData()
{
	if (samples)
	{
		delete[] samples;
		samples = 0;
		sample_count = 0;
		max_sample = 0;
	}
	if (chunks)
	{
		delete[] chunks;
		chunks = 0;
		chunk_count = 0;
		max_chunk = 0;
	}
}

void Track::SetDescriptor(uint32 dtype, uint8* ddata, uint32 dsize)
{
	if (descriptor_size)
	{
		delete[] descriptor_data;
		descriptor_size = 0;
	}
	if (dtype == 0 || dsize == 0)
		return;
	descriptor_type = dtype;
	descriptor_size = dsize;
	descriptor_data = new uint8[dsize];
	memcpy(descriptor_data, ddata, dsize);
}

void Track::SetType(int tt)
{ 
	tracktype = tt;
	//TODO: default properties
}

bool Track::CreateSamples(uint32 count)
{
	samples = new Sample[count + 1];  // for NextSample
	if (samples == 0)
		return false;
	sample_count = count;
	//memset(&samples[count], 0, sizeof(Sample));
	samples[count].Reset();
	return true;
}

uint32 Track::GetSyncSampleCount() const
{
	uint32 c = 0;
	for (uint32 i = 0; i < sample_count; i++)
	{
		if (samples[i].IsSync())
			++c;
	}
	return c;
}

bool Track::CreateChunks(uint32 count)
{
	chunks = new Chunk[count + 1]; //for NextChunk
	if (chunks == 0)
		return false;
	chunk_count = count;
	//memset(&chunks[count], 0, sizeof(Chunk));
	chunks[count].Reset();
	return true;
}

uint32 Track::GetMaxSampleSize() const
{
	uint32 maxsize = 0;
	for (uint32 i = 0; i < sample_count; i++)
	{
		uint32 s = samples[i].GetSize();
		if (maxsize < s)
			maxsize = s;
	}
	return maxsize;
}

uint32 Track::GetMaxChunkSize() const
{
	uint32 maxsize = 0;
	for (uint32 i = 0; i < chunk_count; i++)
	{
		uint32 s = chunks[i].GetSize();
		if (maxsize < s)
			maxsize = s;
	}
	return maxsize;
}

uint32 Track::GetSampleIndex(Sample* s) const
{
	for (uint32 i = 0; i < sample_count; i++)
	{
		if (s == &samples[i])
			return i;
	}
	return sample_count; // do not find
}

#if 0
uint32 Track::FindSampleByTime(double t) const
{
	for (uint32 i = 0; i < sample_count; i++)
	{
		if (samples[i].GetTime() >= t)
			return i;
	}
	return sample_count; // do not find
}
#endif

uint32 Track::FindSampleByTime(uint32 timestamp) const
{
	uint32 i1 = 0;
	uint32 i2 = GetSampleCount() - 1;
	uint32 delta = (i2 - i1) >> 1;
	while (delta > 0)
	{
		uint32 i3 = i1 + delta;
		uint32 t3 = GetSample(i3)->GetTime();
		if (timestamp > t3)
			i1 = i3;
		else
			i2 = i3;
		delta = (i2 - i1) >> 1;
	}
	uint32 t2 = GetSample(i2)->GetTime();
	if (timestamp >= t2)
		return i2;
	return i1;
}


// Movie
//====================================================================


Movie::Movie()
: descriptor_type(0),
  descriptor_size(0)
{
}

Movie::~Movie()
{
	if (descriptor_size)
	{
		delete[] descriptor_data;
		descriptor_size = 0;
	}
}

void Movie::SetDescriptor(uint32 dtype, uint8* ddata, uint32 dsize)
{
	if (descriptor_size)
	{
		delete[] descriptor_data;
		descriptor_size = 0;
	}
	if (dtype == 0 || dsize == 0)
		return;
	descriptor_type = dtype;
	descriptor_size = dsize;
	descriptor_data = new uint8[dsize];
	memcpy(descriptor_data, ddata, dsize);
}

void Movie::ClearData()
{
	Track* t = FirstTrack();
	while (t)
	{
		t->ClearData();
		t = NextTrack(t);
	}
}

Track* Movie::GetTrackByID(uint32 id) const
{
	Track* t = FirstTrack();
	while (t)
	{
		if (t->GetID() == id)
			return t;
		t = NextTrack(t);
	}
	return 0;
}

Track* Movie::GetTrackByIndex(int index) const
{
	int i = 0;
	Track* t = FirstTrack();
	while (t)
	{
		if (i == index)
			return t;
		t = NextTrack(t);
		++i;
	}
	return 0;
}

uint32 Movie::GetSampleCount() const
{
	uint32 sum = 0;
	Track* track = FirstTrack();
	while (track)
	{
		sum += track->GetSampleCount();
		track = NextTrack(track);
	}
	return sum;
}

//====================================================================

#ifdef _XMLDUMP

bool Movie::DumpAttributes(XmlWriter& w) const
{
	bool b = Object::DumpAttributes(w);
	b &= w.OutAttribute("tracks", ChildrenCount());
	b &= w.OutAttribute("samples", GetSampleCount());
	//b &= w.OutAttribute("samplessize", NodeSize());
	return b;
}

bool Track::DumpAttributes(XmlWriter& w) const
{
	bool b = Object::DumpAttributes(w);
	b &= w.OutAttribute("chunks", GetChunkCount());
	b &= w.OutAttribute("samples", GetSampleCount());
	//b &= w.OutAttribute("samplessize", NodeSize());
	return b;
}

bool Track::DumpChildren(XmlWriter& w) const
{
	uint32 total = GetChunkCount();
	for (uint32 i = 0; i < total; i++)
		if (!GetChunk(i)->Dump(w))
			return false;
	return true;
}

bool Chunk::DumpAttributes(XmlWriter& w) const
{
	bool b = w.OutAttribute("addr", GetAddress());
	b &= w.OutAttribute("size", GetSize());
	b &= w.OutAttribute("samples", ChildrenCount());
	return b;
}

bool Chunk::DumpChildren(XmlWriter& w) const
{
	Sample* s = FirstSample();
	while (s)
	{
		if (!s->Dump(w))
			return false;
		s = NextSample(s);
	}
	return true;
}

bool Sample::DumpAttributes(XmlWriter& w) const
{
	bool b = w.OutAttribute("addr", GetAddress());
	b &= w.OutAttribute("size", GetSize());
	b &= w.OutAttribute("time", TIME_UNIT * GetTime(), "%f");
	b &= w.OutAttribute("duration", TIME_UNIT * GetDuration(), "%f");
	b &= w.OutAttribute("sync", IsSync());
	return b;
}

#endif //_XMLDUMP

bool Chunk::DumpAsMemory(Writer& w) const
{
	bool b = true;
	Sample* sample = FirstSample();
	while (sample)
	{
		b &= w.Write((void*)sample->GetAddress(), sample->GetSize());
		sample = NextSample(sample);
	}
	return b;
}


// for Create, 3/22/2006
//====================================================================
Sample* Track::AddNewSample()
{
	const int SIZE_STEP = 2048;
	if (sample_count == max_sample)
	{
		max_sample += SIZE_STEP;

		Sample* new_samples = new Sample[max_sample + 1]; //for NextSample
		if (new_samples == 0)
		{
			max_sample -= SIZE_STEP;
			return 0;
		}
		if (sample_count)
		{
			memcpy(new_samples, samples, sample_count * sizeof(Sample));
			delete[] samples;
		}
		samples = new_samples;
	}
	++sample_count;
	Sample* p = samples + sample_count;
	//memset(p, 0, sizeof(Sample));
	p->Reset();
	return --p;
}

Chunk* Track::AddNewChunk()
{
	const int SIZE_STEP = 512;
	if (chunk_count == max_chunk)
	{
		max_chunk += SIZE_STEP;
		Chunk* new_chunks = new Chunk[max_chunk + 1]; //for NextSample
		if (new_chunks == 0)
		{
			max_chunk -= SIZE_STEP;
			return 0;
		}
		if (chunk_count)
		{
			memcpy(new_chunks, chunks, chunk_count * sizeof(Sample));
			delete[] chunks;
			AdjustSamplesChunk(int((uint8*)new_chunks - (uint8*)chunks));
		}
		chunks = new_chunks;
	}
	++chunk_count;
	Chunk* p = chunks + chunk_count;
	//memset(p, 0, sizeof(Chunk));
	p->Reset();
	p->SetFirstSample(-1); //!!! Init should be -1
	--p;
	p->SetTrack(this);
	return p;
}

#if 1
void Track::AdjustSamplesChunk(int offset)
{
	Sample* s = samples;
	for (uint32 i = 0; i < sample_count; i++, s++)
	{
		uint8* p = (uint8*) s->GetChunk();
		p += offset;
		s->SetChunk((Chunk*)p);
	}
}
#endif

Chunk* Track::NewSampleAutoAddChunk(Sample*& sample)
{
	sample = AddNewSample();
	if (sample == NULL)
		return NULL;

	Chunk* chunk = LastChunk();
	int cc = 0;
	if (chunk == 0)
	{
		chunk = AddNewChunk();
		chunk->SetFirstSample(GetSampleCount() - 1);
	}
	else
	{
		cc = chunk->ChildrenCount();
		if (cc == samples_per_chunk)
		{
			chunk = AddNewChunk();
			chunk->SetFirstSample(GetSampleCount() - 1);
		}
	}
	sample->SetChunk(chunk);
	//int ichunk = GetChunkCount() - 1;
	//sample->SetChunk(ichunk);
	++cc;
	if (cc == samples_per_chunk)
	{
		chunk->SetSize(chunk->ChildrenSize());
		return chunk;
	}
	else
		return 0;
}

Chunk* Track::AddSampleEnd()
{
	Chunk* chunk = LastChunk();
	if (chunk)
	{
		if (chunk->ChildrenCount() < samples_per_chunk)
		{
			chunk->SetSize(chunk->ChildrenSize());
			return chunk;
		}
	}
	return 0;
}

Chunk* Track::NewSampleAddToChunk(Sample*& sample)
{
	sample = AddNewSample();
	if (sample == NULL)
		return NULL;

	Chunk* chunk = LastChunk();
	if (chunk)
	{
		if (chunk->FirstSample() == 0)
			chunk->SetFirstSample(GetSampleCount() - 1);
		sample->SetChunk(chunk);
		//int ichunk = GetChunkCount() - 1;
		//sample->SetChunk(ichunk);
	}
	return chunk;
}

Chunk* Track::EndChunk()
{
	Chunk* chunk = LastChunk();
	if (chunk)
	{
		chunk->SetSize(chunk->ChildrenSize());
	}
	return chunk;
}

void Track::UpdateDuration()
{
	uint32 dur = 0;
	Sample* s = samples;
	for (uint32 i = 0; i < sample_count; i++, s++)
	{
		dur += s->GetDuration();
	}
	SetDuration(dur);
}

void Track::UpdateSize()
{
	size = 0;
	Sample* s = samples;
	for (uint32 i = 0; i < sample_count; i++, s++)
	{
		size += s->GetSize();
	}
}

void Movie::UpdateDuration()
{
	uint32 d = 0;
	Track* track = FirstTrack();
	while (track)
	{
		track->UpdateDuration();
		// max track duration as movie duration
		if (d < track->GetDuration())
			d = track->GetDuration();
		track = NextTrack(track);
	}
	SetDuration(d);
}

void Movie::UpdateSize()
{
	size = 0;
	Track* track = FirstTrack();
	while (track)
	{
		track->UpdateSize();
		size += track->GetSize();
		track = NextTrack(track);
	}
}

