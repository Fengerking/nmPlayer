/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010	    *
*									                                    *
************************************************************************/
/***********************************************************************
File:		voAPEDecompress.c

Contains:	APE Decompress all functions

Written by:	Huaping Liu

Change History (most recent first):
2009-12-11		LHP			Create file

************************************************************************/
#include  <stdio.h>
#include  "voAPE_Global.h"
#include  "voAPETable.h"

/*one APE frame start flush or mark operation */
static void voAPEFrameStartFlush(voDecodeBytes *Dec_Pro)
{
	int m_nCurrentBitIndex;
	int nMod;

	m_nCurrentBitIndex = Dec_Pro->m_nCurrentBitIndex;

	/* make sure APE frame start Byte alignment */
	nMod = m_nCurrentBitIndex % 8;
	if(nMod !=0 )
		m_nCurrentBitIndex += 8 - nMod;  
	/*ignore the first byte... (slows compression too much to not output this dummy byte)*/
	voDecodeValueXBits(Dec_Pro, 8);  

	Dec_Pro->m_RangeCoderInfo.buffer = voDecodeValueXBits(Dec_Pro, 8);  
	Dec_Pro->m_RangeCoderInfo.low = Dec_Pro->m_RangeCoderInfo.buffer >> (8 - EXTRA_BITS);
	Dec_Pro->m_RangeCoderInfo.range = (unsigned int) 1 << EXTRA_BITS;

}

//EndFrame will normalize m_RangeCoderInfo structrue, update for next frame decode.
static void voFinalize(voDecodeBytes   *Dec_Pro, int m_nVersion)
{

	//normalize
	while(Dec_Pro->m_RangeCoderInfo.range <= BOTTOM_VALUE)
	{
		Dec_Pro->m_nCurrentBitIndex += 8;
		Dec_Pro->m_RangeCoderInfo.range <<= 8;
	}
	// used to back-pedal the last two bytes out
	// this should never have been a problem because we've outputted and normalized beforehand
	// but stopped doing it as of 3.96 in case it accounted for rare decompression failures
	if(m_nVersion <= 3950)
		Dec_Pro->m_nCurrentBitIndex -= 16;

}

//When start frame, must check CRC, will inital some state
static void  voStartFrame(voAPE_GData *st)
{
	int nVersion;
	int nComplevel;
	voCNNFState     *Xcnnf = st->Xcnnf;
	voCNNFState     *Ycnnf = st->Ycnnf;
	voDecodeBytes   *Dec_Proc = st->DecProcess;
	voAPEBlocks     *Blk_Info  = &(Dec_Proc->Blk_State);
	voAPE_Framehdr  *FrameHdr  = &(Dec_Proc->m_Framehdr);

	nVersion   = st->ape_info->nVersion;                       /* Get the Version info */
	nComplevel = st->ape_info->nCompressionLevel;              /* Get the Compression level */
	FrameHdr->m_nCRC = 0xFFFFFFFF;                             /* Init m_nCRC = 0xFFFFFFFF */

	//get the frame header
	FrameHdr->m_nStoredCRC = voDecodeValue(DECODE_VALUE_METHOD_UNSIGNED_INT, Dec_Proc);
	Blk_Info->m_bErrorDecodingCurrentFrame = 0;   //FALSE

	Blk_Info->m_nErrorDecodingCurrentFrameOutputSilenceBlocks = 0;

	//get any 'special' codes if the file uses them (for silence, FALSE stereo, etc)
	FrameHdr->m_nSpecialCodes = 0;
	//printf("nBlocks = %d\n", st->DecProcess->Blk_State.m_nCurrentFrame);

	if(nVersion > 3820)
	{
		if(FrameHdr->m_nStoredCRC & 0x80000000)
		{
			FrameHdr->m_nSpecialCodes = voDecodeValue(DECODE_VALUE_METHOD_UNSIGNED_INT, Dec_Proc);
		}
		FrameHdr->m_nStoredCRC &= 0x7FFFFFFF;
	}

	//m_spNewPredictorX->Flush();
	//m_spNewPredictorY->Flush();
	if(st->PredFlag == 0)
	{
		voAPE_PreInit(Xcnnf, nComplevel, nVersion);
		voAPE_PreInit(Ycnnf, nComplevel, nVersion);
		st->PredFlag = 1;
	}

	voAPEPre_Reset(Xcnnf);
	voAPEPre_Reset(Ycnnf);

	Xcnnf->Array_St.k = 10;
	Xcnnf->Array_St.nKSum = (1 << Xcnnf->Array_St.k) * 16;

	Ycnnf->Array_St.k = 10;
	Ycnnf->Array_St.nKSum = (1 << Ycnnf->Array_St.k) * 16;

	FrameHdr->m_nLastX = 0; 

	voAPEFrameStartFlush(Dec_Proc);

}

static const unsigned int crc32_table[256] = 
{ 
0,  
0x77073096,  0xee0e612c,  0x990951ba,  0x076dc419,  0x706af48f,  0xe963a535,  0x9e6495a3,  0x0edb8832,  
0x79dcb8a4,  0xe0d5e91e,  0x97d2d988,  0x09b64c2b,  0x7eb17cbd,  0xe7b82d07,  0x90bf1d91,  0x1db71064,  
0x6ab020f2,  0xf3b97148,  0x84be41de,  0x1adad47d,  0x6ddde4eb,  0xf4d4b551,  0x83d385c7,  0x136c9856,  
0x646ba8c0,  0xfd62f97a,  0x8a65c9ec,  0x14015c4f,  0x63066cd9,  0xfa0f3d63,  0x8d080df5,  0x3b6e20c8,  
0x4c69105e,  0xd56041e4,  0xa2677172,  0x3c03e4d1,  0x4b04d447,  0xd20d85fd,  0xa50ab56b,  0x35b5a8fa,  
0x42b2986c,  0xdbbbc9d6,  0xacbcf940,  0x32d86ce3,  0x45df5c75,  0xdcd60dcf,  0xabd13d59,  0x26d930ac,  
0x51de003a,  0xc8d75180,  0xbfd06116,  0x21b4f4b5,  0x56b3c423,  0xcfba9599,  0xb8bda50f,  0x2802b89e,  
0x5f058808,  0xc60cd9b2,  0xb10be924,  0x2f6f7c87,  0x58684c11,  0xc1611dab,  0xb6662d3d,  0x76dc4190,  
0x01db7106,  0x98d220bc,  0xefd5102a,  0x71b18589,  0x06b6b51f,  0x9fbfe4a5,  0xe8b8d433,  0x7807c9a2,  
0x0f00f934,  0x9609a88e,  0xe10e9818,  0x7f6a0dbb,  0x086d3d2d,  0x91646c97,  0xe6635c01,  0x6b6b51f4,  
0x1c6c6162,  0x856530d8,  0xf262004e,  0x6c0695ed,  0x1b01a57b,  0x8208f4c1,  0xf50fc457,  0x65b0d9c6,  
0x12b7e950,  0x8bbeb8ea,  0xfcb9887c,  0x62dd1ddf,  0x15da2d49,  0x8cd37cf3,  0xfbd44c65,  0x4db26158,  
0x3ab551ce,  0xa3bc0074,  0xd4bb30e2,  0x4adfa541,  0x3dd895d7,  0xa4d1c46d,  0xd3d6f4fb,  0x4369e96a,  
0x346ed9fc,  0xad678846,  0xda60b8d0,  0x44042d73,  0x33031de5,  0xaa0a4c5f,  0xdd0d7cc9,  0x5005713c,  
0x270241aa,  0xbe0b1010,  0xc90c2086,  0x5768b525,  0x206f85b3,  0xb966d409,  0xce61e49f,  0x5edef90e,  
0x29d9c998,  0xb0d09822,  0xc7d7a8b4,  0x59b33d17,  0x2eb40d81,  0xb7bd5c3b,  0xc0ba6cad,  0xedb88320,  
0x9abfb3b6,  0x03b6e20c,  0x74b1d29a,  0xead54739,  0x9dd277af,  0x04db2615,  0x73dc1683,  0xe3630b12,  
0x94643b84,  0x0d6d6a3e,  0x7a6a5aa8,  0xe40ecf0b,  0x9309ff9d,  0x0a00ae27,  0x7d079eb1,  0xf00f9344,  
0x8708a3d2,  0x1e01f268,  0x6906c2fe,  0xf762575d,  0x806567cb,  0x196c3671,  0x6e6b06e7,  0xfed41b76,  
0x89d32be0,  0x10da7a5a,  0x67dd4acc,  0xf9b9df6f,  0x8ebeeff9,  0x17b7be43,  0x60b08ed5,  0xd6d6a3e8,  
0xa1d1937e,  0x38d8c2c4,  0x4fdff252,  0xd1bb67f1,  0xa6bc5767,  0x3fb506dd,  0x48b2364b,  0xd80d2bda,  
0xaf0a1b4c,  0x36034af6,  0x41047a60,  0xdf60efc3,  0xa867df55,  0x316e8eef,  0x4669be79,  0xcb61b38c,  
0xbc66831a,  0x256fd2a0,  0x5268e236,  0xcc0c7795,  0xbb0b4703,  0x220216b9,  0x5505262f,  0xc5ba3bbe,  
0xb2bd0b28,  0x2bb45a92,  0x5cb36a04,  0xc2d7ffa7,  0xb5d0cf31,  0x2cd99e8b,  0x5bdeae1d,  0x9b64c2b0,  
0xec63f226,  0x756aa39c,  0x026d930a,  0x9c0906a9,  0xeb0e363f,  0x72076785,  0x05005713,  0x95bf4a82,  
0xe2b87a14,  0x7bb12bae,  0x0cb61b38,  0x92d28e9b,  0xe5d5be0d,  0x7cdcefb7,  0x0bdbdf21,  0x86d3d2d4,  
0xf1d4e242,  0x68ddb3f8,  0x1fda836e,  0x81be16cd,  0xf6b9265b,  0x6fb077e1,  0x18b74777,  0x88085ae6,  
0xff0f6a70,  0x66063bca,  0x11010b5c,  0x8f659eff,  0xf862ae69,  0x616bffd3,  0x166ccf45,  0xa00ae278,  
0xd70dd2ee,  0x4e048354,  0x3903b3c2,  0xa7672661,  0xd06016f7,  0x4969474d,  0x3e6e77db,  0xaed16a4a,  
0xd9d65adc,  0x40df0b66,  0x37d83bf0,  0xa9bcae53,  0xdebb9ec5,  0x47b2cf7f,  0x30b5ffe9,  0xbdbdf21c,  
0xcabac28a,  0x53b39330,  0x24b4a3a6,  0xbad03605,  0xcdd70693,  0x54de5729,  0x23d967bf,  0xb3667a2e,  
0xc4614ab8,  0x5d681b02,  0x2a6f2b94,  0xb40bbe37,  0xc30c8ea1,  0x5a05df1b,  0x2d02ef8d
};


#define  CRC_CHECK_BYTE   *pCRC = (*pCRC >> 8) ^ crc32_table[(*pCRC & 0xFF) ^ *pOutput++];

	// decompress and convert from(x,y) -> (l,r)
static void XY2nRnL(int X, int Y, unsigned char * pOutput, unsigned int *pCRC, voAPE_FILE_INFO *ape_info)
{
		int nR;
		int nL;
		int nFs = 0;
		int nChs = 0;
		int nBits = 0;
		int RV, LV;
		short R;
		unsigned int nTemp = 0;


		nChs = ape_info->nChannels;
		nBits = ape_info->nBitsPerSample;

		if(nChs == 2)
		{
			if(nBits == 16)
			{
				nR = X - (Y/2);
				nL = nR + Y;
				if((nR < -32768) || (nR > 32767) || (nL < -32768) || (nL > 32767))
				{
					*(short *) pOutput =  0;
					*(short *) pOutput =  0;
					return;
				}

				*(short *) pOutput = (short) nR;
				CRC_CHECK_BYTE
					CRC_CHECK_BYTE

					*(short *) pOutput = (short) nL;
				CRC_CHECK_BYTE
					CRC_CHECK_BYTE
			}
			else if(nBits == 24)
			{
				RV = X - (Y/2);
				LV = RV + Y;

				if(RV < 0)
					nTemp = ((unsigned int)(RV + 0x800000)) | 0x800000;
				else
					nTemp = (unsigned int) RV;

				*pOutput = (unsigned char)((nTemp >> 0) & 0xFF);
				CRC_CHECK_BYTE
					*pOutput = (unsigned char)((nTemp >> 8) & 0xFF);
				CRC_CHECK_BYTE
					*pOutput = (unsigned char)((nTemp >> 16)& 0xFF);
				CRC_CHECK_BYTE

					nTemp = 0;

				if(LV < 0)
					nTemp = ((unsigned int)(LV + 0x800000)) | 0x800000;
				else
					nTemp = (unsigned int) LV;

				*pOutput = (unsigned char)((nTemp >> 0) & 0xFF);
				CRC_CHECK_BYTE
					*pOutput = (unsigned char)((nTemp >> 8) & 0xFF);
				CRC_CHECK_BYTE
					*pOutput = (unsigned char)((nTemp >> 16)& 0xFF);
				CRC_CHECK_BYTE
			}
			else if(nBits == 8)
			{
				unsigned char R = (X - (Y >> 1) + 128);
				*pOutput  = R;
				CRC_CHECK_BYTE
					*pOutput  = (unsigned char)(R + Y);
				CRC_CHECK_BYTE
			}
		}
		else if(nChs == 1)
		{
			if(nBits == 16)
			{
				R = X;
				*(short *)pOutput = (short) R;
				CRC_CHECK_BYTE;
				CRC_CHECK_BYTE;
			}
			else if(nBits == 24)
			{
				RV = X;
				if(RV < 0)
					nTemp = ((unsigned int)(RV + 0x800000)) | 0x800000;
				else
					nTemp = (unsigned int)RV;
				*pOutput = (unsigned char)((nTemp >> 0) & 0xFF);
				CRC_CHECK_BYTE
					*pOutput = (unsigned char)((nTemp >> 8) & 0xFF);
				CRC_CHECK_BYTE
					*pOutput = (unsigned char)((nTemp >> 16)& 0xFF);
				CRC_CHECK_BYTE
			}
			else if(nBits == 8)
			{
				R = X + 128;
				*pOutput = R;
				CRC_CHECK_BYTE;
			}
		}
}

/***********************************************************************
*
* Description:  Decoder one output frame data form bit-stream Blocks 
*
*
************************************************************************/
static void voDecodeBlocksToFrameBuffer(voAPE_GData *st, int nBlocks)
{
		int nVersion;
		int nChannels;
		// decode the samples
		int X, Y;
		int nX, nY;
		int nBlocksProcessed;
		unsigned char *outputbuf;

		voAPE_FILE_INFO *APE_info;
		FrameStream     *Buf_Ptr;
		voAPE_Framehdr  *FrameHdr;
		voAPEBlocks     *Blk_Info;
		voCNNFState     *Xcnnf;
		voCNNFState     *Ycnnf;

		APE_info = st->ape_info;
		Buf_Ptr  = st->stream_buf;
		Xcnnf    = st->Xcnnf;
		Ycnnf    = st->Ycnnf;

		FrameHdr = &(st->DecProcess->m_Framehdr);
		Blk_Info = &(st->DecProcess->Blk_State);

		nVersion  = APE_info->nVersion;
		nChannels = APE_info->nChannels;
		outputbuf = Buf_Ptr->outbuf;

		//APE Codec only support stereo and Mono mode
		if(nChannels == 2)
		{
			if((FrameHdr->m_nSpecialCodes & SPECIAL_FRAME_LEFT_SILENCE) 
				&& (FrameHdr->m_nSpecialCodes & SPECIAL_FRAME_RIGHT_SILENCE))
			{
				for(nBlocksProcessed = 0; nBlocksProcessed < nBlocks; nBlocksProcessed++)
				{
					XY2nRnL(0, 0, outputbuf + (nBlocksProcessed << 2), &FrameHdr->m_nCRC, APE_info);
				}
			}
			else if(FrameHdr->m_nSpecialCodes & SPECIAL_FRAME_PSEUDO_STEREO)
			{
				for(nBlocksProcessed = 0; nBlocksProcessed < nBlocks; nBlocksProcessed++)
				{
					X = voDecompressValue3930to3950(st, voDecodeValueRange(st, &(Xcnnf->Array_St)));
					XY2nRnL(X, 0, outputbuf + (nBlocksProcessed << 2), &FrameHdr->m_nCRC, APE_info);
				}
			}
			else
			{
				if(nVersion >= 3950)
				{
					for(nBlocksProcessed = 0; nBlocksProcessed < nBlocks; nBlocksProcessed++)
					{
						nY = voDecodeValueRange(st, &(Ycnnf->Array_St));          //will get bits for input buffer
						nX = voDecodeValueRange(st, &(Xcnnf->Array_St));          //will get bits for input buffer

						Y  = voDecompressValue(nY, FrameHdr->m_nLastX, Ycnnf);
						X  = voDecompressValue(nX, Y, Xcnnf);
						//printf("nY = %d, nX = %d, Y = %d, X = %d\n", nY, nX, Y, X);
						FrameHdr->m_nLastX = X;

						XY2nRnL(X, Y, outputbuf + (nBlocksProcessed << 2), &FrameHdr->m_nCRC, APE_info);    // output the decoder data to output buffer
					}
				}
				else
				{
					for(nBlocksProcessed = 0; nBlocksProcessed < nBlocks; nBlocksProcessed++)
					{
						X = voDecompressValue3930to3950(st, voDecodeValueRange(st, &(Xcnnf->Array_St)));
						Y = voDecompressValue3930to3950(st, voDecodeValueRange(st, &(Ycnnf->Array_St)));
						XY2nRnL(X, Y, outputbuf + (nBlocksProcessed << 2), &FrameHdr->m_nCRC, APE_info);
					}
				}
			}
		}
		else
		{
			if(FrameHdr->m_nSpecialCodes & SPECIAL_FRAME_MONO_SILENCE)
			{
				for(nBlocksProcessed = 0; nBlocksProcessed < nBlocks; nBlocksProcessed++)
				{
					XY2nRnL(0, 0, outputbuf + (nBlocksProcessed << 1), &FrameHdr->m_nCRC, APE_info);
				}
			}
			else
			{
				for(nBlocksProcessed = 0; nBlocksProcessed < nBlocks; nBlocksProcessed++)
				{
					nX = voDecodeValueRange(st, &(Xcnnf->Array_St));
					X = voDecompressValue(nX, 0, Xcnnf);
					XY2nRnL(X, 0, outputbuf + (nBlocksProcessed << 1), &FrameHdr->m_nCRC, APE_info);
				}
			}
		}

		//bump frame decode position
		Blk_Info->m_nCurrentFrameBufferBlock += nBlocks;
}


static void voEndFrame(voAPE_GData *st)
{
		unsigned int    m_nCRC;
		int             nVersion;
		voDecodeBytes   *Dec_Pro;
		voAPEBlocks     *Blk_info;
		voAPE_Framehdr  *FrameHdr;

		nVersion = st->ape_info->nVersion;                    /* get the version information */ 
		Dec_Pro  = st->DecProcess;
		Blk_info = &(Dec_Pro->Blk_State);
		FrameHdr = &(Dec_Pro->m_Framehdr);
		Blk_info->m_nCurrentFrame++;                          //update the have been decoded frame number.

		voFinalize(Dec_Pro, nVersion);

		m_nCRC = FrameHdr->m_nCRC;
		//check the CRC
		m_nCRC = m_nCRC ^ 0xFFFFFFFF;
		m_nCRC >>= 1;
		if(m_nCRC != FrameHdr->m_nStoredCRC)
		{
			// error
			Blk_info->m_bErrorDecodingCurrentFrame = 1;   //TRUE
		}
}

int  voAPEDecodeStep(VOPTR hCodec)
{
		unsigned char nOffset = 0;
		int   nFrameBlocks;                                  
		/*get the current frame block number, APE frame is constant length, but the last 
		frame maybe is not complete frame*/
		int   nFrameOffsetBlocks;                            //calc the current block offset in current frame
		int   nFrameBlocksLeft;
		int   nBlocksThisPass;  
		voAPE_GData     *gData;
		voAPE_FILE_INFO *ape_info;
		voDecodeBytes   *Dec_Pro;
		voAPEBlocks     Blk_Info;
		FrameStream     *bs_buf;

		gData    = (voAPE_GData *)hCodec;                   /* get APE decoder global structure */
		bs_buf   = (FrameStream *)gData->stream_buf;          /* I/O bits stream structure */
		ape_info = (voAPE_FILE_INFO *)gData->ape_info;      /* get APE info structure form global st */
		Dec_Pro  = (voDecodeBytes *)gData->DecProcess;      /* get APE decoder process structure */ 
		Blk_Info = Dec_Pro->Blk_State;                      /* get APE decoder process Block info st */

		///* conjunction bs buffer operation pointer */
		//Dec_Pro->m_pBitArray = (int *)bs_buf->frame_ptr;           /* input buffer ptr */
		//Dec_Pro->m_nCurrentBitIndex = bs_buf->over_byte << 3;      /* has adjusted at outside */

		/* get frame blocks number */
		if(Blk_Info.m_nCurrentFrame < ape_info->nTotalFrames)
			nFrameBlocks = ape_info->nBlocksPerFrame;                  /* 294912 */
		else
			nFrameBlocks = ape_info->nFinalFrameBlocks;                /* Final Frame Blocks number */

		if(nFrameBlocks < 0)
			return -1;

		// analyze
		nFrameOffsetBlocks = Blk_Info.m_nCurrentFrameBufferBlock % nFrameBlocks;
		nFrameBlocksLeft = nFrameBlocks - nFrameOffsetBlocks;
		nBlocksThisPass = (nFrameBlocksLeft > BLOCKS_NUMBERS) ? BLOCKS_NUMBERS : nFrameBlocksLeft;
		//nBlocksThisPass = min(nFrameBlocksLeft, BLOCKS_NUMBERS);        //maybe mark the last frame some data, under read buffer mode

		//start the frame if we need to
		if(nFrameOffsetBlocks == 0)
		{
			nOffset = *(bs_buf->frame_ptr);
			bs_buf->frame_ptr += 4;
			bs_buf->set_len -= 4;
			bs_buf->over_byte = nOffset;
			/* conjunction bs buffer operation pointer */
			Dec_Pro->m_pBitArray = (int *)bs_buf->frame_ptr;           /* input buffer ptr */
			Dec_Pro->m_nCurrentBitIndex = bs_buf->over_byte << 3;      /* has adjusted at outside */

			Dec_Pro->m_nCurrentBitIndex = bs_buf->over_byte << 3;      /* has adjusted at outside */
			voStartFrame(gData);                                   //get the CRC number of the frame, and then flush some state buffer.
		}
		else
		{
			/* conjunction bs buffer operation pointer */
			Dec_Pro->m_pBitArray = (int *)bs_buf->frame_ptr;           /* input buffer ptr */
			Dec_Pro->m_nCurrentBitIndex = bs_buf->over_byte << 3;      /* has adjusted at outside */
		}
		//decode data
		voDecodeBlocksToFrameBuffer(gData, nBlocksThisPass);

		//end the frame if we need to
		if((nFrameOffsetBlocks + nBlocksThisPass) >= nFrameBlocks)
		{
			/*check CRC, and decide whether have error data in the frame */
			voEndFrame(gData);  
			bs_buf->over_byte = 0;
			bs_buf->set_len -= (Dec_Pro->m_nCurrentBitIndex >> 3);
			bs_buf->frame_ptr += (Dec_Pro->m_nCurrentBitIndex >> 3);
		}
		else
		{
			bs_buf->over_byte = (Dec_Pro->m_nCurrentBitIndex >> 3) - ((Dec_Pro->m_nCurrentBitIndex >> 5) << 2);
			bs_buf->set_len  -= (Dec_Pro->m_nCurrentBitIndex >> 3) - bs_buf->over_byte; 
			bs_buf->frame_ptr  += (Dec_Pro->m_nCurrentBitIndex >> 3) - bs_buf->over_byte;
		}

		//if((Dec_Pro->m_nCurrentBitIndex >> 3) > 4096)
		//printf("%d\n", Dec_Pro->m_nCurrentBitIndex >> 3);
		//bs_buf->used_len += (Dec_Pro->m_nCurrentBitIndex >> 3);   /* one output frame used Byte*/


		return VO_ERR_NONE;

}











