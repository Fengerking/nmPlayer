#include "PIFF.h"

UuidBox::UuidBox(void)
{
}

UuidBox::~UuidBox(void)
{
}

#ifdef SMOOTH_STREAMING

VO_U32 UuidBox::ParseUuidBox()
{
	
}
VO_U32 UuidBox::ParseProtectHeader()
{

}
#else
MP4RRC UuidBox::ParseUuidBox(Reader* r, int bodySize)
{
	MP4RRC rc = MP4RRC_OK;

	uint8 *buf = new uint8[bodySize];
	if (!r->Read(buf, bodySize)) 
		return MP4RRC_READ_FAILED;

	int uuidsize = sizeof (uuid);

	MemStream ms(buf, bodySize);
	ReaderLSB r2(&ms);
	char uuidc[sizeof (uuid)];
	r2.Read(&uuidc,uuidsize);

	//r2.Move(uuidsize);
	if (!memcmp(uuidc,PROTECTION_HEADER_UUID,uuidsize))
		ParseProtectHeader(&r2);
	else if (!memcmp(uuidc, SAMPLE_ENCRYTION_UUID, uuidsize));
		ParseSampleEncryption(&r2);
	/* If it is a TfxdBox */

	delete []buf;
	return rc;
}

MP4RRC UuidBox::ParseProtectHeader(Reader* r)
{
	MP4RRC rc = MP4RRC_OK;
	//skip version + flags = 4byte
	r->Move(4);
	uint8 systemID[16];
	r->Read(&systemID,sizeof(systemID));
	uint32 dataSize = 0;
	r->Read(&dataSize, 4);
	Util::Swap32(&dataSize);
	uint8 *data = new uint8[dataSize];
	r->Read(data,dataSize);
	if(!memcmp(systemID, PLAYREADY_SYSTEM_ID, sizeof(systemID)))
	{
		//Playready callback
	}
	delete []data;

	return rc;
}
MP4RRC UuidBox::ParseSampleEncryption(Reader* r)
{
	MP4RRC rc = MP4RRC_OK;
	//skip version + flags = 4byte
	r->Move(4);
	uint32 sample_count = 0
	r->Read(&sample_count, 4);
	Util::Swap32(&sample_count);
	uint8 *data = new uint8[dataSize];
	r->Read(data,dataSize);
	if(!memcmp(systemID, PLAYREADY_SYSTEM_ID, sizeof(systemID)))
	{
		//Playready callback
	}
	delete []data;
}
#endif