#include "isombox.h"
#include "mpxutil.h"

//using namespace ISOM;

//====================================================================

Box* Box::Load(Reader& r)
{
	Box base;
	if (!base.LoadBase(r))
		return 0;
	Box* box = CreateBoxByType(base.GetType());
	if (box == 0)
		return 0;
	box->Init(base);

	//long p1 = r.Position();
	if (box->LoadExt(r))
	{
		//long p2 = r.Position();
		//box->selfsize += p2 - p1;
		if (box->LoadChildren(r))
		{
			return box;
		}
	}
	delete box;
	return 0;

}


//====================================================================

#ifdef _XMLDUMP

bool Box::Dump(XmlWriter& w) const
{
	char typetext[5];
	Util::Int32Text(GetType(), typetext);
	bool b = w.StartTag(typetext);

	b &= DumpBase(w);
	b &= DumpExt(w);

	b &= w.CloseTag();

	b &= DumpChildren(w);

	b &= w.EndTag(typetext);
	return b;
}

bool Box::DumpBase(XmlWriter& w) const
{
	//bool b = w.OutAttribute("addr", GetAddress(), "0x%08x");
	bool b = w.OutAttribute("addr", GetAddress());
	b &= w.OutAttribute("size", GetSize());
	return b;
}

bool Box::DumpExt(XmlWriter& w) const
{
	return true;
}

#endif //_XMLDUMP

bool Box::LoadChildren(Reader& r)
{
	int leftsize = GetSize() - selfsize;
	while (leftsize > 8) //error tolerance, 7/27/2007
	{
		long p1 = r.Position();
		Box* b = Load(r);
		if (b == 0)
			return false;
		long p2 = r.Position();
		if (b->GetSize() == 0) //error tolerance, 4/17/2007
		{
			leftsize = 0;
		}
		else
			leftsize -= p2 - p1;
		AddChild(b);
	}
	r.Move(leftsize); //error tolerance, 10/8/2007
	return true;
}


bool Box::Dump(Writer& w) const
{
	bool b = DumpBase(w);
	b &= DumpExt(w);
	b &= DumpChildren(w);
	return b;
}

bool Box::DumpBase(Writer& w) const
{
	bool b = w.Write(size);
	b &= w.Write(type);
	return b;
}

bool Box::DumpExt(Writer& w) const
{
	return true;
}

void Box::UpdateSize()
{
	size = CalcBaseSize();
	size += CalcExtSize();
	Box* child = FirstChild();
	while (child)
	{
		child->UpdateSize();
		size += child->GetSize();
		child = NextChild(child);
	}
}

uint32 Box::CalcBaseSize() const
{
	uint32 c = 8;
	if (size == 1)
		c += 8;
	if (type == btUUID)
		c += 16;
	return c;
}

uint32 Box::CalcExtSize() const
{
	return 0;
}


Box::Box(uint32 boxtype)
: type(boxtype)
{
}

Box::~Box()
{
}

void Box::Init(const Box& b)
{
	SetSize(b.GetSize());
	SetAddress(b.GetAddress());
	type = b.type;
	//largesize = b.largesize;
	//uuid = b.uuid;
	selfsize = b.selfsize;
}

bool Box::LoadBase(Reader& r)
{
	long p1 = r.Position();
	SetAddress(p1);
	uint32 size;
	if (!r.Read(&size))
		return false;
	SetSize(size);
	if (!r.Read(&type))
		return false;
	if (size == 1) //allow size = 0
	{
#if 1 // do not support large size
		return false;
#else
		//if (size == 0) // error tolerance
			//return false;
		if (!r.Read(&largesize))
			return false;
#endif
	}
#if 0 //7/11/2006
	if (type == btUUID)
#if 1 // do not support UUID
		return false;
#else
		if (!r.Read(&uuid))
			return false;
#endif 
#endif //7/11/2006
	long p2 = r.Position();
	selfsize = p2 - p1;
	return true;
}

bool Box::LoadExt(Reader& r)
{
	return true;
}

bool BoxWithoutChild::LoadExt(Reader& r)
{
	// just skip others
	return SkipLoadExt(r);
}

bool MdatBox::LoadExt(Reader& r)
{
	// just skip others
	SkipLoadExt(r);
	return true; //ignore return values
}

bool FullBoxWithoutChild::LoadExt(Reader& r)
{
	if (!FullBox::LoadExt(r))
		return false;
	// just skip others
	return SkipLoadExt(r);
}

uint32 Box::GetType() const
{ 
	return type; 
}


//====================================================================

FullBox::FullBox(uint32 boxtype, uint8 v, uint32 f)
: Box(boxtype),
  version(v),
  flags(f)
{
}

bool FullBox::LoadExt(Reader& r)
{
	if (!r.Read(&flags))
		return false;
	version = (uint8)(flags >> 24);
	flags &= 0xffffff;
	selfsize += 4;
	return true;
}

#ifdef _XMLDUMP

bool FullBox::DumpExt(XmlWriter& w) const
{
	bool b = w.OutAttribute("version", version, "0x%02x");
	b &= w.OutAttribute("flags", flags, "0x%06x");
	return b;
}

#endif //_XMLDUMP

bool FullBox::DumpBase(Writer& w) const
{
	bool b = Box::DumpBase(w);
	uint32 temp = version;
	temp <<= 24;
	temp |= flags;
	b &= w.Write(temp);
	return true;
}

uint32 FullBox::CalcBaseSize() const
{
	return Box::CalcBaseSize() + 4;
}

//====================================================================

FileBox::FileBox()
: Box(btFILE)
{
}


FileBox::~FileBox()
{
}

bool FileBox::Load(Reader& r, uint32 fsize)
{
	SetSize(fsize);
	selfsize = 0;
	return LoadChildren(r);
}

//====================================================================

FileTypeBox::FileTypeBox()
: Box(btFTYP)
{
	compatible_brands_count = 0;
	major_brand = FOURCC2_isom;
	minor_version = 0;
}

FileTypeBox::~FileTypeBox()
{
}

bool FileTypeBox::LoadExt(Reader& r)
{
	long p1 = r.Position();
	if (!r.Read(&major_brand))
		return false;
	if (!r.Read(&minor_version))
		return false;
	if (GetType() != btFTYP)
		return false;
	long p2 = r.Position();
	selfsize += p2 - p1;
	compatible_brands_count = (GetSize() - selfsize) / 4;
	if (compatible_brands_count > compatible_brands_max)
		//return false;
		compatible_brands_count = compatible_brands_max;
	if (compatible_brands_count)
	{
		for (uint32 i = 0; i < compatible_brands_count; i++)
		{
			if (!r.Read(&compatible_brands[i]))
				return false;
		}
	}
	selfsize = GetSize();
	return true;
}

#ifdef _XMLDUMP

bool FileTypeBox::DumpExt(XmlWriter& w) const
{
	bool b = w.OutAttributeText("major_brand", Util::Int32Text(major_brand));
	b &= w.OutAttribute("minor_version", minor_version, "0x%08x");
	char buf[256];
	buf[0] = 0;
	for (uint32 i = 0; i < compatible_brands_count; ++i)
	{
		strcat(buf, Util::Int32Text(compatible_brands[i]));
		strcat(buf, ";");
	}
	b &= w.OutAttributeText("compatible_brands", buf);
	return b;
}

#endif //_XMLDUMP

bool FileTypeBox::DumpExt(Writer& w) const
{
	bool b = w.Write(major_brand);
	b &= w.Write(minor_version);
	for (uint32 i = 0; i < compatible_brands_count; i++)
		b &= w.Write(compatible_brands[i]);
	return b;
}

uint32 FileTypeBox::CalcExtSize() const
{
	return 8 + 4 * compatible_brands_count;
}

//====================================================================

MovieHeaderBox::MovieHeaderBox(uint8 v, uint32 f)
: FullBox(btMVHD, v, f)
{
	rate = 0x00010000;
	volume = 0x0100;
	matrix[0] = 0x00010000;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = 0x00010000;
	matrix[5] = 0;
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = 0x40000000;
	for (int i = 0; i < 6; i++)
		pre_defined[i] = 0;

	if (version == 1)
	{
		v1.creation_time = v1.modification_time =  Util::CurrentTime();
	}
	else
	{
		v0.creation_time = v0.modification_time = (uint32) Util::CurrentTime();
	}
}

void MovieHeaderBox::UpdateTimeStamp()
{
	if (version == 1)
	{
		v1.modification_time = Util::CurrentTime();
	}
	else
	{
		v0.modification_time = (uint32)Util::CurrentTime();
	}
}

void MovieHeaderBox::SetDuration(uint64 dur, uint32 scale)
{
	if (version == 1)
	{
		v1.duration = dur;
		timescale = scale;
	}
	else
	{
		v0.duration = (uint32)dur;
		timescale = scale;
	}
}

bool MovieHeaderBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	if (version == 1)
	{
		b &= r.Read(&v1.creation_time);
		b &= r.Read(&v1.modification_time);
		b &= r.Read(&timescale);
		b &= r.Read(&v1.duration);
	}
	else
	{
		b &= r.Read(&v0.creation_time);
		b &= r.Read(&v0.modification_time);
		b &= r.Read(&timescale);
		b &= r.Read(&v0.duration);
	}
	b &= r.Read(&rate);
	b &= r.Read(&volume);
	b &= r.Move(2 + 4 * 2);
	int i;
	for (i = 0; i < 9; i++)
		b &= r.Read(&matrix[i]);
	for (i = 0; i < 6; i++)
		b &= r.Read(&pre_defined[i]);
	b &= r.Read(&next_track_ID);
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

bool MovieHeaderBox::DumpExt(Writer& w) const
{
	bool b = true;
	if (version == 1)
	{
		b &= w.Write(v1.creation_time);
		b &= w.Write(v1.modification_time);
		b &= w.Write(timescale);
		b &= w.Write(v1.duration);
	}
	else
	{
		b &= w.Write(v0.creation_time);
		b &= w.Write(v0.modification_time);
		b &= w.Write(timescale);
		b &= w.Write(v0.duration);
	}
	b &= w.Write(rate);
	b &= w.Write(volume);
	uint16 t1 = 0;
	b &= w.Write(t1);
	uint32 t2 = 0;
	b &= w.Write(t2);
	b &= w.Write(t2);
	int i;
	for (i = 0; i < 9; i++)
		b &= w.Write(matrix[i]);
	for (i = 0; i < 6; i++)
		b &= w.Write(pre_defined[i]);
	b &= w.Write(next_track_ID);
	return b;
}

uint32 MovieHeaderBox::CalcExtSize() const
{
	uint32 s = 4 + 2 + 2 + 4 * 2 + 4 * 9 + 4 * 6 + 4;
	s += version == 1 ? 28 : 16;
	return s;
}

#ifdef _XMLDUMP

bool MovieHeaderBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	if (version == 1)
	{
		b &= w.OutAttributeText("creation_time", Util::TimeText(v1.creation_time));
		b &= w.OutAttributeText("modification_time", Util::TimeText(v1.modification_time));
		b &= w.OutAttribute("duration", v1.duration, "%I64u");
		b &= w.OutAttribute("timescale", timescale, "%u");
	}
	else
	{
		b &= w.OutAttributeText("creation_time", Util::TimeText(v0.creation_time));
		b &= w.OutAttributeText("modification_time", Util::TimeText(v0.modification_time));
		b &= w.OutAttribute("duration", v0.duration, "%u");
		b &= w.OutAttribute("timescale", timescale, "%u");
	}
	b &= w.OutAttribute("rate", rate, "0x%08x");
	b &= w.OutAttribute("volume", volume, "0x%04x");
	b &= w.OutAttribute("next_track_ID", next_track_ID);
	return b;
}

#endif //_XMLDUMP


//====================================================================

TrackHeaderBox::TrackHeaderBox(uint8 v, uint32 f)
: FullBox(btTKHD, v, f),
	width(0),
	height(0)
{
	layer = 0;
	alternate_group = 0;
	matrix[0] = 0x00010000;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = 0x00010000;
	matrix[5] = 0;
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = 0x40000000;
	if (version == 1)
	{
		v1.creation_time = v1.modification_time = Util::CurrentTime();
	}
	else
	{
		v0.creation_time = v0.modification_time = (uint32)Util::CurrentTime();
	}
}

bool TrackHeaderBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	if (version == 1)
	{
		b &= r.Read(&v1.creation_time);
		b &= r.Read(&v1.modification_time);
		b &= r.Read(&track_ID);
		b &= r.Move(4);
		b &= r.Read(&v1.duration);
	}
	else
	{
		b &= r.Read(&v0.creation_time);
		b &= r.Read(&v0.modification_time);
		b &= r.Read(&track_ID);
		b &= r.Move(4);
		b &= r.Read(&v0.duration);
	}
	b &= r.Move(4 * 2);
	b &= r.Read(&layer);
	b &= r.Read(&alternate_group);
	b &= r.Read(&volume);
	b &= r.Move(2);
	for (int i = 0; i < 9; i++)
		b &= r.Read(&matrix[i]);
	b &= r.Read(&width);
	b &= r.Read(&height);
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}


void TrackHeaderBox::UpdateTimeStamp()
{
	if (version == 1)
	{
		v1.modification_time = Util::CurrentTime();
	}
	else
	{
		v0.modification_time = (uint32)Util::CurrentTime();
	}
}

void TrackHeaderBox::SetVideoSize(double w, double h)
{
	width = Util::Fixed32(w);
	height = Util::Fixed32(h);
}

bool TrackHeaderBox::DumpExt(Writer& w) const
{
	bool b = true;
	uint16 t16 = 0;
	uint32 t32 = 0;
	if (version == 1)
	{
		b &= w.Write(v1.creation_time);
		b &= w.Write(v1.modification_time);
		b &= w.Write(track_ID);
		b &= w.Write(t32);
		b &= w.Write(v1.duration);
	}
	else
	{
		b &= w.Write(v0.creation_time);
		b &= w.Write(v0.modification_time);
		b &= w.Write(track_ID);
		b &= w.Write(t32);
		b &= w.Write(v0.duration);
	}
	b &= w.Write(t32);
	b &= w.Write(t32);
	b &= w.Write(layer);
	b &= w.Write(alternate_group);
	b &= w.Write(volume);
	b &= w.Write(t16);
	for (int i = 0; i < 9; i++)
		b &= w.Write(matrix[i]);
	b &= w.Write(width);
	b &= w.Write(height);
	return b;
}

uint32 TrackHeaderBox::CalcExtSize() const
{
	uint32 s = 4 * 2 + 2 + 2 + 2 + 2 + 4 * 9 + 4 + 4;
	s += version == 1 ? 32 : 20;
	return s;
}

#ifdef _XMLDUMP

bool TrackHeaderBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	if (version == 1)
	{
		b &= w.OutAttributeText("creation_time", Util::TimeText(v1.creation_time));
		b &= w.OutAttributeText("modification_time", Util::TimeText(v1.modification_time));
		b &= w.OutAttribute("duration", v1.duration, "%I64u");
		b &= w.OutAttribute("track_ID", track_ID, "%u");
	}
	else
	{
		b &= w.OutAttributeText("creation_time", Util::TimeText(v0.creation_time));
		b &= w.OutAttributeText("modification_time", Util::TimeText(v0.modification_time));
		b &= w.OutAttribute("duration", v0.duration, "%u");
		b &= w.OutAttribute("track_ID", track_ID, "%u");
	}
	b &= w.OutAttribute("layer", layer);
	b &= w.OutAttribute("alternate_group", alternate_group);
	b &= w.OutAttribute("volume", volume, "0x%04x");
	b &= w.OutAttribute("width", width, "%08x");
	b &= w.OutAttribute("height", height, "%08x");
	return b;
}

#endif //_XMLDUMP

//====================================================================

MediaHeaderBox::MediaHeaderBox(uint8 v, uint32 f)
: FullBox(btMDHD, v, f)
{
	pre_defined = 0;
	language = Util::LanguageCode(FOURCC2_enu);

	if (version == 1)
	{
		v1.creation_time = v1.modification_time = Util::CurrentTime();
	}
	else
	{
		v0.creation_time = v0.modification_time = (uint32) Util::CurrentTime();
	}
}

bool MediaHeaderBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	if (version == 1)
	{
		b &= r.Read(&v1.creation_time);
		b &= r.Read(&v1.modification_time);
		b &= r.Read(&timescale);
		b &= r.Read(&v1.duration);
	}
	else
	{
		b &= r.Read(&v0.creation_time);
		b &= r.Read(&v0.modification_time);
		b &= r.Read(&timescale);
		b &= r.Read(&v0.duration);
	}
	b &= r.Read(&language);
	b &= r.Read(&pre_defined);
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

bool MediaHeaderBox::DumpExt(Writer& w) const
{
	bool b = true;
	if (version == 1)
	{
		b &= w.Write(v1.creation_time);
		b &= w.Write(v1.modification_time);
		b &= w.Write(timescale);
		b &= w.Write(v1.duration);
	}
	else
	{
		b &= w.Write(v0.creation_time);
		b &= w.Write(v0.modification_time);
		b &= w.Write(timescale);
		b &= w.Write(v0.duration);
	}
	b &= w.Write(language);
	b &= w.Write(pre_defined);
	return b;
}

uint32 MediaHeaderBox::CalcExtSize() const
{
	uint32 s = 4;
	s += version == 1 ? 28 : 16;
	return s;
}

void MediaHeaderBox::UpdateTimeStamp()
{
	if (version == 1)
	{
		v1.modification_time = Util::CurrentTime();
	}
	else
	{
		v0.modification_time = (uint32)Util::CurrentTime();
	}
}

void MediaHeaderBox::SetDuration(uint64 dur, uint32 scale)
{
	if (version == 1)
	{
		v1.duration = dur;
		timescale = scale;
	}
	else
	{
		v0.duration = (uint32)dur;
		timescale = scale;
	}
}

#ifdef _XMLDUMP

bool MediaHeaderBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	if (version == 1)
	{
		b &= w.OutAttributeText("creation_time", Util::TimeText(v1.creation_time));
		b &= w.OutAttributeText("modification_time", Util::TimeText(v1.modification_time));
		b &= w.OutAttribute("duration", v1.duration, "%I64u");
		b &= w.OutAttribute("timescale", timescale, "%u");
	}
	else
	{
		b &= w.OutAttributeText("creation_time", Util::TimeText(v0.creation_time));
		b &= w.OutAttributeText("modification_time", Util::TimeText(v0.modification_time));
		b &= w.OutAttribute("duration", v0.duration, "%u");
		b &= w.OutAttribute("timescale", timescale, "%u");
	}
	b &= w.OutAttributeText("language", Util::LanguageText(language));
	b &= w.OutAttribute("pre_defined", pre_defined);
	return b;
}

#endif //_XMLDUMP

//====================================================================

HandlerBox::HandlerBox(uint8 v, uint32 f)
: FullBox(btHDLR, v, f),
	handler_type(0),
	name(0)
{
	pre_defined = 0;
	handler_type = 0;
}

HandlerBox::~HandlerBox()
{
	if (name)
	{
		delete[] name;
		name = 0;
	}
}

bool HandlerBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&pre_defined);
	b &= r.Read(&handler_type);
	b &= r.Move(4 * 3);
	if (name)
		delete[] name;
	long p2 = r.Position();
	selfsize += p2 - p1;
	name = r.ReadString(GetSize() - selfsize);
	selfsize = GetSize();
	return b;
}

bool HandlerBox::DumpExt(Writer& w) const
{
	uint32 t32 = 0;
	bool b = w.Write(pre_defined);
	b &= w.Write(handler_type);
	b &= w.Write(t32);
	b &= w.Write(t32);
	b &= w.Write(t32);
	if (name)
		b &= w.Write(name, (uint32)strlen(name) + 1);
	else
		b &= w.Write(&name, 1);
	return b;
}

uint32 HandlerBox::CalcExtSize() const
{
	uint32 s = 21;
	if (name)
		s += (uint32)strlen(name);
	return s;
}

#ifdef _XMLDUMP

bool HandlerBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("pre_defined", pre_defined);
	b &= w.OutAttributeText("handler_type", Util::Int32Text(handler_type));
	b &= w.OutAttributeText("name", name);
	return b;
}

#endif //_XMLDUMP


//====================================================================

EditListBox::EditListBox(uint8 v, uint32 f)
: FullBox(btELST, v, f)
, entry_count (0)
, media_rate_integer(1)
, media_rate_fraction(0)
{
}

void EditListBox::Release()
{
	if (entry_count > 0)
	{
		if (version == 1)
			delete[] v1;
		else
			delete[] v0;
		entry_count = 0;
	}
}

EditListBox::~EditListBox()
{
	Release();
}

bool EditListBox::LoadExt(Reader& r)
{
	Release();
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&entry_count);
	if (entry_count > 0)
	{
		if (version == 1)
			v1 = new V1[entry_count];
		else
			v0 = new V0[entry_count];
	}
	for (uint32 i = 0; i < entry_count; i++)
	{
		if (version == 1)
		{
			b &= r.Read(&v1[i].segment_duration);
			b &= r.Read(&v1[i].media_time);
		}
		else
		{
			b &= r.Read(&v0[i].segment_duration);
			b &= r.Read(&v0[i].media_time);
		}
	}
	b &= r.Read(&media_rate_integer);
	b &= r.Read(&media_rate_fraction);
	long p2 = r.Position();
	selfsize += p2 - p1;
#if 0
	b &= (selfsize == GetSize());
	return b;
#else
	r.Move(GetSize() - selfsize);
	selfsize = GetSize(); //6/1/2007
	return true;
#endif
}

void EditListBox::SetEntryCount(int count)
{
	Release();
	if (version == 1)
		v1 = new V1[count];
	else
		v0 = new V0[count];
	entry_count = count;
}

bool EditListBox::SetEntry(int index, uint32 dur, int32 time)
{
	if ((uint32)index >= entry_count)
		return false;
	if (version == 1)
	{
		v1[index].segment_duration = dur;
		v1[index].media_time = time;
	}
	else
	{
		v0[index].segment_duration = dur;
		v0[index].media_time = time;
	}
	return true;
}

bool EditListBox::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	for (uint32 i = 0; i < entry_count; i++)
	{
		if (version == 1)
		{
			b &= w.Write(v1[i].segment_duration);
			b &= w.Write(v1[i].media_time);
		}
		else
		{
			b &= w.Write(v0[i].segment_duration);
			b &= w.Write(v0[i].media_time);
		}
	}
	b &= w.Write(media_rate_integer);
	b &= w.Write(media_rate_fraction);
	return b;
}

uint32 EditListBox::CalcExtSize() const
{
	return 8 + (version ? 16 : 8) * entry_count; //4 + ... + 2 + 2;
}

#ifdef _XMLDUMP

bool EditListBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("entry_count", entry_count);
	b &= w.OutAttribute("media_rate_integer", media_rate_integer);
	b &= w.OutAttribute("media_rate_fraction", media_rate_fraction);
	return b;
}

bool EditListBox::DumpChildren(XmlWriter& w) const
{
	bool b = true;
	for (uint32 i = 0; i < entry_count; i++)
	{
		w.StartTag("entry");
		if (version == 1)
		{
			b &= w.OutAttribute("segment_duration", v1[i].segment_duration);
			b &= w.OutAttribute("media_time", v1[i].media_time);
		}
		else
		{
			b &= w.OutAttribute("segment_duration", v0[i].segment_duration);
			b &= w.OutAttribute("media_time", v0[i].media_time);
		}
		w.EndTag();
	}
	return b;
}

#endif //_XMLDUMP

//====================================================================

VideoMediaHeaderBox::VideoMediaHeaderBox(uint8 v, uint32 f)
: FullBox(btVMHD, v, f)
{
	graphicsmode = 0;
	opcolor[0] = 0;
	opcolor[1] = 0;
	opcolor[2] = 0;
}

bool VideoMediaHeaderBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&graphicsmode);
	for (uint32 i = 0; i < 3; i++)
		b &= r.Read(&opcolor[i]);
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

bool VideoMediaHeaderBox::DumpExt(Writer& w) const
{
	bool b = w.Write(graphicsmode);
	for (uint32 i = 0; i < 3; i++)
		b &= w.Write(opcolor[i]);
	return b;
}

uint32 VideoMediaHeaderBox::CalcExtSize() const
{
	return 2 + 2 * 3;
}

#ifdef _XMLDUMP

bool VideoMediaHeaderBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("graphicsmode", graphicsmode);
	b &= w.OutAttribute("opcolor.red", opcolor[0]);
	b &= w.OutAttribute("opcolor.green", opcolor[1]);
	b &= w.OutAttribute("opcolor.blue", opcolor[2]);
	return b;
}

#endif //_XMLDUMP

//====================================================================

SoundMediaHeaderBox::SoundMediaHeaderBox(uint8 v, uint32 f)
: FullBox(btSMHD, v, f)
{
	balance = 0;
}

bool SoundMediaHeaderBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&balance);
	b &= r.Move(2);
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

#ifdef _XMLDUMP

bool SoundMediaHeaderBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("balance", balance, "0x%04x");
	return b;
}

#endif //_XMLDUMP

bool SoundMediaHeaderBox::DumpExt(Writer& w) const
{
	bool b = w.Write(balance);
	uint16 t16 = 0;
	b &= w.Write(t16);
	return b;
}

uint32 SoundMediaHeaderBox::CalcExtSize() const
{
	return 4;
}

//====================================================================

HintMediaHeaderBox::HintMediaHeaderBox(uint8 v, uint32 f)
: FullBox(btHMHD, v, f)
{
}

bool HintMediaHeaderBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&maxPDUsize);
	b &= r.Read(&avgPDUsize);
	b &= r.Read(&maxbitrate);
	b &= r.Read(&avgbitrate);
	b &= r.Move(4);
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

#ifdef _XMLDUMP

bool HintMediaHeaderBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("maxPDUsize", maxPDUsize);
	b &= w.OutAttribute("avgPDUsize", avgPDUsize);
	b &= w.OutAttribute("maxbitrate", maxbitrate);
	b &= w.OutAttribute("avgbitrate", avgbitrate);
	return b;
}

#endif //_XMLDUMP

bool HintMediaHeaderBox::DumpExt(Writer& w) const
{
	bool b = w.Write(maxPDUsize);
	b &= w.Write(avgPDUsize);
	b &= w.Write(maxbitrate);
	b &= w.Write(avgbitrate);
	uint32 t32 = 0;
	b &= w.Write(t32);
	return b;
}

uint32 HintMediaHeaderBox::CalcExtSize() const
{
	return 2 * 2 + 4 * 3;
}

//====================================================================

NullMediaHeaderBox::NullMediaHeaderBox(uint8 v, uint32 f)
: FullBox(btNMHD, v, f)
{
}

//====================================================================

DataReferenceBox::DataReferenceBox(uint8 v, uint32 f)
: FullBox(btDREF, v, f),
	entry_count(0)
{
}

bool DataReferenceBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	b &= r.Read(&entry_count);
	selfsize += 4;
	return b;
}

bool DataReferenceBox::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	return b;
}

uint32 DataReferenceBox::CalcExtSize() const
{
	return 4;
}

#ifdef _XMLDUMP

bool DataReferenceBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("entry_count", entry_count);
	return b;
}

#endif //_XMLDUMP


//====================================================================

SampleDescriptionBox::SampleDescriptionBox(uint8 v, uint32 f)
: FullBox(btSTSD, v, f),
	entry_count(0)
{
}

bool SampleDescriptionBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	b &= r.Read(&entry_count);
	selfsize += 4;
	return b;
}

bool SampleDescriptionBox::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	return b;
}

uint32 SampleDescriptionBox::CalcExtSize() const
{
	return 4;
}

#ifdef _XMLDUMP

bool SampleDescriptionBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("entry_count", entry_count);
	return b;
}

#endif //_XMLDUMP

//====================================================================

SampleEntry::SampleEntry(uint32 fmt)
: Box(fmt),
	data_reference_index(1)
{
}

bool SampleEntry::LoadExt(Reader& r)
{
	bool b = Box::LoadExt(r);
	b &= r.Move(6);
	b &= r.Read(&data_reference_index);
	selfsize += 6 + 2;
	return b;
}

#ifdef _XMLDUMP

bool SampleEntry::DumpExt(XmlWriter& w) const
{
	bool b = Box::DumpExt(w);
	b &= w.OutAttribute("data_reference_index", data_reference_index);
	return b;
}

#endif //_XMLDUMP

bool SampleEntry::DumpExt(Writer& w) const
{
	uint64 t64 = 0;
	bool b = w.Write(&t64, 6);
	b &= w.Write(data_reference_index);
	return b;
}

uint32 SampleEntry::CalcExtSize() const
{
	return 6 + 2;
}

//====================================================================

VisualSampleEntry::VisualSampleEntry(uint32 codingname)
: SampleEntry(codingname)
{
	horizresolution = 0x00480000; // 72 dpi
	vertresolution = 0x00480000; // 72 dpi
	frame_count = 1;
	depth = 0x0018;
	pre_defined = -1;
	memset(compressorname, 0, 32);
#ifdef _DEBUG
	width = 352;
	height = 240;
#endif //_DEBUG
}

bool VisualSampleEntry::LoadExt(Reader& r)
{
	bool b = SampleEntry::LoadExt(r);
	long p1 = r.Position();
	b &= r.Move(2 + 2 + 4 * 3);
	b &= r.Read(&width);
	b &= r.Read(&height);
	b &= r.Read(&horizresolution);
	b &= r.Read(&vertresolution);
	b &= r.Move(4);
	b &= r.Read(&frame_count);
	b &= r.Read(&compressorname, 32);
	b &= r.Read(&depth);
	b &= r.Read(&pre_defined);
	long p2 = r.Position();
	selfsize += p2 - p1;
	//b &= (selfsize == GetSize());
	return b;
}

bool VisualSampleEntry::DumpExt(Writer& w) const
{
	bool b = SampleEntry::DumpExt(w);
	uint16 t16 = 0;
	uint32 t32 = 0;
	b &= w.Write(t16);
	b &= w.Write(t16);
	b &= w.Write(t32);
	b &= w.Write(t32);
	b &= w.Write(t32);
	b &= w.Write(width);
	b &= w.Write(height);
	b &= w.Write(horizresolution);
	b &= w.Write(vertresolution);
	b &= w.Write(t32);
	b &= w.Write(frame_count);
	b &= w.Write(compressorname, 32);
	b &= w.Write(depth);
	b &= w.Write(pre_defined);
	return b;
}

uint32 VisualSampleEntry::CalcExtSize() const
{
	uint32 s = SampleEntry::CalcExtSize();
	s += 2 + 2 + 4 * 3 + 2 + 2 + 4 + 4 + 4 + 2 + 32 + 2 + 2;
	return s;
}

#ifdef _XMLDUMP

bool VisualSampleEntry::DumpExt(XmlWriter& w) const
{
	bool b = Box::DumpExt(w);
	b &= w.OutAttribute("width", width);
	b &= w.OutAttribute("height", height);
	b &= w.OutAttribute("horizresolution", horizresolution, "0x%08x");
	b &= w.OutAttribute("vertresolution", vertresolution, "0x%08x");
	b &= w.OutAttribute("frame_count", frame_count);
	b &= w.OutAttribute("depth", depth);
	b &= w.OutAttribute("pre_defined", pre_defined);
	b &= w.OutAttributeText("compressorname", compressorname + 1);
	return b;
}

#endif //_XMLDUMP

//====================================================================

AudioSampleEntry::AudioSampleEntry(uint32 codingname)
: SampleEntry(codingname)
{
	channelcount = 2;
	samplesize = 16;
#ifdef _DEBUG
#endif //_DEBUG
}

bool AudioSampleEntry::LoadExt(Reader& r)
{
	bool b = SampleEntry::LoadExt(r);
	long p1 = r.Position();
	uint16 version;
	b &= r.Read(&version);
	b &= r.Move(6);
	b &= r.Read(&channelcount);
	b &= r.Read(&samplesize);
	b &= r.Move(2 + 2);
	b &= r.Read(&samplerate);
	if (samplerate == 0) //error compatible
		b &= r.Read(&samplerate);
	else
		b &= r.Move(2); //16.16, 11/29/2005
	if (version == 1) //for QuickTime
		r.Move(16);
	long p2 = r.Position();
	selfsize += p2 - p1;
	//b &= (selfsize == GetSize());
	return b;
}

bool AudioSampleEntry::DumpExt(Writer& w) const
{
	bool b = SampleEntry::DumpExt(w);
	uint16 t16 = 0;
	uint32 t32 = 0;
	b &= w.Write(t32);
	b &= w.Write(t32);
	b &= w.Write(channelcount);
	b &= w.Write(samplesize);
	b &= w.Write(t16);
	b &= w.Write(t16);
	b &= w.Write(samplerate);
	b &= w.Write(t16); //11/29/2005
	return b;
}

uint32 AudioSampleEntry::CalcExtSize() const
{
	uint32 s = SampleEntry::CalcExtSize();
	s += 4 * 2 + 2 + 2 + 2 + 2 + 4;
	return s;
}

#ifdef _XMLDUMP

bool AudioSampleEntry::DumpExt(XmlWriter& w) const
{
	bool b = Box::DumpExt(w);
	b &= w.OutAttribute("channelcount", channelcount);
	b &= w.OutAttribute("samplesize", samplesize);
	b &= w.OutAttribute("samplerate", samplerate, "%u");
	return b;
}

#endif //_XMLDUMP

//====================================================================

SyncSampleBox::SyncSampleBox(uint8 v, uint32 f)
: FullBox(btSTSS, v, f),
	entry_count(0),
	sample_number(0)
{
}

SyncSampleBox::~SyncSampleBox()
{
	Release();
}

void SyncSampleBox::Release()
{
	if (sample_number)
	{
		delete[] sample_number;
		sample_number = 0;
	}
}

void SyncSampleBox::SetMaxEntryCount(uint32 c)
{
	Release();
	sample_number = new uint32[c];
}

bool SyncSampleBox::LoadExt(Reader& r)
{
	Release();
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&entry_count);
	if (entry_count > 0)
	{
		sample_number = new uint32[entry_count];
	}
	for (uint32 i = 0; i < entry_count; i++)
	{
		b &= r.Read(&sample_number[i]);
	}
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

bool SyncSampleBox::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	for (uint32 i = 0; i < entry_count; i++)
	{
		b &= w.Write(sample_number[i]);
	}
	return b;
}

uint32 SyncSampleBox::CalcExtSize() const
{
	return 4 + 4 * entry_count;
}


#ifdef _XMLDUMP

bool SyncSampleBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("entry_count", entry_count);
	return b;
}

bool SyncSampleBox::DumpChildren(XmlWriter& w) const
{
	bool b = true;
	for (uint32 i = 0; i < entry_count; i++)
	{
		w.StartTag("entry");
		b &= w.OutAttribute("sample_number", sample_number[i]);
		w.EndTag();
	}
	return b;
}

#endif //_XMLDUMP


//====================================================================

SampleToChunkBox::SampleToChunkBox(uint8 v, uint32 f)
: FullBox(btSTSC, v, f),
	entry_count(0),
	entries(0)
{
}

SampleToChunkBox::~SampleToChunkBox()
{
	Release();
}

void SampleToChunkBox::Release()
{
	if (entries)
	{
		delete[] entries;
		entries = 0;
	}
}

void SampleToChunkBox::SetMaxEntryCount(uint32 c)
{
	Release();
	entries = new Entry[c];
}

bool SampleToChunkBox::AddEntry(uint32 chunk, uint32 spc, uint32 sdi)
{
	if (entry_count > 0)
	{
		Entry& e = entries[entry_count - 1];
		if (spc == e.samples_per_chunk && sdi == e.sample_description_index)
			return false;
	}
	Entry& e = entries[entry_count];
	e.first_chunk = chunk;
	e.samples_per_chunk = spc;
	e.sample_description_index = sdi;
	++entry_count;
	return true;
}

bool SampleToChunkBox::LoadExt(Reader& r)
{
	Release();
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&entry_count);
	if (entry_count > 0)
	{
		entries = new Entry[entry_count];
		for (uint32 i = 0; i < entry_count; i++)
		{
			b &= r.Read(&entries[i].first_chunk);
			b &= r.Read(&entries[i].samples_per_chunk);
			b &= r.Read(&entries[i].sample_description_index);
		}
	}
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

bool SampleToChunkBox::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	for (uint32 i = 0; i < entry_count; i++)
	{
		b &= w.Write(entries[i].first_chunk);
		b &= w.Write(entries[i].samples_per_chunk);
		b &= w.Write(entries[i].sample_description_index);
	}
	return b;
}

uint32 SampleToChunkBox::CalcExtSize() const
{
	return 4 + 12 * entry_count;
}

#ifdef _XMLDUMP

bool SampleToChunkBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("entry_count", entry_count);
	return b;
}

bool SampleToChunkBox::DumpChildren(XmlWriter& w) const
{
	bool b = true;
	for (uint32 i = 0; i < entry_count; i++)
	{
		w.StartTag("entry");
		b &= w.OutAttribute("first_chunk", entries[i].first_chunk);
		b &= w.OutAttribute("samples_per_chunk", entries[i].samples_per_chunk);
		b &= w.OutAttribute("sample_description_index", entries[i].sample_description_index);
		w.EndTag();
	}
	return b;
}

#endif //_XMLDUMP

//====================================================================

SampleSizeBox::SampleSizeBox(uint8 v, uint32 f)
: FullBox(btSTSZ, v, f),
  sample_size(0),
  sample_count(0),
  entry_size(0)
{
}

SampleSizeBox::~SampleSizeBox()
{
	Release();
}

void SampleSizeBox::Release()
{
	if (entry_size)
	{
		delete[] entry_size;
		entry_size = 0;
	}
}

void SampleSizeBox::SetSampleCount(uint32 count)
{
	Release();
	sample_count = count;
	entry_size = new uint32[sample_count];
	sample_size = 0;
}


bool SampleSizeBox::LoadExt(Reader& r)
{
	Release();
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&sample_size);
	b &= r.Read(&sample_count);
	if (sample_size == 0 && sample_count > 0)
	{
		entry_size = new uint32[sample_count];
		for (uint32 i = 0; i < sample_count; i++)
		{
			b &= r.Read(&entry_size[i]);
		}
	}
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

bool SampleSizeBox::DumpExt(Writer& w) const
{
	bool b = w.Write(sample_size);
	b &= w.Write(sample_count);
	if (sample_size == 0)
	{
		for (uint32 i = 0; i < sample_count; i++)
		{
			b &= w.Write(entry_size[i]);
		}
	}
	return b;
}

uint32 SampleSizeBox::CalcExtSize() const
{
	uint32 s = 8;
	if (sample_size == 0)
		s += 4 * sample_count;
	return s;
}

#ifdef _XMLDUMP

bool SampleSizeBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("sample_size", sample_size);
	b &= w.OutAttribute("sample_count", sample_count);
	return b;
}

bool SampleSizeBox::DumpChildren(XmlWriter& w) const
{
	bool b = true;
	if (sample_size > 0)
		return b;
	for (uint32 i = 0; i < sample_count; i++)
	{
		w.StartTag("entry");
		b &= w.OutAttribute("entry_size", entry_size[i]);
		w.EndTag();
	}
	return b;
}

#endif //_XMLDUMP

//====================================================================

ChunkOffsetBox::ChunkOffsetBox(uint8 v, uint32 f)
: FullBox(btSTCO, v, f),
	entry_count(0),
	chunk_offset(0)
{
}

ChunkOffsetBox::~ChunkOffsetBox()
{
	Release();
}

void ChunkOffsetBox::Release()
{
	if (chunk_offset)
	{
		delete[] chunk_offset;
		chunk_offset = 0;
	}
}

void ChunkOffsetBox::SetEntryCount(uint32 c)
{
	Release();
	entry_count = c;
	chunk_offset = new uint32[entry_count];
}

bool ChunkOffsetBox::LoadExt(Reader& r)
{
	Release();
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&entry_count);
	if (entry_count > 0)
	{
		chunk_offset = new uint32[entry_count];
		for (uint32 i = 0; i < entry_count; i++)
		{
			b &= r.Read(&chunk_offset[i]);
		}
	}
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

bool ChunkOffsetBox::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	for (uint32 i = 0; i < entry_count; i++)
	{
		b &= w.Write(chunk_offset[i]);
	}
	return b;
}

uint32 ChunkOffsetBox::CalcExtSize() const
{
	return 4 + 4 * entry_count;
}

#ifdef _XMLDUMP

bool ChunkOffsetBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("entry_count", entry_count);
	return b;
}

bool ChunkOffsetBox::DumpChildren(XmlWriter& w) const
{
	bool b = true;
	for (uint32 i = 0; i < entry_count; i++)
	{
		w.StartTag("entry");
		b &= w.OutAttribute("chunk_offset", chunk_offset[i]);
		w.EndTag();
	}
	return b;
}

#endif //_XMLDUMP

//====================================================================

CopyrightBox::CopyrightBox()
: notice(0)
{
}

CopyrightBox::~CopyrightBox()
{
	if (notice)
	{
		delete[] notice;
		notice = 0;
	}
}

bool CopyrightBox::LoadExt(Reader& r)
{
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&language);
	long p2 = r.Position();
	selfsize += p2 - p1;
	notice = r.ReadString(GetSize() - selfsize);
	selfsize = GetSize();
	return b;
}

#ifdef _XMLDUMP

bool CopyrightBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttributeText("language", Util::LanguageText(language));
	if (notice)
		b &= w.OutAttributeText("notice", notice);
	return b;
}

#endif //_XMLDUMP

//UnknownBox
//5/18/2005
//====================================================================

UnknownBox::UnknownBox(uint32 boxtype)
: Box(boxtype),
  bodysize(0)
{
}

UnknownBox::~UnknownBox()
{
	Release();
}

void UnknownBox::Release()
{
	if (bodysize)
	{
		delete[] bodydata;
		bodysize = 0;
	}
}

void UnknownBox::SetBody(uint8* d, uint32 s)
{
	Release();
	bodysize = s;
	bodydata = new uint8[bodysize];
	memcpy(bodydata, d, s);
}

bool UnknownBox::LoadExt(Reader& r)
{
	Release();
	bool b = Box::LoadExt(r);
	bodysize = GetSize() - selfsize;
	if (bodysize > 0)
	{
		bodydata = new uint8[bodysize];
		b &= r.Read(bodydata, bodysize);
		selfsize += bodysize;
	}
	return b;
}

#ifdef _XMLDUMP

bool UnknownBox::DumpExt(XmlWriter& w) const
{
	bool b = Box::DumpExt(w);
	b &= w.OutAttribute("bodysize", bodysize);
	b &= w.OutAttributeData("bodydata", bodydata, bodysize);
	return b;
}

#endif //_XMLDUMP

bool UnknownBox::DumpExt(Writer& w) const
{
	bool b = w.Write(bodydata, bodysize);
	return b;
}

uint32 UnknownBox::CalcExtSize() const
{
	return bodysize;
}


//====================================================================

UnknownFullBox::UnknownFullBox(uint32 boxtype, uint8 v, uint32 f)
: FullBox(boxtype, v, f),
  bodysize(0)
{
}

UnknownFullBox::~UnknownFullBox()
{
	Release();
}

void UnknownFullBox::Release()
{
	if (bodysize)
	{
		delete[] bodydata;
		bodysize = 0;
	}
}

void UnknownFullBox::SetBody(uint8* d, uint32 s)
{
	Release();
	bodysize = s;
	bodydata = new uint8[bodysize];
	memcpy(bodydata, d, s);
}

bool UnknownFullBox::LoadExt(Reader& r)
{
	Release();
	bool b = FullBox::LoadExt(r);
	bodysize = GetSize() - selfsize;
	if (bodysize > 0)
	{
		bodydata = new uint8[bodysize];
		b &= r.Read(bodydata, bodysize);
		selfsize += bodysize;
	}
	return b;
}

#ifdef _XMLDUMP

bool UnknownFullBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("bodysize", bodysize);
	return b;
}

#endif //_XMLDUMP

bool UnknownFullBox::DumpExt(Writer& w) const
{
	bool b = w.Write(bodydata, bodysize);
	return b;
}

uint32 UnknownFullBox::CalcExtSize() const
{
	return bodysize;
}


//====================================================================

TimeToSampleBox::TimeToSampleBox(uint8 v, uint32 f)
: FullBox(btSTTS, v, f),
	entry_count(0),
	entries(0)
{
}

TimeToSampleBox::~TimeToSampleBox()
{
	Release();
}

void TimeToSampleBox::Release()
{
	if (entries)
	{
		delete[] entries;
		entries = 0;
	}
}

void TimeToSampleBox::SetMaxEntryCount(uint32 c)
{
	Release();
	entries = new Entry[c];
}

bool TimeToSampleBox::AddEntry(uint32 delta)
{
	if (entry_count > 0)
	{
		Entry& e = entries[entry_count - 1];
		if (delta == e.sample_delta)
		{
			++e.sample_count;
			return false;
		}
	}
	Entry& e = entries[entry_count];
	e.sample_count = 1;
	e.sample_delta = delta;
	++entry_count;
	return true;
}

bool TimeToSampleBox::AddEntries(uint32 delta, uint32 count)
{
	Entry& e = entries[entry_count];
	e.sample_count = count;
	e.sample_delta = delta;
	++entry_count;
	return true;
}

bool TimeToSampleBox::LoadExt(Reader& r)
{
	Release();
	bool b = FullBox::LoadExt(r);
	long p1 = r.Position();
	b &= r.Read(&entry_count);
	if (entry_count > 0)
	{
		entries = new Entry[entry_count];
		for (uint32 i = 0; i < entry_count; i++)
		{
			b &= r.Read(&entries[i].sample_count);
			b &= r.Read(&entries[i].sample_delta);
		}
	}
	long p2 = r.Position();
	selfsize += p2 - p1;
	b &= (selfsize == GetSize());
	return b;
}

bool TimeToSampleBox::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	for (uint32 i = 0; i < entry_count; i++)
	{
		b &= w.Write(entries[i].sample_count);
		b &= w.Write(entries[i].sample_delta);
	}
	return b;
}

uint32 TimeToSampleBox::CalcExtSize() const
{
	return 4 + 8 * entry_count;
}

#ifdef _XMLDUMP

bool TimeToSampleBox::DumpExt(XmlWriter& w) const
{
	bool b = FullBox::DumpExt(w);
	b &= w.OutAttribute("entry_count", entry_count);
	return b;
}

bool TimeToSampleBox::DumpChildren(XmlWriter& w) const
{
	bool b = true;
	for (uint32 i = 0; i < entry_count; i++)
	{
		w.StartTag("entry");
		b &= w.OutAttribute("sample_count", entries[i].sample_count);
		b &= w.OutAttribute("sample_delta", entries[i].sample_delta);
		w.EndTag();
	}
	return b;
}

#endif //_XMLDUMP

//DescriptorBox
//5/18/2005
//====================================================================

DescriptorBox::DescriptorBox(uint32 boxtype)
: UnknownBox(boxtype)
{
}


//====================================================================

ESDBox::ESDBox(uint8 v, uint32 f)
: UnknownFullBox(FOURCC2_esds, v, f)
{
}


#ifdef _XMLDUMP

#include "../MP4Base/mp4desc.h"

bool ESDBox::DumpExt(XmlWriter& w) const
{
	return w.OutAttributeData("bodydata", bodydata, bodysize);
}

bool ESDBox::DumpChildren(XmlWriter& w) const
{
	MP4::Descriptor* desc = MP4::Descriptor::Load(bodydata, size - 12); // -12, 4/10/2007
	if (desc)
	{
		desc->Dump(w);
		delete desc;
	}
	return true;
}

#endif //_XMLDUMP


//====================================================================

bool MovieBox::LoadChildren(Reader& r)
{
#ifdef _XMLDUMP //keep real address
	bool b = Box::LoadChildren(r);
#else //_XMLDUMP
	int leftsize = GetSize() - selfsize;
	uint8* buf = new uint8[leftsize];
	bool b = r.Read(buf, leftsize);
	if (b)
	{
		MemStream ms(buf, leftsize);
		Stream* oldStream = r.GetStream();
		r.SetStream(&ms);
		b = Box::LoadChildren(r);
		r.SetStream(oldStream);
	}
	delete buf;
#endif //_XMLDUMP
	return b;
}



#if 0 //DataBox Removed

//DataBox
//4/27/2006
//====================================================================

DataBox::DataBox(uint32 boxtype)
: Box(boxtype),
  datasize(0),
  data(0)
{
}

DataBox::~DataBox()
{
	Release();
}

void DataBox::Release()
{
	if (data)
	{
		delete[] data;
		data = 0;
	}
}

void DataBox::SetData(uint8* d, uint32 s)
{
	Release();
	datasize = s;
	data = new uint8[datasize];
	memcpy(data, d, s);
}

bool DataBox::LoadExt(Reader& r)
{
	Release();
	bool b = Box::LoadExt(r);
	r.Read(&flags);
	version = flags >> 24;
	flags &= 0x00ffffff;
	r.Move(4); //skip 4 zeros
	selfsize += 8; 
	datasize = GetSize() - selfsize;
	if (datasize > 0)
	{
		data = new uint8[datasize];
		b &= r.Read(data, datasize);
		selfsize += datasize;
	}
	return b;
}

#ifdef _XMLDUMP

bool DataBox::DumpExt(XmlWriter& w) const
{
	bool b = Box::DumpExt(w);
	b &= w.OutAttribute("version", version);
	b &= w.OutAttribute("flags", flags);
	b &= w.OutAttribute("datasize", datasize);
	if (datasize > 0)
	{
		b &= w.OutAttributeData("data", data, datasize);
		char text[1024];
		if (GetDataText(text, 1024))
			w.OutAttributeText("totext", text);
	}
	return b;
}

#endif //_XMLDUMP

uint32 DataBox::GetDataInt() const
{
	uint32 u = 0;
	for (int i = 0; i < datasize; i++)
	{
		u <<= 8;
		u += data[i];
	}
	return u;
}

char* DataBox::GetDataText(char* buf, int bufsize) const
{
	memset(buf, 0, bufsize);
	switch (flags)
	{
	case 0:
		itoa(GetDataInt(), buf, 10);
		break;

	default:
		if (datasize < bufsize)
			memcpy(buf, data, datasize);
		break;
	}
	return buf;
}


bool DataBox::DumpExt(Writer& w) const
{
	// todo
	bool b = w.Write(data, datasize);
	return b;
}

uint32 DataBox::CalcExtSize() const
{
	return datasize + 8;
}

#endif //DataBox Removed

//====================================================================

FreeBox::FreeBox()
: Box(btFREE)
{
}

bool FreeBox::DumpExt(Writer& w) const
{
	int datasize = GetSize() - 8;
	if (datasize < 0)
		return false;
	if (datasize == 0)
		return true;
	uint8* data = new uint8[datasize];
	memset(data, 0, datasize);
	bool b = w.Write(data, datasize);
	delete[] data;
	return b;
}


//====================================================================


Box* Box::CreateBoxByType(uint32 type)
{
#define SW(_t, _c) case _t: return new _c()

	switch (type)
	{
	SW(btFTYP, FileTypeBox);
	SW(btMVHD, MovieHeaderBox);
	SW(btTKHD, TrackHeaderBox);
	SW(btMDHD, MediaHeaderBox);
	SW(btHDLR, HandlerBox);
	SW(btELST, EditListBox);
	SW(btVMHD, VideoMediaHeaderBox);
	SW(btSMHD, SoundMediaHeaderBox);
	SW(btHMHD, HintMediaHeaderBox);
	SW(btNMHD, NullMediaHeaderBox);
	SW(btDREF, DataReferenceBox);
	SW(btSTSD, SampleDescriptionBox);
	SW(btSTTS, TimeToSampleBox);
	SW(btSTSS, SyncSampleBox);
	SW(btSTSC, SampleToChunkBox);
	SW(btSTSZ, SampleSizeBox);
	SW(btSTCO, ChunkOffsetBox);
	SW(btCPRT, CopyrightBox);
	//SW(btAVCC, AVCConfigurationBox);
	SW(FOURCC2_esds, ESDBox);
	case FOURCC2_avcC:
	case FOURCC2_damr:
	case FOURCC2_d263:
		return new DescriptorBox(type);
	case FOURCC2_iods:
		return new UnknownFullBox(type);
	case FOURCC2_soun:
	case FOURCC2_samr:
	case FOURCC2_sawb:
	case FOURCC2_sawp:
	case FOURCC2_mp4a:
		return new AudioSampleEntry();
	case FOURCC2_vide:
	case FOURCC2_mp4v:
	case FOURCC2_s263:
	case FOURCC2_s264:
	case FOURCC2_avc1:
		return new VisualSampleEntry();
	SW(FOURCC2_hint, HintSampleEntry);
	case btMOOV:
		return new MovieBox();
	case btTRAK:
	case btEDTS:
	case btMDIA:
	case btMINF:
	case btDINF:
	case btSTBL:
	case btUDTA:
	case FOURCC2_wave: //for QuickTime
		return new Box();
	case btURL:
	case btURN:
		return new FullBoxWithoutChild();
#if 0
	case 'meta':
		return new FullBox();
	case 'ilst':
		return new Box();
#endif

	case FOURCC2_mdat:
		return new MdatBox();

	default: return new BoxWithoutChild();
	}
	return 0;

#undef SW
}


