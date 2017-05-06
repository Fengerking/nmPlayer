
// test case refer toFIPS PUB 180-1

#include "voType.h"

#ifndef __VO_SHA1_H__
#define __VO_SHA1_H__

#include <memory.h>

// default will support utility functions,
// can disable by define VO_SHA1_UTILITY_NO_SUPPORT before including the voSHA1.h
#define VO_SHA1_UTILITY_SUPPORT
#if defined(VO_SHA1_UTILITY_NO_SUPPORT)
#undef VO_SHA1_UTILITY_SUPPORT
#else
#include <stdio.h>
#include <string.h>

// Two formats for ReportHashByFormat(...)
enum
{
    VO_REPORT_FMT_HEX = 0,
    VO_REPORT_FMT_DIGIT = 1
};
#endif

// default will use little endian,
// can use big endian by define VO_SHA1_BIG_ENDIAN before including the voSHA1.h
#define VO_SHA1_LITTLE_ENDIAN
#if defined(VO_SHA1_BIG_ENDIAN)
#undef VO_SHA1_LITTLE_ENDIAN
#endif

// default will wiping variable,
// can disable by define VO_SHA1_NO_WIPE_MODE before including the voSHA1.h
#define VO_SHA1_WIPE_MODE
#if defined(VO_SHA1_NO_WIPE_MODE)
#undef VO_SHA1_WIPE_MODE
#endif

// Block of SHA1
typedef union
{
	VO_U8  c[64];
	VO_U32 lData[16];
} VO_SHA1_BLOCK;

class voSHA1
{
public:
	voSHA1();
	~voSHA1();
    
	void Reset();
	void Process(VO_U8 *data, VO_U32 len);
	// Finalize hash
	void Finalize();
    
	void GetHashData(VO_U8 *puDest);
    
#ifdef VO_SHA1_UTILITY_SUPPORT
	bool HashByFile(char *szFileName);
	// Report functions: as pre-formatted and raw data
	void ReportHashByFormat(char *szReport, unsigned char uReportType = VO_REPORT_FMT_HEX);
#endif
    
private:
	void DoTransform(VO_U32 *state, VO_U8 *buffer);
    
private:
    // pointer to the byte m_nBlocak
	VO_SHA1_BLOCK *m_pBlock;
    
	// Variables of member
	VO_U8  m_nBlocak[64];
    
	VO_U8  m_nBuffer[64];
	VO_U8  m_nDigest[20];
	VO_U32 m_nState[5];
	VO_U32 m_nCount[2];
};

#endif
