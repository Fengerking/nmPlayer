
#include "voSHA1.h"
#include "stdlib.h"

#ifdef VO_SHA1_UTILITY_SUPPORT
#define SHA1_MAX_FILE_BUFFER 8000
#endif

// Rotate x bits to the left
#ifndef ROL32
#ifdef _MSC_VER
#define ROL32(_val32, _nBits) _rotl(_val32, _nBits)
#else
#define ROL32(_val32, _nBits) (((_val32)<<(_nBits))|((_val32)>>(32-(_nBits))))
#endif
#endif

#ifdef VO_SHA1_LITTLE_ENDIAN
#define SHABLK0(i) (m_pBlock->lData[i] = \
	(ROL32(m_pBlock->lData[i],24) & 0xFF00FF00) | (ROL32(m_pBlock->lData[i],8) & 0x00FF00FF))
#else
#define SHABLK0(i) (m_pBlock->lData[i])
#endif

#define SHABLK(i) (m_pBlock->lData[i&15] = ROL32(m_pBlock->lData[(i+13)&15] ^ m_pBlock->lData[(i+8)&15] \
	^ m_pBlock->lData[(i+2)&15] ^ m_pBlock->lData[i&15],1))

// SHA-1 rounds
#define _R0(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK0(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R1(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R2(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0x6ED9EBA1+ROL32(v,5); w=ROL32(w,30); }
#define _R3(v,w,x,y,z,i) { z+=(((w|x)&y)|(w&x))+SHABLK(i)+0x8F1BBCDC+ROL32(v,5); w=ROL32(w,30); }
#define _R4(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0xCA62C1D6+ROL32(v,5); w=ROL32(w,30); }

voSHA1::voSHA1()
{
	m_pBlock = (VO_SHA1_BLOCK *)m_nBlocak;

	Reset();
}

voSHA1::~voSHA1()
{
	Reset();
}

void voSHA1::Reset()
{
	// SHA1 initialization constants
	m_nState[0] = 0x67452301;
	m_nState[1] = 0xEFCDAB89;
	m_nState[2] = 0x98BADCFE;
	m_nState[3] = 0x10325476;
	m_nState[4] = 0xC3D2E1F0;

	m_nCount[0] = 0;
	m_nCount[1] = 0;
}

void voSHA1::DoTransform(VO_U32 *state, VO_U8 *buffer)
{
	// Copy state[] to working vars
	VO_U32 a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];

	memcpy(m_pBlock, buffer, 64);

	// 4 rounds of 20 operations each. Loop unrolled.
	_R0(a,b,c,d,e, 0); _R0(e,a,b,c,d, 1); _R0(d,e,a,b,c, 2); _R0(c,d,e,a,b, 3);
	_R0(b,c,d,e,a, 4); _R0(a,b,c,d,e, 5); _R0(e,a,b,c,d, 6); _R0(d,e,a,b,c, 7);
	_R0(c,d,e,a,b, 8); _R0(b,c,d,e,a, 9); _R0(a,b,c,d,e,10); _R0(e,a,b,c,d,11);
	_R0(d,e,a,b,c,12); _R0(c,d,e,a,b,13); _R0(b,c,d,e,a,14); _R0(a,b,c,d,e,15);
	_R1(e,a,b,c,d,16); _R1(d,e,a,b,c,17); _R1(c,d,e,a,b,18); _R1(b,c,d,e,a,19);
	_R2(a,b,c,d,e,20); _R2(e,a,b,c,d,21); _R2(d,e,a,b,c,22); _R2(c,d,e,a,b,23);
	_R2(b,c,d,e,a,24); _R2(a,b,c,d,e,25); _R2(e,a,b,c,d,26); _R2(d,e,a,b,c,27);
	_R2(c,d,e,a,b,28); _R2(b,c,d,e,a,29); _R2(a,b,c,d,e,30); _R2(e,a,b,c,d,31);
	_R2(d,e,a,b,c,32); _R2(c,d,e,a,b,33); _R2(b,c,d,e,a,34); _R2(a,b,c,d,e,35);
	_R2(e,a,b,c,d,36); _R2(d,e,a,b,c,37); _R2(c,d,e,a,b,38); _R2(b,c,d,e,a,39);
	_R3(a,b,c,d,e,40); _R3(e,a,b,c,d,41); _R3(d,e,a,b,c,42); _R3(c,d,e,a,b,43);
	_R3(b,c,d,e,a,44); _R3(a,b,c,d,e,45); _R3(e,a,b,c,d,46); _R3(d,e,a,b,c,47);
	_R3(c,d,e,a,b,48); _R3(b,c,d,e,a,49); _R3(a,b,c,d,e,50); _R3(e,a,b,c,d,51);
	_R3(d,e,a,b,c,52); _R3(c,d,e,a,b,53); _R3(b,c,d,e,a,54); _R3(a,b,c,d,e,55);
	_R3(e,a,b,c,d,56); _R3(d,e,a,b,c,57); _R3(c,d,e,a,b,58); _R3(b,c,d,e,a,59);
	_R4(a,b,c,d,e,60); _R4(e,a,b,c,d,61); _R4(d,e,a,b,c,62); _R4(c,d,e,a,b,63);
	_R4(b,c,d,e,a,64); _R4(a,b,c,d,e,65); _R4(e,a,b,c,d,66); _R4(d,e,a,b,c,67);
	_R4(c,d,e,a,b,68); _R4(b,c,d,e,a,69); _R4(a,b,c,d,e,70); _R4(e,a,b,c,d,71);
	_R4(d,e,a,b,c,72); _R4(c,d,e,a,b,73); _R4(b,c,d,e,a,74); _R4(a,b,c,d,e,75);
	_R4(e,a,b,c,d,76); _R4(d,e,a,b,c,77); _R4(c,d,e,a,b,78); _R4(b,c,d,e,a,79);

	// Add the working vars back into state
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;

	// Wipe variables
#ifdef VO_SHA1_WIPE_MODE
	a = b = c = d = e = 0;
#endif
}

// Use this function to hash in binary data and strings
void voSHA1::Process(VO_U8 *data, VO_U32 len)
{
	VO_U32 i, j;

	j = (m_nCount[0] >> 3) & 63;

	if((m_nCount[0] += len << 3) < (len << 3)) m_nCount[1]++;

	m_nCount[1] += (len >> 29);

	if((j + len) > 63)
	{
		i = 64 - j;
		memcpy(&m_nBuffer[j], data, i);
		DoTransform(m_nState, m_nBuffer);

		for(; i + 63 < len; i += 64) DoTransform(m_nState, &data[i]);

		j = 0;
	}
	else i = 0;

	memcpy(&m_nBuffer[j], &data[i], len - i);
}

#ifdef VO_SHA1_UTILITY_SUPPORT
// Hash in file contents
bool voSHA1::HashByFile(char *szFileName)
{
	unsigned long ulFileSize, ulRest, ulBlocks;
	unsigned long i;
	VO_U8 uData[SHA1_MAX_FILE_BUFFER];
	FILE *fIn;

	if(szFileName == NULL) return false;

	fIn = fopen(szFileName, "rb");
	if(fIn == NULL) return false;

	fseek(fIn, 0, SEEK_END);
	ulFileSize = (unsigned long)ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	if(ulFileSize != 0)
	{
		ulBlocks = ulFileSize / SHA1_MAX_FILE_BUFFER;
		ulRest = ulFileSize % SHA1_MAX_FILE_BUFFER;
	}
	else
	{
		ulBlocks = 0;
		ulRest = 0;
	}

	for(i = 0; i < ulBlocks; i++)
	{
		fread(uData, 1, SHA1_MAX_FILE_BUFFER, fIn);
		Process((VO_U8 *)uData, SHA1_MAX_FILE_BUFFER);
	}

	if(ulRest != 0)
	{
		fread(uData, 1, ulRest, fIn);
		Process((VO_U8 *)uData, ulRest);
	}

	fclose(fIn); fIn = NULL;
	return true;
}
#endif

void voSHA1::Finalize()
{
	VO_U32 i;
	VO_U8 finalcount[8];

	for(i = 0; i < 8; i++)
		finalcount[i] = (VO_U8)((m_nCount[((i >= 4) ? 0 : 1)]
			>> ((3 - (i & 3)) * 8) ) & 255); // Endian independent

	Process((VO_U8 *)"\200", 1);

	while ((m_nCount[0] & 504) != 448)
		Process((VO_U8 *)"\0", 1);

	Process(finalcount, 8); // Cause a SHA1DoTransform()

	for(i = 0; i < 20; i++)
	{
		m_nDigest[i] = (VO_U8)((m_nState[i >> 2] >> ((3 - (i & 3)) * 8) ) & 255);
	}

	// Wipe variables for security reasons
#ifdef VO_SHA1_WIPE_MODE
	i = 0;
	memset(m_nBuffer, 0, 64);
	memset(m_nState, 0, 20);
	memset(m_nCount, 0, 8);
	memset(finalcount, 0, 8);
	DoTransform(m_nState, m_nBuffer);
#endif
}

#ifdef VO_SHA1_UTILITY_SUPPORT
// Get the final hash as a pre-formatted string
void voSHA1::ReportHashByFormat(char *szReport, unsigned char uReportType)
{
	unsigned char i;
	char szTemp[16];

	if(szReport == NULL) return;

	if(uReportType == VO_REPORT_FMT_HEX)
	{
		sprintf(szTemp, "%02X", m_nDigest[0]);
		strcat(szReport, szTemp);

		for(i = 1; i < 20; i++)
		{
			sprintf(szTemp, " %02X", m_nDigest[i]);
			strcat(szReport, szTemp);
		}
	}
	else if(uReportType == VO_REPORT_FMT_DIGIT)
	{
		sprintf(szTemp, "%u", m_nDigest[0]);
		strcat(szReport, szTemp);

		for(i = 1; i < 20; i++)
		{
			sprintf(szTemp, " %u", m_nDigest[i]);
			strcat(szReport, szTemp);
		}
	}
	else strcpy(szReport, "Error: Unknown report type!");
}
#endif

// Get the raw message digest
void voSHA1::GetHashData(VO_U8 *puDest)
{
	memcpy(puDest, m_nDigest, 20);
}
