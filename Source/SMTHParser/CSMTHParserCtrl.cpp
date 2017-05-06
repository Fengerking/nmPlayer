


//smth-ParseBox_r.cpp

#include "voLog.h"
#include "CSMTHParserCtrl.h"
#include "voType.h"

/*
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

VO_U32 FF =0xff;
#ifdef _VONAMESPACE
}
#endif
*/

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//#define _CONTROLLER_DUMPFILE

#define	EXTEND_DATA_LENGTH_FOR_DRM		1024


#ifdef _CONTROLLER_DUMPFILE
FILE *pDumpFile_a = NULL;
FILE *pDumpFile_v = NULL;
FILE *pDumpFile_s = NULL;
#endif


CSMTHParserCtrl::CSMTHParserCtrl()
:m_pIVSturct(NULL)
,m_pSampleEncyptionBox(NULL)
,m_uSampleEncyptionSize(0)
,m_uTrackID(0)
,m_drmCustomerType(VOSMTH_DRM_COMMON_PlayReady)
{
	m_fpDrmPtr.pUserData = NULL;
	m_fpDrmPtr.FRCB = NULL;

#ifdef _CONTROLLER_DUMPFILE
#ifdef _LINUX
	if( !pDumpFile_a)
		pDumpFile_a = fopen("/sdcard/audio_smth","wb");

	if( !pDumpFile_v)
		pDumpFile_v = fopen("/sdcard/video_smth","wb");
	if( !pDumpFile_s)
		pDumpFile_s = fopen("/sdcard/subtitle_smth","wb");
#elif defined WIN32
	if( !pDumpFile_a)
		pDumpFile_a = fopen("d:/audio_smth","wb");
	if( !pDumpFile_v)
		pDumpFile_v = fopen("d:/video_smth","wb");
	if( !pDumpFile_s)
		pDumpFile_s = fopen("d:/subtitle_smth","wb");

#endif
#endif
	
}

CSMTHParserCtrl::~CSMTHParserCtrl()
{
	if(m_pSampleEncyptionBox){
		delete m_pSampleEncyptionBox;
		m_pSampleEncyptionBox = NULL;
	}

#ifdef _CONTROLLER_DUMPFILE
	if(pDumpFile_a)
		fclose(pDumpFile_a);
	if(pDumpFile_v)
		fclose(pDumpFile_v);
	if(pDumpFile_s)
		fclose(pDumpFile_s);
#endif
	
}

void CSMTHParserCtrl::CheckFor4B(VO_S64 &boxsize,VO_SMTH_BOX *box)
{
	if (boxsize < 9) 
	{   
		m_Fragment.pData += sizeof(VO_U32);  
		m_Fragment.length_t -= sizeof(VO_U32); 
		boxsize -= sizeof(VO_U32); 
	}
}
VO_U32 CSMTHParserCtrl::Open(PARSERPROC pProc, VO_VOID* pUserData)
{
	m_bProcDownloadEnd = VO_FALSE;
	m_pProc		= pProc;
	m_pUserData	= pUserData;
	return VO_ERR_NONE;
}
VO_U32 CSMTHParserCtrl::Close()
{
	return VO_ERR_NONE;//
}
VO_U32 CSMTHParserCtrl::SetParam(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 nRc= VO_ERR_NONE;
	switch(uID)
	{
	case VO_DATASOURCE_PID_DRM_FP_CB:
		{
			VOLOGI("Set DRM CB.(%p)", pParam);
			VO_DATASOURCE_FILEPARSER_DRMCB *pFpDrm  = (VO_DATASOURCE_FILEPARSER_DRMCB *)pParam;
			m_fpDrmPtr.FRCB = pFpDrm->FRCB;
			m_fpDrmPtr.pUserData = pFpDrm->pUserData;
			m_drmCustomerType = VOSMTH_DRM_NEW_DRM ;
		}
		break;
	default:
		VOLOGR("unknow ID");
		nRc = VO_ERR_NOT_IMPLEMENT;
		break;
	}

	return nRc;
}
VO_U32 CSMTHParserCtrl::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	//USE nStreamID to indicate Video or Audio ; 0 for Video ,1 for Audio,3 for stop;

	VOLOGR("+ Process");
	if(m_pIVSturct)
	{
		SAMPLE_ST *_pSample = m_pIVSturct->pSample;		
		if(_pSample)
		{
			for(VO_U32 n = 0; n< m_pIVSturct->nSampleCount; n++)
			{
				if(_pSample->pEntries)	delete []_pSample->pEntries; 
				if(_pSample->pcInitializationVector)	delete []_pSample->pcInitializationVector;
				_pSample ++;
			}
			delete m_pIVSturct->pSample;
		}
		delete m_pIVSturct;
		m_pIVSturct = NULL;
	}

	m_u64Starttime = 0;
	m_u64Starttime = *(VO_S64*)pBuffer->pReserved;
	m_u32StreamID = pBuffer->nStreamID;
	ToParser(pBuffer->pBuf,pBuffer->nBufLen,pBuffer->nStreamID);
	VOLOGR("- Process");
	return VO_ERR_NONE;
}

VO_S32 CSMTHParserCtrl::ParserFirstLevel(VO_SMTH_BOX *box)
{
	//first level
	VOLOGR("+ ParserFirstLevel");
	CHECK_FAIL(GetBoxType(box));	
	switch(box->type)
	{
	case VO_MOOF:
		CHECK_FAIL(ParseBox_MOOF(box));	break;
	case VO_MDAT:
		CHECK_FAIL(ParseBox_MDAT(box)); break;
	case VO_UUID:
		CHECK_FAIL(ParseBox_UUID(box)); break;
	default:return VO_ERR_FAILED;
	}
	VOLOGR("- ParserFirstLevel");
	return VO_ERR_NONE;
}
VO_S32 CSMTHParserCtrl::ParserSecondLevel(VO_SMTH_BOX *box)
{
	//second level
	CHECK_FAIL(GetBoxType(box));	
	switch(box->type)
	{
	case VO_MFHD:
		CHECK_FAIL(ParseBox_MFHD(box)); break;
	case VO_TRAF:
		CHECK_FAIL(ParseBox_TRAF(box)); break;
	case VO_UUID:
		CHECK_FAIL(ParseBox_UUID(box)); break;
	default: return VO_ERR_FAILED;
	}
	return VO_ERR_NONE;
}
VO_S32 CSMTHParserCtrl::ParserThirdLevel(VO_SMTH_BOX *box)
{
	//third level
	CHECK_FAIL(GetBoxType(box));	
	switch(box->type)
	{
	case VO_TFHD:
		CHECK_FAIL(ParseBox_TFHD(box)); break;
	case VO_TRUN:
		CHECK_FAIL(ParseBox_TRUN(box)); break;
	case VO_SDTP:
		CHECK_FAIL(ParseBox_SDTP(box)); break;
	case VO_UUID:
		CHECK_FAIL(ParseBox_UUID(box)); break;
	case VO_UNKNOWN:
		if( box->curSize > 0)
		{
			m_Fragment.pData += box->curSize;
			m_Fragment.length_t -= box->curSize;
		}
		break;
	default: return VO_ERR_NONE;
	}
	return VO_ERR_NONE;
}

VO_S32 CSMTHParserCtrl::ToParser(VO_PBYTE pBuf,VO_U64 bufsize,VO_U32 inputtype)
{
	
	VO_SMTH_FRAGMENT_INFO v;
	VO_SMTH_BOX box;

	m_Fragment.pData = pBuf;
	m_Fragment.length_t = bufsize;
	/*0 video; 1 audio; 2 text;*/
	if( inputtype == 0 )
		m_Fragment.type = VO_PARSER_OT_VIDEO;
	if( inputtype == 1 )
		m_Fragment.type = VO_PARSER_OT_AUDIO;
	if( inputtype == 2 )
		m_Fragment.type = VO_PARSER_OT_TEXT;

	box.fragmentInfo = &v;
	memset(box.fragmentInfo, 0x00, sizeof (VO_SMTH_FRAGMENT_INFO)); 

	//moof
	VO_S32 boxSize ,tmpboxSize;
	boxSize= tmpboxSize =VO_S32(m_Fragment.length_t);
	while (tmpboxSize)
	{
		if(ParserFirstLevel(&box) != VO_ERR_NONE)
			break;
		tmpboxSize = VO_S32(tmpboxSize - (box.curSize + 8));
	}
	box.curSize = VO_S64(boxSize);
// 	CHECK_FAIL(ParserFirstLevel(&box));
// 	//mfra
// 	CHECK_FAIL(ParserFirstLevel(&box));
// 	CHECK_FAIL(ParserFirstLevel(&box));
	VOLOGR("+release IV_STRUCT");
	if(m_pIVSturct)
	{
		SAMPLE_ST *_pSample = m_pIVSturct->pSample;		
		if(_pSample)
		{
			for(VO_U32 n = 0; n< m_pIVSturct->nSampleCount; n++)
			{
				if(_pSample->pEntries)	delete []_pSample->pEntries; 
				if(_pSample->pcInitializationVector)	delete []_pSample->pcInitializationVector;
				_pSample ++;
			}
			delete m_pIVSturct->pSample;
		}
		delete m_pIVSturct;
		m_pIVSturct = NULL;
	}
	VOLOGR("-release IV_STRUCT");
	ToRelease(box.fragmentInfo);
	return VO_ERR_NONE;
}

VO_S32 CSMTHParserCtrl::GetBoxType(VO_SMTH_BOX* box)
{
	VO_U32 size = 0;
	VO_U32 boxType;
	VO_U32 len = sizeof (VO_U32) + sizeof (boxType);

	//4 B for size
	CHECK_FAIL(GetBoxValue(&size));
	//4 B for boxType
	CHECK_FAIL(GetBoxValue(&boxType));

	VOLOGR("boxType 0x%04x,size: 0x%04x",boxType,size);
	switch(boxType)
	{
	case 0x6d6f6f66://moof
		box->type = VO_MOOF;break;
	case 0x6d666864://mfhd
		box->type = VO_MFHD;break;	
	case 0x74726166://traf
		box->type = VO_TRAF;break;
	case 0x74666864://tfhd
		box->type = VO_TFHD;break;
	case 0x7472756e://trun
		box->type = VO_TRUN;break;
	case 0x6d646174://mdat
		box->type = VO_MDAT;break;
	case 0x73647470://sdtp
		box->type = VO_SDTP;break;
	case 0x75756964://uuid
		box->type = VO_UUID;break;
	default: box->type = VO_UNKNOWN;break;
	}

	//if (box->type == VO_UNKNOWN) return VO_ERR_FAILED;

    box->curSize =  size;
	box->curSize -= len;
	return VO_ERR_NONE;
}
VO_S32 CSMTHParserCtrl::GetBoxValue( VO_U8 *pValue,VO_U32 size)
{
	CHECK_FAIL(GetBoxBuf(pValue, size)) ;

	return VO_ERR_NONE;
}
VO_S32 CSMTHParserCtrl::GetBoxValue( VO_U16 *pValue,VO_U32 size)
{
	CHECK_FAIL(GetBoxBuf(pValue, size)) ;
#ifndef __NEW_SWAPENDIAN
	*pValue = (VO_U32) be16toh(*pValue); 
#else
	*pValue = (VO_U32) BSWAP_16(*pValue); 
#endif
	return VO_ERR_NONE;
}

VO_S32 CSMTHParserCtrl::GetBoxValue(VO_U32 *pValue, VO_U32 size) 
{	
	CHECK_FAIL(GetBoxBuf(pValue, size)) ;
#ifndef __NEW_SWAPENDIAN
	*pValue = (VO_U32) be32toh(*pValue); 
#else
	*pValue = (VO_U32) BSWAP_32(*pValue); 
#endif
	return VO_ERR_NONE;
}
VO_S32 CSMTHParserCtrl::GetBoxValue(VO_U64 *pValue, VO_U32 size) 
{	
	CHECK_FAIL(GetBoxBuf(pValue, size)) ;
#ifndef __NEW_SWAPENDIAN
	*pValue = (VO_U64) be64toh(*pValue); 
#else
	*pValue = (VO_U64) BSWAP_64(*pValue); 
#endif
	return VO_ERR_NONE;
}
 //read size bytes from buffer
 VO_S32 CSMTHParserCtrl::GetBoxBuf(VO_VOID *pValue, VO_U32 size)
 {
	 if(m_Fragment.length_t < size * sizeof(VO_CHAR)) return VO_ERR_FAILED;

	 memcpy(pValue,m_Fragment.pData,size);
	 m_Fragment.length_t -= size;
	 if( m_Fragment.length_t >0 ) m_Fragment.pData += size;

	 return VO_ERR_NONE;
 }

 VO_S32 CSMTHParserCtrl::ParseBox_MOOF(VO_SMTH_BOX* box)
 {
	 VO_S32 boxSize ,tmpboxSize;
	 boxSize= tmpboxSize = (VO_S32)box->curSize;
	 while (tmpboxSize)
	 {
		 CHECK_FAIL(ParserSecondLevel(box));
		 tmpboxSize = VO_S32(tmpboxSize - (box->curSize + 8));
	 }
	 box->curSize = VO_S32(boxSize);
	 return VO_ERR_NONE;
 }

VO_S32 CSMTHParserCtrl::ParseBox_MFHD(VO_SMTH_BOX* box)
 {
	 VO_S64 boxsize = box->curSize;
	
	 CheckFor4B(boxsize, box);
	// box->curSize += boxsize;
	 VO_U32 len;
	CHECK_FAIL(GetBoxValue(&len));

	 box->fragmentInfo->index = len;
	 boxsize -= sizeof (len);

	 return VO_ERR_NONE;
 }

VO_S32 CSMTHParserCtrl::ParseBox_TRAF(VO_SMTH_BOX* box)
{
	VO_S32 boxSize ,tmpboxSize;
	boxSize= tmpboxSize =VO_S32(box->curSize);
	while (tmpboxSize)
	{
		CHECK_FAIL(ParserThirdLevel(box));
		tmpboxSize = VO_S32(tmpboxSize - (box->curSize + 8));
	}
	box->curSize = VO_S64(boxSize);
// 	//tfhd
// 	CHECK_FAIL(ParserThirdLevel(box));
// 	//trun
// 	CHECK_FAIL(ParserThirdLevel(box));
// 	//sdtp
// 	CHECK_FAIL(ParserThirdLevel(box));
// 	CHECK_FAIL(ParserThirdLevel(box));

	return VO_ERR_NONE;
}
VO_S32  CSMTHParserCtrl::CheckBoxFlag(VO_SMTH_BOX* box,VO_U32 &setting, VO_U32 flag, VO_U64 id)
{  
	VO_S32 len = sizeof(VO_U32);
	if (flag & id) 
	{
		if(GetBoxValue(&(setting), len)!=VO_ERR_NONE)
			setting = 0;
		else
			return len; 
	}
	else
		setting = 0;
	return 0;
}

VO_S32  CSMTHParserCtrl::CheckBoxFlag(VO_SMTH_BOX* box,VO_U64 &setting,VO_U32 flag, VO_U64 id)
{  
	VO_S32 len = sizeof(VO_U64);
	if (flag & id) 
	{
		if(GetBoxValue(&(setting), len) !=VO_ERR_NONE)
			setting = 0;
		else
			return len; 
	}
	else
		setting = 0;
	return 0;
}

VO_S32 CSMTHParserCtrl::ParseBox_TFHD(VO_SMTH_BOX* box)
{
	VO_S64 boxSize = box->curSize;
	VO_U32 boxFlag;
	CHECK_FAIL(GetBoxValue(&boxFlag));
	boxSize -= sizeof (boxFlag);

	VO_U32 track_id;
	CHECK_FAIL(GetBoxValue(&track_id));
	m_uTrackID = box->fragmentInfo->track_id = track_id;


	VO_U64 setting64;
	VO_U32 setting32;

	boxSize -= CheckBoxFlag(box,setting64,boxFlag,VO_SMTH_TFHD_BASE_DATA_OFFSET_PRESENT);
	box->fragmentInfo->defaults.base_data_offset = setting64;

	boxSize -=CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TFHD_SAMPLE_DESCRIPTION_INDEX_PRESENT);
	box->fragmentInfo->defaults.sample_description_index = setting32;

	boxSize -=CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TFHD_DEFAULT_SAMPLE_DURATION_PRESENT);
	box->fragmentInfo->defaults.default_sample_duration = setting32;

	boxSize -= CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TFHD_DEFAULT_SAMPLE_SIZE_PRESENT);
	box->fragmentInfo->defaults.default_sample_size = setting32;

	boxSize -= CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TFHD_DEFAULT_SAMPLE_FLAGS_PRESENT);
	box->fragmentInfo->defaults.default_sample_flags = setting32;

	return VO_ERR_NONE;
}

VO_S32 CSMTHParserCtrl::ParseBox_TRUN(VO_SMTH_BOX* box)
{
	VO_S64 boxSize = box->curSize;
	VO_U32 boxFlag;

	CHECK_FAIL(GetBoxValue(&boxFlag));

	VO_U32 setting32;
	VO_U32 samplecounts;
	CHECK_FAIL(GetBoxValue( &setting32));
	samplecounts = box->fragmentInfo->sampleno =setting32; 

	boxSize -=CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TFHD_BASE_DATA_OFFSET_PRESENT);
	box->fragmentInfo->data_offset = setting32;

	boxSize -= sizeof (setting32) + sizeof (samplecounts);
	setting32 = box->fragmentInfo->settings ;
	boxSize -=CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TRUN_FIRST_SAMPLE_FLAGS_PRESENT);
	box->fragmentInfo->settings = setting32;

	if(samplecounts > 0)
	{	
		setting32 = 0;
		box->fragmentInfo->samples = new VO_SMTH_SAMPLE_INFO[samplecounts * sizeof (VO_SMTH_SAMPLE_INFO)];
		memset(box->fragmentInfo->samples, 0x00, samplecounts*sizeof(VO_SMTH_SAMPLE_INFO));
		VO_SMTH_SAMPLE_INFO* tmp_sample = box->fragmentInfo->samples;
		if(!tmp_sample) return VO_ERR_OUTOF_MEMORY;


		for( VO_U32 i = 0; i < samplecounts; i++)
		{
			tmp_sample[i].nKey = 0;

			boxSize -=CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TRUN_SAMPLE_DURATION_PRESENT);
			if((tmp_sample[i].sample_duration =setting32)==0)
				 tmp_sample[i].sample_duration = box->fragmentInfo->defaults.default_sample_duration;
			
			boxSize -=CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TRUN_SAMPLE_SIZE_PRESENT);
			if((tmp_sample[i].sample_size = setting32)==0)
				tmp_sample[i].sample_size = box->fragmentInfo->defaults.default_sample_size;

			boxSize -=CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TRUN_SAMPLE_FLAGS_PRESENT);
			if((tmp_sample[i].sample_flags = setting32)==0)
				tmp_sample[i].sample_flags = box->fragmentInfo->defaults.default_sample_flags;

			boxSize -=CheckBoxFlag(box, setting32,boxFlag,VO_SMTH_TRUN_SAMPLE_COMPOSITION_TIME_OFFSET_PRESENT);
			tmp_sample[i].sample_composition_time_offset = setting32;
			VOLOGR("index %d Sample_composition_time_offset %d",i, tmp_sample[i].sample_composition_time_offset);

		}
		tmp_sample[0].nKey = 1;

	}

	return VO_ERR_NONE;
}

VO_S32 CSMTHParserCtrl::ParseBox_SDTP(VO_SMTH_BOX* box)
{
	/*skip sdtp box */
//	VO_S64 boxSize = box->curSize;
	VO_U32 boxFlag;
	CHECK_FAIL(GetBoxValue(&boxFlag));
	VO_U8 setting;
	for(VO_U32 i =0 ;i< box->fragmentInfo->sampleno ;i++)
	{
		CHECK_FAIL(GetBoxValue(&setting));
		box->fragmentInfo->samples[i].nKey = setting & 0x20 ? 1:0; 
	}


	//m_Fragment.pData += box->curSize;
	//m_Fragment.length_t -= box->curSize;

	return VO_ERR_NONE;
}



VO_S32 CSMTHParserCtrl::ParseBox_MDAT(VO_SMTH_BOX* box)
{
	/**get every sample **/
	SAMPLE_ST *_pSampleST = NULL;
	VO_U32 _nIVsize = 0;
	if(m_pIVSturct && m_pIVSturct->pSample)
	{
		_pSampleST = m_pIVSturct->pSample;
		_nIVsize = m_pIVSturct->nIV_Size;
	}


	VO_U64 starttime = m_u64Starttime;
	for(VO_U32 i = 0 ;i< box->fragmentInfo->sampleno; ++i)
	{
		VO_U32 size = box->fragmentInfo->samples[i].sample_size;
		
		if(size > 1024 * 100 )
		{
			VOLOGI(" sample size is a big value:%d", size);	
		}

		VO_U32	EXsize = size;
		if( m_drmCustomerType == VOSMTH_DRM_NEW_DRM && m_fpDrmPtr.FRCB &&m_fpDrmPtr.pUserData)
		{
			EXsize += EXTEND_DATA_LENGTH_FOR_DRM;
		}
		
		VO_BYTE *outputbuf =new VO_BYTE[EXsize];

		if (!outputbuf) return VO_ERR_OUTOF_MEMORY;

		if (GetBoxBuf(outputbuf, size ) != VO_ERR_NONE)
		{ 
			delete []outputbuf;
			return VO_ERR_FAILED;
		}
		VO_SOURCE_SAMPLE sample;
		sample.Buffer = outputbuf;
		sample.Size =  size;
		sample.Time = starttime + box->fragmentInfo->samples[i].sample_composition_time_offset;
		starttime +=  box->fragmentInfo->samples[i].sample_duration;

		if( m_drmCustomerType == VOSMTH_DRM_NEW_DRM && m_fpDrmPtr.FRCB &&m_fpDrmPtr.pUserData)
		{
			VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO piff_info;
			piff_info.pSampleEncryptionBox = m_pSampleEncyptionBox;
			piff_info.uSampleEncryptionBoxSize = m_uSampleEncyptionSize;
			piff_info.uSampleIndex = i;
			piff_info.uTrackId = m_uTrackID;
			//temporarily set the EXsize by sample.Size
			EXsize = sample.Size;
			if(VO_ERR_NONE == m_fpDrmPtr.FRCB(m_fpDrmPtr.pUserData, sample.Buffer, sample.Size, 0, &EXsize, VO_DRM2DATATYPE_MEDIASAMPLE, VO_DRM2_DATAINFO_SAMPLEDECYPTION_BOX, &piff_info)){
				//set the sampe size again.
				sample.Size = EXsize;
			}
//			m_pNewDrmApi->DataProcess_FR(m_pNewDrmApi->hHandle, sample.Buffer, sample.Size, 0, &sample.Size, VO_DRM2DATATYPE_MEDIASAMPLE, &piff_info);
			VOLOGI("Call Drm Process");
		}
		sample.Size |= ( box->fragmentInfo->samples[i].nKey == 1? 0x80000000:0);
		VO_PARSER_OUTPUT_BUFFER output_buf;
		output_buf.nStreamID = m_uTrackID;
		output_buf.nType = m_Fragment.type;

		//2010-11-19
		VOLOGI(" Sample Size:%lu, type=0x%08x, offset=%ld, duration=%lu; time: %lld, 0x%08x", 
			sample.Size, output_buf.nType, box->fragmentInfo->samples[i].sample_composition_time_offset , box->fragmentInfo->samples[i].sample_duration, sample.Time, sample.Time);
		IssueParseResult(output_buf.nType, &sample);

		/*VO_SMTH_FRAME_BUFFER frame_buf;
		frame_buf.nSize = size;
		frame_buf.pData = outputbuf;
		frame_buf.nStartTime = starttime;
		frame_buf.nEndTime = frame_buf.nStartTime + box->fragmentInfo->samples[i].sample_duration;

		starttime = frame_buf.nEndTime;
		frame_buf.nStartTime += box->fragmentInfo->samples[i].sample_composition_time_offset;

		frame_buf.nPos = VO_SMTH_FRAME_POS_WHOLE;
		frame_buf.nCodecType = 0 ;
		frame_buf.nFrameType = box->fragmentInfo->samples[i].nKey ;

		if( m_drmCustomerType == VOSMTH_DRM_NEW_DRM)
		{
			VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO piff_info;
			piff_info.pSampleEncryptionBox = m_pSampleEncyptionBox;
			piff_info.uSampleEncryptionBoxSize = m_uSampleEncyptionSize;
			piff_info.uSampleIndex = i;
			piff_info.uTrackId = m_uTrackID;
			m_pNewDrmApi->DataProcess_FR(m_pNewDrmApi->hHandle, frame_buf.pData,frame_buf.nSize, 0, &frame_buf.nSize, VO_DRM2DATATYPE_MEDIASAMPLE, &piff_info);
			VOLOGI("Call Drm Process");
		}
		VO_PARSER_OUTPUT_BUFFER output_buf;
		output_buf.nStreamID = m_uTrackID;
		output_buf.nType = m_Fragment.type;
		
		//2010-11-19
		VOLOGR(" frameSize:%d ", frame_buf.nSize );
		IssueParseResult(output_buf.nType, &frame_buf);
		*/

		
		
		delete []outputbuf;
		outputbuf = NULL;
	
	}
	
	return VO_ERR_NONE;
}

VO_S32 CSMTHParserCtrl::ParseBox_UUID(VO_SMTH_BOX* box )
{
	VOLOGR("+ParseBox_UUID curlength%lld",m_Fragment.length_t);
	if(0)//! m_pDrmcallback )//donot parser UUID
	{
		/*skip uuid box */
		m_Fragment.pData += box->curSize;
		m_Fragment.length_t -= box->curSize;
	}
	else
	{
		VO_S64 boxSize = box->curSize;
		VO_BYTE uuid[16];
		memset(uuid,0x00,sizeof(uuid));
		int uuidsize = sizeof(uuid);
		VOLOGR("buffer before 0x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",m_Fragment.pData[0],m_Fragment.pData[1],m_Fragment.pData[2],m_Fragment.pData[3],m_Fragment.pData[4],m_Fragment.pData[5]
																				,m_Fragment.pData[6],m_Fragment.pData[7],m_Fragment.pData[8],m_Fragment.pData[9],m_Fragment.pData[10],m_Fragment.pData[11]
																				,m_Fragment.pData[12],m_Fragment.pData[13],m_Fragment.pData[14],m_Fragment.pData[15]);
	/*	memcpy(uuid,m_Fragment.pData,uuidsize);
		m_Fragment.pData += uuidsize;
		m_Fragment.length_t -= uuidsize;*/
		if(GetBoxBuf(uuid,uuidsize) != VO_ERR_NONE)
		{
			return VO_ERR_FAILED;
		}
		boxSize -= uuidsize;
		VOLOGR("UUID ProtectHeader 0x%x%x%x%x%x%x",uuid[0],uuid[1],uuid[2],uuid[3],uuid[4],uuid[5]);

		VOLOGR("buffer after 0x%x%x%x%x%x%x",m_Fragment.pData[0],m_Fragment.pData[1],m_Fragment.pData[2],m_Fragment.pData[3],m_Fragment.pData[4],m_Fragment.pData[5]);

		if (memcmp(uuid,PROTECTION_HEADER_UUID,uuidsize) == 0)
		{
			/*skip uuid box */
			m_Fragment.pData += box->curSize;
			m_Fragment.length_t -= box->curSize;//
		}
		else if (memcmp(uuid, SAMPLE_ENCRYTION_UUID, uuidsize) == 0)
		{
			VOLOGR("It is SAMPLE_ENCRYTION_UUID");
			if( m_drmCustomerType ==  VOSMTH_DRM_NEW_DRM)
			{
				if(m_pSampleEncyptionBox){
					delete m_pSampleEncyptionBox;
					m_pSampleEncyptionBox = NULL;
				}

				m_pSampleEncyptionBox = new VO_BYTE[VO_U32(box->curSize+ 8 +1)];
				memset(m_pSampleEncyptionBox, 0x00, VO_U32(box->curSize+8 +1));
				m_uSampleEncyptionSize = 0;
				m_Fragment.pData -= uuidsize + 8;
				m_Fragment.length_t += uuidsize +8;
				if(GetBoxBuf(m_pSampleEncyptionBox,VO_U32(box->curSize +8)) != VO_ERR_NONE)
				{
					return VO_ERR_FAILED;
				}
				m_uSampleEncyptionSize = VO_U32( box->curSize + 8);
			}
		}
		else
		{
			/*skip uuid box */
			m_Fragment.pData += boxSize;
			m_Fragment.length_t -= boxSize;
		}
	}
	VOLOGR("-ParseBox_UUID");
	return VO_ERR_NONE;
}
VO_VOID CSMTHParserCtrl::ToRelease(VO_SMTH_FRAGMENT_INFO *f)
{
	if (f->data) free(f->data);
	if (f->samples) delete []f->samples;
	f->data = NULL;
	f->samples = NULL;
}


VO_VOID	CSMTHParserCtrl::IssueParseResult(VO_U32 nType, VO_VOID* pValue, VO_VOID* pReserved)
{
	if (m_pProc)
	{
		VO_PARSER_OUTPUT_BUFFER out;
		out.nStreamID	= m_uTrackID;//m_u32StreamID;
		out.nType		= nType;
		out.pOutputData	= pValue;
		out.pUserData	= m_pUserData;
		out.pReserved	= pReserved;


#ifdef _CONTROLLER_DUMPFILE

		VO_SOURCE_SAMPLE* ptr_sample = (VO_SOURCE_SAMPLE*)pValue;
		if(pDumpFile_v && nType == VO_PARSER_OT_VIDEO )
		{
			fwrite( ptr_sample->Buffer, 1, ptr_sample->Size&(~0x80000000), pDumpFile_v);
		}
		else if(pDumpFile_a && nType == VO_PARSER_OT_AUDIO )
		{
			fwrite( ptr_sample->Buffer, 1, ptr_sample->Size&(~0x80000000), pDumpFile_a);
		}
		else if(pDumpFile_a && nType == VO_PARSER_OT_TEXT )
		{
			fwrite( ptr_sample->Buffer, 1, ptr_sample->Size&(~0x80000000), pDumpFile_s);
		}
#endif


		m_pProc(&out);
	}
}