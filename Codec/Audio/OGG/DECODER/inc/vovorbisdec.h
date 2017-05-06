//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    vovorbisdec.h

Abstract:

    Ogg decoder header file.

Author:

    Witten Wen 04-September-2009

Revision History:

*************************************************************************/

#ifndef __VO_VORBIS_DEC_H_
#define __VO_VORBIS_DEC_H_

//#include "macros.h"
#include "ogg.h"
#include "codebook.h"
#include "vorbismemory.h"

#define INPUTBUFFERSIZE	0x8000

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Standard Vorbis format 2 to set parameter */
struct VORBISFORMAT2
{
	size_t Channels;
	size_t SamplesPerSec;
	size_t BitsPerSample;
	size_t HeadPacketSize[3];
};

/* VorbisInfo contains all the setup information specific to the
   specific compression/decompression mode in progress (eg,
   psychoacoustic settings, channel setup, options, codebook
   etc). VorbisInfo and substructures are in backends.h.
*********************************************************************/
typedef struct VorbisInfo{
	int		m_Version;			/* vorbis version, must be 0 */
	int		m_Channels;			/* vorbis audio channel */
	long	m_Rate;			/* vorbis audio sample rate */
	int		m_SampleBits;		/* vorbis audio sample bits */

	/* The below bitrate declarations are *hints*.
	Combinations of the three values carry the following implications:

	all three set to the same value: 
	implies a fixed rate bitstream
	only nominal set: 
	implies a VBR stream that averages the nominal bitrate.  No hard 
	upper/lower limit
	upper and or lower set: 
	implies a VBR bitstream that obeys the bitrate limits. nominal 
	may also be set to give a nominal rate.
	none set:
	the coder does not care to speculate.
	*/

	long	m_BitrateUp;		/* bit rate upper value */
	long	m_BitrateNom;		/* bit rate nominal value */
	long	m_BitrateLow;		/* bit rate lower value */
//	bitrate_window;

	void	*m_pCodecSetup;		//codec_setup;		/* struct CodecSetupInfo */
} VorbisInfo;//vorbis_info;

/* the comments are not part of VorbisInfo so that VorbisInfo can be
   static storage */
typedef struct VorbisComment{
  /* unlimited user comment fields.  libvorbis writes 'libvorbis'
     whatever vendor is set to in encode */
  char **m_ppUserComments;//user_comments;
  int   *m_pCommentLengths;//comment_lengths;
  int    m_Comments;//comments;
  char  *m_pVendor;//vendor;

} VorbisComment;//vorbis_comment;

/* vorbis_dsp_state buffers the current vorbis audio
   analysis/synthesis state.  The DSP state belongs to a specific
   logical bitstream ****************************************************/
typedef struct VorbisDSPState{
//  int analysisp;
  VorbisInfo *m_pVI;//vi;

  OGG_S32	**m_ppPCM;//pcm;
  OGG_S32	**m_ppPCMRet;//pcmret;
  int		m_PCMStorage;//pcm_storage;
  int		m_PCMCurrent;//pcm_current;
  int		m_PCMReturned;//pcm_returned;

//  int		m_PreExtraPlt;//preextrapolate;
  int		m_EOffLag;//eofflag;

  long		m_lW;//previous window size 
  long		m_W;//current window size
  long		m_nW;//nW
  long		m_CenterW;//centerW;

  OGG_S64	m_GranulePos;//granulepos;
  OGG_S64	m_Sequence;//sequence;

  void       *m_pBackendState;//backend_state;
} VorbisDSPState;//vorbis_dsp_state;

struct AllocChain{//alloc_chain{
	void *ptr;
	struct AllocChain *next;
};	//alloc_chain

/* vorbis_block is a single block of data to be processed as part of
the analysis/synthesis stream; it belongs to a specific logical
bitstream, but is independant from other vorbis_blocks belonging to
that logical bitstream. *************************************************/
typedef struct VorbisBlock{
	/* necessary stream state for linking to the framing abstraction */
	OGG_S32  **m_ppPCM;//pcm;       /* this is a pointer into local storage */ 
	//  oggpack_buffer opb;

	long  m_lW;//lW;
	long  m_W;//W;
	long  m_nW;//nW;
	int   m_PCMEnd;//pcmend;
//	int   m_Mode;//mode;

	int         m_EOffLag;//eofflag;
	OGG_S64		m_GranulePos;//granulepos;
	OGG_S64		m_Sequence;//sequence;
	VorbisDSPState *vd; /* For read-only access of configuration */

	/* local storage to avoid remallocing; it's up to the mapping to 
	structure it */
	void               *m_pLocalStore;//localstore;
	long                m_LocalTop;//localtop;
	long                m_LocalAlloc;//localalloc;
	long                m_TotleUse;//totaluse;
	struct AllocChain	*m_pReap;//reap;

} VorbisBlock;//vorbis_block;

typedef struct PacketStage{
	VO_U8		*m_pPageBgn;	
	VO_U32		m_HeaderBytes;
	VO_U32		m_BodyBytes;
	VO_S32		m_PacketBegin;
	VO_S32		m_PacketLength;
	VO_S32		m_NextPacketBgn;
//	VO_S32		m_NextPacketLth;
//	VO_S32		m_spanlcstr;
	VO_U8		*m_pSpanPacketBuf;
	VO_U8		*m_pSpanPacketEnd;
}PacketStage;

typedef struct VOOGGInputBitStream
{
	
	VO_U8		*m_ptr;
	VO_U32		m_CBitUsed;
	VO_U32		m_PacketLength;
	VO_BOOL		m_Flag;
}VOOGGInputBitStream;

typedef struct VOOGGInputBuffer{
	VO_U8		*m_pBuffer;		//OGG input Buffer
	VO_U8		*m_pDataEnd;	//the end pos of the current input Data;
	VO_U8		*m_pNextPcktBgn;	//the begin pos of the next packet;
	VO_U32		m_BufferSize;
	VO_U32		m_DataLength;
	VO_U32		m_CurBlockLen;
}VOOGGInputBuffer;

#define  NOTOPEN   0
#define  PARTOPEN  1
#define  OPENED    2
#define  STREAMSET 3
#define  INITSET   4

typedef enum _OGGDecState{
	OGGDecStateDone	= 0,
	OGGDecStateInput,
	OGGDecStateDecode
}OGGDecState;

typedef struct CVOVorbisObjectDecoder{
//	void			*datasource; /* Pointer to a FILE *, etc. */
	void			*m_pDataBuffer; /* Pointer to a FILE *, etc. */
	int				m_DataLength;
	int				m_Seekable;
	int				m_HeaderOK;
	int				m_PacketNo;
//	OGG_S64			offset;
//	OGG_S64			end;

	/* If the FILE handle isn't seekable (eg, a pipe), only the current wenmp1999 13482624616
		stream appears */
	int				m_Links;//links;
//	OGG_U32			*serialnos;
	OGG_S64			*pcmlengths;	//目前为止，不太清楚作用
	VorbisInfo		*vi;
	VorbisComment	*vc;

	/* Decoding working state local storage */
//	OGG_S64			pcm_offset;
	int				m_ReadyState;//ready_state;
	OGG_U32			m_CurSerialNo;//current_serialno;
//	int				current_link;
	int				m_HeaderRead;	
//	int				read_mode;	

//	OGG_S64			bittrack;	//don't delete temporarily, maybe useful later.
	OGG_S64			samptrack;

	OGGStreamState	*os; /* take physical pages, weld into a logical
							stream of packets */
	VorbisDSPState	vd; /* central working state for the packet->PCM decoder */
	VorbisBlock		vb; /* local working space for packet->PCM decode */
	
	//Witten added
	OGGPacket			*op;

	//new AP need memory operation
	VO_MEM_OPERATOR		*vopMemOP;

	VOOGGInputBuffer	*m_oggbuffer;
	VOOGGInputBitStream	*m_oggbs;

	PacketStage			*m_packetstage;

	OGGDecState			m_decState;
	
	OGG_U32				m_fileformat;

	OGG_U32				m_inputused;
	//voCheck lib need
	VO_PTR				hCheck;
}CVOVorbisObjectDecoder;

/* mode ************************************************************/
typedef struct {
  int m_BlockFlag;//blockflag;
  int m_WindowType;//windowtype;
  int m_TransformType;//transformtype;
  int m_Mapping;//mapping;
} VorbisInfoMode;//vorbis_info_mode;

typedef void VorbisLookMapping;//vorbis_look_mapping;
typedef void VorbisLookFloor;
typedef void VorbisLookResidue;
//typedef void vorbis_look_transform;

typedef void VorbisInfoFloor;//vorbis_info_floor;
typedef void VorbisInfoResidue;//vorbis_info_residue;
typedef void VorbisInfoMapping;//vorbis_info_mapping;

typedef struct PrivateState {
  /* local lookup storage */
  const void			*m_pWindow[2];//window[2];

  /* backend lookups are tied to the mode, not the backend or naked mapping */
  int					m_Modebits;//modebits;
  VorbisLookMapping		**m_ppMode;//mode;

  OGG_S64				m_SampleCount;//sample_count;

} PrivateState;		//private_state


/* CodecSetupInfo contains all the setup information specific to the
   specific compression/decompression mode in progress (eg,
   psychoacoustic settings, channel setup, options, codebook
   etc).  
*********************************************************************/
typedef struct CodecSetupInfo {

  /* Vorbis supports only short and long blocks, but allows the
     encoder to choose the sizes */

  long		m_BlockSize[2];	

  /* modes are the primary means of supporting on-the-fly different
     blocksizes, different channel mappings (LR or M/A),
     different residue backends, etc.  Each mode consists of a
     blocksize flag and a mapping (along with the mapping setup */

  int        m_Modes;
  int        m_Maps;
  int        m_Times;
  int        m_Floors;
  int        m_Residues;
  int        m_Books;			/* the number of books */

  VorbisInfoMode       *m_pModeParam[64];
  int                   m_MapType[64];
  VorbisInfoMapping    *m_pMapParam[64];
  int                   m_TimeType[64];
  int                   m_FloorType[64];
  VorbisInfoFloor      *m_pFloorParam[64];
  int                   m_ResidueType[64];
  VorbisInfoResidue    *m_pResidueParam[64];
  StaticCodebook       *m_pBookParam[256];
  CodeBook             *m_pFullbooks;

//  int    passlimit[32];     /* iteration limit per couple/quant pass */
//  int    coupling_passes;
} CodecSetupInfo;		//codec_setup_info

//from ogg.h
extern long voOGGDecPackReadBits(VOOGGInputBitStream *obs, int bits);
extern void voOGGDecPackReadinit(VOOGGInputBitStream *obs, OGGPacket *op);
extern long voOGGDecPackLookBits(VOOGGInputBitStream *obs, int bits);	//oggpack_look
extern void voOGGDecPackAdvBits(VOOGGInputBitStream *obs, int bits);	//oggpack_adv
extern void voOGGDecSetNextBlock(CVOVorbisObjectDecoder *pvorbisdec);
extern void voOGGDecSetNextBlockSpan(CVOVorbisObjectDecoder *pvorbisdec);
extern void voOGGDecClearBitStream(CVOVorbisObjectDecoder *pvorbisdec); 
extern void voOGGDecResetOBS2Packet(VOOGGInputBitStream *obs);
extern void BufferReset(VOOGGInputBuffer *pInput);
extern void BufferResetSpan(CVOVorbisObjectDecoder *pvorbisdec);
extern void ReadString(VOOGGInputBitStream *obs, char *buf, int bytes);

/* Vorbis PRIMITIVES: synthesis layer *******************************/
extern	int		voOGGDecHeaderParser(CVOVorbisObjectDecoder *pvorbisdec);
extern	int     voOGGDecInfoInit(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi);
extern  void    VorbisInfoUnint(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi);
extern	void    VorbisCommentInit(VorbisComment *vc);
extern  int     OGGBufferInit(CVOVorbisObjectDecoder *pvorbisdec);
extern  void	OGGBufferUnint(CVOVorbisObjectDecoder *pvorbisdec);
extern  void	PacketStageReset(PacketStage *packetstage);

//for codebook.h
extern void voOGGDecClearStaticbook(CVOVorbisObjectDecoder *pvorbisdec, StaticCodebook *b);//vorbis_staticbook_clear
extern void voOGGDecFreeStaticbook(CVOVorbisObjectDecoder *pvorbisdec, StaticCodebook *b);//vorbis_staticbook_destroy
extern void voOGGDecClearVorbisBook(CVOVorbisObjectDecoder *pvorbisdec, CodeBook *b);	//vorbis_book_clear

extern long voOGGDecBookDecode(CodeBook *book, VOOGGInputBitStream *obs);
extern long voOGGDecBookDecodevSet(CodeBook *book, OGG_S32 *a, VOOGGInputBitStream *obs, int n, int point);//vorbis_book_decodev_set
extern int voOGGDecStaticbookUnpack(CVOVorbisObjectDecoder *pvorbisdec, VOOGGInputBitStream *obs, StaticCodebook *s);
extern long VorbisBookDecodevsAdd(CVOVorbisObjectDecoder *pvorbisdec, 
							  CodeBook *book,
							  OGG_S32 *a,
							  int n,
							  int point);//vorbis_book_decodevs_add
extern long VorbisBookDecodevAdd(CVOVorbisObjectDecoder *pvorbisdec,
							 CodeBook *book,
							 OGG_S32 *a,
							 int n,
							 int point);//vorbis_book_decodev_add
extern long VorbisBookDecodevvAdd(CVOVorbisObjectDecoder *pvorbisdec,
							  CodeBook *book,
							  OGG_S32 **a,
							  long offset,
							  int ch,
							  int n,
							  int point);//vorbis_book_decodevv_add
extern int VorbisBookDecodeInit(CVOVorbisObjectDecoder *pvorbisdec, CodeBook *c,const StaticCodebook *s);//vorbis_book_init_decode

//from block.h
extern void *voOGGDecBlockAlloc(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb, long bytes);
extern void voOGGDecBlockRipcord(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb);//_vorbis_block_ripcord
extern int  voOGGDecBlockClear(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb);//vorbis_block_clear
extern void voOGGDecDSPClear(CVOVorbisObjectDecoder *pvorbisdec, VorbisDSPState *v);//vorbis_dsp_clear

//for oggframing.c
//extern	ogg_sync_state *ogg_sync_create(CVOVorbisObjectDecoder *pvorbisdec);
extern	OGGStreamState *OGGStreamCreate(CVOVorbisObjectDecoder *pvorbisdec, int serialno);
extern  long  voOGGDecCheckHeader(CVOVorbisObjectDecoder *pvorbisdec);
extern  int   voOGGDecStreamResetSerialno(OGGStreamState *os, int serialno);
extern  OGG_U32 voOGGDecGetPacketSerialno(VOOGGInputBitStream *obs);
extern  int   voOGGDecStreamPageIn(OGGStreamState *os, VOOGGInputBitStream *obs);
//extern  int   StreamPacketOut(OGGStreamState *os, CVOVorbisObjectDecoder *pvorbisdec, OGGPacket *op);//ogg_stream_packetout
extern  int voOGGDecGetPacket(CVOVorbisObjectDecoder *pvorbisdec, OGGStreamState *os, OGGPacket *op);
extern int voOGGDecAudioPacket(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb, OGGPacket *op, int decodep);
extern  int voOGGDecSynthesisPcmout(VorbisDSPState *v, OGG_S32 ***pcm);	//vorbis_synthesis_pcmout
extern  void voOGGDecSaveHeadRestData(VOOGGInputBuffer *pInput);

extern  void VorbisInfoClear(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi);	//vorbis_info_clear
extern  void VorbisCommentClear(CVOVorbisObjectDecoder *pvorbisdec, VorbisComment *vc);//vorbis_comment_clear
extern  int ProcessPacketFile(CVOVorbisObjectDecoder *pvorbisdec, int readp, int spanp);
extern  int ProcessPacket(CVOVorbisObjectDecoder *pvorbisdec, int readp, int spanp);

extern  int voOGGDecSynthesisInit(CVOVorbisObjectDecoder *pvorbisdec, 
								VorbisDSPState *v, VorbisInfo *vi);	//vorbis_synthesis_init

#define OV_SUCCESS	  0
#define OV_PACKEND	  -4
#define OV_SPANLACE	  -5

#define OV_FALSE      -1  
#define OV_EOF        -2
#define OV_HOLE       -3

#define OV_EREAD      -128
#define OV_EFAULT     -129
#define OV_EIMPL      -130
#define OV_EINVAL     -131
#define OV_ENOTVORBIS -132
#define OV_EBADHEADER -133
#define OV_EVERSION   -134
#define OV_ENOTAUDIO  -135
#define OV_EBADPACKET -136
#define OV_EBADLINK   -137
#define OV_ENOSEEK    -138



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__VO_VORBIS_DEC_H_





