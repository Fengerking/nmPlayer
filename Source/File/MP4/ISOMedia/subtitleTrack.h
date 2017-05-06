#ifndef _SUBTITLE_TRACK_H
#define _SUBTITLE_TRACK_H





#include "isomrd.h"
#include "subtitleType.h"
#include "3gpTTextOp.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif



class TrackSubtitle : public Track
{
public:
	TrackSubtitle();
	~TrackSubtitle();

	//virtual uint32 GetSampleSize(int index);


#ifdef _SUPPORT_TTEXT
	uint32 GetBitrate();
	virtual MP4RRC Open(Reader* r);
	uint32 GetMaxSampleSize();
	uint32 GetSampleData(int index, void* buffer, int buffersize);
	int GetSampleChunk(int index, int& chunksampleindex, int& chunksamplecount) {  return _stscBox.GetSampleChunk2(index, chunksampleindex, chunksamplecount);  }
	virtual uint32 GetSampleAddress(int index);
	MP4RRC SetDrmCallBack(CvoDrmCallback *pDrmCallback);
	virtual uint32 GetSampleBuffer(void **ptr) { *ptr = &m_stTx3gTextSample; return 0;}
#endif

	virtual MP4RRC ParseDescriptor(Reader* r, int bodysize);
	MP4RRC ParseTextSampleEntry(Reader* r, int bodysize);

	virtual MP4RRC Close();

private:
	MP4RRC ParseTextSample(Reader* r, Tx3gTextSample *pTextSample, int bodysize);
	MP4RRC ParseStyleRecord(Reader* r, Tx3gStyleRecord *ptr);
	MP4RRC ParseRGBAColor(Reader* r, Tx3gRGBAColor *ptr);
	MP4RRC ParseHrefBox(Reader* r, Tx3gTextHyperTextBox *ptr);
	MP4RRC ParseKaraokeBox(Reader* r, Tx3gTextKaraokeBox *ptr);
	MP4RRC ParseBoxRecord(Reader* r, Tx3gBoxRecord *ptr);
	MP4RRC ParseFontRecord(Reader* r, Tx3gFontRecord *ptr);
private:

	Tx3gTTextOp m_3gpTTextOp;

	Tx3gDescription m_stTx3gDescription;
	Tx3gTextSample m_stTx3gTextSample;
	int m_nCurTextLength;

private:

	int _index_cache;
	uint32 _count_real_samples;
	uint32 _pre_sample_size;
	//	int GetPreSampleByteSize();

	int _trackIndex;

	int _IsEncryption;

	Reader *m_pReader;
	
};


#ifdef _VONAMESPACE
}
#endif
#endif	//_SUBTITLE_TRACK_H