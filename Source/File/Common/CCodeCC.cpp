
#include "CCodeCC.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

unsigned int	CCodeCC::GetCodecByCC(VO_SOURCE2_TRACK_TYPE nType, unsigned int uCC)
{
	VOLOGI(" + GetCodecByCC.");

	unsigned int codectype = 0; 
	if(nType == VO_SOURCE2_TT_AUDIO)
		codectype = fCodecGetAudioCodec(uCC);
	if(nType == VO_SOURCE2_TT_VIDEO)
		codectype = fCodecGetVideoCodec(uCC);

	VOLOGI(" - GetCodecByCC. Type:%d, codec:%d",nType, codectype);

	return codectype;
}


unsigned int	CCodeCC::GetCodecByCC(VO_SOURCE_TRACKTYPE nType, unsigned int uCC)
{
	VOLOGI(" + GetCodecByCC.");

	unsigned int codectype = 0; 
	if(nType == VO_SOURCE_TT_AUDIO)
		codectype = fCodecGetAudioCodec(uCC);
	if(nType == VO_SOURCE_TT_VIDEO)
		codectype = fCodecGetVideoCodec(uCC);

	VOLOGI(" - GetCodecByCC. Type:%d, codec:%d",nType, codectype);

	return codectype;
}
unsigned int CCodeCC::GetCC(const char *cType)
{
	unsigned int b = 0;
	unsigned char pType[128];
	memcpy(pType, cType, strlen(cType));
	pType[strlen(cType)] = '\0';
	VOLOGI("CC1:%s",pType);
	/*´óÐ´cc×ª»»Ð¡Ð´cc*/
	strlwr((char*)pType);
	unsigned char *p =(unsigned char*) pType;
	VOLOGI("CC2:%s",pType);
	/*get 4 Byte ahead*/
	int i = 4;//strlen(cType) -1;
	while(*p && i)
	{
		b |=( (int)*p) << (8 * (i-1));
		p++;
		i --;
	}
	VOLOGI("CC3:0x%08x",b);
	return GetCC(b);
}
unsigned int CCodeCC::GetCC(unsigned int uType)
{
	VOLOGI("+ GetCC(unsigned int uType)");

// 	unsigned long long llType;
// 	memcpy((void*)&llType, cType, sizeof(llType));
	unsigned int uCC = 0;
	switch(uType)
	{
	case FOURCC2_mp4a://0x6d703461
		{
			/*
			MemStream ms(m_pTrack->GetDescriptorData(), m_pTrack->GetDescriptorSize());
			ReaderMSB r(&ms);
			ESDescriptor* ed = (ESDescriptor*)Descriptor::Load(r);
			if (ed == NULL)
			{
				uCC = FCC(uType);
				break;
			}
			DecoderConfigDescriptor* dcd = (DecoderConfigDescriptor*)ed->GetChildByTag(DecoderConfigDescrTag);
			if(dcd)
			{
				DecoderSpecificInfo* dsi = (DecoderSpecificInfo*)dcd->GetChildByTag(DecSpecificInfoTag);
				if((0x40 == dcd->objectTypeIndication) || (0x67 == dcd->objectTypeIndication))
					uCC = AudioFlag_ISOMPEG4_AAC;
				else if(0xE1 == dcd->objectTypeIndication)
					uCC = AudioFlag_QCELP;
				else if(0x6B == dcd->objectTypeIndication || 0x69 == dcd->objectTypeIndication)
					uCC = AudioFlag_MP3;
				else if (0xDD == dcd->objectTypeIndication) //211, OGG
					uCC = AudioFlag_OGG_1; //?? 9/13/2011, Jason
				else
					uCC = FCC(uType);
			}
			else
				uCC = FCC(uType);

			delete ed;*/
		}
		break;

	case FOURCC2_samr:
		uCC = AudioFlag_AMR_NB;
		break;

	case FOURCC2_sawb:
		uCC = AudioFlag_AMR_WB;
		break;

	case FOURCC2_sawp:
		uCC = AudioFlag_AMR_WB_PLUS;
		break;

	case FOURCC2_sevc:  //4/28/2009, Jason
	case FOURCC2_evrc: //EVRC, 4/11/2011, Jason
		uCC = AudioFlag_EVRC;
		break;

	case FOURCC2__mp3:
		uCC = AudioFlag_MP3;
		break;

	case FOURCC2_ac_3:
		uCC = AudioFlag_AC3;
		break;

	case FOURCC2_EC_3:
	case FOURCC2_ec_3: //eAC3, East, 2010/03/09
		uCC = AudioFlag_EAC3;
		break;
	case FOURCC2_aacl:
		uCC = AudioFlag_AAC;

	case FOURCC2_mp4v:
		{
	/*		MemStream ms(m_pTrack->GetDescriptorData(), m_pTrack->GetDescriptorSize());
			ReaderMSB r(&ms);
			ESDescriptor* ed = (ESDescriptor*)Descriptor::Load(r);
			if (ed == NULL)
			{
				uCC = FCC(uType);
				break;
			}
			DecoderConfigDescriptor* dcd = (DecoderConfigDescriptor*)ed->GetChildByTag(DecoderConfigDescrTag);
			if(dcd)
			{
				DecoderSpecificInfo* dsi = (DecoderSpecificInfo*)dcd->GetChildByTag(DecSpecificInfoTag);
				if(0x20 == dcd->objectTypeIndication)	//MPEG4
					uCC = FOURCC2_MP4V;
				else if(0x21 == dcd->objectTypeIndication)
					uCC = FOURCC2_H264;
				else if(0x60 <= dcd->objectTypeIndication && 0x65 >= dcd->objectTypeIndication)	//0x60 - 0x65
					uCC = FOURCC2_MPEG;
				else
					uCC = FCC(uType);
			}
			else
				uCC = FCC(uType);

			delete ed;*/
		}
		break;

	case FOURCC2_h264:
	case FOURCC2_s264:
		uCC = FOURCC_H264;
		break;

	case FOURCC2_avc1:
		uCC = FOURCC_AVC1;
		break;

	case FOURCC2_s263:
	case FOURCC2_H263: //10/13/2008, Jason
		uCC = FOURCC_H263;
		break;
		
	case FOURCC2_jpeg://05/02//2012, Aiven 
	case FOURCC2_JPEG:
		uCC = FOURCC_MJPG;
		break;		

	case FOURCC2_alac:
		uCC = FOURCC_ALAC;
		break;

	case FOURCC2_sqcp:
		uCC = AudioFlag_QCELP;
		break;

	case 0x6d732000: // 'ms ', AC3? 6/20/2011, Jason
		uCC = AudioFlag_AC3; //AudioFlag_MS_ADPCM; // ???
		break;
	case 0x6d730011: // 'ms ', 2012/3/9, add by Leon
		uCC = AudioFlag_IMA_ADPCM; 
		break;

	case 0x6d730055: // 7/4/2011
		uCC = AudioFlag_MP3;
		break;

	case FOURCC2_ulaw: //6/22/2011, Jason
		uCC= AudioFlag_MS_MULAW;
		break;
	case FOURCC2_sowt://8/25/2011, Leon
	case FOURCC2_raw ://11/24/2011.leon
		uCC= AudioFlag_MS_PCM;
		break;
	case FOURCC2_alaw: //6/22/2011, Jason 
		uCC= AudioFlag_MS_ALAW;
		break;
	
	case FOURCC2_wmap:
	case FOURCC2_wma ://09/01/2011, Leon 
		uCC = AudioFlag_WMA_PRO_V9;
		break;
	case FOURCC2_vc_1://09/01//2011, Leon 
		uCC = FOURCC_WVC1;
		break;
	default:
		uCC = FCC(uType);
		break;
	}
	VOLOGI("- GetCC. 0x%08x",uCC);
	return uCC;
}
