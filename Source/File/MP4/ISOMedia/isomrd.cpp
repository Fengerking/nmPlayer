#include "isomrd.h"
#include "isomscan.h"
#include "mpxutil.h"
#include "voLog.h"
#include <time.h>
#include "voOSFunc.h"
#include "ISOBMFileDataStruct.h"
#include "fragmentTrack.h"
#include "SegmentTrack.h"
#include "subtitleTrack.h"
#include "CvoBaseDrmCallback.h"
#include "CMp4Reader2.h"
#ifndef _IOS
#include <assert.h>
#define ASSERT assert
#else
#undef ASSERT
#define ASSERT(x) if (!(x)) VOLOGE("ASSERT FAILED: " #x)
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
	uint64 _time_divisor = 1000; //default 1/1000 s


#define MAX_SAMPLE_SIZE 0x800000 //8M, for error check

	/* seconds since 1 Jan 1904 */ 
#define UTC_OFFSET 2082844800




	void SetTimeUnit(uint32 divisor)
	{
		_time_divisor = divisor;
	}

	void ConverTime(uint64 time, char *strTime)
	{
		//modified by Ma Tao for WinCE build
#if  defined(_WIN32_WCE)
		const uint64 offset = (uint64)0x0153b281e0fb4000;
		SYSTEMTIME st;
		time *= 10000000;
		time += offset;
		FileTimeToSystemTime((FILETIME*)&time, &st);
		int i = 0;
		//20040302T182152.000Z
		sprintf(strTime +i , "%04d", st.wYear);
		sprintf(strTime +(i+=4), "%02d", st.wMonth);
		sprintf(strTime + (i += 2), "%02d", st.wDay);
		sprintf(strTime + (i+= 2),"%c", 'T');
		sprintf(strTime + (i +=1), "%02d", st.wHour);
		sprintf(strTime + (i +=2), "%02d", st.wMinute);
		sprintf(strTime + (i +=2), "%02d", st.wSecond);
		sprintf(strTime + (i +=2), "%s", ".000Z");
		strTime[i+5] = '\0';
#else 
		if(time == 0) return ;
		if(time< UTC_OFFSET) time = 0;
		else
			time -= UTC_OFFSET;
		time_t tt = (time_t)time;
		tm *t = gmtime(&tt);
		int i = 0;
		//20040302T182152.000Z
		sprintf(strTime +i , "%04d", t->tm_year + 1900);
		sprintf(strTime +(i+=4), "%02d", t->tm_mon+1 );
		sprintf(strTime + (i += 2), "%02d", t->tm_mday);
		sprintf(strTime + (i+= 2),"%c", 'T');
		sprintf(strTime + (i +=1), "%02d", t->tm_hour);
		sprintf(strTime + (i +=2), "%02d", t->tm_min);
		sprintf(strTime + (i +=2), "%02d", t->tm_sec);
		sprintf(strTime + (i +=2), "%s", ".000Z");
		strTime[i+5] = '\0';
#endif 
		//strcpy(strTime, asctime(t));
		VOLOGR("ConverTime:%s",strTime);
	}
	//09/01/2011,leon
	MP4RRC CheckType(uint32 sampletype)
	{

		switch (sampletype)
		{
		case FOURCC2_soun:
		case FOURCC2_samr:
		case FOURCC2_sawb:
		case FOURCC2_sawp:
		case FOURCC2_mp4a:
		case FOURCC2_sevc: //EVRC, 4/28/2009, Jason
		case FOURCC2_evrc: //EVRC, 4/11/2011, Jason
		case FOURCC2__mp3:
		case FOURCC2_ac_3:
		case FOURCC2_ec_3: //eAC3, East, 2010/03/09
		case FOURCC2_alac:
		case FOURCC2_sqcp:
		case 0x6d732000: // 'ms ', AC3? 6/20/2011, Jason
		case 0x6d730055: // 'ms ', MP3 7/4/2011, Jason
		case 0x6d730011: //20120312,Leon ,ADPCM, #11375
		case FOURCC2_ulaw://6/22/2011, Jason
		case FOURCC2_sowt://8/25/2011,Leon
		case FOURCC2_alaw: 
		case FOURCC2_wma ://0x776d6120 8/25/2011,Leon
		case FOURCC2_raw :
			return MP4RRC_AUDIO_TRACK;

		case FOURCC2_DX50: //#12277
		case FOURCC2_vide:
		case FOURCC2_mp4v:
		case FOURCC2_s263:
		case FOURCC2_H263: //0x48323633 ,10/13/2008, Jason 
		case FOURCC2_s264:
		case FOURCC2_avc1://0x61766331
		case FOURCC2_vc_1://0x76632d31 8/25/2011,Leon
		case FOURCC2_jpeg:// added by Aiven, 05/02.2012
		case FOURCC2_JPEG:
		case FOURCC2_DIV3:// add by Leon, 05/16/2012
		case FOURCC2_div3:// add by Leon, 05/16/2012
		case FOURCC2_HVC1: 
		case FOURCC2_hvc1:
		case FOURCC2_hev1:
		case FOURCC2_avc3:
			return MP4RRC_VIDEO_TRACK;

//#ifdef _SUPPORT_TTEXT
		case FOURCC2_tx3g:
		case FOURCC2_stpp:
			return MP4RRC_SUBTITLE_TRACK;
//#endif //_SUPPORT_TTEXT

		case FOURCC2_drms:
			return MP4RRC_OK; //MP4RRC_DRMS_SAMPLE_TYPE;

		case FOURCC2_encv://09/01/2011,Leon 
			return MP4RRC_VIDEO_TRACK;
		case FOURCC2_enca://09/01/2011,Leon
			return MP4RRC_AUDIO_TRACK;

		case FOURCC2_rtp :
			return MP4RRC_UNSUPPORTED_SAMPLE_TYPE;
#ifdef _DEBUG
		case FOURCC2_mp4s:
			return MP4RRC_UNSUPPORTED_SAMPLE_TYPE;
#endif //_DEBUG

		default:
			return MP4RRC_UNKNOWN_SAMPLE_TYPE;
		}

		return MP4RRC_OK;
	}

#ifdef _VONAMESPACE
}
#endif


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


// BufferInStream
//==============================================================================

BufferInStream::BufferInStream()
: _ss(0)
, _bs(0)
{
}

BufferInStream::~BufferInStream()
{
	if (_bs)
	{
		delete GetBuffer();
		delete _bs;
	}
}

bool BufferInStream::Init(Stream* source, uint32 maxsize, uint32 blocksize)
{
#if 0
	_ss = source;
	_maxsize = maxsize;
	_blocksize = blocksize;

	uint8* buf = new uint8[blocksize];
	if (!buf)
		return false;
	_bs = new MPx::MemStream(buf, blocksize);
	_offset = _start = source->Position();
	uint32 readsize = GetBufSize();
	if (readsize > _maxsize)
		readsize = _maxsize;
	if (!source->Read(GetBuffer(), readsize))
		return false;
	_bs->SetSize(readsize);
	return true;
#else
	if (!Init1(source, blocksize))
		return false;
	return Init2(maxsize, source->Position());
#endif
}


bool BufferInStream::Init1(Stream* source, uint32 blocksize)
{
	if (_bs)
	{
		delete GetBuffer();
		delete _bs;
		_bs = 0;
	}
	_ss = source;
	_blocksize = blocksize;

	uint8* buf = new uint8[blocksize];
	if (!buf)
		return false;
	_bs = new MemStream(buf, blocksize);
	if(!_bs)
		return false;
	return true;
}

bool BufferInStream::Init2(uint32 maxsize, ABSPOS position)
{
	uint32 readsize = GetBufSize();
	VOLOGI("BufferInStream::Init2 maxsize: %d, readsize: %d ",maxsize,readsize);
	if (readsize > maxsize)
		readsize = maxsize;
	if (!_ss->SyncRead(position, GetBuffer(), readsize))
	{
		VOLOGI("BufferInStream::Init2------failed to read data from I/O ");
		return false;
	}
	_bs->SetSize(readsize);
	_maxsize = maxsize;
	_offset = _start = position;
	return true;
}

ABSPOS BufferInStream::Position()
{
	return _offset + _bs->Position();
}

bool BufferInStream::SetPosition(ABSPOS pos)
{
	ASSERT(pos >= _start);
	ASSERT(pos < _start + _maxsize);
	if(!_bs && !_ss) 
		return false;

//	VOLOGR("+SetPosition.bs %p",_bs);
	if (pos < _offset || pos >= _offset + GetBufSize())
	{
		//if (!_ss->SetPosition(pos))
		//	return false;
		//_offset = pos;
		//if (!_ss->Read(GetBuffer(), GetBufSize()))
		//	return false;
		VOLOGR("+SetPosition");
		_offset = (pos - _start) / GetBufSize() * GetBufSize() + _start;
		int64 readsize = GetBufSize();
		if (_offset + readsize > _start + _maxsize)
			readsize = _start + _maxsize - _offset;
			VOLOGR("-SetPosition");
		VOLOGR("_ss address :0x%08x",_ss);
		if ( !_ss->SyncRead(_offset, GetBuffer(), uint32(readsize)))
        {
            VOLOGE("SyncRead failed");
			return false;
        }
		_bs->SetSize(uint32(readsize));
	}
	bool b = _bs->SetPosition(pos - _offset);
//	VOLOGR("-SetPosition");

	return b;
}

bool BufferInStream::Move(RELPOS delta)
{
	return SetPosition(Position() + delta);
}

bool BufferInStream::Read(void* data, uint32 size)
{
	ASSERT(Position() + size <= _start + _maxsize);

	uint32 count = size;
	uint8* pDest = (uint8*) data;
	while (count > 0)
	{
		uint32 cLeft = uint32(uint64(GetBufSize()) - _bs->Position());
		if (cLeft > count)
			return _bs->Read(pDest, count);
		_bs->Read(pDest, cLeft);
		count -= cLeft;
		pDest += cLeft;
		_offset += GetBufSize();
		int64 readsize = int64(GetBufSize());
		if (_offset + readsize > _start + _maxsize)
		{
			readsize = _start + _maxsize - _offset;
			if (readsize == 0)
				break;
		}
		if (!_ss->SyncRead(_offset, GetBuffer(), uint32(readsize)))
			return false;
		_bs->SetSize(uint32(readsize));
		_bs->SetPosition(0); // reset the buffer stream
	}
	return true;
}

bool BufferInStream::Write(const void* data, uint32 size)
{
	return false;
}


bool BufferInStream::Flush()
{
	return false;
}


// Movie
//==============================================================================
Movie::Movie():_mehdduration(0),m_pPssh(0)
{
	m_bIsStreaming = VO_FALSE;
	memset(m_strGPS, 0x00, sizeof(m_strGPS));
	memset(m_stDefaultInfo, 0, sizeof(DEFAULT_SAMPLEINFO) * 10);

	m_nMinorVersion = 0;
	m_uEnableBox = VO_MAXU32;
	m_bHasFrag = VO_TRUE;
	m_DrmReader = NULL;
	m_bIsCENCDrm   = VO_FALSE;
	m_pCencDrmCallbackProc = NULL;
	m_pPssh = NULL;
}
Movie::~Movie()
{
	if(m_pPssh)
	{
		delete []m_pPssh;
		m_pPssh = NULL;
	}
}
MP4RRC Movie::Open(Reader* r)
{	VO_U32 g = voOS_GetSysTime();
    //ASSERT(5 > 11);

	MP4RRC rc = MP4RRC_OK;
	m_pReader = r;
	_headersize = 0;
	r->SetPosition(0); //reset
	Scanner scanner(r);

	int ftypSize = scanner.FindTag(FOURCC2_ftyp);
	_nFileFormat = FOURCC2_mp4l;

	if (ftypSize == 0 )
		r->SetPosition(0);

	ABSPOS startPos = r->Position() + ftypSize;

/*09/01/2011,mofidy by leon*/
	rc = ParseFtypBox(r,ftypSize);
	if (rc)
		return rc;

	VOLOGR("Open11 Cost:%d", voOS_GetSysTime() - g);
	g = voOS_GetSysTime();
	if (!r->SetPosition(startPos))
		return MP4RRC_SEEK_FAILED;
	int moovSize = scanner.FindTag(FOURCC2_moov);
	if (moovSize == 0)
		return MP4RRC_NO_MOOV;
	ABSPOS moovPos = r->Position() - 8;
	ABSPOS markPos = r->Position();
	int mvexSize = scanner.FindTag(FOURCC2_mvex,moovSize + startPos);
	if(mvexSize >0)
		ParseMvexBox(r, mvexSize);
	else
		m_bHasFrag = VO_FALSE;
	r->SetPosition(markPos);
#ifndef CENC
	int psshSize = 0;
	while(psshSize = scanner.FindTag(FOURCC2_pssh,moovSize + startPos))
	{
			if(m_pPssh)
			{
				delete []m_pPssh;
				m_pPssh = 0;
			}
			m_bIsCENCDrm = VO_TRUE;
			m_pPssh = new VO_BYTE[psshSize + 8 + 1];
			memset(m_pPssh,0,psshSize + 8 + 1);
			r->Move(-8);
			if(!r->Read(m_pPssh,psshSize + 8))
				break;
			VO_DRM2_INFO_GENERAL_DATA cencdrm_general_data;
			cencdrm_general_data.pData = m_pPssh;
			cencdrm_general_data.uSize = psshSize + 8;
			if(m_pCencDrmCallbackProc)
				m_pCencDrmCallbackProc->FRCB(m_pCencDrmCallbackProc->pUserData,NULL,NULL,NULL,NULL,VO_DRM2DATATYPE_CENC_PSSH,VO_DRM2_DATAINFO_SAMPLEDECYPTION_BOX,&cencdrm_general_data);
	}
#endif
	r->SetPosition(markPos);
	int mvhdSize = scanner.FindTag(FOURCC2_mvhd);
	if (mvhdSize == 0)
		return MP4RRC_NO_MVHD;

	// maybe udat or other boxes between moov & mvhd
	ABSPOS trakBoxesSize = moovSize - (r->Position() - 8 - moovPos) - mvhdSize;

	_cTracks = 0;
	_nTrakPos = r->Position() + mvhdSize;
	rc = ParseMvhdBox(r, mvhdSize);
	if (rc)
		return rc;
	VOLOGR("Open12 Cost:%d", voOS_GetSysTime() - g);
	g = voOS_GetSysTime();

	if (!r->SetPosition(markPos))
		return MP4RRC_SEEK_FAILED;
	int udtaSize = scanner.FindTag(FOURCC2_udta,markPos + moovSize);
	if (udtaSize > 0)
	{
		ParseUdtaBox(r, udtaSize);
	}

	VOLOGR("Open13 Cost:%d", voOS_GetSysTime() - g);
	g = voOS_GetSysTime();
	// check moov box
	if (!r->SetPosition(moovPos + moovSize))
		return MP4RRC_BAD_MOOV;
	uint8 temp[8];
	if (!r->Read(temp, 8))
		return MP4RRC_BAD_MOOV;
	_headersize = moovSize + 8;
	_headerpos = moovPos;
	VOLOGR("Open14 Cost:%d", voOS_GetSysTime() - g);
	g = voOS_GetSysTime();
	if (!r->SetPosition(_nTrakPos))
		return MP4RRC_SEEK_FAILED;
	rc = ParseTrakBoxes(r, int(trakBoxesSize));
	if (rc)
		return rc;

	VOLOGR("Open15 Cost:%d", voOS_GetSysTime() - g);
	g = voOS_GetSysTime();
	
	if (!(IsIsml()|| IsDash()) && m_bHasFrag)
	{
		_nFileFormat = FOURCC2_isml;
	}
	VOLOGR("MovieOpen--");
	return rc;
}

MP4RRC Movie::Open(Reader* r,VO_BOOL isStreaming/* = VO_FALSE*/)
{
	m_bIsStreaming = isStreaming;

	VO_U32 g = voOS_GetSysTime();
	MP4RRC rcc = Open(r);
	
	VOLOGR("Open1 Cost:%d", voOS_GetSysTime() - g);
	g = voOS_GetSysTime();


	if(IsIsml() || IsDash())
	{
		Scanner scanner(r);
		r->SetPosition(0);
		uint64 moovsize = scanner.FindTag(FOURCC2_moov);
		if(moovsize<= 0) return MP4RRC_NO_MOOV;
		uint64 moovoffset = r->Position();
		Scanner scanner1(r);
		int uuidsize = scanner1.FindTag(FOURCC2_uuid, moovsize + moovoffset) ;
		if(uuidsize > 0)
		{
			VO_DRM_INFO info;
			info.nDRMHeader = uuidsize + 8;

			VO_PBYTE pDrmHead = new VO_BYTE[info.nDRMHeader];
			r->Move(-8);
			r->Read(pDrmHead,info.nDRMHeader);

			info.pDRMHeader = pDrmHead;
			m_DrmReader->DRMInfo(VO_DRMTYPE_PlayReady,&info);
			delete []pDrmHead;
		}
		VOLOGR("Open03 Cost:%d", voOS_GetSysTime() - g);
		g = voOS_GetSysTime();
	
	}
	VOLOGR("Open3 Cost:%d", voOS_GetSysTime() - g);
	g = voOS_GetSysTime();
	return rcc;
}

MP4RRC Movie::ProgressiveOpen(Reader* r)
{
	MP4RRC rc = MP4RRC_OK;
	while ((rc = Open(r,VO_FALSE)))
	{
		if (int(r->GetErrorCode()) != MPXERR_DATA_DOWNLOADING)
			break;
		r->ClearErrorCode();
	}
	return rc;
}

MP4RRC Movie::Close()
{
	return MP4RRC_OK;
}

MP4RRC Movie::ParseFtypBox(Reader* r,int bodySize)
{
	/*09/01/2011,add by leon*/
	_nFileFormat = FOURCC2_mp4l;
	if(bodySize == 0) return MP4RRC_OK;

	const int BUFSIZE = 1024;
	uint8 buf[BUFSIZE]; 
	//2012/07/13, add by leon 
	int mSize = bodySize < BUFSIZE ? bodySize : BUFSIZE;
	if (!r->Read(buf, mSize)) 
		return MP4RRC_READ_FAILED;

	MemStream ms(buf, mSize);
	ReaderMSB r2(&ms);

	//r2.Move(8);
	uint32 nMajorBand;
	r2.Read(&nMajorBand);
	bodySize -= 4;
	_nFileFormat = nMajorBand;

	r2.Read(&m_nMinorVersion);
	bodySize -= 4;

	if(_nFileFormat == FOURCC2_isml)
		return MP4RRC_OK;

	uint32 compatibility_brand;
	for(uint32 i = bodySize; i> 0; i -= 4)
	{
		if(!r2.Read(&compatibility_brand))
			return MP4RRC_READ_FAILED;
		switch(compatibility_brand)
		{
		case FOURCC2_dash:
			_nFileFormat = FOURCC2_dash;
			break;
		case  FOURCC2_qt://2012/03/12, Leon, fix quicktime #11375
			_nFileFormat = FOURCC2_qt;
			break;
		}
	}
	if(nMajorBand == FOURCC2_dash)
		_nFileFormat = FOURCC2_dash;
	return MP4RRC_OK;
}

MP4RRC Movie::ParseUdtaBox(Reader* r, int bodySize)
{
 	Scanner sc(r);
	uint32 tag= 0xA978797A;
#ifdef LIEYUN_LG
	ABSPOS mark = r->Position();
#endif
	int tagSize = sc.FindTag(tag,r->Position() + bodySize);
	VOLOGR("2. %d", tagSize);
	if(tagSize >0)
	{	
		memset(m_strGPS, 0x00, sizeof(m_strGPS));
		r->Move(4);
		bool bb = r->Read(m_strGPS, bodySize -8 -4);
		if(!bb)
			return MP4RRC_READ_FAILED;
	}
#ifdef LIEYUN_LG
	r->SetPosition(mark);
	tagSize = sc.FindTag(FOURCC2_cypt);
	if(tagSize >0)
	{	
		m_nEVideoContent = 0;
		bool bb = r->Read(&m_nEVideoContent);
		if(!bb)
			return MP4RRC_READ_FAILED;
	}
#endif
	return MP4RRC_OK;
}

MP4RRC Movie::ParseMvexBox(Reader* r, int bodySize)
{

	Scanner scanner(r);
	ABSPOS posb = r->Position();
	ABSPOS mark = r->Position() + bodySize;

	ABSPOS pos = r->Position();
	int mehdSize = scanner.FindTag(FOURCC2_mehd, mark);
	if(mehdSize > 0)
	{
		uint8 version = 0;
		r->Read(&version);
		r->Move(3);
		
		if(1 == version)
		{
			uint64 cur64 = 0;
			r->Read(&cur64);
			_mehdduration = cur64;
		}
		else
		{
			uint32 cur32 = 0;
			r->Read(&cur32);
			_mehdduration = cur32;
			 
		}
		
	}
	else
	{
		r->SetPosition(pos);
	}
	int trexSize = 0;
	int i = 0;
	while ( (trexSize = scanner.FindTag(FOURCC2_trex, mark)) )
	{
		DEFAULT_SAMPLEINFO *pInfo = &m_stDefaultInfo[i ++];
		uint32 version;
		r->Read(&version);
		r->Read(&pInfo->nTrackID);
		uint32 default_sample_description_index;
		r->Read(&default_sample_description_index);
		r->Read(&pInfo->nDefaultDuration);
		r->Read(&pInfo->nDefaultSize);
		uint32 default_sample_flags;
		r->Read(&default_sample_flags);
	}

	

	return MP4RRC_OK;
}
MP4RRC Movie::ParseMvhdBox(Reader* r, int bodySize)
{
	const int BUFSIZE = 1024;
	uint8 buf[BUFSIZE]; 
	if (!r->Read(buf, bodySize < BUFSIZE ? bodySize : BUFSIZE)) 
		return MP4RRC_READ_FAILED;

	MemStream ms(buf, bodySize);
	ReaderMSB r2(&ms);
	uint8 version;
	r2.Read(&version);
	r2.Move(3);

	m_strCreationTime[0] = '\0';
	m_strModificationTime[0]= '\0';
	if (version == 1)
	{
		uint64 dur;

		//2011-12-1 , add by leon,
		//change data format 
		uint64 c_time;
		r2.Read(&c_time);
		ConverTime(c_time, m_strCreationTime);
		r2.Read(&c_time);
		ConverTime(c_time, m_strModificationTime);

		r2.Read(&_timescale);

		r2.Read(&dur);
		//duration = (double)(dur / scale);
		_duration = dur;
	}
	else
	{
		uint32 dur;
	//2011-12-1 , add by leon,
		uint32 c_time;
		r2.Read(&c_time);
		ConverTime(c_time, m_strCreationTime);
		r2.Read(&c_time);
		ConverTime(c_time, m_strModificationTime);
	
	//	r2.Move(8);
		r2.Read(&_timescale);
		r2.Read(&dur);
		//duration = (double)dur / scale;
		_duration = dur;
	}

#if 1
	int32 rate;
	int16 volume;
	//int32 matrix[9];
	uint32 pre_defined[6];

	r2.Read(&rate); // 4
	r2.Read(&volume); // 2
	r2.Move(2 + 4 * 2);
	int i;
	for (i = 0; i < 9; i++)
		r2.Read(&_matrix[i]);
	for (i = 0; i < 6; i++)
		r2.Read(&pre_defined[i]);
#else
	r2.Move(76); //  4 + 2 + 2 + 4 * 2 + 4 * 9 + 4 * 6;
#endif
	uint32 next_track_ID;
	r2.Read(&next_track_ID);
	_cTracks = next_track_ID - 1;

	return MP4RRC_OK;
}

void Movie::GetMatrix(int32 matrix[9])
{
	memcpy(matrix, _matrix, sizeof(_matrix));
}

MP4RRC Movie::ParseTrakBoxes(Reader* r, int totalSize)
{
#if 1
	ABSPOS pos1 = r->Position();
	_cTracks = MoveToTrak(r, -1, false, totalSize) + 1;
	ABSPOS pos2 = r->Position();
	ABSPOS realSize = pos2 - pos1;
#if 0  //8/1/2008
	if (realSize > totalSize)
		return MP4RC_INVALID_TRAKS;
#endif
	if (realSize < totalSize)
		return MP4RRC_MISS_TRACK;
#else //strict check
	int c = MoveToTrak(r, -1, false, totalSize) + 1;
	if ((_cTracks > 16) || (_cTracks < 1) ) //treat as invalid, don't use it
		_cTracks = c;
	else
	if (c < _cTracks)
		return MP4RRC_MISS_TRACK;
#endif
	return MP4RRC_OK;
}

MP4RRC Movie::ParseTfraBoxes(Reader* r, int totalSize)
{
//	ABSPOS pos1 = r->Position();
	Scanner scanner(r);
	ABSPOS pos = GetTfraPos(r, 0,0);
	if(pos)
		return MP4RRC_OK;
	else
		return MP4RRC_SEEK_FAILED;
}

VO_S32 Movie::GetTfraPos(Reader* r, int index, int totalSize)
{
	int current = -1; //current track index
	if (!r->SetPosition(_nTfraPos))
		return current;
	Scanner scanner(r);
	//r->SetPosition(0);
	while (true)
	{
		int tfraSize = scanner.FindTag(FOURCC2_tfra); 
		if (tfraSize == 0)
		{
			break;
		}	
		ABSPOS pos = r->Position();	
		r->Move(4);
		uint32 _track;
		r->Read(&_track);
		if (int(_track) == index+1)
			return int(pos);
		if (!r->Move(tfraSize))
			break;
	}
	return -1;
}
VO_S32 Movie::MoveToTrak(Reader* r, int index, bool onlySupported, int totalSize )
{
	int current = -1; //current track index
	if (!r->SetPosition(_nTrakPos))
		return current;
	Scanner scanner(r);
	//6/3/2010, don't move out of moov
	ABSPOS endPos = totalSize ? _nTrakPos + totalSize : 0x7fffffff;
	while (true)
	{
		ABSPOS mark = r->Position();
		int trakSize = scanner.FindTag(FOURCC2_trak, endPos); //6/3/2010
		if (trakSize == 0)
		{
			//r->SetPosition(mark);
			break;
		}

		// check sample type
		mark = r->Position();
		MP4RRC rc = CheckTrackValid(r);
		r->SetPosition(mark);
		if (rc) //not valid
		{
			if ( onlySupported || ((rc != MP4RRC_UNSUPPORTED_SAMPLE_TYPE) && (rc != MP4RRC_UNKNOWN_SAMPLE_TYPE)) )
				continue;
		}

		current++;
		if (current == index)
			break;
		if (!r->Move(trakSize))
			break;
	}
	return current;
}
MP4RRC Movie::CheckTrackValid(Reader* r)
{
	Scanner scanner(r);
	int mdiaSize = scanner.FindTag(FOURCC2_mdia);
	if (mdiaSize == 0)
		return MP4RRC_NO_MDIA;

	int minfoSize = scanner.FindTag(FOURCC2_minf);
	if (minfoSize == 0)
		return MP4RRC_NO_MINF;

	int stblSize = scanner.FindTag(FOURCC2_stbl);
	if (stblSize == 0)
		return MP4RRC_NO_STBL;

	ABSPOS mark = r->Position();
	VO_U32 uSize = scanner.FindTag(FOURCC2_stsz);
	if(uSize)
	{
		r->Move(8); // version, flag ,sample size
		VO_U32 uSampleCount;
		r->Read(&uSampleCount);
		if (uSampleCount)
		{
			m_bHasFrag = VO_FALSE;
		}
	}
	r->SetPosition(mark);

	int stsdSize = scanner.FindTag(FOURCC2_stsd);
	if (stsdSize == 0)
		return MP4RRC_NO_STSD;

	

	if (!r->Move(12))
		return MP4RRC_SEEK_FAILED;

	uint32 sampletype;
	if (!r->Read(&sampletype, 4))
		return MP4RRC_READ_FAILED;
	Util::Swap32(&sampletype);

	switch (CheckType(sampletype))
	{
	case MP4RRC_VIDEO_TRACK:
	case MP4RRC_AUDIO_TRACK:
	case MP4RRC_SUBTITLE_TRACK:
	case MP4RRC_OK:
		return MP4RRC_OK;
	default:
		return MP4RRC_UNKNOWN_SAMPLE_TYPE;
	}

	return MP4RRC_OK;
}


MP4RRC Movie::OpenTrack(int index, Track*& track)
{
	Reader* r = GetSourceReader();
	int ret = MoveToTrak(r, index, true ,int(_headersize) );
	if (ret != index)
		return MP4RRC_NO_SUCH_TRACK;
	ABSPOS mark = r->Position();
	int tt = CheckTrackType(r);


	if(GetIsStreaming())
	{
		track = new TrackSegment();
	}
	//09/07/2011,leon, add for piff
	else if(IsIsml() ||IsDash() )
	{
		track = new TrackFragment();
	}
	else
	{
		switch(tt)
		{
		case 0:
			track = new Track();
			break;
		case 1:
			track = new TrackS1();
			break;
		case 2:
			{
				VOLOGR("This is TText track");
				track = new TrackSubtitle();
				break;
			}

		}
	}
	track->SetMovie(this);
	r->SetPosition(mark);
	

	//09/01/2011, add leon
	track->SetFileFormat(_nFileFormat);
	return track->Open(r);
}

VO_S32 Movie::CheckTrackType(Reader* r)
{
	ABSPOS trakPos = r->Position();
	ABSPOS endPos = GetHeaderPos() + GetHeaderSize();
	Scanner scanner(r);

	int tkhdSize = scanner.FindTag(FOURCC2_tkhd, endPos);
	if (tkhdSize == 0)
		return 0;

	r->SetPosition(trakPos);
	int mdiaSize = scanner.FindTag(FOURCC2_mdia, endPos);
	if (mdiaSize == 0)
		return 0;

	int minfoSize = scanner.FindTag(FOURCC2_minf, endPos);
	if (minfoSize == 0)
		return 0;

	int stblSize = scanner.FindTag(FOURCC2_stbl, endPos);
	if (stblSize == 0)
		return 0;
	ABSPOS stblPos = r->Position();
	ABSPOS stblPos2 = stblPos + stblSize; //boundary of child boxes


	r->SetPosition(stblPos);
	int stsdSize = scanner.FindTag(FOURCC2_stsd);
	if (stsdSize == 0)
		return MP4RRC_NO_STSD;
	if (!r->Move(12))
		return MP4RRC_SEEK_FAILED;
	uint32 sampletype;
	if (!r->Read(&sampletype, 4))
		return MP4RRC_READ_FAILED;
	Util::Swap32(&sampletype);
	if(sampletype == FOURCC2_tx3g)
		return 2;

	r->SetPosition(stblPos);
	int stszSize = scanner.FindTag(FOURCC2_stsz, stblPos2);
	if (stszSize == 0)
		return 0;
	StszBox stszBox;
	MP4RRC rc = stszBox.Init(r->GetStream(), stszSize);
	if (rc)
		return 0;

	int sz = stszBox.GetSampleSize();
	return sz == 1 ? 1 : 0;
}
uint32 Movie::GetDefaultDuration(uint32 nTrackID)
{

	for( uint32 i = 0 ;i < 10; i++)
	{
		DEFAULT_SAMPLEINFO *pInfo = &m_stDefaultInfo[i];
		if( pInfo->nTrackID == nTrackID)
			return pInfo->nDefaultDuration;
	}
	return 0;
}
uint32 Movie::GetDefaultSize(uint32 nTrackID)
{
	for( uint32 i = 0 ;i < 10; i++)
	{
		DEFAULT_SAMPLEINFO *pInfo = &m_stDefaultInfo[i];
		if( pInfo->nTrackID == nTrackID)
			return pInfo->nDefaultSize;
	}
	return 0;
}

VO_VOID Movie::DisableBoxType(VO_U32 uBoxType)
{	
	if (uBoxType == FOURCC2_mfra)
	{
		m_uEnableBox &= 0xFFFFFFFE;
	}
}
// Track
//==============================================================================

Track::Track()
: _descriptor_data(0),
  m_pTence(0),
  m_bIsCencDrm(VO_FALSE)
#ifdef _SUPPORT_TSOFFSET
, _tsoffset(0)
#endif //_SUPPORT_TSOFFSET
{
	_stcoBox = 0;
	m_bPDIsmv = VO_FALSE;
#ifdef _SUPPORT_LANG
	_lang = 0;
#endif //_SUPPORT_LANG
}

Track::~Track()
{
	VOLOGR("+~Track");
	Close();
	VOLOGR("-~Track");
}

MP4RRC Track::Open(Reader* r)
{
	VO_U32 s = voOS_GetSysTime();

	ABSPOS trakPos = r->Position();
	ABSPOS endPos = _movie->GetHeaderPos() + _movie->GetHeaderSize();

	Scanner scanner(r);

	int tkhdSize = scanner.FindTag(FOURCC2_tkhd, endPos);
	if (tkhdSize == 0)
		return MP4RRC_NO_TKHD;

	MP4RRC rc = ParseTkhdBox(r, tkhdSize);
	if (rc)
		return rc;

	VOLOGR("openT1 cost:%d ms", voOS_GetSysTime() -s);
	s = voOS_GetSysTime();

	r->SetPosition(trakPos);
	int edtsSize = scanner.FindTag(FOURCC2_edts, endPos);
	if (edtsSize)
	{
		int elstSize = scanner.FindTag(FOURCC2_elst, endPos);
		if (elstSize)
			ParseElstBox(r, elstSize);
	}
	VOLOGR("openT2 cost:%d ms", voOS_GetSysTime() -s);
	s = voOS_GetSysTime();


	r->SetPosition(trakPos);
	int mdiaSize = scanner.FindTag(FOURCC2_mdia, endPos);
	if (mdiaSize == 0)
		return MP4RRC_NO_MDIA;
	ABSPOS mdiaPos = r->Position();

	VOLOGR("openT3 cost:%d ms", voOS_GetSysTime() -s);
	s = voOS_GetSysTime();

	int mdhdSize = scanner.FindTag(FOURCC2_mdhd, endPos);
	if (mdhdSize == 0)
		return MP4RRC_NO_MDHD;
	rc = ParseMdhdBox(r, mdhdSize);
	if (rc)
		return rc;
	VOLOGR("openT4 cost:%d ms", voOS_GetSysTime() -s);
	s = voOS_GetSysTime();
	r->SetPosition(mdiaPos);
	int hdlrSize = scanner.FindTag(FOURCC2_hdlr, endPos);
	if (hdlrSize == 0)
		return MP4RRC_NO_HDLR;
	rc = ParseHdlrBox(r, hdlrSize);
	if (rc)
		return rc;

	VOLOGR("openT5 cost:%d ms", voOS_GetSysTime() -s);
	s = voOS_GetSysTime();

	r->SetPosition(mdiaPos);
	int minfoSize = scanner.FindTag(FOURCC2_minf, endPos);
	if (minfoSize == 0)
		return MP4RRC_NO_MINF;

	VOLOGR("openT6 cost:%d ms", voOS_GetSysTime() -s);
	s = voOS_GetSysTime();

	int stblSize = scanner.FindTag(FOURCC2_stbl, endPos);
	if (stblSize == 0)
		return MP4RRC_NO_STBL;
	ABSPOS stblPos = r->Position();
	ABSPOS stblPos2 = stblPos + stblSize; //boundary of child boxes

	VOLOGR("openT7 cost:%d ms", voOS_GetSysTime() -s);
	s = voOS_GetSysTime();

	int stsdSize = scanner.FindTag(FOURCC2_stsd, stblPos2);
	if (stsdSize == 0)
		return MP4RRC_NO_STSD;
	rc = ParseStsdBox(r, stsdSize);
	if (rc)
		return rc;

	VOLOGR("openT8 cost:%d ms", voOS_GetSysTime() -s);
	s = voOS_GetSysTime();

	if( ! IsIsml() && !IsDash())
	{
		r->SetPosition(stblPos);
		int sttsSize = scanner.FindTag(FOURCC2_stts, stblPos2);
		if (sttsSize == 0)
			return MP4RRC_NO_STTS;
		rc = _sttsBox.Init(r->GetStream(), sttsSize);
		if (rc )
			return rc;

		r->SetPosition(stblPos);
		int cttsSize = scanner.FindTag(FOURCC2_ctts, stblPos2);
		if (cttsSize != 0)
		{
			rc = _cttsBox.Init(r->GetStream(), cttsSize);
			if (rc )
				return rc;
		}

		r->SetPosition(stblPos);
		int stszSize = scanner.FindTag(FOURCC2_stsz, stblPos2);
		if (stszSize == 0)
			return MP4RRC_NO_STSZ;
		rc = _stszBox.Init(r->GetStream(), stszSize);
		if (rc )
			return rc;

		r->SetPosition(stblPos);
		int stcoSize = scanner.FindTag(FOURCC2_stco, stblPos2);
		if (stcoSize == 0)
		{
			r->SetPosition(stblPos);
			stcoSize = scanner.FindTag(FOURCC2_co64, stblPos2);
			if (stcoSize == 0)
				return MP4RRC_NO_STCO;
			_stcoBox = new Co64Box();
		}
		else
			_stcoBox = new StcoBox();

		rc = _stcoBox->Init(r->GetStream(), stcoSize);
		if (rc)
			return rc;

		// stsc box must after stco box
		r->SetPosition(stblPos);
		int stscSize = scanner.FindTag(FOURCC2_stsc, stblPos2);
		if (stscSize == 0)
			return MP4RRC_NO_STSC;

#ifdef _SUPPORT_TTEXT
		if(GetSampleType() == FOURCC2_tx3g)
			rc = _stscBox.Init2(r->GetStream(), stscSize, GetChunkCount());
		else
			rc = _stscBox.Init(r->GetStream(), stscSize, GetChunkCount());
#else
		rc = _stscBox.Init(r->GetStream(), stscSize, GetChunkCount());
#endif
		if (rc)
			return rc;

		//Note! stss box is optional
		VOLOGR("openT9 cost:%d ms", voOS_GetSysTime() -s);
		s = voOS_GetSysTime();

		r->SetPosition(stblPos);
		int stssSize = scanner.FindTag(FOURCC2_stss, stblPos2);
		if (stssSize)
		{
			rc = _stssBox.Init(r->GetStream(), stssSize, GetSampleCount());
			if (rc)
				return rc;
		}
		VOLOGR("openT10 cost:%d ms", voOS_GetSysTime() -s);
		s = voOS_GetSysTime();
	#ifdef _ERR_TOL
		_sttsBox.SetAvgDuration(_duration, GetSampleCount());
	#endif //_ERR_TOL	

	} 
					
	// cache
	_current_sample_chunk = -2;

	// data stream, for get sample data
	if (!_data_stream.Init1(r->GetStream(), 65536))
		return MP4RC_INIT_DATA_STREAM_FAIL;
	_prev_sample_chunk = -1;
	VOLOGR("openT11 cost:%d ms", voOS_GetSysTime() -s);
	s = voOS_GetSysTime();
	if(!IsIsml() && !IsDash())
	{
	 //check mdat data, 3/16/2011
		uint32 size = GetSampleData(0, NULL, 0); //don't read data
		if (size == ERR_CHECK_READ)
			return MP4RC_INIT_DATA_STREAM_FAIL;
	}

	return MP4RRC_OK;
}

MP4RRC Track::Close()
{
	if (_descriptor_data)
	{
		delete[] _descriptor_data;
		_descriptor_data = 0;
	}
	if (_stcoBox)
	{
		delete _stcoBox;
		_stcoBox = 0;
	}
	return MP4RRC_OK;
}

MP4RRC Track::ParseTkhdBox(Reader* r, int bodySize)
{
	const int BUFSIZE = 1024;
	uint8 buf[BUFSIZE]; 
	if (!r->Read(buf, bodySize < BUFSIZE ? bodySize : BUFSIZE)) 
		return MP4RRC_READ_FAILED;

	MemStream ms(buf, bodySize);
	ReaderMSB r2(&ms);
	uint8 version;
	r2.Read(&version);
	r2.Move(3);
	if (version == 1)
	{
		uint64 ctime, mtime, dur;
		uint32 tID;
		r2.Read(&ctime);
		r2.Read(&mtime);
		r2.Read(&tID);
		r2.Move(4);
		r2.Read(&dur);

		//09/07/2011,leon
		_nTrack_ID = tID;
	}
	else
	{
		uint32 ctime, mtime, dur;
		uint32 tID;
		r2.Read(&ctime);
		r2.Read(&mtime);
		r2.Read(&tID);
		r2.Move(4);
		r2.Read(&dur);

		//09/07/2011,leon
		_nTrack_ID = tID;
	}

	r2.Move(12); //8 + 2 + 2+ 2 + 2
	int16 volume;
	r2.Read(&volume);
	r2.Move(2);
	//int32 matrix[9];
	int i;
	for (i = 0; i < 9; i++)
		r2.Read(&_matrix[i]);
	int32 width, height;
	r2.Read(&width);
	r2.Read(&height);

	return MP4RRC_OK;
}


MP4RRC Track::ParseElstBox(Reader* r, int bodySize)
{
	const int BUFSIZE = 1024;
	uint8 buf[BUFSIZE]; 
	if (!r->Read(buf, bodySize < BUFSIZE ? bodySize : BUFSIZE)) 
		return MP4RRC_READ_FAILED;

	MemStream ms(buf, bodySize);
	ReaderMSB r2(&ms);
	uint8 version;
	uint32 entry_count;
	r2.Read(&version);
	r2.Move(3);
	r2.Read(&entry_count);

	if (version == 1)
	{
		uint64 segment_duration;
		int64 media_time;
		r2.Read(&segment_duration);
		r2.Read(&media_time);
#ifdef _SUPPORT_TSOFFSET
		if (media_time == -1)
			_tsoffset = segment_duration;
#endif //_SUPPORT_TSOFFSET
	}
	else
	{
		uint32 segment_duration;
		int32 media_time;
		r2.Read(&segment_duration);
		r2.Read(&media_time);
#ifdef _SUPPORT_TSOFFSET
		if (media_time == -1)
			_tsoffset = segment_duration;
#endif //_SUPPORT_TSOFFSET
	}

#ifdef _SUPPORT_TSOFFSET
	_tsoffset *= _time_divisor;
	_tsoffset /= _movie->GetTimeScale();
#endif //_SUPPORT_TSOFFSET
	return MP4RRC_OK;
}

void Track::GetMatrix(int32 matrix[9])
{
	memcpy(matrix, _matrix, sizeof(_matrix));
}

MP4RRC Track::ParseMdhdBox(Reader* r, int bodySize)
{
	const int BUFSIZE = 1024; 
	uint8 buf[BUFSIZE]; 
	int realsize = bodySize < BUFSIZE ? bodySize : BUFSIZE;
	if (!r->Read(buf, realsize)) 
		return MP4RRC_READ_FAILED;

	MemStream ms(buf, realsize);
	ReaderMSB r2(&ms);
	uint8 version;
	r2.Read(&version);
	r2.Move(3);
	if (version == 1)
	{
		if (realsize < 36) return MP4RRC_BAD_DATA;//4 + 16 + 8 + 8
		uint64 dur;
		r2.Move(16);
		r2.Read(&_timescale);
		r2.Read(&dur);
		//duration = (double)(dur / scale);
		_duration = dur;
	}
	else
	{
		if (realsize < 20) return MP4RRC_BAD_DATA;//4 + 8 + 4 + 4
		uint32 dur;
		r2.Move(8);
		r2.Read(&_timescale);
		r2.Read(&dur);
		//duration = (double)dur / scale;
		_duration = dur;
	}

#ifdef _SUPPORT_LANG
	r2.Read(&_lang);
	uint16 eng = LANG_ENG; //for test
#endif //_SUPPORT_LANG

	return MP4RRC_OK;
}

MP4RRC Track::ParseHdlrBox(Reader* r, int bodySize)
{
	const int BUFSIZE = 1024;
	uint8 buf[BUFSIZE]; 
	if (!r->Read(buf, bodySize < BUFSIZE ? bodySize : BUFSIZE)) 
		return MP4RRC_READ_FAILED;

	MemStream ms(buf, bodySize);
	ReaderMSB r2(&ms);
	uint8 version;
	r2.Read(&version);
	r2.Move(7);  //flags, pre_defined
	r2.Read(&_handler_type);

	return MP4RRC_OK;
}

MP4RRC Track::ParseStsdBox(Reader* r, int bodySize)
{
	//stsd box include  <pinf addr="493" size="32768" /> 
	const int BUFSIZE = 16384;
	uint8 buf[BUFSIZE]; 
	int realsize = bodySize < BUFSIZE ? bodySize : BUFSIZE;
	if (!r->Read(buf, realsize)) 
		return MP4RRC_READ_FAILED;

	MemStream ms(buf, bodySize);
	ReaderMSB r2(&ms);
	uint8 version;
	r2.Read(&version); //1
	r2.Move(3); //4
	uint32 entry_count;
	r2.Read(&entry_count); //8

	uint32 sampleEntrySize;
	r2.Read(&sampleEntrySize); //12
	r2.Read(&_sample_type); //16 
	r2.Move(8); //+6, data_reference_index(2)
	//leftsize -= 22;
	//ASSERT(leftsize > 0);
	MP4RRC rrc = CheckType(_sample_type);
	switch (rrc)
	{
	case MP4RRC_VIDEO_TRACK:
		return ParseVideoSampleEntry(&r2, realsize);
	case MP4RRC_AUDIO_TRACK:
		return ParseAudioSampleEntry(&r2, realsize);
	//12/10/2011, add by leon for subtitle
	case MP4RRC_SUBTITLE_TRACK:
		return ParseTextSampleEntry(&r2, realsize);
	//12/10/2011, add by leon for subtitle

	case MP4RRC_OK:
		return MP4RRC_OK;
	default:
		return rrc;


	}

	return MP4RRC_OK;
}

MP4RRC Track::ParseAudioSampleEntry(Reader* r, int bodysize)
{
	memset(&_audiofmt, 0x00, sizeof(_audiofmt));
	memset(&_audioqtff, 0x00, sizeof(_audioqtff));

	ABSPOS endPos = r->Position() + bodysize;
	uint16 version;
	r->Read(&version);
	r->Move(6); //2 + 4;
	r->Read(&_audiofmt.channelcount);
	r->Read(&_audiofmt.samplesize);
	r->Read(&_audiofmt.compressionID); //QuickTime
	r->Read(&_audiofmt.packetsize);  //QuickTime
	r->Read(&_audiofmt.samplerate);
	if (_audiofmt.samplerate == 0) //error compatible
		r->Read(&_audiofmt.samplerate);
	else
		r->Move(2); //16.16, 11/29/2005
	//QuickTime specific, qtff.pdf, p122


	if ((version == 1) || 
		(version == 2)) //6/29/2001, Jason, Task #6574
	{
		//2012/03/12, Leon, fix quicktime #11375
		_nFileFormat = FOURCC2_qt  ;
		//2012/02/14,modify by Leon
		r->Read(&_audioqtff.samplesPerPacket);
		r->Read(&_audioqtff.bytesPerPacket);
		r->Read(&_audioqtff.bytesPerFrame);
		r->Read(&_audioqtff.bytesPerSample);

		if (_audiofmt.compressionID) //should be 0 for version 0
		{
			ABSPOS mark = r->Position();
			r->Move(8);
			Scanner scanner(r);
			uint32 esdsSize = scanner.FindTag(FOURCC2_esds, endPos);
 			if (esdsSize == 0) // Task #6459, #6574
			{
				r->SetPosition(mark);
				if (version == 2)
					r->Move(20);  // TODO: don't know the spec (SoundDescriptionV2??), just from clips of Task #6574
				uint32 waveSize = scanner.FindTag(FOURCC2_wave, endPos);
				if (waveSize)
					esdsSize = scanner.FindTag(FOURCC2_esds, endPos);
			}
			if (esdsSize)
				r->Move(-8);
#if 0 // some mov file has mp3 audio, no esds, but KMPlayer can play it, so remove such check, East 20110607
			else
 				return MP4RRC_NO_ESDS;
#endif
		}
	}

	return ParseDescriptor(r, bodysize);
}

MP4RRC Track::ParseVideoSampleEntry(Reader* r, int bodysize)
{
	r->Move(16); //2 + 2 + 4 * 3;
	r->Read(&_videofmt.width);
	r->Read(&_videofmt.height);
	r->Read(&_videofmt.horizresolution);
	r->Read(&_videofmt.vertresolution);
	r->Move(4);
	r->Read(&_videofmt.frame_count);
	r->Read(&_videofmt.compressorname, 32);
	r->Read(&_videofmt.depth);
	r->Read(&_videofmt.pre_defined);

	return ParseDescriptor(r, bodysize);
}

MP4RRC Track::ParseDescriptor(Reader* r, int bodysize)
{
	uint32 size, type;
	int leftsize = int(bodysize - r->Position());
	while (true)  //skip non FOURCC2_esds boxes, 12/11/2008
	{
		if (!r->Read(&size) || !r->Read(&type) || size == 0) // 11/3/2009, size==0 should be error
		{
			_descriptor_size = 0;
			return MP4RRC_OK;
		}
		if (int(size) > leftsize)
			return MP4RRC_BAD_DESCRIPTOR;
		if (type == FOURCC2_esds || type == FOURCC2_esda ) //same for FOURCC2_esda? 1/13/2001, Jason
		{
			if (!r->Move(4)) //version, flags
				return MP4RRC_SEEK_FAILED;
			size -= 4; // 4 byte version & flags
			break;
		}
		else if ( (type == FOURCC2_avcC) // FOURCC2_avcC also valid, 12/12/2008
			|| (type == FOURCC2_devc)  // EVRC, FOURCC2_sevc also valid, 4/28/2009
			||(type == FOURCC2_avc3)
			)
			break;
		else if( type == FOURCC2_alac )	//add FOURCC2_alac 20100712
			break;
		else if (type == FOURCC2_d263) //5/17/2011
			break;

		else if (type == FOURCC2_wfex ) break;//77666578,09/11/2011,leon
		else if( type == FOURCC2_dvc1 )break;//64766331,09/11/2011,leon
		else if( type == FOURCC2_glbl )break;//04/06/2012,leon,#12277
		else if(type == FOURCC2_hvcC||
				type == FOURCC2_hev1||
				type == FOURCC2_hvc1||
				type == FOURCC2_hevC)
				break;

		if (!r->Move(size - 8))
				return MP4RRC_SEEK_FAILED;
	}
	_descriptor_type = type;
	_descriptor_size = size - 8; //8 byte header,
	if (_descriptor_size > 0)
	{
		_descriptor_data = new uint8[_descriptor_size];
		/* 09/11/2011,leon */
		if(type == FOURCC2_dvc1)
		{
			for( ; _descriptor_size>5 ;)
			{
				r->Read(_descriptor_data, _descriptor_size);
				if(_descriptor_data[0] == 0x25 && _descriptor_data[1] == 0x00 && _descriptor_data[2] == 0x00 &&_descriptor_data[3] == 0x01 &&_descriptor_data[4] == 0x0F)
					break;
				memset(_descriptor_data,0x00 ,_descriptor_size);
				_descriptor_size --;
				r->Move(- (int64(_descriptor_size)));
			}
		}
		else
		{
		/* 09/11/2011,leon */
			if (!r->Read(_descriptor_data, _descriptor_size))
				return MP4RRC_READ_FAILED;
		}
	}
	return MP4RRC_OK;
}


uint32 Track::GetSampleAddressBase(int index)
{
    //VOLOGR("index:%d", index);
	uint32 addr = GetSampleAddress(index);
#ifdef _CHECK_READ
	if (addr == ERR_CHECK_READ)
    {
        VOLOGE("GetSampleAddressBase failed");
		return ERR_CHECK_READ;
    }
#endif //_CHECK_READ
	return uint32(_current_chunk_address);
}

uint32 Track::GetSampleAddressEnd(int index)
{
    //VOLOGR("index:%d", index);
	uint32 addr = GetSampleAddress(index);
#ifdef _CHECK_READ
	if (addr == ERR_CHECK_READ)
    {
        VOLOGE("GetSampleAddressEnd failed");
		return ERR_CHECK_READ;
    }
#endif //_CHECK_READ
    //VOLOGR("addr: %d", _current_chunk_address + _current_chunk_size);
	return uint32(_current_chunk_address) + _current_chunk_size;
}

uint32 Track::GetSampleAddress(int index)
{
    //VOLOGR("index:%d", index);
	int chunkSampleIndex;
	int chunkSampleCount;
	int chunkIndex = GetSampleChunk(index, chunkSampleIndex, chunkSampleCount);
#ifdef _CHECK_READ
	if (chunkIndex < 0)
    {
        VOLOGE("GetSampleAddress failed 1");
		return ERR_CHECK_READ;
    }
#endif //_CHECK_READ

	if (chunkIndex == _current_sample_chunk)
	{
		if (index >= _current_sample_index)
		{
			uint64 addr = _current_sample_address;
			for (int i = _current_sample_index; i < index; i++)
			{
				uint32 size = GetSampleSize(i);
#ifdef _CHECK_READ
                if (size == ERR_CHECK_READ)
                {
                    VOLOGE("GetSampleAddress failed 2");
                    return ERR_CHECK_READ;
                }
#endif //_CHECK_READ
				addr += size;
			}
			_current_sample_index = index;
			_current_sample_address = addr;
			return uint32(addr);
		}
	}

    //VOLOGR("21");
	uint64 addr = _current_chunk_address = uint32( GetChunkOffset(chunkIndex));
	_current_chunk_size = 0;
	int i;
	for (i = chunkSampleIndex; i < index; i++)
	{
        uint32 size = GetSampleSize(i);
#ifdef _CHECK_READ
        if (size == ERR_CHECK_READ)
        {
            VOLOGE("GetSampleAddress failed 3");
            return ERR_CHECK_READ;
        }
#endif //_CHECK_READ
        addr += uint64(size);
		_current_chunk_size += GetSampleSize(i);
	}
    //VOLOGR("22")
	for (i = index; i < chunkSampleIndex + chunkSampleCount; i++)
    {
        uint32 size = GetSampleSize(i);
#ifdef _CHECK_READ
        if (size == ERR_CHECK_READ)
        {
            VOLOGE("GetSampleAddress failed 4");
            return ERR_CHECK_READ;
        }
#endif //_CHECK_READ
		_current_chunk_size += size;
    }

	_current_sample_chunk = chunkIndex;
	_current_sample_index = index;
	_current_sample_address = uint32(addr);

    //VOLOGR("addr: %d", addr);
	return uint32(addr);
}

//#include <windows.h>

uint32 Track::GetSampleData(int index, void* buffer, int buffersize)
{
    //VOLOGR("index:%d", index);
	uint32 addrSample = GetSampleAddress(index);
	
#ifdef _CHECK_READ
	if (addrSample == ERR_CHECK_READ)
    {
        //VOLOGR("GetSampleData failed");
		return ERR_CHECK_READ;
    }
#endif //_CHECK_READ
	if (_current_sample_chunk != _prev_sample_chunk)
	{
		VOLOGI("Track::GetSampleData _current_sample_chunk != _prev_sample_chunk _current_sample_chunk. ");
		if (!_data_stream.Init2(_current_chunk_size, _current_chunk_address))
			return ERR_CHECK_READ; //return a special error code indicate the I/O error
		_prev_sample_chunk = _current_sample_chunk;
	}
	uint32 sizeSample = GetSampleSize(index);
	if (sizeSample > 0 && uint32(buffersize) < sizeSample )
		return 0;

	VOLOGI("Track::GetSampleData call _data_stream.SetPosition isaudio: %d, index:%d, buffer:%p, addr:%d, size:%d",IsAudio(),index, buffer, addrSample, sizeSample);
	if (!_data_stream.SetPosition(addrSample))
		return ERR_CHECK_READ;

	if (!_data_stream.Read(buffer, sizeSample))
		return ERR_CHECK_READ;
	return sizeSample;
}

MP4RRC Track::GetSampleInfo(int index)
{
    //VOLOGR("index:%d", index);
	if (index < 0 || (uint32)index >= GetSampleCount())
	{
		VOLOGI("Track::GetSampleInfo,index out of range, no such sample");
		return MP4RRC_NO_SUCH_SAMPLE;
	}

	uint32 addr = GetSampleAddress(index);

#ifdef _CHECK_READ
	if (addr == ERR_CHECK_READ)
    {
        //VOLOGR("GetSampleInfo failed");
		return MP4RRC_READ_FAILED;
    }
#endif //_CHECK_READ
	return MP4RRC_OK;
}

uint64 Track::GetRealDurationByFilePos(uint64 pos)
{
	uint32 counts = GetSampleCount();
	uint64 addr = 0;
	uint32 i = 0;
	for( i = 0; i< counts; i ++)
	{
		addr =  GetSampleAddress(i);
#ifdef _CHECK_READ
		if (addr == ERR_CHECK_READ)
			break;
#endif
		if(addr >= pos) break;
	}
	int index =0;
	if(IsVideo())
		index = GetPrevSyncPoint(i> 0 ? i -2 : 0 );
	else
		index = i -2;
	uint64 time = GetSampleTime(index);
	VOLOGR("IsVideo:%d, Position:%llu, FrameIndex: %d, TimeStamp:%d", IsVideo(), pos, index,time);
	return time;
}

uint32 Track::GetDuration()
{
	if (!GetTimeScale())
	{
		return 0;
	}
	if (_duration)
	{
		return (uint32)(_time_divisor * _duration / GetTimeScale()); 
	}
	if(_movie->GetMehdDuration())
	{
		return (uint32)(_time_divisor * (_movie->GetMehdDuration()/ GetTimeScale()));
	}
	if (!_movie->GetTimeScale())
	{
		return 0;
	}
	return _movie->GetDuration();
}
uint32 Track::GetMaxSampleSize()
{
	uint32 total = GetSampleCount();
	uint32 maxsize = 0;
// 	if(IsVideo() && GetWidth()>0 && GetHeight()>0 )
// 		maxsize = uint32(GetWidth() * GetHeight() * 3 /4);
// 	else
// 	{
		if (total < 1536) // 512 * 3
			maxsize = GetMaxSampleSize(0, total);
		else
		{
			int pos1 = 0;
			int pos2 = total - 512;
			int pos3 = total / 2 - 256;
			uint32 size1 = GetMaxSampleSize(pos1, pos1 + 512);
			uint32 size2 = GetMaxSampleSize(pos2, pos2 + 512);
			uint32 size3 = GetMaxSampleSize(pos3, pos3 + 512);
			maxsize = size1 > size2 ? size1 : size2;
			if (maxsize < size3)
				maxsize = size3;
			//2012/02/20,modify by leon, tow ways all maybe  have big frame
	// 		if (maxsize > MAX_SAMPLE_SIZE)
	// 			maxsize = 0;
 			maxsize *= 6;
		}
		//2012/02/20,add by leon
		if (maxsize > MAX_SAMPLE_SIZE *6)
			maxsize = 0;	
	//}
	return maxsize;
}

uint32 Track::GetMaxSampleSize(int iFrom, int iTo)
{
	uint32 maxsize = 0;
	for (int i = iFrom; i < iTo; i++)
	{
		uint32 size = GetSampleSize(i);
		if (maxsize < size)
			maxsize = size;
	}
	return maxsize;
}

uint32 Track::GetTotalSampleSize(int iFrom, int iTo)
{
	uint32 sum = 0;
	for (int i = iFrom; i < iTo; i++)
		sum += GetSampleSize(i);
	return sum;
}

uint32 Track::GetTotalSampleSize()
{
	uint32 total = GetSampleCount();
	return GetTotalSampleSize(0, total);
}

uint32 Track::GetBitrate()
{
	uint32 total = GetSampleCount();
	uint32 countSample = 0;
	uint32 countSize = 0;
	if (total < 1536) // 512 * 3
	{
		countSample = total;
		countSize = GetTotalSampleSize(0, total);
	}
	else
	{
		countSample = 1536;
		int pos1 = total / 4 - 384;
		int pos2 = total * 3 / 4 - 128;
		int pos3 = total / 2 - 256;
		countSize += GetTotalSampleSize(pos1, pos1 + 512);
		countSize += GetTotalSampleSize(pos2, pos2 + 512);
		countSize += GetTotalSampleSize(pos3, pos3 + 512);
	}

	//bitrate = countSize * 8 / (duration * countSample / total)
	uint64 bitrate = countSize;
	bitrate *= total;
	bitrate *= GetTimeScale();
	if(_duration == 0)
		return 0;
	bitrate /= _duration;
	bitrate /= countSample;
	bitrate <<= 3; // *8  in case overflow, put it last
	return (uint32) bitrate;
}

VO_U32 Track::TrackGenerateIndex(VO_U32 uIndex, Reader* pReader)
{
	if (uIndex >= GetSampleCount())
	{
		return VO_ERR_SOURCE_END;
	}
	uint32 addrSample = GetSampleAddress(uIndex);
#ifdef _CHECK_READ
	if (addrSample == ERR_CHECK_READ)
	{
		VOLOGR("GetSampleAddress failed:uIndex=%d",uIndex);
		return VO_ERR_SOURCE_END;
	}
#endif //_CHECK_READ

	uint32 sizeSample = GetSampleSize(uIndex);
	return pReader->SetPosition(addrSample + sizeSample )? VO_ERR_SOURCE_OK : VO_ERR_SOURCE_END;
}

VO_BOOL Track::IsIndexReady(VO_U32 uIndex, VO_U32* pMaxIndex)
{
	if (pMaxIndex == NULL)
	{
		return VO_TRUE;
	}
	return (uIndex <= (*pMaxIndex)) ? VO_TRUE : VO_FALSE;
}
VO_U32 Track::GetMaxTimeByIndex(VO_U32 uIndex,VO_S64& ullTs)
{
	if (uIndex >= GetSampleCount())
	{
		return VO_ERR_SOURCE_END;
	}
	ullTs = GetSampleTime(uIndex);
	return VO_ERR_SOURCE_OK;
}
// BufferInBox
//==============================================================================

BufferInBox::BufferInBox()
: _reader(&_bs)
, _entry_count(0)
, _current_entry(0)
, m_uMaxInitSize(32768)
{
}

BufferInBox::~BufferInBox()
{
}

MP4RRC BufferInBox::Init(Stream* stream, uint32 bodySize)
{
	_body_size = bodySize;
	if (_bs.Init(stream, bodySize, m_uMaxInitSize))///< for tfra box,we should load all data in memory
		return MP4RRC_OK;
	return MP4RRC_READ_FAILED; //???
}



// SttsBox
//==============================================================================

SttsBox::SttsBox()
: _index_base(0)
, _time_base(0)
{

}

MP4RRC SttsBox::Init(Stream* stream, uint32 bodySize)
{
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
		return rc;
	GetReader()->Move(4); // version, flag
	GetReader()->Read(&_entry_count);
	_data_pos = GetReader()->Position();
	if (_entry_count > 0)
	{
		GetReader()->Read(&_current_count);
		GetReader()->Read(&_current_delta);
		_current_entry++;
	}
#ifndef _ERR_TOL
	else
		return MP4RC_INVALID_STTS;
#endif //_ERR_TOL
	return MP4RRC_OK;
}

CttsBox::CttsBox()
: _index_base(0)
, _time_base(0)
{

}
MP4RRC CttsBox::Init(Stream* stream, uint32 bodySize)
{
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
		return rc;
	GetReader()->Move(4); // version, flag
	GetReader()->Read(&_entry_count);
	_data_pos = GetReader()->Position();
	_current_count = 0;
	_current_delta =0;
	return MP4RRC_OK;
}
int64 CttsBox::GetSampleTime(int32 index)
{

	if(_entry_count <= 0 ) return 0;
	
	VOLOGR("ctts1 index %d,%d",index, _current_delta);
	while (index >= int(_index_base + _current_count))
	{
		if (_current_entry == _entry_count)
			break;
		_index_base += _current_count;
		if (!GetReader()->Read(&_current_count)) return 0;
		if (!GetReader()->Read(&_current_delta)) return 0;
		_current_entry++;
	}
	while (index < int(_index_base))
	{
		if (_current_entry == 0)
			break;
		if (!GetReader()->Move(-16)) return 0;
		if (!GetReader()->Read(&_current_count)) return 0;
		if (!GetReader()->Read(&_current_delta)) return 0;
		_current_entry--;
		_index_base -= _current_count;
	}
	return (int64)_current_delta;
}

uint64 SttsBox::GetSampleTime(int32 index)
{
#ifdef _ERR_TOL
	if (_entry_count == 0)
		return (uint32)(_avg_dur_mul * index / _avg_dur_div);
#else //_ERR_TOL
	ASSERT(_entry_count > 0);
#endif //_ERR_TOL

	while (index > int(_index_base + _current_count))
	{
		if (_current_entry == _entry_count)
			break;
		_index_base += _current_count;
		_time_base += uint64(_current_delta) *  uint64(_current_count);
		if (!GetReader()->Read(&_current_count)) return 0;
		if (!GetReader()->Read(&_current_delta)) return 0;
		_current_entry++;
	}
	while (index < int(_index_base))
	{
		if (_current_entry == 0)
			break;
		if (!GetReader()->Move(-16)) return 0;
		if (!GetReader()->Read(&_current_count)) return 0;
		if (!GetReader()->Read(&_current_delta)) return 0;
		_current_entry--;
		_index_base -= _current_count;
		_time_base -= uint64(_current_delta) *  uint64(_current_count);
	}
	return (uint64)_current_delta * (uint64)(index - _index_base) + _time_base;
}

int SttsBox::GetSampleIndex(uint64 time)
{
	while (time >= _time_base + uint64(_current_delta) *  uint64(_current_count))
	{
		ASSERT(_current_entry <= _entry_count);
		// time may be > total length
		if (_current_entry == _entry_count)
			return _index_base + _current_count - 1; //return the last sample
		_index_base += _current_count;
		_time_base += uint64(_current_delta) *  uint64(_current_count);
		if (!GetReader()->Read(&_current_count)) return -1;
		if (!GetReader()->Read(&_current_delta)) return -1;
		_current_entry++;
	}
	while (time < _time_base)
	{
		ASSERT(_current_entry > 0);
		if (!GetReader()->Move(-16)) return -1;
		if (!GetReader()->Read(&_current_count)) return -1;
		if (!GetReader()->Read(&_current_delta)) return -1;
		_index_base -= _current_count;
		_time_base -= uint64(_current_delta) *  uint64(_current_count);
		//ASSERT(_current_entry >= 0);
		_current_entry--;
	}
	ASSERT(time >= _time_base);
	ASSERT(time < _time_base + uint64(_current_delta) *  uint64(_current_count));
	int index = (time - _time_base + (uint64)_current_delta - 1) / (uint64)_current_delta + _index_base;
	return index;
}


// StssBox
//==============================================================================
StssBox::StssBox()
{

}
MP4RRC StssBox::Init(Stream* stream, uint32 bodySize, uint32 sampleCount)
{
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
		return rc;
	GetReader()->Move(4); // version, flag
	GetReader()->Read(&_entry_count);
	_data_pos = GetReader()->Position();

	_sample_count = sampleCount;
	_current_sync_index = -1;
	_next_sync_index = _sample_count + 1; 
	if (_current_entry < _entry_count)
	{
		GetReader()->Read(&_current_sync_index);
		_current_entry++;
	}
	if (_current_entry < _entry_count)
	{
		GetReader()->Read(&_next_sync_index);
		_current_entry++;
	}
	return MP4RRC_OK;
}

uint32 StssBox::GetSampleSync(int index)
{
	if (_entry_count == 0)
		return 0;
	index += 1; // 1 based
	while (index >= _next_sync_index)
	{
		_current_sync_index = _next_sync_index;
		_current_entry++;
		if (_current_entry > _entry_count)
		{
			_next_sync_index = _sample_count + 1;
			_current_entry = _entry_count +1;
			break;
		}
		if (!GetReader()->Read(&_next_sync_index))
			return 0;
	}
	while (index < _current_sync_index)
	{
		_current_entry--;
		if (_current_entry <= 1)
		{
			if (!GetReader()->Move(-4))
				return 0;
			_next_sync_index = _current_sync_index;
			_current_sync_index = -1;
			_current_entry = 1;
			break;
		}
		else if (_current_entry == _entry_count)
		{
			if (!GetReader()->Move(-8)) return 0;
			if (!GetReader()->Read(&_current_sync_index)) return 0;
			if (!GetReader()->Read(&_next_sync_index)) return 0;
		}
		else
		{
			if (!GetReader()->Move(-12)) return 0;
			if (!GetReader()->Read(&_current_sync_index)) return 0;
			if (!GetReader()->Read(&_next_sync_index)) return 0;
		}
	}
	return index == _current_sync_index ? 1 : 0;
}

int StssBox::GetNextSyncPoint(int index)
{
	if (_entry_count == 0)
		return -1;
	index += 1; // 1 based
	if(index > int(_sample_count)) return -1;
	VOLOGR("+ index:%d, _current_sync_index:%d ,_next_sync_index:%d,_current_entry:%d,_sample_count:%d, _entry_count:%d "
					,index, _current_sync_index, _next_sync_index, _current_entry, _sample_count, _entry_count);
	while (index <= _current_sync_index)
	{
		_current_entry--;
		if (_current_entry <= 1)
		{
			if (!GetReader()->Move(-4)) return -1;
			_next_sync_index = _current_sync_index;
			_current_sync_index = -1;
			_current_entry = 1;
			break;
		}
		else if (_current_entry == _entry_count)
		{
			if (!GetReader()->Move(-8)) return -1;
			if (!GetReader()->Read(&_current_sync_index)) return -1;
			if (!GetReader()->Read(&_next_sync_index)) return -1;
		}
		else
		{
			if (!GetReader()->Move(-12)) return -1;
			if (!GetReader()->Read(&_current_sync_index)) return -1;
			if (!GetReader()->Read(&_next_sync_index)) return -1;
		}
	}

	while (index > _next_sync_index)
	{
		_current_sync_index = _next_sync_index;
		_current_entry++;
		if (_current_entry > _entry_count)
		{
			_next_sync_index = _sample_count + 1;
			_current_entry = _entry_count +1;
			break;
		}
		if (!GetReader()->Read(&_next_sync_index)) return -1;
	}
	VOLOGR("- index:%d, _current_sync_index:%d ,_next_sync_index:%d,_current_entry:%d,_sample_count:%d, _entry_count:%d "
		,index, _current_sync_index, _next_sync_index, _current_entry, _sample_count, _entry_count);
	return int(_next_sync_index > int(_sample_count) ? - 1 : _next_sync_index - 1);
}


int StssBox::GetPrevSyncPoint(int index)
{
	if (_entry_count == 0)
		return -1;
	index += 1; // 1 based
	VOLOGR("+ index:%d, _current_sync_index:%d ,_next_sync_index:%d,_current_entry:%d,_sample_count:%d, _entry_count:%d "
		,index, _current_sync_index, _next_sync_index, _current_entry, _sample_count, _entry_count);
	while (index >= _next_sync_index)
	{
		_current_sync_index = _next_sync_index;
		_current_entry++;
		if (_current_entry > _entry_count)
		{
			_next_sync_index = _sample_count + 1;
			_current_entry = _entry_count +1;
			break;
		}
		if (!GetReader()->Read(&_next_sync_index)) return -1;
	}

	while (index < _current_sync_index)
	{
		_current_entry--;
		if (_current_entry <= 1)
		{
			if (!GetReader()->Move(-4)) return -1;
			_next_sync_index = _current_sync_index;
			_current_sync_index = -1;
			_current_entry = 1;
			break;
		}
		else if (_current_entry == _entry_count)
		{
			if (!GetReader()->Move(-8)) return -1;
			if (!GetReader()->Read(&_current_sync_index)) return -1;
			if (!GetReader()->Read(&_next_sync_index)) return -1;
		}
		else
		{
			if (!GetReader()->Move(-12)) return -1;
			if (!GetReader()->Read(&_current_sync_index)) return -1;
			if (!GetReader()->Read(&_next_sync_index)) return -1;
		}
	}
	VOLOGR("- index:%d, _current_sync_index:%d ,_next_sync_index:%d,_current_entry:%d,_sample_count:%d, _entry_count:%d "
		,index, _current_sync_index, _next_sync_index, _current_entry, _sample_count, _entry_count);
	return _current_sync_index < 0 ? - 1 : _current_sync_index - 1;
}

// StscBox
//==============================================================================
#ifdef _SUPPORT_TTEXT
MP4RRC StscBox::Init2(Stream* stream, uint32 bodySize, uint32 chunkCount)
{
	if (chunkCount < 1)
		return MP4RC_NO_CHUNK;
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
		return rc;
	GetReader()->Move(4); // version, flag
	GetReader()->Read(&_entry_count);
	_data_pos = GetReader()->Position();

	_last_chunk_index = chunkCount + 1;

	// init the data
	GetReader()->Read(&_chunk_index_current);
	GetReader()->Read(&_samples_per_chunk);
	//GetReader()->Move(4);  //sample_description_index
	uint32 sample_description_index;
	GetReader()->Read(&sample_description_index);
	_current_entry++;
	if (_entry_count == 1)
	{
		_chunk_index_next = 0xFFFFFF;
		m_nNextChunkSampleCount = _samples_per_chunk;
	}
	else
	{
		GetReader()->Read(&_chunk_index_next);
		GetReader()->Read(&m_nNextChunkSampleCount);
		GetReader()->Read(&sample_description_index);
	}

	//12/27/2011, leon add
	m_nCurChunkIndex = _chunk_index_current -1;
	m_nNextChunkIndex = _chunk_index_next -1;
	m_nCurChunkSampleCount = _samples_per_chunk;
	m_nNextChunkSampleCount = m_nNextChunkSampleCount;
	m_nCurSampleIndex = 0;
	return MP4RRC_OK;
}


MP4RRC StscBox::GetSampleChunk2(int index, int &chunksampleindex, int &chunksamplecount)
{

	uint32 sample_description_index;
	bool b = true;
	//if(index > _entry_count ) return MP4RRC_READ_FAILED;
	while(1)
	{
		if(index < int(m_nCurChunkIndex))
		{
			GetReader()->SetPosition(_data_pos);
			b &= GetReader()->Read(&m_nCurChunkIndex);
			b &= GetReader()->Read(&m_nCurChunkSampleCount);
			b &= GetReader()->Read(&sample_description_index);
			b &= GetReader()->Read(&m_nNextChunkIndex);
			b &= GetReader()->Read(&m_nNextChunkSampleCount);
			b &= GetReader()->Read(&sample_description_index);
			m_nCurSampleIndex = 0;
			// 
// 			m_nNextChunkIndex = m_nCurChunkIndex ;
// 			m_nNextChunkSampleCount = m_nCurChunkSampleCount;
// 			GetReader()->Move(4 * 3 * (-1) * 3);	//move back 3 group 
// 			b &= GetReader()->Read(&m_nCurChunkIndex);
// 			b &= GetReader()->Read(&m_nCurChunkSampleCount);
// 			b &= GetReader()->Read(&sample_description_index);
// 
// 			if(!b)
// 			{
// 				m_nCurChunkIndex = 0;
// 				m_nCurChunkSampleCount = 0;
// 			}
// 
// 			m_nCurChunkIndex --;// -1 ,because index starts from 0,and m_nCurChunkIndex starts from 1
// 			if(m_nCurChunkIndex < 0) m_nCurChunkIndex = 0;
// 			GetReader()->Move(4 * 3 * (1) * 1);	//move front 1 group
		}
		else if(index >= int(m_nCurChunkIndex) && index < int(m_nNextChunkIndex))
		{
			chunksamplecount = m_nCurChunkSampleCount;
			chunksampleindex = m_nCurSampleIndex + (index - m_nCurChunkIndex ) * chunksamplecount;
			return MP4RRC_OK;
		}
		else if (index >= int(m_nNextChunkIndex))
		{
			m_nCurSampleIndex += (m_nNextChunkIndex - m_nCurChunkIndex)* m_nCurChunkSampleCount;

			m_nCurChunkIndex = m_nNextChunkIndex;
			m_nCurChunkSampleCount = m_nNextChunkSampleCount;

			b &= GetReader()->Read(&m_nNextChunkIndex);
			b &= GetReader()->Read(&m_nNextChunkSampleCount);
			b &= GetReader()->Read(&sample_description_index);
			
			if(!b)
			{
				m_nNextChunkIndex = 0xFFFFFFFF;
				m_nNextChunkSampleCount = 0;
				break; 
			}
			m_nNextChunkIndex --;// -1 ,because index starts from 0,and m_nCurChunkIndex starts from 1
			if(m_nNextChunkIndex < 0) m_nNextChunkIndex = 0;
		}		


	}
	return MP4RRC_READ_FAILED;
}
#endif
MP4RRC StscBox::Init(Stream* stream, uint32 bodySize, uint32 chunkCount)
{
	if (chunkCount < 1)
		return MP4RC_NO_CHUNK;
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
		return rc;
	GetReader()->Move(4); // version, flag
	GetReader()->Read(&_entry_count);
	_data_pos = GetReader()->Position();

	_last_chunk_index = chunkCount + 1;

	// init the data
	GetReader()->Read(&_chunk_index_current);
	GetReader()->Read(&_samples_per_chunk);
	//GetReader()->Move(4);  //sample_description_index
	uint32 sample_description_index;
	GetReader()->Read(&sample_description_index);
	_current_entry++;
	if (_current_entry == _entry_count)
		_chunk_index_next = _last_chunk_index;
	else
		GetReader()->Read(&_chunk_index_next);
	_sample_index_base = 0;
	_sample_current_count = _samples_per_chunk * (_chunk_index_next - _chunk_index_current);


	 //12/27/2011, leon add
	m_nCurChunkIndex = _chunk_index_current -1;
	m_nNextChunkIndex = _chunk_index_next -1;
	m_nCurChunkSampleCount = _samples_per_chunk;
	GetReader()->Read(&m_nNextChunkSampleCount);
	GetReader()->Move(-4);
	
	return MP4RRC_OK;
}
int StscBox::GetSampleChunk(int index, int& chunksampleindex, int& chunksamplecount)
{
	bool b = true;
	while (index >= int(_sample_index_base + _sample_current_count))
	{
		if (_chunk_index_next == _last_chunk_index) //2/4/2010 error in StscBox
			return -1;

		_sample_index_base += _sample_current_count;
		b = GetReader()->Read(&_samples_per_chunk);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		//GetReader()->Move(4);  //sample_description_index
		uint32 sample_description_index;
		b = GetReader()->Read(&sample_description_index);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		_chunk_index_current = _chunk_index_next;
		_current_entry++;
		if (_current_entry == _entry_count)
			_chunk_index_next = _last_chunk_index;
		else
			b = GetReader()->Read(&_chunk_index_next);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		_sample_current_count = _samples_per_chunk * (_chunk_index_next - _chunk_index_current);
	}

	while (index < int(_sample_index_base))
	{
		if (_current_entry == _entry_count)
			b = GetReader()->Move(-24);
		else
			b = GetReader()->Move(-28);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		b = GetReader()->Read(&_chunk_index_current);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		b = GetReader()->Read(&_samples_per_chunk);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		//GetReader()->Move(4);  //sample_description_index
		uint32 sample_description_index;
		b = GetReader()->Read(&sample_description_index);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		b = GetReader()->Read(&_chunk_index_next);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		_current_entry--;
		_sample_current_count = _samples_per_chunk * (_chunk_index_next - _chunk_index_current);
		_sample_index_base -= _sample_current_count;
	}

	uint32 chunkindex = _chunk_index_current + (index - _sample_index_base) / _samples_per_chunk;
	uint32 samplebase = _sample_index_base + (index - _sample_index_base) / _samples_per_chunk * _samples_per_chunk;
	chunksampleindex = samplebase;
	chunksamplecount = _samples_per_chunk;
	return chunkindex - 1; // 1 based to 0 based
}

int32 StscBox::GetSamplesPerChunk(int index)
{
	bool b = true;
	++index; // 0 based to 1 based
	while (index > int(_chunk_index_current))
	{
		if (_chunk_index_next == _last_chunk_index) //2/4/2010 error in StscBox
			return -1;

		_sample_index_base += _sample_current_count;
		b = GetReader()->Read(&_samples_per_chunk);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		//GetReader()->Move(4);  //sample_description_index
		uint32 sample_description_index;
		b = GetReader()->Read(&sample_description_index);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		_chunk_index_current = _chunk_index_next;
		_current_entry++;
		if (_current_entry == _entry_count)
			_chunk_index_next = _last_chunk_index;
		else
			b = GetReader()->Read(&_chunk_index_next);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		_sample_current_count = _samples_per_chunk * (_chunk_index_next - _chunk_index_current);
	}

	while (index < int(_chunk_index_current))
	{
		if (_current_entry == _entry_count)
			b = GetReader()->Move(-24);
		else
			b = GetReader()->Move(-28);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		b = GetReader()->Read(&_chunk_index_current);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		b = GetReader()->Read(&_samples_per_chunk);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		//GetReader()->Move(4);  //sample_description_index
		uint32 sample_description_index;
		b = GetReader()->Read(&sample_description_index);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		b = GetReader()->Read(&_chunk_index_next);
#ifdef _CHECK_READ
		if (!b) return -1;
#endif //_CHECK_READ
		_current_entry--;
		_sample_current_count = _samples_per_chunk * (_chunk_index_next - _chunk_index_current);
		_sample_index_base -= _sample_current_count;
	}

	//uint32 chunkindex = _chunk_index_current + (index - _sample_index_base) / _samples_per_chunk;
	//uint32 samplebase = _sample_index_base + (index - _sample_index_base) / _samples_per_chunk * _samples_per_chunk;
	return _samples_per_chunk;
}

// StszBox
//==============================================================================

MP4RRC StszBox::Init(Stream* stream, uint32 bodySize)
{
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
		return rc;
	GetReader()->Move(4); // version, flag
	GetReader()->Read(&_sample_size);
	GetReader()->Read(&_entry_count);
	_data_pos = GetReader()->Position();
	return MP4RRC_OK;
}

uint32 StszBox::GetSampleSize(int index)
{
	if (_sample_size) //all the same
			return _sample_size;
	ASSERT(index >=0 && index < GetSampleCount());
	bool b = GetReader()->SetPosition(index * 4 + _data_pos);
#ifdef _CHECK_READ
	if (!b)
    {
        VOLOGE("SetPosition %d failed", index * 4 + _data_pos);
		return ERR_CHECK_READ;
    }
#endif //_CHECK_READ
	uint32 size;
	b = GetReader()->Read(&size);
#ifdef _CHECK_READ
	if (!b)
    {
        VOLOGE("Read 4 byte failed");
		return ERR_CHECK_READ;
    }
#endif //_CHECK_READ
	return size;
}


// StcoBox
//==============================================================================

MP4RRC StcoBox::Init(Stream* stream, uint32 bodySize)
{
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
		return rc;
	GetReader()->Move(4); // version, flag
	GetReader()->Read(&_entry_count);
	_data_pos = GetReader()->Position();
	return MP4RRC_OK;
}

uint32 StcoBox::GetChunkOffset(int index)
{
	ASSERT(index >=0 && index < GetChunkCount());
	GetReader()->SetPosition(index * 4 + _data_pos);
	uint32 offset;
	GetReader()->Read(&offset);
	return offset;
}

uint32 Co64Box::GetChunkOffset(int index)
{
	ASSERT(index >=0 && index < GetChunkCount());
	GetReader()->SetPosition(index * 8 + _data_pos);
	uint32 offset;
	GetReader()->Read(&offset); //ignore the high 4 bytes... TODO: support 64bits
	GetReader()->Read(&offset);
	return offset;
}

// Track vs TrackS1
//==============================================================================

TrackS1::TrackS1()
: Track()
{
	_index_cache = 0;
	_count_real_samples = 0;
	_pre_sample_size = 0;
}

uint32 Track::GetSampleCount()  
{ 
	return _stszBox.GetSampleCount(); 
}

uint32 TrackS1::GetSampleCount()
{
	uint32 count = _stcoBox->GetChunkCount(); 
	return count;
}

#if 1
uint32 TrackS1::GetSampleAddress(int index)
{
	while (_index_cache < index)
	{
		_count_real_samples += _stscBox.GetSamplesPerChunk(_index_cache);
		++_index_cache;
	}
	while (_index_cache > index)
	{
		--_index_cache;
		_count_real_samples -= _stscBox.GetSamplesPerChunk(_index_cache);
	}

	uint32 addr = uint32(_current_chunk_address = uint64(GetChunkOffset(index)));

	_current_chunk_size = GetSampleSize(index);
	_current_sample_chunk = index;
	return addr;
}
uint32 TrackS1::GetSampleSize(int index) 
{ 
	uint32 size =  _current_chunk_size = _stscBox.GetSamplesPerChunk(index) /_audioqtff.samplesPerPacket * _audioqtff.bytesPerFrame;/*_stscBox.GetSamplesPerChunk(index)*/
	_current_sample_chunk = index;
	return size; 
}
int TrackS1::FindSampleByTime(uint32 time) 
{ 
	uint32 delta = _sttsBox.GetCurrentDelta();
	uint32 ts = (uint32)(((uint64)time * GetTimeScale() + _time_divisor - 1) / _time_divisor / delta);

	uint32 size = _stscBox.GetSamplesPerChunk(0);
	int index = ts / size;
	return index; 
}
int TrackS1::GetPreSampleByteSize()
{  
	int size = GetChannelCount() * GetSampleBits() / 8 ;
	return size > 0? size :1;
}
#else
uint32 TrackS1::GetSampleAddress(int index)
{
	while (_index_cache < index)
	{
		_count_real_samples += _stscBox.GetSamplesPerChunk(_index_cache);
		++_index_cache;
	}
	while (_index_cache > index)
	{
		--_index_cache;
		_count_real_samples -= _stscBox.GetSamplesPerChunk(_index_cache);
	}
	//08/26/2011 add by leon
	if(!_pre_sample_size) _pre_sample_size = GetPreSampleByteSize();
	//08/26/2011 add by leon

	uint32 addr = _current_chunk_address = GetChunkOffset(index);
	//08/26/2011 modify by leon
	_current_chunk_size = _stscBox.GetSamplesPerChunk(index) * _pre_sample_size;/*_stscBox.GetSamplesPerChunk(index)*/
	_current_sample_chunk = index;
	return addr;
}
uint32 TrackS1::GetSampleSize(int index) 
{ 
	//08/26/2011 add by leon
	if(!_pre_sample_size) _pre_sample_size = GetPreSampleByteSize();
	//08/26/2011 add by leon

	//08/26/2011 modify by leon
	uint32 size =  _current_chunk_size = _stscBox.GetSamplesPerChunk(index) * _pre_sample_size;/*_stscBox.GetSamplesPerChunk(index)*/
	_current_sample_chunk = index;
	return size; 
}
int TrackS1::FindSampleByTime(uint32 time) 
{ 
	uint32 delta = _sttsBox.GetCurrentDelta();
	uint32 ts = (uint32)(((uint64)time * GetTimeScale() + _time_divisor - 1) / _time_divisor / delta);
	//simple way, support sample sizes are same (except last one)
	/*modify by Leon , #7794
	* ts is a pre byte time, so ...
	*/
	uint32 size = GetSampleSize(0) / GetPreSampleByteSize();
	/*uint32 size = GetSampleSize(0); */// so, use the size value of first chunk
	int index = ts / size;
	return index; 
}
//08/26/2011 add by leon
int TrackS1::GetPreSampleByteSize()
{  
	int size = GetChannelCount() * GetSampleBits() / 8 ;
	return size > 0? size :1;
}

//08/26/2011 add by leon
#endif

uint32 Track::GetSampleSize(int index) 
{ 
	return _stszBox.GetSampleSize(index); 
}

#ifndef _SUPPORT_TSOFFSET
uint64 Track::GetSampleTime(int32 index)
{
	return _time_divisor * (_sttsBox.GetSampleTime(index) + _cttsBox.GetSampleTime(index)) / GetTimeScale(); 
}

uint64 Track::GetSampleTime2(int32 index)
{ 
	return _time_divisor * (_sttsBox.GetSampleTime(index +1) + _cttsBox.GetSampleTime(index +1)) / GetTimeScale(); 
}
int Track::FindSampleByTime(uint32 time)
{ 
	return _sttsBox.GetSampleIndex(((uint64(time) * GetTimeScale() + _time_divisor - 1) / _time_divisor));
}

#endif

uint64 TrackS1::GetSampleTime(int32 index)
{
	if (index != _current_sample_chunk)
		GetSampleAddress(index);
	uint64 delta = _sttsBox.GetCurrentDelta();
	uint64 ts = _time_divisor * _count_real_samples * delta / GetTimeScale();
	return ts;
}

uint64 TrackS1::GetSampleTime2(int32 index)
{
	if (index != _current_sample_chunk)
		GetSampleAddress(index);
	uint64 delta = _sttsBox.GetCurrentDelta();
	uint64 ts = _time_divisor * (_count_real_samples + _current_chunk_size) * delta / GetTimeScale();
	return ts;
}


