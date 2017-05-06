	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		fCC.h

	Contains:	fourcc and twocc define header file

	Written by:	East Zhou

	Change History (most recent first):
	2009-03-25		East		Create file

*******************************************************************************/

#ifndef __fCC_H__
#define __fCC_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifndef mmioFOURCC
#define mmioFOURCC(ch0, ch1, ch2, ch3)\
	((VO_U32)(VO_U8)(ch0) | ((VO_U32)(VO_U8)(ch1) << 8) |\
	((VO_U32)(VO_U8)(ch2) << 16) | ((VO_U32)(VO_U8)(ch3) << 24 ))
#endif	//mmioFOURCC
//////////////////////////////////////////////////////////////////////////
//Video FOURCC
//////////////////////////////////////////////////////////////////////////
//XviD(XVID, XVIX)
#define FOURCC_XVID					mmioFOURCC('X', 'V', 'I', 'D')
#define FOURCC_xvid					mmioFOURCC('x', 'v', 'i', 'd')
#define FOURCC_XVIX					mmioFOURCC('X', 'V', 'I', 'X')
#define FOURCC_XviD					mmioFOURCC('X', 'v', 'i', 'D')

//DivX 3(DIV3, DIV4, DIV5, DIV6, AP41, COL1, DVX3, MPG3)
#define FOURCC_div3					mmioFOURCC('d', 'i', 'v', '3')
#define FOURCC_DIV3					mmioFOURCC('D', 'I', 'V', '3')
#define FOURCC_DIV4					mmioFOURCC('D', 'I', 'V', '4')
#define FOURCC_DIV5					mmioFOURCC('D', 'I', 'V', '5')
#define FOURCC_DIV6					mmioFOURCC('D', 'I', 'V', '6')
#define FOURCC_AP41					mmioFOURCC('A', 'P', '4', '1')
#define FOURCC_COL1					mmioFOURCC('C', 'O', 'L', '1')
#define FOURCC_DVX3					mmioFOURCC('D', 'V', 'X', '3')
#define FOURCC_MPG3					mmioFOURCC('M', 'P', 'G', '3')

//DivX 4(DIVX, MP4S, MP4V, M4S2)
#define FOURCC_DIVX					mmioFOURCC('D', 'I', 'V', 'X')
#define FOURCC_divx					mmioFOURCC('d', 'i', 'v', 'x')
#define FOURCC_MP4S					mmioFOURCC('M', 'P', '4', 'S')
#define FOURCC_MP4V					mmioFOURCC('M', 'P', '4', 'V')
#define FOURCC_mp4v					mmioFOURCC('m', 'p', '4', 'v')
#define FOURCC_M4S2					mmioFOURCC('M', '4', 'S', '2')

//DivX 5(DX50, BLZ0, DXGM)
#define FOURCC_DX50					mmioFOURCC('D', 'X', '5', '0')
#define FOURCC_BLZ0					mmioFOURCC('B', 'L', 'Z', '0')
#define FOURCC_DXGM					mmioFOURCC('D', 'X', 'G', 'M')

//Other MPEG4(3IV2, 3IVX, RMP4, DM4V, WV1F, FMP4, HDX4, DVX1)
#define FOURCC_3IV2					mmioFOURCC('3', 'I', 'V', '2')
#define FOURCC_3IVX					mmioFOURCC('3', 'I', 'V', 'X')
#define FOURCC_RMP4					mmioFOURCC('R', 'M', 'P', '4')
#define FOURCC_DM4V					mmioFOURCC('D', 'M', '4', 'V')
#define FOURCC_WV1F					mmioFOURCC('W', 'V', '1', 'F')
#define FOURCC_FMP4					mmioFOURCC('F', 'M', 'P', '4')
#define FOURCC_HDX4					mmioFOURCC('H', 'D', 'X', '4')
#define FOURCC_DVX1					mmioFOURCC('D', 'V', 'X', '1')	//Lucent DV Codec
#define FOURCC_MPV4					mmioFOURCC('M', 'P', 'V', '4')	//Not Standard, cann't find any information!!
#define FOURCC_SEDG					mmioFOURCC('S', 'E', 'D', 'G')	//Samsung MPEG-4

//MP43
#define FOURCC_MP43					mmioFOURCC('M', 'P', '4', '3')

//MP42(MP42, DIV2)
#define FOURCC_MP42					mmioFOURCC('M', 'P', '4', '2')
#define FOURCC_DIV2					mmioFOURCC('D', 'I', 'V', '2')

//MP41(MP41, MPG4, DIV1)
#define FOURCC_MP41					mmioFOURCC('M', 'P', '4', '1')
#define FOURCC_MPG4					mmioFOURCC('M', 'P', 'G', '4')
#define FOURCC_DIV1					mmioFOURCC('D', 'I', 'V', '1')

//Screen Video(MSS1)
#define FOURCC_MSS1					mmioFOURCC('M', 'S', 'S', '1')

//H263(H263, D263, L263, M263, X263)
#define FOURCC_h263					mmioFOURCC('h', '2', '6', '3')
#define FOURCC_H263					mmioFOURCC('H', '2', '6', '3')
#define FOURCC_D263					mmioFOURCC('D', '2', '6', '3')
#define FOURCC_L263					mmioFOURCC('L', '2', '6', '3')
#define FOURCC_M263					mmioFOURCC('M', '2', '6', '3')
#define FOURCC_X263					mmioFOURCC('X', '2', '6', '3')

//S263
#define FOURCC_S263					mmioFOURCC('S', '2', '6', '3')

//H264(H264, X264, VSSH, DAVC, AVC1)
#define FOURCC_H264					mmioFOURCC('H', '2', '6', '4')
#define FOURCC_h264					mmioFOURCC('h', '2', '6', '4')
#define FOURCC_X264					mmioFOURCC('X', '2', '6', '4')
#define FOURCC_VSSH					mmioFOURCC('V', 'S', 'S', 'H')
#define FOURCC_DAVC					mmioFOURCC('D', 'A', 'V', 'C')
#define FOURCC_AVC1					mmioFOURCC('A', 'V', 'C', '1')
#define FOURCC_avc1					mmioFOURCC('a', 'v', 'c', '1')

//H265
#define FOURCC_h265					mmioFOURCC('H', '2', '6', '5')
#define FOURCC_HVC1                 mmioFOURCC('H', 'V', 'C', '1')
#define FOURCC_hvc1                 mmioFOURCC('h', 'v', 'c', '1')
#define FOURCC_hev1					mmioFOURCC('h', 'e', 'v', '1')

//MPEG(MPEG, MPG1, MPG2, 1001, 1002)
#define FOURCC_MPEG					mmioFOURCC('M', 'P', 'E', 'G')
#define FOURCC_MPG1					mmioFOURCC('M', 'P', 'G', '1')
#define FOURCC_mpg1					mmioFOURCC('m', 'p', 'g', '1')
#define FOURCC_MPG2					mmioFOURCC('M', 'P', 'G', '2')
#define FOURCC_mpg2					mmioFOURCC('m', 'p', 'g', '2')

//MJPEG(MJPG, LJPG)
#define FOURCC_MJPG					mmioFOURCC('M', 'J', 'P', 'G')
#define FOURCC_LJPG					mmioFOURCC('L', 'J', 'P', 'G')

//WMV
#define FOURCC_WMV1					mmioFOURCC('W', 'M', 'V', '1')
#define FOURCC_WMV2					mmioFOURCC('W', 'M', 'V', '2')
#define FOURCC_WMV3					mmioFOURCC('W', 'M', 'V', '3')
#define FOURCC_wmv3					mmioFOURCC('w', 'm', 'v', '3')
#define FOURCC_WMVA					mmioFOURCC('W', 'M', 'V', 'A')
#define FOURCC_WMVP					mmioFOURCC('W', 'M', 'V', 'P')		//Picture
#define FOURCC_WVC1					mmioFOURCC('W', 'V', 'C', '1')
#define FOURCC_wvc1					mmioFOURCC('w', 'v', 'c', '1')
#define FOURCC_WVP2					mmioFOURCC('W', 'V', 'P', '2')

//On2 VP6
#define FOURCC_VP60					mmioFOURCC('V', 'P', '6', '0')
#define FOURCC_VP61					mmioFOURCC('V', 'P', '6', '1')
#define FOURCC_VP62					mmioFOURCC('V', 'P', '6', '2')
#define FOURCC_VP6F					mmioFOURCC('V', 'P', '6', 'F')
#define FOURCC_VP70					mmioFOURCC('V', 'P', '7', '0')
#define FOURCC_FLV4					mmioFOURCC('F', 'L', 'V', '4')

//On2 VP8
#define FOURCC_VP80					mmioFOURCC('V', 'P', '8', '0')

//S263 in flv
#define FOURCC_FLV1					mmioFOURCC('F', 'L', 'V', '1')

//apple lossless audio codec
#define FOURCC_ALAC					mmioFOURCC('A', 'L', 'A', 'C')

//VisualOn defined REAL
#define FOURCC_REAL					mmioFOURCC('R', 'E', 'A', 'L')


//////////////////////////////////////////////////////////////////////////
//Audio Format Tag
//reference doc: http://wiki.multimedia.cx/index.php?title=TwoCC
//////////////////////////////////////////////////////////////////////////
//0x0000 Microsoft Unknown Wave Format 
#define AudioFlag_MS_UNKNOWN		0x0000

//0x0001 Microsoft PCM Format 
#define AudioFlag_MS_PCM			0x0001

//0x0002 Microsoft ADPCM Format 
#define AudioFlag_MS_ADPCM			0x0002

//0x0003 IEEE Float 
#define AudioFlag_IEEE_Float		0x0003

//0x0004 Compaq Computer VSELP (codec for Windows CE 2.0 devices) 

//0x0005 IBM CVSD 

//0x0006 Microsoft ALAW (CCITT A-Law) 
#define AudioFlag_MS_ALAW			0x0006

//0x0007 Microsoft MULAW (CCITT u-Law) 
#define AudioFlag_MS_MULAW			0x0007

//0x0008 Microsoft DTS 
#define AudioFlag_MS_DTS			0x0008

//0x0009 Microsoft DRM 
#define AudioFlag_MS_DRM			0x0009

//0x000A Microsoft WMSpeech 
#define AudioFlag_MS_WM_SPEECH		0x000A

//0x000B Microsoft Windows Media RT Voice 
#define AudioFlag_MS_WM_RT_VOICE	0x000B

//0x0010 OKI ADPCM 

//0x0011 Intel DVI ADPCM (IMA ADPCM) 
#define AudioFlag_IMA_ADPCM			0x0011

//0x0012 Videologic MediaSpace ADPCM 

//0x0013 Sierra Semiconductor ADPCM 

//0x0014 Antex Electronics G.723 ADPCM 

//0x0015 DSP Solutions DigiSTD 

//0x0016 DSP Solutions DigiFIX 

//0x0017 Dialogic OKI ADPCM 

//0x0018 MediaVision ADPCM 

//0x0019 Hewlett-Packard CU codec 

//0x001A Hewlett-Packard HP DYNAMIC VOICE 

//0x0020 Yamaha ADPCM 

//0x0021 Speech Compression SONARC 

//0x0022 DSP Group True Speech 

//0x0023 Echo Speech EchoSC1 

//0x0024 AudioFile AF36 

//0x0025 Audio Processing Technology APTX 

//0x0026 AudioFile AF10 

//0x0027 Prosody 1612 codec for CTI Speech Card 

//0x0028 Merging Technologies S.A. LRC 

//0x0030 Dolby Labs AC2 
#define AudioFlag_AC2				0x0030

//0x0031 Microsoft GSM 6.10 
#define AudioFlag_MS_GSM_610		0x0031

//0x0032 MSNAudio 
#define AudioFlag_MSN_AUDIO			0x0032

//0x0033 Antex Electronics ADPCME 

//0x0034 Control Resources VQLPC 

//0x0035 DSP Solutions DigiREAL 

//0x0036 DSP Solutions DigiADPCM 

//0x0037 Control Resources CR10 

//0x0038 Natural MicroSystems VBXADPCM 

//0x0039 Roland RDAC (Crystal Semiconductor IMA ADPCM) 

//0x003A Echo Speech EchoSC3 

//0x003B Rockwell ADPCM 

//0x003C Rockwell Digit LK 

//0x003D Xebec Multimedia Solutions 

//0x0040 Antex Electronics G.721 ADPCM 

//0x0041 Antex Electronics G.728 CELP 

//0x0042 Microsoft GSM723 
#define AudioFlag_MS_GSM_723		0x0042

//0x0043 IBM AVC ADPCM 

//0x0045 ITU-T G.726 ADPCM 
#define AudioFlag_IT_G726_ADPCM		0x0045

//0x0050 MPEG-1 layer 1, 2 
#define AudioFlag_MP1				0x0050

//0x0052 InSoft RT24 (ACM codec is an alternative codec) 

//0x0053 InSoft PAC 

//0x0055 MPEG-1 Layer 3 (MP3) 
#define AudioFlag_MP3				0x0055

//0x0057 AMR-NB 
#define AudioFlag_AMR_NB			0x0057

//0x0058 AMR-WB 
#define AudioFlag_AMR_WB			0x0058

//0x0059 Lucent G.723 

//0x0060 Cirrus Logic 

//0x0061 ESS Technology ESPCM / Duck DK4 ADPCM 

//0x0062 Voxware file-mode codec / Duck DK3 ADPCM 

//0x0063 Canopus Atrac 

//0x0064 APICOM G.726 ADPCM 

//0x0065 APICOM G.722 ADPCM 

//0x0066 Microsoft DSAT 
#define AudioFlag_MS_DSAT			0x0066

//0x0067 Microsoft DSAT Display 
#define AudioFlag_MS_DSAT_DISPLAY	0x0067

//0x0069 Voxware Byte Aligned (bitstream-mode codec) 

//0x0070 Voxware AC8 (Lernout & Hauspie CELP 4.8 kbps) 

//0x0071 Voxware AC10 (Lernout & Hauspie CBS 8kbps) 

//0x0072 Voxware AC16 (Lernout & Hauspie CBS 12kbps) 

//0x0073 Voxware AC20 (Lernout & Hauspie CBS 16kbps) 

//0x0074 Voxware MetaVoice (file and stream oriented) 

//0x0075 Voxware MetaSound (file and stream oriented) 

//0x0076 Voxware RT29HW 

//0x0077 Voxware VR12 

//0x0078 Voxware VR18 

//0x0079 Voxware TQ40 

//0x007A Voxware SC3 

//0x007B Voxware SC3 

//0x0080 Softsound 

//0x0081 Voxware TQ60 

//0x0082 Microsoft MSRT24 (ACM codec is an alternative codec) 
#define AudioFlag_MS_MSRT24			0x0067

//0x0083 AT&T Labs G.729A 

//0x0084 Motion Pixels MVI MV12 

//0x0085 DataFusion Systems G.726 

//0x0086 DataFusion Systems GSM610 

//0x0088 Iterated Systems ISIAudio 

//0x0089 Onlive 

//0x008A Multitude, Inc. FT SX20 

//0x008B Infocom ITS A/S G.721 ADPCM 

//0x008C Convedia G729 

//0x008D Not specified congruency, Inc. 

//0x0091 Siemens Business Communications SBC24 

//0x0092 Sonic Foundry Dolby AC3 SPDIF 

//0x0093 MediaSonic G.723 

//0x0094 Aculab PLC Prosody 8KBPS 

//0x0097 ZyXEL ADPCM 

//0x0098 Philips LPCBB 

//0x0099 Studer Professional Audio AG Packed 

//0x00A0 Malden Electronics PHONYTALK 

//0x00A1 Racal Recorder GSM 

//0x00A2 Racal Recorder G720.a 

//0x00A3 Racal G723.1 

//0x00A4 Racal Tetra ACELP 

//0x00B0 NEC AAC NEC Corporation 

//0x00E1 Microsoft ADPCM 
#define AudioFlag_MS_ADPCM_2		0x00E1

//0x00F6 ADIF AAC defined by VisualOn internal
#define AudioFlag_ADIF_AAC			0x00F6

//0x00F7 ADTS AAC defined by VisualOn internal
#define AudioFlag_ADTS_AAC			0x00F7

//0x00F8 LATM AAC defined by VisualOn internal
#define AudioFlag_LATM_AAC			0x00F8

//0x00F9 LOAS AAC defined by VisualOn internal
#define AudioFlag_LOAS_AAC			0x00F9

//0x00FF AAC 
#define AudioFlag_AAC				0x00FF

//0x0100 Rhetorex ADPCM 

//0x0101 IBM mu-law / BeCubed Software IRAT 

//0x0102 IBM A-law 

//0x0103 IBM AVC ADPCM 

//0x0111 Vivo G.723 

//0x0112 Vivo Siren 

//0x0120 Philips Speech Processing CELP 

//0x0121 Philips Speech Processing GRUNDIG 

//0x0123 Digital G.723 

//0x0125 Sanyo LD ADPCM 

//0x0130 Sipro Lab Telecom ACELP.net 

//0x0131 Sipro Lab Telecom ACELP.4800 

//0x0132 Sipro Lab Telecom ACELP.8V3 

//0x0133 Sipro Lab Telecom ACELP.G.729 

//0x0134 Sipro Lab Telecom ACELP.G.729A 

//0x0135 Sipro Lab Telecom ACELP.KELVIN 

//0x0136 VoiceAge AMR 

//0x0140 Dictaphone G.726 ADPCM 

//0x0150 Qualcomm PureVoice 
#define AudioFlag_QCELP				0x0150

//0x0151 Qualcomm HalfRate 

//0x0155 Ring Zero Systems TUB GSM 

//0x0160 Windows Media Audio V1 / DivX audio (WMA) 
#define AudioFlag_WMA_V1			0x0160

//0x0161 Windows Media Audio V2 V7 V8 V9 / DivX audio (WMA) / Alex AC3 Audio 
#define AudioFlag_WMA_V2			0x0161

//0x0162 Windows Media Audio Professional V9 
#define AudioFlag_WMA_PRO_V9		0x0162

//0x0163 Windows Media Audio Lossless V9 
#define AudioFlag_WMA_LOS_V9		0x0163

//0x0164 WMA Pro over S/PDIF 
#define AudioFlag_WMA_PRO			0x0164

//0x0170 UNISYS NAP ADPCM 

//0x0171 UNISYS NAP ULAW 

//0x0172 UNISYS NAP ALAW 

//0x0173 UNISYS NAP 16K 

//0x0174 MM SYCOM ACM SYC008 SyCom Technologies 

//0x0175 MM SYCOM ACM SYC701 G726L SyCom Technologies 

//0x0176 MM SYCOM ACM SYC701 CELP54 SyCom Technologies 

//0x0177 MM SYCOM ACM SYC701 CELP68 SyCom Technologies 

//0x0178 Knowledge Adventure ADPCM 

//0x0180 Fraunhofer IIS MPEG2AAC 

//0x0190 Digital Theater Systems DTS DS 

//0x0200 Creative Labs ADPCM 

//0x0202 Creative Labs FastSpeech8 

//0x0203 Creative Labs FastSpeech10 

//0x0210 UHER Informatic ADPCM 

//0x0215 Ulead DV ACM 

//0x0216 Ulead DV ACM 

//0x0220 Quarterdeck 

//0x0230 I-link Worldwide ILINK VC 

//0x0240 Aureal Semiconductor RAW SPORT 

//0x0241 ESST AC3 

//0x0250 Interactive Products HSX 

//0x0251 Interactive Products RPELP 

//0x0260 Consistent Software CS2 

//0x0270 Sony ATRAC3 (SCX, same as MiniDisk LP2) 

//0x0271 Sony SCY 

//0x0272 Sony ATRAC3 

//0x0273 Sony SPC 

//0x0280 TELUM Telum Inc. 

//0x0281 TELUMIA Telum Inc. 

//0x0285 Norcom Voice Systems ADPCM Norcom Electronics Corporation 

//0x0300 Fujitsu FM TOWNS SND 
//0x0301 - 0x0308 Not specified Fujitsu Corporation 

//0x0350 DEVELOPMENT Micronas Semiconductors, Inc. 

//0x0351 Micronas Semiconductors, Inc. CELP833 

//0x0400 BTV Digital (Brooktree digital audio format) 

//0x0401 Intel Music Coder (IMC) 

//0x0402 Ligos Indeo Audio 

//0x0450 QDesign Music 

//0x0500 On2 VP7 On2 Technologies AVC Audio 

//0x0501 On2 VP6 On2 Technologies 

//0x0680 AT&T Labs VME VMPCM 

//0x0681 AT&T Labs TPC 

//0x0700 YMPEG Alpha (dummy for MPEG-2 compressor) 

//0x08AE ClearJump LiteWave (lossless) 

//0x1000 Olivetti GSM 

//0x1001 Olivetti ADPCM 

//0x1002 Olivetti CELP 

//0x1003 Olivetti SBC 

//0x1004 Olivetti OPR 

//0x1100 Lernout & Hauspie codec 

//0x1101 Lernout & Hauspie CELP codec 

//0x1102 Lernout & Hauspie SBC codec 

//0x1103 Lernout & Hauspie SBC codec 

//0x1104 Lernout & Hauspie SBC codec 

//0x1400 Norris Communication 

//0x1401 AT&T Labs ISIAudio 

//0x1500 AT&T Labs Soundspace Music Compression 

#define  AudioFlag_MPEG_RAW_AAC               0x1601 /* Microsoft Corporation */

//0x181C VoxWare RT24 speech codec 

//0x181E Lucent elemedia AX24000P Music codec 

//0x1971 Sonic Foundry LOSSLESS 

//0x1979 Innings Telecom Inc. ADPCM 

//0x1C07 Lucent SX8300P speech codec 

//0x1C0C Lucent SX5363S G.723 compliant codec 

//0x1F03 CUseeMe DigiTalk (ex-Rocwell) 

//0x1FC4 NCT Soft ALF2CD ACM 

//0x2000 Dolby AC3 / FAST Multimedia AG DVM 
#define AudioFlag_AC3				0x2000

//0x2001 Dolby DTS (Digital Theater System) 
#define AudioFlag_DTS				0x2001

//0x2002 RealAudio 1 / 2 14.4 
#define AudioFlag_RA_1_2_144		0x2002

//0x2003 RealAudio 1 / 2 28.8 
#define AudioFlag_RA_1_2_288		0x2003

//0x2004 RealAudio G2 / 8 Cook (low bitrate)
#define AudioFlag_RA_G2				0x2004

//0x2005 RealAudio 3 / 4 / 5 Music (DNET) 
#define AudioFlag_RA_DNET			0x2005

//0x2006 RealAudio 10 AAC (RAAC) 
#define AudioFlag_RA_AAC			0x2006

//0x2007 RealAudio 10 AAC+ (RACP) 
#define AudioFlag_RA_AAC_PLUS		0x2007

//0x2500 Reserved range to 0x2600 Microsoft 

//0x3313 makeAVIS (ffvfw fake AVI sound from AviSynth scripts) 

//0x4143 Divio MPEG-4 AAC audio 

//0x4201 Nokia adaptive multirate Nokia Mobile Phones 

//0x4243 Divio's G726 Divio, Inc. 

//0x434C LEAD Speech 

//0x4C41 Apple lossless audio codec
#define AudioFlag_ALAC				0x4C41

//0x564C LEAD Vorbis 

//0x5756 WavPack Audio 

//0x674f Ogg Vorbis (mode 1) 
#define AudioFlag_OGG_1				0x674f

//0x6750 Ogg Vorbis (mode 2) 
#define AudioFlag_OGG_2				0x6750

//0x6751 Ogg Vorbis (mode 3) 
#define AudioFlag_OGG_3				0x6751

//0x676f Ogg Vorbis (mode 1+) 
#define AudioFlag_OGG_1_PLUS		0x676f

//0x6770 Ogg Vorbis (mode 2+) 
#define AudioFlag_OGG_2_PLUS		0x6770

//0x6771 Ogg Vorbis (mode 3+) 
#define AudioFlag_OGG_3_PLUS		0x6771

//0x566f Ogg Vorbis
#define AudioFlag_OGG_4_PLUS		0x566f


//0x7000 3COM NBX 3Com Corporation 

//0x706D FAAD AAC 

//0x77A1 The True Audio 

//0x7A21 GSM-AMR (CBR, no SID) 

//0x7A22 GSM-AMR (VBR, including SID) 

//0xA100 Comverse Infosys Ltd. G723 1 

//0xA101 Comverse Infosys Ltd. AVQSBC 

//0xA102 Comverse Infosys Ltd. OLDSBC 

//0xA103 Symbol Technology's G729A Symbol Technologies Canada 

//0xA104 VoiceAge AMR WB VoiceAge Corporation 

//0xA105 Ingenient Technologies Inc. G726 

//0xA106 ISO/MPEG-4 advanced audio Coding 
#define AudioFlag_ISOMPEG4_AAC		0xA106

//0xA107 Encore Software Ltd's G726 

//0xA109 Speex ACM Codec xiph.org 

//0xAEB1 AWB+ defined by VisualOn internal
#define AudioFlag_AMR_WB_PLUS		0xAEB1

//0xC0CC GigaLink Audio Codec 

//0xDFAC DebugMode SonicFoundry Vegas FrameServer ACM Codec 

//0xE02C EVRC defined by VisualOn internal
#define AudioFlag_EVRC				0xE02C

//0xEAC3 Enhanced AC-3 defined by VisualOn internal
#define AudioFlag_EAC3				0xEAC3

//0xF1AC Free Lossless Audio Codec FLAC 
#define AudioFlag_FLAC				0xF1AC

//0xF1AE Monkey's Audio Codec APE
#define AudioFlag_APE				0xF1AE


//0xFFFC VDOwave Audio 

//0xFFFE Extensible wave format 
#define AudioFlag_WAVE_EXT			0xFFFE

//0xFFFF In Development / Unregistered 

#ifdef _VONAMESPACE
}
#endif
#endif // __fCC_H__
