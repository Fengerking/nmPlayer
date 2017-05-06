//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    info.c

Abstract:

    Vorbis infomation decode file, maintain the info structure, info <-> header packets.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/

#include "functionreg.h"
#include "vovorbisdec.h"
#include "ogg.h"

/* helpers */
void ReadString(VOOGGInputBitStream *obs, char *buf, int bytes)//_v_readstring
{
    while(bytes--)
	{
        *buf++ = (char)voOGGDecPackReadBits(obs, 8);
    }
}

/* used by synthesis, which has a full, alloced vi */
int voOGGDecInfoInit(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi)
{
    voOGGMemset(vi, 0, sizeof(VorbisInfo));//memset(vi,0,sizeof(*vi));

    vi->m_pCodecSetup = (CodecSetupInfo *)voOGGCalloc(1, sizeof(CodecSetupInfo));//_ogg_calloc(1,sizeof(codec_setup_info));
    if(!vi->m_pCodecSetup)
	    return VO_ERR_OUTOF_MEMORY;

    return VO_ERR_NONE;
}

void VorbisInfoUnint(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi)
{
	if(vi->m_pCodecSetup)
	{
		voOGGFree(vi->m_pCodecSetup);
		vi->m_pCodecSetup = NULL;
	}
}

void VorbisCommentInit(VorbisComment *vc)
{
  voOGGMemset(vc,0,sizeof(VorbisComment));//memset(vc,0,sizeof(*vc));
}

/* Header packing/unpacking ********************************************/

void VorbisCommentClear(CVOVorbisObjectDecoder *pvorbisdec, VorbisComment *vc)//vorbis_comment_clear
{
    if(vc)
    {
        long i;
        for(i = 0; i < vc->m_Comments; i++)
            if (vc->m_ppUserComments[i]) 
				voOGGFree(vc->m_ppUserComments[i]);
        if (vc->m_ppUserComments)
			voOGGFree(vc->m_ppUserComments);
	    if (vc->m_pCommentLengths)
			voOGGFree(vc->m_pCommentLengths);
        if (vc->m_pVendor)
			voOGGFree(vc->m_pVendor);
        voOGGMemset(vc,0,sizeof(*vc));
    }
}

static int VorbisUnpackComment(CVOVorbisObjectDecoder *pvorbisdec, VorbisComment  *vc, VOOGGInputBitStream *obs)
{
	int i;
	int vendorlen = voOGGDecPackReadBits(obs, 32);
	if (vendorlen < 0)
		goto err_out;
	vc->m_pVendor = (char *)voOGGCalloc(vendorlen+1, 1);
	if(!vc->m_pVendor){
		goto err_out;
	}
	ReadString(obs, vc->m_pVendor, vendorlen);
#if 1
	vc->m_Comments = voOGGDecPackReadBits(obs, 32);
	if (vc->m_Comments < 0)
		goto err_out;

	vc->m_ppUserComments = (char **)voOGGCalloc(vc->m_Comments+1, sizeof(*vc->m_ppUserComments));
	vc->m_pCommentLengths=(int *)voOGGCalloc(vc->m_Comments+1,  sizeof(*vc->m_pCommentLengths));
	if(!vc->m_ppUserComments||!vc->m_pCommentLengths){
		goto err_out;
	}
	
	for (i = 0; i < vc->m_Comments; i++)
	{
		int len = voOGGDecPackReadBits(obs, 32);
		if (len < 0)
			goto err_out;
		vc->m_pCommentLengths[i] = len;
		vc->m_ppUserComments[i] = (char *)voOGGCalloc(len + 1, 1);
		ReadString(obs, vc->m_ppUserComments[i], len);
	}	  
	if(voOGGDecPackReadBits(obs, 1) != 1)
		goto err_out; /* EOP check */

	if(pvorbisdec->m_fileformat == 0)
		voOGGDecResetOBS2Packet(obs);
#endif
	return(0);
err_out:
	VorbisCommentClear(pvorbisdec, vc);
	return(OV_EBADHEADER);
}

void RestOGGStreamStateSpan(OGGStreamState *os)
{
	os->m_Laceptr = os->m_LaceSize = 0;
	os->m_InitFlag = VO_TRUE;
}

void voOGGDecResetOGGStreamState(OGGStreamState *os)
{
	if(os->m_Laceptr == os->m_LaceSize)
	{
		os->m_Laceptr = os->m_LaceSize = 0;
		os->m_InitFlag = VO_TRUE;
	}
}

static int VorbisUnpackInfo(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi, VOOGGInputBitStream *obs)
{
	CodecSetupInfo     *ci = (CodecSetupInfo *)vi->m_pCodecSetup;
	if (!ci)
		return(OV_EFAULT);
	
	vi->m_Version = voOGGDecPackReadBits(obs, 32);
	if (vi->m_Version != 0) 
		return(OV_EVERSION);

	vi->m_SampleBits = 16;
	vi->m_Channels = voOGGDecPackReadBits(obs, 8);
	vi->m_Rate = voOGGDecPackReadBits(obs, 32);
	
	vi->m_BitrateUp = voOGGDecPackReadBits(obs, 32);
	vi->m_BitrateNom = voOGGDecPackReadBits(obs, 32);
	vi->m_BitrateLow = voOGGDecPackReadBits(obs, 32);
	
	ci->m_BlockSize[0] = 1<<voOGGDecPackReadBits(obs, 4);
	ci->m_BlockSize[1] = 1<<voOGGDecPackReadBits(obs, 4);
	
	if (vi->m_Rate < 1)
		goto err_out;
	if (vi->m_Channels < 1)
		goto err_out;
	if (ci->m_BlockSize[0] < 64)
		goto err_out; 
	if (ci->m_BlockSize[1] < ci->m_BlockSize[0])
		goto err_out;
	if (ci->m_BlockSize[1] > 8192)
		goto err_out;
	
	if (voOGGDecPackReadBits(obs, 1) != 1)
		goto err_out; /* EOP check */
	
	//the block is end, and set the ogg input bit stream flag to false to reset.
	if(pvorbisdec->m_fileformat == 0)
		voOGGDecResetOBS2Packet(obs);
	else
	{
		voOGGDecSetNextBlock(pvorbisdec);
		voOGGDecResetOGGStreamState(pvorbisdec->os);
	}
	return(0);

err_out:
//	vorbis_info_clear(vi);
	return(OV_EBADHEADER);
}

void VorbisInfoClear(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi)	//vorbis_info_clear
{
	CodecSetupInfo     *ci = (CodecSetupInfo *)vi->m_pCodecSetup;
	int i;

	if(ci)
	{

		for(i=0; i<ci->m_Modes; i++)
			if(ci->m_pModeParam[i])
				voOGGFree(ci->m_pModeParam[i]);

		for(i=0; i<ci->m_Maps; i++) /* unpack does the range checking */
			if (ci->m_pMapParam[i])
				Mapping_P[ci->m_MapType[i]]->free_info(pvorbisdec, ci->m_pMapParam[i]);

		for(i=0; i<ci->m_Floors; i++) /* unpack does the range checking */
			if (ci->m_pFloorParam[i])
				Floor_P[ci->m_FloorType[i]]->free_info(pvorbisdec, ci->m_pFloorParam[i]);

		for(i=0; i<ci->m_Residues; i++) /* unpack does the range checking */
			if (ci->m_pResidueParam[i])
				Residue_P[ci->m_ResidueType[i]]->free_info(pvorbisdec, ci->m_pResidueParam[i]);

		for(i=0; i<ci->m_Books; i++)
		{
			if (ci->m_pBookParam[i])
			{
				/* knows if the book was not alloced */
				voOGGDecFreeStaticbook(pvorbisdec, ci->m_pBookParam[i]);
			}
			if (ci->m_pFullbooks)
				voOGGDecClearVorbisBook(pvorbisdec, ci->m_pFullbooks+i);
		}
		if (ci->m_pFullbooks)
			voOGGFree(ci->m_pFullbooks);

		voOGGFree(ci);
	}

	voOGGMemset(vi, 0, sizeof(*vi));
}

/* all of the real encoding details are here.  The modes, books,
   everything. refer to [4.2.4. Setup header] in SPEC for more details */
static int VorbisUnpackBooks(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi, VOOGGInputBitStream *obs)
{
	CodecSetupInfo     *ci = (CodecSetupInfo *)vi->m_pCodecSetup;
	int i;
	if (!ci)
		return(OV_EFAULT);
	
	/* codebooks */
	ci->m_Books = voOGGDecPackReadBits(obs, 8) + 1;
	/*ci->book_param=_ogg_calloc(ci->books,sizeof(*ci->book_param));*/
	for(i=0; i<ci->m_Books; i++)
	{
		ci->m_pBookParam[i] = (StaticCodebook *)voOGGCalloc(1, sizeof(StaticCodebook));	
		if(voOGGDecStaticbookUnpack(pvorbisdec, obs, ci->m_pBookParam[i])){
			goto err_out;
		}
	}
	
	/* time backend settings */
	ci->m_Times = voOGGDecPackReadBits(obs, 6) + 1;
	for(i=0; i<ci->m_Times; i++)
	{
		ci->m_TimeType[i] = voOGGDecPackReadBits(obs, 16);  //only placeholders in Vorbis I, value should be 0
		if (ci->m_TimeType[i]<0 || ci->m_TimeType[i]>=VI_TIMEB){ 
			goto err_out;
		}

	}
	
	/* floor backend settings */
	ci->m_Floors = voOGGDecPackReadBits(obs, 6) + 1;
	for(i=0; i<ci->m_Floors; i++)
	{
		ci->m_FloorType[i] = voOGGDecPackReadBits(obs, 16);
		if (ci->m_FloorType[i]<0 || ci->m_FloorType[i]>=VI_FLOORB){
			goto err_out;
		}
		ci->m_pFloorParam[i] = Floor_P[ci->m_FloorType[i]]->unpack(pvorbisdec, vi, obs); 
		if (!ci->m_pFloorParam[i]){
			goto err_out;
		}
	}
	
	/* residue backend settings */
	ci->m_Residues = voOGGDecPackReadBits(obs, 6) + 1;
	for(i=0; i<ci->m_Residues; i++)
	{
		ci->m_ResidueType[i] = voOGGDecPackReadBits(obs, 16);
		if (ci->m_ResidueType[i]<0 || ci->m_ResidueType[i]>=VI_RESB){
			goto err_out;
		}
		ci->m_pResidueParam[i] = Residue_P[ci->m_ResidueType[i]]->unpack(pvorbisdec, vi, obs);
		if (!ci->m_pResidueParam[i]){
			goto err_out;
		}
	}
	
	/* map backend settings */
	ci->m_Maps = voOGGDecPackReadBits(obs, 6) + 1;
	for(i=0; i<ci->m_Maps; i++)
	{
		ci->m_MapType[i] = voOGGDecPackReadBits(obs, 16);
		if (ci->m_MapType[i]<0 || ci->m_MapType[i]>=VI_MAPB){
			goto err_out;
		}
		ci->m_pMapParam[i] = Mapping_P[ci->m_MapType[i]]->unpack(pvorbisdec, vi);
		if (!ci->m_pMapParam[i]){
			goto err_out;
		}
	}
	
	/* mode settings */
	ci->m_Modes = voOGGDecPackReadBits(obs, 6) + 1;
	for (i=0; i<ci->m_Modes; i++)
	{
		ci->m_pModeParam[i] = (VorbisInfoMode *)voOGGCalloc(1, sizeof(*ci->m_pModeParam[i]));
		ci->m_pModeParam[i]->m_BlockFlag = voOGGDecPackReadBits(obs, 1);
		ci->m_pModeParam[i]->m_WindowType = voOGGDecPackReadBits(obs, 16);
		ci->m_pModeParam[i]->m_TransformType = voOGGDecPackReadBits(obs, 16);
		ci->m_pModeParam[i]->m_Mapping = voOGGDecPackReadBits(obs, 8);
		
		if (ci->m_pModeParam[i]->m_WindowType >= VI_WINDOWB){
			goto err_out;
		}
		if (ci->m_pModeParam[i]->m_TransformType >= VI_WINDOWB){
			goto err_out;
		}
		if (ci->m_pModeParam[i]->m_Mapping >= ci->m_Maps){
			goto err_out;
		}
	}
	
	if(voOGGDecPackReadBits(obs, 1)!=1){
		goto err_out; /* top level EOP check */
	}
	
	
	if(pvorbisdec->m_fileformat == 0)
		voOGGDecResetOBS2Packet(obs);
	else
	{
		//the block is end, and set the ogg input bit stream flag to false to reset.
		if(pvorbisdec->os->m_Laceptr == pvorbisdec->os->m_LaceSize)
		{
			voOGGDecSetNextBlock(pvorbisdec);
		}
		voOGGDecResetOGGStreamState(pvorbisdec->os);
	}

	return(0);
err_out:
	VorbisInfoClear(pvorbisdec, vi);
	return(OV_EBADHEADER);
}

#if 1
/* The Vorbis header is in three packets; the initial small packet in
   the first page that identifies basic parameters, a second packet
   with bitstream comments and a third packet that holds the
   codebook. */

int voOGGDecHeaderParser(CVOVorbisObjectDecoder *pvorbisdec)
{
//	oggpack_buffer opb;
	//int result = 0;
	VorbisInfo  *vi = pvorbisdec->vi;
	VorbisComment  *vc = pvorbisdec->vc;
	VOOGGInputBitStream *obs = pvorbisdec->m_oggbs;
	OGGPacket  *op = pvorbisdec->op;

	if(op){
		if(pvorbisdec->m_fileformat == 1)
			voOGGDecPackReadinit(obs, op);
		
		/* Which of the three types of header is this? */
		/* Also verify header-ness, vorbis */
		{
			char buffer[6];
			int packtype = voOGGDecPackReadBits(obs, 8);
			voOGGMemset(buffer, 0, 6);
			ReadString(obs, buffer, 6);
			if(voOGGMemcmp(buffer, "vorbis", 6))
			{
				/* not a vorbis header */
				return(OV_ENOTVORBIS);
			}
			switch(packtype)
			{
			case 0x01: /* least significant *bit* is read first */
				if(pvorbisdec->m_fileformat == 1)
					if (!op->m_BOS)
					{
						/* Not the initial packet */
						return(OV_EBADHEADER);
					}
				if (vi->m_Rate!=0)
				{
					/* previously initialized info header */
					return(OV_EBADHEADER);
				}
				
				return (VorbisUnpackInfo(pvorbisdec, vi, obs));//解析初始化包
				
				
			case 0x03: /* least significant *bit* is read first */
				if (vi->m_Rate == 0){
					/* um... we didn't get the initial header */
					return(OV_EBADHEADER);
				}
				
				return(VorbisUnpackComment(pvorbisdec, vc, obs));
				
			case 0x05: /* least significant *bit* is read first */
				if(vi->m_Rate==0 || vc->m_pVendor==NULL){
					/* um... we didn;t get the initial header or comments yet */
					return(OV_EBADHEADER);
				}
				
				return (VorbisUnpackBooks(pvorbisdec, vi, obs));

			default:
				/* Not a valid vorbis header type */
				return(OV_EBADHEADER);
				break;
			}
		}
	}
	return(OV_EBADHEADER);
}
#endif
int OGGBufferInit(CVOVorbisObjectDecoder *pvorbisdec)
{
	VOOGGInputBuffer *InputBuffer = (VOOGGInputBuffer *)pvorbisdec->m_oggbuffer;

	if((InputBuffer->m_pBuffer    = voOGGMalloc(INPUTBUFFERSIZE)) == NULL)
		return VO_ERR_OUTOF_MEMORY;
	voOGGMemset(InputBuffer->m_pBuffer, 0, INPUTBUFFERSIZE);

	InputBuffer->m_pNextPcktBgn = InputBuffer->m_pDataEnd = InputBuffer->m_pBuffer;
	InputBuffer->m_BufferSize   = INPUTBUFFERSIZE;

	return VO_ERR_NONE;
}

void OGGBufferUnint(CVOVorbisObjectDecoder *pvorbisdec)
{
	VOOGGInputBuffer *InputBuffer = (VOOGGInputBuffer *)pvorbisdec->m_oggbuffer;
	if(InputBuffer->m_pBuffer)
	{
		voOGGFree(InputBuffer->m_pBuffer);
		InputBuffer->m_pBuffer = NULL;
	}
}
