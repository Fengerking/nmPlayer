#ifndef _ISOM_SEGMENT_H
#define _ISOM_SEGMENT_H

#include "isomrd.h"
#include "fragmentTrack.h"
#include "voDrmCallback.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#define SEGMENT_TIME_UNIT 1000

class SegmentsBox: public FragmentsBox
 {
 public: 
 	SegmentsBox();
 	virtual ~SegmentsBox();
 	MP4RRC Init(Reader* r, int trackIndex);
//  	uint32 GetSampleCount() const { return _sample_count;}
//  	uint32 GetSampleSize(int index);
  	virtual uint64 GetSampleTime(int index);
	virtual uint64 GetSampleTime2(int index, int nTimeScale);
	virtual uint64 GetDurationByIndex(int index);
	virtual uint64 GetTimeScale();
	virtual uint64 GetAllSampleSize(); 
	virtual int GetSampleSync(int index);//default is sync
	virtual MP4RRC GetFragmentInfo(int index);
	virtual int FindSampleByIndex(void *pValue, int flag = 1);

	
 private:
 	MP4RRC GetFragmentInfo();//get whole fragment info, save to *_pFragment_info
	
 public:
 
 	MP4RRC SetDrmCallBack(VO_PTR pValue);
 	
 	uint32 DecryptionByIndex(int isVideo, int index, void* buffer, int buffersize);
	
 private:
	 SidxBox _sidxBox;
	 bool m_bSidxBox;
	 VO_U64 m_nSampleDataOffset;
 };

class TrackSegment : public Track
{
public:
	TrackSegment();
	~TrackSegment();
	virtual MP4RRC Open(Reader* r);

	virtual uint32 GetSampleCount();
	virtual uint32 GetSampleSize(int index);
	virtual uint64 GetSampleTime(int32 index);
	virtual uint64 GetSampleTime2(int32 index);
	virtual uint32 GetBitrate();
	uint32 GetMaxSampleSize();

	virtual int FindSampleByTime(uint32 time) ;

	virtual uint32 GetSampleSync(int index); //always sync
	virtual int GetNextSyncPoint(int index);
	virtual int GetPrevSyncPoint(int index);

	uint32 GetSampleAddress(int index);
	virtual MP4RRC ParseStsdBox(Reader* r, int bodySize);
	virtual MP4RRC ParseDescriptor(Reader* r, int bodysize);
	uint32 GetSampleData(int index, void* buffer, int buffersize);


	MP4RRC SetDrmCallBack(CvoDrmCallback *pDrmCallback);
private:
	SegmentsBox _segmentBox;
	int _index_cache;
	uint32 _count_real_samples;
	uint32 _pre_sample_size;
	//	int GetPreSampleByteSize();

	int _trackIndex;
//	FragmentsBox _fragmentBox;
	int _IsEncryption;
	char *m_pSenc;//cenc drm senc box;	
};
#ifdef _VONAMESPACE
}
#endif
#endif	//_ISOM_SEGMENT_H