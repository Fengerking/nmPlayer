
#ifndef __VOFORMATCHECKER_H__
#define __VOFORMATCHECKER_H__


#include "voFile.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
enum FileFormat
{
	FileFormat_Unknown	= 0,
	FileFormat_TS		= 1,
	FileFormat_AAC		= 2,
	FileFormat_MP4		= 3,
	FileFormat_SMTH		= 4,
	FileFormat_MP3		= 5,
	FileFormat_WEBVTT	= 6,
	FileFormat_TTML		= 7,
	FileFormat_ID3		= 8,
};
class voFormatChecker
{
public:
	voFormatChecker();
	~voFormatChecker();
public:
	VO_U32 FormatCheckStart(VO_PBYTE *ppBuffer, VO_U32 *pSize, VO_FILE_FORMAT *pFileFormat );
	VO_VOID Reset();
	VO_U32 GetInternalBuffer(VO_BYTE **ppBuffer, VO_U32 *pSize );
private:
	VO_PBYTE m_pBuffer;
	VO_U32   m_uBufferSize;
	VO_U32   m_uUsedSize;
	VO_FILE_FORMAT m_nFileFormat;
};
#ifdef _VONAMESPACE
}
#endif
#endif //__VOFORMATCHECKER_H__

