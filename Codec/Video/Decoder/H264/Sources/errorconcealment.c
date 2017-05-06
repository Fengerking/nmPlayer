#include "global.h"
#if FEATURE_MATRIXQUANT
// exported functions
// quantization initialization
extern const avdUInt8 dequant_coef8[384];
extern const avdUInt8 dequant_coef[96];
int quant_intra_default[16] = {
	6,13,20,28,
	13,20,28,32,
	20,28,32,37,
	28,32,37,42
};

int quant_inter_default[16] = {
	10,14,20,24,
	14,20,24,27,
	20,24,27,30,
	24,27,30,34
};

int quant8_intra_default[64] = {
	6,10,13,16,18,23,25,27,
	10,11,16,18,23,25,27,29,
	13,16,18,23,25,27,29,31,
	16,18,23,25,27,29,31,33,
	18,23,25,27,29,31,33,36,
	23,25,27,29,31,33,36,38,
	25,27,29,31,33,36,38,40,
	27,29,31,33,36,38,40,42
};

int quant8_inter_default[64] = {
	9,13,15,17,19,21,22,24,
	13,13,17,19,21,22,24,25,
	15,17,19,21,22,24,25,27,
	17,19,21,22,24,25,27,28,
	19,21,22,24,25,27,28,30,
	21,22,24,25,27,28,30,32,
	22,24,25,27,28,30,32,33,
	24,25,27,28,30,32,33,35
};

int quant_org[16] = { //to be use if no q matrix is chosen
	16,16,16,16,
	16,16,16,16,
	16,16,16,16,
	16,16,16,16
};

int quant8_org[64] = { //to be use if no q matrix is chosen
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16
};



void assign_quant_params(ImageParameters *img)
{
	//initMatrixDequantDone
	seq_parameter_set_rbsp_t* sps = img->active_sps;
	pic_parameter_set_rbsp_t* pps = img->active_pps;
	int disableMatrix = !pps->pic_scaling_matrix_present_flag && !sps->seq_scaling_matrix_present_flag;
	int i,j,k,m,n,temp;
	int n_ScalingList;
	avdUInt16	*tmp16;
	//TMBBitStream	*mbBits = GetMBBits(currMB);
	
	if(!disableMatrix&&!img->initMatrixDequantDone)
	{
		TDequantArray_coef* InvLevelScale4x4_Intra;
		TDequantArray_coef* InvLevelScale4x4_Inter;
		TDequant8Array_coef* InvLevelScale8x8_Intra;
		TDequant8Array_coef* InvLevelScale8x8_Inter;
		TDequantArray*		dequantcoef;
		TDequant8Array*		dequantcoef8;
		int  *qmatrix[12];
		int shift;
		//if (img->ioInfo->inNumber>1000)//20111231: wierd workaround
		//	AVD_ERROR_CHECK2(img, "assign quant params in middle",ERROR_InvalidBitstream);
		//we need to realloc the dequan_coef_shift
		SafevoH264AlignedFree(img,img->dequant_coef_shift);
#if FEATURE_T8x8
		SafevoH264AlignedFree(img,img->dequant_coef_shift8);
#endif//FEATURE_T8x8
		img->dequant_coef_shift = tmp16 = (avdUInt16 *)voH264AlignedMalloc(img,1501, 96*6*sizeof(avdUInt16));CHECK_ERR_RET_VOID
			/*	i = 0;
			do {
			j = 0;
			do {
			*tmp16++ = (dequant_coef[j]<<i);
			} while (++j < 96);
			} while (++i < 9);*/
#if FEATURE_T8x8
			img->dequant_coef_shift8 = tmp16 = (avdUInt16 *)voH264AlignedMalloc(img,1502, 384*6*sizeof(avdUInt16));CHECK_ERR_RET_VOID
			/*i = 0;
			do {
			j = 0;
			do {
			*tmp16++ = (dequant_coef8[j]<<i);
			} while (++j < 384);
			} while (++i < 9);*/
#endif//FEATURE_T8x8
			n_ScalingList = 8;

		if(sps->seq_scaling_matrix_present_flag) // check sps first
		{
			for(i=0; i<n_ScalingList; i++)
			{
				if(i<6)
				{
					if(!sps->seq_scaling_list_present_flag[i]) // fall-back rule A
					{
						if(i==0)
							qmatrix[i] = quant_intra_default;
						else if(i==3)
							qmatrix[i] = quant_inter_default;
						else
							qmatrix[i] = qmatrix[i-1];
					}
					else
					{
						if(sps->UseDefaultScalingMatrix4x4Flag[i])
							qmatrix[i] = (i<3) ? quant_intra_default : quant_inter_default;
						else
							qmatrix[i] = sps->ScalingList4x4[i];
					}
				}
				else
				{
					if(!sps->seq_scaling_list_present_flag[i]) // fall-back rule A
					{
						if(i==6)
							qmatrix[i] = quant8_intra_default;
						else if(i==7)
							qmatrix[i] = quant8_inter_default;
						else
							qmatrix[i] = qmatrix[i-2];
					}
					else
					{
						if(sps->UseDefaultScalingMatrix8x8Flag[i-6])
							qmatrix[i] = (i==6 || i==8 || i==10) ? quant8_intra_default:quant8_inter_default;
						else
							qmatrix[i] = sps->ScalingList8x8[i-6];
					}
				}
			}
		}

		if(pps->pic_scaling_matrix_present_flag) // then check pps
		{
			for(i=0; i<n_ScalingList; i++)
			{
				if(i<6)
				{
					if(!pps->pic_scaling_list_present_flag[i]) // fall-back rule B
					{
						if (i==0)
						{
							if(!sps->seq_scaling_matrix_present_flag)
								qmatrix[i] = quant_intra_default;
						}
						else if (i==3)
						{
							if(!sps->seq_scaling_matrix_present_flag)
								qmatrix[i] = quant_inter_default;
						}
						else
							qmatrix[i] = qmatrix[i-1];
					}
					else
					{
						if(pps->UseDefaultScalingMatrix4x4Flag[i])
							qmatrix[i] = (i<3) ? quant_intra_default:quant_inter_default;
						else
							qmatrix[i] = pps->ScalingList4x4[i];
					}
				}
				else
				{
					if(!pps->pic_scaling_list_present_flag[i]) // fall-back rule B
					{
						if (i==6)
						{
							if(!sps->seq_scaling_matrix_present_flag)
								qmatrix[i] = quant8_intra_default;
						}
						else if(i==7)
						{
							if(!sps->seq_scaling_matrix_present_flag)
								qmatrix[i] = quant8_inter_default;
						}
						else  
							qmatrix[i] = qmatrix[i-2];
					}
					else
					{
						if(pps->UseDefaultScalingMatrix8x8Flag[i-6])
							qmatrix[i] = (i==6 || i==8 || i==10) ? quant8_intra_default:quant8_inter_default;
						else
							qmatrix[i] = pps->ScalingList8x8[i-6];
					}
				}
			}
		}
		img->initMatrixDequantDone = 1;
		//CalculateQuant4x4Param(currSlice);
		dequantcoef = dequant_coef;
		InvLevelScale4x4_Intra = img->dequant_coef_shift;
		InvLevelScale4x4_Inter = (avdUInt8*)img->dequant_coef_shift+ sizeof(TDequantArray_coef);
		//shift = 4;
		for(k=0; k<6; k++)
		{
			for(i=0; i<4; i++)
			{
				for(j=0; j<4; j++)
				{
					int deQ = (*dequantcoef)[k][i][j];
					temp = (i<<2)+j;
					(*InvLevelScale4x4_Intra)[0][k][i][j] = deQ * qmatrix[0][temp];
					(*InvLevelScale4x4_Intra)[1][k][i][j] = deQ * qmatrix[1][temp];
					(*InvLevelScale4x4_Intra)[2][k][i][j] = deQ * qmatrix[2][temp];

					(*InvLevelScale4x4_Inter)[0][k][i][j] = deQ * qmatrix[3][temp];
					(*InvLevelScale4x4_Inter)[1][k][i][j] = deQ * qmatrix[4][temp];
					(*InvLevelScale4x4_Inter)[2][k][i][j] = deQ * qmatrix[5][temp];
				}
			}
		}
		if(img->Transform8x8Mode)
		{
			//CalculateQuant8x8Param(currSlice);
			dequantcoef8 = dequant_coef8;
			InvLevelScale8x8_Intra = img->dequant_coef_shift8;
			InvLevelScale8x8_Inter = (avdUInt8*)img->dequant_coef_shift8+ sizeof(TDequant8Array_coef);
			for(k=0; k<6; k++)
			{
				for(i=0; i<8; i++)
				{
					for(j=0; j<8; j++)
					{
						int deQ = (*dequantcoef8)[k][i][j];
						temp = (i<<3)+j;
						(*InvLevelScale8x8_Intra)[0][k][i][j] = deQ * qmatrix[6][temp];
						(*InvLevelScale8x8_Inter)[0][k][i][j] = deQ * qmatrix[7][temp];
					}
				}
			}
		}
	}


}


static const char ZZ_SCAN[16]  =
{  0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

static const char ZZ_SCAN8[64] =
{  0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};
void Scaling_List(int *scalingList, int sizeOfScalingList, avdInt8 *UseDefaultScalingMatrix, Bitstream *s)
{
	int j, scanj;
	int delta_scale, lastScale, nextScale;

	lastScale      = 8;
	nextScale      = 8;

	for(j=0; j<sizeOfScalingList; j++)
	{
		scanj = (sizeOfScalingList==16) ? ZZ_SCAN[j]:ZZ_SCAN8[j];

		if(nextScale!=0)
		{
			delta_scale = avd_se_v(s);
			nextScale = (lastScale + delta_scale + 256) &255;
			*UseDefaultScalingMatrix = (scanj==0 && nextScale==0);
		}

		scalingList[scanj] = (nextScale==0) ? lastScale:nextScale;
		lastScale = scalingList[scanj];
	}
}
#endif//FEATURE_MATRIXQUANT
