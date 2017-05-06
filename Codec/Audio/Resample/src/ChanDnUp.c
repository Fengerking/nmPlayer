/**********************************************************************
*
* VisualOn voplayer
*
* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
*
***********************************************************************/

/*********************************************************************
*
* Filename:	
*		ChanDnUp.c
* 
* Abstact:	
*
*		Channel up and down mix functions and definations.
*
* Author:
*
*		Witten Wen 2010-7-27
*
* Revision History:
*
******************************************************/
#include "ChanDnUp.h"
#include "resample.h"

const int rgrgfltChDnMix6_2[2][6] = 
{
	{0x002978d5,	0x00000000,	0x001d4fdf,	0x002374bc,	0x0015c28f,	0x00000000},
	{0x00000000,	0x002978d5,	0x001d4fdf,	0x0015c28f,	0x002374bc,	0x00000000},	
};

const int rgrgfltChDnMix8_2[2][8] =
{
	{0x001d0e56,	0x00000000,	0x00149ba5,	0x0018d4fe,	0x000f3b64,	0x001a9fbe,	0x000bc6a7,	0x00000000},
	{0x00000000,	0x001d0e56,	0x00149ba5,	0x000f3b64,	0x0018d4fe,	0x000bc6a7,	0x001a9fbe,	0x00000000},
};

const int rgrgfltChDnMix8_6[6][8] = 
{
	{0x00424dd3,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00183126,	0x00000000,	0x00000000},
	{0x00000000,	0x00424dd3,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00183126,	0x00000000},
	{0x00000000,	0x00000000,	0x00424dd3,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000},
	{0x00000000,	0x00000000,	0x00000000,	0x00424dd3,	0x00000000,	0x003db22d,	0x00000000,	0x00000000},
	{0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00424dd3,	0x00000000,	0x003db22d,	0x00000000},
	{0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000},
};

static const int g_ChannelMasks[] =  { 0, MCMASK_MONO, MCMASK_STEREO,
MCMASK_THREE, MCMASK_QUAD, MCMASK_5, MCMASK_5DOT1, MCMASK_6DOT1, MCMASK_7DOT1 };

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))
#endif

VO_S32 DefaultChannelMask(VO_U32 nChannels)
{
  if (nChannels >= SIZEOF_ARRAY(g_ChannelMasks)) return 0;
  return g_ChannelMasks[nChannels];
}

VO_VOID	DumbBubble(VO_S32 *rgfltPhi, VO_S32 *rgiCMap, VO_S32 cChan)
{
	VO_S32	i, iTmp;
    VO_BOOL	fChanged = VO_FALSE;
    VO_S32	fltTmp;

	do {
        fChanged = VO_FALSE;
        for(i = 0; i < cChan-1; i++)
        {
            if (rgfltPhi[i] > rgfltPhi[i+1])
            {
                fltTmp			= rgfltPhi[i];
                iTmp			= rgiCMap[i];
                rgfltPhi[i]		= rgfltPhi[i+1];
                rgiCMap[i]		= rgiCMap[i+1];
                rgfltPhi[i+1]	= fltTmp;
                rgiCMap[i+1]	= iTmp;
                fChanged		= VO_TRUE;
            }
        }
    } while (fChanged);
}

VO_S32 InitNonLFEChDnMixThroughProjections(VO_S32	cSrcChan, 
										   VO_U32	nSrcChanMask, 
										   VO_S32	cDstChan, 
										   VO_U32	nDstChanMask, 
										   VO_S32	rgrgfltChDnMix[MAX_CHANNEL][MAX_CHANNEL])
{
	int i, j;

	for(i=0;i<MAX_CHANNEL;i++)
	{
		for (j=0;j<MAX_CHANNEL;j++) 
		{
			rgrgfltChDnMix[i][j] = (VO_S32)0;
		}
	}

	if(cSrcChan == 5 && nSrcChanMask == 0x37 && cDstChan == 2 && nDstChanMask == 3)
		for(i=0; i<2; i++)
			for(j=0; j<6; j++)
				rgrgfltChDnMix[i][j] = rgrgfltChDnMix6_2[i][j];
	else if(cSrcChan == 7 && nSrcChanMask == 0x637 && cDstChan == 2 && nDstChanMask == 3)
		for(i=0; i<2; i++)
			for(j=0; j<8; j++)
				rgrgfltChDnMix[i][j] = rgrgfltChDnMix8_2[i][j];
	else if(cSrcChan == 7 && nSrcChanMask == 0x637 && cDstChan == 5 && nDstChanMask == 0x37)
		for(i=0; i<6; i++)
			for(j=0; j<8; j++)
				rgrgfltChDnMix[i][j] = rgrgfltChDnMix8_6[i][j];
	else
	{
		const VO_S32 rgfltSpeakerDirection[] =
		{
			-30 + 360, // VO_CHANNEL_FRONT_LEFT
			30, // VO_CHANNEL_FRONT_RIGHT
			0,   // VO_CHANNEL_FRONT_CENTER 
			0,   // VO_CHANNEL_LOW_FREQUENCY 
			-135 + 360, // VO_CHANNEL_BACK_LEFT              
			+135, // VO_CHANNEL_BACK_RIGHT
			-15 + 360,  // VO_CHANNEL_FRONT_LEFT_OF_CENTER
			+15,  // VO_CHANNEL_FRONT_RIGHT_OF_CENTER
			+180, // VO_CHANNEL_BACK_CENTER
			-110 + 360, // VO_CHANNEL_SIDE_LEFT
			+110  // VO_CHANNEL_SIDE_RIGHT
			// Any top-speakers should be dealt with by outer systems.
			/*
			#define VO_CHANNEL_TOP_CENTER              0x800
			#define VO_CHANNEL_TOP_FRONT_LEFT          0x1000
			#define VO_CHANNEL_TOP_FRONT_CENTER        0x2000
			#define VO_CHANNEL_TOP_FRONT_RIGHT         0x4000
			#define VO_CHANNEL_TOP_BACK_LEFT           0x8000
			#define VO_CHANNEL_TOP_BACK_CENTER         0x10000
			#define VO_CHANNEL_TOP_BACK_RIGHT          0x20000
			*/
		};

		VO_S32	rgiInCMap[MAX_CHANNEL], rgiOutCMap[MAX_CHANNEL];
		VO_S32	iTmp0 = 0;
		VO_U32	nMask = 1;

		VO_S32 rgfltInPhi[MAX_CHANNEL];	
		VO_S32 fltSource;
		VO_S32 fltG1, fltG2;
		VO_S32   iTarg1, iTarg2;
		VO_S32 fltTmp, fltBTmp;	
		VO_S32 rgfltOutPhi[MAX_CHANNEL], rgfltOutDel[MAX_CHANNEL];	

		for(i = 0; i < cSrcChan; i++)
		{
			rgiInCMap[i] = i;

			// Find the position of ith source channel
			while (!(nSrcChanMask & nMask))
			{
				nMask <<= 1;
				iTmp0++;
			}
			if (iTmp0 <= 10)
				rgfltInPhi[i] = rgfltSpeakerDirection[iTmp0];
			else
			{
				// internal erorr: we can not deal with top speakers
				return VO_ERR_INVALID_ARG;
			}

			nMask <<= 1;
			iTmp0++;
		}

		DumbBubble(&rgfltInPhi[0], &rgiInCMap[0], cSrcChan);

		nMask = 1;
		iTmp0 = 0;

		for(i = 0; i < cDstChan; i++)
		{
			rgiOutCMap[i] = i;

			// Find the position of ith destination channel
			while (!(nDstChanMask & nMask))
			{
				nMask <<= 1;
				iTmp0++;				
			}
			if (iTmp0 <= 10)
			{
				rgfltOutPhi[i] = rgfltSpeakerDirection[iTmp0];
			}
			else
			{
				// internal erorr: we can not deal with top speakers
				return VO_ERR_INVALID_ARG;
			}

			nMask <<= 1;
			iTmp0++;
		}

		DumbBubble(&rgfltOutPhi[0], &rgiOutCMap[0], cDstChan);

		for(i = 0; i <  cDstChan-1; i++)
		{
			rgfltOutDel[i] = rgfltOutPhi[i+1] - rgfltOutPhi[i];
		}
		rgfltOutDel[cDstChan-1] = rgfltOutPhi[0] - rgfltOutPhi[cDstChan-1] + 360;

		for (i = 0; i < cSrcChan; i++)
		{
			fltSource = rgfltInPhi[i];

			j = 0;
			while ((fltSource > rgfltOutPhi[j]) && (j < cDstChan))
			{
				j++;
			}
			if (j == 0 ||  j > cDstChan-1)
			{
				iTarg1 = cDstChan - 1;
				iTarg2 = 0;
			} 
			else 
			{
				iTarg1 = j-1;
				iTarg2 = j;
			}

			fltTmp = fltSource - rgfltOutPhi[iTarg1];
			while ( fltTmp < 0 ) 
			{
				fltTmp = fltTmp + 360;
			}
			fltTmp = (fltTmp<<8)/rgfltOutDel[iTarg1];

			//        fltG1 = (Float) cos(fltTmp * fltPi/2.0F);
			fltG1 = lookup_sine(256 - fltTmp);//fltTmp * fltPi/2.0F);
			if (fltG1 < 0 )
			{
				fltG1 = 0;
			}
			//        fltG2 = (Float) sin(fltTmp * fltPi/2.0F);
			fltG2 = lookup_sine(fltTmp);//(fltTmp * fltPi/2.0F);
			if ( fltG2 < 0) 
			{
				fltG2 = 0;
			}
			if (cDstChan == 1 )
			{
				//			fltG1 = fltG2 = 1.0F;
				fltG1 = fltG2 = 0x8000;
			}

			rgrgfltChDnMix[rgiOutCMap[iTarg1]][rgiInCMap[i]] = fltG1;
			rgrgfltChDnMix[rgiOutCMap[iTarg2]][rgiInCMap[i]] = fltG2;
		}

		fltBTmp = 0;
		for (i = 0; i < cDstChan; i++)
		{
			fltTmp = 0;
			for(j = 0; j < cSrcChan; j++)
			{
				fltTmp += rgrgfltChDnMix[i][j];
			}
			if (fltBTmp < fltTmp)
			{
				fltBTmp = fltTmp;
			}
		}

		if (fltBTmp > 0 )
		{ 
			for(i = 0; i < cDstChan; i++)
			{
				for(j = 0; j < cSrcChan; j++)
				{
					//            rgrgfltChDnMixMtx[i][j] = 
					//                ((Float) (   (Int)     (1000.0F* rgrgfltChDnMixMtx[i][j]/fltBTmp + 0.5F) ) )/1000.0F;
					rgrgfltChDnMix[i][j] = (VO_S32)(((VO_S64)rgrgfltChDnMix[i][j] << CHDN_FRAC_BITS)/ fltBTmp);
				}
			}
			
		}
		else
		{
			// internal erorr: we can not deal with top speakers
			return VO_ERR_INVALID_ARG;
		}
	}
	return VO_ERR_NONE;
}

// Sets -1 if not found
VO_VOID GetChannelNumAtSpeakerPosition(VO_U32 nChannelMask, VO_U32 nSpeakerPosition, VO_S16 *pnChannelNum)
{
	
    *pnChannelNum = -1;
    if (nChannelMask & nSpeakerPosition) {
      // Determine the exact position: count 1's from LSB to required speaker position
        VO_U32 nTmp = 0x1;
        VO_S32 iCh;
        for (iCh = 0; iCh < 32; iCh++) {
            if (nTmp & nChannelMask) 
                *pnChannelNum = *pnChannelNum + 1;
            if (nTmp & nSpeakerPosition)
                break;
            nTmp <<= 1;
        } 
    }
}

VO_S32 InitChDnMixThroughProjections(VO_S32	cSrcChannel, 
									 VO_U32	nSrcChannelMask,
									 VO_S32	cDstChannel, 
									 VO_U32	nDstChannelMask, 
									 int**	rgrgfltChDnMix)
{
	VO_S32 hr = VO_ERR_NONE;
	VO_S32 i, iChSrc, iChDst, cSrcChannelTmp = 0, cDstChannelTmp = 0;
	VO_U32	nSrcChannelMaskTmp, nDstChannelMaskTmp, nMask = 0x1;
	VO_S32	rgrgfltChDnMixTmp[MAX_CHANNEL][MAX_CHANNEL];
	VO_S16	nSrcSubWooferChan = -1, nDstSubWooferChan = -1;

	if(rgrgfltChDnMix == NULL ||
		cSrcChannel > MAX_CHANNEL ||
		cSrcChannel < 1 ||
		cDstChannel > MAX_CHANNEL ||
		cDstChannel < 1 ||
		nSrcChannelMask == 0 ||
		nDstChannelMask == 0)
		return VO_ERR_INVALID_ARG;

	//not support Top speakers
	if((nSrcChannelMask & 0xFFFFF800) || 
		(nDstChannelMask & 0xFFFFF800))
		return VO_ERR_INVALID_ARG;
	
	for(i=0; i<18; i++)
	{
		cSrcChannelTmp += (nSrcChannelMask & nMask) ? 1 : 0;
        cDstChannelTmp += (nDstChannelMask & nMask) ? 1 : 0;
        nMask <<= 1;
	}

	if((cSrcChannelTmp != cSrcChannel) || (cDstChannelTmp != cDstChannel))
		return VO_ERR_INVALID_ARG;

	//Exclude sub-woofer and generate up-down matrix
	nSrcChannelMaskTmp = nSrcChannelMask;
	if(nSrcChannelMaskTmp & VO_CHANNEL_LOW_FREQUENCY)
	{
		nSrcChannelMaskTmp &= ~VO_CHANNEL_LOW_FREQUENCY;
		cSrcChannelTmp--;
	}

	nDstChannelMaskTmp = nDstChannelMask;
	if(nDstChannelMaskTmp & VO_CHANNEL_LOW_FREQUENCY)
	{
		nDstChannelMaskTmp &= ~VO_CHANNEL_LOW_FREQUENCY;
		cDstChannelTmp--;
	}

	if((cSrcChannelTmp < 1) || (cDstChannelTmp < 1))
		return VO_ERR_INVALID_ARG;

	//Prepare a matrix for the non-lfe(Low Frequency) channels (Low Frequency)
	hr = InitNonLFEChDnMixThroughProjections(cSrcChannelTmp, nSrcChannelMaskTmp, 
											cDstChannelTmp, nDstChannelMaskTmp, rgrgfltChDnMixTmp);
	if(hr < 0)return hr;

	// Find the channel number of LFE in src, dst, if present.
	GetChannelNumAtSpeakerPosition(nSrcChannelMask, VO_CHANNEL_LOW_FREQUENCY, &nSrcSubWooferChan);

	GetChannelNumAtSpeakerPosition(nDstChannelMask, VO_CHANNEL_LOW_FREQUENCY, &nDstSubWooferChan);

	if ((nSrcSubWooferChan != -1) && (nDstSubWooferChan != -1))
    {
		//both src and dst have LFE
		rgrgfltChDnMix[nDstSubWooferChan][nSrcSubWooferChan] = 0x80000;//CHDN_FROM_FLOAT(1.0F);
        for (iChDst = 0; iChDst < nDstSubWooferChan; iChDst++)
        {
            for (iChSrc = 0; iChSrc < nSrcSubWooferChan; iChSrc++)
            {
                rgrgfltChDnMix[iChDst][iChSrc] = rgrgfltChDnMixTmp[iChDst][iChSrc];
            }
            for (iChSrc = nSrcSubWooferChan+1; iChSrc < cSrcChannel; iChSrc++)
            {
                rgrgfltChDnMix[iChDst][iChSrc] = rgrgfltChDnMixTmp[iChDst][iChSrc-1];
            }
        }
        for (iChDst = nDstSubWooferChan+1; iChDst < cDstChannel; iChDst++)
        {
            for (iChSrc = 0; iChSrc < nSrcSubWooferChan; iChSrc++)
            {
                rgrgfltChDnMix[iChDst][iChSrc] = rgrgfltChDnMixTmp[iChDst-1][iChSrc];
            }
            for (iChSrc = nSrcSubWooferChan+1; iChSrc < cSrcChannel; iChSrc++)
            {
                rgrgfltChDnMix[iChDst][iChSrc] = rgrgfltChDnMixTmp[iChDst-1][iChSrc-1];
            }
        }
	}
	else if((nSrcSubWooferChan == -1) && (nDstSubWooferChan != -1))
	{
		// Only dst has an LFE: pump silence out of it
        for (iChDst = 0; iChDst < nDstSubWooferChan; iChDst++)
        {
            for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)
            {
                rgrgfltChDnMix[iChDst][iChSrc] = rgrgfltChDnMixTmp[iChDst][iChSrc];
            }
        }
        // leave nDstSubWooferChan with zeros
        for (iChDst = nDstSubWooferChan+1; iChDst < cDstChannel; iChDst++)
        {
            for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)
            {
                rgrgfltChDnMix[iChDst][iChSrc] = rgrgfltChDnMixTmp[iChDst-1][iChSrc];
            }
        }
	}
	else if((nSrcSubWooferChan != -1) && (nDstSubWooferChan == -1))
	{
		// Only Src has an LFE: spread 20% of it over all output channels
        for (iChDst = 0; iChDst < cDstChannel; iChDst++)
        {
            for (iChSrc = 0; iChSrc < nSrcSubWooferChan; iChSrc++)
            {
                rgrgfltChDnMix[iChDst][iChSrc] = (VO_S32)(((VO_S64)rgrgfltChDnMixTmp[iChDst][iChSrc]) * cDstChannel*10/(cDstChannel*10 + 2));
            }
            iChSrc = nSrcSubWooferChan;
            rgrgfltChDnMix[iChDst][iChSrc] = (1<<CHDN_FRAC_BITS)*2 / (cDstChannel*10+2);
            for (iChSrc = nSrcSubWooferChan+1; iChSrc < cSrcChannel; iChSrc++)
            {
                rgrgfltChDnMix[iChDst][iChSrc] = (VO_S32)(((VO_S64)rgrgfltChDnMixTmp[iChDst][iChSrc-1]) * cDstChannel*10/(cDstChannel*10 + 2));
            }
        }
	}
	else
	{
		// Neither src nor dst have LFEs: Simple copy
        for (iChDst = 0; iChDst < cDstChannel; iChDst++)
        {
            for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)
            {
                rgrgfltChDnMix[iChDst][iChSrc] = rgrgfltChDnMixTmp[iChDst][iChSrc];
            }
        }
	}

	return hr;
}

VO_S32 InitChanDnUpMix(ResInfo *pRes)
{
	VO_S32 hr = VO_ERR_NONE;
	VO_S32 iCh = 0, k = 0;
	VO_S32 cSrcChannel, cDstChannel;

	if(pRes == NULL)
		return VO_ERR_INVALID_ARG;

	cSrcChannel = pRes->ChanIn;
	cDstChannel = pRes->ChanOut;

	// Do not leak memory if re-initializing.
	if(pRes->m_rgrgfltChDnMix)
	{
		for(k=0; k<pRes->m_cDstChannelInChDnMix; k++)
		{
			if(pRes->m_rgrgfltChDnMix[k])
				voRESFree(pRes->m_rgrgfltChDnMix[k]);
		}
		voRESFree(pRes->m_rgrgfltChDnMix);
	}	
	if(pRes->m_pChTemp)
	{
		voRESFree(pRes->m_pChTemp);
	}

	// Allocate memory for m_rgrgfltChDnMix.
    pRes->m_rgrgfltChDnMix = (int **)  voRESCalloc (sizeof(int *) * cDstChannel);
    if (pRes->m_rgrgfltChDnMix == NULL) 
    {
        return VO_ERR_OUTOF_MEMORY;
    }
	pRes->m_cDstChannelInChDnMix = cDstChannel;
	for(iCh=0; iCh<cDstChannel; iCh++)
	{
		pRes->m_rgrgfltChDnMix[iCh] = (int *)voRESCalloc(sizeof(int) * cSrcChannel);
		if(pRes->m_rgrgfltChDnMix[iCh] == NULL)
			return VO_ERR_OUTOF_MEMORY;
	}
	
	pRes->ChanInMask = DefaultChannelMask(cSrcChannel);
	pRes->ChanOutMask = DefaultChannelMask(cDstChannel);

	if(pRes->ChanUpDown)
	{
		if((hr = InitChDnMixThroughProjections(cSrcChannel, pRes->ChanInMask,
						cDstChannel, pRes->ChanOutMask, pRes->m_rgrgfltChDnMix)<0))
			return hr;
	}
	return hr;
}

VO_S32	ChannelDownMixType(ResInfo *pRes, VO_U8 *pSrc, VO_U32 nBlocks, VO_U8 *pDst)
{
	VO_S32	SrcChan = pRes->ChanIn;
	VO_S32	DstChan = pRes->ChanOut;

//	VO_S32 *ptTemp = pRes->tmpbuf;

	VO_S32 iBlock, iChSrc, iChDst;
    VO_S32 sTemp=0;

	if(!pRes->ChanUpDown)
		return nBlocks;

	if (6 == SrcChan && 2 == DstChan)
    {
		// Unroll loop for popular scenario
        // Block transform: channel down mixing.
		VO_S16 *pSrcT = (VO_S16 *)pSrc;
		VO_S16 *pDstT = (VO_S16 *)pDst;

		int chdnMix00 = pRes->m_rgrgfltChDnMix[0][0];
        int chdnMix01 = pRes->m_rgrgfltChDnMix[0][1];
        int chdnMix02 = pRes->m_rgrgfltChDnMix[0][2];
        int chdnMix03 = pRes->m_rgrgfltChDnMix[0][3];
        int chdnMix04 = pRes->m_rgrgfltChDnMix[0][4];
        int chdnMix05 = pRes->m_rgrgfltChDnMix[0][5];
        int chdnMix10 = pRes->m_rgrgfltChDnMix[1][0];
        int chdnMix11 = pRes->m_rgrgfltChDnMix[1][1];
        int chdnMix12 = pRes->m_rgrgfltChDnMix[1][2];
        int chdnMix13 = pRes->m_rgrgfltChDnMix[1][3];
        int chdnMix14 = pRes->m_rgrgfltChDnMix[1][4];
        int chdnMix15 = pRes->m_rgrgfltChDnMix[1][5];
		iBlock = nBlocks;
		while (iBlock--)
        {
			int spTemp0, spTemp1;
			int	sample0 = *pSrcT++;//GetSample(0);
			int	sample1 = *pSrcT++;//GetSample(1);
			int	sample2 = *pSrcT++;//GetSample(2);
			int	sample3 = *pSrcT++;//GetSample(3);
			int	sample4 = *pSrcT++;//GetSample(4);
			int	sample5 = *pSrcT++;//GetSample(5);

			spTemp0 =	(int)MULT_CHDN(sample0, chdnMix00) +
						(int)MULT_CHDN(sample1, chdnMix01) +
						(int)MULT_CHDN(sample2, chdnMix02) +
						(int)MULT_CHDN(sample3, chdnMix03) +
						(int)MULT_CHDN(sample4, chdnMix04) +
						(int)MULT_CHDN(sample5, chdnMix05);

			spTemp1 =	(int)MULT_CHDN(sample0, chdnMix10) +
						(int)MULT_CHDN(sample1, chdnMix11) +
						(int)MULT_CHDN(sample2, chdnMix12) +
						(int)MULT_CHDN(sample3, chdnMix13) +
						(int)MULT_CHDN(sample4, chdnMix14) +
						(int)MULT_CHDN(sample5, chdnMix15);

			*pDstT++ = DNMIXROUNGE(spTemp0);
//			SetSample(sTemp, 0);

			*pDstT++ = DNMIXROUNGE(spTemp1);
//			SetSample(sTemp, 1);

//			DNMIXADSRCPTR(6);
//			DNMIXADDSTPTR(2);
		}
	}
#if 0
	else if(SrcChan=8 && DstChan=2)
	{
		VO_S16 *pSrcT = (VO_S16 *)pSrc;
		VO_S16 *pDstT = (VO_S16 *)pDst;

		int chdnMix00 = pRes->m_rgrgfltChDnMix[0][0];
        int chdnMix01 = pRes->m_rgrgfltChDnMix[0][1];
        int chdnMix02 = pRes->m_rgrgfltChDnMix[0][2];
        int chdnMix03 = pRes->m_rgrgfltChDnMix[0][3];
        int chdnMix04 = pRes->m_rgrgfltChDnMix[0][4];
        int chdnMix05 = pRes->m_rgrgfltChDnMix[0][5];
		int chdnMix06 = pRes->m_rgrgfltChDnMix[0][6];
        int chdnMix07 = pRes->m_rgrgfltChDnMix[0][7];
        int chdnMix10 = pRes->m_rgrgfltChDnMix[1][0];
        int chdnMix11 = pRes->m_rgrgfltChDnMix[1][1];
        int chdnMix12 = pRes->m_rgrgfltChDnMix[1][2];
        int chdnMix13 = pRes->m_rgrgfltChDnMix[1][3];
        int chdnMix14 = pRes->m_rgrgfltChDnMix[1][4];
        int chdnMix15 = pRes->m_rgrgfltChDnMix[1][5];
		int chdnMix16 = pRes->m_rgrgfltChDnMix[1][6];
        int chdnMix17 = pRes->m_rgrgfltChDnMix[1][7];

		while (iBlock--)
        {
			int spTemp0, spTemp1;
			int	sample0 = *pSrcT++;//GetSample(0);
			int	sample1 = *pSrcT++;//GetSample(1);
			int	sample2 = *pSrcT++;//GetSample(2);
			int	sample3 = *pSrcT++;//GetSample(3);
			int	sample4 = *pSrcT++;//GetSample(4);
			int	sample5 = *pSrcT++;//GetSample(5);
			int	sample6 = *pSrcT++;//GetSample(6);
			int	sample7 = *pSrcT++;//GetSample(7);

			spTemp0 =	(int)MULT_CHDN(sample0, chdnMix00) +
						(int)MULT_CHDN(sample1, chdnMix01) +
						(int)MULT_CHDN(sample2, chdnMix02) +
						(int)MULT_CHDN(sample3, chdnMix03) +
						(int)MULT_CHDN(sample4, chdnMix04) +
						(int)MULT_CHDN(sample5, chdnMix05) +
						(int)MULT_CHDN(sample6, chdnMix06) +
						(int)MULT_CHDN(sample7, chdnMix07);

			spTemp1 =	(int)MULT_CHDN(sample0, chdnMix10) +
						(int)MULT_CHDN(sample1, chdnMix11) +
						(int)MULT_CHDN(sample2, chdnMix12) +
						(int)MULT_CHDN(sample3, chdnMix13) +
						(int)MULT_CHDN(sample4, chdnMix14) +
						(int)MULT_CHDN(sample5, chdnMix15) +
						(int)MULT_CHDN(sample6, chdnMix16) +
						(int)MULT_CHDN(sample7, chdnMix17);

			*pDstT++ = DNMIXROUNGE(spTemp0);
			*pDstT++ = DNMIXROUNGE(spTemp1);
	}
#endif
	else if(SrcChan > DstChan)
	{
		VO_S16 *pSrcT = (VO_S16 *)pSrc;
		VO_S16 *pDstT = (VO_S16 *)pDst;
		// Block transform: channel down mixing.
		iBlock = nBlocks;
        while (iBlock--)
		{
			// Initialize
 //           ptTemp = (VO_S32 *)memset(ptTemp, 0, sizeof(CoefType) * DstChan);
			
			for (iChDst = 0; iChDst < DstChan; iChDst++)
            {
				sTemp = 0;
                for (iChSrc = 0; iChSrc < SrcChan; iChSrc++)
                {
                    int c = pSrcT[iChSrc];//GET_COEFF(iChSrc);
                    sTemp += MULT_CHDN(c, pRes->m_rgrgfltChDnMix[iChDst][iChSrc]);
                }
				*pDstT++ = (VO_S16)DNMIXROUNGE(sTemp);
            }
			pSrcT += SrcChan;
            //Stride ahead to the next block
//            DNMIXADVSRCPTR(cSrcChannel);
//            DNMIXADVDSTPTR(cDstChannel);
        }
	}
	else if(SrcChan < DstChan)
	{
		// up mixing done backwards so as to not overwrite needed memory
        // Block transform: channel up mixing.
		VO_S16 *pSrcT = ((VO_S16 *)pSrc) + SrcChan * (nBlocks-1);
		VO_S16 *pDstT = ((VO_S16 *)pDst) + DstChan * nBlocks - 1;
		iBlock = nBlocks;
//		pDstT--;
		while(iBlock--)
		{
			// Initialize
//            ptTemp = (VO_S32 *)memset(ptTemp, 0, sizeof(CoefType) * DstChan);
			for (iChDst = 0; iChDst < DstChan; iChDst++)
            {
				sTemp = 0;
                for (iChSrc = 0; iChSrc < SrcChan; iChSrc++)
                {
                    sTemp += MULT_CHDN(pSrcT[iChSrc], pRes->m_rgrgfltChDnMix[iChDst][iChSrc]);
                }
				*pDstT-- = (VO_S16)DNMIXROUNGE(sTemp);
            }
			pSrcT -= SrcChan;
            //Stride back to the previous block
//            DNMIXADVSRCPTR(-cSrcChannel);
//            DNMIXADVDSTPTR(-cDstChannel);
		}
	}
	return nBlocks;
}

