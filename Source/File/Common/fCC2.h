	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		fCC2.h


*******************************************************************************/

#ifndef __fCC2_H__
#define __fCC2_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#ifndef MAKEFOURCC2
#define MAKEFOURCC2(ch0, ch1, ch2, ch3)									\
	((VO_U32)(VO_U8)(ch3) | ((VO_U32)(VO_U8)(ch2) << 8) |   \
	((VO_U32)(VO_U8)(ch1) << 16) | ((VO_U32)(VO_U8)(ch0) << 24 ))
#endif	//MAKEFOURCC22
//////////////////////////////////////////////////////////////////////////



#define FOURCC2_meta					MAKEFOURCC2('m', 'e', 't', 'a')
#define FOURCC2_ilst					MAKEFOURCC2('i', 'l', 's', 't')
#define FOURCC2_ID32					MAKEFOURCC2('I', 'D', '3', '2')
#define FOURCC2_data					MAKEFOURCC2('d', 'a', 't', 'a')
#define FOURCC2_mean					MAKEFOURCC2('m', 'e', 'a', 'n')
#define FOURCC2_name					MAKEFOURCC2('n', 'a', 'm', 'e')

#define FOURCC2_____        MAKEFOURCC2('-','-','-','-')
#define FOURCC2_aART        MAKEFOURCC2('a','A','R','T')
#define FOURCC2_akID        MAKEFOURCC2('a','k','I','D')
#define FOURCC2_albm        MAKEFOURCC2('a','l','b','m')
#define FOURCC2_apID        MAKEFOURCC2('a','p','I','D')
#define FOURCC2_atID        MAKEFOURCC2('a','t','I','D')
#define FOURCC2_auth        MAKEFOURCC2('a','u','t','h')
#define FOURCC2_catg        MAKEFOURCC2('c','a','t','g')
#define FOURCC2_cnID        MAKEFOURCC2('c','n','I','D')
#define FOURCC2_covr        MAKEFOURCC2('c','o','v','r')
#define FOURCC2_cpil        MAKEFOURCC2('c','p','i','l')
#define FOURCC2_desc        MAKEFOURCC2('d','e','s','c')
#define FOURCC2_disk        MAKEFOURCC2('d','i','s','k')
#define FOURCC2_dscp        MAKEFOURCC2('d','s','c','p')
#define FOURCC2_geID        MAKEFOURCC2('g','e','I','D')
#define FOURCC2_gnre        MAKEFOURCC2('g','n','r','e')
#define FOURCC2_grup        MAKEFOURCC2('g','r','u','p')
#define FOURCC2_hdvd        MAKEFOURCC2('h','d','v','d')
#define FOURCC2_keyw        MAKEFOURCC2('k','e','y','w')
#define FOURCC2_ldes        MAKEFOURCC2('l','d','e','s')
#define FOURCC2_pcst        MAKEFOURCC2('p','c','s','t')
#define FOURCC2_perf        MAKEFOURCC2('p','e','r','f')
#define FOURCC2_pgap        MAKEFOURCC2('p','g','a','p')
#define FOURCC2_plID        MAKEFOURCC2('p','l','I','D')
#define FOURCC2_purd        MAKEFOURCC2('p','u','r','d')
#define FOURCC2_purl        MAKEFOURCC2('p','u','r','l')
#define FOURCC2_rtng        MAKEFOURCC2('r','t','n','g')
#define FOURCC2_sfID        MAKEFOURCC2('s','f','I','D')
#define FOURCC2_soaa        MAKEFOURCC2('s','o','a','a')
#define FOURCC2_soal        MAKEFOURCC2('s','o','a','l')
#define FOURCC2_soar        MAKEFOURCC2('s','o','a','r')
#define FOURCC2_soco        MAKEFOURCC2('s','o','c','o')
#define FOURCC2_sonm        MAKEFOURCC2('s','o','n','m')
#define FOURCC2_sosn        MAKEFOURCC2('s','o','s','n')
#define FOURCC2_stik        MAKEFOURCC2('s','t','i','k')
#define FOURCC2_titl        MAKEFOURCC2('t','i','t','l')
#define FOURCC2_tmpo        MAKEFOURCC2('t','m','p','o')
#define FOURCC2_trkn        MAKEFOURCC2('t','r','k','n')
#define FOURCC2_tven        MAKEFOURCC2('t','v','e','n')
#define FOURCC2_tves        MAKEFOURCC2('t','v','e','s')
#define FOURCC2_tvnn        MAKEFOURCC2('t','v','n','n')
#define FOURCC2_tvsh        MAKEFOURCC2('t','v','s','h')
#define FOURCC2_tvsn        MAKEFOURCC2('t','v','s','n')

#define FOURCC2_Xnam  MAKEFOURCC2('\251','n','a','m')     
#define FOURCC2_Xart  MAKEFOURCC2('\251','a','r','t')     
#define FOURCC2_XART  MAKEFOURCC2('\251','A','R','T')     
#define FOURCC2_Xalb  MAKEFOURCC2('\251','a','l','b')     
#define FOURCC2_Xwrt  MAKEFOURCC2('\251','w','r','t')     
#define FOURCC2_Xcmt  MAKEFOURCC2('\251','c','m','t')     
#define FOURCC2_Xgen  MAKEFOURCC2('\251','g','e','n')     
#define FOURCC2_Xday  MAKEFOURCC2('\251','d','a','y')     
#define FOURCC2_Xtoo  MAKEFOURCC2('\251','t','o','o')     
#define FOURCC2_Xenc  MAKEFOURCC2('\251','e','n','c')     
#define FOURCC2_Xlyr  MAKEFOURCC2('\251','l','y','r')     
#define FOURCC2_Xst3  MAKEFOURCC2('\251','s','t','3')     





//add by leon
#define FOURCC2_mp4a         MAKEFOURCC2('m','p','4','a')
#define FOURCC2_h264         MAKEFOURCC2('h','2','6','4')
#define FOURCC2_s264         MAKEFOURCC2('s','2','6','4')
#define FOURCC2_s263         MAKEFOURCC2('s','2','6','3')
#define FOURCC2_alaw         MAKEFOURCC2('a','l','a','w') //20110627, Jason
#define FOURCC2_sowt         MAKEFOURCC2('s','o','w','t')
#define FOURCC2_alac         MAKEFOURCC2('a','l','a','c') //20100712, yangyi
#define FOURCC2_tx3g         MAKEFOURCC2('t','x','3','g')
#define FOURCC2_sawb         MAKEFOURCC2('s','a','w','b')
#define FOURCC2_samr         MAKEFOURCC2('s','a','m','r')
#define FOURCC2_sevc         MAKEFOURCC2('s','e','v','c')	//4/28/2009, Jason
#define FOURCC2_evrc         MAKEFOURCC2('e','v','r','c') //EVRC, 4/11/2011, Jason
#define FOURCC2_sawp         MAKEFOURCC2('s','a','w','p')
#define FOURCC2__mp3         MAKEFOURCC2('.','m','p','3')
#define FOURCC2_ac_3         MAKEFOURCC2('a','c','-','3')
#define FOURCC2_ec_3         MAKEFOURCC2('e','c','-','3') //eAC3, East, 2010/03/09
#define FOURCC2_sqcp         MAKEFOURCC2('s','q','c','p')
#define FOURCC2_raw          MAKEFOURCC2('r','a','w',' ')//11/24/2011.leon
#define FOURCC2_wma          MAKEFOURCC2('w','m','a',' ')//09/01/2011, Leon 
#define FOURCC2_vc_1         MAKEFOURCC2('v','c','-','1')//09/01//2011, Leon 
#define FOURCC2_ulaw         MAKEFOURCC2('u','l','a','w') //20110627, Jason
#define FOURCC2_soun         MAKEFOURCC2('s','o','u','n')
#define FOURCC2_vide         MAKEFOURCC2('v','i','d','e')
#define FOURCC2_drms         MAKEFOURCC2('d','r','m','s')
#define FOURCC2_encv         MAKEFOURCC2('e','n','c','v')
#define FOURCC2_enca         MAKEFOURCC2('e','n','c','a')
#define FOURCC2_rtp          MAKEFOURCC2('r','t','p',' ')
#define FOURCC2_dash  			 MAKEFOURCC2('d','a','s','h')
#define FOURCC2_qt 				 MAKEFOURCC2('q','t',' ',' ')
#define FOURCC2_cypt			MAKEFOURCC2('c','y','p','t')
#define FOURCC2_isml 				 MAKEFOURCC2('i','s','m','l')
#define FOURCC2_text 				 MAKEFOURCC2('t','e','x','t')
#define FOURCC2_esda MAKEFOURCC2('e','s','d','a')
#define FOURCC2_avcC MAKEFOURCC2('a','v','c','C')
#define FOURCC2_devc MAKEFOURCC2('d','e','v','c')
#define FOURCC2_d263 MAKEFOURCC2('d','2','6','3')
#define FOURCC2_wfex MAKEFOURCC2('w','f','e','x')
#define FOURCC2_dvc1 MAKEFOURCC2('d','v','c','1')
#define FOURCC2_mp4l MAKEFOURCC2('m','p','4','l')
#define FOURCC2_EC_3      MAKEFOURCC2('E','C','-','3')
#define FOURCC2_aacl      MAKEFOURCC2('a','a','c','l')
#define FOURCC2_wmap      MAKEFOURCC2('w','m','a','p')



#define FOURCC2_mp4v					MAKEFOURCC2('m', 'p', '4', 'v')
#define FOURCC2_H263					MAKEFOURCC2('h', '2', '6', '3')
#define FOURCC2_avc1					MAKEFOURCC2('a', 'v', 'c', '1')
#define FOURCC2_mp4s					MAKEFOURCC2('m', 'p', '4', 's')

#define FOURCC2_avc3					MAKEFOURCC2('a', 'v', 'c', '3')

#define FOURCC2_hvc1					MAKEFOURCC2('h', 'v', 'c', '1')
#define FOURCC2_HVC1					MAKEFOURCC2('H', 'V', 'C', '1')
#define FOURCC2_hev1					MAKEFOURCC2('h', 'e', 'v', '1')
#define FOURCC2_hvcC					MAKEFOURCC2('h', 'v', 'c', 'C')
#define FOURCC2_hevC					MAKEFOURCC2('h', 'e', 'v', 'C')
#define FOURCC2_stpp					MAKEFOURCC2('s', 't', 'p', 'p')

#define FOURCC2_uuid					MAKEFOURCC2('u', 'u', 'i', 'd')	//use extended type(GUID)

#define FOURCC2_ftyp					MAKEFOURCC2('f', 't', 'y', 'p')	//file type and compatibility

#define FOURCC2_moov					MAKEFOURCC2('m', 'o', 'o', 'v')	//container for all the metadata
#define FOURCC2_mvhd					MAKEFOURCC2('m', 'v', 'h', 'd')	//movie header, overall declarations
#define FOURCC2_trak					MAKEFOURCC2('t', 'r', 'a', 'k')	//container for an individual track or stream
#define FOURCC2_tkhd					MAKEFOURCC2('t', 'k', 'h', 'd')	//track header, overall information about the track
#define FOURCC2_tref					MAKEFOURCC2('t', 'r', 'e', 'f')	//track reference container
#define FOURCC2_edts					MAKEFOURCC2('e', 'd', 't', 's')	//edit list container
#define FOURCC2_elst					MAKEFOURCC2('e', 'l', 's', 't')	//an edit list
#define FOURCC2_mdia 				MAKEFOURCC2('m', 'd', 'i', 'a')	//container for the media information in a track
#define FOURCC2_mdhd 				MAKEFOURCC2('m', 'd', 'h', 'd')	//media header, overall information about the media
#define FOURCC2_hdlr 				MAKEFOURCC2('h', 'd', 'l', 'r')	//handler, declares the media (handler) type
#define FOURCC2_minf 				MAKEFOURCC2('m', 'i', 'n', 'f')	//media information container
#define FOURCC2_vmhd 				MAKEFOURCC2('v', 'm', 'h', 'd')	//video media header, overall information (video track only)
#define FOURCC2_smhd 				MAKEFOURCC2('s', 'm', 'h', 'd')	//sound media header, overall information (sound track only)
#define FOURCC2_hmhd 				MAKEFOURCC2('h', 'm', 'h', 'd')	//hint media header, overall information (hint track only)
#define FOURCC2_nmhd 				MAKEFOURCC2('n', 'm', 'h', 'd')	//Null media header, overall information (some tracks only)
#define FOURCC2_dinf 				MAKEFOURCC2('d', 'i', 'n', 'f')	//data information box, container
#define FOURCC2_dref 				MAKEFOURCC2('d', 'r', 'e', 'f')	//data reference box, declares source(s) of media data in track
#define FOURCC2_stbl 				MAKEFOURCC2('s', 't', 'b', 'l')	//sample table box, container for the time/space map
#define FOURCC2_stsd 				MAKEFOURCC2('s', 't', 's', 'd')	//sample descriptions (codec types, initialization etc.)
#define FOURCC2_stts 				MAKEFOURCC2('s', 't', 't', 's')	//(decoding) time-to-sample
#define FOURCC2_ctts 				MAKEFOURCC2('c', 't', 't', 's')	//(composition) time to sample
#define FOURCC2_stsc 				MAKEFOURCC2('s', 't', 's', 'c')	//sample-to-chunk, partial data-offset information
#define FOURCC2_stsz 				MAKEFOURCC2('s', 't', 's', 'z')	//sample sizes (framing)
#define FOURCC2_stz2 				MAKEFOURCC2('s', 't', 'z', '2')	//compact sample sizes (framing)
#define FOURCC2_stco 				MAKEFOURCC2('s', 't', 'c', 'o')	//chunk offset, partial data-offset information
#define FOURCC2_co64 				MAKEFOURCC2('c', 'o', '6', '4')	//64-bit chunk offset
#define FOURCC2_stss 				MAKEFOURCC2('s', 't', 's', 's')	//sync sample table (random access points)
#define FOURCC2_stsh 				MAKEFOURCC2('s', 't', 's', 'h')	//shadow sync sample table
#define FOURCC2_padb 				MAKEFOURCC2('p', 'a', 'd', 'b')	//sample padding bits
#define FOURCC2_stdp 				MAKEFOURCC2('s', 't', 'd', 'p')	//sample degradation priority
#define FOURCC2_mvex 				MAKEFOURCC2('m', 'v', 'e', 'x')	//movie extends box
#define FOURCC2_mehd 				MAKEFOURCC2('m', 'e', 'h', 'd')	//movie extends header box
#define FOURCC2_trex 				MAKEFOURCC2('t', 'r', 'e', 'x')	//track extends defaults

#define FOURCC2_moof 				MAKEFOURCC2('m', 'o', 'o', 'f')	//movie fragment
#define FOURCC2_mfhd 				MAKEFOURCC2('m', 'f', 'h', 'd')	//movie fragment header
#define FOURCC2_traf 				MAKEFOURCC2('t', 'r', 'a', 'f')	//track fragment
#define FOURCC2_tfhd 				MAKEFOURCC2('t', 'f', 'h', 'd')	//track fragment header
#define FOURCC2_trun 				MAKEFOURCC2('t', 'r', 'u', 'n')	//track fragment run
#define FOURCC2_tfdt 				MAKEFOURCC2('t', 'f', 'd', 't')	//TrackFragmentBaseMediaDecodeTimeBox

#define FOURCC2_mfra 				MAKEFOURCC2('m', 'f', 'r', 'a')	//movie fragment random access
#define FOURCC2_tfra 				MAKEFOURCC2('t', 'f', 'r', 'a')	//track fragment random access
#define FOURCC2_mfro 				MAKEFOURCC2('m', 'f', 'r', 'o')	//movie fragment random access offset

#define FOURCC2_mdat 				MAKEFOURCC2('m', 'd', 'a', 't')	//media data container

#define FOURCC2_free 				MAKEFOURCC2('f', 'r', 'e', 'e')	//free space

#define FOURCC2_skip 				MAKEFOURCC2('s', 'k', 'i', 'p')	//free space
#define FOURCC2_udta 				MAKEFOURCC2('u', 'd', 't', 'a')	//user-data
#define FOURCC2_cprt 				MAKEFOURCC2('c', 'p', 'r', 't')	//copyright etc.


#define FOURCC2_sinf 				MAKEFOURCC2('s', 'i', 'n', 'f')	//copyright etc.
#define FOURCC2_frma 				MAKEFOURCC2('f', 'r', 'm', 'a')	//copyright etc.
#define FOURCC2_schi 				MAKEFOURCC2('s', 'c', 'h', 'i')	//copyright etc.
#define FOURCC2_esds 				MAKEFOURCC2('e', 's', 'd', 's')	//copyright etc.
#define FOURCC2_wave 				MAKEFOURCC2('w', 'a', 'v', 'e')	//copyright etc.
#define FOURCC2_DX50 				MAKEFOURCC2('D', 'X', '5', '0')	//
#define FOURCC2_sidx				MAKEFOURCC2('s', 'i', 'd', 'x')	//sidx
#define FOURCC2_ssix                MAKEFOURCC2('s', 's', 'i', 'x')	//sidx
#define FOURCC2_ftab				MAKEFOURCC2('f', 't', 'a', 'b')	//ftab
#define FOURCC2_glbl				MAKEFOURCC2('g', 'l', 'b', 'l')	//glbl
#define FOURCC2_mvex				MAKEFOURCC2('m', 'v', 'e', 'x')	
#define FOURCC2_trex				MAKEFOURCC2('t', 'r', 'e', 'x')	
#define FOURCC2_mehd				MAKEFOURCC2('m', 'e', 'h', 'd')	
#define FOURCC2_div3				MAKEFOURCC2('d', 'i', 'v', '3')	// add by Leon, 05/16/2012
#define FOURCC2_DIV3				MAKEFOURCC2('D', 'I', 'V', '3')	// add by Leon, 05/16/2012


//subtitle
#define FOURCC2_styl				MAKEFOURCC2('s', 't', 'y', 'l')	//styl
#define FOURCC2_hlit				MAKEFOURCC2('h', 'l', 'i', 't')	//hlit
#define FOURCC2_hclr				MAKEFOURCC2('h', 'c', 'l', 'r')	//hclr
#define FOURCC2_krok				MAKEFOURCC2('k', 'r', 'o', 'k')	//krok
#define FOURCC2_dlay				MAKEFOURCC2('d', 'l', 'a', 'y')	//dlay
#define FOURCC2_href				MAKEFOURCC2('h', 'r', 'e', 'f')	//href
#define FOURCC2_tbox				MAKEFOURCC2('t', 'b', 'o', 'x')	//tbox
#define FOURCC2_blnk				MAKEFOURCC2('b', 'l', 'n', 'k')	//blnk
#define FOURCC2_twrp				MAKEFOURCC2('t', 'w', 'r', 'p')	//twrp

//add by MaTao for file writer at 2012.3.19
#define FOURCC2_file				MAKEFOURCC2('f', 'i', 'l', 'e')	
#define FOURCC2_url 				MAKEFOURCC2('u', 'r', 'l', ' ')	
#define FOURCC2_urn 				MAKEFOURCC2('u', 'r', 'n', ' ')	
#define FOURCC2_damr 				MAKEFOURCC2('d', 'a', 'm', 'r')	
#define FOURCC2_isom 				MAKEFOURCC2('i', 's', 'o', 'm')	
#define FOURCC2_enu 				MAKEFOURCC2(' ', 'e', 'n', 'u')	
#define FOURCC2_iods 				MAKEFOURCC2('i', 'o', 'd', 's')	
#define FOURCC2_hint 				MAKEFOURCC2('h', 'i', 'n', 't')	
#define FOURCC2_3gp6 				MAKEFOURCC2('3', 'g', 'p', '6')
#define FOURCC2_3gp5 				MAKEFOURCC2('3', 'g', 'p', '5')
#define FOURCC2_3gp4 				MAKEFOURCC2('3', 'g', 'p', '4')
#define FOURCC2_3g2a 				MAKEFOURCC2('3', 'g', '2', 'a')
#define FOURCC2_mp42 				MAKEFOURCC2('m', 'p', '4', '2')
#define FOURCC2_mp41 				MAKEFOURCC2('m', 'p', '4', '1')
#define FOURCC2_null 				MAKEFOURCC2('n', 'u', 'l', 'l')


//add by MaTao for audio file writer at 2012.3.19
#define FOURCC2_FFIR 				MAKEFOURCC2('F', 'F', 'I', 'R')
#define FOURCC2_MCLQ				MAKEFOURCC2('M', 'C', 'L', 'Q')
#define FOURCC2_tmf					MAKEFOURCC2(' ', 't', 'm', 'f')
#define FOURCC2_tarv				MAKEFOURCC2('t', 'a', 'r', 'v')
#define FOURCC2_atad				MAKEFOURCC2('a', 't', 'a', 'd')

#define FOURCC2_jpeg				MAKEFOURCC2('j', 'p', 'e', 'g')
#define FOURCC2_JPEG				MAKEFOURCC2('J', 'P', 'E', 'G')

//cenc
#define FOURCC2_pssh				MAKEFOURCC2('p', 's', 's', 'h')  
#define FOURCC2_tenc				MAKEFOURCC2('t', 'e', 'n', 'c')
#define FOURCC2_senc				MAKEFOURCC2('s', 'e', 'n', 'c')
#define FOURCC2_saiz				MAKEFOURCC2('s', 'a', 'i', 'z')
#define FOURCC2_saio				MAKEFOURCC2('s', 'a', 'i', 'o')
//0xFFFF In Development / Unregistered 

#ifdef _VONAMESPACE
}
#endif
#endif // __fCC_H__
