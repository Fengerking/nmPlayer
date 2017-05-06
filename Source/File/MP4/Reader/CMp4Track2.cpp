#include "CMp4Track2.h"
#include "mp4cfg.h"
#include "mp4desc.h"
#include "fMacros.h"
#include "fCC.h"
#include "voLog.h"
#include "fCodec.h"

#include "voOSFunc.h"
#include "voSource.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
//const double TIME_UNIT = 1.0;	//all time multiply this value is MS
const int TIME_UNIT = 1;	//all time multiply this value is MS

#ifndef FCC
#define FCC(ch4) ((((VO_U32)(ch4) & 0xFF) << 24) |     \
	(((VO_U32)(ch4) & 0xFF00) << 8) |    \
	(((VO_U32)(ch4) & 0xFF0000) >> 8) |  \
	(((VO_U32)(ch4) & 0xFF000000) >> 24))
#endif	//FCC



SeekTable::SeekTable(VO_MEM_OPERATOR* pMemOp)
	: CvoBaseMemOpr(pMemOp)
{
	m_entries= VO_NULL;
	m_count= 0;
}

SeekTable::~SeekTable()
{
	Release();
}

void SeekTable::Release()
{
	SAFE_MEM_FREE(m_entries);
	m_count = 0;
}

VO_BOOL SeekTable::Init(Track* pTrack, VO_U32 step)
{
	Release();
	VO_U32 samples = pTrack->GetSampleCount();
	m_count = (samples + step - 1) / step + 1; //for end time
	m_entries = NEW_OBJS(SeekEntry, m_count);
	if(!m_entries)
		return VO_FALSE;

	SeekEntry* p = m_entries;
	VO_U32 dwLastSampleAddr = -1;
	VO_U32 dwSampleAddr = 0;
	for(VO_U32 i = 0; i < samples; i += step)
	{
		dwSampleAddr = pTrack->GetSampleAddressEnd(i);
#ifdef _CHECK_READ
		if (dwSampleAddr == ERR_CHECK_READ)
			return VO_FALSE;
#endif //_CHECK_READ
		if(dwLastSampleAddr != dwSampleAddr)
		{
			p->addr = dwSampleAddr;
			p->time = pTrack->GetSampleTime(i);

			dwLastSampleAddr = dwSampleAddr;
			++p;
		}
		else
			m_count--;
	}

	// last sample
	dwSampleAddr = pTrack->GetSampleAddressEnd(samples - 1);
#ifdef _CHECK_READ
	if (dwSampleAddr == ERR_CHECK_READ)
		return VO_FALSE;
#endif //_CHECK_READ
	if(dwLastSampleAddr != dwSampleAddr)
	{
		p->addr = dwSampleAddr;
		p->time = pTrack->GetSampleTime2(samples - 1);
	}
	else
		m_count--;

	return VO_TRUE;
}

VO_S64 SeekTable::Find(VO_S64 key, COMPARE_KEY compare, SeekEntry** ppResult1, SeekEntry** ppResult2)
{
	SeekEntry* p = m_entries;
	VO_U32 step = m_count;
	SeekEntry* p1 = p;
	SeekEntry* p2 = p + step - 1;
	VO_S64 dir = 0;
	VO_S32 rc = 0;
	while ((dir = compare(key, p)))
	{
		if (step == 1)
		{
			*ppResult1 = p;
			rc = dir > 0 ? -1 : 1; //RC_BEFORE_KEY : RC_AFTER_KEY;
			break;
		}
		step++;
		step /= 2;
		if (dir > 0)
		{
			p += step;
			if (p > p2)
			{
				*ppResult1 = p2;
				rc = -1;
				break;
			}
		}
		else
		{
			p -= step;
			if (p < p1)
			{
				*ppResult1 = p1;
				rc = 1;
				break;
			}
		}
	}
	switch (rc)
	{
	case 1:
		if (ppResult2)
		{
			*ppResult2 = (*ppResult1) - 1;
			if ((*ppResult2) < p1)
				*ppResult2 = p1;
		}
		break;

	case -1:
		if (ppResult2)
		{
			*ppResult2 = (*ppResult1) + 1;
			if ((*ppResult2) > p2)
				*ppResult2 = p2;
		}
		break;

	default: //rc == 0
		*ppResult1 = p;
		if (ppResult2)
			*ppResult2 = p;
		break;
	}
	return rc;
}

CMp4Track2::CMp4Track2(Track* pTrack, Reader* pReader, VO_MEM_OPERATOR* pMemOp)
: CBaseTrack((pTrack->IsVideo() ? VOTT_VIDEO : (pTrack->IsAudio() ? VOTT_AUDIO : VOTT_TEXT)), 0, ((pTrack->IsIsml() || pTrack->IsDash()) ? pTrack->GetDuration() : pTrack->GetSampleTime2(pTrack->GetSampleCount() - 1)) * TIME_UNIT, pMemOp)
	, m_seeker(pMemOp)
	
{
	m_pReader = pReader;
	m_pTrack = pTrack;
	m_pHeadData = VO_NULL;
	m_dwHeadData = 0;
	m_dwCurrIndex = 0;
	m_qwCurrTime = 0;
	m_dwMaxSampleSize = 0;
	m_dwSampleCount = pTrack->GetSampleCount();
		
	m_nNALLengthSize = 0;
	m_nCurThumbnailScanCount = 0;
	m_nCurSyncFrame = 0;
	m_bIsStreaming = VO_FALSE;
	m_IsCencDrm = VO_FALSE;

}

CMp4Track2::~CMp4Track2()
{
	VOLOGR("+~CMp4Track2");
	m_pTrack->Close();
	delete m_pTrack;
	VOLOGR("-~CMp4Track2");

}
VO_U32 CMp4Track2::ResetIndex()
{
	return m_pTrack->ResetIndex();
}
//12/19/2011, leon , add for text track

VO_U32 CMp4Track2::Prepare()
{
#ifdef _SUPPORT_TTEXT
	if(GetType() == VOTT_TEXT){
		//get m_pSampleData from subtitle track
		m_pTrack->GetSampleBuffer((void**)&m_pSampleData );
		return VO_ERR_SOURCE_OK;
	}
#endif	
	return CBaseTrack::Prepare();
}
//12/27/2011, leon , add for text track
VO_U32 CMp4Track2::Unprepare()
{
#ifdef _SUPPORT_TTEXT
	if(GetType() == VOTT_TEXT){
		return VO_ERR_SOURCE_OK;
	}
#endif	
	
	return CBaseTrack::Unprepare();
}

VO_U32 CMp4Track2::GetFirstFrame(VO_SOURCE_SAMPLE* pSample)
{
	VO_S32 dwIndex = 0;
	if(pSample->Flag & VO_SOURCE_SAMPLE_FLAG_NOFRAMESKIPED)
		dwIndex = 0;
	else
		dwIndex = m_pTrack->GetNextSyncPoint(dwIndex);
	uint32 size = m_pTrack->GetSampleData(dwIndex, m_pSampleData, m_dwMaxSampleSize);

	//10/19/2011,add by leon, if file has only one key, it will return ERR_CHECK_READ.
	if(size == ERR_CHECK_READ)  return VO_ERR_SOURCE_SEEKFAIL ;
	//10/19/2011,add by leon, if file has only one key, it will return ERR_CHECK_READ.

	pSample->Buffer = m_pSampleData;
	pSample->Size = size;
	pSample->Time = m_pTrack->GetSampleTime(dwIndex) * TIME_UNIT;
	pSample->Duration = 1;
#if 0 //don't set sync on size field
	if(m_pTrack->GetSampleSync(dwIndex))
		pSample->Size |= 0x80000000;
#endif
	return VO_ERR_SOURCE_OK;
}
FILE *ff = NULL;

VO_U32 CMp4Track2::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	VOLOGI("+GetSampleN");
	if(VOTT_VIDEO == m_nType)	//video track need to process drop frame
	{
		VO_S32 nCurrIndex = -1;
		while(true)
		{
			nCurrIndex = m_pTrack->GetNextSyncPoint(m_dwCurrIndex + 1);

			if(-1 == nCurrIndex || pSample->Time < m_pTrack->GetSampleTime(nCurrIndex) * TIME_UNIT)
				break;
			VOLOGR("TrackID %d,Sample %d, time (%lld %lld)", m_pTrack->GetTrackID(),nCurrIndex, pSample->Time, m_pTrack->GetSampleTime(nCurrIndex) * TIME_UNIT);

			m_dwCurrIndex = nCurrIndex;
			
			pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;
		}
	}
	m_dwSampleCount = m_pTrack->GetSampleCount();
	VOLOGR("m_dwCurrIndex:%d,m_dwSampleCount:%d",m_dwCurrIndex,m_dwSampleCount);
	if(m_dwCurrIndex >= VO_S32(m_dwSampleCount))
		return VO_ERR_SOURCE_END;
	uint32 size = m_pTrack->GetSampleData(m_dwCurrIndex, m_pSampleData, m_dwMaxSampleSize);
#ifdef _CHECK_READ
	if (size == ERR_CHECK_READ)
#else //_CHECK_READ
	if (size == 0)
#endif //_CHECK_READ
	{
		uint32 err = m_pReader->GetErrorCode();
		m_pReader->ClearErrorCode();
		switch((VO_S32)err)
		{
		case 0:
			VOLOGR("Source End!");
			return VO_ERR_SOURCE_END;

		case MPXERR_DATA_DOWNLOADING:
			return VO_ERR_SOURCE_NEEDRETRY;
		}
	}

	pSample->Buffer = m_pSampleData;

	if(m_nType == VOTT_VIDEO && m_nNALLengthSize >0)
	{

		if(!(m_pSampleData[0] ==0x00 && m_pSampleData[1] == 0x00 &&m_pSampleData[2] ==0x00 && m_pSampleData[3] == 0x01 ))
		{
			VO_U32 codec_cc = 0;
			GetCodecCC(&codec_cc);
			if(!(FOURCC_HVC1 == codec_cc))
			{

				uint32 len =0;
				VO_PBYTE buf = m_pSampleData;

				while(len != size )
				{
					uint32 sSize = 0;
					if( len >size || len + m_nNALLengthSize > size) 
					{
						VOLOGE("ERROR - CORRUPT NAL ");
						size = 0;
						break;
					}
					for(VO_U32 i =0 ;i<m_nNALLengthSize;i++)
						sSize |= buf [i] << (m_nNALLengthSize - 1 - i)* 8;
					len += sSize + m_nNALLengthSize;
					buf += sSize + m_nNALLengthSize;
				}
			}

		}

	}

	pSample->Size = size;
	pSample->Time = m_pTrack->GetSampleTime(m_dwCurrIndex) * TIME_UNIT;
	pSample->Duration = 1;

	if(m_pTrack->GetSampleSync(m_dwCurrIndex))
		pSample->Size |= 0x80000000;


	VOLOGI("Type: %d, [%c] Index: %u, Time: %lld, Size: %d", m_nType, ((pSample->Size & 0x80000000) ? 'I' : ' '), m_dwCurrIndex, pSample->Time, size);
	m_dwCurrIndex++;
	m_qwCurrTime = pSample->Time;
	VOLOGI("-GetSampleN");
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::SetPosN(VO_S64* pPos)
{
//	if( !m_pTrack->IsCanSeek()) return VO_ERR_NOT_IMPLEMENT;
	VO_U32 dwTime = (VO_U32)(*pPos / TIME_UNIT);
	if (dwTime > m_pTrack->GetDuration())
		return VO_ERR_SOURCE_END;

	VOLOGI("+SetPosN");
	VO_S32 nIndex = 0;
	if (m_pTrack->IsPDIsmv())
	{
		ResetIndex();
		nIndex = m_pTrack->FindSampleByTime(dwTime);
		if(nIndex < 0)
			return VO_ERR_SOURCE_END;
		m_dwCurrIndex = 0;
		if(VOTT_VIDEO == m_nType)
		{
			m_pTrack->GetPrevSyncPoint(nIndex);
			m_dwCurrIndex = 0;

			VOLOGI("+CMp4Track2::SetPosN m_pTrack->GetSampleTime");
			*pPos = m_pTrack->GetSampleTime(m_dwCurrIndex) * TIME_UNIT;
			VOLOGI("-CMp4Track2::SetPosN m_pTrack->GetSampleTime");
		}
		//else
		//{
		//	m_dwCurrIndex =+ 30; //30 is an estimate value, to ensure audio time >= video time. 
		//}
	}
	else
	{
		nIndex = m_pTrack->FindSampleByTime(dwTime);
		if(nIndex < 0)
			return VO_ERR_SOURCE_END;
		//*pPos = m_pTrack->GetSampleTime(m_dwCurrIndex) * TIME_UNIT;

		if(VOTT_VIDEO == m_nType)
		{
			m_dwCurrIndex = m_pTrack->GetPrevSyncPoint(nIndex);
			if(-1 == int(m_dwCurrIndex))
				m_dwCurrIndex = 0;
			VOLOGI("+CMp4Track2::SetPosN m_pTrack->GetSampleTime index: %d",m_dwCurrIndex);
			*pPos = m_pTrack->GetSampleTime(m_dwCurrIndex) * TIME_UNIT;
			VOLOGI("-CMp4Track2::SetPosN m_pTrack->GetSampleTime time: %d",(VO_U32)(*pPos));
		}
		else
			m_dwCurrIndex = nIndex;
	}
	VOLOGI("Type: %d, Pos: %u -> %u, Index: %d", m_nType, dwTime, (VO_U32)(*pPos), nIndex);
	VOLOGI("-SetPosN");
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::GetSampleK(VO_SOURCE_SAMPLE* pSample)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	VO_S32 nCurrIndex = (VO_SOURCE_PM_FF == m_PlayMode) ? m_pTrack->GetNextSyncPoint(m_dwCurrIndex) : 
		m_pTrack->GetPrevSyncPoint(m_dwCurrIndex);

	if(-1 == nCurrIndex || nCurrIndex > (VO_S32)m_dwSampleCount)
		return VO_ERR_SOURCE_END;

	uint32 size = m_pTrack->GetSampleData(nCurrIndex, m_pSampleData, m_dwMaxSampleSize);
#ifdef _CHECK_READ
	if (size == ERR_CHECK_READ)
#else //_CHECK_READ
	if (size == 0)
#endif //_CHECK_READ
	{
		uint32 err = m_pReader->GetErrorCode();
		m_pReader->ClearErrorCode();
		switch((VO_S32)err)
		{
		case 0:
			return VO_ERR_SOURCE_END;

		case MPXERR_DATA_DOWNLOADING:
			return VO_ERR_SOURCE_NEEDRETRY;
		}
	}

	pSample->Buffer = m_pSampleData;
	pSample->Size = size;
	pSample->Time = m_pTrack->GetSampleTime(nCurrIndex) * TIME_UNIT;
	pSample->Duration = 1;
	if(m_pTrack->GetSampleSync(nCurrIndex))
		pSample->Size |= 0x80000000;

	if(VO_SOURCE_PM_FF == m_PlayMode)
		m_dwCurrIndex = nCurrIndex + 1;
	else
		m_dwCurrIndex = nCurrIndex - 1;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::SetPosK(VO_S64* pPos)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	return SetPosN(pPos);
}

VO_U32 CMp4Track2::GetHeadData(VO_CODECBUFFER* pHeadData)
{

#ifdef _SUPPORT_SEGMENT
	if(GetIsStreaming())//m_pTrack->IsDash())
	{
		VO_U32 nCodecCC = 0;
		GetCodecCC(&nCodecCC);
		if(m_pTrack->IsAudio())
		{	
			pHeadData->Length = m_dwHeadData;
			pHeadData->Buffer = m_pHeadData;
		}
		else if((nCodecCC == FOURCC_HVC1)||(nCodecCC == FOURCC_AVC1))
		{
			if (nCodecCC == FOURCC_AVC1)
			{
				ConvertHeadData(m_pHeadData,m_dwHeadData);
				pHeadData->Length = m_nHeadSize;
				pHeadData->Buffer = m_pSeqHeadData;
			}
			else if(nCodecCC == FOURCC_HVC1)
			{
				Hevc_ConvertHeadData(m_pHeadData,m_dwHeadData);
				pHeadData->Length = m_nHeadSize;
				pHeadData->Buffer = m_pSeqHeadData;
			}
		}

	}
	else
	{
#endif
		pHeadData->Length = m_dwHeadData;
		pHeadData->Buffer = m_pHeadData;
#ifdef _SUPPORT_SEGMENT
	}
#endif

	return VO_ERR_SOURCE_OK;
}

void CMp4Track2::PrepareOggHeadData(void* dsidata, int dsisize)
{
	struct VORBISFORMAT2 {
	  VO_U32 Channels;
	  VO_U32 SamplesPerSec;
	  VO_U32 BitsPerSample;
	  VO_U32 HeaderSize[3]; // Sizes of three init packets
	};

	VO_S32 packet_size[3];
	memset( packet_size , 0 , sizeof( packet_size ) );

	VO_PBYTE ptr_pos = (VO_PBYTE)dsidata;
	VO_PBYTE ptrVorbis = (VO_PBYTE)dsidata;
	VO_U32 nDsiSize = dsisize;

	VO_S32 nPageSeg = VO_S32(*ptrVorbis);
	ptrVorbis++;
	if(nPageSeg == 0)
	{			
		int i = 0;
		packet_size[0] = (VO_S32)*ptrVorbis;
		ptrVorbis += (1 +packet_size[0]);
		ptrVorbis ++;
		packet_size[1] = (VO_S32)*ptrVorbis;
		ptrVorbis += (1 +packet_size[1]);
		ptrVorbis ++;
		packet_size[2] = nDsiSize - packet_size[0] - packet_size[1] - 7;//(VO_S32)*ptrVorbis;
		packet_size[0] += 1;
		packet_size[1] += 1;
		packet_size[2] += 1;
		m_dwHeadData = packet_size[0] +  packet_size[1] + packet_size[2] + sizeof( VORBISFORMAT2 );		
	}
	else if(nPageSeg ==2)
	{
		ptr_pos ++;
		VO_S32 index = 0;
		while( index < nPageSeg )
		{
			packet_size[index] = packet_size[index] + (VO_U32)*ptr_pos;

			if( *ptr_pos < 0xff )
				index++;

			ptr_pos++;
		}
		packet_size[index] = dsisize - ( ptr_pos - (VO_PBYTE)dsidata ) - packet_size[0] - packet_size[1];
		m_dwHeadData = dsisize - ( ptr_pos - (VO_PBYTE)dsidata ) + sizeof( VORBISFORMAT2 );
	}
	m_pHeadData = (VO_PBYTE)MemAlloc(m_dwHeadData);
	VORBISFORMAT2 * ptr_vorbis_head = (VORBISFORMAT2*)m_pHeadData;

	ptr_vorbis_head->BitsPerSample = 16;
	ptr_vorbis_head->Channels = m_pTrack->GetChannelCount();
	ptr_vorbis_head->SamplesPerSec = m_pTrack->GetSampleRate();
	ptr_vorbis_head->HeaderSize[0] = packet_size[0];
	ptr_vorbis_head->HeaderSize[1] = packet_size[1];
	ptr_vorbis_head->HeaderSize[2] = packet_size[2];
	
	if(nPageSeg == 0)
	{
			
		VO_PBYTE ptrVorbis = (VO_PBYTE)dsidata;
		VO_PBYTE ptrHead = m_pHeadData;
		ptrHead += sizeof( VORBISFORMAT2 );
		ptrVorbis += 2;
		MemCopy( ptrHead , ptrVorbis , packet_size[0]);
		ptrVorbis += (packet_size[0] + 1);
		ptrHead += packet_size[0];
		MemCopy( ptrHead , ptrVorbis , packet_size[1]);
		ptrVorbis += (packet_size[1] + 1);
		ptrHead += packet_size[1];
		MemCopy( ptrHead , ptrVorbis , packet_size[2]);
	}
	else if(nPageSeg ==2)
	{
		MemCopy( m_pHeadData + sizeof( VORBISFORMAT2 ) , ptr_pos , packet_size[0] + packet_size[1] + packet_size[2] );
	}

}
VO_U32 CMp4Track2::Init(VO_U32 nSourceOpenFlags)
{
	//get head data information!!
	switch(m_pTrack->GetSampleType())
	{

	case FOURCC2_mp4a:
		{
			MemStream ms(m_pTrack->GetDescriptorData(), m_pTrack->GetDescriptorSize());
			ReaderMSB r(&ms);
			ESDescriptor* ed = (ESDescriptor*)Descriptor::Load(r);
			if (ed == NULL)
				break;
			DecoderConfigDescriptor* dcd = (DecoderConfigDescriptor*)ed->GetChildByTag(DecoderConfigDescrTag);
			DecoderSpecificInfo* dsi = dcd ? (DecoderSpecificInfo*)dcd->GetChildByTag(DecSpecificInfoTag) : VO_NULL;
			if(!dsi)
			{
				delete ed;
				break;
			}
			/*add by leon, 05/16/2012, #14418*/
			if(m_pTrack->GetSampleRate() <= 1)
				m_pTrack->SetSampleRate(dcd->avgBitrate);


			m_dwHeadData = dsi->GetBodySize();
			if(m_dwHeadData > 0)
			{
				if (0xDD == dcd->objectTypeIndication) //OGG
				{
					PrepareOggHeadData(dsi->GetBodyData(), m_dwHeadData);
				}
				else
				{
					m_pHeadData = (VO_PBYTE)MemAlloc(m_dwHeadData);
					if(m_pHeadData)
						MemCopy(m_pHeadData, dsi->GetBodyData(), m_dwHeadData);
				}
			}

			delete ed;
		}
		break;

	case FOURCC2_s264:
	case FOURCC2_mp4v:	
		{
			MemStream ms(m_pTrack->GetDescriptorData(), m_pTrack->GetDescriptorSize());
			ReaderMSB r(&ms);
			ESDescriptor* ed = (ESDescriptor*)Descriptor::Load(r);
			if (ed == NULL)
				break;
			DecoderConfigDescriptor* dcd = (DecoderConfigDescriptor*)ed->GetChildByTag(DecoderConfigDescrTag);
			DecoderSpecificInfo* dsi = dcd ? (DecoderSpecificInfo*)dcd->GetChildByTag(DecSpecificInfoTag) : VO_NULL;
			if(!dsi)
			{
				delete ed;
				break;
			}

			uint32 dsisize  = 0;
			if(dsi)
			{
				dsisize = dsi->GetBodySize();
				if(dsisize == 8)
					dsisize = 0;
			}

			m_dwHeadData = dsisize;
			if(m_dwHeadData > 0)
			{
				m_pHeadData = (VO_PBYTE)MemAlloc(m_dwHeadData);
				if(m_pHeadData)
					MemCopy(m_pHeadData, dsi->GetBodyData(), m_dwHeadData);
			}

			delete ed;
		}
		break;

	case FOURCC2_s263:
	case FOURCC2_H263: //10/13/2008, Jason
		break;

	case FOURCC2_jpeg: //10/13/2008, Jason
	case FOURCC2_JPEG:
	case FOURCC2_stpp:
		break;

	case FOURCC2_alaw: //20110627, Jason
	case FOURCC2_ulaw: //20110627, Jason
	case FOURCC2_sowt:
	case FOURCC2_alac: //20100712, yangyi
	case 0x6d730011://20120312,Leon ,ADPCM, #11375
		{
			m_dwHeadData = m_pTrack->GetDescriptorSize() + 18;

			m_pHeadData = (VO_PBYTE)MemAlloc(m_dwHeadData);

			if( m_pHeadData )
			{
				MemSet( m_pHeadData , 0 , m_dwHeadData );
				VO_WAVEFORMATEX * ptr_wavformat = (VO_WAVEFORMATEX *)m_pHeadData;
				ptr_wavformat->nChannels = m_pTrack->GetChannelCount();
				ptr_wavformat->nSamplesPerSec = m_pTrack->GetSampleRate();
				ptr_wavformat->wBitsPerSample = m_pTrack->GetSampleBits();
				ptr_wavformat->cbSize = (VO_U16)m_pTrack->GetDescriptorSize();

				MemCopy( m_pHeadData + 18 , m_pTrack->GetDescriptorData() , m_pTrack->GetDescriptorSize() );
			}
		}
		break;

	case FOURCC2_tx3g:
	case FOURCC2_sawb:
	case FOURCC2_samr:
	case FOURCC2_sevc:	//4/28/2009, Jason
	case FOURCC2_evrc: //EVRC, 4/11/2011, Jason
	case FOURCC2_avc1:	//2009/6/1, East
	case FOURCC2_avc3:
	case FOURCC2_DX50: //04/06/2012, Leon,#12277
	case FOURCC2_hvc1:
	case FOURCC2_hev1:
	default:
		{
			m_dwHeadData = m_pTrack->GetDescriptorSize();
			if(m_dwHeadData > 0)
			{
				m_pHeadData = (VO_PBYTE)MemAlloc(m_dwHeadData);
				if(m_pHeadData)
					MemCopy(m_pHeadData, m_pTrack->GetDescriptorData(), m_dwHeadData);
			}
		}
		break;
	}

	switch(m_pTrack->GetSampleType())
	{
	case FOURCC2_alaw: //20110628, Jason
		m_wFormatTag = AudioFlag_MS_ALAW;
		m_wBitsPerSample = 8;
		break;
	case 0x6d730011://20120312,Leon ,ADPCM, #11375
		m_wFormatTag = AudioFlag_IMA_ADPCM;
		m_wBitsPerSample = m_pTrack->GetSampleBits();
		break;
	case FOURCC2_ulaw: //20110628, Jason
		m_wFormatTag = AudioFlag_MS_MULAW;
		m_wBitsPerSample = 8;
		break;
	case FOURCC2_avc1://07/25/2011 Leon
	case FOURCC2_avc3:
		{
			AVCDecoderConfigurationRecord an1;
			MemStream ms(m_pTrack->GetDescriptorData(), m_pTrack->GetDescriptorSize());
			ReaderMSB r(&ms);
			an1.Load(r);
			m_nNALLengthSize = 1 + an1.lengthSizeMinusOne;
			break;
		}//07/25/2011 Leon
	case FOURCC2_hvc1:
	case FOURCC2_hev1:
		{
			HEVCDecoderConfigurationRecord hevDCR;
			MemStream ms(m_pTrack->GetDescriptorData(), m_pTrack->GetDescriptorSize());
			ReaderMSB r(&ms);
			hevDCR.Load(r);
			m_nNALLengthSize = 1 + hevDCR.lengthSizeMinusOne;
			break;
			
		}
	default:
		m_wFormatTag = AudioFlag_MS_UNKNOWN;
		m_wBitsPerSample = m_pTrack->GetSampleBits();
		break;
	}
	
	if(m_pTrack->IsDash())
	{
		m_dwMaxSampleSize = m_pTrack->GetMaxSampleSize();
	}
	else
	{
		m_dwMaxSampleSize = m_pTrack->GetMaxSampleSize();
		if (m_dwMaxSampleSize == 0)
			return VO_ERR_SOURCE_ERRORDATA;
	}


//	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags) && (VO_SOURCE_OPENPARAM_FLAG_OPENPD == (nSourceOpenFlags & 0xFF)))
	if(VO_SOURCE_OPENPARAM_FLAG_OPENPD == (nSourceOpenFlags & 0xFF))
	{
#if 0   ///<it cost too much time and I/O to list all sample,and it is not nessary
		int step = 0x8000 / m_dwMaxSampleSize;
		if (step < 1)
			step = 1;
		if (step > 8)
			step = 8;
		m_seeker.Init(m_pTrack, step);
#endif
		
	}

	return CBaseTrack::Init(nSourceOpenFlags);
}

VO_U32 CMp4Track2::Uninit()
{
	SAFE_MEM_FREE(m_pHeadData);

	return CBaseTrack::Uninit();
}

VO_U32 CMp4Track2::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(pdwMaxSampleSize)
		*pdwMaxSampleSize = m_dwMaxSampleSize;

	return m_dwMaxSampleSize ? VO_ERR_SOURCE_OK : VO_ERR_SOURCE_ERRORDATA;
}

VO_U32 CMp4Track2::GetCodecCC(VO_U32* pCC)
{
	uint32 uType = m_pTrack->GetSampleType();
	VOLOGI("+ GetCodecCC. Type:0x%08x",uType);
	switch(uType)
	{
	case FOURCC2_mp4a://0x6d703461
		{
			MemStream ms(m_pTrack->GetDescriptorData(), m_pTrack->GetDescriptorSize());
			ReaderMSB r(&ms);
			ESDescriptor* ed = (ESDescriptor*)Descriptor::Load(r);
			if (ed == NULL)
			{
				*pCC = FCC(uType);
				break;
			}
			DecoderConfigDescriptor* dcd = (DecoderConfigDescriptor*)ed->GetChildByTag(DecoderConfigDescrTag);
			if(dcd)
			{
			//	DecoderSpecificInfo* dsi =
				(DecoderSpecificInfo*)dcd->GetChildByTag(DecSpecificInfoTag);
				if((0x40 == dcd->objectTypeIndication) || (0x67 == dcd->objectTypeIndication))
					*pCC = AudioFlag_ISOMPEG4_AAC;
				else if(0xE1 == dcd->objectTypeIndication)
					*pCC = AudioFlag_QCELP;
				else if(0x6B == dcd->objectTypeIndication || 0x69 == dcd->objectTypeIndication)
					*pCC = AudioFlag_MP3;
				else if (0xDD == dcd->objectTypeIndication) //211, OGG
					*pCC = AudioFlag_OGG_1; //?? 9/13/2011, Jason
				else
					*pCC = FCC(uType);
			}
			else
				*pCC = FCC(uType);

			delete ed;
		}
		break;

	case FOURCC2_samr:
		*pCC = AudioFlag_AMR_NB;
		break;

	case FOURCC2_sawb:
		*pCC = AudioFlag_AMR_WB;
		break;

	case FOURCC2_sawp:
		*pCC = AudioFlag_AMR_WB_PLUS;
		break;

	case FOURCC2_sevc:  //4/28/2009, Jason
	case FOURCC2_evrc: //EVRC, 4/11/2011, Jason
		*pCC = AudioFlag_EVRC;
		break;

	case FOURCC2__mp3:
		*pCC = AudioFlag_MP3;
		break;

	case FOURCC2_ac_3:
		*pCC = AudioFlag_AC3;
		break;

	case FOURCC2_ec_3: //eAC3, East, 2010/03/09
		*pCC = AudioFlag_EAC3;
		break;

	case FOURCC2_mp4v:
		{
			MemStream ms(m_pTrack->GetDescriptorData(), m_pTrack->GetDescriptorSize());
			ReaderMSB r(&ms);
			ESDescriptor* ed = (ESDescriptor*)Descriptor::Load(r);
			if (ed == NULL)
			{
				*pCC = FCC(uType);
				break;
			}
			DecoderConfigDescriptor* dcd = (DecoderConfigDescriptor*)ed->GetChildByTag(DecoderConfigDescrTag);
			if(dcd)
			{
				//DecoderSpecificInfo* dsi = ;
				(DecoderSpecificInfo*)dcd->GetChildByTag(DecSpecificInfoTag);
				if(0x20 == dcd->objectTypeIndication)	//MPEG4
					*pCC = FOURCC_MP4V;
				else if(0x21 == dcd->objectTypeIndication)
					*pCC = FOURCC_H264;
				else if(0x60 <= dcd->objectTypeIndication && 0x65 >= dcd->objectTypeIndication)	//0x60 - 0x65
					*pCC = FOURCC_mpg2;//ISO 14496-11
				else
					*pCC = FCC(uType);
			}
			else
				*pCC = FCC(uType);

			delete ed;
		}
		break;

	case FOURCC2_s264:
		*pCC = FOURCC_H264;
		break;

	case FOURCC2_s263:
	case FOURCC2_H263: //10/13/2008, Jason
		*pCC = FOURCC_H263;
		break;
		
	case FOURCC2_jpeg: //10/13/2008, Jason
	case FOURCC2_JPEG:
		*pCC = FOURCC_MJPG;
		break;

	case FOURCC2_avc1:
	case FOURCC2_avc3:
		*pCC = FOURCC_AVC1;
		break;
	case FOURCC2_hvc1:
	case FOURCC2_hev1:
		*pCC = FOURCC_HVC1;
		break;

	case FOURCC2_alac:
		*pCC = FOURCC_ALAC;
		break;

	case FOURCC2_sqcp:
		*pCC = AudioFlag_QCELP;
		break;

	case 0x6d732000: // 'ms ', AC3? 6/20/2011, Jason
		*pCC = AudioFlag_AC3; //AudioFlag_MS_ADPCM; // ???
		break;
	case 0x6d730011: // 'ms ', 20120312,Leon ,ADPCM, #11375
		*pCC = AudioFlag_IMA_ADPCM; 
		break;

	case 0x6d730055: // 7/4/2011
		*pCC = AudioFlag_MP3;
		break;

	case FOURCC2_ulaw: //6/22/2011, Jason
		*pCC= AudioFlag_MS_MULAW;
		break;
	case FOURCC2_sowt://8/25/2011, Leon
	case FOURCC2_raw ://11/24/2011.leon
		*pCC= AudioFlag_MS_PCM;
		break;
	case FOURCC2_alaw: //6/22/2011, Jason 
		*pCC= AudioFlag_MS_ALAW;
		break;

	case FOURCC2_wma ://09/01/2011, Leon 
		*pCC = AudioFlag_WMA_PRO_V9;
		break;
	case FOURCC2_vc_1://09/01//2011, Leon 
		*pCC = FOURCC_WVC1;
		break;
	default:
		*pCC = FCC(uType);
		break;
	}
	VOLOGI("- GetCodecCC. CC:0x%08x",*pCC);
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::GetNextKeyFrame(VO_SOURCE_SAMPLE* pSample)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	int nRC = m_pTrack->GetNextSyncPoint(m_dwCurrIndex);
	if(-1 == nRC)
		return VO_ERR_SOURCE_NEEDRETRY;

	pSample->Size = m_pTrack->GetSampleSize(nRC) | 0x80000000;
	pSample->Time = m_pTrack->GetSampleTime(nRC) * TIME_UNIT;
	pSample->Duration = 1;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	VO_U32 uTime = VO_U32(llTime / TIME_UNIT);
	VO_S32 nIndex = m_pTrack->FindSampleByTime(uTime);
	if(nIndex < 0)
		return VO_ERR_INVALID_ARG;

	VO_S32 nPrevKeyframeIndex = m_pTrack->GetPrevSyncPoint(nIndex);
	*pllPreviousKeyframeTime = (-1 == nPrevKeyframeIndex) ? VO_MAXS64 : (m_pTrack->GetSampleTime(nPrevKeyframeIndex) * TIME_UNIT);

	VO_S32 nNextKeyframeIndex = m_pTrack->GetNextSyncPoint(nIndex + 1);
	*pllNextKeyframeTime = (-1 == nNextKeyframeIndex) ? VO_MAXS64 : (m_pTrack->GetSampleTime(nNextKeyframeIndex) * TIME_UNIT);	

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	if(VOTT_AUDIO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	pAudioFormat->Channels = GetAudioChannelCount();
	pAudioFormat->SampleBits = VO_S32(m_pTrack->GetSampleBits());
	pAudioFormat->SampleRate = m_pTrack->GetSampleRate();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	pVideoFormat->Width = m_pTrack->GetWidth();
	pVideoFormat->Height = m_pTrack->GetHeight();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::GetBitrate(VO_U32* pdwBitrate)
{
	if(pdwBitrate)
		*pdwBitrate = m_pTrack->GetBitrate() / 8;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::GetFrameNum(VO_U32* pdwFrameNum)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	if(pdwFrameNum)
		*pdwFrameNum = m_pTrack->GetSampleCount();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track2::GetFrameTime(VO_U32* pdwFrameTime)
{
	if(VOTT_VIDEO != m_nType || !m_pTrack->GetSampleCount())
		return VO_ERR_NOT_IMPLEMENT;

	if(pdwFrameTime)
		*pdwFrameTime = VO_U64(100) * m_pTrack->GetDuration() / m_pTrack->GetSampleCount();

	return VO_ERR_SOURCE_OK;
}

VO_U16 CMp4Track2::GetAudioChannelCount()
{
	if(FOURCC2_mp4a != m_pTrack->GetSampleType())
		return m_pTrack->GetChannelCount();

	MemStream ms(m_pTrack->GetDescriptorData(), m_pTrack->GetDescriptorSize());
	ReaderMSB r(&ms);
	ESDescriptor* ed = (ESDescriptor*)Descriptor::Load(r);
	if (ed == NULL)
		return m_pTrack->GetChannelCount();
	DecoderConfigDescriptor* dcd = (DecoderConfigDescriptor*)ed->GetChildByTag(DecoderConfigDescrTag);
	if(dcd)
	{
		if(0xE1 == dcd->objectTypeIndication)//AudioFlag_QCELP
			return 1;                        //QCELP has fix channel number = 1;
	}
	DecoderSpecificInfo* dsi = dcd ? (DecoderSpecificInfo*)dcd->GetChildByTag(DecSpecificInfoTag) : VO_NULL;
	if(!dsi)
	{
		delete ed;
		return m_pTrack->GetChannelCount();
	}

	AudioSpecificConfig cfg;
	cfg.Load(dsi->GetBodyData(), dsi->GetBodySize());
	VO_U16 val = VO_U16(cfg.channelConfiguration);
	delete ed;
	return val;
}

VO_U32 CMp4Track2::GetMediaTimeByFilePos(VO_S64 llFilePos, VO_S64* pllMediaTime, VO_BOOL bStart)
{
	SeekEntry *pResult1, *pResult2;
	VO_S64 rc = m_seeker.FindByAddr(llFilePos, &pResult1, &pResult2);
	if (rc == -2)
		return VO_ERR_SOURCE_END;
	else
	{
		*pllMediaTime = (rc == -1) ? pResult1->time : pResult2->time; //always return the result before the pos
		if (rc && (pResult1->addr == pResult2->addr) && pResult1->time) //end of file. begin of file is OK
			return VO_ERR_SOURCE_END;

		return VO_ERR_SOURCE_OK;
	}
}

VO_U32 CMp4Track2::GetFilePosByMediaTime(VO_S64 llMediaTime, VO_S64* pllFilePos, VO_BOOL bStart)
{
	VO_U32 uTime =VO_U32( llMediaTime / TIME_UNIT);
	VO_S32 nIndex = m_pTrack->FindSampleByTime(uTime);
	if(nIndex < 0)
		return VO_ERR_SOURCE_END;

	*pllFilePos = bStart ? m_pTrack->GetSampleAddressBase(nIndex) : m_pTrack->GetSampleAddressEnd(nIndex);

#ifdef _CHECK_READ
	if(ERR_CHECK_READ == *pllFilePos)
		return VO_ERR_SOURCE_END;
#endif //_CHECK_READ

	return VO_ERR_SOURCE_OK;
}

VO_U32  CMp4Track2::GetThumbnailSyncSampleInfo(VO_PTR pParam)
{
	VOLOGR("+GetThumbnailSyncSampleInfo");
	VO_SOURCE_THUMBNAILINFO* pThumbNailInfo = (VO_SOURCE_THUMBNAILINFO *)pParam;
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	VO_S32 nCurrIndex =  m_pTrack->GetNextSyncPoint(m_dwCurrIndex); 
		
	if(-1 == nCurrIndex || nCurrIndex > (VO_S32)m_dwSampleCount)
		return VO_ERR_SOURCE_END;
	uint32 size = m_pTrack->GetSampleData(nCurrIndex, m_pSampleData, m_dwMaxSampleSize);
#ifdef _CHECK_READ
	if (size == ERR_CHECK_READ)
#else //_CHECK_READ
	if (size == 0)
#endif //_CHECK_READ
	{
		uint32 err = m_pReader->GetErrorCode();
		m_pReader->ClearErrorCode();
		switch((VO_S32)err)
		{
		case 0:
			return VO_ERR_SOURCE_END;

		case MPXERR_DATA_DOWNLOADING:
			return VO_ERR_SOURCE_NEEDRETRY;
		}
	}
	if(VO_SOURCE_THUMBNAILMODE_DATA == pThumbNailInfo->uFlag)
		pThumbNailInfo->pSampleData = m_pSampleData;
	
	pThumbNailInfo->uSampleSize = size;
	pThumbNailInfo->ullTimeStamp = m_pTrack->GetSampleTime(nCurrIndex) * TIME_UNIT;
	pThumbNailInfo->ullFilePos = m_pTrack->GetSampleAddress(nCurrIndex);
	pThumbNailInfo->uPrivateData = 0;
	m_dwCurrIndex = nCurrIndex + 1;

	VOLOGR("-GetThumbnailSyncSampleInfo");
	return VO_ERR_SOURCE_OK;
}

VO_U32 	CMp4Track2::GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{

	if(VO_SOURCE_THUMBNAILMODE_INFOONLY == pThumbNailInfo->uFlag)
	{
		
		GetThumbnailSyncSampleInfo(pThumbNailInfo);
	}
	else if(VO_SOURCE_THUMBNAILMODE_DATA == pThumbNailInfo->uFlag)
	{
		SetPos(&pThumbNailInfo->ullTimeStamp);
		GetThumbnailSyncSampleInfo(pThumbNailInfo);
	}
	return VO_ERR_NONE;	
}

//2011/11/16, add by leon, for Eric, getthumbnail slowly.
VO_U32 CMp4Track2::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_PID_SOURCE_THUMBNAILSCANRANGE:
		{
			if(VOTT_VIDEO != m_nType || !pParam)
				return VO_ERR_NOT_IMPLEMENT;

			m_ThumbnallScanCount += *(VO_S32 *)pParam;
			VOLOGR("ThumbnailScanCount:%d", m_ThumbnallScanCount);		
			return VO_ERR_NONE;
		}
	}
	return CBaseTrack::GetParameter(uID, pParam);

}
VO_U32 CMp4Track2::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	if(!IsInUsed()) return VO_ERR_INVALID_ARG;

	VOLOGR("+GetParameter. ID: %d", uID);
	switch (uID)
	{
	case  VO_PID_SOURCE_GETTHUMBNAIL :
		{
			if(VOTT_VIDEO != m_nType)
				return VO_ERR_NOT_IMPLEMENT;
			VO_SOURCE_THUMBNAILINFO* pThumbNailInfo = (VO_SOURCE_THUMBNAILINFO *)pParam;
			return GetThumbNail(pThumbNailInfo);
		}
	case VO_PID_SOURCE_VIDEOPROFILELEVEL:
		{
			if(VOTT_VIDEO != m_nType)
				return VO_ERR_NOT_IMPLEMENT;

			VO_SOURCE_TRACKINFO ti;
			if (VO_ERR_SOURCE_OK == GetInfo(&ti) && ti.Codec != VO_VIDEO_CodingH263)
				break;

			dec_spec_data dsd = {0};
			H263DecSpecStruc h263ds = {0};

			dsd.size	= sizeof(h263ds);
			dsd.buf		= &h263ds;

			if ( dsd.size <  VO_S32(m_pTrack->GetDescriptorSize()) )
				return VO_ERR_OUTPUT_BUFFER_SMALL;

			dsd.size = VO_S32(m_pTrack->GetDescriptorSize());
			MemCopy(dsd.buf, m_pTrack->GetDescriptorData(), dsd.size);

			VO_VIDEO_PROFILELEVEL* pProfileLevel = (VO_VIDEO_PROFILELEVEL*)pParam;
			pProfileLevel->Profile	= VO_VIDEO_PROFILETYPE_MAX;
			pProfileLevel->Level	= VO_VIDEO_LEVELTYPE_MAX;

			switch (h263ds.H263_Profile)
			{
			case 0x00:
			default:
				pProfileLevel->Profile = VO_VIDEO_H263ProfileBaseline;
				break;
			};

			switch (h263ds.H263_Level)
			{
			case 0x0a:
				{
					pProfileLevel->Level = VO_VIDEO_H263Level10;
					break;
				}
			case 0x14:
				{
					pProfileLevel->Level = VO_VIDEO_H263Level20;
					break;
				}
			case 0x1E:
				{
					pProfileLevel->Level = VO_VIDEO_H263Level30;
					break;
				}
			case 0x28:
				{
					pProfileLevel->Level = VO_VIDEO_H263Level40;
					break;
				}
			case 0x2D:
				{
					pProfileLevel->Level = VO_VIDEO_H263Level45;
					break;
				}
			case 0x32:
				{
					pProfileLevel->Level = VO_VIDEO_H263Level50;
					break;
				}
			case 0x3C:
				{
					pProfileLevel->Level = VO_VIDEO_H263Level60;
					break;
				}
			case 0x46:
				{
					pProfileLevel->Level = VO_VIDEO_H263Level70;
					break;
				}
			default:
				break;
			};

			return VO_ERR_NONE;
		}

	case VO_PID_FILE_GETDECSPECDATA:
	{
		dec_spec_data* dsd = (dec_spec_data*) pParam;
		if ( (dsd->size < VO_S32(m_pTrack->GetDescriptorSize())) || (dsd->buf == NULL) )
		{
			dsd->size = m_pTrack->GetDescriptorSize();
			return VO_ERR_OUTPUT_BUFFER_SMALL;
		}
		dsd->size = m_pTrack->GetDescriptorSize();
		MemCopy(dsd->buf, m_pTrack->GetDescriptorData(), dsd->size);
		return VO_ERR_SOURCE_OK;
	}

#if 1 
	case VO_PID_SOURCE_WAVEFORMATEX:
	{
		VO_WAVEFORMATEX* wfx = *(VO_WAVEFORMATEX**)pParam;


#ifdef _SUPPORT_PIFF
		//09/09/2011 add by leon for piff 
		//for wma DescriptorData is WAVEFORMATEX, so do not create WAVEFORMAT
		if(m_pTrack->GetSampleType() == FOURCC2_wma && m_pTrack->IsIsml() )
			wfx = (VO_WAVEFORMATEX*)m_pTrack->GetDescriptorData();
		else
		//09/09/2011 add by leon for piff
#endif
		{
			memset(wfx, 0, VO_WAVEFORMATEX_STRUCTLEN);
			wfx->wFormatTag = m_wFormatTag; //WAVE_FORMAT_PCM;         /**< format type */
			wfx->nChannels = m_pTrack->GetChannelCount();          /**< number of channels (i.e. mono, stereo...) */
			wfx->nSamplesPerSec = m_pTrack->GetSampleRate();     /**< sample rate */
			wfx->wBitsPerSample = m_wBitsPerSample;

			//2012/03/12, Leon, fix quicktime #11375
			if(m_pTrack->IsMov())
			{
				wfx->nAvgBytesPerSec = m_pTrack->GetSampleRate() * m_pTrack->GetChannelCount() * m_pTrack->GetSampleBits() / 8;//m_pTrack->GetBitrate() / 8;
				wfx->nBlockAlign = VO_U16(m_pTrack->GetBytesPerFrame());
			}
			else
			{
				//wfx->cbSize = 0; //!!! sizeof(VO_WAVEFORMATEX); //VO_WAVEFORMATEX_STRUCTLEN;             /**< the count in bytes of the size of */
				wfx->wBitsPerSample = m_wBitsPerSample;     /**< number of bits per sample of mono data */
				wfx->nAvgBytesPerSec = m_pTrack->GetBitrate() / 8;    /**< for buffer estimation */
				wfx->nBlockAlign = wfx->nChannels * wfx->wBitsPerSample / 8;        /**< block size of data */
			}
		}
		return VO_ERR_SOURCE_OK;
	}
#endif

	default:
		break;
	}

	return CBaseTrack::GetParameter(uID, pParam);
}
