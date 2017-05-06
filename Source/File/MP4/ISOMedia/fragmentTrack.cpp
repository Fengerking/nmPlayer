#include "fragmentTrack.h"
#include "isomscan.h"
#include "mpxutil.h"
#include "voLog.h"
#include "ISOBMFileDataStruct.h"
#include "CMp4Reader2.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#ifdef NEW_GET_FRAGMENT_INFO
#define FIRST_LOAD_INDEX 10
#endif

typedef VO_U8 uuid[16];
/** The uuid of a SampleEncryptionBox, namely a specific UUIDBox. */
static const uuid SAMPLE_ENCRYTION_UUID = {  0xa2, 0x39, 0x4f, 0x52,
0x5a, 0x9b, 0x4f, 0x14,
0xa2, 0x44, 0x6c, 0x42,
0x7c, 0x64, 0x8d, 0xf4 };
/** The uuid of a ProtectionSystemSpecificHeaderBox, namely a specific UUIDBox. */
static const uuid PROTECTION_HEADER_UUID = {  0xd0, 0x8a, 0x4f, 0x18,
0x10, 0xf3, 0x4a, 0x82,
0xb6, 0xc8, 0x32, 0xd8,
0xab, 0xa1, 0x83, 0xd3 };
//TrackEncryptionBox extend_type
static const uuid DEFAULT_ENCRYPTION_UUID = {  0x89, 0x74, 0xdb, 0xce,
0x7b, 0xe7, 0x4c, 0x51,
0x84, 0xf9, 
0x71, 0x48,0xf9, 0x88, 0x25, 0x54 };
//Microsoft Playready systemID
static const uuid PLAYREADY_SYSTEM_ID = {  0x9A, 0x04, 0xF0, 0x79, 
0x98, 0x40, 0x42, 0x86,
0xAB, 0x92, 0xE6, 0x5B,
0xE0, 0x88, 0x5F, 0x95 };

TfraBox::TfraBox()
:_trackID(VO_MAXU32)
 ,m_uTfraBaseMoofIndex(0)
 ,m_uTfraBasePos(0)
 ,m_uTfraCurrPos(0)
 ,m_udatastartpos(0)
{
	VOLOGR("TfraBox::TfraBox()*************enter TfraBox structure reset m_uTfraBasePos!*******");
}

MP4RRC TfraBox::Init(Stream* stream, uint32 bodySize)
{
	SetMaxInitSize(bodySize + 8);
	VOLOGR("TfraBox::Init------start to init buffer with the size BufferInBox::Init body size = %d\n",bodySize);
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
	{
		VOLOGR("TfraBox::Init------failed to init buffer with the size BufferInBox::Init return value = %d\n",rc);
		return rc;
	}
	VOLOGR("TfraBox::Init------succeed to init buffer with the size BufferInBox::Init body size = %d\n",bodySize);
	uint32 rr = 0;
	GetReader()->Read(&_version); // version, flag
	GetReader()->Move(3);
	GetReader()->Read(&_trackID);
	GetReader()->Read(&rr);
	_length_size_of_traf_num = (rr >>4) & 0x3;
	_length_size_of_trun_num = (rr >> 2) & 0x3;
	_length_size_of_sample_num = rr & 0x3;
	GetReader()->Read(&_entry_count);
	_data_pos = GetReader()->Position();
	m_uTfraCurrPos = _data_pos;
	m_uTfraBasePos = _data_pos;
	m_udatastartpos = _data_pos;
	VOLOGR("TfraBox::Init------****** first init the m_uTfraBasePos and address is: %d; m_udatastartpos and address is: %d",(int)m_uTfraBasePos,(int)m_udatastartpos);
	return MP4RRC_OK;
}

MP4RRC TfraBox::ReadOneTfraBlock()
{
	if(_version ==1)
	{
		uint64 _tmp_offset = 0;
		uint64 _tmp_time = 0;
		GetReader()->Read(&_tmp_time);
		GetReader()->Read(&_tmp_offset);
		_current_begin_time = _tmp_time;
		_current_offset = _tmp_offset;
	}
	else
	{
		uint32 _tmp_offset = 0;
		uint32 _tmp_time = 0;
		GetReader()->Read(&_tmp_time);
		GetReader()->Read(&_tmp_offset);
		_current_begin_time = _tmp_time;
		_current_offset = _tmp_offset;
	}
	_current_traf_number = 0; 
	GetReader()->Read(&_current_traf_number,_length_size_of_sample_num +1);
	_current_trun_number= 0 ;
	GetReader()->Read(&_current_trun_number,_length_size_of_traf_num +1);
	_current_sample_number = 0;
	GetReader()->Read(&_current_sample_number,_length_size_of_trun_num +1);
	return MP4RRC_OK;
}

MP4RRC TfraBox::SetSeekingFragmentInfoByTime( VO_U64 ullTime)
{
	uint32 size =0;

	_version ==1 ? size += 16 : size += 8;

	size += _length_size_of_sample_num +1;
	size += _length_size_of_traf_num +1;
	size += _length_size_of_trun_num +1;

	GetReader()->SetPosition(m_uTfraBasePos);
	ABSPOS prepos = m_uTfraBasePos;
	ReadOneTfraBlock();
	uint32 i = 0;
	int IsFound = 0;
	VOLOGR("TfraBox::SetSeekingFragmentInfoByTime _current_begin_time: %d",(uint32)_current_begin_time);
	if(_current_begin_time >= ullTime) //back to seek
	{
		if(m_udatastartpos > prepos - size)
		{
			m_uTfraBaseMoofIndex = 0;
			m_uTfraBasePos = m_udatastartpos;
			GetReader()->SetPosition(m_uTfraBasePos);
			ReadOneTfraBlock();
			GetReader()->SetPosition(m_uTfraBasePos);
			return MP4RRC_OK;
		}
		prepos -= size;
		GetReader()->SetPosition(prepos);
		for(i = m_uTfraBaseMoofIndex; i >= 0; i--)
		{
			ReadOneTfraBlock();
			if(_current_begin_time <= ullTime)
			{
				m_uTfraBaseMoofIndex = i - 1;
				m_uTfraBasePos = prepos;
				GetReader()->SetPosition(prepos);
			}
			else
			{
				//GetReader()->Move(-size*2);
				prepos -= size;
				GetReader()->SetPosition(prepos);
				
				continue;
			}
			return MP4RRC_OK;
		}
	}
	else
	{
		for(i = m_uTfraBaseMoofIndex; i < _entry_count; i++)
		{

			ReadOneTfraBlock();
			if(_current_begin_time > ullTime)
			{
				if(i > 1)
				{
					m_uTfraBaseMoofIndex = i - 1;
					
				}
				m_uTfraBasePos = prepos - size;
				IsFound = 1;
				break;
			}
			else
			{
				prepos += size;
				GetReader()->SetPosition(prepos);
				continue;
			}
			
		}
		if(!IsFound)
		{
			if(i > 0)
				m_uTfraBaseMoofIndex = i - 1;
			m_uTfraBasePos = prepos - size;
			GetReader()->SetPosition(m_uTfraBasePos);
		}
		_current_index = m_uTfraBaseMoofIndex;
		return MP4RRC_OK;
	}
	return MP4RC_NO_SAMPLE;
}

MP4RRC TfraBox::SetFragmentInfoByIndex(int index)
{
	uint32 size =0;

	_version ==1 ? size += 16 : size += 8;

	size += _length_size_of_sample_num +1;
	size += _length_size_of_traf_num +1;
	size += _length_size_of_trun_num +1;
	VOLOGR("+TfraBox::SetFragmentInfoByIndex to GetReader()->SetPosition address m_uTfraBasePos :%d",(int)m_uTfraBasePos);
	//GetReader()->SetPosition(_data_pos);
	GetReader()->SetPosition(m_uTfraBasePos);
	for(uint32 i = m_uTfraBaseMoofIndex; i< _entry_count; i++)
	{
		VOLOGR("TfraBox::SetFragmentInfoByIndex begin to tfra moof current index :%d m_uTfraBaseMoofIndex: %d",i,m_uTfraBaseMoofIndex);
		if(i !=uint32( index))
		{
			GetReader()->Move(size);
			continue;
		}

		if(_version ==1)
		{
			uint64 _tmp_offset = 0;
			uint64 _tmp_time = 0;
			GetReader()->Read(&_tmp_time);
			GetReader()->Read(&_tmp_offset);
			_current_begin_time = _tmp_time;
			_current_offset = _tmp_offset;
		}
		else
		{
			uint32 _tmp_offset = 0;
			uint32 _tmp_time = 0;
			GetReader()->Read(&_tmp_time);
			GetReader()->Read(&_tmp_offset);
			_current_begin_time = _tmp_time;
			_current_offset = _tmp_offset;
		}
		_current_traf_number = 0; 
		GetReader()->Read(&_current_traf_number,_length_size_of_sample_num +1);
		_current_trun_number= 0 ;
		GetReader()->Read(&_current_trun_number,_length_size_of_traf_num +1);
		_current_sample_number = 0;
		GetReader()->Read(&_current_sample_number,_length_size_of_trun_num +1);
		_current_index = i;
		VOLOGR("-TfraBox::SetFragmentInfoByIndex  m_uTfraBasePos address  :%d  _current_begin_time: %d  _current_offset: %d",(int)m_uTfraBasePos,(int)_current_begin_time,(int)_current_offset);
		return MP4RRC_OK;
	}
	return MP4RC_NO_SAMPLE;
}

FragmentsBox::FragmentsBox()
{
	_current_index = 0;
	m_nBaseMediaDecodetime= 0;
	m_bNoDefaultDuration = VO_FALSE;

	m_nTrackID = -1;
	m_nTrackPos = 0;
	memset(&_current_fragment_info, 0 , sizeof(FRAGMENT_INFO));
	_current_fragment_info._current_entry = -1 ;
	_pMin_Fragment_info = NULL;
	m_FilterTrackID = -1;
	m_uEnableBox = VO_MAXU32;
	m_uBaseMoofIndex = 0;
	m_pSencInfo = NULL;
	m_bIsCencDrm = VO_FALSE;
}
FragmentsBox::~FragmentsBox()
{
	VOLOGR("+~FragmentsBox");
	_current_fragment_info.Uninit();
	DelFragmentInfoEntry(_pMin_Fragment_info);
	_pMin_Fragment_info = NULL;
	if(m_pSencInfo)
	{
		delete []m_pSencInfo->pSenc;
		delete m_pSencInfo;
		m_pSencInfo = NULL;
	}
	VOLOGR("-~FragmentsBox");
}

MP4RRC FragmentsBox::GetFragmentInfo(int index)
{
	return GetFragmentInfo(index, _current_fragment_info, _trunBox,VO_TRUE);
}

MP4RRC FragmentsBox::GetFragmentInfo(int index, FRAGMENT_INFO &ppInfo,TrunBox &pTrunBox,VO_BOOL bNeedDrmInfo,Reader *pReader)
{
	index += m_uBaseMoofIndex;
	voCAutoLock lock(&m_lock);
	VOLOGR("+GetFragmentInfo");
	FRAGMENT_INFO *tmpInfo = &ppInfo;
	tmpInfo->Reset();
	
	MP4RRC rc;
	int64 moof_offset = 0 ;
	///<the pReader != NULL indicate use the other reader to only get some infromation
	Reader* pTmpReader = (pReader == NULL ? m_pReader : pReader);
	TfhdBox tmp;
	TfhdBox& tfhdBox = (pReader == NULL ? _tfhdBox : tmp);
	VOLOGR("+GetFragmentInfo 1:%x,%x",pTmpReader,index);
	Scanner scanner(pTmpReader);
	if(index != -2)
	{
		if (MP4RRC_OK != FindMoofInfoByIndex(index,&tmpInfo->_moof_offset,&tmpInfo->_begin_time, pTmpReader))
		{
			VOLOGI("Failed to find moof with FindMoofInfoByIndex function");
			return MP4RRC_NO_MOOF;
		}
		moof_offset = tmpInfo->_moof_offset;
	}
	else
	{
		moof_offset = 0;
		tmpInfo->_begin_time = 0;

		uint32 moofsize = scanner.FindTag(FOURCC2_moof);

		if(moofsize > 0)
			moof_offset = pTmpReader->Position() - 8;
		else
		{
			VOLOGI("Failed to find moof with scanner.FindTag(...) funciton");
			return MP4RRC_NO_MOOF;
		}

	}
	VOLOGR("log1");
	VOLOGR("FragmentsBox::GetFragmentInfo pTmpReader->SetPosition and moof_offset: %d",(int)moof_offset);
	pTmpReader->SetPosition(moof_offset);
	int moofsize = scanner.FindTag(FOURCC2_moof);

	if(moofsize <=0) 
	{
		VOLOGI(" moof size <= 0 than return error");
		return MP4RRC_NO_MOOF;
	}
	tmpInfo->_moof_size = moofsize;
	
	ABSPOS mark = pTmpReader->Position();
	int trafsize = 0;
	int tfhdsize = 0;
	ABSPOS offset = mark;
	ABSPOS trafoffset = 0;
	ABSPOS tmpposition = 0;

	for(VO_U32 i =0; ;i++)
	{
		pTmpReader->SetPosition(offset);
		VOLOGR("FragmentsBox::GetFragmentInfo find moof traf, moof size: %d",moofsize);
		trafsize = scanner.FindTag(FOURCC2_traf,moof_offset + moofsize);
		if(trafsize <=0)
		{
			VOLOGR("Traf size <= 0 than return error");
			return MP4RRC_NO_TRAF;
		}
		trafoffset = pTmpReader->Position();
		offset = trafoffset + trafsize;

		tfhdsize = scanner.FindTag(FOURCC2_tfhd,mark + trafsize);
		if(tfhdsize <=0)
			return MP4RRC_NO_TFHD;
		VOLOGR("+FragmentsBox::GetFragmentInfo to init tfhdBox,tfhdsize: %d",tfhdsize);
		tfhdBox.Init(pTmpReader->GetStream(), tfhdsize);
		
		tmpposition = pTmpReader->Position();

		if(m_nTrackPos == -3)
			break;
		else if ( i == m_nTrackPos)
		{
#ifndef CENC
				int size = scanner.FindTag(FOURCC2_saiz,trafoffset + trafsize);
				if(size > 0)
					m_SaizBox.Init(pTmpReader->GetStream(),size);
				pTmpReader->SetPosition(tmpposition);
				size = scanner.FindTag(FOURCC2_saio,trafoffset + trafsize);
				if(size > 0)
					m_SaioBox.Init(pTmpReader->GetStream(),size);
#ifdef USEOLD
			pTmpReader->SetPosition(trafoffset);
			int sencSize = scanner.FindTag(FOURCC2_senc,trafoffset + trafsize);
			if(sencSize > 0)
			{
				VOLOGR("+FragmentsBox::GetFragmentInfo ++++++senc exist,sencSize: %d",sencSize);
				int tmpsenc = sencSize + 8;
				if(m_pSencInfo)
				{
					delete []m_pSencInfo->pSenc;
					delete m_pSencInfo;
					m_pSencInfo = NULL;
				}
				
				m_pSencInfo = new SENC_STRUCT_INFO;
				m_pSencInfo->pSenc = new VO_BYTE[tmpsenc + 1];
				m_pSencInfo->size = tmpsenc;
				memset(m_pSencInfo->pSenc,0,(tmpsenc + 1));
				pTmpReader->Move(-8);
				pTmpReader->Read(m_pSencInfo->pSenc,tmpsenc);
			}
#endif
#endif
			break;
		}
	}
	
	tmpInfo->_sample_description_index = tfhdBox.GetSampleDescriptionIndex();
	tmpInfo->_default_sample_duration = tfhdBox.GetDefaultSampleDuration();
	if(pReader == NULL)
		m_bNoDefaultDuration = tmpInfo->_default_sample_duration == 0? VO_TRUE : VO_FALSE;
	tmpInfo->_default_sample_size = tfhdBox.GetDefaultSampleSize();
	tmpInfo->_default_sample_flags = tfhdBox.GetDefaultSampleFlags();

	pTmpReader->SetPosition(tmpposition);//back to the position before find the senc 
	ABSPOS markTfdt = pTmpReader->Position();
	int tfdtsize = scanner.FindTag(FOURCC2_tfdt, trafoffset + trafsize);
	if(tfdtsize > 0)
	{
		uint8 version;
		pTmpReader->Read(&version);
		pTmpReader->Move(3);
		if(version ==1)
		{
			uint64 dt = 0;
			pTmpReader->Read(&dt);
			if(pReader == NULL)
				m_nBaseMediaDecodetime = (uint64)dt;
		}
		else
		{
			uint32 dt = 0;
			pTmpReader->Read(&dt);
			if(pReader == NULL)
				m_nBaseMediaDecodetime = (uint64)dt;
		}

	}
	pTmpReader->SetPosition(markTfdt);

	int trunsize = scanner.FindTag(FOURCC2_trun,trafoffset + trafsize);
	if(trunsize <=0) return MP4RRC_NO_TRUN;

	VOLOGR("+FragmentsBox::GetFragmentInfo ++++++Trun box init,trunsize: %d",trunsize);
	rc = pTrunBox.Init(pTmpReader->GetStream(), trunsize);
	pTrunBox.SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_DURATION,&tmpInfo->_default_sample_duration);
	pTrunBox.SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_SIZE,&tmpInfo->_default_sample_size);

	VOLOGR("log3");
	if(rc) 
		return MP4RRC_INVALID_TRUN;

	tmpInfo->_sample_count = pTrunBox.GetSampleCount();
	VOLOGR("Samplecount:%d",tmpInfo->_sample_count)

	int uuidsize = scanner.FindTag(FOURCC2_uuid, trafoffset + trafsize);
	if(uuidsize > 0 && bNeedDrmInfo)
	{
		ParserUUID(pTmpReader,tmpInfo,uuidsize);
	}
	VOLOGR("log4");
	
	pTmpReader->SetPosition(moof_offset + moofsize + 8);

	int mdatsize = scanner.FindTag(FOURCC2_mdat,moof_offset + moofsize + 8 + 8);
	if(mdatsize <= 0) 
		return MP4RRC_NO_MDAT;
	tmpInfo->_sample_data_offset = pTmpReader->Position() ;
	VOLOGR("+FragmentsBox::GetFragmentInfo _sample_data_offset : %d",(int32)tmpInfo->_sample_data_offset);
	VOLOGR("-GetFragmentInfo");
	return MP4RRC_OK;
}
VO_U32 FragmentsBox::GenerateIndex(VO_U32 uIndex, Reader* pReader)
{
	VOLOGR("++GenerateIndex++1:%x:%x",pReader,uIndex);
	MIN_FRAGMENT_INFO * pLast = NULL;
	FRAGMENT_INFO tmpInfo;
	TrunBox	tmpTrunBox;

	FRAGMENT_INFO tmpNextFragmetInfo;
	memset(&tmpNextFragmetInfo,0,sizeof(FRAGMENT_INFO));

	VOLOGR("++GenerateIndex++2:%x:%x",pReader,uIndex);
	MIN_FRAGMENT_INFO * pTmp = GetFragmentInfoBySequence(uIndex,0,&pLast);
	VOLOGR("++GenerateIndex++3:%x:%x",pReader,uIndex);
	if (!pTmp)
	{
		pTmp = NewFragmentInfoEntry();
		if (pLast)
		{
			pLast->pNext = pTmp;
		}
		else
		{
			if (uIndex != 0)
			{
				VOLOGE("can not find the last fragment:%d",uIndex);
				return VO_ERR_SOURCE_END;
			}
		}
		pTmp->_sequenceNum = uIndex;
	}
	if(pTmp->_sampleCounts == 0)
	{
		VOLOGR("++GenerateIndex++4:%x:%x",pReader,uIndex);
		if (MP4RRC_OK != GetFragmentInfo(uIndex, tmpInfo, tmpTrunBox,VO_TRUE,pReader))
		{
			VOLOGE("can not GetFragmentInfo:%d",uIndex);
			return VO_ERR_SOURCE_END;
		}
		VOLOGR("++GenerateIndex++5:%x:%x",pReader,uIndex);
		if (uIndex == 0)
		{
			tmpInfo._begin_time = 0;
			tmpInfo._sync_index = 0;
		}
		else
		{
			VOLOGR("++GenerateIndex++7:%x:%x",pReader,uIndex);
			MIN_FRAGMENT_INFO * pTmpLast = GetFragmentInfoBySequence(uIndex - 1,0);
			if (!IsFragmentUsed(pTmpLast))
			{
				VOLOGE("%d fragment information is null",uIndex - 1);
				return VO_ERR_SOURCE_END;
			}
			VOLOGR("++GenerateIndex++8:%x:%x",pReader,uIndex);
			tmpInfo._begin_time = pTmpLast->_beginTime +  pTmpLast->_duration;
			tmpInfo._sync_index = pTmpLast->_syncIndex + pTmpLast->_sampleCounts;
		}
		tmpInfo._sync_next_index = tmpInfo._sync_index + tmpInfo._sample_count;

		pTmp->_sampleCounts =tmpInfo._sample_count;
		pTmp->_duration = GetFragmentDuration(tmpTrunBox);
		pTmp->_beginTime = tmpInfo._begin_time;
		pTmp->_syncIndex = tmpInfo._sync_index;
		pTmp->_moofOffset = tmpInfo._moof_offset;

		tmpNextFragmetInfo.Reset();

		TrunBox tmpTrunBox;
		if(MP4RRC_OK != GetFragmentInfo(uIndex +1, tmpNextFragmetInfo,tmpTrunBox,VO_FALSE,pReader))
			pTmp->_duration = 0x7fffffffffffffffll;
		SetSampleCount(tmpInfo._sync_next_index);
		tmpNextFragmetInfo.Uninit();
		VOLOGR("%d track's %d index is generated ok",m_FilterTrackID, uIndex)
	}
	return VO_ERR_SOURCE_OK;
}

VO_BOOL FragmentsBox::IsIndexReady(VO_U32 uIndex, VO_U32* pMaxIndex)
{
	MIN_FRAGMENT_INFO * pInfo = NULL;
	VO_U32 uMoofIndex = 0;
	do 
	{
		pInfo = GetFragmentInfoBySequence(uMoofIndex++,0,NULL);

	} while (IsFragmentUsed(pInfo) && pInfo->_syncIndex + pInfo->_sampleCounts <= uIndex);
	
	return IsFragmentUsed(pInfo);
}
VO_U32 FragmentsBox::GetMaxTimeByIndex(VO_U32 uIndex,VO_S64& ullTs)
{
#if 1
	MIN_FRAGMENT_INFO * pInfo = NULL;
	pInfo = GetFragmentInfoBySequence(uIndex,0,NULL);
	if (IsFragmentUsed(pInfo))
	{
		ullTs =  pInfo->_beginTime + pInfo->_duration;
		return VO_ERR_SOURCE_OK;
	}
	return VO_ERR_SOURCE_ERRORDATA;
#else
	MIN_FRAGMENT_INFO * pInfo = NULL;
	VO_U32 uMoofIndex = 0;
	do 
	{
		pInfo = GetFragmentInfoBySequence(uMoofIndex++,0,NULL);

	} while (IsFragmentUsed(pInfo) && pInfo->_syncIndex + pInfo->_sampleCounts > uIndex);
	if (IsFragmentUsed(pInfo))
	{
		return pInfo->_beginTime + pInfo->_duration;
	}
	return 0xFFFFFFFFFFFFFFFFLL;
#endif
}

VO_U32 FragmentsBox::GetSampleCount()
{
	voCAutoLock lock(&m_lock);
	return m_uSampleCount;
}

VO_VOID FragmentsBox::SetSampleCount(VO_U32 uSampleCnt)
{
	voCAutoLock lock(&m_lock);
	m_uSampleCount = uSampleCnt;
//	VOLOGE("++++++++++m_uSampleCount==%d+++++++++",m_uSampleCount);
}
int FragmentsBox::FindSampleByIndex(void *pValue,int flag/*= 1 sample index, =2 timestampe*/)
{
	VOLOGR("+ FindSampleByIndex. %lld" ,flag ==1 ?*(int*)pValue: *(uint64*)pValue );

	uint32 entrycount = _tfraBox.GetEntryCount();
	VOLOGR("log2");
	int index = 0;
	uint64 timestampe = 0;
	if(flag ==1)
	{
		index = *(int*)pValue;
		if( index < 0 )
			SetSampleCount(0);

		if(int(_current_fragment_info._sync_index) <= index)
		{
			if( int(_current_fragment_info._sync_index + _current_fragment_info._sample_count) > index )
			{
				VOLOGR("FragmentsBox::FindSampleByIndex return from 1. _sync_index:%d, _sample_count:%d, index:%d",_current_fragment_info._sync_index , _current_fragment_info._sample_count,index);
				return index  - int( _current_fragment_info._sync_index);
			}
		}
		else
		{
			_current_fragment_info.Reset();
			_current_fragment_info._current_entry = -1;
		}
	}
	else if(flag == 2)
	{	
		timestampe = *(uint64*)pValue;
		_current_fragment_info.Reset();
		_current_fragment_info._current_entry = -1;
	}
	VOLOGR("log3");
	int prev_sample_count = -1;
	int tmpCounts = 0;
	uint32 currententry = _current_fragment_info._current_entry +1;

	if(entrycount && currententry == entrycount && index > _current_fragment_info._sync_next_index) 
	{
		VOLOGI("FragmentsBox::FindSampleByIndex return from 2.currententry:%d,entrycount:%d,_sync_next_index:%d, index:%d",currententry,entrycount,_current_fragment_info._sync_next_index,index);
		return -1;
	}
	VOLOGR("Log3,currententry:%d,entrycount:%d",currententry,entrycount);

	while (1)
	{
		MIN_FRAGMENT_INFO * pTmp = GetFragmentInfoBySequence(currententry,entrycount);
		if (!pTmp)
		{
			break;
		}

		if(pTmp->_sampleCounts == 0)
			break;
		if( flag ==1)
		{	
			if(int(pTmp->_syncIndex) <= index && int(pTmp->_syncIndex + pTmp->_sampleCounts) > index)
				break;
		}
		else if(flag == 2)
		{
			if(pTmp->_beginTime <= timestampe && pTmp->_beginTime + pTmp->_duration > timestampe)
				break;
		}
		currententry++;
	}
	prev_sample_count = currententry >= 1 ? GetFragmentInfoBySequence(currententry -1,entrycount)->_syncIndex : 0;
	tmpCounts = currententry >= 1 ? (GetFragmentInfoBySequence(currententry -1,entrycount)->_syncIndex + GetFragmentInfoBySequence(currententry -1,entrycount)->_sampleCounts ) : 0;

	if(currententry == 0) 
	{
		prev_sample_count = -1;
		tmpCounts = 0;
	}
	VOLOGR("Log4,currententry:%d,entrycount:%d",currententry,entrycount);
	uint32 i = currententry;
	FRAGMENT_INFO tmpNextFragmetInfo;
	memset(&tmpNextFragmetInfo,0,sizeof(FRAGMENT_INFO));

	while(1)
	{
		if (entrycount && i >= entrycount)
		{
			break;
		}
		if (MP4RRC_OK != GetFragmentInfo(i, _current_fragment_info, _trunBox,VO_TRUE))
		{
			break;
		}
		if (!entrycount)
		{
			if (i == 0)
			{
				_current_fragment_info._begin_time = 0;
			}
			else
			{
				MIN_FRAGMENT_INFO * pTmp = GetFragmentInfoBySequence(i - 1,entrycount);
				if (!IsFragmentUsed(pTmp))
				{
					break; 
				}
				_current_fragment_info._begin_time = pTmp->_beginTime +  pTmp->_duration;
			}
		}
	
		_current_fragment_info._sync_prev_index = prev_sample_count;
		prev_sample_count = tmpCounts;
		_current_fragment_info._sync_index = tmpCounts;
		tmpCounts += _current_fragment_info._sample_count;
		_current_fragment_info._sync_next_index = tmpCounts;
		_current_fragment_info._current_index = 0;
		_current_fragment_info._current_entry = i;


		MIN_FRAGMENT_INFO * pLast = NULL;
		MIN_FRAGMENT_INFO * pTmp = GetFragmentInfoBySequence(i,entrycount,&pLast);
		if (!pTmp)
		{
			pTmp = NewFragmentInfoEntry();
			if (pLast)
			{
				pLast->pNext = pTmp;
			}
			else
			{
				VOLOGE("can not find the last fragment:%d",i);
			}
			
			pTmp->_sequenceNum = i;
		}
		if(pTmp->_sampleCounts  == 0)
		{
			pTmp->_sampleCounts =_current_fragment_info._sample_count;
			pTmp->_duration = GetFragmentDuration(_trunBox);
			pTmp->_beginTime = _current_fragment_info._begin_time;
			pTmp->_syncIndex = _current_fragment_info._sync_index;
			pTmp->_moofOffset = _current_fragment_info._moof_offset;

			tmpNextFragmetInfo.Reset();

			TrunBox tmpTrunBox;
			if(MP4RRC_OK != GetFragmentInfo(i +1, tmpNextFragmetInfo,tmpTrunBox,VO_FALSE))
				pTmp->_duration = 0x7fffffffffffffffll;
			SetSampleCount(_current_fragment_info._sync_next_index + tmpNextFragmetInfo._sample_count);
		}
		VOLOGR("FragmentsBox::FindSampleByIndex FragmentID:%d, _Duration:%llu,BeginTime:%llu, index:%d, Timestamp:%llu, SyncIndex:%d, PreSyncIndex:%d,SampleCounts:%d",i, pTmp->_duration,  pTmp->_beginTime, index, timestampe,pTmp->_syncIndex,_current_fragment_info._sync_prev_index,m_uSampleCount  );
		if(flag == 1)
		{	
			if(int(pTmp->_syncIndex) <= index && int(pTmp->_syncIndex + pTmp->_sampleCounts) > index)
			{
				VOLOGR("FragmentsBox::FindSampleByIndex ++++++++++++++++get the exact sample by index!");
				tmpNextFragmetInfo.Uninit();
				return index - int(_current_fragment_info._sync_index);
			}
		}
		else if (flag == 2)
		{
			if(pTmp->_beginTime <= timestampe && pTmp->_beginTime + pTmp->_duration > timestampe)
			{
				VOLOGR("FragmentsBox::FindSampleByIndex ++++++++++++++++get the exact sample by index!");
				tmpNextFragmetInfo.Uninit();
				return pTmp->_syncIndex;
			}
		}
	
		i++;
	}
	tmpNextFragmetInfo.Uninit();
	VOLOGR("- FindSampleByIndex. ");
	return 0;
}

VO_U64 FragmentsBox::SeekToMoofByTime(VO_U64 ullTime)
{
	if (0 == _tfraBox.GetEntryCount())
	{
		return 0;
	}

	VO_U64 ullTs = 0;
	_tfraBox.SetSeekingFragmentInfoByTime(ullTime);
	m_uBaseMoofIndex = _tfraBox.GetBaseMoofIndex();

	ullTs = _tfraBox.GetCurFragmentBeginTime();
	return ullTs;

}

MP4RRC FragmentsBox::GetFragmentInfo()
{
	uint32 entrycount = _tfraBox.GetEntryCount();
	_pMin_Fragment_info = new MIN_FRAGMENT_INFO[entrycount];
	if(!_pMin_Fragment_info)
		return MP4RRC_READ_FAILED;
	memset(_pMin_Fragment_info, 0, sizeof(MIN_FRAGMENT_INFO) * entrycount);
	return MP4RRC_OK;
}

MP4RRC FragmentsBox::CreateFragmentInfo()
{
	_pMin_Fragment_info = NewFragmentInfoEntry();
	if(!_pMin_Fragment_info)
		return MP4RRC_READ_FAILED;
	memset(_pMin_Fragment_info, 0, sizeof(MIN_FRAGMENT_INFO));
	return MP4RRC_OK;
}
uint32 FragmentsBox::GetSampleSize(int index)
{
	voCAutoLock lock(&m_lock);
	int sindex = FindSampleByIndex(&index);
	if(sindex < 0) return 0;
	VO_U32 uSampleSize = _trunBox.GetSampleSize(sindex);
	return  uSampleSize ? uSampleSize : _current_fragment_info._default_sample_size;
}
uint64 FragmentsBox::GetSampleTime(int index)
{
	voCAutoLock lock(&m_lock);
	uint64 timex = 0;

	uint32 sindex = FindSampleByIndex(&index);
	_current_fragment_info._default_sample_duration ? timex =    _current_fragment_info._default_sample_duration * sindex  :  timex = _trunBox.GetSampleTime(sindex);
	timex += _current_fragment_info._begin_time;
	return timex;
}


MP4RRC FragmentsBox::SetParam(VO_U32 ID, VO_PTR pParam)
{
	switch(ID)
	{
	case VO_PID_FRAGMENT_DEFAULT_SAMPLE_SIZE:
		{
			_trunBox.SetParam(ID,pParam);
			break;
		}
	case VO_PID_FRAGMENT_DEFAULT_SAMPLE_DURATION:
		{
			_trunBox.SetParam(ID,pParam);
			if(*((uint32*)pParam) > 0)
				m_bNoDefaultDuration = VO_FALSE;
			break;
		}
	default:
		return MP4RRC_MAX;
	}
	return MP4RRC_OK;
}
uint64 FragmentsBox::GetSampleAddress(int index)
{

	uint64 addr = 0;
	uint32 sindex = FindSampleByIndex(&index);
	addr = _trunBox.GetSampleAddress(sindex);
	addr += _current_fragment_info._sample_data_offset;
	return addr;
}
MP4RRC FragmentsBox::Init(Reader* r,int trackIndex)
{
	VOLOGI("+ Init");
	MP4RRC rc;
	r->SetPosition(0);	
	Scanner scanner(r);
	m_pReader = r;
	int index = FIRST_LOAD_INDEX;

	if (!(m_uEnableBox & 0x00000001))
	{
		m_FilterTrackID = trackIndex;
		CreateFragmentInfo();
		FindSampleByIndex( &index);	
	}
	else
	{
#ifndef USE_OLDER
		r->SetPosition(0);
		ABSPOS markpos1 = r->Position();
		Scanner scanner1(r);
		int moofsize = scanner1.FindTag(FOURCC2_moof);
		if(moofsize <= 0)
			return MP4RRC_NO_MOOF;
		ABSPOS markpos2 = r->Position();
		r->SetPosition(0);
		int mfraSize = scanner1.FindTag(FOURCC2_mfra,markpos2);
		if(mfraSize <= 0)
		{
			if(false == m_pReader->SeekToEnd())
			{
				VOLOGI("Mp4Parser:m_pReader->SeekToEnd() failed");
				return MP4RRC_READ_FAILED;
			}
			if(false == r->Move(-16 +4))
			{
				VOLOGI("Mp4Parser:r->Move(-16 +4) failed");
				return MP4RRC_READ_FAILED;
			}
			uint32 tag = 0;
			r->Read(&tag);

			if(tag == (FOURCC2_mfro))
			{
				r->Move(4);
				uint32 mfraoffset = 0;
				r->Read(&mfraoffset);
				m_pReader->SeekToEnd();
				int32 ddd =  -(int (mfraoffset)) ;
				r->Move( ddd);
			}
			mfraSize = scanner1.FindTag(FOURCC2_mfra);
			if(0 >= mfraSize)
			{
				m_FilterTrackID = trackIndex;
				CreateFragmentInfo();
				FindSampleByIndex( &index);
			}
		}
#else
		if(false == m_pReader->SeekToEnd())
		{
			VOLOGR("Mp4Parser:m_pReader->SeekToEnd() failed");
			return MP4RRC_READ_FAILED;
		}
		if(false == r->Move(-16 +4))
		{
			VOLOGR("Mp4Parser:r->Move(-16 +4) failed");
			return MP4RRC_READ_FAILED;
		}
		uint32 tag = 0;
		r->Read(&tag);

		if(tag == (FOURCC2_mfro))
		{
			r->Move(4);
			uint32 mfraoffset = 0;
			r->Read(&mfraoffset);
			m_pReader->SeekToEnd();
			int32 ddd =  -(int (mfraoffset)) ;
			r->Move( ddd);
		}
		else
			r->SetPosition(0);
		Scanner scanner1(r);
		int mfraSize = scanner1.FindTag(FOURCC2_mfra);
#endif
		if(mfraSize > 0 )
		{
			VOLOGI("Has MFRA.");
			while(!(int(_tfraBox.GetTrackID() )== trackIndex ))
			{
				int tfraSize = scanner.FindTag(FOURCC2_tfra,mfraSize);
				ABSPOS mark = r->Position();
				if(tfraSize < 0)
				{
					VOLOGI("Mp4Parser:No Tfra Box");
					return MP4RRC_NO_TFRA;
				}
				rc = _tfraBox.Init(r->GetStream(), tfraSize);
				if(rc)
				{
					VOLOGI("Mp4Parser:Invalid Tfra Box");
					return MP4RRC_INVALID_TFRA;
				}
				if(!r->SetPosition(mark))
				{
					VOLOGI("Mp4Parser:SetPosition(mark) failed");
					return MP4RRC_READ_FAILED;
				}
				if (!r->Move(tfraSize))
				{
					VOLOGR("Mp4Parser:Move(tfraSize) failed");
					return MP4RRC_READ_FAILED;
				}
			}
			rc = GetFragmentInfo();
			if(rc != MP4RRC_OK)
			{
				VOLOGI("Mp4Parser:GetFragmentInfo failed");
				return rc;
			}
			
			FindSampleByIndex( &index);
		}
#ifdef USEMFRA
		else
		{
			VOLOGI("Mp4Parser:No Mfra Box");
			return MP4RRC_NO_MFRA;
		}
#endif
	}
	VOLOGI("- Init");
	return MP4RRC_OK;
}
int FragmentsBox::GetSampleSync(int index) //default is sync
{
	uint32 sindex  = FindSampleByIndex(  &index);
	return sindex ? 0 : 1;
}

int FragmentsBox::GetNextSyncPoint(int index)//default is sync
{
	int ret = FindSampleByIndex(  &index);
	VOLOGR("Samplecount:%d, SyncIndex:%d,SyncPreIndex:%d",_current_fragment_info._sample_count,_current_fragment_info._sync_index,_current_fragment_info._sync_prev_index);

	if(ret == -1 || _current_fragment_info._sync_next_index == 0) 
		return -1;

	return  _current_fragment_info._sync_next_index;
}
int FragmentsBox::GetPrevSyncPoint(int index)//default is sync
{
	FindSampleByIndex( & index);
	VOLOGR("Samplecount:%d, SyncIndex:%d,SyncPreIndex:%d",_current_fragment_info._sample_count,_current_fragment_info._sync_index,_current_fragment_info._sync_prev_index);
	return _current_fragment_info._sync_prev_index;
}
uint32 FragmentsBox::GetSampleIndex(uint64 time)
{
	FindSampleByIndex(& time, 2);	
	VOLOGR("Samplecount:%d, SyncIndex:%d",_current_fragment_info._sample_count,_current_fragment_info._sync_index);

	for(uint32 n = 0; n< _current_fragment_info._sample_count; n++)
	{
		uint64 t = GetSampleTime(n + _current_fragment_info._sync_index);
		VOLOGR("t:%llu, time:%llu, Index:%d",t,time,n+ _current_fragment_info._sync_index );

		if(t > time) return n+ _current_fragment_info._sync_index;
	}
	return _current_fragment_info._sample_count + _current_fragment_info._sync_index;
}

VO_U32 FragmentsBox::ResetIndex()
{
	_current_index = 0;
	_current_fragment_info.Uninit();
	memset(&_current_fragment_info, 0 , sizeof(FRAGMENT_INFO));
	_current_fragment_info._current_entry = -1 ;
	uint32 entrycount = _tfraBox.GetEntryCount();
	memset(_pMin_Fragment_info, 0, sizeof(MIN_FRAGMENT_INFO) * entrycount);
	return MP4RRC_OK;
}
MP4RRC FragmentsBox::ParserUUID(Reader* r, FRAGMENT_INFO *pInfo, int bodySize)
{
	if (!pInfo)
	{
		return MP4RRC_OK;
	}
	if (!pInfo->EncyptBox.pBoxData || pInfo->EncyptBox.uBoxMaxSize < bodySize + 8)
	{
		if (pInfo->EncyptBox.pBoxData)
		{
			delete []pInfo->EncyptBox.pBoxData;
		}
		pInfo->EncyptBox.uBoxMaxSize = bodySize + 8;
		pInfo->EncyptBox.pBoxData = new VO_BYTE[pInfo->EncyptBox.uBoxMaxSize];
	}
	pInfo->EncyptBox.uBoxCurSize = bodySize + 8;
	r->Move(-8);
	r->Read(pInfo->EncyptBox.pBoxData,pInfo->EncyptBox.uBoxCurSize);
	return MP4RRC_OK;
}

MP4RRC FragmentsBox::FindMoofInfoByIndex(VO_U64 uIndex,VO_U64* pPos,VO_U64 *pTs, Reader *pReader)
{
	voCAutoLock lock(&m_lock);
	
	Reader* pTmpReader = (pReader == NULL ? m_pReader : pReader);
	VOLOGR("++++FindMoofInfoByIndex++++:%x,%x",pTmpReader,uIndex);
	if (_tfraBox.GetEntryCount())
	{
		MP4RRC ret = MP4RRC_OK;
		if(MP4RRC_OK ==(ret = _tfraBox.SetFragmentInfoByIndex(uIndex)))
		{
			if (pPos)
			{		
				if(VO_MAXU64 == (*pPos = _tfraBox.GetCruFragmentOffset()))
					return MP4RRC_NO_MOOF;		
			}
			if (pTs)
			{
				*pTs = _tfraBox.GetCurFragmentBeginTime();
			}
		}
		return ret;
	}
	VOLOGR("FindMoofInfoByIndex11:%x,%x",pTmpReader,uIndex);
	MIN_FRAGMENT_INFO *pTmp = _pMin_Fragment_info;
	MIN_FRAGMENT_INFO *pLast =  NULL;
	while(pTmp)
	{
		if (pTmp->_sequenceNum == uIndex)
		{
			break;
		}
		pLast = pTmp;
		pTmp = pTmp->pNext;
	}
	VOLOGR("FindMoofInfoByIndex12:%x,%x",pTmpReader,uIndex);
	if (IsFragmentUsed(pTmp))
	{
		if (pPos)
		{
			*pPos = pTmp->_moofOffset;
		}
		if (pTs)
		{
			*pTs = pTmp->_beginTime;
		}
		VOLOGR("----FindMoofInfoByIndex----:%x,%x",pTmpReader,uIndex);
		return MP4RRC_OK;
	}
	VOLOGR("FindMoofInfoByIndex13:%x,%x",pTmpReader,uIndex);
	VO_S32 uTmpSequence = -1;
	VO_U64 ullTmpPos = 0;

	if (pLast)
	{
		uTmpSequence = pLast->_sequenceNum;
		ullTmpPos = pLast->_moofOffset;
	}
	
	Scanner scanner(pTmpReader);
	pTmpReader->SetPosition(ullTmpPos);

	VOLOGR("FindMoofInfoByIndex14:%x,%x",pTmpReader,uIndex);
	VO_U64 ullEndPos = 0;
	VO_U64 ullBeingPos = 0;
	VO_U64 ullMarkPos = 0;
	VO_U32 uBoxSize = 0;
	VO_U32 uMoofBeginPos = 0;
	while(1)
	{
		while(1)
		{
			VOLOGR("FindMoofInfoByIndex15:%x,%x",pTmpReader,uIndex);
			uint32 uBoxSize = scanner.FindTag(FOURCC2_moof);
			if (!uBoxSize)
			{
				return MP4RRC_NO_MOOF;
			}
			uMoofBeginPos = pTmpReader->Position() - 8;
			ullEndPos = pTmpReader->Position() + uBoxSize;
			ullMarkPos = ullEndPos;

			uBoxSize = scanner.FindTag(FOURCC2_traf,ullEndPos);
			if(uBoxSize <=0)
				return MP4RRC_NO_TRAF;

			ullEndPos = pTmpReader->Position() + uBoxSize;

			uBoxSize = scanner.FindTag(FOURCC2_tfhd,ullEndPos);

			if(uBoxSize <=0)
				return MP4RRC_NO_TFHD;

			pTmpReader->Move(4);
			VO_U32 uTrackID = 0;
			pTmpReader->Read(&uTrackID);

			pTmpReader->SetPosition(ullMarkPos);
			if (uTrackID == m_FilterTrackID)
			{
				break;
			}
			
		}
		if(uTmpSequence == -1 || uTmpSequence++ == uIndex)
		{
			if (pPos)
			{
				*pPos = uMoofBeginPos;
			}
			if (pTs)
			{
				*pTs = 0;
			}
			VOLOGR("----FindMoofInfoByIndex----:%x,%x",pTmpReader,uIndex);
			return MP4RRC_OK;  
		}
	}
	return MP4RRC_READ_FAILED; 
}

VO_U64 FragmentsBox::GetFragmentDuration(TrunBox& trunBox)
{
	voCAutoLock lock(&m_lock);
	VO_U64 ullDuration = 0;
	for (VO_U32 nCnt = 0;nCnt < trunBox.GetSampleCount();nCnt++)
	{
		ullDuration += trunBox.GetSampleDuration(nCnt);
	}
	return ullDuration;
}

MIN_FRAGMENT_INFO * FragmentsBox::GetFragmentInfoBySequence(VO_U32 uSequence,VO_U32 uEntryCnt,MIN_FRAGMENT_INFO ** pLast)
{
	voCAutoLock lock(&m_lock);
	if (uEntryCnt)
	{
		if (pLast)
		{
			*pLast = _pMin_Fragment_info + uEntryCnt - 1; 
		}
		if (uSequence < uEntryCnt)
		{
			return _pMin_Fragment_info + uSequence;
		}
		return NULL;
	}
	MIN_FRAGMENT_INFO * pTmp = _pMin_Fragment_info;
	while(pTmp)
	{
		if (pTmp->_sequenceNum == uSequence)
		{
			break;
		}
		if (pLast)
		{
			*pLast = pTmp; 
		}
		pTmp = pTmp->pNext;
	}
	return pTmp;
}

VO_BOOL FragmentsBox::IsFragmentUsed(MIN_FRAGMENT_INFO * pFragmentInfo)
{
	voCAutoLock lock(&m_lock);
	if (pFragmentInfo && pFragmentInfo->_sampleCounts)
	{
		return VO_TRUE;
	}
	else
		return VO_FALSE;
}

VO_VOID FragmentsBox::SetBoxType(VO_U32 uBoxType)
{
	m_uEnableBox = uBoxType;
}

MIN_FRAGMENT_INFO* FragmentsBox::NewFragmentInfoEntry()
{
	voCAutoLock lock(&m_lock);
	MIN_FRAGMENT_INFO* pEntry = new MIN_FRAGMENT_INFO;
	memset(pEntry,0,sizeof(MIN_FRAGMENT_INFO));
	return pEntry;
}

VO_VOID FragmentsBox::DelFragmentInfoEntry(MIN_FRAGMENT_INFO* pEntry)
{
	if (pEntry)
	{
		if (pEntry->pNext)
		{
			DelFragmentInfoEntry(pEntry->pNext);
		}
		delete pEntry;
	}
}
SidxBox::SidxBox()
:_timeScale(0)
,_duration(0)
{
	memset(&m_SidxStructure,0,sizeof(VO_SegmentIndexStructure));
}
SidxBox::~SidxBox()
{
	if(m_SidxStructure.ref_count > 0)
	{
		delete []m_SidxStructure.pSidxrefs;
		m_SidxStructure.pSidxrefs = 0;
	}
}
VO_SegmentIndexStructure *SidxBox::InitRefsidx(Reader* r,ABSPOS sidxoffset,uint32 bodySize)
{
	r->SetPosition(sidxoffset);
	Scanner scanner(r);
	VO_U32 sidxsize = scanner.FindTag(FOURCC2_sidx);
	if(0 >= sidxsize)
		return NULL;
	ABSPOS sidxpos0 = r->Position();
	VO_SegmentIndexStructure *pSgIdx = Init2(r->GetStream(),sidxpos0,sidxsize);	
	return pSgIdx;
}
VO_VOID SidxBox::GenerateFinalSidx(VO_SegmentIndexStructure *pSdx,uint32 num_offset)
{
	m_SidxStructure.timescale = pSdx->timescale;
	for(int i = 0; i < pSdx->ref_count; i++)
	{
		m_SidxStructure.pSidxrefs[num_offset + i].breference_type = pSdx->pSidxrefs[i].breference_type;
		m_SidxStructure.pSidxrefs[num_offset + i].bstarts_with_SAP = pSdx->pSidxrefs[i].bstarts_with_SAP;
		m_SidxStructure.pSidxrefs[num_offset + i].nreference_data_offset = pSdx->pSidxrefs[i].nreference_data_offset;
		m_SidxStructure.pSidxrefs[num_offset + i].nreference_size = pSdx->pSidxrefs[i].nreference_size;
		m_SidxStructure.pSidxrefs[num_offset + i].nSAP_delta_time = pSdx->pSidxrefs[i].nSAP_delta_time;
		m_SidxStructure.pSidxrefs[num_offset + i].nsubsegment_duration = pSdx->pSidxrefs[i].nsubsegment_duration;
	}
}
VO_VOID SidxBox::FreeSidxStructure(VO_SegmentIndexStructure *pSdx)
{
	if(pSdx)
	{
		delete []pSdx->pSidxrefs;
		pSdx->pSidxrefs = NULL;
		delete pSdx;
		pSdx = NULL;
	}
}

MP4RRC SidxBox::InitSidx2(Reader* r, uint32 bodySize)
{
	MP4RRC rc = MP4RRC_OK;
	Scanner scanner(r);
	r->SetPosition(0);
	VO_U32 sidxsize = scanner.FindTag(FOURCC2_sidx);
	if(0 >= sidxsize)
	{
		VOLOGI("SidxBox::InitSidx2,not found the sidx box");
		return MP4RRC_NO_SIDX;
	}
	ABSPOS sidxpos0 = r->Position();
	VO_SegmentIndexStructure *pSgIdx = Init2(r->GetStream(),sidxpos0,sidxsize);
	if(ENUM_NO_REF_SIDX_TYPE == pSgIdx->sidx_type)
	{
		VO_U32 num_offset = 0;
		m_SidxStructure.ref_count = pSgIdx->ref_count;
		m_SidxStructure.pSidxrefs = new VO_SIDXReference[pSgIdx->ref_count];
		memset(m_SidxStructure.pSidxrefs,0,sizeof(VO_SIDXReference)*pSgIdx->ref_count);
		GenerateFinalSidx(pSgIdx,num_offset);
		FreeSidxStructure(pSgIdx);
	}
	else if(ENUM_TOTAL_REF_SIDX_TYPE == pSgIdx->sidx_type)
	{
		const int maxnumber = 15;
		SidxBox sidxbox[maxnumber];
		VO_SegmentIndexStructure *pRefSgIdx[maxnumber];
		memset(pRefSgIdx,0,sizeof(VO_SegmentIndexStructure *)*maxnumber);
		for(int ref_count = 0; ref_count < pSgIdx->ref_count; ref_count++)
		{
			pRefSgIdx[ref_count] = sidxbox[ref_count].InitRefsidx(r,pSgIdx->pSidxrefs[ref_count].nreference_data_offset,pSgIdx->pSidxrefs[ref_count].nreference_size);
			m_SidxStructure.ref_count += pRefSgIdx[ref_count]->ref_count;
		}
		m_SidxStructure.pSidxrefs = new VO_SIDXReference[m_SidxStructure.ref_count];
		VO_U32 num_offset = 0;
		for(int ref_count = 0; ref_count < pSgIdx->ref_count; ref_count++)
		{
			GenerateFinalSidx(pRefSgIdx[ref_count],num_offset);
			num_offset += pRefSgIdx[ref_count]->ref_count;
			FreeSidxStructure(pRefSgIdx[ref_count]);
		}

	}
	return rc;
}

VO_SegmentIndexStructure *SidxBox::Init2(Stream* stream, ABSPOS baseoffset,uint32 bodySize)
{
	VO_U32 rc = BufferInBox::Init(stream, bodySize);
	if(VO_ERR_SOURCE_OK != rc)
		return NULL;
	Reader *r = GetReader();
	uint8 version = 0;
	r->Read(&version);
	r->Move(3);
	VO_SegmentIndexStructure *pSgIdx = new VO_SegmentIndexStructure;
	memset(pSgIdx,0,sizeof(VO_SegmentIndexStructure));
	r->Read(&pSgIdx->reference_ID);
	r->Read(&pSgIdx->timescale);
	if(0 == version)
	{
		r->ReadBits(&pSgIdx->earliest_presentation_time,32);
		r->ReadBits(&pSgIdx->first_offset,32);
	}
	else//version == 1
	{
		r->Read(&pSgIdx->earliest_presentation_time);
		r->Read(&pSgIdx->first_offset);
	}
	r->Move(2);
	r->Read(&pSgIdx->ref_count);
	if(0 >= pSgIdx->ref_count)
		return NULL;
	pSgIdx->pSidxrefs = new VO_SIDXReference[pSgIdx->ref_count];
	memset(pSgIdx->pSidxrefs,0,sizeof(VO_SIDXReference)*pSgIdx->ref_count);
	VO_U16 uReferenceTypeNum = 0;
	VO_U64 uoffset = baseoffset + bodySize + pSgIdx->first_offset;
	for(int ref_count = 0; ref_count < pSgIdx->ref_count;ref_count++)
	{
		 r->ReadBits(&pSgIdx->pSidxrefs[ref_count].breference_type,1);
		 if(1 == pSgIdx->pSidxrefs[ref_count].breference_type)
			uReferenceTypeNum += 1;
		 r->ReadBits(&pSgIdx->pSidxrefs[ref_count].nreference_size,31);
		 r->Read(&pSgIdx->pSidxrefs[ref_count].nsubsegment_duration);
		 r->ReadBits(&pSgIdx->pSidxrefs[ref_count].bstarts_with_SAP,1);
		 r->ReadBits(&pSgIdx->pSidxrefs[ref_count].nSAP_type,3);
		 r->ReadBits(&pSgIdx->pSidxrefs[ref_count].nSAP_delta_time,28);
		 pSgIdx->pSidxrefs[ref_count].nreference_data_offset = uoffset;
		 uoffset += pSgIdx->pSidxrefs[ref_count].nreference_size;
	}
	if(0 == uReferenceTypeNum)
		pSgIdx->sidx_type = ENUM_NO_REF_SIDX_TYPE;
	else if((0 < uReferenceTypeNum) &&(uReferenceTypeNum < pSgIdx->ref_count))
		pSgIdx->sidx_type = ENUM_PART_REF_SIDX_TYPE;
	else
		pSgIdx->sidx_type = ENUM_TOTAL_REF_SIDX_TYPE;
	return pSgIdx;
}
MP4RRC SidxBox::InitSidx(Reader* r, uint32 bodySize)
{
	Scanner scanner(r);
	uint32 sidxsize = scanner.FindTag(FOURCC2_sidx);
	if(0 >= sidxsize)
	{
		VOLOGI("SidxBox::InitSidx,failed to found the sidx box so return MP4RRC_NO_SIDX error");
		return MP4RRC_NO_SIDX;
	}
	return Init(r->GetStream(),sidxsize);
}
MP4RRC SidxBox::Init(Stream* stream, uint32 bodySize)
{
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (MP4RRC_OK != rc)
	{
		VOLOGI("SidxBox::InitSidx,failed to init BufferInbox than return  MP4RRC_READ_FAILED error");
		return rc;
	}
	Reader *r = GetReader();
	uint8 version = 0;
	r->Read(&version);
	r->Move(3);
	
	r->Read(&m_SidxStructure.reference_ID);
	r->Read(&m_SidxStructure.timescale);
	if(0 == version)
	{
		r->ReadBits(&m_SidxStructure.earliest_presentation_time,32);
		r->ReadBits(&m_SidxStructure.first_offset,32);
	}
	else//version == 1
	{
		r->Read(&m_SidxStructure.earliest_presentation_time);
		r->Read(&m_SidxStructure.first_offset);
	}
	r->Move(2);
	r->Read(&m_SidxStructure.ref_count);
	if(0 >= m_SidxStructure.ref_count)
		return MP4RRC_NO_SUBSIDX;
	m_SidxStructure.pSidxrefs = new VO_SIDXReference[m_SidxStructure.ref_count];
	memset(m_SidxStructure.pSidxrefs,0,sizeof(VO_SIDXReference)*m_SidxStructure.ref_count);
	for(int ref_count = 0; ref_count < m_SidxStructure.ref_count;ref_count++)
	{
		 r->ReadBits(&m_SidxStructure.pSidxrefs[ref_count].breference_type,1);
		 r->ReadBits(&m_SidxStructure.pSidxrefs[ref_count].nreference_size,31);
		 r->Read(&m_SidxStructure.pSidxrefs[ref_count].nsubsegment_duration);
		 r->ReadBits(&m_SidxStructure.pSidxrefs[ref_count].bstarts_with_SAP,1);
		 r->ReadBits(&m_SidxStructure.pSidxrefs[ref_count].nSAP_type,3);
		 r->ReadBits(&m_SidxStructure.pSidxrefs[ref_count].nSAP_delta_time,28);
	}

	_data_pos = GetReader()->Position();

	return MP4RRC_OK;
}
SaioBox::SaioBox():m_aux_info_type(0),
				   m_aux_info_type_parameter(0),
				   m_enrycount(0),
				   m_pOffsetArr(NULL)
{

}
SaioBox::~SaioBox()
{
	if(m_enrycount > 0)
	{
		delete []m_pOffsetArr;
		m_pOffsetArr = NULL;
	}
}
MP4RRC SaioBox::Init(Stream* stream, uint32 bodySize)
{
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if(rc)
		return rc;
	Reader *r = GetReader();
	uint8 version = 0;
	r->Read(&version);
    uint32 flag = 0;
	r->ReadBits(&flag,24);
	if(flag)
	{
		r->Read(&m_aux_info_type);
		r->Read(&m_aux_info_type_parameter);
	}
	r->Read(&m_enrycount);
	if(0 >= m_enrycount)
		return rc;
	m_pOffsetArr = new uint64[m_enrycount];
	if(0 == version)
	{
		for(int index = 0; index < m_enrycount; index++)
			r->ReadBits(&m_pOffsetArr[index],32);
	}
	else
	{
		for(int index = 0; index < m_enrycount; index++)
			r->Read(&m_pOffsetArr[index]);
	}
}
SaizBox::SaizBox() :m_aux_info_type(0),
				  m_aux_info_type_parameter(0),
				  m_default_sample_info_size(0),
				  m_sample_count(0),
				  m_pSamplesizeArr(NULL)
{

}
SaizBox::~SaizBox()
{
	if(m_sample_count)
	{
		delete []m_pSamplesizeArr;
		m_pSamplesizeArr = NULL;

	}
}
MP4RRC SaizBox::Init(Stream* stream, uint32 bodySize)
{
	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if(rc)
		return rc;
	Reader *r = GetReader();
	r->Move(1);//version
    uint32 flag = 0;
	r->ReadBits(&flag,24);
	if(flag)
	{
		r->Read(&m_aux_info_type);
		r->Read(&m_aux_info_type_parameter);
	}
	r->Read(&m_default_sample_info_size);
	r->Read(&m_sample_count);
	if(0 < m_sample_count)
	{
		m_pSamplesizeArr = new uint8[m_sample_count];
		if(0 == m_default_sample_info_size)
		{
			for(int index = 0; index < m_sample_count;index++)
				r->Read(&m_pSamplesizeArr[index]);
		}
		else
		{
			for(int index = 0; index < m_sample_count;index++)
				m_pSamplesizeArr[index] = m_default_sample_info_size;
			
		}
	}
}
uint32 SaizBox::GetOffsetByIndex(uint32 index)
{
	if(0 == index)
		return 0 ;
	if(index > m_sample_count)
		return -1;
	uint32 offset = 0;
	for(int i = 0; i < index; i++)
	{
		offset += m_pSamplesizeArr[i];
	}
	return offset;
}
TfhdBox::TfhdBox()
{
	m_nTrackID = -1;
	
	m_nSampleDescriptionIndex = 0;
	m_nDefaultSampleDuration = 0;
	m_nDefaultSampleSize = 0;
	m_nDefaultSampleFlags = 0;
}
MP4RRC TfhdBox::Init(Stream* stream, uint32 bodySize)
{
	VOLOGR("+TfhdBox::Init");
	m_nTrackID = -1;
	m_nSampleDescriptionIndex = 0;
	m_nDefaultSampleDuration = 0;
	m_nDefaultSampleSize = 0;
	m_nDefaultSampleFlags = 0;

	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
		return rc;

	uint32 vf;//version ,flags
	GetReader()->Read(&vf);
	uint32 tf_flags = vf & 0x3fffff;
	GetReader()->Read(&m_nTrackID);
	if(tf_flags & 0x01 ) GetReader()->Move(8);//base_data_offset
	if(tf_flags & 0x02 )
		GetReader()->Read(& m_nSampleDescriptionIndex);//sample_description_index

	if(tf_flags & 0x08 )
		GetReader()->Read(& m_nDefaultSampleDuration);
	else
		m_bNoDefaultDuration = VO_TRUE;

	if(tf_flags & 0x10 ) GetReader()->Read(& m_nDefaultSampleSize);
	if(tf_flags & 0x20 ) GetReader()->Read(& m_nDefaultSampleFlags);
	_data_pos = GetReader()->Position();
	VOLOGR("-TfhdBox::Init.TrackID:%d", m_nTrackID);
	return MP4RRC_OK;
}

MP4RRC TrunBox::Init(Stream* stream, uint32 bodySize)
{
	VOLOGR("+TrunBox::Init");
	m_bNoSampleDuration = VO_FALSE;
	m_nDefaultDuration = 0;
	m_nDefaultSize = 0;
	memset(&_present_sample_info, 0x00, sizeof(_present_sample_info));

	MP4RRC rc = BufferInBox::Init(stream, bodySize);
	if (rc)
		return rc;
	uint32 vf = 0;
	//Reader *r = GetReader();
	GetReader()->Read(&vf); // version, flag
	GetReader()->Read(&_sample_count);

	uint32 data_offset;
	uint32 first_sample_flags;
	_tr_flags = vf & 0x3fffff;
	if(_tr_flags & 0x01 ) 
		GetReader()->Read(&data_offset);
// 		GetReader()->Move(4);//data_offset
	if(_tr_flags & 0x04 )
		GetReader()->Read(&first_sample_flags);
	if(!(_tr_flags & 0x100))
		m_bNoSampleDuration = VO_TRUE;
	_data_pos = GetReader()->Position();
	VOLOGR("-TrunBox::Init.%d",_sample_count);
	return MP4RRC_OK;
}
MP4RRC TrunBox::GetSampleInfo(int index)
{
	VOLOGR("+TrunBox: GetSampleInfo,%d,count:%d",index,_sample_count);
	uint32 size = 0;
	if(_tr_flags & 0x100 ) size += 4;
	if(_tr_flags & 0x200 ) size += 4;
	if(_tr_flags & 0x400 ) size += 4;
	if(_tr_flags & 0x800 ) size += 4;

	memset(&_present_sample_info, 0x00, sizeof(_present_sample_info));
	bool bFalseRead = false;
	if(!GetReader()->SetPosition(_data_pos))
	{
		bFalseRead = true;
	}
	
	for(uint32 i = 0; i< _sample_count; i++)
	{

		if(!bFalseRead&& (_tr_flags & 0x100) )
		{
			if(!GetReader()->Read(& _present_sample_info._sample_duration))
				return MP4RRC_READ_FAILED;
		}
		else
			_present_sample_info._sample_duration = m_nDefaultDuration;

		if(!bFalseRead&& (_tr_flags & 0x200) )
		{
			if(!GetReader()->Read(& _present_sample_info._sample_size))
				return MP4RRC_READ_FAILED;
		}
		else
			_present_sample_info._sample_size = m_nDefaultSize;

		if(!bFalseRead&& (_tr_flags & 0x400) ) 
		{
			if(!GetReader()->Read(& _present_sample_info._sample_flags))
				return MP4RRC_READ_FAILED;
		}
		if(!bFalseRead&& (_tr_flags & 0x800) )
		{
			if(!GetReader()->Read(& _present_sample_info._sample_composition_time_offset))
				return MP4RRC_READ_FAILED;
		}
		if(int(i )== index) break;
		_present_sample_info._sample_time += _present_sample_info._sample_duration;
		_present_sample_info._sample_addr += _present_sample_info._sample_size;

	}
	VOLOGR("TrunBox::GetSampleInfo _present_sample_info._sample_addr; %d",(uint32)_present_sample_info._sample_addr,(uint32)_present_sample_info._sample_time);
	VOLOGR("-TrunBox: GetSampleInfo,%d",index);
	return MP4RRC_OK;
}

uint32 TrunBox::SetParam(int id, VO_PTR pParam)
{
	switch(id)
	{
	case VO_PID_FRAGMENT_DEFAULT_SAMPLE_SIZE:
		{
			if( m_nDefaultSize == 0 )
				m_nDefaultSize = *((uint32*)pParam);
			break;
		}
	case VO_PID_FRAGMENT_DEFAULT_SAMPLE_DURATION:
		{
			if( m_nDefaultDuration == 0 )
				m_nDefaultDuration = *((uint32*)pParam);
			break;
		}
	default:
		return MP4RRC_MAX;
	}
	return MP4RRC_OK;
}
uint32 TrunBox::GetSampleSize(int index) 
{
	GetSampleInfo(index);
	return _present_sample_info._sample_size;
}
uint64 TrunBox::GetSampleDuration(int index)
{	
	GetSampleInfo(index);
	return _present_sample_info._sample_duration;
}
uint64 TrunBox::GetSampleTime(int index)
{	
	GetSampleInfo(index);
	VOLOGI("TrunBox::GetSampleTime _sample_time : %d, _sample composition_time_offset: %d",(int)_present_sample_info._sample_time,(int)_present_sample_info._sample_composition_time_offset);
	return _present_sample_info._sample_time + _present_sample_info._sample_composition_time_offset;
}
uint64 TrunBox::GetSampleAddress(int index)
{
	GetSampleInfo(index);
	return _present_sample_info._sample_addr;
}
TrackFragment::TrackFragment()
: Track()
{
	_index_cache = 0;
	_count_real_samples = 0;
	_pre_sample_size = 0;
}
TrackFragment::~TrackFragment()
{

}
uint32 TrackFragment::GetMaxSampleSize()
{
	if(IsVideo())
		return GetWidth() * GetHeight() * 1;
	if(IsAudio())
		return GetSampleBits() * GetChannelCount() * GetSampleRate() / 8;

	return 0;
}
uint32 TrackFragment::GetBitrate()
{
	if(IsAudio())
		return Track::GetBitrate();
	return 0;
}

MP4RRC TrackFragment::Open(Reader* r)
{
	VO_U32 s = voOS_GetSysTime();
	MP4RRC rcc = Track::Open(r);
	if(rcc != MP4RRC_OK)
		return rcc;
	if (_movie)
	{
		_fragmentBox.SetBoxType(_movie->GetBoxType());
	}
	rcc = _fragmentBox.Init(r, GetTrackID());
	if(rcc != MP4RRC_OK)
	{
		VOLOGI("TrackFragment::Open,call_fragmentBox.Init(...) failed and track id = %d", GetTrackID());
		return rcc;
	}
	s = voOS_GetSysTime() - s;
	VOLOGR("End Decryption. Whole Cost: %d ms", s );


	return MP4RRC_OK;
}
uint32 TrackFragment::GetSampleCount()  
{ 
	return _fragmentBox.GetSampleCount();
}

uint32 TrackFragment::GetSampleSize(int index)
{
	return _fragmentBox.GetSampleSize(index);
}
uint64 TrackFragment::GetSampleTime(int32 index)
{
	uint64 time =_time_divisor * _fragmentBox.GetSampleTime(index) /GetTimeScale();
	return time;
}
int TrackFragment::FindSampleByTime(uint32 time)
{
	int sampleindex = 0;
	if (IsPDIsmv())
	{
		_fragmentBox.SeekToMoofByTime((((uint64)time) * GetTimeScale() + _time_divisor - 1) / _time_divisor);
#ifndef LIEYUN_DEBUG
		return sampleindex;
#else
		if(IsAudio())
			return _fragmentBox.GetSampleIndex((((uint64)time) * GetTimeScale() + _time_divisor - 1) / _time_divisor);
		else
			return sampleindex;
#endif
		//sampleindex = 0;
		//return sampleindex;
	}
	return _fragmentBox.GetSampleIndex((((uint64)time) * GetTimeScale() + _time_divisor - 1) / _time_divisor); 

}
uint64 TrackFragment::GetSampleTime2(int32 index)
{
	return _time_divisor * _fragmentBox.GetSampleTime(index+1) /GetTimeScale();
}
uint32 TrackFragment::GetSampleAddress(int index)
{
	_current_chunk_size = GetSampleSize(index);
	_current_chunk_address = _fragmentBox.GetSampleAddress(index);
	_current_sample_chunk = index;
	VOLOGR("IsAudio:%d ,index: %d, addr: %d",IsAudio(), index, (uint32)_current_chunk_address);
	return uint32(_current_chunk_address);
}
uint32 TrackFragment::GetSampleSync(int index)
{
	if( !IsVideo() ) return 0;
	return _fragmentBox.GetSampleSync(index);
}
int TrackFragment::GetNextSyncPoint(int index)
{
	VOLOGR("+GetNextSyncPoint");
	int ret = _fragmentBox.GetNextSyncPoint(index);
	VOLOGR("-GetNextSyncPoint.Ret:%d",ret);
	return ret;
}
int TrackFragment::GetPrevSyncPoint(int index)
{
	return _fragmentBox.GetPrevSyncPoint(index);
}
MP4RRC TrackFragment::ParseDescriptor(Reader* r, int bodysize)
{
	return Track::ParseDescriptor(r,bodysize);
}

MP4RRC TrackFragment::ParseStsdBox(Reader* r, int bodySize)
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

	int schisize = scanner.FindTag(FOURCC2_schi,mark2 + sinfsize);
	if(schisize <=0)	return MP4RRC_NO_SCHI;

	Scanner scanner1(r);
	int uuidsize = scanner.FindTag(FOURCC2_uuid, r->Position() + schisize);

	if(uuidsize >0) 
		_fragmentBox.ParserUUID(r, NULL,uuidsize);

	return MP4RRC_OK;
}

uint32 TrackFragment::GetSampleData(int index, void* buffer, int buffersize)
{
	uint32 sizeSample = Track::GetSampleData(index, buffer,buffersize);
	if((0 >= sizeSample)||(ERR_CHECK_READ == sizeSample))
		return 0;
	VO_U32 s = voOS_GetSysTime() ;

	VOLOGR("Start Decryption. SampleIndex: %d, BufferSize: %10d . ISEncryption:%s ", index, sizeSample, _IsEncryption? "yes":"no" );
	if(_IsEncryption)
	{
		if (_movie)
		{
			VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO info;
			info.pSampleEncryptionBox = _fragmentBox._current_fragment_info.EncyptBox.pBoxData;
			info.uSampleEncryptionBoxSize = _fragmentBox._current_fragment_info.EncyptBox.uBoxCurSize;
			info.uSampleIndex = index - _fragmentBox._current_fragment_info._sync_index;
			info.uTrackId = _nTrack_ID;

			if (_movie->GetDrmReader())
			{
				VO_U32 uReturnValue = 0;
				_movie->GetDrmReader()->DRMData(VO_DRMDATATYPE_MEDIASAMPLE,(VO_PBYTE)buffer,sizeSample,(VO_PBYTE *)(&buffer),&uReturnValue,&info);
			}
		}
	}	
	s = voOS_GetSysTime() - s;
	VOLOGR("End Decryption. Whole Cost: %d ms", s );
	return sizeSample;
}

VO_U32 TrackFragment::TrackGenerateIndex(VO_U32 uIndex, Reader* pReader)
{
	return _fragmentBox.GenerateIndex(uIndex, pReader);
}

VO_BOOL TrackFragment::IsIndexReady(VO_U32 uIndex, VO_U32* pMaxIndex)
{
	return _fragmentBox.IsIndexReady(uIndex,pMaxIndex);
}

VO_U32 TrackFragment::GetMaxTimeByIndex(VO_U32 uIndex,VO_S64& ullTs)
{

	VO_U32 ret = _fragmentBox.GetMaxTimeByIndex(uIndex,ullTs);
	if(VO_ERR_SOURCE_OK == ret)
	{
		ullTs = _time_divisor * ullTs /GetTimeScale();
	}
	return ret;
}
VO_U32 TrackFragment::ResetIndex()
{
	_prev_sample_chunk = -1;
	return _fragmentBox.ResetIndex();
}
VO_VOID FRAGMENT_INFO::Reset()
{
	_sample_description_index = 0;
	_default_sample_duration = 0;
	_default_sample_size = 0;
	_default_sample_flags = 0;
	_sample_count = 0;
	_current_index = 0;
	_moof_size = 0;
	_sample_data_offset = 0;
	_sync_index = 0;
	_sync_next_index = 0;
	_sync_prev_index = 0;
	_begin_time = 0;
	_moof_offset = 0;
	_current_entry = 0;
}


VO_VOID FRAGMENT_INFO::Uninit()
{
	if (EncyptBox.pBoxData)
	{
		delete []EncyptBox.pBoxData;
		EncyptBox.pBoxData = NULL;
	}
}
