
#ifndef __FILEFORMAT_CHECKER_H__

#define __FILEFORMAT_CHECKER_H__

#ifdef _LIB

#include "voSource.h"
#include "voMP4.h"

#endif





#include "voType.h"
#include "..\ID3\ID3v2DataStruct.h"
#include "vo_buffer_stream.h"

const VO_U32 s_dwMP3SamplingRates[4][3] = 
{
	{11025, 12000, 8000,  },
	{0,     0,     0,     },
	{22050, 24000, 16000, },
	{44100, 48000, 32000  }
};

const VO_U32 s_dwMP3Bitrates[2][3][15] = 
{
	{
		{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
		{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
		{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,}
	},
	{
		{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,}
	}
};

const VO_U32 s_dwMP3Coefficients[2][3] = 
{
	{48,144,144},
	{48,144,72}
};

VO_BOOL is_mp4( VO_PBYTE ptr_buffer , VO_S32 buffersize );
VO_BOOL is_asf ( VO_PBYTE ptr_buffer , VO_S32 buffersize );
VO_BOOL is_avi( VO_PBYTE ptr_buffer , VO_S32 buffersize );
VO_BOOL is_awb(VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_amr(VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_mp3 (VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_aac (VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_wav (VO_PBYTE pHeadData, VO_S32 nHeadSize);

VO_PBYTE MP3FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
VO_BOOL MP3ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize);
VO_PBYTE AACFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
VO_BOOL AACParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize);

VO_BOOL SkipID3v2Data( vo_buffer_stream * ptr_reader )
{
	ID3v2Header header;
	if( -1 == ptr_reader->read( (VO_PBYTE)&header , ID3V2_HEADER_LEN) )
		return VO_FALSE;

	//judge if it is ID3v2 header!!
	static const VO_CHAR byteID3[3] = {0x49, 0x44, 0x33};
	if(memcmp(header.szTag, (VO_PTR)byteID3, 3))
	{
		ptr_reader->seek( -1 * ID3V2_HEADER_LEN , VO_FILE_CURRENT );
		return VO_FALSE;
	}

	//calculate the size and skip it!!
	VO_U32 dwSize = CALCULATE_SIZE_SYNCHSAFE(header.btSize);
	ptr_reader->seek( dwSize , VO_FILE_CURRENT );

	return VO_TRUE;
}

VO_BOOL get_rightlibrary( VO_PBYTE ptr_buffer , VO_S32 buffer_size , VO_TCHAR* ptr_modulename , VO_TCHAR* ptr_apiname )
{

	if( is_mp4( ptr_buffer , buffer_size ) )
	{
		vostrcpy(ptr_modulename, _T("voMP4FR"));
		vostrcpy(ptr_apiname, _T("voGetMP4ReadAPI"));
	}
	else if( is_asf( ptr_buffer , buffer_size ) )
	{
		vostrcpy(ptr_modulename, _T("voASFFR"));
		vostrcpy(ptr_apiname, _T("voGetASFReadAPI"));
	}
	else if( is_avi( ptr_buffer , buffer_size ) )
	{
		vostrcpy(ptr_modulename, _T("voAVIFR"));
		vostrcpy(ptr_apiname, _T("voGetAVIReadAPI"));
	}
	else if( is_mp3( ptr_buffer , buffer_size ) ||
		is_aac( ptr_buffer , buffer_size ) ||
		is_amr( ptr_buffer , buffer_size ) ||
		is_awb( ptr_buffer , buffer_size ) ||
		is_wav( ptr_buffer , buffer_size ) )
	{
		vostrcpy(ptr_modulename, _T("voAudioFR"));
		vostrcpy(ptr_apiname, _T("voGetAudioReadAPI"));
	}
	else
	{
		return VO_FALSE;
	}

#if defined _WIN32
	vostrcat(ptr_modulename, _T(".Dll"));
#elif defined LINUX
	vostrcat(ptr_modulename, _T(".so"));
#endif

	return VO_TRUE;
}

#ifdef _LIB

VO_BOOL get_api( VO_PBYTE ptr_buffer , VO_S32 buffer_size , VO_SOURCE_READAPI * ptr_api )
{
	if( is_mp4( ptr_buffer , buffer_size ) )
	{
		if( 0 == voGetMP4ReadAPI( ptr_api , 0 ) )
			return VO_TRUE;
	}

	return VO_FALSE;
}

#endif


VO_BOOL is_mp4( VO_PBYTE ptr_buffer , VO_S32 buffersize )
{
	VOLOGE( "is_mp4?" );
	if(!ptr_buffer || buffersize < 8)
		return VO_FALSE;

	VOLOGE( "begin determine" );

	VO_PBYTE p = ptr_buffer;
	if(p[4] == 'f' && p[5] == 't' && p[6] == 'y' && p[7] == 'p')	//file type box!!
	{
		VO_U32 dwSize = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
		if(buffersize < dwSize + 8)
			return VO_TRUE;

		p += dwSize;
	}

	//top level box type
	if(p[4] == 'm' && p[5] == 'o' && p[6] == 'o' && p[7] == 'v')
		return VO_TRUE;

	if(p[4] == 'm' && p[5] == 'o' && p[6] == 'o' && p[7] == 'f')
		return VO_TRUE;

	if(p[4] == 'm' && p[5] == 'f' && p[6] == 'r' && p[7] == 'a')
		return VO_TRUE;

	if(p[4] == 'm' && p[5] == 'd' && p[6] == 'a' && p[7] == 't')
		return VO_TRUE;

	if(p[4] == 'f' && p[5] == 'r' && p[6] == 'e' && p[7] == 'e')
		return VO_TRUE;

	if(p[4] == 's' && p[5] == 'k' && p[6] == 'i' && p[7] == 'p')
		return VO_TRUE;

	if(p[4] == 'u' && p[5] == 'd' && p[6] == 't' && p[7] == 'a')
		return VO_TRUE;

	if(p[4] == 'b' && p[5] == 'r' && p[6] == 'o' && p[7] == 'g')
		return VO_TRUE;

	if(p[4] == 'u' && p[5] == 'u' && p[6] == 'i' && p[7] == 'd')
		return VO_TRUE;

	VOLOGE( "Failed!" );

	return VO_FALSE;
}

VO_BOOL is_asf ( VO_PBYTE ptr_buffer , VO_S32 buffersize )
{
	if (ptr_buffer == NULL || buffersize < 16)
		return VO_FALSE;

	//{75b22630-668e-11cf-a6d9-00aa0062ce6c}
	VO_PBYTE p = ptr_buffer;
	if(p[0] != 0x30) return VO_FALSE;
	if(p[1] != 0x26) return VO_FALSE;
	if(p[2] != 0xb2) return VO_FALSE;
	if(p[3] != 0x75) return VO_FALSE;
	if(p[4] != 0x8e) return VO_FALSE;
	if(p[5] != 0x66) return VO_FALSE;
	if(p[6] != 0xcf) return VO_FALSE;
	if(p[7] != 0x11) return VO_FALSE;
	if(p[8] != 0xa6) return VO_FALSE;
	if(p[9] != 0xd9) return VO_FALSE;
	if(p[10] != 0x00) return VO_FALSE;
	if(p[11] != 0xaa) return VO_FALSE;
	if(p[12] != 0x00) return VO_FALSE;
	if(p[13] != 0x62) return VO_FALSE;
	if(p[14] != 0xce) return VO_FALSE;
	if(p[15] != 0x6c) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL is_avi( VO_PBYTE ptr_buffer , VO_S32 buffersize )
{
	if (ptr_buffer == NULL || buffersize < 12)
		return VO_FALSE;

	VO_PBYTE p = ptr_buffer;
	//"RIFF"
	if(p[0] != 0x52) return VO_FALSE;
	if(p[1] != 0x49) return VO_FALSE;
	if(p[2] != 0x46) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;
	p += 8;
	//"AVI "
	if(p[0] != 0x41) return VO_FALSE;
	if(p[1] != 0x56) return VO_FALSE;
	if(p[2] != 0x49) return VO_FALSE;
	if(p[3] != 0x20) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL is_mp3 (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	VO_PBYTE pBuf = pHeadData;
	VO_U32 dwBufSize = nHeadSize;
	VO_PBYTE pMP3Head = NULL;
	VO_BYTE btMP3Head[4];

	VO_U32 dwFrameSize = 0;
	while(pMP3Head = MP3FindHeadInBuffer(pBuf, dwBufSize))
	{
		dwBufSize -= (pMP3Head - pBuf);
		pBuf = pMP3Head;

		if(MP3ParseHeader(pMP3Head, &dwFrameSize))
		{
			memcpy( btMP3Head, pMP3Head, sizeof(btMP3Head));
			btMP3Head[2] &= 0x0C;
			btMP3Head[3] &= 0x8C;

			//check next header!!
			if(dwBufSize < dwFrameSize + 4)
				return VO_FALSE;

			pMP3Head = pBuf + dwFrameSize;

			if(!memcmp( btMP3Head, pMP3Head, 2) && btMP3Head[2] == (pMP3Head[2] & 0x0C) && btMP3Head[3] == (pMP3Head[3] & 0x8C))	//this header is what we want to find!!
				return VO_TRUE;
		}

		dwBufSize--;
		pBuf++;
	}

	return VO_FALSE;
}

VO_BOOL is_aac (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	//check if ADIF format!!
	if(pHeadData[0] == 0x41 && pHeadData[1] == 0x44 && pHeadData[2] == 0x49 && pHeadData[3] == 0x46)
		return VO_TRUE;

	VO_PBYTE pBuf = pHeadData;
	VO_U32 dwBufSize = nHeadSize;
	VO_PBYTE pAACHead = NULL;
	VO_BYTE btAACHead[4];

	VO_U32 dwFrameSize = 0;
	while(pAACHead = AACFindHeadInBuffer(pBuf, dwBufSize))
	{
		dwBufSize -= (pAACHead - pBuf);
		pBuf = pAACHead;
		if(AACParseHeader(pAACHead, &dwFrameSize))
		{
			memcpy( btAACHead, pAACHead, sizeof(btAACHead));
			btAACHead[3] &= 0xF0;

			//check next header!!
			if(dwBufSize < dwFrameSize + 6)
				return VO_FALSE;

			pAACHead = pBuf + dwFrameSize;
			if(!memcmp( btAACHead, pAACHead, 3) && btAACHead[3] == (pAACHead[3] & 0xF0))	//this header is what we want to find!!
				return VO_TRUE;
		}

		dwBufSize--;
		pBuf++;
	}

	return VO_FALSE;
}

VO_BOOL is_amr(VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 6)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"#!AMR\n"
	if(p[0] != 0x23) return VO_FALSE;
	if(p[1] != 0x21) return VO_FALSE;
	if(p[2] != 0x41) return VO_FALSE;
	if(p[3] != 0x4d) return VO_FALSE;
	if(p[4] != 0x52) return VO_FALSE;
	if(p[5] != 0x0a) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL is_awb(VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 9)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"#!AMR-WB\n"
	if(p[0] != 0x23) return VO_FALSE;
	if(p[1] != 0x21) return VO_FALSE;
	if(p[2] != 0x41) return VO_FALSE;
	if(p[3] != 0x4d) return VO_FALSE;
	if(p[4] != 0x52) return VO_FALSE;
	if(p[5] != 0x2d) return VO_FALSE;
	if(p[6] != 0x57) return VO_FALSE;
	if(p[7] != 0x42) return VO_FALSE;
	if(p[8] != 0x0a) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL is_wav (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 12)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"RIFF"
	if(p[0] != 0x52) return VO_FALSE;
	if(p[1] != 0x49) return VO_FALSE;
	if(p[2] != 0x46) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;
	p += 8;
	//"WAVE"
	if(p[0] != 0x57) return VO_FALSE;
	if(p[1] != 0x41) return VO_FALSE;
	if(p[2] != 0x56) return VO_FALSE;
	if(p[3] != 0x45) return VO_FALSE;

	return VO_TRUE;
}


VO_PBYTE MP3FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	if(dwLen < 4)
		return NULL;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - 3;
	while(pHead < pTail)
	{
		if(0xFF != pHead[0])
		{
			pHead++;
			continue;
		}

		//0x11111111 found!!
		if(0xE0 != (pHead[1] & 0xE0))
		{
			pHead += 2;
			continue;
		}

		//0x11111111 111XXXXX found!!
		return pHead;
	}

	return NULL;
}

VO_BOOL MP3ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize)
{
	//check version
	VO_BYTE ver = (pHeader[1] >> 3) & 0x03;
	if(1 == ver)
		return VO_FALSE;

	//check layer
	VO_BYTE layer = 3 - ((pHeader[1] >> 1) & 0x03);
	if(3 == layer)
		return VO_FALSE;

	//check sampling rate
	VO_BYTE idx = (pHeader[2] >> 2) & 0x03;
	if(0x03 == idx)	//reserved
		return VO_FALSE;

	VO_U32 dwSamplesPerSec = s_dwMP3SamplingRates[ver][idx];

	//check bit rate
	idx = (pHeader[2] >> 4) & 0x0F;
	if(0xF == idx)	//free or bad
		return VO_FALSE;

	VO_BYTE bLSF = (3 != ver) ? 1 : 0;
	VO_U32 dwBitrate = s_dwMP3Bitrates[bLSF][layer][idx] * 1000;	//KBit to Bit
	if(!dwBitrate)
		return VO_FALSE;

	if(pdwFrameSize)
	{
		//padding bit
		VO_BYTE btPaddingBit = (pHeader[2] >> 1) & 0x01;

		//frame size;
		if(0 == layer)
			*pdwFrameSize = (12 * dwBitrate / dwSamplesPerSec + btPaddingBit) * 4;
		else
			*pdwFrameSize = s_dwMP3Coefficients[bLSF][layer] * dwBitrate / dwSamplesPerSec + btPaddingBit;
	}

	return VO_TRUE;
}

VO_PBYTE AACFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	if(dwLen < 6)
		return NULL;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - 5;
	while(pHead < pTail)
	{
		if(0xFF != pHead[0])
		{
			pHead++;
			continue;
		}

		//0x11111111 found!!
		if(0xF0 != (pHead[1] & 0xF0))
		{
			pHead += 2;
			continue;
		}

		//0x11111111 1111XXXX found!!
		return pHead;
	}

	return NULL;
}

VO_BOOL AACParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize)
{
	//check ID
	//if(!(pHeader[1] & 0x8))
	//	return VO_FALSE;

	//check Layer
	//if(pHeader[1] & 0x6)
	//	return VO_FALSE;

	//check Profile
	if(0x3 == (pHeader[2] >> 6))
		return VO_FALSE;

	//check Sampling rate frequency index
	if(((pHeader[2] >> 2) & 0xF) > 0xB)
		return VO_FALSE;

	VO_U32 dwFrameSize = ((pHeader[3] & 0x03) << 11) | (pHeader[4] << 3) | (pHeader[5] >> 5);
	if(dwFrameSize < 6)
		return VO_FALSE;

	if(pdwFrameSize)
		*pdwFrameSize = dwFrameSize;

	return VO_TRUE;
}

























#endif