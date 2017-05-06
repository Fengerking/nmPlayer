
#ifndef __FILEFORMAT_CHECKER_H__

#define __FILEFORMAT_CHECKER_H__

#ifdef _IOS

#include "voSource.h"
#include "voAudioFile.h"
#include "voASF.h"
#include "voAVI.h"
#include "voFLV.h"
#include "voMKV.h"
#include "voMP4.h"
#include "voMpegFile.h"
#include "voOGGFile.h"
#include "voRealFile.h"
#include "voTS.h"

#endif


#include "voType.h"
#include "../ID3/ID3v2DataStruct.h"
#include "vo_buffer_stream.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

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

enum PD_MEDIA_FORMAT
{
	PD_MEDIA_UNKNOWN,
	PD_MEDIA_MP4,
	PD_MEDIA_MOV,
	PD_MEDIA_ASF,
	PD_MEDIA_AVI,
	PD_MEDIA_AWB,
	PD_MEDIA_AMR,
	PD_MEDIA_MP3,
	PD_MEDIA_AAC,
	PD_MEDIA_WAV,
	PD_MEDIA_AC3,
	PD_MEDIA_FLAC,
	PD_MEDIA_QCP,
	PD_MEDIA_FLV,
	PD_MEDIA_MKV,
	PD_MEDIA_OGG,
	PD_MEDIA_REAL,
	PD_MEDIA_PS,
	PD_MEDIA_TS,
	PD_MEDIA_PLS,
	PD_MEDIA_M3U,
};

VO_PBYTE FindTSPacketHeader(VO_PBYTE pData, VO_S32 nDataSize, VO_S32 nPacketSize);
VO_BOOL FindTSPacketHeader2(VO_PBYTE pData, VO_S32 cbData, VO_S32 packetSize);


VO_BOOL is_mp4( VO_PBYTE ptr_buffer , VO_S32 buffersize );
VO_BOOL is_mov( VO_PBYTE ptr_buffer , VO_S32 buffersize );
VO_BOOL is_asf ( VO_PBYTE ptr_buffer , VO_S32 buffersize );
VO_BOOL is_ac3 ( VO_PBYTE ptr_buffer , VO_S32 buffersize );
VO_BOOL is_flac (VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_qcp (VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_real(VO_PBYTE ptr_buffer, VO_S32 buffersize);
VO_BOOL is_avi( VO_PBYTE ptr_buffer , VO_S32 buffersize );
VO_BOOL is_awb(VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_amr(VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_mp3 (VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_aac (VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_wav (VO_PBYTE pHeadData, VO_S32 nHeadSize);
VO_BOOL is_flv( VO_PBYTE pHeadData, VO_S32 nHeadSize );
VO_BOOL is_mkv( VO_PBYTE pHeadData, VO_S32 nHeadSize );
VO_BOOL is_ogg( VO_PBYTE pHeadData, VO_S32 nHeadSize );
VO_BOOL is_ps ( VO_PBYTE pHeadData , VO_S32 nHeadSize );
VO_BOOL is_ts ( VO_PBYTE pHeadData , VO_S32 nHeadSize );
//pls or m3u playlist
VO_BOOL is_pls( VO_PBYTE pHeadData, VO_S32 nHeadSize );
VO_BOOL is_m3u( VO_PBYTE pHeadData, VO_S32 nHeadSize );

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

PD_MEDIA_FORMAT get_rightlibrary( VO_PBYTE ptr_buffer , VO_S32 buffer_size , VO_TCHAR* ptr_modulename , VO_TCHAR* ptr_apiname )
{
	PD_MEDIA_FORMAT mf = PD_MEDIA_UNKNOWN;

	if( is_mov( ptr_buffer , buffer_size ) )
	{
		VOLOGI("MOV Use MP4 FileParser" );
		vostrcpy(ptr_modulename, _T("voMP4FR"));
		vostrcpy(ptr_apiname, _T("voGetMP4ReadAPI"));
		mf = PD_MEDIA_MOV;

		//for gemtek case, we should disable MOV support
		//return PD_MEDIA_UNKNOWN;
	}
	else if( is_mp4( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use MP4 FileParser" );
		vostrcpy(ptr_modulename, _T("voMP4FR"));
		vostrcpy(ptr_apiname, _T("voGetMP4ReadAPI"));
		mf = PD_MEDIA_MP4;
	}
	else if( is_asf( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use ASF FileParser" );
		vostrcpy(ptr_modulename, _T("voASFFR"));
		vostrcpy(ptr_apiname, _T("voGetASFReadAPI"));
		mf = PD_MEDIA_ASF;
	}
	else if(is_ts( ptr_buffer , buffer_size ))
	{
		VOLOGI("Use TS FileParser" );
		vostrcpy(ptr_modulename, _T("voTsParser"));
		vostrcpy(ptr_apiname, _T("voGetMTVReadAPI"));
		mf = PD_MEDIA_TS;
	}
	else if(is_ps( ptr_buffer , buffer_size))
	{
		VOLOGI("Use PS FileParser" );
		vostrcpy(ptr_modulename, _T("voMPGFR"));
		vostrcpy(ptr_apiname, _T("voGetMPGReadAPI"));
		mf = PD_MEDIA_PS;
	}
	else if( is_real( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use REAL FileParser");
		vostrcpy(ptr_modulename, _T("voRealFR"));
		vostrcpy(ptr_apiname, _T("voGetRealReadAPI"));
		mf = PD_MEDIA_REAL;
	}
	else if( is_avi( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use AVI FileParser" );
		vostrcpy(ptr_modulename, _T("voAVIFR"));
		vostrcpy(ptr_apiname, _T("voGetAVIReadAPI"));
		mf = PD_MEDIA_AVI;
	}
	else if( is_flv( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use FLV FileParser" );
		vostrcpy(ptr_modulename, _T("voFLVFR"));
		vostrcpy(ptr_apiname, _T("voGetFLVReadAPI"));
		mf = PD_MEDIA_FLV;
	}
	else if( is_mkv( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use MKV FileParser" );
		vostrcpy(ptr_modulename, _T("voMKVFR"));
		vostrcpy(ptr_apiname, _T("voGetMKVReadAPI"));
		mf = PD_MEDIA_MKV;
	}
	else if( is_ogg( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use Ogg FileParser" );
		vostrcpy(ptr_modulename, _T("voOGGFR"));
		vostrcpy(ptr_apiname, _T("voGetOGGReadAPI"));
		mf = PD_MEDIA_OGG;
	}
	else if( is_mp3( ptr_buffer , buffer_size ))
	{
		VOLOGI("Use Audio FileParser" );
		vostrcpy(ptr_modulename, _T("voAudioFR"));
		vostrcpy(ptr_apiname, _T("voGetAudioReadAPI"));
		mf = PD_MEDIA_MP3;
	}
	else if( is_aac( ptr_buffer , buffer_size )) 
	{
		VOLOGI("Use Audio FileParser" );
		vostrcpy(ptr_modulename, _T("voAudioFR"));
		vostrcpy(ptr_apiname, _T("voGetAudioReadAPI"));
		mf = PD_MEDIA_AAC;
	}
	else if(is_amr( ptr_buffer , buffer_size ))
	{
		VOLOGI("Use Audio FileParser" );
		vostrcpy(ptr_modulename, _T("voAudioFR"));
		vostrcpy(ptr_apiname, _T("voGetAudioReadAPI"));
		mf = PD_MEDIA_AMR;
	}
	else if(is_awb( ptr_buffer , buffer_size ))
	{
		VOLOGI("Use Audio FileParser" );
		vostrcpy(ptr_modulename, _T("voAudioFR"));
		vostrcpy(ptr_apiname, _T("voGetAudioReadAPI"));
		mf = PD_MEDIA_AWB;
	}
	else if(is_wav( ptr_buffer , buffer_size ))
	{
		VOLOGI("Use Audio FileParser" );
		vostrcpy(ptr_modulename, _T("voAudioFR"));
		vostrcpy(ptr_apiname, _T("voGetAudioReadAPI"));
		mf = PD_MEDIA_WAV;
	}
	else if(is_ac3( ptr_buffer , buffer_size ))
	{
		VOLOGI("Use Audio FileParser" );
		vostrcpy(ptr_modulename, _T("voAudioFR"));
		vostrcpy(ptr_apiname, _T("voGetAudioReadAPI"));
		mf = PD_MEDIA_AC3;
	}
	else if( is_flac( ptr_buffer , buffer_size ))
	{
		VOLOGI("Use Audio FileParser" );
		vostrcpy(ptr_modulename, _T("voAudioFR"));
		vostrcpy(ptr_apiname, _T("voGetAudioReadAPI"));
		mf = PD_MEDIA_FLAC;
	}
	else if( is_qcp ( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use Audio FileParser" );
		vostrcpy(ptr_modulename, _T("voAudioFR"));
		vostrcpy(ptr_apiname, _T("voGetAudioReadAPI"));
		mf = PD_MEDIA_QCP;
	}
	else if( is_pls( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use pls playlist" );

		mf = PD_MEDIA_PLS;
	}
	else if( is_m3u( ptr_buffer , buffer_size ) )
	{
		VOLOGI("Use m3u playlist" );

		mf = PD_MEDIA_M3U;
	}
	else
	{
		VOLOGI("Can not find Right File Parser!" );
		return PD_MEDIA_UNKNOWN;
	}

#if defined _WIN32
	vostrcat(ptr_modulename, _T(".Dll"));
#elif defined LINUX
	vostrcat(ptr_modulename, _T(".so"));
#endif

	return mf;
}

//pls or m3u playlist
VO_BOOL is_pls( VO_PBYTE pHeadData, VO_S32 nHeadSize )
{
	if( memcmp( pHeadData , "[playlist]" , strlen("[playlist]") ) )
	{
		return VO_FALSE;
	}
	else
	{

		return VO_TRUE;
	}
}


VO_BOOL is_m3u( VO_PBYTE pHeadData, VO_S32 nHeadSize )
{
	if( memcmp( pHeadData , "#EXTM3U"  , strlen("#EXTM3U")) && memcmp( pHeadData , "http://"  , strlen("http://")) )
	{
		return VO_FALSE;
	}
	else
	{

		return VO_TRUE;
	}
}

VO_PBYTE FindTSPacketHeader(VO_PBYTE pData, VO_S32 nDataSize, VO_S32 nPacketSize)
{
	VO_PBYTE p1 = pData;
	VO_PBYTE p2 = pData + nDataSize - nPacketSize;
	while((p1 < p2) && (p1 - pData <= nPacketSize))
	{
		if((*p1 == 0x47) && (*(p1 + nPacketSize) == 0x47))
			return p1;
		p1++;
	}
	return NULL;
}

VO_BOOL FindTSPacketHeader2(VO_PBYTE pData, VO_S32 cbData, VO_S32 packetSize)
{
	VO_PBYTE p = FindTSPacketHeader(pData, cbData, packetSize);
	if (p)
	{
		VO_PBYTE p2 = p + packetSize * 2;
		if (p2 < pData + cbData)
		{
			if (*p2 == 0x47)
				return VO_TRUE;
		}
	}
	return VO_FALSE;
}

VO_BOOL is_ts ( VO_PBYTE ptr_buffer , VO_S32 buffersize )
{
	if (ptr_buffer == NULL)
		return VO_FALSE;

	if (buffersize >= 189 && FindTSPacketHeader2(ptr_buffer, buffersize, 188))
		return VO_TRUE;

	if (buffersize >= 193 && FindTSPacketHeader2(ptr_buffer, buffersize, 192))
		return VO_TRUE;

	if (buffersize >= 205 && FindTSPacketHeader2(ptr_buffer, buffersize, 204))
		return VO_TRUE;

	return VO_FALSE;
}

VO_PBYTE MPGFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen;
	VO_U32 LeftData = 0xFFFFFFFF;
	const VO_U32 wAVCSyncWord = 0x000001BA;///<or 0x000001
	while(pHead < pTail)
	{
		LeftData <<= 8;
		LeftData |= *pHead++;
		if (LeftData == wAVCSyncWord)
		{
			return pHead;
		}
	}
	return NULL;
}

VO_BOOL MPGParseHeader(VO_PBYTE pHeader, VO_U32 dwLen)
{
	if (dwLen < 11)///<8 bytes's MPEG1 Packer header+ next 3 bytes sync word
	{
		return VO_FALSE;
	}
	VO_PBYTE pHead = pHeader;
	VO_BOOL beMPEG2 = ((pHead[0] >> 6)==0x01)?VO_TRUE:VO_FALSE;
	if(!beMPEG2 && (pHead[0] >> 4) != 0x2)///<not mpeg1
		return VO_FALSE;
	
	if (beMPEG2)
	{
		if (dwLen < 13)///<10 bytes's MPEG1 Packer header+ next 3 bytes's sync word
		{
			return VO_FALSE;
		}
		if(((pHead[0]&0x4) != 0x4) || ((pHead[2]&0x4) != 0x4) 
			||((pHead[4]&0x4) != 0x4) || ((pHead[5]&0x1) != 0x1) 
			|| ((pHead[8] & 0x3) != 0x3))
		{
			return VO_FALSE;
		}
		pHead += 10;

	}
	else
	{
		if (((pHead[0] & 0x1) != 0x1) || ((pHead[2] & 0x1) != 0x1)
			|| ((pHead[4] & 0x1) != 0x1) || ((pHead[5] & 0x80) != 0x80)|| ((pHead[7] & 0x1) != 0x1)
			)
		{
			return VO_FALSE;
		}
		pHead += 8;
	}
	if (pHead[0] != 0x0 || pHead[1] != 0x0 || pHead[2] != 0x1)///<next synec work 0x000001
	{
		return VO_FALSE;
	}
	return VO_TRUE;
}

VO_BOOL is_ps (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{	
	if (!pHeadData || nHeadSize < 15)///<4 bytes's syncword + 8 bytes's MPEG1 Packer header+ next 3 bytes sync word
	{
		return VO_FALSE;
	}
	VO_PBYTE pMPGHead = MPGFindHeadInBuffer(pHeadData,nHeadSize);
	
	if(!pMPGHead || !MPGParseHeader(pMPGHead,nHeadSize - (pMPGHead - pHeadData)))
		return VO_FALSE;
	
	return VO_TRUE; 
}


VO_BOOL is_mov ( VO_PBYTE ptr_buffer , VO_S32 buffersize )
{
	if (ptr_buffer == NULL || buffersize < 10)
		return VO_FALSE;

	VO_PBYTE p = ptr_buffer + 4;
	if (p[0] != 'f') return VO_FALSE;
	if (p[1] != 't') return VO_FALSE;
	if (p[2] != 'y') return VO_FALSE;
	if (p[3] != 'p') return VO_FALSE;
	if ( (p[4] == 'q') && (p[5] == 't') ) //qt
		return VO_TRUE;

	return VO_FALSE;
}

VO_BOOL is_flac (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"fLaC"
	if(p[0] != 0x66) return VO_FALSE;
	if(p[1] != 0x4c) return VO_FALSE;
	if(p[2] != 0x61) return VO_FALSE;
	if(p[3] != 0x43) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL is_qcp (VO_PBYTE pHeadData, VO_S32 nHeadSize)
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
	//"QLCM"
	if(p[0] != 0x51) return VO_FALSE;
	if(p[1] != 0x4c) return VO_FALSE;
	if(p[2] != 0x43) return VO_FALSE;
	if(p[3] != 0x4d) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL is_ac3 ( VO_PBYTE ptr_buffer , VO_S32 buffersize )
{
	const VO_U16 vo_ac3_frame_size_tab[38][3] = { { 64,   69,   96   },  { 64,   70,   96   },{ 80,   87,   120  }, { 80,   88,   120  },
		{ 96,   104,  144  },   { 96,   105,  144  },   { 112,  121,  168  },  { 112,  122,  168  }, { 128,  139,  192  },  { 128,  140,  192  },
		{ 160,  174,  240  }, { 160,  175,  240  }, { 192,  208,  288  }, { 192,  209,  288  }, { 224,  243,  336  },  { 224,  244,  336  },
		{ 256,  278,  384  },{ 256,  279,  384  },{ 320,  348,  480  },{ 320,  349,  480  },{ 384,  417,  576  },{ 384,  418,  576  },
		{ 448,  487,  672  }, { 448,  488,  672  },{ 512,  557,  768  },{ 512,  558,  768  },{ 640,  696,  960  },{ 640,  697,  960  }, { 768,  835,  1152 },
		{ 768,  836,  1152 }, { 896,  975,  1344 },{ 896,  976,  1344 },{ 1024, 1114, 1536 },{ 1024, 1115, 1536 },{ 1152, 1253, 1728 },  { 1152, 1254, 1728 },
		{ 1280, 1393, 1920 },  { 1280, 1394, 1920 },
	};
	VO_U16 FirstFrmPos = 0;
	VO_U16 syn_word = 0;
	VO_U8 frmsizecode = 0;
	VO_U8 streadid = 0;
	VO_U16 framesize = 0;
	VO_PBYTE p = ptr_buffer;

	while(VO_TRUE)
	{
		syn_word = (((syn_word&0x0000)|(p[FirstFrmPos]))<<8)|(((syn_word&0xFF00)|p[FirstFrmPos+1]));
		if(0x0B77 == syn_word)
		{
			//VO_U8 strmid_frmsize = 0;
			frmsizecode = p[FirstFrmPos + 4]&0x3F;
			streadid = (p[FirstFrmPos + 4]&0xB0)>>6;
			framesize = vo_ac3_frame_size_tab[frmsizecode][streadid]*2;
			syn_word = (((syn_word&0x0000)|(p[FirstFrmPos + framesize]))<<8)|(((syn_word&0xFF00)|p[FirstFrmPos + framesize + 1]));
			if(0x0B77 == syn_word)// a ac3 format file
			{
				break;
			}

		}

		if((FirstFrmPos + 2) < buffersize)
		{
			FirstFrmPos += 2;
		}
		else
		{
			break;
		}
	}	

	if(FirstFrmPos + 2 >= buffersize)
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL is_mp4( VO_PBYTE ptr_buffer , VO_S32 buffersize )
{
	if(!ptr_buffer || buffersize < 8)
		return VO_FALSE;

	VO_U32 totalsum = 0;
	VO_U32 dwSize = 0;

	VO_PBYTE p = ptr_buffer;
	if(p[4] == 'f' && p[5] == 't' && p[6] == 'y' && p[7] == 'p')	//file type box!!
	{
		dwSize = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
		if((VO_U32)buffersize < dwSize + 8)
			return VO_TRUE;

		p += dwSize;
		totalsum += dwSize;
	}

	
	do
	{
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

		dwSize = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
		totalsum += dwSize;
		
		if(totalsum + 8 >  (VO_U32)buffersize || dwSize == 0)
		{
			return VO_FALSE;
		}

		//skip user-defined top level box type
		p += dwSize;

	}while(dwSize < 256);

	return VO_FALSE;
}

VO_BOOL is_real(VO_PBYTE ptr_buffer, VO_S32 buffersize)
{
	if (ptr_buffer == NULL || buffersize < 4)
		return VO_FALSE;

	VO_PBYTE p = ptr_buffer;
	//".RMF"
	if(p[0] != 0x2e) return VO_FALSE;
	if(p[1] != 0x52) return VO_FALSE;
	if(p[2] != 0x4d) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;

	return VO_TRUE;
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
	while( (pMP3Head = MP3FindHeadInBuffer(pBuf, dwBufSize)) )
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
			{
				VOLOGI("Use mp3 FileParser" );
				return VO_TRUE;
			}
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
	{
		VOLOGI("Use aac FileParser" );
		return VO_TRUE;
	}

	VO_PBYTE pBuf = pHeadData;
	VO_U32 dwBufSize = nHeadSize;
	VO_PBYTE pAACHead = NULL;
	VO_BYTE btAACHead[4];

	VO_U32 dwFrameSize = 0;
	while( (pAACHead = AACFindHeadInBuffer(pBuf, dwBufSize)) )
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
			{
				VOLOGI("Use aac FileParser" );
				return VO_TRUE;
			}
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

	VOLOGI("Use amr FileParser" );
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

	VOLOGI("Use awb FileParser" );
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

	VOLOGI("Use wav FileParser" );
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

VO_BOOL is_flv( VO_PBYTE pHeadData, VO_S32 nHeadSize )
{
	if (pHeadData == NULL || nHeadSize < 3)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"FLV"
	if(p[0] != 0x46) return VO_FALSE;
	if(p[1] != 0x4c) return VO_FALSE;
	if(p[2] != 0x56) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL is_mkv( VO_PBYTE pHeadData, VO_S32 nHeadSize )
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//IS EBML
	if(p[0] != 0x1a) return VO_FALSE;
	if(p[1] != 0x45) return VO_FALSE;
	if(p[2] != 0xdf) return VO_FALSE;
	if(p[3] != 0xa3) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL is_ogg( VO_PBYTE pHeadData, VO_S32 nHeadSize )
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//IS Oggs
	if(p[0] != 0x4f) return VO_FALSE;
	if(p[1] != 0x67) return VO_FALSE;
	if(p[2] != 0x67) return VO_FALSE;
	if(p[3] != 0x53) return VO_FALSE;

	return VO_TRUE;
}

#ifdef _VONAMESPACE
}
#endif

#endif
