#ifndef _ISOM_FRAGMENT_H
#define _ISOM_FRAGMENT_H

#include "isomrd.h"
#include "voDrmCallback.h"
#include "fMacros.h"
#include "voOSFunc.h"
#include "voCMutex.h"
#include "voLog.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define  NEW_GET_FRAGMENT_INFO

#define VO_PID_FRAGMENT_DEFAULT_SAMPLE_SIZE			0x0001
#define VO_PID_FRAGMENT_DEFAULT_SAMPLE_DURATION		0x0002
enum ENUM_SIDX_TYPE
{
	ENUM_TOTAL_REF_SIDX_TYPE = 1,
	ENUM_PART_REF_SIDX_TYPE,
	ENUM_NO_REF_SIDX_TYPE 
};

typedef struct
{
	VO_BYTE breference_type;
	VO_U32  nreference_size;
	VO_U32  nsubsegment_duration;
	VO_BYTE bstarts_with_SAP;
	VO_U32  nSAP_type;
	VO_U32  nSAP_delta_time;
	VO_U64  nreference_data_offset;
} VO_SIDXReference;

typedef struct __sidx_structure
{
	VO_U32 reference_ID;
	VO_U32 timescale;
	VO_U64 earliest_presentation_time;
	VO_U64 first_offset;
	VO_U16 ref_count;
	ENUM_SIDX_TYPE sidx_type;
	VO_SIDXReference *pSidxrefs;
} VO_SegmentIndexStructure;

struct EncyptionBox
{
	VO_PBYTE	pBoxData;
	VO_U32		uBoxCurSize;
	VO_U32		uBoxMaxSize;
};

struct SENC_STRUCT_INFO
{
	VO_BYTE	 *pSenc;
	VO_U32    size;
};
//09/05/2011,leon
class TfraBox : public BufferInBox
{
public:
	TfraBox();
	MP4RRC Init(Stream* stream, uint32 bodySize);

	int32 GetSamplesPerChunk(int index);
	uint32 GetSampleIndexBase() const { return _sample_index_base; }
	/* not implement
	** uint32 GetCurFragmentSampleCount() const { return _sample_current_count; }
	 not implement*/

	uint32 GetTrackID() const { return _trackID;}
	MP4RRC SetFragmentInfoByIndex( int index);
	MP4RRC SetSeekingFragmentInfoByTime( VO_U64 ullTime);
	MP4RRC ReadOneTfraBlock();
	uint64 GetCurFragmentBeginTime() const {return _current_begin_time;}
	uint64 GetCruFragmentOffset(){return _current_offset;}
	VO_U32	GetCurSampleIndex(){return _current_sample_number;}
	VO_VOID  SetBaseMoofIndex(VO_U32 index){m_uTfraBaseMoofIndex = index;}
	VO_U32   GetBaseMoofIndex(){return m_uTfraBaseMoofIndex;}
	ABSPOS   GetBasePos(){return m_uTfraBasePos;}
	VO_VOID  SetBasePos(ABSPOS pos){
		m_uTfraBasePos = pos;
		VOLOGR("TfraBox::SetBasePos to set the m_uTfraBasePos and the pos is: %d",(int)pos);
	}
	ABSPOS   GetCurPos(){return m_uTfraCurrPos;}
private:
	uint32 _last_chunk_index; 
	uint32 _chunk_index_current;
	uint32 _chunk_index_next;
	uint32 _samples_per_chunk;
	uint32 _sample_index_base;
	uint32 _sample_current_count;

	uint32 _trackID;
	uint16 _length_size_of_traf_num;
	uint16 _length_size_of_trun_num;
	uint16 _length_size_of_sample_num;
	uint8 _version;
	uint32 _current_traf_number;
	uint32	_current_trun_number;
	uint32	_current_sample_number;
	//2012/01/19, uint32 to uint 64
	uint64 _current_begin_time;
	uint64 _current_offset;

	uint32 _current_index;

	//for seek 
	VO_U32	m_uTfraBaseMoofIndex; //save the moof index after seeking
	ABSPOS  m_uTfraBasePos;//save the postion of tfra after seeking
	ABSPOS  m_uTfraCurrPos;//save the postion of the tfra after current action
	ABSPOS  m_udatastartpos;//the data offset of the tfra box
	

};

//09/05/2011,leon
struct SAMPLE_INFO
{
	uint32 _sample_duration;
	uint32 _sample_size;
	uint32 _sample_flags;
	uint32 _sample_composition_time_offset;

	uint32 _sample_time;//add
	uint32 _sample_addr;

};

class SidxBox: public BufferInBox
{
public:
	SidxBox();
	~SidxBox();
	MP4RRC Init(Stream* stream, uint32 bodySize);
	MP4RRC InitSidx(Reader* r, uint32 bodySize);
	VO_SegmentIndexStructure *Init2(Stream* stream, ABSPOS baseoffset,uint32 bodySize);
	MP4RRC InitSidx2(Reader* r,uint32 bodySize); //for dash-if mode.
	VO_SegmentIndexStructure *InitRefsidx(Reader* r,ABSPOS sidxoffset,uint32 bodySize);
	VO_VOID GenerateFinalSidx(VO_SegmentIndexStructure *pSdx,uint32 num_offset);
	VO_VOID FreeSidxStructure(VO_SegmentIndexStructure *pSdx);
	uint32 GetTimeScale() const { return _timeScale;}
	uint32 GetDuration()const {return _duration; }
	uint32 GetTrackID()const {return _trackID; }
	VO_SegmentIndexStructure GetSementIndexStructure()const {return m_SidxStructure;}
private:
	uint32 _trackID;
	uint32 _timeScale;
	uint32 _duration;
	VO_SegmentIndexStructure m_SidxStructure;
};
class SaioBox: public BufferInBox
{
public:
	SaioBox();
	~SaioBox();
	MP4RRC Init(Stream* stream, uint32 bodySize);
	uint32 m_aux_info_type;
	uint32 m_aux_info_type_parameter;
	uint32 m_enrycount;
	uint64 *m_pOffsetArr;
};
class SaizBox: public BufferInBox
{
public:
	SaizBox();
	~SaizBox();
	MP4RRC Init(Stream* stream, uint32 bodySize);
	uint32 GetOffsetByIndex(uint32 index);
	uint32 m_aux_info_type;
	uint32 m_aux_info_type_parameter;
	uint8  m_default_sample_info_size;
	uint32 m_sample_count;
	uint8  *m_pSamplesizeArr;
};
class TfhdBox: public BufferInBox
{
public:
	TfhdBox();
	MP4RRC Init(Stream* stream, uint32 bodySize);
	uint32 GetTrackId() const { return m_nTrackID;}
	uint32 GetSampleDescriptionIndex() const { return m_nSampleDescriptionIndex;}
	uint32 GetDefaultSampleDuration() const { return m_nDefaultSampleDuration;}
	uint32 GetDefaultSampleSize() const { return m_nDefaultSampleSize;}
	uint32 GetDefaultSampleFlags() const { return m_nDefaultSampleFlags;}

	uint32 SetParam(int id, VO_PTR pParam);
private:
	uint32 m_nTrackID;
	uint32 m_nSampleDescriptionIndex;
	uint32 m_nDefaultSampleDuration;
	uint32 m_nDefaultSampleSize;
	uint32 m_nDefaultSampleFlags;
	bool m_bNoDefaultDuration;

};

class TrunBox: public BufferInBox
{
public:
	MP4RRC Init(Stream* stream, uint32 bodySize);
	uint32 GetSampleCount() const { return _sample_count;}
	uint32 GetSampleSize(int index) ;
	uint64 GetSampleDuration(int index);
	uint64 GetSampleTime(int index);
	uint64 GetSampleAddress(int index);
	VO_BOOL GetNoSampleDuration(){return m_bNoSampleDuration;}
	uint32 SetParam(int id, VO_PTR pParam);
private:
	MP4RRC GetSampleInfo(int index);
	uint32 _sample_count;
	uint32 m_nDefaultDuration;
	uint32 m_nDefaultSize;
	uint32 _tr_flags;
	VO_BOOL m_bNoSampleDuration;

	SAMPLE_INFO _present_sample_info;
};

struct FRAGMENT_INFO
{
	///<from tfhd box
	uint32 _sample_description_index;
	uint32 _default_sample_duration;
	uint32 _default_sample_size;
	uint32 _default_sample_flags;

	uint32 _sample_count;///<trun sample count of current moof
	uint32 _current_index;

	uint32 _moof_size;
	uint64 _sample_data_offset;///<mdata positon of current trun to related
	uint32 _sync_index;//the first sample of moof
	int32 _sync_next_index;
	int32 _sync_prev_index;
	
	///<from tfra box
	uint64 _begin_time;
	uint64 _moof_offset;
	EncyptionBox	EncyptBox;
	int _current_entry;
	VO_VOID Reset();
	VO_VOID Uninit();

};
struct MIN_FRAGMENT_INFO
{
	uint32 _syncIndex;
	uint32 _sampleCounts;
	uint64 _beginTime;
	uint64 _duration;
	uint32 _sequenceNum;
	uint64 _moofOffset;
	struct MIN_FRAGMENT_INFO *pNext;
};

class FragmentsBox
{
public: 
	FragmentsBox();
	virtual ~FragmentsBox();
	MP4RRC			Init(Reader* r, int trackIndex);
	virtual VO_U32	GetSampleCount();
	uint32			GetSampleSize(int index);
	virtual uint64	GetSampleTime(int index);
	uint64			GetSampleAddress(int index);
	virtual int		GetSampleSync(int index);//default is sync
	virtual int		GetNextSyncPoint(int index);//default is sync
	virtual int		GetPrevSyncPoint(int index);//default is sync
	virtual uint32	GetSampleIndex(uint64 pos);
	VO_U32			ResetIndex();
	//2012/02/08, add for leon for dash
	virtual MP4RRC	GetFragmentInfo(int index);
	virtual VO_BOOL IsNeedGather21Chunk() {   return VO_BOOL(_trunBox.GetNoSampleDuration() && GetNoDefaultDuration()); }
	virtual VO_BOOL	GetNoDefaultDuration(){ return m_bNoDefaultDuration;}
	virtual VO_U32	GetTrackID() { return _tfhdBox.GetTrackId();}
	virtual void	SetTrackPos(int nTrackPos) { m_nTrackPos = nTrackPos;}
	virtual MP4RRC	SetParam(VO_U32 ID, VO_PTR pParam);	
	VO_VOID			SetBoxType(VO_U32 uBoxType);
	MP4RRC			ParserUUID(Reader* r, FRAGMENT_INFO *pInfo, int bodySize);
	virtual VO_U32	GenerateIndex(VO_U32 uIndex, Reader* pReader);
	virtual VO_BOOL	IsIndexReady(VO_U32 uIndex, VO_U32* pMaxIndex);
	virtual VO_U32	GetMaxTimeByIndex(VO_U32 uIndex,VO_S64& ullTs);
	virtual	VO_VOID SetSampleCount(VO_U32 uSampleCnt);
	virtual VO_U64	SeekToMoofByTime(VO_U64 ullTime);
	VO_VOID         SetCencDrmFlag(VO_BOOL bvalue){m_bIsCencDrm = bvalue;}
private:
	virtual int		FindSampleByIndex(void *pValue, int flag = 1);
	virtual MP4RRC	GetFragmentInfo(int index, FRAGMENT_INFO &ppInfo,TrunBox &pTrunBox,VO_BOOL bNeedDrmInfo,Reader *pReader = NULL);//this index indicates the number of moof index

	MP4RRC			GetFragmentInfo();//get whole fragment info, save to *_pFragment_info
	MP4RRC			CreateFragmentInfo();
	MP4RRC			FindMoofInfoByIndex(VO_U64 uIndex,VO_U64* pPos,VO_U64 *pTs, Reader *pReader = NULL);
	VO_U64			GetFragmentDuration(TrunBox& trunBox);
	MIN_FRAGMENT_INFO * GetFragmentInfoBySequence(VO_U32 uSequence,VO_U32 uEntryCnt,MIN_FRAGMENT_INFO ** pLast = NULL);
	VO_BOOL				IsFragmentUsed(MIN_FRAGMENT_INFO * pFragmentInfo);
	MIN_FRAGMENT_INFO*	NewFragmentInfoEntry();
	VO_VOID				DelFragmentInfoEntry(MIN_FRAGMENT_INFO* pEntry);
public:
	Reader *			m_pReader;
	VO_U32				m_uSampleCount;
	VO_U32				m_uEnableBox;
	int					_current_index;
	uint64				m_nBaseMediaDecodetime;
	FRAGMENT_INFO		_current_fragment_info;
	TrunBox				_trunBox;
	MIN_FRAGMENT_INFO *	_pMin_Fragment_info;
	SENC_STRUCT_INFO	*m_pSencInfo;
	VO_BOOL              m_bIsCencDrm;
	SaizBox              m_SaizBox;
	SaioBox				 m_SaioBox;
private:	
	uint64				_present_time;
	int					m_nTrackID;
	int					m_FilterTrackID;
	int					m_nTrackPos;
	TfhdBox				_tfhdBox;
	TfraBox				_tfraBox;
	ABSPOS				_tfraPos;
	VO_BOOL				m_bNoDefaultDuration;
	voCMutex			m_lock;
	VO_U32				m_uBaseMoofIndex;
};

class TrackFragment : public Track
{
public:
	TrackFragment();
	~TrackFragment();
	virtual MP4RRC		Open(Reader* r);

	virtual uint32		GetSampleCount();
	virtual uint32		GetSampleSize(int index);
	virtual uint64		GetSampleTime(int32 index);
	virtual uint64		GetSampleTime2(int32 index);
	virtual uint32		GetBitrate();
	uint32				GetMaxSampleSize();

	virtual int			FindSampleByTime(uint32 time) ;

	virtual uint32		GetSampleSync(int index); //always sync
	virtual int			GetNextSyncPoint(int index);
	virtual int			GetPrevSyncPoint(int index);

	uint32				GetSampleAddress(int index);
	uint32				GetSampleData(int index, void* buffer, int buffersize);
	VO_U32				TrackGenerateIndex(VO_U32 uIndex, Reader* pReader);
	VO_BOOL				IsIndexReady(VO_U32 uIndex, VO_U32* pMaxIndex);
	VO_U32				GetMaxTimeByIndex(VO_U32 uIndex,VO_S64& ullTs);
	VO_U32				ResetIndex();
protected:
	virtual MP4RRC		ParseStsdBox(Reader* r, int bodySize);
	virtual MP4RRC		ParseDescriptor(Reader* r, int bodysize);
	
	
private:

	int					_index_cache;
	uint32				_count_real_samples;
	uint32				_pre_sample_size;
	int					_trackIndex;
	FragmentsBox		_fragmentBox;
	int					_IsEncryption;
	
};

//} //namespace ISOMR

#ifdef _VONAMESPACE
}
#endif
#endif	//_ISOM_RD_H