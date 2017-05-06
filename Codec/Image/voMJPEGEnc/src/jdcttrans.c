#include "jconfig.h"
#include "jencoder.h"
#include "stdio.h"

#ifdef VOARMV6
extern void fdct_armv6(INT16 *);
#endif


#ifdef IPP_SUPPORT
void	jpeg_dctquantfwd(JPEG_MCU_BLOCK*		outblock,
						 JPEG_QUANT_TABLE*		quant_tbl)
{
	UINT32 bc;
	JPEG_QUANT_TBL* qtab;
	
	for(bc = 0; bc < outblock->blockNum; bc++)
	{
		qtab = quant_tbl->Quant_tbl_ptrs[outblock->QuantImage[bc]];
		ippiDCTQuantFwd_JPEG_16s_I(outblock->WorkBlock[bc],
								   qtab->quantval);
	}

	return;
}
#else //IPP_SUPPORT

#if 0

void FDCT(INT16* dataptr)
{
	INT32 ctr;
	INT32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	INT32 tmp10, tmp11, tmp12, tmp13;
	INT32 z1, z2, z3, z4, z5, z11, z13;	
	INT16* dataptrbk;

	dataptrbk = dataptr;
	
	for (ctr = JPEG_BLOCK_LINE; ctr; ctr--) 
	{
		tmp0 = dataptr[0] + dataptr[7];
		tmp7 = dataptr[0] - dataptr[7];
		tmp1 = dataptr[1] + dataptr[6];
		tmp6 = dataptr[1] - dataptr[6];
		tmp2 = dataptr[2] + dataptr[5];
		tmp5 = dataptr[2] - dataptr[5];
		tmp3 = dataptr[3] + dataptr[4];
		tmp4 = dataptr[3] - dataptr[4];
		
		tmp10 = tmp0 + tmp3;	
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;
		
		dataptr[0] = tmp10 + tmp11;
		dataptr[4] = tmp10 - tmp11;
		
		z1 = (((tmp12 + tmp13) * FIX_0_707106781) >> 8);
		dataptr[2] = tmp13 + z1;		
		dataptr[6] = tmp13 - z1;
		
		tmp10 = tmp4 + tmp5;	
		tmp11 = tmp5 + tmp6;
		tmp12 = tmp6 + tmp7;
		
		z5 = (((tmp10 - tmp12) * FIX_0_382683433) >> 8); 
		z2 = ((tmp10 * FIX_0_541196100) >> 8) + z5; 
		z4 = ((tmp12 * FIX_1_306562965) >> 8) + z5; 
		z3 = ((tmp11 * FIX_0_707106781) >> 8); 
		
		z11 = tmp7 + z3;		
		z13 = tmp7 - z3;
		
		dataptr[5] = z13 + z2;	
		dataptr[3] = z13 - z2;
		dataptr[1] = z11 + z4;
		dataptr[7] = z11 - z4;
		
		dataptr += JPEG_BLOCK_LINE;	
	}
	
	/* Pass 2: process columns. */		
	dataptr = dataptrbk;
	for (ctr = JPEG_BLOCK_LINE; ctr; ctr--) 
	{
		tmp0 = dataptr[ 0] + dataptr[56];
		tmp7 = dataptr[ 0] - dataptr[56];
		tmp1 = dataptr[ 8] + dataptr[48];
		tmp6 = dataptr[ 8] - dataptr[48];
		tmp2 = dataptr[16] + dataptr[40];
		tmp5 = dataptr[16] - dataptr[40];
		tmp3 = dataptr[24] + dataptr[32];
		tmp4 = dataptr[24] - dataptr[32];			
		
		tmp10 = tmp0 + tmp3;	
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;
		
		dataptr[ 0] = tmp10 + tmp11; 
		dataptr[32] = tmp10 - tmp11;
		
		z1 = (((tmp12 + tmp13) * FIX_0_707106781) >> 8) ; 
		dataptr[16] = tmp13 + z1; 
		dataptr[48] = tmp13 - z1;
		
		tmp10 = tmp4 + tmp5;
		tmp11 = tmp5 + tmp6;
		tmp12 = tmp6 + tmp7;
		
		z5 = (((tmp10 - tmp12) * FIX_0_382683433) >> 8); 
		z2 = ((tmp10 * FIX_0_541196100) >> 8) + z5; 
		z4 = ((tmp12 * FIX_1_306562965) >> 8) + z5;
		z3 = ((tmp11 * FIX_0_707106781) >> 8); 
		
		z11 = tmp7 + z3;		
		z13 = tmp7 - z3;
		
		dataptr[40] = z13 + z2;
		dataptr[24] = z13 - z2;
		dataptr[ 8] = z11 + z4;
		dataptr[56] = z11 - z4;
		
		dataptr++;			
	}
}

void	jpeg_dctquantfwd(JPEG_MCU_BLOCK*		outblock,
						 JPEG_QUANT_TABLE*		quant_tbl)
{
	INT32 ctr;
	INT16 *dataptr;
	INT16 *qvaltab, *invqvaltab;
	INT32 bc; 
	INT32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	INT32 tmp10, tmp11, tmp12, tmp13;
	INT32 z1, z2, z3, z4, z5, z11, z13;	
	
	for(bc = 0; bc < outblock->blockNum; bc++)
	{
		/* Pass 1: process rows. */
		dataptr = outblock->WorkBlock[bc];
#if 0
		FDCT(dataptr);
#else
		for (ctr = JPEG_BLOCK_LINE; ctr; ctr--) 
		{
			tmp0 = dataptr[0] + dataptr[7];
			tmp7 = dataptr[0] - dataptr[7];
			tmp1 = dataptr[1] + dataptr[6];
			tmp6 = dataptr[1] - dataptr[6];
			tmp2 = dataptr[2] + dataptr[5];
			tmp5 = dataptr[2] - dataptr[5];
			tmp3 = dataptr[3] + dataptr[4];
			tmp4 = dataptr[3] - dataptr[4];
			
			tmp10 = tmp0 + tmp3;	
			tmp13 = tmp0 - tmp3;
			tmp11 = tmp1 + tmp2;
			tmp12 = tmp1 - tmp2;
			
			dataptr[0] = tmp10 + tmp11;
			dataptr[4] = tmp10 - tmp11;
			
			z1 = (((tmp12 + tmp13) * FIX_0_707106781) >> 8);
			dataptr[2] = tmp13 + z1;		
			dataptr[6] = tmp13 - z1;
			
			tmp10 = tmp4 + tmp5;	
			tmp11 = tmp5 + tmp6;
			tmp12 = tmp6 + tmp7;

			z5 = (((tmp10 - tmp12) * FIX_0_382683433) >> 8); 
			z2 = ((tmp10 * FIX_0_541196100) >> 8) + z5; 
			z4 = ((tmp12 * FIX_1_306562965) >> 8) + z5; 
			z3 = ((tmp11 * FIX_0_707106781) >> 8); 
			
			z11 = tmp7 + z3;		
			z13 = tmp7 - z3;
			
			dataptr[5] = z13 + z2;	
			dataptr[3] = z13 - z2;
			dataptr[1] = z11 + z4;
			dataptr[7] = z11 - z4;
			
			dataptr += JPEG_BLOCK_LINE;	
		}
		
		/* Pass 2: process columns. */		
		dataptr = outblock->WorkBlock[bc];
		for (ctr = JPEG_BLOCK_LINE; ctr; ctr--) 
		{
			tmp0 = dataptr[ 0] + dataptr[56];
			tmp7 = dataptr[ 0] - dataptr[56];
			tmp1 = dataptr[ 8] + dataptr[48];
			tmp6 = dataptr[ 8] - dataptr[48];
			tmp2 = dataptr[16] + dataptr[40];
			tmp5 = dataptr[16] - dataptr[40];
			tmp3 = dataptr[24] + dataptr[32];
			tmp4 = dataptr[24] - dataptr[32];			
			
			tmp10 = tmp0 + tmp3;	
			tmp13 = tmp0 - tmp3;
			tmp11 = tmp1 + tmp2;
			tmp12 = tmp1 - tmp2;
			
			dataptr[ 0] = tmp10 + tmp11; 
			dataptr[32] = tmp10 - tmp11;
			
			z1 = (((tmp12 + tmp13) * FIX_0_707106781) >> 8) ; 
			dataptr[16] = tmp13 + z1; 
			dataptr[48] = tmp13 - z1;
			
			tmp10 = tmp4 + tmp5;
			tmp11 = tmp5 + tmp6;
			tmp12 = tmp6 + tmp7;

			z5 = (((tmp10 - tmp12) * FIX_0_382683433) >> 8); 
			z2 = ((tmp10 * FIX_0_541196100) >> 8) + z5; 
			z4 = ((tmp12 * FIX_1_306562965) >> 8) + z5;
 			z3 = ((tmp11 * FIX_0_707106781) >> 8); 
			
			z11 = tmp7 + z3;		
			z13 = tmp7 - z3;
			
			dataptr[40] = z13 + z2;
			dataptr[24] = z13 - z2;
			dataptr[ 8] = z11 + z4;
			dataptr[56] = z11 - z4;
			
			dataptr++;			
		}
#endif
		
		dataptr = outblock->WorkBlock[bc];
		ctr = outblock->QuantImage[bc];
		qvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->quantval;
		invqvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->invquantval;
		{
			INT32 qval, ddata, invqval;	
			for (ctr = 0; ctr < JPEG_BLOCK_SIZE; ctr++) 
			{
				qval = qvaltab[ctr];
				ddata = dataptr[ctr];
				invqval = invqvaltab[ctr];
			
#if 0
				if(ddata > 0)
				{
					ddata += (qval >> 1);					
					if(ddata - qval < 0)
					{
						dataptr[ctr] = 0;
					}
					else
					{
						//ddata /= qval;
						ddata = ddata * invqval >> 16;
						dataptr[ctr] = (INT16) ddata;
					}
				}
				else
				{
					ddata = -ddata + (qval >> 1);
					
					if(ddata - qval< 0)
					{
						dataptr[ctr] = 0;
					}
					else
					{
						//ddata /= qval;
						ddata = ddata * invqval >> 16;
						ddata = -ddata;
						dataptr[ctr] = (INT16) ddata;
					}				
				}
#else				
				if(abs(ddata) < qval)
				{
					dataptr[ctr] = 0;
				}
				else
				{
					ddata = ddata * invqval >> 16;
					dataptr[ctr] = (INT16) ddata;
				}
#endif
			}
		}
	}

	return;
}

#else

//#define PRECISE_QUANT
#ifdef PRECISE_QUANT

#define Quant(n) if(ddata > 0)				\
				{							\
					ddata += (qval >> 1);	\
					if(ddata - qval < 0)	\
					{						\
						dataptr[n] = 0;		\
					}						\
					else					\
					{						\
						ddata = ddata * invqval >> 16;	\
						dataptr[n] =  ddata;			\
					}									\
				}										\
				else									\
				{										\
					ddata = -ddata + (qval >> 1);		\
					if(ddata - qval< 0)					\
					{									\
						dataptr[n] = 0;					\
					}									\
					else								\
					{									\
						ddata = ddata * invqval >> 16;	\
						ddata = -ddata;					\
						dataptr[n] =  ddata;			\
					}									\
				}										\

#else

#define Quant(n) if(abs(ddata) < qval)					\
				 {										\
				 	 dataptr[n] = 0;						\
				 }										\
				 else									\
				 {										\
					 ddata = ddata * invqval >> 16;		\
					 dataptr[n] = (UINT16)( ddata);					\
				 }										\

#endif  

void	jpeg_dctquantfwd1(JPEG_MCU_BLOCK*		outblock,
						 JPEG_QUANT_TABLE*		quant_tbl)
{
	INT32 ctr;
	//INT16 *dataptr;
	INT16 *dataptr;
	UINT16 *qvaltab, *invqvaltab;
	INT32 bc; 
	INT32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	INT32 tmp10, tmp11, tmp12, tmp13;
	INT32 z1, z2, z3, z4, z5, z11, z13;
	INT32 qval, ddata, invqval;	
//	static tim=0;
	
	for(bc = 0; bc < outblock->blockNum; bc++)
	{
		/* Pass 1: process rows. */
		dataptr = outblock->WorkBlock[bc];
		ctr = outblock->QuantImage[bc];
		qvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->quantval;
		invqvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->invquantval;
		//fdct_int32(dataptr,qvaltab,invqvaltab);
		//continue;
		for (ctr = JPEG_BLOCK_LINE; ctr; ctr--) 
		{
			tmp0 = dataptr[0] + dataptr[7];
			tmp7 = dataptr[0] - dataptr[7];
			tmp1 = dataptr[1] + dataptr[6];
			tmp6 = dataptr[1] - dataptr[6];
			tmp2 = dataptr[2] + dataptr[5];
			tmp5 = dataptr[2] - dataptr[5];
			tmp3 = dataptr[3] + dataptr[4];
			tmp4 = dataptr[3] - dataptr[4];
			
			tmp10 = tmp0 + tmp3;	
			tmp13 = tmp0 - tmp3;
			tmp11 = tmp1 + tmp2;
			tmp12 = tmp1 - tmp2;
			
			dataptr[0] = (UINT16)( tmp10 + tmp11);
			dataptr[4] = (UINT16)(tmp10 - tmp11);
			
			z1 = (((tmp12 + tmp13) * FIX_0_707106781) >> 8);
			dataptr[2] = (UINT16)(tmp13 + z1);		
			dataptr[6] = (UINT16)(tmp13 - z1);
			//dataptr[2] = (tmp13*437+tmp12*181)>>8;
			//dataptr[6] = (tmp13*75-tmp12*181)>>8;
			
			tmp10 = tmp4 + tmp5;	
			tmp11 = tmp5 + tmp6;
			tmp12 = tmp6 + tmp7;
			
			z5 = (((tmp10 - tmp12) * FIX_0_382683433) >> 8); 
			z2 = ((tmp10 * FIX_0_541196100) >> 8) + z5; 
			z4 = ((tmp12 * FIX_1_306562965) >> 8) + z5; 
			z3 = ((tmp11 * FIX_0_707106781) >> 8); 
			
			z11 = tmp7 + z3;	
			z13 = tmp7 - z3;

			dataptr[5] = (UINT16)(z13 + z2);	
			dataptr[3] = (UINT16)(z13 - z2);
			dataptr[1] = (UINT16)(z11 + z4);
			dataptr[7] = (UINT16)(z11 - z4);
			
			//dataptr[5] = (tmp4*237 + tmp5*56 -tmp6*279 + tmp7*177)>>8;//z13 + z2;	
			//dataptr[3] = (-tmp4*237 - tmp5*418 - tmp6*83 + tmp7*354 )>>8;//z13 - z2;
			//dataptr[1] = (tmp4*98 + tmp5*279 + tmp6*417 + tmp7*492 )>>8;//z11 + z4;
			//dataptr[7] = (-tmp4*98 - tmp5*83 - tmp6*55 + tmp7*20 )>>8;//z11 - z4;
			
			dataptr += JPEG_BLOCK_LINE;	
		}
		
		/* Pass 2: process columns. */		
		dataptr = outblock->WorkBlock[bc];
		//ctr = outblock->QuantImage[bc];
		//qvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->quantval;
		//invqvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->invquantval;
		for (ctr = JPEG_BLOCK_LINE; ctr; ctr--) 
		{
			tmp0 = dataptr[ 0] + dataptr[56];
			tmp7 = dataptr[ 0] - dataptr[56];
			tmp1 = dataptr[ 8] + dataptr[48];
			tmp6 = dataptr[ 8] - dataptr[48];
			tmp2 = dataptr[16] + dataptr[40];
			tmp5 = dataptr[16] - dataptr[40];
			tmp3 = dataptr[24] + dataptr[32];
			tmp4 = dataptr[24] - dataptr[32];			
			
			tmp10 = tmp0 + tmp3;	
			tmp13 = tmp0 - tmp3;
			tmp11 = tmp1 + tmp2;
			tmp12 = tmp1 - tmp2;
			
			ddata = tmp10 + tmp11;
			qval = qvaltab[0];
			invqval = invqvaltab[0];
			Quant(0);
			/*if(abs(ddata) < qval)					
			{										
				dataptr[0] = 0;						
			}										
			else									
			{										
				ddata = ddata * invqval >> 16;		
				dataptr[0] = ddata;					
			}*/	

			ddata = tmp10 - tmp11;
			qval = qvaltab[32];
			invqval = invqvaltab[32];
			Quant(32);
			
			z1 = (((tmp12 + tmp13) * FIX_0_707106781) >> 8); 
			
			ddata = tmp13 + z1;
			qval = qvaltab[16];
			invqval = invqvaltab[16];
			Quant(16);
			
			ddata = tmp13 - z1;
			qval = qvaltab[48];
			invqval = invqvaltab[48];
			Quant(48);
			
			tmp10 = tmp4 + tmp5;
			tmp11 = tmp5 + tmp6;
			tmp12 = tmp6 + tmp7;
			
			z5 = (((tmp10 - tmp12) * FIX_0_382683433) >> 8); 
			z2 = ((tmp10 * FIX_0_541196100) >> 8) + z5; 
			z4 = ((tmp12 * FIX_1_306562965) >> 8) + z5;
			z3 = ((tmp11 * FIX_0_707106781) >> 8); 
			
			z11 = tmp7 + z3;		
			z13 = tmp7 - z3;
			
			ddata = z13 + z2;
			qval = qvaltab[40];
			invqval = invqvaltab[40];
			Quant(40);			
			
			ddata = z13 - z2;
			qval = qvaltab[24];
			invqval = invqvaltab[24];
			Quant(24);
			
			ddata = z11 + z4;
			qval = qvaltab[8];
			invqval = invqvaltab[8];
			Quant(8);
			
			ddata = z11 - z4;
			qval = qvaltab[56];
			invqval = invqvaltab[56];
			Quant(56);
			
			dataptr++;
			qvaltab++;
			invqvaltab++;
		}
		//{
		//	int k=0;
		//	int j,i;
		//	FILE *fp =fopen("E:\\das_c.txt","a");
		//	for(i=0;i<JPEG_BLOCK_LINE;i++)
		//	{
		//		for(j=0;j<JPEG_BLOCK_LINE;j++)
		//		{
		//			//fprintf(fp,"%4d ",dataptr[k++]);
		//			fprintf(fp,"%4d ",outblock->WorkBlock[bc][k++]);
		//		}
		//		fprintf(fp,"\n");
		//	}
		//	fclose(fp);
		//}
	}
		
	return;
}

#ifdef VOARMV6
void	jpeg_dctquantfwd_ARMv6(JPEG_MCU_BLOCK*		outblock,
						 JPEG_QUANT_TABLE*		quant_tbl)
{
	INT32 ctr;
	INT16 *dataptr;
	UINT16 *qvaltab, *invqvaltab;
	INT32 bc; //i; 
//	INT32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
//	INT32 tmp10, tmp11, tmp12, tmp13;
//	INT32 z1, z2, z3, z4, z5, z11, z13;
//	INT32 qval, ddata, invqval;	
	
	for(bc = 0; bc < outblock->blockNum; bc++)
	{
		dataptr = outblock->WorkBlock[bc];
		ctr = outblock->QuantImage[bc];
		qvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->quantval;
		invqvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->invquantval;

		//for(i=0;i<64;i++)
			//dataptr[i]=dataptr[i]*qvaltab[i];
		fdct_armv6(dataptr);
	}		
	return;
}
#endif

#ifdef VOARMV7

void fdct_armv7(INT16 *,UINT16 *,UINT16 *);	

void	jpeg_dctquantfwd_ARMv7(JPEG_MCU_BLOCK*		outblock,
						 JPEG_QUANT_TABLE*		quant_tbl)
{
	INT32 ctr;
	INT16 *dataptr;
	UINT16 *qvaltab, *invqvaltab;
	INT32 bc; //i; 

	//INT16 *dataptr2 = (INT16 *)mem_malloc(64*sizeof(INT16), ALIGN_NUM_32);
	//UINT16 *qvaltab2=(UINT16 *)mem_malloc(64*sizeof(UINT16), ALIGN_NUM_32);
	//UINT16 *invqvaltab2=(UINT16 *)mem_malloc(64*sizeof(UINT16), ALIGN_NUM_32);
	
	for(bc = 0; bc < outblock->blockNum; bc++)
	{
		dataptr = outblock->WorkBlock[bc];
		ctr = outblock->QuantImage[bc];
		qvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->quantval;
		invqvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->invquantval;
		//memcpy(qvaltab2,qvaltab,64*2);
		//memcpy(invqvaltab2,invqvaltab,64*2);
		//for(i=0;i<64;i++)
			//dataptr2[i]=dataptr[i];

		fdct_armv7(dataptr,qvaltab,invqvaltab);		

		//for(i=0;i<64;i++)
		{
			//dataptr[i]=dataptr2[i];
		}
	}

	//mem_free(dataptr2);
	//mem_free(qvaltab2);
	//mem_free(invqvaltab2);
		
	return;
}
#endif

#endif

#endif //IPP_SUPPORT
