
#ifndef __VO_OGG_VORBIS_TRACK_H_

#define __VO_OGG_VORBIS_TRACK_H_

#include "oggtrack.h"

#define MAX_VORBIS_SAMPLESIZE 65536

#define __VO_TEST

class ogg_vorbis_track : public oggtrack
{
public:
	ogg_vorbis_track(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CBaseStreamFileReader* pReader, VO_MEM_OPERATOR* pMemOp);
	~ogg_vorbis_track();

	virtual VO_U32 GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32 GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32 GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32 GetCodecCC(VO_U32* pCC);
	virtual VO_U32 Init(VO_U32 nSourceOpenFlags);
	virtual VO_U32 GetSampleN(VO_SOURCE_SAMPLE* pSample);

	virtual VO_U32 GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);

	virtual VO_U32 CalcTheDuration();
	static VO_U64 CalcTimeCodecByGranule(VO_PTR pUser, VO_U64 granule_pos);
protected:
	virtual VO_VOID add_sample( PBaseStreamMediaSampleIndexEntry ptr_entry );
	virtual VO_U64 calculate_timecode( VO_U32 packet_number , VO_U32 packets_in_page );

private:
	VO_VOID prepare_header_data();

	VO_BOOL is_first_frame;

#ifdef __VO_TEST
	FILE * m_fp;
#endif
};



#endif
