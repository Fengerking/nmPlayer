//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    residue.c

Abstract:

    residue backend 0, 1 and 2 unpack c file.

Author:

    Witten Wen 9-October-2009

Revision History:

*************************************************************************/


#include "vovorbisdec.h"
#include "residue.h"
//#include "block.h"

static void voRes0_free_info(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfoResidue *i)
{
	VorbisInfoResidue0 *info = (VorbisInfoResidue0 *)i;
	if(info)
	{
		voOGGMemset(info, 0, sizeof(*info));
		voOGGFree(info);
	}
}

static void voRes0_free_look(CVOVorbisObjectDecoder *pvorbisdec, VorbisLookResidue *i)
{
	int j;
	if(i)
	{

		VorbisLookResidue0 *look = (VorbisLookResidue0 *)i;

		for(j=0; j<look->m_Parts; j++)
			if (look->m_pppPartBooks[j])voOGGFree(look->m_pppPartBooks[j]);
		voOGGFree(look->m_pppPartBooks);
		for(j=0; j<look->m_Partvals; j++)
			voOGGFree(look->m_ppDecodeMap[j]);
		voOGGFree(look->m_ppDecodeMap);
		
		voOGGMemset(look, 0, sizeof(*look));
		voOGGFree(look);
	}
}

static int icount(unsigned int v)
{
	int ret = 0;
	while(v)
	{
		ret += v&1;
		v >>= 1;
	}
	return(ret);
}

/* VorbisInfo is for range checking */
VorbisInfoResidue *voRes0_unpack(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi, VOOGGInputBitStream *obs)
{
	int j, acc = 0;
	CodecSetupInfo     *ci   = (CodecSetupInfo *)vi->m_pCodecSetup;
	VorbisInfoResidue0 *info = (VorbisInfoResidue0 *)voOGGCalloc(1, sizeof(*info));
	

	info->m_Begin = voOGGDecPackReadBits(obs, 24);
	info->m_End   = voOGGDecPackReadBits(obs, 24);
	info->m_Grouping   = voOGGDecPackReadBits(obs, 24) + 1;
	info->m_Partitions = voOGGDecPackReadBits(obs, 6) + 1;
	info->m_GroupBook  = voOGGDecPackReadBits(obs, 8);

	for(j=0; j<info->m_Partitions; j++)
	{
		int cascade = voOGGDecPackReadBits(obs, 3);
		if(voOGGDecPackReadBits(obs, 1))
			cascade |= (voOGGDecPackReadBits(obs, 5)<<3);
		info->m_SecondStage[j] = cascade;

		acc += icount(cascade);
	}
	for(j=0; j<acc; j++)
		info->m_BookList[j] = voOGGDecPackReadBits(obs, 8);

	if (info->m_GroupBook>=ci->m_Books)goto errout;
	for(j=0; j<acc; j++)
		if (info->m_BookList[j]>=ci->m_Books)goto errout;

	return(info);
errout:
	voRes0_free_info(pvorbisdec, info);
	return(NULL);
}

VorbisLookResidue *voRes0_look(CVOVorbisObjectDecoder *pvorbisdec, 
							   VorbisDSPState *vd, 
							   VorbisInfoMode *vm,
							   VorbisInfoResidue *vr)
{
	VorbisInfoResidue0 *info = (VorbisInfoResidue0 *)vr;
	CodecSetupInfo     *ci   = (CodecSetupInfo *)vd->m_pVI->m_pCodecSetup;
	int j, k, acc = 0;
	int dim;
	int maxstage = 0;
	VorbisLookResidue0 *look = (VorbisLookResidue0 *)voOGGCalloc(1, sizeof(*look));
	

	
	look->info = info;
//	look->map = vm->m_Mapping;

	look->m_Parts = info->m_Partitions;
//	look->fullbooks = ci->m_pFullbooks;
	look->m_pPhraseBook = ci->m_pFullbooks + info->m_GroupBook;
	dim = look->m_pPhraseBook->m_Dim;

	look->m_pppPartBooks = (CodeBook ***)voOGGCalloc(look->m_Parts, sizeof(*look->m_pppPartBooks));

	for(j=0; j<look->m_Parts; j++)
	{
		int stages = ILOG(info->m_SecondStage[j]);
		if(stages)
		{
			if (stages>maxstage)
				maxstage = stages;
			look->m_pppPartBooks[j] = (CodeBook **)voOGGCalloc(stages, sizeof(*look->m_pppPartBooks[j]));
			for(k=0; k<stages; k++)
				if (info->m_SecondStage[j]&(1<<k))
				{
					look->m_pppPartBooks[j][k] = ci->m_pFullbooks + info->m_BookList[acc++];
#ifdef TRAIN_RES
					look->training_data[k][j] = voOGGCalloc(look->m_pppPartBooks[j][k]->m_Entries,
						sizeof(***look->training_data));
#endif
				}
		}
	}

	look->m_Partvals = look->m_Parts;
	for(j=1; j<dim; j++)
		look->m_Partvals *= look->m_Parts;
	look->m_Stages = maxstage;
	look->m_ppDecodeMap = (int **)voOGGMalloc(look->m_Partvals * sizeof(*look->m_ppDecodeMap));
	for(j=0; j<look->m_Partvals; j++)
	{
		long val = j;
		long mult = look->m_Partvals/look->m_Parts;
		look->m_ppDecodeMap[j] = (int *)voOGGMalloc(dim * sizeof(*look->m_ppDecodeMap[j]));
		for(k=0; k<dim; k++)
		{
			long deco = val / mult;
			val  -= deco * mult;
			mult /= look->m_Parts;
			look->m_ppDecodeMap[j][k] = deco;
		}
	}

	return(look);
}

/* a truncated packet here just means 'stop working'; it's not an error */
//_01inverse
static int ResInverse01(CVOVorbisObjectDecoder *pvorbisdec, 
						VorbisBlock *vb, VorbisLookResidue *vl,
					  OGG_S32 **in, int ch,
					  long (*decodepart)(CVOVorbisObjectDecoder *, CodeBook *, 
							OGG_S32 *, int, int))
{

	long i, j, k, l, s;
	VorbisLookResidue0 *look = (VorbisLookResidue0 *)vl;
	VorbisInfoResidue0 *info = look->info;

	/* move all this setup out later */
	int samples_per_partition = info->m_Grouping;
	int partitions_per_word = look->m_pPhraseBook->m_Dim;
	int max = vb->m_PCMEnd>>1;
	int end = (info->m_End<max?info->m_End:max);
	int n = end - info->m_Begin;

	if(n>0)
	{
		int partvals = n / samples_per_partition;		
		int partwords = (partvals + partitions_per_word - 1) / partitions_per_word;
		int ***partword = (int ***)voOGGMalloc(ch*sizeof(*partword));
		if(partvals*samples_per_partition<n){
			//unknown why? This statement should never happen. Can we find such case?
			goto errout;
		}
		if(partword==NULL){
			goto errout;
		}

		for(j=0; j<ch; j++)
			partword[j] = (int **)voOGGDecBlockAlloc(pvorbisdec, vb, partwords*sizeof(*partword[j]));

		for(s=0; s<look->m_Stages; s++)
		{
			/* each loop decodes on partition codeword containing 
			partitions_pre_word partitions */
			for(i=0,l=0; i<partvals; l++)
			{
				if (s==0)
				{
					/* fetch the partition word for each channel */
					for(j=0; j<ch; j++)
					{
						int temp = voOGGDecBookDecode(look->m_pPhraseBook, pvorbisdec->m_oggbs);
						if (temp==-1){
							goto errout;
						}
						partword[j][l] = look->m_ppDecodeMap[temp];
						if (partword[j][l]==NULL){
							goto errout;
						}
					}
				}

				/* now we decode residual values for the partitions for ResInverse01*/
				for(k=0; k<partitions_per_word && i<partvals; k++,i++)
					for(j=0; j<ch; j++)
					{
						long offset = info->m_Begin + i*samples_per_partition;
						if (info->m_SecondStage[partword[j][l][k]]&(1<<s))
						{
							CodeBook *stagebook = look->m_pppPartBooks[partword[j][l][k]][s];
							if(stagebook)
							{
								if(decodepart(pvorbisdec, stagebook, in[j]+offset, samples_per_partition,-8) == -1)
									goto eopbreak;
							}
						}
					}
			} 
		}
		voOGGFree(partword);
	}
errout:
eopbreak:
	return(0);
}

int voRes0_inverse(CVOVorbisObjectDecoder *pvorbisdec, 
				 VorbisBlock *vb, 
				 VorbisLookResidue *vl,
				 OGG_S32 **in, 
				 int *nonzero, 
				 int ch)
{
	int i, used = 0;
	for(i=0; i<ch; i++)
		if(nonzero[i])
			in[used++] = in[i];
	if(used)
		return(ResInverse01(pvorbisdec, vb, vl, in, used, VorbisBookDecodevsAdd));
	else
		return(0);
}

int voRes1_inverse(CVOVorbisObjectDecoder *pvorbisdec, 
				 VorbisBlock *vb,
				 VorbisLookResidue *vl,
				 OGG_S32 **in,
				 int *nonzero,
				 int ch)
{
	int i, used = 0;
	for(i=0; i<ch; i++)
		if (nonzero[i])
			in[used++] = in[i];
	if(used)
		return(ResInverse01(pvorbisdec, vb, vl, in, used, VorbisBookDecodevAdd));
	else
		return(0);

}

/* duplicate code here as speed is somewhat more important 
   Refer to SPEC 
 */
int voRes2_inverse(CVOVorbisObjectDecoder *pvorbisdec, 
				 VorbisBlock *vb,
				 VorbisLookResidue *vl,
				 OGG_S32 **in,
				 int *nonzero,
				 int ch)
{
	long i, k, l, s;
	VorbisLookResidue0 *look = (VorbisLookResidue0 *)vl;
	VorbisInfoResidue0 *info = look->info;

	/* move all this setup out later */
	int samples_per_partition = info->m_Grouping;
	int partitions_per_word   = look->m_pPhraseBook->m_Dim;
	int max = (vb->m_PCMEnd*ch)>>1;
	int end = (info->m_End<max?info->m_End:max);
	int n   = end-info->m_Begin;
	int used = 0;

	//check nonzere, if no nonzero vector
	for(i=0; i<ch; i++){
		if (nonzero[i]){
			used++;
		}
	}
	if(!used){     
		return(0); 
	}

	if (n>0)
	{
		int partvals   = n/samples_per_partition;
		int partwords  = (partvals+partitions_per_word-1)/partitions_per_word;
		int **partword = NULL;
		int beginoff   = info->m_Begin/ch;		

		partword = (int **)voOGGDecBlockAlloc(pvorbisdec, vb, partwords*sizeof(*partword));
		samples_per_partition /= ch;
		for (s=0; s<look->m_Stages; s++)
		{
			for (i=0,l=0; i<partvals; l++)
			{
				if (s==0)
				{
					/* fetch the partition word */
					int temp = voOGGDecBookDecode(look->m_pPhraseBook, pvorbisdec->m_oggbs);
					if (temp==-1){
						goto eopbreak;
					}
					partword[l] = look->m_ppDecodeMap[temp];
					if(partword[l]==NULL){
						goto errout;
					}
				}

				/* now we decode residual values for the partition */
				for(k=0; k<partitions_per_word && i<partvals; k++,i++)
					if (info->m_SecondStage[partword[l][k]]&(1<<s))
					{
						CodeBook *stagebook = look->m_pppPartBooks[partword[l][k]][s];

						if (stagebook)
						{
							if (VorbisBookDecodevvAdd(pvorbisdec, stagebook, in,
								i*samples_per_partition+beginoff,ch, samples_per_partition, -8)==-1){
								goto eopbreak;
							}
						}
					}
			}// end of for (i=0,l=0; i<partvals; l++) 
		}//end of for (s=0; s<look->m_Stages; s++) 
	}//end of if (n>0)
errout:
eopbreak:

	return(0);
}

VorbisFuncResidue residue0_exportbundle =
{
	&voRes0_unpack,
	&voRes0_look,
	&voRes0_free_info,
	&voRes0_free_look,
	&voRes0_inverse
};

VorbisFuncResidue residue1_exportbundle = 
{
	&voRes0_unpack,
	&voRes0_look,
	&voRes0_free_info,
	&voRes0_free_look,
	&voRes1_inverse
};

VorbisFuncResidue residue2_exportbundle = 
{
	&voRes0_unpack,
	&voRes0_look,
	&voRes0_free_info,
	&voRes0_free_look,
	&voRes2_inverse
};
