

#include "SegmentTrack.h"
#include "isomscan.h"
#include "mpxutil.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

 SegmentsBox::SegmentsBox()
	 :FragmentsBox()
	 ,m_bSidxBox(false)
	 ,m_nSampleDataOffset(0)
 {

 }
 SegmentsBox::~SegmentsBox()
 {
 
 }

MP4RRC SegmentsBox::GetFragmentInfo(int index)
 {
	 MP4RRC rc = MP4RRC_OK;
	 rc = FragmentsBox::GetFragmentInfo(index);
	 if(rc != MP4RRC_OK)
		 return rc;
	 SetSampleCount(_trunBox.GetSampleCount());
	 _trunBox.SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_DURATION,&_current_fragment_info._default_sample_duration);
	 _trunBox.SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_SIZE,&_current_fragment_info._default_sample_size);

	 return rc;
 }

  MP4RRC SegmentsBox::GetFragmentInfo()
  {
  	uint32 entrycount = 1;//_tfraBox.GetEntryCount();
  	_pMin_Fragment_info = new MIN_FRAGMENT_INFO[entrycount];
  	memset(_pMin_Fragment_info, 0, sizeof(MIN_FRAGMENT_INFO) * entrycount);
  	return MP4RRC_OK;
  }
    int SegmentsBox::FindSampleByIndex(void *pValue, int flag /*==1*/)
    {
  		return *((int*)pValue);
    }
  uint64 SegmentsBox::GetSampleTime2(int index , int nTimeScale)
  {
		return (uint64)(m_nBaseMediaDecodetime +_trunBox.GetSampleTime(index) );
  }
uint64 SegmentsBox::GetTimeScale()
{
	return uint64(_sidxBox.GetTimeScale());
}
  uint64 SegmentsBox::GetDurationByIndex(int index)
  {
	 
	return _trunBox.GetSampleDuration(index);

  }

  uint64 SegmentsBox::GetSampleTime(int index )
  {
	  if(m_bSidxBox)
		  return  (uint64) (1.0 * index *_sidxBox.GetDuration() /_sidxBox.GetTimeScale()/ GetSampleCount()) * SEGMENT_TIME_UNIT ;
	  else
		  return  (uint64) (_trunBox.GetSampleTime(index)) ;

	  
	  return 0 ;
  }

 MP4RRC SegmentsBox::Init(Reader* r,int trackIndex)
 {
 	MP4RRC rc;
 	r->SetPosition(0);	
 	Scanner scanner(r);
 	m_pReader = r;
	uint32 sidxsize = scanner.FindTag(FOURCC2_sidx);
	if(sidxsize > 0 )
	{
		_sidxBox.Init(r->GetStream(), sidxsize);
		m_bSidxBox = true;
	}
	else
	{
		m_bSidxBox = false;
		m_pReader->SetPosition(0);
	}
 	rc =GetFragmentInfo();
	rc =GetFragmentInfo(-2);
 	return rc;
 }
 int SegmentsBox::GetSampleSync(int index) //default is sync
 {
 	uint32 sindex  = FindSampleByIndex(  &index);
 	return sindex ? 0 : 1;
 }
 uint64 SegmentsBox::GetAllSampleSize()
 {
	 uint64 nAllSize = 0;
	 for(uint32 i = 0; i< GetSampleCount(); i ++)
		 nAllSize += GetSampleSize(i);
	 return nAllSize;
 }

TrackSegment::TrackSegment()
: Track()
{
	_index_cache = 0;
	_count_real_samples = 0;
	_pre_sample_size = 0;
	m_pSenc = 0;
}
TrackSegment::~TrackSegment()
{

}
uint32 TrackSegment::GetMaxSampleSize()
{
	if(IsVideo())
		return GetWidth() * GetHeight() * 1;
	if(IsAudio())
		return GetSampleBits() * GetChannelCount() * GetSampleRate() / 8;
	return 0;
}
uint32 TrackSegment::GetBitrate()
{
	if(IsAudio())
		return Track::GetBitrate();
	 return 0;
}
MP4RRC TrackSegment::SetDrmCallBack(CvoDrmCallback *pDrmCallback)
{
	return MP4RRC_OK;// _fragmentBox.SetDrmCallBack((VO_PTR)pDrmCallback);
}
MP4RRC TrackSegment::Open(Reader* r)
{
	MP4RRC rcc = Track::Open(r);
	if(rcc != MP4RRC_OK) return rcc;

	//_fragmentBox.Init(r, GetTrackID());
	_segmentBox.Init(r, GetTrackID());

	return MP4RRC_OK;
}
uint32 TrackSegment::GetSampleCount()  
{ 
	return _segmentBox.GetSampleCount();
}

uint32 TrackSegment::GetSampleSize(int index)
{
	return _segmentBox.GetSampleSize(index);
}

uint64 TrackSegment::GetSampleTime(int32 index)
{

	uint64 time =_segmentBox.GetSampleTime(index);
	return time;
}
int TrackSegment::FindSampleByTime(uint32 time)
{
	return 0;
}
uint64 TrackSegment::GetSampleTime2(int32 index)
{
	return 0;
}
uint32 TrackSegment::GetSampleAddress(int index)
{
	_current_chunk_size = GetSampleSize(index);
	_current_chunk_address = _segmentBox.GetSampleAddress(index);
	_current_sample_chunk = index;
	VOLOGR("IsAudio:%d ,index: %d, addr: 0x%08x",IsAudio(), index, _current_chunk_address);
	return uint32(_current_chunk_address);
}
uint32 TrackSegment::GetSampleSync(int index)
{
	if( !IsVideo() ) return 0;
	return _segmentBox.GetSampleSync(index);
}
int TrackSegment::GetNextSyncPoint(int index)
{
	return -1;//_fragmentBox.GetNextSyncPoint(index);
}
int TrackSegment::GetPrevSyncPoint(int index)
{
	return 0;//_fragmentBox.GetPrevSyncPoint(index);
}
MP4RRC TrackSegment::ParseDescriptor(Reader* r, int bodysize)
{
	return Track::ParseDescriptor(r,bodysize);
}

MP4RRC TrackSegment::ParseStsdBox(Reader* r, int bodySize)
{
	//stsd box include 
	ABSPOS mark = r->Position();
	MP4RRC rrc = Track::ParseStsdBox(r,bodySize);
	if(rrc != MP4RRC_OK) return rrc;
	
	_IsEncryption = 0;


 	if(_sample_type != FOURCC2_encv && _sample_type != FOURCC2_enca) return MP4RRC_OK;
	_IsEncryption = 1;

	r->SetPosition(mark);
  	if(_sample_type == FOURCC2_enca) r->Move(24+ 20);
	else if(_sample_type == FOURCC2_encv) r->Move(24+ 70);
 
  	Scanner scanner(r);
  	int sinfsize = scanner.FindTag(FOURCC2_sinf);
  	if(sinfsize <= 0) return MP4RRC_NO_SINF;
  	ABSPOS mark2 = r->Position();
  	int frmasize = scanner.FindTag(FOURCC2_frma, mark2 + sinfsize);
  	if(frmasize <=0)	return MP4RRC_NO_FRMA;
  	r->Read(&_sample_type);

	ABSPOS mark3 = r->Position();
	int schisize = scanner.FindTag(FOURCC2_schi,mark2 + sinfsize);
	if(schisize <=0)	return MP4RRC_NO_SCHI;
#ifndef CENC
	m_bIsCencDrm = VO_FALSE;
	int tencsize = scanner.FindTag(FOURCC2_tenc,mark3 + schisize);
	if(tencsize > 0)
	{
		int fulltencsize = tencsize + 8;
		r->Move(-8);//size and type
		VO_BYTE *pTenc = new VO_BYTE[fulltencsize];
		memset(pTenc,0,fulltencsize);
		r->Read(pTenc,fulltencsize);
		//VO_DRM2_INFO_GENERAL_DATA cencdata;
		VO_DRM2_TRACKDECYPTION_INFO cencdata;
		cencdata.pTrackEncryptionBox = pTenc;
		cencdata.uTrackEncryptionBox = fulltencsize;
		cencdata.uTrackID = _nTrack_ID;
		_movie->GetCencDrmCallback()->FRCB(_movie->GetCencDrmCallback()->pUserData,NULL,NULL,NULL,NULL,VO_DRM2DATATYPE_CENC_TENC,VO_DRM2_DATAINFO_SAMPLEDECYPTION_BOX,&cencdata);
		delete []pTenc;
		pTenc = NULL;
		m_bIsCencDrm = VO_TRUE;
	}
#endif		
	Scanner scanner1(r);
	scanner.FindTag(FOURCC2_uuid, r->Position() + schisize);
 	return MP4RRC_OK;
}


uint32 TrackSegment::GetSampleData(int index, void* buffer, int buffersize)
{
	uint32 sizeSample = Track::GetSampleData(index, buffer,buffersize);

	if(_IsEncryption ) 
		int i= 0;//_fragmentBox.DecryptionByIndex(IsVideo(), index, buffer,sizeSample);
	return sizeSample;
}