#ifndef __PIFF_H
#define __PIFF_H

#define SMOOTH_STREAMING

//#include "mpxio.h"

typedef char uuid[16];
/** The uuid of a SampleEncryptionBox, namely a specific UUIDBox. */
static const uuid SAMPLE_ENCRYTION_UUID = {  0xa2, 0x39, 0x4f, 0x52,
																				0x5a, 0x9b, 0x4f, 0x14,
																				0xa2, 0x44, 0x6c, 0x42,
																				0x7c, 0x64, 0x8d, 0xf4 };
/** The uuid of a ProtectionSystemSpecificHeaderBox, namely a specific UUIDBox. */
static const uuid PROTECTION_HEADER_UUID = {  0xd0, 0x8a, 0x4f, 0x18,
																					0x10, 0xf3, 0x4a, 0x82,
																					0xb6, 0xc8, 0x32, 0xd8,
																					0xab, 0xa1, 0x83, 0xd3 };

static const uuid PLAYREADY_SYSTEM_ID = {  0x9A, 0x04, 0xF0, 0x79,
																					0x98, 0x40, 0x42, 0x86,
																					0xAB, 0x92, 0xE6, 0x5B,
																					0xE0, 0x88, 0x5F, 0x95 };

#ifdef SMOOTH_STREAMING
class UuidBox
#else
#include "isomrd.h"
#include "mpxutil.h "
class UuidBox :	public BufferInBox
#endif
{
public:
	UuidBox(void);
	~UuidBox(void);
public:

#ifdef SMOOTH_STREAMING
	static VO_U32 ParseUuidBox();	
	static VO_U32 ParseProtectHeader();
#else	
	static MP4RRC ParseUuidBox(Reader* r, int bodySize);	
	static MP4RRC ParseProtectHeader(Reader* r);
#endif

protected:
};
#endif