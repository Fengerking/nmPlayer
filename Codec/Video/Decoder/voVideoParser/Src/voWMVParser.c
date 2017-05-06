#include <stdlib.h>
#include "string.h"
#include "voWMVParser.h"
#include "voVideoParser.h"

#define SC_SEQ          0x0F
#define SC_ENTRY        0x0E
#define SC_FRAME        0x0D
#define SC_FIELD        0x0C
#define SC_SLICE        0x0B
#define SC_ENDOFSEQ     0x0A
#define SC_SEQ_DATA     0x1F
#define SC_ENTRY_DATA   0x1E
#define SC_FRAME_DATA   0x1D
#define SC_FIELD_DATA   0x1C
#define SC_SLICE_DATA   0x1B
typedef enum WMVFrameType {PROGRESSIVE = 0, INTERLACEFIELD, INTERLACEFRAME} WMVFrameType;

VO_VOID BS_flush16 (WMVBitStream* pBitstream, VO_U32 iNumBits)
{    
    // Remove used bits from bit mask filling free space with 0s
    pBitstream->m_uBitMask <<= iNumBits;
    
    // Adjust counter of available bits
    if ((pBitstream->m_iBitsLeft -= iNumBits) < 0)
    {
        VO_U8 *p = pBitstream->m_pCurr;
        
        if (p < pBitstream->m_pLast)
        {
            pBitstream->m_uBitMask += ((p[0] << 8) + p[1]) << (-pBitstream->m_iBitsLeft);
            p += 2;
            pBitstream->m_iBitsLeft += 16;
        }
        else
        {
			//data is not enough
        }
        pBitstream->m_pCurr = p;
    }
}

VO_U32 BS_getBits (WMVBitStream* pBitstream, VO_U32 dwNumBits)
{  
    VO_U32 uMask = 0;
    VO_U32 nBitsLeft = (VO_U32)(pBitstream->m_iBitsLeft + 16);
    if (dwNumBits == 0)
        return (0);

getBits_restart:    
    if (dwNumBits > nBitsLeft && nBitsLeft > 0)
    {
        dwNumBits -= nBitsLeft;
		uMask += (pBitstream->m_uBitMask >> (32 - nBitsLeft)) << dwNumBits;		
        BS_flush16 (pBitstream, nBitsLeft);
        nBitsLeft = (VO_U32)(pBitstream->m_iBitsLeft + 16);
        goto getBits_restart;
    }
    else
    {
        uMask += (pBitstream->m_uBitMask >> (32 - dwNumBits));
        BS_flush16 (pBitstream, dwNumBits);
    }

    return (uMask); 
}
VO_VOID BS_init(WMVBitStream	*pBitstream,VO_U8* pBuffer,VO_U32 BufferLen)
{
	pBitstream->m_uBitMask = 0;
	pBitstream->m_iBitsLeft = -16;
	pBitstream->m_pLast = pBuffer + BufferLen - 1;
	while (pBuffer <= pBitstream->m_pLast && pBitstream->m_iBitsLeft <= 8)
	{
		pBitstream->m_uBitMask += *pBuffer++ << (8 - pBitstream->m_iBitsLeft);
		pBitstream->m_iBitsLeft += 8;
	}	    
	pBitstream->m_pCurr = pBuffer;
}

VO_U32 voDecoderWVC1FrameHeader(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	VO_U8* pframeheader = inBuffer;
	VO_U32 frame_len = nBufLen;
	VO_U32 interlace_type =0;

	if(nBufLen < 1)
		return VO_ERR_VIDEOPARSER_FMHEAD;

	BS_init(pBitstream,pframeheader,frame_len);

	pVideoInfo->isInterlace =0;
	if (pWMV->nInterlacedSource)
	{
		if (0 == BS_getBits(pBitstream, 1)) 
			interlace_type = PROGRESSIVE;     // Progressive
		else 
		{
			pVideoInfo->isInterlace = 1;
			if (0 == BS_getBits(pBitstream, 1))
				interlace_type = INTERLACEFRAME; // Frame Interlace
			else    
				interlace_type = INTERLACEFIELD; // Field interlace
		}
	}
	else 
		interlace_type = PROGRESSIVE;    // Progressive

	if (BS_getBits(pBitstream, 1 ) == 0)
		pVideoInfo->nFrame_type = VO_VIDEO_FRAME_P; //P
	else if (BS_getBits( pBitstream, 1 ) == 0)
		pVideoInfo->nFrame_type = VO_VIDEO_FRAME_B; //B
	else if (BS_getBits( pBitstream, 1 ) == 0)
		pVideoInfo->nFrame_type = VO_VIDEO_FRAME_I;  //I
	else if (BS_getBits( pBitstream, 1 ) == 0)
		pVideoInfo->nFrame_type = VO_VIDEO_FRAME_B;   //BI
	else
		pVideoInfo->nFrame_type = VO_VIDEO_FRAME_P; //skip

	return VO_RETURN_FMHEADER;
}

VO_S32 ReSetVideoInfo(VO_VIDEO_PARSER* pVideoInfo)
{
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream *pTmpBS  = pWMV->pBS;
	pVideoInfo->nCodec = 0xFFFFFFFF;
	pVideoInfo->nVersion = 0xFFFFFFFF;
	pVideoInfo->nProfile = 0xFFFFFFFF;
	pVideoInfo->nLevel = 0xFFFFFFFFl;
	pVideoInfo->nFrame_type = 0xFFFFFFFF;
	pVideoInfo->nWidth = 0xFFFFFFFF;
	pVideoInfo->nHeight = 0xFFFFFFFF;
	pVideoInfo->isInterlace = 0xFFFFFFFF;
	pVideoInfo->isRefFrame = 0xFFFFFFFF;
	pVideoInfo->isVC1 = 0xFFFFFFFF;
	pVideoInfo->FirstFrameisKey = 0xFFFFFFFF;

	memset(pWMV,0xFF,sizeof(WMVPRIVATE));
	pWMV->pBS = pTmpBS;
	return 1;
}

VO_U32 isSequanceHeader(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo,WMVPRIVATE* pWMV)
{
	VO_U32 uStartCode =0;
	VO_U32 iBuffCount = 1;
	VO_BITMAPINFOHEADER* pHeader = NULL;
	VO_U32 iCodecVer =0;

	uStartCode = inBuffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | inBuffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | inBuffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | inBuffer[iBuffCount++];
	if(uStartCode == 0x0000010F)  //has start code this a SEQ_HEADER and is WVC1
	{
		ReSetVideoInfo(pVideoInfo);
		pWMV->iCodecVer = FOURCC_WVC1_WMV;
		pWMV->uStartCode = uStartCode;
		pVideoInfo->isVC1 = 1;
		return 1;
	}
	else
	{		
		pHeader = (VO_BITMAPINFOHEADER *)inBuffer;
        iCodecVer = pHeader->biCompression ;		
		if (iCodecVer == FOURCC_WVC1_WMV || iCodecVer == FOURCC_wvc1_WMV
			|| iCodecVer == FOURCC_WMVA_WMV || iCodecVer == FOURCC_wmva_WMV
			|| iCodecVer == FOURCC_WMV3_WMV || iCodecVer == FOURCC_wmv3_WMV
			|| iCodecVer == FOURCC_WMV2_WMV || iCodecVer == FOURCC_wmv2_WMV
			|| iCodecVer == FOURCC_WMV1_WMV || iCodecVer == FOURCC_wmv1_WMV
			||  iCodecVer == FOURCC_WVP2_WMV || iCodecVer == FOURCC_wvp2_WMV
			||  iCodecVer == FOURCC_WMVP_WMV || iCodecVer == FOURCC_wmvp_WMV)
		{
			ReSetVideoInfo(pVideoInfo);
			if (iCodecVer == FOURCC_WVC1_WMV || iCodecVer == FOURCC_wvc1_WMV
				|| iCodecVer == FOURCC_WMVA_WMV || iCodecVer == FOURCC_wmva_WMV
				|| iCodecVer == FOURCC_WMV3_WMV || iCodecVer == FOURCC_wmv3_WMV)
			{
				pVideoInfo->isVC1 =1;
			}
			else
			{
				pVideoInfo->isVC1 = 0;
			}

			pVideoInfo->nWidth = pHeader->biWidth;
			pVideoInfo->nHeight = pHeader->biHeight;
			pWMV->iCodecVer = iCodecVer;

			return 1;
		}
		else
			return 0;
	}	
}
VO_U32 voParserSeqWVC1(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	VO_U8* pBuffer= inBuffer+ASFBINDING_SIZE;

	if(nBufLen < 10)
		return VO_ERR_VIDEOPARSER_SQHEAD;

	BS_init(pBitstream,pBuffer,nBufLen-1);

	if(BS_getBits(pBitstream, 8))
		return VO_ERR_VIDEOPARSER_INPUTDAT;
	if(BS_getBits(pBitstream, 8))
		return VO_ERR_VIDEOPARSER_INPUTDAT;
	if(BS_getBits(pBitstream, 8)!=1)
		return VO_ERR_VIDEOPARSER_INPUTDAT;
	if(BS_getBits(pBitstream, 8)!=SC_SEQ)
		return VO_ERR_VIDEOPARSER_INPUTDAT;

	pVideoInfo->nProfile = BS_getBits(pBitstream, 2);
	pVideoInfo->nLevel = BS_getBits(pBitstream, 3);
	pWMV->nChromaFormat = BS_getBits(pBitstream, 2);
	pWMV->nFrameRate = BS_getBits(pBitstream, 3);
	pWMV->nBitRate = BS_getBits(pBitstream, 5);
	pWMV->nPostProcInfoPresent = BS_getBits(pBitstream, 1);

	pVideoInfo->nWidth  = 2 * BS_getBits(pBitstream, 12) + 2;
	pVideoInfo->nHeight = 2 * BS_getBits(pBitstream, 12) + 2;

	pWMV->nBroadcastFlags = BS_getBits(pBitstream, 1);
	pWMV->nInterlacedSource = BS_getBits(pBitstream, 1);
	pWMV->nTemporalFrmCntr = BS_getBits(pBitstream, 1);
	pWMV->nSeqFrameInterpolation = BS_getBits(pBitstream, 1);

	return VO_ERR_NONE;
}

VO_U32 voParserSeqWMV3(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	VO_U8* pBuffer= inBuffer;
	
	if(nBufLen < 4 )
		return VO_ERR_VIDEOPARSER_SQHEAD;

	BS_init(pBitstream,pBuffer,nBufLen);

	pVideoInfo->nProfile = BS_getBits(pBitstream, 2);

	pWMV->nYUV411 = BS_getBits(pBitstream, 1);
	pWMV->nSpriteMode = BS_getBits(pBitstream,1);
	pWMV->nFrameRate = 4*BS_getBits(pBitstream,3)+2;
	pWMV->nBitRate = 64*BS_getBits(pBitstream,5)+32;
	pWMV->nLoopFilter = BS_getBits(pBitstream,1);

	pWMV->nXintra8Switch = BS_getBits(pBitstream,1);    
    pWMV->nMultiresEnabled = BS_getBits(pBitstream,1);
    pWMV->n16bitXform = BS_getBits(pBitstream,1);     
    pWMV->nUVHpelBilinear = BS_getBits(pBitstream,1); 
    pWMV->nExtendedMvMode = BS_getBits(pBitstream,1); 
    pWMV->nDQuantCodingOn = BS_getBits(pBitstream,2);

	pWMV->nformSwitch = BS_getBits(pBitstream,1);
    pWMV->nDCTTable_MB_ENABLED = BS_getBits(pBitstream,1);    
    pWMV->nSequenceOverlap = BS_getBits(pBitstream, 1);
    pWMV->nStartCode = BS_getBits(pBitstream, 1);    
    pWMV->nPreProcRange = BS_getBits(pBitstream, 1);    
    pWMV->nNumBFrames = BS_getBits(pBitstream, 3);

	pWMV->nExplicitSeqQuantizer = BS_getBits(pBitstream, 1);

    if (pWMV->nExplicitSeqQuantizer)
        pWMV->nUse3QPDZQuantizer = BS_getBits(pBitstream, 1);
	else 
        pWMV->nExplicitFrameQuantizer = BS_getBits(pBitstream, 1);
    pWMV->nExplicitQuantizer = pWMV->nExplicitSeqQuantizer || pWMV->nExplicitFrameQuantizer;    
    pWMV->nSeqFrameInterpolation = BS_getBits(pBitstream, 1);
	return VO_ERR_NONE;
}

VO_U32 voParserSeqWMV2(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	VO_U8* pBuffer= inBuffer;

	if(nBufLen < 4 )
		return VO_ERR_VIDEOPARSER_SQHEAD;

	BS_init(pBitstream,pBuffer,nBufLen);
	pWMV->nFrameRate = BS_getBits  (pBitstream, 5);
    pWMV->nBitRate = BS_getBits  (pBitstream, 11);
    pWMV->nRndCtrlOn = TRUE_WMV;
    pWMV->nMixedPel = BS_getBits  (pBitstream, 1);
    pWMV->nLoopFilter = BS_getBits  (pBitstream, 1);
    pWMV->nXformSwitch = BS_getBits  (pBitstream, 1);
    pWMV->nXintra8Switch = BS_getBits  (pBitstream, 1);    
    pWMV->nFrmHybridMVOn = BS_getBits  (pBitstream, 1);

    // DCTTABLE S/W at MB level for WMV2.
    pWMV->nDCTTable_MB_ENABLED = BS_getBits  (pBitstream, 1);
    pWMV->nSliceCode = BS_getBits(pBitstream, 3);

	pVideoInfo->nProfile  =0;
	pVideoInfo->isVC1 = 0;

	return VO_ERR_NONE;
}

VO_U32 voParserSeqWMV1(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	VO_BITMAPINFOHEADER *bitinfoheader = (VO_BITMAPINFOHEADER *)inBuffer;
	pVideoInfo->nVersion = 7;
	pVideoInfo->nWidth = bitinfoheader->biWidth;
	pVideoInfo->nHeight = bitinfoheader->biHeight;
	pVideoInfo->nProfile  =0;
	pVideoInfo->isVC1 = 0;

	return VO_ERR_NONE;
}

VO_U32 voParserSeqHeader(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	VO_U32 ret=0;
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	if(pWMV->uStartCode == 0x0000010F) //sequence header start with 0x0000010F
	{ // WVC1
		ret = voParserSeqWVC1(inBuffer,nBufLen,pVideoInfo);		
	}
	else
	{

		if(nBufLen < sizeof(VO_BITMAPINFOHEADER))
			return VO_ERR_VIDEOPARSER_SQHEAD;

		if (pWMV->iCodecVer == FOURCC_WVC1_WMV || pWMV->iCodecVer == FOURCC_wvc1_WMV
				|| pWMV->iCodecVer == FOURCC_WMVA_WMV || pWMV->iCodecVer == FOURCC_wmva_WMV
				|| pWMV->iCodecVer == FOURCC_WVP2_WMV || pWMV->iCodecVer == FOURCC_wvp2_WMV)
		{
			VO_U8* buffer = inBuffer + sizeof(VO_BITMAPINFOHEADER);
			VO_U32 buffer_len = nBufLen -sizeof(VO_BITMAPINFOHEADER);
			ret = voParserSeqWVC1(buffer,buffer_len,pVideoInfo);
		}
		else if(pWMV->iCodecVer == FOURCC_WMV3_WMV || pWMV->iCodecVer == FOURCC_wmv3_WMV)
		{
			VO_U8* buffer = inBuffer + sizeof(VO_BITMAPINFOHEADER);
			VO_U32 buffer_len = nBufLen -sizeof(VO_BITMAPINFOHEADER);
			ret = voParserSeqWMV3(buffer,buffer_len,pVideoInfo);
		}
		else if(pWMV->iCodecVer == FOURCC_WMV2_WMV || pWMV->iCodecVer == FOURCC_wmv2_WMV)
		{
			VO_U8* buffer = inBuffer + sizeof(VO_BITMAPINFOHEADER);
			VO_U32 buffer_len = nBufLen -sizeof(VO_BITMAPINFOHEADER);
			ret = voParserSeqWMV2(buffer,buffer_len,pVideoInfo);
		}
		else if(pWMV->iCodecVer == FOURCC_WMV1_WMV || pWMV->iCodecVer == FOURCC_wmv1_WMV)
		{
			ret = voParserSeqWMV1(inBuffer,nBufLen,pVideoInfo);
		}
		else
		{
			return VO_ERR_VIDEOPARSER_SQHEAD;
		}
	}
	return  VO_RETURN_SQHEADER;
}

VO_U32 voParserFrmWVC1(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	VO_U8* pBuffer= inBuffer;
	VO_U32 i =0;
	VO_U8* pframeheader =NULL;
	VO_U32 frame_len =0;
	VO_U8* psequenceheader =NULL;
	VO_U32 seq_len =0;
	VO_S32 isfrm  = 0;
	VO_S32 isseq = 0;
	//VO_U32 interlace_type = 0;
	VO_U32 ret = 0;

	for(i=0;i<nBufLen;i++)
	{
		if(pBuffer[0+i]==0 && pBuffer[1+i]== 0 && pBuffer[2+i] == 1 && pBuffer[3+i] == SC_FRAME)
		{
			pframeheader =  pBuffer+4;
			frame_len = nBufLen-4-i;
			isfrm = 1;
			ret = voDecoderWVC1FrameHeader(pframeheader,frame_len,pVideoInfo);						
		}
		else if(pBuffer[0+i]==0 && pBuffer[1+i]== 0 && pBuffer[2+i] == 1 && pBuffer[3+i] == SC_SEQ)
		{
			psequenceheader = pBuffer+4;
			seq_len = nBufLen-4-i;
			isseq = 1;

			BS_init(pBitstream,psequenceheader,seq_len);

			pVideoInfo->nProfile = BS_getBits(pBitstream, 2);
			pVideoInfo->nLevel = BS_getBits(pBitstream, 3);
			pWMV->nChromaFormat = BS_getBits(pBitstream, 2);
			pWMV->nFrameRate = BS_getBits(pBitstream, 3);
			pWMV->nBitRate = BS_getBits(pBitstream, 5);
			pWMV->nPostProcInfoPresent = BS_getBits(pBitstream, 1);
			pVideoInfo->nWidth  = 2 * BS_getBits(pBitstream, 12) + 2;
			pVideoInfo->nHeight = 2 * BS_getBits(pBitstream, 12) + 2;
			pWMV->nBroadcastFlags = BS_getBits(pBitstream, 1);
			pWMV->nInterlacedSource = BS_getBits(pBitstream, 1);
			pWMV->nTemporalFrmCntr = BS_getBits(pBitstream, 1);
			pWMV->nSeqFrameInterpolation = BS_getBits(pBitstream, 1);

		}
	}

	if(isseq == 0 && isfrm == 0)  // no seq start code; frm start code  // we treat it as a frame
	{    
		pframeheader =  pBuffer;
		frame_len = nBufLen;
		ret =  voDecoderWVC1FrameHeader(pframeheader,frame_len,pVideoInfo);
		return VO_RETURN_FMHEADER;
	}
	else
	{
		if(isseq == 1 && isfrm == 1)
			return VO_RETURN_SQFMHEADER;
		else if(isseq == 1 )
			return VO_RETURN_SQHEADER;
		else
			return VO_RETURN_FMHEADER;
	}
}

VO_U32 voParserFrmWMVP(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	VO_U32 nFrmCntMod4 =0;
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	VO_U8* pBuffer= inBuffer;

	if(nBufLen < 2)
		return VO_ERR_VIDEOPARSER_FMHEAD;

	BS_init(pBitstream,pBuffer,nBufLen);

	pWMV->nFrmPredType = BS_getBits(pBitstream, 1);
	pWMV->nNumMotionVectorSets = BS_getBits(pBitstream, 1) + 1;
	
	pWMV->nIsBChangedToI = 0;

	if(pWMV->nSeqFrameInterpolation)
		pWMV->nInterpolateCurrentFrame = BS_getBits(pBitstream, 1);
	nFrmCntMod4 = BS_getBits  (pBitstream,2);  
	if(pWMV->nPreProcRange)
	{
		pWMV->nRangeState = BS_getBits  (pBitstream,1);  
	}

	pWMV->nRefFrameNum = -1;
    if (BS_getBits ( pBitstream,1) == 1)
        pVideoInfo->nFrame_type = VO_VIDEO_FRAME_P;
    else
    {
		if (pWMV->nNumBFrames == 0) 
        {
			 pVideoInfo->nFrame_type = VO_VIDEO_FRAME_I;
        }
        else
        { 
			if (BS_getBits(pBitstream, 1) == 1)
                 pVideoInfo->nFrame_type = VO_VIDEO_FRAME_I;
			else
				pVideoInfo->nFrame_type = VO_VIDEO_FRAME_B;
        }

		if (pVideoInfo->nFrame_type == VO_VIDEO_FRAME_B) 
        {
            VO_S32 iShort = 0, iLong = 0;
            iShort = BS_getBits(pBitstream,3);
            if (iShort == 0x7)
            {
                iLong = BS_getBits(pBitstream,4);
                if (iLong == 0xe) // "hole" in VLC
                    return VO_ERR_VIDEOPARSER_INPUTDAT;                
                if (iLong == 0xf)
                    pWMV->nIsBChangedToI = 1;
            }
        }
    }	
	if (pWMV->nIsBChangedToI)
        pVideoInfo->nFrame_type = VO_VIDEO_FRAME_B;
	return VO_RETURN_FMHEADER;

}

VO_U32 voParserFrmWMV3(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	VO_U32 nFrmCntMod4 =0;
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	VO_U8* pBuffer= inBuffer;

	if(nBufLen < 2)
		return VO_ERR_VIDEOPARSER_FMHEAD;

	BS_init(pBitstream,pBuffer,nBufLen);
	
	pWMV->nIsBChangedToI = 0;

	if(pWMV->nSeqFrameInterpolation)
		pWMV->nInterpolateCurrentFrame = BS_getBits(pBitstream, 1);
	nFrmCntMod4 = BS_getBits  (pBitstream,2);  
	if(pWMV->nPreProcRange)
	{
		pWMV->nRangeState = BS_getBits  (pBitstream,1);  
	}

	pWMV->nRefFrameNum = -1;
    if (BS_getBits ( pBitstream,1) == 1)
        pVideoInfo->nFrame_type = VO_VIDEO_FRAME_P;
    else
    {
		if (pWMV->nNumBFrames == 0) 
        {
			 pVideoInfo->nFrame_type = VO_VIDEO_FRAME_I;
        }
        else
        { 
			if (BS_getBits(pBitstream, 1) == 1)
                 pVideoInfo->nFrame_type = VO_VIDEO_FRAME_I;
			else
				pVideoInfo->nFrame_type = VO_VIDEO_FRAME_B;
        }

		if (pVideoInfo->nFrame_type == VO_VIDEO_FRAME_B) 
        {
            VO_S32 iShort = 0, iLong = 0;
            iShort = BS_getBits(pBitstream,3);
            if (iShort == 0x7)
            {
                iLong = BS_getBits(pBitstream,4);
                if (iLong == 0xe) // "hole" in VLC
                    return VO_ERR_VIDEOPARSER_INPUTDAT;                
                if (iLong == 0xf)
                    pWMV->nIsBChangedToI = 1;
            }
        }
    }	
	if (pWMV->nIsBChangedToI)
        pVideoInfo->nFrame_type = VO_VIDEO_FRAME_B;

	pVideoInfo->isInterlace = 0;

	return VO_RETURN_FMHEADER;
}
VO_U32 voParserFrmWMV2(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	//VO_U32 nFrmCntMod4 =0;
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	VO_U8* pBuffer= inBuffer;
	BS_init(pBitstream,pBuffer,nBufLen);

	pVideoInfo->nFrame_type = (VO_VIDEO_FRAMETYPE) BS_getBits(pBitstream, 1);
	pVideoInfo->nProfile =0;
	pVideoInfo->nLevel =0;
	pVideoInfo->isInterlace = 0;

	return VO_RETURN_FMHEADER;
}

VO_U32 voParserFrmWMV1(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	//VO_U32 nFrmCntMod4 =0;
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	VO_U8* pBuffer= inBuffer;
	BS_init(pBitstream,pBuffer,nBufLen);
	pVideoInfo->nFrame_type = (VO_VIDEO_FRAMETYPE) BS_getBits(pBitstream, 2);
	pVideoInfo->nProfile =0;
	pVideoInfo->nLevel =0;
	pVideoInfo->isInterlace = 0;
	return VO_RETURN_FMHEADER;
}

VO_U32 voParserFrmHeader(VO_U8*inBuffer,VO_U32 nBufLen,VO_VIDEO_PARSER *pVideoInfo)
{
	VO_U32 ret = 0;
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	WMVBitStream	*pBitstream = pWMV->pBS;
	//VO_U8* pBuffer= inBuffer;
	 if(pWMV->iCodecVer == FOURCC_WMVA_WMV || pWMV->iCodecVer == FOURCC_wmva_WMV
		 ||pWMV->iCodecVer == FOURCC_WVC1_WMV || pWMV->iCodecVer == FOURCC_wvc1_WMV )
	{
		ret = voParserFrmWVC1(inBuffer,nBufLen,pVideoInfo);
	}
	else if(pWMV->iCodecVer == FOURCC_WMV3_WMV || pWMV->iCodecVer == FOURCC_wmv3_WMV)
	{
		if(pWMV->nSpriteMode)
		{
			ret = voParserFrmWMVP(inBuffer,nBufLen,pVideoInfo);
		}
		else
			ret = voParserFrmWMV3(inBuffer,nBufLen,pVideoInfo);
	}
	else if(pWMV->iCodecVer == FOURCC_WMV2_WMV || pWMV->iCodecVer == FOURCC_wmv2_WMV)
	{
		ret = voParserFrmWMV2(inBuffer,nBufLen,pVideoInfo);
	}
	else if(pWMV->iCodecVer == FOURCC_WMV1_WMV || pWMV->iCodecVer == FOURCC_wmv1_WMV)
	{
		ret = voParserFrmWMV1(inBuffer,nBufLen,pVideoInfo);
	}
	else if(pWMV->iCodecVer == FOURCC_WVP2_WMV || pWMV->iCodecVer == FOURCC_wvp2_WMV)
	{
		BS_init(pBitstream,inBuffer,nBufLen);
		pWMV->nFrmPredType = BS_getBits(pBitstream, 1);
		pWMV->nNumMotionVectorSets = BS_getBits(pBitstream, 1) + 1;
		if(pWMV->nFrmPredType == 0) //IVOP
		{
			if (BS_getBits( pBitstream, 1 ) == 0)
				pVideoInfo->nFrame_type = VO_VIDEO_FRAME_P;
			else if (BS_getBits( pBitstream, 1 ) == 0)
				pVideoInfo->nFrame_type = VO_VIDEO_FRAME_B;
			else if (BS_getBits( pBitstream, 1 ) == 0)
				pVideoInfo->nFrame_type = VO_VIDEO_FRAME_I;
			else if (BS_getBits( pBitstream, 1 ) == 0)
			   pVideoInfo->nFrame_type = VO_VIDEO_FRAME_B;
			else
				pVideoInfo->nFrame_type = VO_VIDEO_FRAME_P; //skip
		}
		else
		{
			pVideoInfo->nFrame_type = VO_VIDEO_FRAME_P;
		}

	}
	else
	{
		return VO_ERR_VIDEOPARSER_NOTSUPPORT;
	}

	return VO_RETURN_FMHEADER;
}

VO_S32 voWMVVC1Init(VO_HANDLE *pParHandle)
{
	WMVPRIVATE *pWMV= NULL;
	VO_VIDEO_PARSER *pParser = (VO_VIDEO_PARSER*)malloc(sizeof(VO_VIDEO_PARSER));
	if(!pParser)
		return VO_ERR_VIDEOPARSER_MEMORY; 
	memset(pParser,0xFF,sizeof(VO_VIDEO_PARSER));	

	pWMV = (WMVPRIVATE*)malloc(sizeof(WMVPRIVATE));
	if(!pWMV)
		return VO_ERR_VIDEOPARSER_MEMORY; 
	memset(pWMV,0xFF,sizeof(WMVPRIVATE));

	pWMV->pBS = (WMVBitStream*)malloc(sizeof(WMVBitStream));
	if(!pWMV->pBS)
		return VO_ERR_VIDEOPARSER_MEMORY; 
	memset(pWMV->pBS,0xFF,sizeof(WMVBitStream));

	pParser->pCodecUser = pWMV;

	*pParHandle = pParser;
	return VO_ERR_NONE;
}

VO_S32 voWMVVC1Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData)
{
	VO_U32 ret = VO_ERR_NONE;
	VO_VIDEO_PARSER* pVideoInfo = (VO_VIDEO_PARSER*)pParHandle;
	VO_U8* inBuffer= pInData->Buffer;
	VO_U32 BufferLen = pInData->Length; 
	//VO_U32 iBuffCount = 1;  //jump the first byte
	//VO_U32 uStartCode =0,iW=0,iH=0;
	//VO_BITMAPINFOHEADER* pHeader = NULL;
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pVideoInfo->pCodecUser;
	//WMVBitStream	*pBitstream = pWMV->pBS;

	if(isSequanceHeader(inBuffer,BufferLen,pVideoInfo,pWMV))
	{//seq header
		ret = voParserSeqHeader(inBuffer,BufferLen,pVideoInfo);
	}
	else
	{//frame header
		ret = voParserFrmHeader(inBuffer,BufferLen,pVideoInfo);
	}

	return ret;
}

VO_S32 voWMVVC1GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue)
{
	VO_VIDEO_PARSER* pParser = (VO_VIDEO_PARSER*)pParHandle;
	WMVPRIVATE* pWMV = (WMVPRIVATE*)pParser->pCodecUser;
	switch(nID)
	{
	case VO_PID_VIDEOPARSER_VERSION:
		*((VO_U32*)pValue) = pWMV->iCodecVer;
		break;
	case VO_PID_VIDEOPARSER_PROFILE:
		*((VO_U32*)pValue) = pParser->nProfile;
		break;
	case VO_PID_VIDEOPARSER_LEVEL:
		*((VO_U32*)pValue) = pParser->nLevel;
		break;
	case VO_PID_VIDEOPARSER_FRAMETYPE:
		*((VO_U32*)pValue) = pParser->nFrame_type;
		break;
	case VO_PID_VIDEOPARSER_WIDTH:
		*((VO_U32*)pValue) = pParser->nWidth;
		break;
	case VO_PID_VIDEOPARSER_HEIGHT:
		*((VO_U32*)pValue) = pParser->nHeight;
		break;
	case VO_PID_VIDEOPARSER_REFFRAME:
		*((VO_U32*)pValue) = (pParser->nFrame_type != VO_VIDEO_FRAME_B);
		break;
	case VO_PID_VIDEOPARSER_INTERLANCE:
		*((VO_U32*)pValue) = pParser->isInterlace;
		break;
	case VO_PID_VIDEOPARSER_ISVC1:
		*((VO_U32*)pValue) = pParser->isVC1;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	if(*((VO_U32*)pValue)  == 0xFFFFFFFF)
		return VO_ERR_VIDEOPARSER_NOVALUE;

	return VO_ERR_NONE;
}

VO_S32 voWMVVC1Uninit(VO_HANDLE pParHandle)
{
	VO_VIDEO_PARSER* pParser = (VO_VIDEO_PARSER*)pParHandle;
	if(pParser)
	{
		WMVPRIVATE *pWMV = (WMVPRIVATE*)pParser->pCodecUser;
		if(pWMV->pBS)
		{
			free(pWMV->pBS);
			pWMV->pBS = NULL;
		}
		if(pWMV)
		{
			free(pWMV);
			pWMV = NULL;
		}
		free(pParser);
		pParser = NULL;
	}
	return VO_ERR_NONE;
}