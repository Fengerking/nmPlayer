//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    oggframing.c

Abstract:

    decode Ogg streams back into raw packets.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/



#include "vovorbisdec.h"
#include "block.h"

/* Static CRC calculation table.  See older code in CVS for dead
   run-time initialization code. */

static OGG_U32 CRCLookup[256] = 
{
  0x00000000,0x04c11db7,0x09823b6e,0x0d4326d9,
  0x130476dc,0x17c56b6b,0x1a864db2,0x1e475005,
  0x2608edb8,0x22c9f00f,0x2f8ad6d6,0x2b4bcb61,
  0x350c9b64,0x31cd86d3,0x3c8ea00a,0x384fbdbd,
  0x4c11db70,0x48d0c6c7,0x4593e01e,0x4152fda9,
  0x5f15adac,0x5bd4b01b,0x569796c2,0x52568b75,
  0x6a1936c8,0x6ed82b7f,0x639b0da6,0x675a1011,
  0x791d4014,0x7ddc5da3,0x709f7b7a,0x745e66cd,
  0x9823b6e0,0x9ce2ab57,0x91a18d8e,0x95609039,
  0x8b27c03c,0x8fe6dd8b,0x82a5fb52,0x8664e6e5,
  0xbe2b5b58,0xbaea46ef,0xb7a96036,0xb3687d81,
  0xad2f2d84,0xa9ee3033,0xa4ad16ea,0xa06c0b5d,
  0xd4326d90,0xd0f37027,0xddb056fe,0xd9714b49,
  0xc7361b4c,0xc3f706fb,0xceb42022,0xca753d95,
  0xf23a8028,0xf6fb9d9f,0xfbb8bb46,0xff79a6f1,
  0xe13ef6f4,0xe5ffeb43,0xe8bccd9a,0xec7dd02d,
  0x34867077,0x30476dc0,0x3d044b19,0x39c556ae,
  0x278206ab,0x23431b1c,0x2e003dc5,0x2ac12072,
  0x128e9dcf,0x164f8078,0x1b0ca6a1,0x1fcdbb16,
  0x018aeb13,0x054bf6a4,0x0808d07d,0x0cc9cdca,
  0x7897ab07,0x7c56b6b0,0x71159069,0x75d48dde,
  0x6b93dddb,0x6f52c06c,0x6211e6b5,0x66d0fb02,
  0x5e9f46bf,0x5a5e5b08,0x571d7dd1,0x53dc6066,
  0x4d9b3063,0x495a2dd4,0x44190b0d,0x40d816ba,
  0xaca5c697,0xa864db20,0xa527fdf9,0xa1e6e04e,
  0xbfa1b04b,0xbb60adfc,0xb6238b25,0xb2e29692,
  0x8aad2b2f,0x8e6c3698,0x832f1041,0x87ee0df6,
  0x99a95df3,0x9d684044,0x902b669d,0x94ea7b2a,
  0xe0b41de7,0xe4750050,0xe9362689,0xedf73b3e,
  0xf3b06b3b,0xf771768c,0xfa325055,0xfef34de2,
  0xc6bcf05f,0xc27dede8,0xcf3ecb31,0xcbffd686,
  0xd5b88683,0xd1799b34,0xdc3abded,0xd8fba05a,
  0x690ce0ee,0x6dcdfd59,0x608edb80,0x644fc637,
  0x7a089632,0x7ec98b85,0x738aad5c,0x774bb0eb,
  0x4f040d56,0x4bc510e1,0x46863638,0x42472b8f,
  0x5c007b8a,0x58c1663d,0x558240e4,0x51435d53,
  0x251d3b9e,0x21dc2629,0x2c9f00f0,0x285e1d47,
  0x36194d42,0x32d850f5,0x3f9b762c,0x3b5a6b9b,
  0x0315d626,0x07d4cb91,0x0a97ed48,0x0e56f0ff,
  0x1011a0fa,0x14d0bd4d,0x19939b94,0x1d528623,
  0xf12f560e,0xf5ee4bb9,0xf8ad6d60,0xfc6c70d7,
  0xe22b20d2,0xe6ea3d65,0xeba91bbc,0xef68060b,
  0xd727bbb6,0xd3e6a601,0xdea580d8,0xda649d6f,
  0xc423cd6a,0xc0e2d0dd,0xcda1f604,0xc960ebb3,
  0xbd3e8d7e,0xb9ff90c9,0xb4bcb610,0xb07daba7,
  0xae3afba2,0xaafbe615,0xa7b8c0cc,0xa379dd7b,
  0x9b3660c6,0x9ff77d71,0x92b45ba8,0x9675461f,
  0x8832161a,0x8cf30bad,0x81b02d74,0x857130c3,
  0x5d8a9099,0x594b8d2e,0x5408abf7,0x50c9b640,
  0x4e8ee645,0x4a4ffbf2,0x470cdd2b,0x43cdc09c,
  0x7b827d21,0x7f436096,0x7200464f,0x76c15bf8,
  0x68860bfd,0x6c47164a,0x61043093,0x65c52d24,
  0x119b4be9,0x155a565e,0x18197087,0x1cd86d30,
  0x029f3d35,0x065e2082,0x0b1d065b,0x0fdc1bec,
  0x3793a651,0x3352bbe6,0x3e119d3f,0x3ad08088,
  0x2497d08d,0x2056cd3a,0x2d15ebe3,0x29d4f654,
  0xc5a92679,0xc1683bce,0xcc2b1d17,0xc8ea00a0,
  0xd6ad50a5,0xd26c4d12,0xdf2f6bcb,0xdbee767c,
  0xe3a1cbc1,0xe760d676,0xea23f0af,0xeee2ed18,
  0xf0a5bd1d,0xf464a0aa,0xf9278673,0xfde69bc4,
  0x89b8fd09,0x8d79e0be,0x803ac667,0x84fbdbd0,
  0x9abc8bd5,0x9e7d9662,0x933eb0bb,0x97ffad0c,
  0xafb010b1,0xab710d06,0xa6322bdf,0xa2f33668,
  0xbcb4666d,0xb8757bda,0xb5365d03,0xb1f740b4
};

OGGStreamState *OGGStreamCreate(CVOVorbisObjectDecoder *pvorbisdec, int serialno)
{
	OGGStreamState *os = (OGGStreamState *)voOGGCalloc(1, sizeof(OGGStreamState));//_ogg_calloc(1,sizeof(*os));
	if (!os)
		return NULL;
	os->m_SerialNo = serialno;
	os->m_PageNo = -1;
	return os;
} 

static int OggbyteInit(VOOGGInputBitStream	*oggbitstream, VOOGGInputBuffer *pInputBuffer)
{
//  voOGGMemset(b,0,sizeof(*b));
	if(oggbitstream)
	{
		if(!oggbitstream->m_Flag)
		{
			oggbitstream->m_ptr		= pInputBuffer->m_pNextPcktBgn;
			oggbitstream->m_PacketLength = 0;
			oggbitstream->m_Flag	= VO_TRUE;
		}
		oggbitstream->m_CBitUsed = 0;
		return 0;
	}else
		return -1;
}

static int OggbyteCurrent(VOOGGInputBitStream	*oggbitstream, VOOGGInputBuffer *pInputBuffer)
{
	if(oggbitstream && oggbitstream->m_Flag)
	{
		oggbitstream->m_ptr		= pInputBuffer->m_pNextPcktBgn - pInputBuffer->m_CurBlockLen;
		oggbitstream->m_PacketLength = 0;
		oggbitstream->m_CBitUsed = 0;
		return 0;
	}else
		return -1;
}

static void set4bytes(VOOGGInputBitStream *obs, VO_U32 val, int pos)
{
	obs->m_ptr[pos++] = (VO_U8)val;
	obs->m_ptr[pos++] = (VO_U8)(val>>8);
	obs->m_ptr[pos++] = (VO_U8)(val>>16);
	obs->m_ptr[pos] = (VO_U8)(val>>24);
}

static unsigned char read1byte(VOOGGInputBitStream	*obs, int pos){
//  _positionB(b,pos);
//  _positionF(b,pos);
	return obs->m_ptr[pos];//b->ptr[pos];
}

static OGG_U32 read4bytes(VOOGGInputBitStream	*obs, int pos)
{
    OGG_U32 ret;
    ret =  obs->m_ptr[pos++];
    ret |= obs->m_ptr[pos++]<<8;
    ret |= obs->m_ptr[pos++]<<16;
    ret |= obs->m_ptr[pos]<<24;
    return ret;
}

static OGG_S64 read8bytes(VOOGGInputBitStream	*obs, int pos)
{
    OGG_S64 ret, ret1 = 0;

	ret =  obs->m_ptr[pos++];
    ret |= obs->m_ptr[pos++]<<8;
    ret |= obs->m_ptr[pos++]<<16;
    ret |= obs->m_ptr[pos++]<<24;

	ret1 |= obs->m_ptr[pos++]<<0;
    ret1 |= obs->m_ptr[pos++]<<8;
    ret1 |= obs->m_ptr[pos++]<<16;
	ret1 |= obs->m_ptr[pos]<<24;

	ret1 <<= 16;

	ret |= (ret1<<16); 
    return ret;
}

static OGG_U32 CheckSums(VOOGGInputBuffer *pInbuffer, VO_U32 bytes)
{
	OGG_U32 CrcReg = 0;
	VO_U32  j;
	VO_U32  post;

	if(pInbuffer)
	{
		unsigned char *data = pInbuffer->m_pNextPcktBgn;
		post = bytes < pInbuffer->m_DataLength ? bytes : pInbuffer->m_DataLength;
		for(j = 0; j < post; ++j)
			CrcReg = (CrcReg<<8)^CRCLookup[((CrcReg >> 24)&0xff)^data[j]];
//		bytes -= j;
//		or = or->next;
	}
	return CrcReg;
}

#define PACKETHEADERSIZE	27
#define OGGTITLE			0x5367674f
/* sync the stream.  This is meant to be useful for finding page
   boundaries.

   return values for this:
  -n) skipped n bytes
   0) page not ready; more data (no bytes skipped)
   n) page synced at current location; page length n bytes
   
*/

long voOGGDecCheckHeader(CVOVorbisObjectDecoder *pvorbisdec)
{
//  oggbyte_buffer page;
	VOOGGInputBitStream	*obs = NULL; 
	VOOGGInputBuffer *oggbuffer = pvorbisdec->m_oggbuffer;
	VO_U32		blocklen = 0, bytes = oggbuffer->m_DataLength;
//	VO_U32		ret = 0;
	PacketStage	*pPcktStg = pvorbisdec->m_packetstage;
//  ogg_page_release(pvorbisdec, og);

//  bytes=oy->fifo_fill;
//	pPcktStg->m_pPageBgn = 
	OggbyteInit(pvorbisdec->m_oggbs, oggbuffer);
	obs = pvorbisdec->m_oggbs;
	if(pPcktStg->m_HeaderBytes == 0)
	{
		if(bytes < PACKETHEADERSIZE)goto check_out; /* not enough for even a minimal header */
    
		/* verify capture pattern */
		if(read4bytes(obs, 0) != OGGTITLE)
			goto check_fail;
//		if(read1byte(obs,0)!=(int)'O' ||
//			read1byte(&page,1)!=(int)'g' ||
//			read1byte(&page,2)!=(int)'g' ||
//			read1byte(&page,3)!=(int)'S'    ) goto sync_fail;

		pPcktStg->m_HeaderBytes = read1byte(obs,26) + PACKETHEADERSIZE;
//    oy->headerbytes=read1byte(obs,26) + MINHEADERSIZE;
	}
	if(bytes < pPcktStg->m_HeaderBytes)goto check_out; /* not enough for header +
                                             seg table */
	if(pPcktStg->m_BodyBytes == 0)
	{
		VO_U32 i;
		/* count up body length in the segment table */
		for(i=0; i < pPcktStg->m_HeaderBytes - PACKETHEADERSIZE; i++)
			pPcktStg->m_BodyBytes += read1byte(obs, PACKETHEADERSIZE + i);//计算body bytes
	}
	
	blocklen = pPcktStg->m_BodyBytes + pPcktStg->m_HeaderBytes;
	if(blocklen > bytes)
	{
		if(blocklen > oggbuffer->m_BufferSize)
		{
			//block length bigger than buffer size
			char *rebuffer = voOGGCalloc(1, blocklen);
			voOGGMemcpy(rebuffer, oggbuffer->m_pNextPcktBgn, oggbuffer->m_DataLength);
			voOGGFree(oggbuffer->m_pBuffer);
			oggbuffer->m_pBuffer = oggbuffer->m_pNextPcktBgn = (unsigned char *)rebuffer;
			oggbuffer->m_pDataEnd = oggbuffer->m_pBuffer + oggbuffer->m_DataLength;
		}
//		else if(blocklen > oggbuffer->m_BufferSize - (oggbuffer->m_pNextPcktBgn - oggbuffer->m_pBuffer))
//		{
			//block length bigger than the rest of buffer
//			BufferReset(oggbuffer);			
//		}
		goto check_out;
	}

	/* we have what appears to be a complete page; last test: verify
		checksum */ /* 进行校验 */
	{
		OGG_U32 chksum = read4bytes(obs, 22);
		set4bytes(obs, 0, 22);

		/* Compare checksums; memory continues to be common access */
		if(chksum != CheckSums(oggbuffer, pPcktStg->m_BodyBytes + pPcktStg->m_HeaderBytes))
		{
      
			/* D'oh.  Mismatch! Corrupt page (or miscapture and not a page
				at all). replace the computed checksum with the one actually
				read in; remember all the memory is common access */
      
			set4bytes(obs, chksum, 22);
			goto check_fail;
		}
		set4bytes(obs, chksum, 22);
	}
	pPcktStg->m_pPageBgn = obs->m_ptr;
	pPcktStg->m_NextPacketBgn  = pPcktStg->m_HeaderBytes;
	oggbuffer->m_CurBlockLen = blocklen;
	oggbuffer->m_pNextPcktBgn = oggbuffer->m_pNextPcktBgn + oggbuffer->m_CurBlockLen;

	return oggbuffer->m_CurBlockLen;

check_fail:

	pPcktStg->m_HeaderBytes	= 0;
	pPcktStg->m_BodyBytes	= 0;
	return -1;
//	cleaninputbuffer();

check_out:
	return 0;
}

int voOGGDecStreamReset(OGGStreamState *os)
{


	//  ogg_buffer_release(os->header_tail);
	//  ogg_buffer_release(os->body_tail);
	//  os->header_tail=os->header_head=0;
	//  os->body_tail=os->body_head=0;

	os->m_EOS = 0;
	os->m_BOS = 0;
	os->m_PageNo = -1;
	os->m_PacketNo = 0;
	os->m_GranulePos = 0;

	os->m_LaceEnd = 0;//>body_fill = 0;
	os->m_LaceSize = 0;//>lacing_fill = 0;

	os->m_InitFlag = VO_TRUE;//>holeflag = 0;
	os->m_SpanLaceStore = 0;
//	os->clearflag = 0;
	os->m_Laceptr = 0;//>laceptr = 0;
//	os->body_fill_next = 0;
	os->m_ResetFlag = VO_FALSE;
	os->m_CurLaceLength = 0;
	
	return OGG_SUCCESS;
}

/* Now we get to the actual framing code */

static int GetPacketVersion(VOOGGInputBitStream *obs)
{
    return read1byte(obs, 4);
}

static int GetPacketContinued(VOOGGInputBitStream *obs)
{
//  oggbyte_buffer ob;
//  oggbyte_init(&ob,og->header);
    return read1byte(obs, 5) & 0x01;
}

static int GetPacketBos(VOOGGInputBitStream *obs)
{
//  oggbyte_buffer ob;
//  oggbyte_init(&ob,og->header);
    return read1byte(obs, 5) & 0x02;
}

static int GetPacketEos(VOOGGInputBitStream *obs)
{
//  oggbyte_buffer ob;
//  oggbyte_init(&ob,og->header);
    return read1byte(obs, 5) & 0x04;
}

OGG_U32 voOGGDecGetPacketSerialno(VOOGGInputBitStream *obs)
{
    return read4bytes(obs, 14);
}

static OGG_U32 GetPacketno(VOOGGInputBitStream *obs)
{
//  oggbyte_buffer ob;
//  oggbyte_init(&ob,og->header);
    return read4bytes(obs, 18);
}

static OGG_S64 GetPacketGranulepos(VOOGGInputBitStream *obs)
{
//  oggbyte_buffer ob;
//  oggbyte_init(&ob,og->header);
    return read8bytes(obs, 6);
}

int voOGGDecStreamResetSerialno(OGGStreamState *os, int serialno)
{
    voOGGDecStreamReset(os);
    os->m_SerialNo = serialno;
    return OGG_SUCCESS;
}

#define FINFLAG 0x80000000UL
#define FINMASK 0x7fffffffUL

int voOGGDecStreamPageIn(OGGStreamState *os, VOOGGInputBitStream *obs)
{

    int serialno = voOGGDecGetPacketSerialno(obs);
    int version  = GetPacketVersion(obs);

    /* check the serial number */
    if (serialno != os->m_SerialNo)
	{
        return OGG_ESERIAL;
    }
    
    if (version > 0)
	{
        return OGG_EVERSION;
    }

    return OGG_SUCCESS;
}

static void GetCurrentLaceLength(OGGStreamState *os, VOOGGInputBitStream *obs, OGGPacket *op)
{
//	os->m_LaceEnd = VO_FALSE;
	while(os->m_Laceptr < os->m_LaceSize)
	{
		int val = os->m_LaceSep[os->m_Laceptr++];//read1byte(obs, 27+os->m_Laceptr++);
		os->m_CurLaceLength += val;
		if(val < 255)
		{
			if(!os->m_CurLaceLength)
				continue;
//			os->m_CurLaceLength |= FINFLAG;
			os->m_LaceEnd = VO_TRUE;
			break;
		}
	}
}

int voOGGDecGetPacket(CVOVorbisObjectDecoder *pvorbisdec, OGGStreamState *os, OGGPacket *op)
{
	VOOGGInputBitStream *obs = pvorbisdec->m_oggbs;
	int continued = 0;
	if(os->m_InitFlag)
	{
		//parse the header, only once for every block, to check the block.
		int pageno = GetPacketno(obs);
		os->m_Laceptr = 0;
		os->m_LaceEnd = VO_FALSE;
		
		os->m_LaceSize = read1byte(obs, 26);
//		if(os->m_LaceSize < 0)
//			os->m_LaceSize = 0;
		voOGGMemcpy(&os->m_LaceSep, &obs->m_ptr[27], os->m_LaceSize);//save lace sequence

		if(pageno != os->m_PageNo)
		{
			if(os->m_PageNo == -1)
				os->m_ResetFlag = VO_TRUE;//reset or seek
			else if(pvorbisdec->m_HeaderRead != 1)
			{
				//maybe packet 2 is cut!
				return OV_EFAULT;//lose block error
			}

		}

		if ((continued = GetPacketContinued(obs)) == 1)
		{
			if(os->m_CurLaceLength == 0)
				if(!os->m_ResetFlag)
					return OV_EFAULT;/*当前segment为跨page的（c为1），但是前一个block的最后一个
									 却不是跨block的（length为0），证明丢了一个block;*/
		}
		else
		{
			if(os->m_CurLaceLength > 0)
				return OV_EFAULT;//当前block的第一个lace并非跨block（c为0），前一个block的最后一个lace为跨block的（length大于0），证明至少丢掉了一个block
		}
		
		if (os->m_Laceptr < os->m_LaceSize)
		{
            os->m_GranulePos = GetPacketGranulepos(obs);
			GetCurrentLaceLength(os, obs, op);
		}

		os->m_PageNo = pageno + 1;
        os->m_EOS  = GetPacketEos(obs);
	    os->m_BOS  = GetPacketBos(obs);
		os->m_InitFlag = VO_FALSE;
	}
	else 
	{
		OggbyteCurrent(obs, pvorbisdec->m_oggbuffer);
		GetCurrentLaceLength(os, obs, op);
	}

	if(op)
	{
		op->m_BOS = os->m_BOS;
		op->m_EOS = os->m_EOS;			//不太清楚用处，暂时这样	Witten
		if(os->m_LaceEnd)
			op->m_GranulePos = os->m_GranulePos;//不太清楚用处，暂时这样	Witten
		else
			op->m_GranulePos = -1;
		op->m_PacketNo = os->m_PacketNo;
	}
	if(os->m_LaceEnd)
	{
		if(!continued || os->m_ResetFlag)
		{
			if(op->m_PacketStage->m_pSpanPacketBuf != 0)
			{
				op->m_PacketStage->m_pSpanPacketBuf = op->m_PacketStage->m_pSpanPacketEnd = 0;
				os->m_SpanLaceStore = 0;
			}
			op->m_PacketStage->m_PacketBegin = op->m_PacketStage->m_NextPacketBgn;
//			op->bytes = os->m_CurLaceLength;
			op->m_PacketStage->m_NextPacketBgn += os->m_CurLaceLength;//op->bytes;
			op->m_PacketStage->m_PacketLength = os->m_CurLaceLength;//op->bytes;
			os->m_CurLaceLength = 0;
			if(os->m_ResetFlag)
			{
				if(continued)
				{
					/*Reset the continued page, so the 1st segment must throw away. 
					And we will read next segment.*/
					GetCurrentLaceLength(os, obs, op);

					op->m_PacketStage->m_PacketBegin = op->m_PacketStage->m_NextPacketBgn;
//					op->bytes = os->m_CurLaceLength;
					op->m_PacketStage->m_NextPacketBgn += os->m_CurLaceLength;//op->bytes;
					op->m_PacketStage->m_PacketLength = os->m_CurLaceLength;//op->bytes;
					os->m_CurLaceLength = 0;
				}
				os->m_ResetFlag = VO_FALSE;
			}
		}else{
			//save the current packet head
			voOGGMemcpy(op->m_PacketStage->m_pSpanPacketEnd, 
				op->m_PacketStage->m_pPageBgn + op->m_PacketStage->m_NextPacketBgn,
				os->m_CurLaceLength - os->m_SpanLaceStore);
			op->m_PacketStage->m_NextPacketBgn += os->m_CurLaceLength - os->m_SpanLaceStore;
			op->m_PacketStage->m_PacketLength = os->m_CurLaceLength;
			op->m_PacketStage->m_pSpanPacketEnd = op->m_PacketStage->m_pSpanPacketBuf + os->m_CurLaceLength;
//			op->m_PacketStage->m_spanlcstr = os->m_SpanLaceStore;
			os->m_CurLaceLength = os->m_SpanLaceStore = 0;
		}
//		if(os->m_Laceptr == os->m_LaceSize)
//			os->m_InitFlag = VO_TRUE;
		os->m_LaceEnd = VO_FALSE;
		os->m_PacketNo++;
		return	OV_SUCCESS;
	}
	else
	{
		os->m_InitFlag = VO_TRUE;
		if(os->m_CurLaceLength > 0)
		{
			if(os->m_SpanLaceStore == 0)
			{
				op->m_PacketStage->m_pSpanPacketBuf = op->m_PacketStage->m_pPageBgn + op->m_PacketStage->m_NextPacketBgn;
				op->m_PacketStage->m_pSpanPacketEnd = op->m_PacketStage->m_pSpanPacketBuf + os->m_CurLaceLength;
			}
			else
			{
				voOGGMemcpy(op->m_PacketStage->m_pSpanPacketEnd, 
					op->m_PacketStage->m_pPageBgn + op->m_PacketStage->m_NextPacketBgn, 
					os->m_CurLaceLength - os->m_SpanLaceStore);
				op->m_PacketStage->m_pSpanPacketEnd = op->m_PacketStage->m_pSpanPacketBuf + os->m_CurLaceLength;
			}
			os->m_SpanLaceStore = os->m_CurLaceLength;
			return OV_SPANLACE;
		}
		return	OV_PACKEND;
	}	
	os->m_BOS = 0;
}


int ProcessPacketFile(CVOVorbisObjectDecoder *pvorbisdec, int readp, int spanp)
{
	int ret = 0;

	/* handle one packet.  Try to fetch it from current stream state */
	/* extract packets from page */
	/* process a packet if we can.  If the machine isn't loaded,
	neither is a page */
	if (pvorbisdec->m_ReadyState==INITSET)
	{
//		int result = StreamPacketOut(pvorbisdec->os, pvorbisdec, pvorbisdec->op);
		int result = voOGGDecGetPacket(pvorbisdec, pvorbisdec->os, pvorbisdec->op);
		OGG_S64 granulepos;
//		if(result<0)
//		{
//			ret = OV_HOLE; /* hole in the data. */
//			goto cleanup;
//		}

		if (result==OV_SUCCESS)
		{
			/* got a packet.  process it */
			granulepos = pvorbisdec->op->m_GranulePos;
			ret = voOGGDecAudioPacket(pvorbisdec, &pvorbisdec->vb, pvorbisdec->op, 1);
			if(!ret)
			{ 			
				
				pvorbisdec->samptrack += voOGGDecSynthesisPcmout(&pvorbisdec->vd,  NULL) ;

				/* update the pcm offset. */
				if (granulepos!=-1 && !pvorbisdec->op->m_EOS)
				{
					int link = 0;//= (pvorbisdec->m_Seekable ? pvorbisdec->current_link : 0);
					int i, samples;

					/* this packet has a pcm_offset on it (the last packet
					completed on a page carries the offset) After processing
					(above), we know the pcm position of the *last* sample
					ready to be returned. Find the offset of the *first*

					As an aside, this trick is inaccurate if we begin
					reading anew right at the last page; the end-of-stream
					granulepos declares the last frame in the stream, and the
					last packet of the last page may be a partial frame.
					So, we need a previous granulepos from an in-sequence page
					to have a reference point.  Thus the !op.e_o_s clause
					above */

					if (pvorbisdec->m_Seekable && link>0)
						granulepos -= pvorbisdec->pcmlengths[link*2];
					if (granulepos<0)
						granulepos = 0; /* actually, this shouldn't be possible
										here unless the stream is very broken */

					samples = voOGGDecSynthesisPcmout(&pvorbisdec->vd, NULL);

					granulepos -= samples;
					for(i=0; i<link; i++)
						granulepos += pvorbisdec->pcmlengths[i*2+1];
//					pvorbisdec->pcm_offset = granulepos;
				}
				ret = OV_SUCCESS;
				goto cleanup;
			}
		}
		else
		{
			ret = result;
		}
	}

cleanup:
//	ogg_packet_release(&op);
//	ogg_page_release(&og);
	return ret;
}

/*
 got a packet.  process it , maybe header packet or audio packet
 */
int ProcessPacket(CVOVorbisObjectDecoder *pvorbisdec, int readp, int spanp)
{

	int ret = 0;
	

	if(pvorbisdec->m_PacketNo < 3)
	{
		/* got a header packet.  process it */
		char buffer[6];
		VOOGGInputBitStream obs_T, *obs;
		int packtype;

		obs = pvorbisdec->m_oggbs;

		obs_T.m_ptr = obs->m_ptr;
		obs_T.m_CBitUsed = obs->m_CBitUsed;
		obs_T.m_PacketLength = obs->m_PacketLength;

		packtype = voOGGDecPackReadBits(obs, 8);
		if(packtype == 1 || packtype == 3 || packtype == 5)
		{
			voOGGMemset(buffer, 0, 6);
			ReadString(obs, buffer, 6);
			if(!voOGGMemcmp(buffer, "vorbis", 6))
			{
				/* this is a vorbis header, skip it! */
				pvorbisdec->m_PacketNo++;
				return (OV_ENOTAUDIO);
			}
		}
		obs->m_ptr = obs_T.m_ptr;
		obs->m_CBitUsed = obs_T.m_CBitUsed;
		obs->m_PacketLength = obs_T.m_PacketLength;
	}

	/* got a audio packet.  process it */
	ret = voOGGDecAudioPacket(pvorbisdec, &pvorbisdec->vb, pvorbisdec->op, 1);
	if(!ret)
	{ 	
		pvorbisdec->samptrack += voOGGDecSynthesisPcmout(&pvorbisdec->vd,  NULL) ;			
	}

	pvorbisdec->m_PacketNo++;
	return ret;
}

