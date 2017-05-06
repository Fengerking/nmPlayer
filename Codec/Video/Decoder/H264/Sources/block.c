
/*!
***********************************************************************
*  \file
*      block.c
*
*  \brief
*      Block functions
*
*  \author
*      Main contributors (see contributors.h for copyright, address and affiliation details)
*      - Inge Lille-Langøy          <inge.lille-langoy@telenor.com>
*      - Rickard Sjoberg            <rickard.sjoberg@era.ericsson.se>
***********************************************************************
*/

#include "contributors.h"

#include "global.h"

#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#endif //!BUILD_WITHOUT_C_LIB

#include "block.h"
#include "image.h"
//#include "mb_access.h"
#include "mbuffer.h"

//#include "H264_C_Type.h"
#include "avd_neighbor.h"



#define Q_BITS          15

const avdUInt8 avdNumLeadingZerosPlus1[256] = {
	9,8,7,7,6,6,6,6,5,5,
	5,5,5,5,5,5,4,4,4,4, //10-19
	4,4,4,4,4,4,4,4,4,4,
	4,4,3,3,3,3,3,3,3,3, //30-39 
	3,3,3,3,3,3,3,3,3,3,  
	3,3,3,3,3,3,3,3,3,3,  
	3,3,3,3,2,2,2,2,2,2, //60-69; 
	2,2,2,2,2,2,2,2,2,2,  
	2,2,2,2,2,2,2,2,2,2,  
	2,2,2,2,2,2,2,2,2,2, 
	2,2,2,2,2,2,2,2,2,2, 
	2,2,2,2,2,2,2,2,2,2, 
	2,2,2,2,2,2,2,2,1,1, //120-127
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1}; // 250-255





#if (DUMP_VERSION & DUMP_SLICE)
	void DumpIntraPred(avdUInt8* src,int stride,int width,int height)
	{
		//Dumploop2(DUMP_SLICE,"IntraPred",height,width,src,DDT_D8,stride);
	}
#else//(DUMP_VERSION & DUMP_SLICE)
#define DumpIntraPred(a,b,c,d)
#endif//(DUMP_VERSION & DUMP_SLICE)
	// Notation for comments regarding prediction and predictors.
	// The pels of the 4x4 block are labelled a..p. The predictor pels above
	// are labelled A..H, from the left I..L, and from above left X, as follows:
	//
	//  X A B C D E F G H
	//  I a b c d
	//  J e f g h
	//  K i j k l
	//  L m n o p
	//
	
	/*!
	***********************************************************************
	* \brief
	*    makes and returns 4x4 blocks with all 5 intra prediction modes
	*
	* \return
	*    DECODING_OK   decoding of intraprediction mode was sucessfull            \n
	*    SEARCH_SYNC   search next sync element as errors while decoding occured
	***********************************************************************
	*/
#if FEATURE_INTERLACE
typedef struct{
	int isTop;
	int isLastRow;//1:lastrow,0: last but one row
}TNeighborCoefInfo;

static const int NeighborRelativeOffset[]={
0,//MBADDR_A_TOP	, 
1,//MBADDR_A_BOTTOM	, 
-2,//MBADDR_B_TOP	, 
-1,//MBADDR_B_BOTTOM	, 
-2,//MBADDR_C_TOP	, 
-1,//MBADDR_C_BOTTOM	, 
-2,//MBADDR_D_TOP	, 
-1,//MBADDR_D_BOTTOM	, 
0,//MBADDR_Curr_TOP	, 
1//MBADDR_Curr_BOTTOM	, 
};
MacroBlock *GetMBAffNeighbor2(ImageParameters *img,TMBsProcessor *info,avdNativeInt nbIdx, avdNativeInt yOff, avdNativeInt *mbY,
	avdNativeUInt maxH)
{
	MacroBlock		*mbInfo;

	mbInfo = info->neighborABCD2[nbIdx];
	if(mbInfo)
	{
		*mbY = (GetYPosition(info->neighborABCDPos[nbIdx].yPosIdx,yOff,maxH)+maxH)&(maxH-1);//GetYPosition(info->neighborABCDPos[nbIdx].yPosIdx,yOff,maxH);
		//int relativeOffset = NeighborRelativeOffset[info->neighborABCDPos[nbIdx].mbAddr];
		//mbY->isTop = IsTopMBAffNeighbor(info,nbIdx);//(info->neighborABCDPos[nbIdx].mbAddr&1)==0;//relativeOffset==0||relativeOffset==-2;
		//mbY->isLastRow = yW==-1;
	}
	return mbInfo;
}
static MacroBlock *GetMBAffNeighbor2AnYPos(ImageParameters *img,TMBsProcessor *info,avdNativeInt yOff, 
														  avdNativeInt *nAY, avdNativeInt number,
		avdNativeUInt maxH)
{
	TMBAddrNYPos	addrNYPos = info->getNeighborAFunc(info,info->neighborA,yOff,maxH);
	int yStart = (GetYPosition(addrNYPos.yPosIdx,yOff,maxH)+maxH)&(maxH-1);
	MacroBlock		*mbInfo = GetNeighborMB(img,info,addrNYPos.mbAddr);

	if(info->getNeighborAFunc == GetNeighborAFrame2Field)
	{
		int i;
		avdNativeInt *nAY2 = nAY + 1;//even row
		nAY[0] = yStart;
		nAY2[0] = yStart + maxH;
		number >>= 1;
		for (i=1;i<number;i++)
		{
			nAY[2] = nAY[0] + 1;
			nAY2[2] = nAY2[0] + 1;
			nAY+=2;nAY2+=2;
		}
	}
	else//Field2Frame
	{
		int i;
		avdNativeInt *nAY2 = nAY + 1;//even row
		nAY[0]  = yStart;
		nAY2[0] = yStart + 2;
		number >>= 1;
		for (i=1;i<number;i++)
		{
			nAY[2] = nAY[0] + 4;
			nAY2[2] = nAY2[0] + 4;
			nAY+=2;nAY2+=2;
		}
	}
	return mbInfo;
}
#endif //FEATURE_INTERLACE
	avdNativeInt intrapred(ImageParameters *img,TMBsProcessor *info,
		avdNativeInt img_x,      //!< location of block X
		avdNativeInt img_y)      //!< location of block Y
	{
		StorablePicture	*dec_picture = img->dec_picture;
		avdUInt8 *Y4Rows0, *Y4Rows1, *Y4Rows2, *Y4Rows3, *tmpY;
		MacroBlock   *currMBInfo = info->currMB;
		MacroBlock	*mbAInfo, *mbBInfo, *mbCInfo, *mbDInfo;
		avdUInt8 *plnY;
		avdUInt32 ui32;
		
#if FEATURE_INTERLACE
		TNeighborCoefInfo	addrNPos;
#endif
		avdNativeInt nBX, nBY; 
		avdUInt32 t1,t2,t3,t4;
		avdNativeInt i, ioff, joff, y4off,predmode;
		avdInt32 s0, yPitch;
		TSizeInfo	*sizeInfo = img->sizeInfo;
		TPosInfo	*posInfo  = GetPosInfo(info);
		TMBBitStream	*mbBits = GetMBBits(info->currMB);
		avdNativeInt P_X, P_A, P_B, P_C, P_D, P_E, P_F, P_G, P_H, P_I, P_J, P_K, P_L;
		AVDIPDeblocker *dbkCur;
		avdUInt8*		pLeftY	= NULL;
		avdUInt8*		pTopY	= NULL;
		avdUInt8* YLeftMBAFF;
		AVDIPDeblocker *dbk = GetIPDeblocker(info->anotherProcessor);
		avdNativeInt nAY[4];
		int start;
		TIME_BEGIN(start)
		if(dbk==NULL)
		{
			AVD_ERROR_CHECK(img,"intrapred:dbk==NULL",ERROR_NULLPOINT);
			;
		}
		if(img->deblockType==DEBLOCK_BY_MB)
		{
			dbkCur  = GetIPDeblocker(info);
		}
		P_X=P_A=P_B= P_C= P_D= P_E= P_F= P_G= P_H= P_I= P_J= P_K= P_L = 128;



		plnY = dec_picture->plnY; 
		yPitch = sizeInfo->yPlnPitch;
		Y4Rows0 = plnY + img_y * yPitch + img_x; 


		if(plnY==NULL)
		{
			AVD_ERROR_CHECK(img,"intrapred:plnY==NULL",ERROR_NULLPOINT);;
		}
		Y4Rows1 = Y4Rows0 + yPitch;
		Y4Rows2 = Y4Rows1 + yPitch;
		Y4Rows3 = Y4Rows2 + yPitch;

		y4off = img_y&0xf;
		ioff = (img_x&0xf)>>2;
		joff = (y4off)>>2;

		//AvdLog(DUMP_SLICEDATA,DUMP_DCORE"MB(%d,%d),sub(%d,%d)\n",info->mb_x,info->mb_y,ioff,joff);
		//info->mb_x==0&&info->mb_y==1&&ioff==2&&joff==0
		predmode = mbBits->uMBS.mbIntra4x4Mode[(y4off)|ioff];
		if(img->deblockType==DEBLOCK_BY_MB&&ioff==0)
		{
			if(!GetMbAffFrameFlag(img))
				pLeftY = GetLeftMBY(dbkCur) + joff*4;
#if FEATURE_INTERLACE
			else//default, the left MB has same type(frame/field)
				pLeftY = GetMBAffLeftY(dbkCur,CurrMBisTop(info)) + joff*4;
#endif
		}
		if(joff==0)
		{
#if FEATURE_INTERLACE

			if(GetMbAffFrameFlag(img))
			{
				mbBInfo = GetMBAffNeighbor2(img,
				info,
				NEIGHBOR_B, 
				-1, 
				&nBY,
				16);
				if (mbBInfo)
				{
					int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_B);
					int isLastRow = nBY==15;
					if(IsNeighborBIsSelf(info))
					{
						pTopY = GetMBAffTopY(dbkCur,isTop,isLastRow);
					}
					else
					{
						pTopY = GetMBAffTopY(dbk,isTop,isLastRow);
					}
					
				}
				if(pTopY)
					pTopY +=  img_x;
			}
			else
#endif//FEATURE_INTERLACE
				pTopY = dbk->topY + img_x;

		}
		if (predmode != VERT_PRED && predmode != DIAG_DOWN_LEFT_PRED && 
			predmode != VERT_LEFT_PRED){
#if FEATURE_INTERLACE
				if(GetMbAffFrameFlag(img)&&ioff==0&&IsNeedToCalcualteNeighborA_MBAFF(info))
				{
					
					mbAInfo = GetMBAffNeighbor2AnYPos(img,info,y4off, 
						nAY,4,16);
					if(mbAInfo)	
					{
						//YLeftMBAFF = Y4Rows0 - 1 - y4off;
						int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
						pLeftY = GetMBAffLeftY(dbkCur,isTop);
						//Dumploop(DUMP_SLICE,info,"4x4DC_PRED_LEFT",4,pLeftY,DDT_D8);
						P_I = pLeftY[nAY[0]];
						P_J = pLeftY[nAY[1]];
						P_K = pLeftY[nAY[2]];
						P_L = pLeftY[nAY[3]];
					}
					
				}
				else			
#endif//FEATURE_INTERLACE
				{
					int leftMBYForIntra;
					mbAInfo = (ioff > 0) ? currMBInfo :GetLeftMB(info);
					if (mbAInfo)
					{

						if(pLeftY)
						{
							//Dumploop(DUMP_SLICE,info,"4x4DC_PRED_LEFT",4,pLeftY,DDT_D8);
							P_I = pLeftY[0];
							P_J = pLeftY[1];
							P_K = pLeftY[2];
							P_L = pLeftY[3];
						}
						else
						{
							P_I = Y4Rows0[-1];
							P_J = Y4Rows1[-1];
							P_K = Y4Rows2[-1];
							P_L = Y4Rows3[-1];
						}

					}
				}//end if(GetMbAffFrameFlag(img))			

		}

		if (predmode != HOR_PRED){

			nBY = img_y - 1;
			nBX = img_x; //
			mbBInfo = (joff > 0) ? currMBInfo :GetUpMB(info);

			
			if (PRED_MODE_NOT_VERT_OR_HORUP(predmode)){

				if (mbBInfo)
				{

					if (joff){
						tmpY = Y4Rows0 - yPitch;
						P_A = tmpY[0];
						P_B = tmpY[1];
						P_C = tmpY[2];
						P_D = tmpY[3];

					}
					else{//TBD_MBAFF:bottom?
						P_A = pTopY[0];
						P_B = pTopY[1];
						P_C = pTopY[2];
						P_D = pTopY[3];
						//Dumploop(DUMP_SLICE,info,"4x4DC_PRED_UP",4,&pTopY[img_x],DDT_D8);
					}

				}

				if (predmode == VERT_LEFT_PRED ||
					predmode == DIAG_DOWN_LEFT_PRED){
						
						avdUInt8* pTopY_C = NULL;
						P_E = P_F = P_G = P_H = P_D;//default
						if (ioff < 3)
							mbCInfo = (ioff != 1 || !(joff&1)) ? mbBInfo : NULL;
						else if(!GetMbAffFrameFlag(img))
							mbCInfo = !joff ? GetUpRightMB(info) : NULL;
#if FEATURE_INTERLACE
						else
						{
							int nCY;
							mbCInfo = !joff ? GetMBAffNeighbor2(img,
								info,
								NEIGHBOR_C, 
								-1, 
								&nCY,
								16)
								:NULL;
							if (mbCInfo)
							{
								int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_C);
								int isLastRow = nCY == 15;
								pTopY_C = GetMBAffTopY(dbk,isTop,isLastRow) + posInfo->pix_x + 16;
								P_E = pTopY_C[0];
								P_F = pTopY_C[1];
								P_G = pTopY_C[2];
								P_H = pTopY_C[3];
							}
						}
#endif//FEATURE_INTERLACE
						if (mbCInfo&&pTopY_C==NULL)
						{

							if (joff){
								tmpY = Y4Rows0 - yPitch + 4;
								P_E = tmpY[0];
								P_F = tmpY[1];
								P_G = tmpY[2];
								P_H = tmpY[3];

							}
							else{
								P_E = pTopY[4];
								P_F = pTopY[5];
								P_G = pTopY[6];
								P_H = pTopY[7];
							}

						}
					
							
				}

				if (PRED_MODE_IS_DIAGDOWNRIGHT_OR_VERTRIGHT_OR_HOR_DOWN(predmode)){
					if(!GetMbAffFrameFlag(img))
					{
						mbDInfo = (ioff > 0) ? mbBInfo : (joff > 0 ? mbAInfo :
							GetUpLeftMB(info));
						if (mbDInfo){
							avdNativeInt  yDPos = 1;
							//YLeftMBAFF = Y4Rows0 - 1 - y4off;
							if (joff){
								if(pLeftY)
									P_X = pLeftY[-1*yDPos];
								else
									P_X = Y4Rows0[(-yPitch -1) *yDPos];

							}
							else 
							{
								if(img->deblockType==DEBLOCK_BY_MB||ioff)
								{
									P_X = pTopY[- 1];
								}
								else
								{				
									P_X = dbk->yuvD[0];
								}
							}	
						}
					}
#if FEATURE_INTERLACE
					else
					{
						mbDInfo = (ioff > 0) ? mbBInfo : (joff > 0 ? 
							GetLeftMBAffMB2(img,info,y4off-1,16,&nBY)
							:GetMBAffNeighbor2(img,
							info,
							NEIGHBOR_D, 
							y4off-1, 
							&nBY,
							16)
							)
							;
						if (mbDInfo)
						{

							if (joff){
								
								if(ioff==0)
								{
									int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
									avdUInt8* pLeftY2 = GetMBAffLeftY(dbkCur,isTop);
									P_X	= pLeftY2[nBY];
									
								}
								else
									P_X = Y4Rows0[(-yPitch -1)];

							}
							else 
							{
								if(ioff)
								{
									P_X = pTopY[-1];
								}
								else
								{
									int isNeighborA = IsNeighborDIsA(info);
									int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_D);
									if(!isNeighborA)
									{
										int isLastRow = nBY==15;
										avdUInt8* pTopY_D =  GetMBAffTopY(dbk,isTop,isLastRow)+img_x;
										P_X = pTopY_D[-1];
										
									}
									else
									{
										int yW = (nBY + 16)&15;
										P_X = *(GetMBAffLeftY(dbkCur,isTop) + yW);
									
									}
								}
								
								
							}	
							
						}
					}
#endif//FEATURE_INTERLACE
			   }
			}
		}
		//AvdLog(DUMP_SLICEDATA,DCORE_DUMP,"Intra4x4(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)\n",	P_X,P_A,P_B,P_C,P_D,P_E,P_F,P_G,P_H,P_I,P_J,P_K,P_L);
		switch (predmode)
		{
		case VERT_PRED:                       /* vertical prediction from block above */

		
			if (!joff)
			{
				if(pTopY==NULL)
					AVD_ERROR_CHECK2(img,"VERT_PRED:pTopY==NULL\n",ERROR_InvalidBitstream);
				s0 = *(avdInt32 *)pTopY; 
			}
			else
				s0 = *(avdInt32 *)(plnY + nBY * yPitch + nBX); 

			*(avdInt32 *)Y4Rows0 = *(avdInt32 *)Y4Rows1 
				= *(avdInt32 *)Y4Rows2 = *(avdInt32 *)Y4Rows3 = s0;

			break;
		case HOR_PRED:                        /* horizontal prediction from left block */

#define PACK_4(val)	\
	t1 = (val) | ((val)<<8); \
	t1 |= t1<<16;
			PACK_4(P_I); 
			*(avdUInt32 *)Y4Rows0 =  t1;
			PACK_4(P_J);
			*(avdUInt32 *)Y4Rows1 =  t1;
			PACK_4(P_K);
			*(avdUInt32 *)Y4Rows2 =  t1;
			PACK_4(P_L);
			*(avdUInt32 *)Y4Rows3 =  t1;

			break;
		case DC_PRED:                         /* DC prediction */
			if (mbBInfo){
				s0 = mbAInfo ?
					(P_A + P_B + P_C + P_D + P_I + P_J + P_K + P_L + 4)>>3
					: (P_A + P_B + P_C + P_D + 2)>>2;
			}
			else{
				s0 = mbAInfo ? (P_I + P_J + P_K + P_L + 2)>>2
					: 128;
			}

			s0 |= (s0<<8);
			*(avdUInt32 *)Y4Rows0 = *(avdUInt32 *)Y4Rows1
				= *(avdUInt32 *)Y4Rows2 = *(avdUInt32 *)Y4Rows3
				= (s0 | (s0<<16));

			break;
		case DIAG_DOWN_LEFT_PRED:

			ui32 = (((P_D + P_F + (P_E<<1) + 2)>>2) | 
				(((P_E + P_G + (P_F<<1) + 2)>>2)<<8) | 
				(((P_F + P_H + (P_G<<1) + 2)>>2)<<16) | 
				(((P_G + P_H + (P_H<<1) + 2)>>2)<<24));
			*(avdUInt32 *)Y4Rows3 = ui32;
			ui32 = (((P_C + P_E + (P_D<<1) + 2)>>2) | (ui32<<8));
			*(avdUInt32 *)Y4Rows2 = ui32;
			ui32 = (((P_B + P_D + (P_C<<1) + 2)>>2) | (ui32<<8));
			*(avdUInt32 *)Y4Rows1 = ui32;
			*(avdUInt32 *)Y4Rows0 = (((P_A + P_C + (P_B<<1) + 2)>>2) | (ui32<<8));
			break;
		case DIAG_DOWN_RIGHT_PRED:
			ui32 = (((P_I + (P_X<<1) + P_A + 2)>>2) | 
				(((P_X + (P_A<<1) + P_B + 2)>>2)<<8) | 
				(((P_A + (P_B<<1) + P_C + 2)>>2)<<16) | 
				(((P_B + (P_C<<1) + P_D + 2)>>2)<<24));
			*(avdUInt32 *)Y4Rows0 = ui32;
			ui32 = (((P_J + (P_I<<1) + P_X + 2)>>2) | (ui32<<8));
			*(avdUInt32 *)Y4Rows1 = ui32;
			ui32 = (((P_K + (P_J<<1) + P_I + 2)>>2) | (ui32<<8));
			*(avdUInt32 *)Y4Rows2 = ui32;
			*(avdUInt32 *)Y4Rows3 = (((P_L + (P_K<<1) + P_J + 2)>>2) | (ui32<<8));
			break;
		case  VERT_RIGHT_PRED:/* diagonal prediction -22.5 deg to horizontal plane */

			ui32 = (((P_X + P_A + 1)>>1) | 
				(((P_A + P_B + 1)>>1)<<8) | 
				(((P_B + P_C + 1)>>1)<<16) | 
				(((P_C + P_D + 1)>>1)<<24));
			*(avdUInt32 *)Y4Rows0 = ui32;
			ui32 = (((P_X + (P_I<<1) + P_J + 2)>>2) | (ui32<<8));
			*(avdUInt32 *)Y4Rows2 = ui32;
			ui32 = (((P_I + (P_X<<1) + P_A + 2)>>2) | 
				(((P_X + (P_A<<1) + P_B + 2)>>2)<<8) | 
				(((P_A + (P_B<<1) + P_C + 2)>>2)<<16) | 
				(((P_B + (P_C<<1) + P_D + 2)>>2)<<24));
			*(avdUInt32 *)Y4Rows1 = ui32;
			*(avdUInt32 *)Y4Rows3 = (((P_I + (P_J<<1) + P_K + 2)>>2) | (ui32<<8));
			break;
		case  HOR_DOWN_PRED:/* diagonal prediction -22.5 deg to horizontal plane */

			ui32 = (((P_X + P_I + 1)>>1) | 
				(((P_I + (P_X<<1) + P_A + 2)>>2)<<8) | 
				(((P_X + (P_A<<1) + P_B + 2)>>2)<<16) | 
				(((P_A + (P_B<<1) + P_C + 2)>>2)<<24));
			*(avdUInt32 *)Y4Rows0 = ui32;
			ui32 = (((P_I + P_J + 1)>>1) | 
				(((P_X + (P_I<<1) + P_J + 2)>>2)<<8) | (ui32<<16));
			*(avdUInt32 *)Y4Rows1 = ui32;
			ui32 = (((P_J + P_K + 1)>>1) | 
				(((P_I + (P_J<<1) + P_K + 2)>>2)<<8) | (ui32<<16));
			*(avdUInt32 *)Y4Rows2 = ui32;
			*(avdUInt32 *)Y4Rows3 = (((P_K + P_L + 1)>>1) | 
				(((P_J + (P_K<<1) + P_L + 2)>>2)<<8) | (ui32<<16));
			break;
		case  VERT_LEFT_PRED:/* diagonal prediction -22.5 deg to horizontal plane */

			ui32 = (((P_B + P_C + 1)>>1) |
				(((P_C + P_D + 1)>>1)<<8) |
				(((P_D + P_E + 1)>>1)<<16) |
				(((P_E + P_F + 1)>>1)<<24));
			*(avdUInt32 *)Y4Rows2 = ui32;
			*(avdUInt32 *)Y4Rows0 = (((P_A + P_B + 1)>>1) | (ui32<<8));
			ui32 = (((P_B + (P_C<<1) + P_D + 2)>>2) |
				(((P_C + (P_D<<1) + P_E + 2)>>2)<<8) |
				(((P_D + (P_E<<1) + P_F + 2)>>2)<<16) |
				(((P_E + (P_F<<1) + P_G + 2)>>2)<<24));
			*(avdUInt32 *)Y4Rows3 = ui32;
			*(avdUInt32 *)Y4Rows1 = (((P_A + (P_B<<1) + P_C + 2)>>2) | (ui32<<8));
			break;
		case  HOR_UP_PRED:/* diagonal prediction -22.5 deg to horizontal plane */

			if (!mbAInfo)
			{

				AVD_ERROR_CHECK(img,"intrapred:HOR_UP_PRED mbAInfo is NULL",ERROR_IntraMBAIsNULL);	
			}
			ui32 = (P_L | (P_L<<8) | (P_L<<16) | (P_L<<24));
			*(avdUInt32 *)Y4Rows3 = ui32;
			ui32 = (((P_K + P_L + 1)>>1) | 
				(((P_K + (P_L<<1) + P_L + 2)>>2)<<8) | (ui32<<16));
			*(avdUInt32 *)Y4Rows2 = ui32;
			ui32 = (((P_J + P_K + 1)>>1) | 
				(((P_J + (P_K<<1) + P_L + 2)>>2)<<8) | (ui32<<16));
			*(avdUInt32 *)Y4Rows1 = ui32;
			*(avdUInt32 *)Y4Rows0 = (((P_I + P_J + 1)>>1) | 
				(((P_I + (P_J<<1) + P_K + 2)>>2)<<8) | (ui32<<16));
			break;
		default:

			AVD_ERROR_CHECK(img,"intrapred:there is not predict mode",ERROR_IntraModeIsNULL);	
			return SEARCH_SYNC;
		}
		TIME_END(start,intra4x4Sum)
		DumpIntraPred(plnY + img_y * yPitch + img_x,yPitch,4,4);
		return DECODING_OK;
	}



	/*!
	***********************************************************************
	* \return
	*    best SAD
	***********************************************************************
	*/
	avdNativeInt intrapred_luma_16x16(ImageParameters *img,TMBsProcessor *info,avdNativeInt predmode)        //!< prediction mode
	{

		////TMBsProcessor *info  = img->mbsProcessor;
		StorablePicture	*dec_picture = img->dec_picture;
		avdUInt8 *plnY, *srcY, *src, *src2;
		avdUInt32 *m1, *m2, tmp32, a, b, c, d;
		MacroBlock *mbAInfo, *mbBInfo, *mbDInfo;
		avdNativeInt s0, s1, s2, ih, iv, ib, ic, iaa,nBY;
		avdNativeInt i, j, yPitch;
		avdUInt8 *clip255,*pTopY;
		TSizeInfo	*sizeInfo = img->sizeInfo;
		TCLIPInfo	*clipInfo = img->clipInfo;
		TPosInfo*  posInfo  = GetPosInfo(info);
		

		AVDIPDeblocker *dbk  = GetIPDeblocker(info->anotherProcessor);
		AVDIPDeblocker *dbkCur;

		int leftMBYForIntra;
		int nAY[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};//default
		int start;
		TIME_BEGIN(start)
		if(img->deblockType==DEBLOCK_BY_MB)
			dbkCur  = GetIPDeblocker(info);

		mbAInfo = GetLeftMB(info);//GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,0,16,&leftMBYForIntra):
		mbBInfo = GetUpMB(info);
		mbDInfo = GetUpLeftMB(info);
		//nBY = yIdx - 1;
		//nBX = xIdx;
#if FEATURE_INTERLACE
		if(GetMbAffFrameFlag(img)&&mbAInfo==NULL&&IsNeedToCalcualteNeighborA_MBAFF(info))
		{
			mbAInfo = GetMBAffNeighbor2AnYPos(img,info,0, 
				nAY,16,16);
		}
#endif//FEATURE_INTERLACE
		plnY = dec_picture->plnY; 
		yPitch = sizeInfo->yPlnPitch;
		srcY = plnY + posInfo->pix_y * yPitch + posInfo->pix_x;	
		i = (yPitch>>2);
		m2 = (avdUInt32 *)srcY;
#if FEATURE_INTERLACE
		if(GetMbAffFrameFlag(img))
		{
			mbBInfo = GetMBAffNeighbor2(img,
				info,
				NEIGHBOR_B, 
				-1, 
				&nBY,
				16);
			if (mbBInfo)
			{
				int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_B);
				int isLastRow = nBY==15;
				pTopY  = IsNeighborBIsSelf(info)? GetMBAffTopY(dbkCur,isTop,isLastRow):GetMBAffTopY(dbk,isTop,isLastRow);
				pTopY += posInfo->pix_x;
			}
			
		}
		else
#endif//FEATURE_INTERLACE
			pTopY = dbk->topY + posInfo->pix_x;
		switch (predmode)
		{
		case VERT_PRED_16:                       // vertical prediction from block above
			if (!mbBInfo)
			{
				AVD_ERROR_CHECK(img,"intrapred_luma_16x16: VERT_PRED_16,mbBInfo is NULL ",ERROR_IntraMBBIsNULL);
			}

#if FEATURE_INTERLACE	
			if(GetMbAffFrameFlag(img))
			{

				int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_B);
				m1 = (avdUInt32 *)(pTopY);
			}
			else
#endif//#if FEATURE_INTERLACE	
				m1 = (avdUInt32 *)pTopY;

			a = *(m1    );
			b = *(m1 + 1);
			c = *(m1 + 2);
			d = *(m1 + 3);
			j = 8;
			do {
				*(m2    ) = a;
				*(m2 + 1) = b;
				*(m2 + 2) = c;
				*(m2 + 3) = d;
				m2 += i;
				*(m2    ) = a;
				*(m2 + 1) = b;
				*(m2 + 2) = c;
				*(m2 + 3) = d;
				m2 += i;
			} while (--j);
			break;
		case HOR_PRED_16:                        // horizontal prediction from left block
			if (!mbAInfo)
			{
				AVD_ERROR_CHECK(img,"intrapred_luma_16x16: HOR_PRED_16,mbAInfo is NULL",ERROR_IntraMBAIsNULL);;
			}
			if(img->deblockType==DEBLOCK_BY_MB)
			{
				int offset = 0;
#if FEATURE_INTERLACE
				if(GetMbAffFrameFlag(img))
				{
					int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
					srcY = GetMBAffLeftY(dbkCur,isTop);
				}
				else
#endif//FEATURE_INTERLACE
					srcY=GetLeftMBY(dbkCur);
				j = 8;
				do {

					a = srcY[nAY[offset]];

					a |= (a<<8);
					a |= (a<<16);
					*m2 = *(m2 + 1) = *(m2 + 2) = *(m2 + 3) 
						= a;
					m2 += i;

					a = srcY[nAY[offset+1]];

					a |= (a<<8);
					a |= (a<<16);
					*m2 = *(m2 + 1) = *(m2 + 2) = *(m2 + 3) 
						= a;
					m2 += i;
					//srcY += (1<<1);
					offset += 2;
				} while (--j);
			}
			else
			{
				srcY--;
				j = 8;
				do {

					a = srcY[0];

					a |= (a<<8);
					a |= (a<<16);
					*m2 = *(m2 + 1) = *(m2 + 2) = *(m2 + 3) 
						= a;
					m2 += i;

					a = srcY[yPitch];

					a |= (a<<8);
					a |= (a<<16);
					*m2 = *(m2 + 1) = *(m2 + 2) = *(m2 + 3) 
						= a;
					m2 += i;
					srcY += (yPitch<<1);
				} while (--j);
			}
			break;
		case DC_PRED_16:                         // DC prediction
			if (mbAInfo)
			{
				avdNativeInt k = 4;
				s2 = 0;
				if(img->deblockType==DEBLOCK_BY_MB)
				{
#if FEATURE_INTERLACE		
					if(GetMbAffFrameFlag(img))
					{
						int offset = 0;
						int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
						src = GetMBAffLeftY(dbkCur,isTop);
						do {
							s2 += src[nAY[offset]] + src[nAY[offset+1]] + src[nAY[offset+2]] + src[nAY[offset+3]];    // sum vert pix
							offset += 4;
						} while (--k);
					}
					else
#endif//#if FEATURE_INTERLACE	
					{
						src = GetLeftMBY(dbkCur);
						//Dumploop(DUMP_SLICE,info,"DC_PRED_LEFT",16,src,DDT_D8);
						do {
							s2 += src[0] + src[1] + src[2] + src[3];    // sum vert pix
							src += 4;
							
						} while (--k);
					}
				}
				else
				{
					src = srcY - 1;
					a = (yPitch<<1);
					b = a + yPitch;
					c = (yPitch<<2);
					do {
						s2 += src[0] + src[yPitch] + src[a] + src[b];    // sum vert pix
						src += c;
					} while (--k);
				}
			}		
			if (mbBInfo)
			{
				
				srcY = pTopY;

				s1 = srcY[0] + srcY[1] + srcY[2] + srcY[3] + srcY[4] + srcY[5] + 
					srcY[6] + srcY[7] + srcY[8] + srcY[9] + srcY[10] + srcY[11] + 
					srcY[12] + srcY[13] + srcY[14] + srcY[15];

				if (mbAInfo)
					s0=(s1+s2+16)>>5;       // no edge
				else
					s0=(s1+8)>>4;              // left edge
			}
			else if (mbAInfo)
				s0=(s2+8)>>4;              // upper edge
			else
				s0=128;                            // top left corner, nothing to predict from

			tmp32 = ((s0<<8) | (s0));
			tmp32 |= (tmp32<<16);
			j = 8;
			do {
				m2[0] = m2[1] = m2[2] = m2[3] = tmp32;
				m2 += i;
				m2[0] = m2[1] = m2[2] = m2[3] = tmp32;
				m2 += i;
			} while (--j);
			break;
		case PLANE_16:// 16 bit integer plan pred

			if (!mbBInfo || !mbDInfo  || !mbAInfo)
			{
				AVD_ERROR_CHECK(img,"intrapred_luma_16x16: PLANE_16,mbAInfo is NULL",ERROR_IntraMBAIsNULL);;
			}	
			ih = iv = 0;

			
			plnY = pTopY;


			if(img->deblockType==DEBLOCK_BY_MB)
			{
				int offset = 0;
#if FEATURE_INTERLACE
				if(GetMbAffFrameFlag(img))
				{
					int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
					int idx = 8;
					int idx2 = 6;
					src  = GetMBAffLeftY(dbkCur,isTop);
					//src2 = GetMBAffLeftY(dbkCur,isTop);
					i = 7;
					do {
						ih += (8 - i) * (plnY[15 - i] - plnY[i - 1]);
						iv += (8 - i) * (src[nAY[idx]] - src[nAY[idx2]]);
						idx++;
						idx2--;
					} while (--i);
					src += nAY[15];
				}
				else
#endif//FEATURE_INTERLACE
				{
					src  = GetLeftMBY(dbkCur)+8;
					src2 = src - 2;
					i = 7;
					do {
						ih += (8 - i) * (plnY[15 - i] - plnY[i - 1]);
						iv += (8 - i) * (src[0] - src2[0]);
						src++;
						src2--;
					} while (--i);
				}
				
			}
			else
			{
				src  = srcY - 1 + (yPitch<<3);
				src2 = src - (yPitch<<1);
				i = 7;
				do {
					ih += (8 - i) * (plnY[15 - i] - plnY[i - 1]);
					iv += (8 - i) * (src[0] - src2[0]);
					src += yPitch;
					src2 -= yPitch;
				} while (--i);
			}


			if(img->deblockType==DEBLOCK_BY_MB)
			{
#if FEATURE_INTERLACE
				if (GetMbAffFrameFlag(img))
				{
					int P_X2=128;
					mbDInfo = GetMBAffNeighbor2(img,
						info,
						NEIGHBOR_D, 
						-1, 
						&nBY,
						16);
					if (mbDInfo)
					{
						int isNeighborA = IsNeighborDIsA(info);
						int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_D);
						if(!isNeighborA)
						{
							int isLastRow = nBY==15;
							P_X2 = *(GetMBAffTopY(dbk,isTop,isLastRow) + posInfo->pix_x - 1);
						}
						else
						{
							int yW = GetLumaYW(nBY);/// + 16)&15;
							P_X2 = *(GetMBAffLeftY(dbkCur,isTop) + yW);
						}
					}
					ih += ((plnY[15] - P_X2)<<3);
					iv += ((src[0] - P_X2)<<3);
				}
				else
#endif//FEATURE_INTERLACE
				{
					ih += ((plnY[15] - plnY[-1])<<3);
					iv += ((src[0] - plnY[-1])<<3);
				}
				
			}
			else
			{
				ih += ((plnY[15] - dbk->yuvD[0])<<3);
				iv += ((src[0] - dbk->yuvD[0])<<3);
			}


			iaa = ((src[0] + plnY[15])<<4) + 16;
			ib  = (ih + (ih<<2) + 32)>>6;
			ic  = (iv + (iv<<2) + 32)>>6;
			s0 = iaa - 7 * ic;
			iaa = 7 * ib;
			yPitch -= MB_BLOCK_SIZE;
			clip255 = clipInfo->clip255;
			j = MB_BLOCK_SIZE;
			do {
				s1 = s0 - iaa;
				i = MB_BLOCK_SIZE;
				do {
					int tmp1 = s1>>5;
					*srcY++ = iClip3(0,255,tmp1);
					s1 += ib;
				} while (--i);
				srcY += yPitch;
				s0 += ic;
			} while (--j);
			break;
		default:
			// indication of fault in bitstream,exit
			AVD_ERROR_CHECK(img,"16x16 intrapred:there is not predict mode",ERROR_IntraModeIsNULL);;	

			return SEARCH_SYNC;
		}
		
		//DUMP_SavePredTmp(img,plnY + posInfo->pix_y * yPitch + posInfo->pix_x,yPitch,16);
#if DUMP_VERSION
		DumpIntraPred(plnY + posInfo->pix_y * yPitch + posInfo->pix_x,yPitch,16,16);

#endif//DUMP_VERSION
		TIME_END(start,intra16Sum)
		return img->error;//DECODING_OK;
	}

	void intrapred_chroma_uv(ImageParameters *img,TMBsProcessor *info)
	{
		avdUInt8 *imgUV, *srcUV, *tmpPt,*tmpUV;  
		MacroBlock *mbAInfo, *mbBInfo, *mbDInfo;
		avdUInt32 *m1, *m2, tmp32a, tmp32b, tmp32c;
		avdNativeInt i, j, uv;
		avdNativeInt js0=0, js1=0, js2=0, js3=0;
		avdNativeInt f0=0, f1=0, f2=0, f3=0;
		avdNativeInt ih, iv, ib, ic, iaa;
		TSizeInfo	*sizeInfo = img->sizeInfo;
		avdNativeInt uvPitch = (sizeInfo->yPlnPitch>>1);
		TPosInfo*  posInfo  = GetPosInfo(info);
		avdNativeInt xIdx = posInfo->pix_c_x;
		avdNativeInt yIdx = posInfo->pix_c_y;
		avdNativeInt uvP4 = (uvPitch>>2);
		avdNativeInt nBX, nBY,P_X2; 
		StorablePicture	*dec_picture = img->dec_picture;
		avdUInt8 *clip255;
		TMBBitStream	*mbBits = GetMBBits(info->currMB);
		
		int nAY[8]={0,1,2,3,4,5,6,7};//default
#ifdef VOI_INPLACE_DEBLOCKING
		AVDIPDeblocker *dbk  = GetIPDeblocker(info->anotherProcessor);
#endif

		AVDIPDeblocker *dbkCur;
		int leftMBYForIntra;
		int start;
		TIME_BEGIN(start)
		if(img->deblockType==DEBLOCK_BY_MB)
			dbkCur  = GetIPDeblocker(info);


		mbAInfo = GetLeftMB(info);//GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,0,8,&leftMBYForIntra):
		mbBInfo = GetUpMB(info);
		mbDInfo = GetUpLeftMB(info);
		nBY = yIdx - 1;
		nBX = xIdx;
#if FEATURE_INTERLACE
		if(GetMbAffFrameFlag(img)&&mbAInfo==NULL&&IsNeedToCalcualteNeighborA_MBAFF(info))
		{
			mbAInfo = GetMBAffNeighbor2AnYPos(img,info,0, 
				nAY,8,8);
		}
#endif//#if FEATURE_INTERLACE	


		clip255 = img->clipInfo->clip255;
		for (uv = 0; uv < 2; uv++){

			imgUV = uv == 0 ? dec_picture->plnU : dec_picture->plnV;


			if(!GetMbAffFrameFlag(img))
				srcUV = &dbk->topUV[uv][xIdx];
#if FEATURE_INTERLACE
			else
			{
				mbBInfo = GetMBAffNeighbor2(img,
					info,
					NEIGHBOR_B, 
					-1, 
					&nBY,
					8);
				if (mbBInfo)
				{
					int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_B);
					int isLastRow = nBY==7;
					srcUV = IsNeighborBIsSelf(info)? GetMBAffTopUV(dbkCur,isTop,isLastRow,uv):GetMBAffTopUV(dbk,isTop,isLastRow,uv);
					srcUV +=  xIdx;
				}
				
			}
#endif//FEATURE_INTERLACE
#if DUMP_VERSION
			tmpUV = srcUV;
#endif
			switch (mbGetCPredMode(mbBits))
			{
			case DC_PRED_8:
				if (mbBInfo){
					js0 = srcUV[0] + srcUV[1] + srcUV[2] + srcUV[3];
					js1 = srcUV[4] + srcUV[5] + srcUV[6] + srcUV[7];
				}

				if (mbAInfo){
					if(img->deblockType==DEBLOCK_BY_MB)
					{
						avdUInt8* pUV1;
#if FEATURE_INTERLACE	
						if(GetMbAffFrameFlag(img))
						{
							int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
							pUV1 = GetMBAffLeftUV(dbkCur,isTop,uv);
							js2 = pUV1[nAY[0]] + pUV1[nAY[1]] + pUV1[nAY[2] ]+ pUV1[nAY[3]];
							js3 = pUV1[nAY[4]] + pUV1[nAY[5]] + pUV1[nAY[6]] + pUV1[nAY[7]];
						}
						else
#endif//#if FEATURE_INTERLACE	
						{
							pUV1 = GetLeftMBUV(dbkCur,uv);
							js2 = pUV1[0] + pUV1[1] + pUV1[2] + pUV1[3];
							js3 = pUV1[4] + pUV1[5] + pUV1[6] + pUV1[7];
						}
					}
					else
					{
						j = nBY * uvPitch + nBX + uvPitch - 1;
						tmp32a = (uvPitch<<1);
						tmp32b = tmp32a + uvPitch;
						imgUV += j;
						js2 = imgUV[0] + imgUV[uvPitch] +
							imgUV[tmp32a] + imgUV[tmp32b];

						imgUV += (uvPitch<<2);
						js3 = imgUV[0] + imgUV[uvPitch] +
							imgUV[tmp32a] + imgUV[tmp32b];

						imgUV -= (j + (uvPitch<<2));
					}


					if (mbBInfo){
						f0 = ((js0+js2+4)>>3);
						f0 |= (f0<<8);
						f0 |= (f0<<16);
						f1 = ((js3+2)>>2);
						f1 |= (f1<<8);
						f1 |= (f1<<16);
						f2 = ((js1+2)>>2);
						f2 |= (f2<<8);
						f2 |= (f2<<16);
						f3 = ((js1+js3+4)>>3);
						f3 |= (f3<<8);
						f3 |= (f3<<16);
					}
					else{
						f0 = ((js2+2)>>2);
						f0 |= (f0<<8);
						f0 |= (f0<<16);
						f2 = f0;
						f1 = ((js3+2)>>2);
						f1 |= (f1<<8);
						f1 |= (f1<<16);
						f3 = f1;
					}
				}
				else if (mbBInfo){
					f0 = ((js0+2)>>2);
					f0 |= (f0<<8);
					f0 |= (f0<<16);
					f1 = f0;
					f2 = ((js1+2)>>2);
					f2 |= (f2<<8);
					f2 |= (f2<<16);
					f3 = f2;
				}
				else
					f0 = f1 = f2 = f3 = 0x80808080;


				m1 = (avdUInt32 *)(imgUV + yIdx * uvPitch + xIdx);

				tmp32a = (uvP4<<1);
				tmp32b = ((uvP4<<1) + uvP4);
				*m1 = *(m1 + uvP4) = *(m1 + tmp32a) 
					= *(m1 + tmp32b) = f0;
				m1++;
				*m1 = *(m1 + uvP4) = *(m1 + tmp32a) 
					= *(m1 + tmp32b) = f2;

				m1 += (uvPitch - 1);
				*m1 = *(m1 + uvP4) = *(m1 + tmp32a) 
					= *(m1 + tmp32b) = f1;
				m1++;
				*m1 = *(m1 + uvP4) = *(m1 + tmp32a) 
					= *(m1 + tmp32b) = f3;

				break;
			case HOR_PRED_8:

				if (!mbAInfo)
				{
					AVD_ERROR_CHECK2(img,"unexpected HOR_PRED_8 chroma intra prediction mode",ERROR_IntraMBAIsNULL);;
				}


				srcUV = imgUV + yIdx * uvPitch + xIdx;

				j = 8;
				if(img->deblockType==DEBLOCK_BY_MB)
				{
					avdUInt8* pUV1;
#if FEATURE_INTERLACE
					if(GetMbAffFrameFlag(img))
					{
						int i;
						int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
						pUV1 = GetMBAffLeftUV(dbkCur,isTop,uv);
						
						for (i=0;i<8;i++)
						{
							tmp32a = pUV1[nAY[i]];
							m1 = (avdUInt32 *)srcUV;
							tmp32a |= (tmp32a<<8);
							*m1 = *(m1 + 1) = (tmp32a | (tmp32a<<16));
							srcUV += uvPitch;
						}
					}
					else
#endif//FEATURE_INTERLACE
					{
						pUV1 = GetLeftMBUV(dbkCur,uv);

						do {
							tmp32a = *pUV1++;
							m1 = (avdUInt32 *)srcUV;
							tmp32a |= (tmp32a<<8);
							*m1 = *(m1 + 1) = (tmp32a | (tmp32a<<16));
							srcUV += uvPitch;
						} while (--j);
					}
				}

				else
				{
					do {
						tmp32a = srcUV[-1];
						m1 = (avdUInt32 *)srcUV;
						tmp32a |= (tmp32a<<8);
						*m1 = *(m1 + 1) = (tmp32a | (tmp32a<<16));
						srcUV += uvPitch;
					} while (--j);
				}
				break;
			case VERT_PRED_8:

				if (!mbBInfo)
				{
					AVD_ERROR_CHECK2(img,"unexpected VERT_PRED_8 chroma intra prediction mode",ERROR_IntraMBBIsNULL);;
				}


				m1 = (avdUInt32 *)(imgUV + yIdx * uvPitch + xIdx);


				if(GetMbAffFrameFlag(img))
				{
					m2 = (avdUInt32 *)(srcUV);
				}
				else
					m2 = (avdUInt32 *)&dbk->topUV[uv][xIdx];

				j = 8; 
				tmp32a = *m2;
				tmp32b = *(m2 + 1);
				do {
					*m1 = tmp32a;
					*(m1 + 1) = tmp32b;
					m1 += uvP4;
				} while (--j);
				break;
			case PLANE_8:

				if (!mbDInfo || !mbAInfo || !mbBInfo)
				{
					AVD_ERROR_CHECK2(img,"unexpected PLANE_8 chroma intra prediction mode",ERROR_IntraMBAIsNULL);;
				}
				ih = (srcUV[4] - srcUV[2]) +
					((srcUV[5] - srcUV[1])<<1) +
					((srcUV[6] - srcUV[0]) * 3);
				if(img->deblockType==DEBLOCK_BY_MB)
				{
#if FEATURE_INTERLACE
					if(GetMbAffFrameFlag(img))
					{
						mbDInfo = GetMBAffNeighbor2(img,
							info,
							NEIGHBOR_D, 
							-1, 
							&nBY,
							8);
						if (mbDInfo)
						{
							int isNeighborA = IsNeighborDIsA(info);
							int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_D);
							if(!isNeighborA)
							{
								int isLastRow = nBY==7;
								avdUInt8* tmp = GetMBAffTopUV(dbk,isTop,isLastRow,uv);
								P_X2 = tmp[xIdx - 1];
							}
							else
							{
								int yW = GetChromaYW(nBY); //(nBY + 16)&15;
								avdUInt8* tmp = GetMBAffLeftUV(dbkCur,isTop,uv);
								P_X2 = tmp[yW];
							}
						}
						ih+=((srcUV[7] - P_X2)<<2);
					}
					else
#endif//FEATURE_INTERLACE
						ih+=((srcUV[7] - srcUV[-1])<<2);
				}
				else
				{
					ih+=((srcUV[7] - dbk->yuvD[uv+1])<<2);
				}


				
				if(img->deblockType==DEBLOCK_BY_MB)
				{
					avdUInt8* pUV1;
#if FEATURE_INTERLACE					
					if(GetMbAffFrameFlag(img))
					{
						int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
						pUV1 = GetMBAffLeftUV(dbkCur,isTop,uv);
						iv = (pUV1[nAY[4]] - pUV1[nAY[2]]) +
							((pUV1[nAY[5]] - pUV1[nAY[1]])<<1) +
							((pUV1[nAY[6]] - pUV1[nAY[0]]) * 3);
						iaa = ((srcUV[7] + pUV1[nAY[7]])<<4) + 16;//TBD is it pUV1[7] or pUV1[3]?
					}
					else
#endif//#if FEATURE_INTERLACE	
					{
						pUV1 = GetLeftMBUV(dbkCur,uv);
						iv = (pUV1[4] - pUV1[2]) +
							((pUV1[5] - pUV1[1])<<1) +
							((pUV1[6] - pUV1[0]) * 3);
						iaa = ((srcUV[7] + pUV1[7])<<4) + 16;//TBD: is it pUV1[7] or pUV1[3]?
					}
					if(1)
					{
						if(GetMbAffFrameFlag(img))
							iv+=((pUV1[nAY[7]] - P_X2)<<2);
						else
							iv+=((pUV1[7] - srcUV[-1])<<2);
					}
					else
					{
						iv+=((pUV1[3] - dbk->yuvD[uv+1])<<2);
					}

					
				}
				else
				{
					j = nBY * uvPitch + nBX + uvPitch + (uvPitch<<2) - 1;
					iv = (imgUV[j] - imgUV[-(uvPitch<<1)+j]) +
						((imgUV[uvPitch+j] - imgUV[-uvPitch - (uvPitch*2)+j])<<1) +
						((imgUV[uvPitch*2+j] - imgUV[-(uvPitch<<2)+j]) * 3) ;

					if(0)
					{
						iv+=((imgUV[uvPitch + (uvPitch<<1)+j] - srcUV[-1])<<2);
					}
					else
					{
						iv+=((imgUV[uvPitch + (uvPitch<<1)+j] - dbk->yuvD[uv+1])<<2);
					}
					iaa = ((srcUV[7] + imgUV[uvPitch + (uvPitch<<1)+j])<<4) + 16;
				}

				ib  = (ih + (ih<<4) + 16)>>5;
				ic  = (iv + (iv<<4) + 16)>>5;
				srcUV = imgUV + yIdx * uvPitch + xIdx;
				js0 = iaa - 3 * ic;
				iaa = -3 * ib;
				js1 = uvPitch - 8;
				j = 8;
				do {
					js2 = iaa + js0;
					i = 8;
					do {
						int tmp1 = js2>>5;
						*srcUV++ = iClip3(0,255,tmp1);
						js2 += ib;
					} while (--i);
					js0 += ic;
					srcUV += js1;
				} while (--j);
				break;
			default:

				AVD_ERROR_CHECK2(img,"illegal chroma intra prediction mode", ERROR_IntraModeIsNULL);;
				break;
			}
			DumpIntraPred(tmpUV,uvPitch,8,8);

		} // for uv;
		TIME_END(start,intracSum)

	}
#if FEATURE_T8x8
#define  imgpel avdUInt8
#define P_X (PredPel[0])
#define H_0 (PredPel[1])
#define H_1 (PredPel[2])
#define H_2 (PredPel[3])
#define H_3 (PredPel[4])
#define H_4 (PredPel[5])
#define H_5 (PredPel[6])
#define H_6 (PredPel[7])
#define H_7 (PredPel[8])
#define H_8 (PredPel[9])
#define H_9 (PredPel[10])
#define H_10 (PredPel[11])
#define H_11 (PredPel[12])
#define H_12 (PredPel[13])
#define H_13 (PredPel[14])
#define H_14 (PredPel[15])
#define H_15 (PredPel[16])
#define V_0 (PredPel[17])
#define V_1 (PredPel[18])
#define V_2 (PredPel[19])
#define V_3 (PredPel[20])
#define V_4 (PredPel[21])
#define V_5 (PredPel[22])
#define V_6 (PredPel[23])
#define V_7 (PredPel[24])

#define P2_Z (PredPel[0])
#define P_Z  P2_Z
#define P2_A (PredPel[1])
#define P2_B (PredPel[2])
#define P2_C (PredPel[3])
#define P2_D (PredPel[4])
#define P2_E (PredPel[5])
#define P2_F (PredPel[6])
#define P2_G (PredPel[7])
#define P2_H (PredPel[8])
#define P2_I (PredPel[9])
#define P2_J (PredPel[10])
#define P2_K (PredPel[11])
#define P2_L (PredPel[12])
#define P2_M (PredPel[13])
#define P2_N (PredPel[14])
#define P2_O (PredPel[15])
#define P2_P (PredPel[16])
#define P2_Q (PredPel[17])
#define P2_R (PredPel[18])
#define P2_S (PredPel[19])
#define P2_T (PredPel[20])
#define P2_U (PredPel[21])
#define P2_V (PredPel[22])
#define P2_W (PredPel[23])
#define P2_X (PredPel[24])
#define  block_available_up_left (int)mbDInfo
#define  block_available_up		(int)mbBInfo
#define  block_available_left		(int)mbAInfo


	/*!
	*************************************************************************************
	* \brief
	*    Prefiltering for Intra8x8 prediction (Vertical)
	*************************************************************************************

	/ / X A B C D E F G H
	//  I a b c d
	//  J e f g h
	//  K i j k l
	//  L m n o p
	//
	*/
#define DUMP_I8x8 0
#if(DUMP_I8x8)//(DUMP_VERSION & DUMP_SLICE)
#define DUMP_NEIGHBOR_BEFORE_UP \
	Dumploop(DUMP_SLICE, info,"Intra8_Neighbor_before_up",16,&(H_0),DDT_D8);
#define DUMP_NEIGHBOR_BEFORE_LEFT \
	Dumploop(DUMP_SLICE, info,"Intra8_Neighbor_before_left",8,&(V_0),DDT_D8);
#define DUMP_NEIGHBOR_BEFORE_UPLEFT \
	Dumploop(DUMP_SLICE, info,"Intra8_Neighbor_before_upleft",1,&(P_Z),DDT_D8);
#define DUMP_NEIGHBOR_AFTER_UP \
	Dumploop(DUMP_SLICE, info,"Intra8_Neighbor_after_up",16,&(H_0),DDT_D8);
#define DUMP_NEIGHBOR_AFTER_LEFT \
	Dumploop(DUMP_SLICE, info,"Intra8_Neighbor_after_left",8,&(V_0),DDT_D8);
#define DUMP_NEIGHBOR_AFTER_UPLEFT \
	Dumploop(DUMP_SLICE, info,"Intra8_Neighbor_after_upleft",1,&(P_Z),DDT_D8);
#define DUMP_NEIGHBOR_BEFORE
#else//(DUMP_VERSION & DUMP_SLICE)
#define DUMP_NEIGHBOR_BEFORE
#define DUMP_NEIGHBOR_BEFORE_UP
#define DUMP_NEIGHBOR_BEFORE_LEFT
#define DUMP_NEIGHBOR_BEFORE_UPLEFT
#define DUMP_NEIGHBOR_AFTER_UPLEFT
#define DUMP_NEIGHBOR_AFTER_UP
#define DUMP_NEIGHBOR_AFTER_LEFT
#endif//(DUMP_VERSION & DUMP_SLICE)
#define FILTER_UPLEFT(X) if(mbDInfo)\
	{	DUMP_NEIGHBOR_BEFORE_UPLEFT\
	if(mbAInfo && mbBInfo)\
	{\
	P_X = ((V_0 + ((X)<<1) + H_0 + 2)>>2);\
	}\
	else\
	{\
	if(mbBInfo)\
	P_X = (((X) + ((X)<<1) + H_0 + 2)>>2);\
		else if (mbAInfo)\
		P_X = (((X) + ((X)<<1) + V_0 + 2)>>2);\
	}\
	DUMP_NEIGHBOR_AFTER_UPLEFT\
	}
#define FILTER_LEFT(X) if(mbAInfo){\
	int temp=V_0;\
	int temp2;\
	DUMP_NEIGHBOR_BEFORE_LEFT\
	if (mbDInfo)\
	V_0= (((X) + (V_0<<1) + V_1 + 2)>>2);\
	else\
	V_0= ((V_0 + (V_0<<1) + V_1 + 2)>>2);\
	temp2=V_1;V_1= ((temp+ (V_1<<1) + V_2 + 2)>>2);\
	temp=V_2;V_2= ((temp2 + (V_2<<1) + V_3 + 2)>>2);\
	temp2=V_3;V_3= ((temp + (V_3<<1) + V_4 + 2)>>2);\
	temp=V_4;V_4= ((temp2 + (V_4<<1) + V_5 + 2)>>2);\
	temp2=V_5;V_5= ((temp + (V_5<<1) + V_6 + 2)>>2);\
	temp=V_6;V_6= ((temp2 + (V_6<<1) + V_7+ 2)>>2);\
	V_7= ((temp + (V_7<<1) + V_7 + 2)>>2);\
	DUMP_NEIGHBOR_AFTER_LEFT\
	}

#define FILTER_UP(X) if(mbBInfo){\
	int temp=H_0;\
	int temp2;\
	DUMP_NEIGHBOR_BEFORE_UP\
	if (mbDInfo)\
	H_0= (((X) + (H_0<<1) + H_1 + 2)>>2);\
	else\
	H_0= ((H_0 + (H_0<<1) + H_1 + 2)>>2);\
	temp2=H_1;H_1= ((temp + (H_1<<1) + H_2 + 2)>>2);\
	temp=H_2;H_2= ((temp2 + (H_2<<1) + H_3 + 2)>>2);\
	temp2=H_3;H_3= ((temp+ (H_3<<1) + H_4 + 2)>>2);\
	temp=H_4;H_4= ((temp2 + (H_4<<1) + H_5 + 2)>>2);\
	temp2=H_5;H_5= ((temp + (H_5<<1) + H_6 + 2)>>2);\
	temp=H_6;H_6= ((temp2 + (H_6<<1) + H_7 + 2)>>2);\
	temp2=H_7;H_7= ((temp + (H_7<<1) + H_8 + 2)>>2);\
	temp=H_8;H_8= ((temp2 + (H_8<<1) + H_9 + 2)>>2);\
	temp2=H_9;H_9= ((temp + (H_9<<1) + H_10 + 2)>>2);\
	temp=H_10;H_10= ((temp2 + (H_10<<1) + H_11 + 2)>>2);\
	temp2=H_11;H_11= ((temp + (H_11<<1) + H_12 + 2)>>2);\
	temp=H_12;H_12= ((temp2 + (H_12<<1) + H_13 + 2)>>2);\
	temp2=H_13;H_13= ((temp + (H_13<<1) + H_14 + 2)>>2);\
	temp=H_14;H_14= ((temp2 + (H_14<<1) + H_15 + 2)>>2);\
	H_15= ((temp + (H_15<<1) + H_15 + 2)>>2);\
	DUMP_NEIGHBOR_AFTER_UP\
	}


	avdNativeInt intrapred8x8(ImageParameters *img,TMBsProcessor *info,
		avdNativeInt img_x,      //!< location of block X
		avdNativeInt img_y)      //!< location of block Y
	{
		StorablePicture	*dec_picture = img->dec_picture;
		avdUInt8 *Y4Rows0, *Y4Rows1, *Y4Rows2, *Y4Rows3, *Y4Rows4, *Y4Rows5, *Y4Rows6, *Y4Rows7, *tmpY;
		MacroBlock   *currMBInfo = info->currMB;
		////TMBsProcessor *info  = img->mbsProcessor;
		MacroBlock	*mbAInfo=NULL, *mbBInfo=NULL, *mbCInfo=NULL, *mbDInfo=NULL;
		avdUInt8 *plnY;
		avdUInt32 ui32;
		avdUInt64 ui64,ui64_1;
		TSizeInfo	*sizeInfo = img->sizeInfo;
		TMBBitStream	*mbBits = GetMBBits(info->currMB);
		AVDIPDeblocker *dbkCur,*dbkCurBottom,*dbkBottom;
		avdUInt8*		pLeftY = NULL,*pTopY=NULL;
		avdUInt8* YLeftMBAFF;
		avdNativeInt nBX, nBY; 
		avdNativeInt i, ioff, joff, y8off,predmode;
		avdInt32 s0, yPitch;
		TPosInfo* posInfo = GetPosInfo(info);
		int multiCoreNum = img->vdLibPar->multiCoreNum;
		avdNativeInt nAY[8];
#define INIT_PRED_VAL 128
		avdUInt32 PredArray[7];
		imgpel *PredPel = (imgpel *)PredArray;
#ifdef VOI_INPLACE_DEBLOCKING

		AVDIPDeblocker *dbk  = GetIPDeblocker(info->anotherProcessor);
		int start;
		TIME_BEGIN(start)
		if(dbk==NULL)
		{
			AVD_ERROR_CHECK(img,"intrapred:dbk==NULL",ERROR_NULLPOINT);
		}
		if(img->deblockType==DEBLOCK_BY_MB)
		{
			dbkCur  = GetIPDeblocker(info);
		}

#endif
		//memset(PredPel,INIT_PRED_VAL,sizeof(PredPel));
		PredArray[0] = PredArray[1] = PredArray[2] = PredArray[3] = PredArray[4] = PredArray[5] = PredArray[6] = 0x80808080;

		plnY = dec_picture->plnY; 
		yPitch = sizeInfo->yPlnPitch;
		Y4Rows0 = plnY + img_y * yPitch + img_x; 
		

		if(plnY==NULL)
		{
			AVD_ERROR_CHECK(img,"intrapred:plnY==NULL",ERROR_NULLPOINT);;
		}
		Y4Rows1 = Y4Rows0 + yPitch;
		Y4Rows2 = Y4Rows1 + yPitch;
		Y4Rows3 = Y4Rows2 + yPitch;
		Y4Rows4 = Y4Rows3 + yPitch;
		Y4Rows5 = Y4Rows4 + yPitch;
		Y4Rows6 = Y4Rows5 + yPitch;
		Y4Rows7 = Y4Rows6 + yPitch;

		ioff = (img_x&0xf)>>2;
		y8off = img_y&0xf;
		joff = (y8off)>>2;
		
		predmode = mbBits->uMBS.mbIntra4x4Mode[(y8off)|ioff];


		if(img->deblockType==DEBLOCK_BY_MB&&ioff==0)
		{
			if(!GetMbAffFrameFlag(img))
				pLeftY = GetLeftMBY(dbkCur) + joff*4;
			else//default, the left MB has same type(frame/field)
				pLeftY = GetMBAffLeftY(dbkCur,CurrMBisTop(info)) + joff*4;
		}
		if(joff==0)
		{
#if FEATURE_INTERLACE

			if(GetMbAffFrameFlag(img))
			{
				mbBInfo = GetMBAffNeighbor2(img,
					info,
					NEIGHBOR_B, 
					-1, 
					&nBY,
					16);
				if (mbBInfo)
				{
					int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_B);
					int isLastRow = nBY==15;
					if(IsNeighborBIsSelf(info))
					{
						pTopY = GetMBAffTopY(dbkCur,isTop,isLastRow);
					}
					else
					{
						pTopY = GetMBAffTopY(dbk,isTop,isLastRow);
					}
					pTopY +=  img_x;
				}
			}
			else
#endif//FEATURE_INTERLACE
				pTopY = dbk->topY + img_x;
		}
#if (DUMP_VERSION & DUMP_SLICE)
		//AvdLog(DUMP_SLICEDATA,DUMP_DCORE"\nT8x8_PredMode=%d\n",predmode);
#endif//#if (DUMP_VERSION & DUMP_SLICE)


		if (1)//predmode != VERT_PRED && predmode != DIAG_DOWN_LEFT_PRED && predmode != VERT_LEFT_PRED)

		{
#if FEATURE_INTERLACE
			if(GetMbAffFrameFlag(img)&&ioff==0&&IsNeedToCalcualteNeighborA_MBAFF(info))
			{

				mbAInfo = GetMBAffNeighbor2AnYPos(img,info,y8off, 
					nAY,8,16);
				if(mbAInfo)	
				{
					//YLeftMBAFF = Y4Rows0 - 1 - y4off;
					int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
					pLeftY = GetMBAffLeftY(dbkCur,isTop);
					//Dumploop(DUMP_SLICE,info,"4x4DC_PRED_LEFT",4,pLeftY,DDT_D8);
					V_0 = pLeftY[nAY[0]];
					V_1 = pLeftY[nAY[1]];
					V_2 = pLeftY[nAY[2]];
					V_3 = pLeftY[nAY[3]];
					V_4 = pLeftY[nAY[4]];
					V_5 = pLeftY[nAY[5]];
					V_6 = pLeftY[nAY[6]];
					V_7 = pLeftY[nAY[7]];
				}

			}
			else					
#endif//FEATURE_INTERLACE
			{
				int leftMBYForIntra;
				mbAInfo = (ioff > 0) ? currMBInfo : GetLeftMB(info);
				if (mbAInfo)
				{

					if(pLeftY)
					{

						V_0 = *pLeftY++;
						V_1 = *pLeftY++;
						V_2 = *pLeftY++;
						V_3 = *pLeftY++;
						V_4 = *pLeftY++;
						V_5 = *pLeftY++;
						V_6 = *pLeftY++;
						V_7 = *pLeftY;
						pLeftY-=7;
					}
					else

					{
						V_0 = Y4Rows0[-1];
						V_1 = Y4Rows1[-1];
						V_2 = Y4Rows2[-1];
						V_3 = Y4Rows3[-1];
						V_4 = Y4Rows4[-1];
						V_5 = Y4Rows5[-1];
						V_6 = Y4Rows6[-1];
						V_7 = Y4Rows7[-1];
					}
				}
			}
			

		}

		if (1)//predmode != HOR_PRED)
		{

			nBY = img_y - 1;
			nBX = img_x; //

			mbBInfo = (joff > 0) ? currMBInfo : GetUpMB(info);
			if (1)//PRED_MODE_NOT_VERT_OR_HORUP(predmode))
			{
				if (mbBInfo)
				{

					if (joff){
						tmpY = Y4Rows0 - yPitch;
						H_0 = tmpY[0];
						H_1 = tmpY[1];
						H_2 = tmpY[2];
						H_3 = tmpY[3];
						H_4 = tmpY[4];
						H_5 = tmpY[5];
						H_6 = tmpY[6];
						H_7 = tmpY[7];
					}
					else{
						H_0 = pTopY[0];
						H_1 = pTopY[1];
						H_2 = pTopY[2];
						H_3 = pTopY[3];
						H_4 = pTopY[4];
						H_5 = pTopY[5];
						H_6 = pTopY[6];
						H_7 = pTopY[7];
					}
				}

				if (1)//predmode == VERT_LEFT_PRED ||predmode == DIAG_DOWN_LEFT_PRED)
				{
					int hasmbC=0;
					avdUInt8* pTopY_C = NULL;
					H_8 = 
					H_9 = 
					H_10 = 
					H_11 = 
					H_12 = 
					H_13 = 
					H_14 =
					H_15 = H_7;//default
					if (ioff < 2)
						hasmbC = (!joff) ? (int)mbBInfo : 1;
					else if(!GetMbAffFrameFlag(img))
						hasmbC = !joff ? (int)GetUpRightMB(info) : NULL;
					else
					{
						int nCY;
						mbCInfo = !joff ? GetMBAffNeighbor2(img,
							info,
							NEIGHBOR_C, 
							-1, 
							&nCY,
							16)
							:NULL;
						if (mbCInfo)
						{
							int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_C);
							int isLastRow = nCY == 15;
							pTopY_C = GetMBAffTopY(dbk,isTop,isLastRow) + posInfo->pix_x + 16;
							H_8  = pTopY_C[0];
							H_9  = pTopY_C[1];
							H_10 = pTopY_C[2];
							H_11 = pTopY_C[3];
							H_12 = pTopY_C[4];
							H_13 = pTopY_C[5];
							H_14 = pTopY_C[6];
							H_15 = pTopY_C[7];
							hasmbC = (int)mbCInfo;
						}
					}

					if (hasmbC&&pTopY_C==NULL)
					{

						if (joff){

							tmpY = Y4Rows0 - yPitch + 8;
							H_8 = tmpY[0];
							H_9 = tmpY[1];
							H_10 = tmpY[2];
							H_11 = tmpY[3];
							H_12 = tmpY[4];
							H_13 = tmpY[5];
							H_14 = tmpY[6];
							H_15 = tmpY[7];

						}
						else{
							H_8  = pTopY[8];
							H_9  = pTopY[9];
							H_10  = pTopY[10];
							H_11  = pTopY[11];
							H_12  = pTopY[12];
							H_13  = pTopY[13];
							H_14  = pTopY[14];
							H_15  = pTopY[15];
						}

					}
					
				}

				if(!GetMbAffFrameFlag(img))
				{
					int leftMBYForIntra;
					mbDInfo = (ioff > 0) ? mbBInfo : (joff > 0 ? mbAInfo :
						GetUpLeftMB(info));
					if (mbDInfo)
					{

						if (joff)
						{
							if(pLeftY)
								P_X = pLeftY[-1];
							else

								P_X = Y4Rows0[-yPitch - 1];
						}
						else 
						{
							if(img->deblockType==DEBLOCK_BY_MB)
							{
								P_X = pTopY[-1];
							}
							else
							{
								if (ioff)
									P_X = pTopY[-1];
								else
									P_X = dbk->yuvD[0];
							}
						}	
					}
				}
				else
				{
					mbDInfo = (ioff > 0) ? mbBInfo : (joff > 0 ? 
						GetLeftMBAffMB2(img,info,y8off-1,16,&nBY)
						:GetMBAffNeighbor2(img,
						info,
						NEIGHBOR_D, 
						y8off-1, 
						&nBY,
						16)
						);
					if (mbDInfo)
					{

						if (joff){

							if(ioff==0)
							{
								int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_A);//GetLeftMBYPos(info).mbAddr&1;
								avdUInt8* pLeftY2 = GetMBAffLeftY(dbkCur,isTop);
								P_X	= pLeftY2[nBY];

							}
							else
								P_X = Y4Rows0[(-yPitch -1)];

						}
						else 
						{
							if(ioff)
							{
								P_X = pTopY[-1];
							}
							else
							{
								int isNeighborA = IsNeighborDIsA(info);
								int isTop = IsTopMBAffNeighbor(info,NEIGHBOR_D);
								if(!isNeighborA)
								{
									int isLastRow = nBY==15;
									avdUInt8* pTopY_D =  GetMBAffTopY(dbk,isTop,isLastRow)+img_x ;
									P_X = pTopY_D[- 1];

								}
								else
								{
									int yW = (nBY + 16)&15;
									P_X = *(GetMBAffLeftY(dbkCur,isTop) + yW);

								}
							}


						}	
					}
				}//else MBAFF
			 }
		}
		

		switch (predmode)
		{
		case VERT_PRED:                       /* vertical prediction from block above */
			{
				//LowPassForIntra8x8PredHor(&(P_Z), block_available_up_left, block_available_up, block_available_left);
				FILTER_UP(P_X)
					ui64=(H_7<<24)|(H_6<<16)|(H_5<<8)|(H_4);//|(H_3<<24)|(H_2<<16)|(H_1<<8)|H_0;
				ui64=(ui64<<24)|(H_3<<16)|(H_2<<8)|(H_1);
				ui64=(ui64<<8)|H_0;
#if 0//def VOI_INPLACE_DEBLOCKING
				if (!joff)
					s0 = *(avdInt32 *)&pTopY[img_x]; 
				else
#endif // VOI_INPLACE_DEBLOCKING
				{
					*(avdUInt64 *)Y4Rows0 = *(avdUInt64 *)Y4Rows1 
						= *(avdUInt64 *)Y4Rows2 = *(avdUInt64 *)Y4Rows3 
						= *(avdUInt64 *)Y4Rows4 = *(avdUInt64 *)Y4Rows5 
						= *(avdUInt64 *)Y4Rows6 = *(avdUInt64 *)Y4Rows7 = ui64;
				}
			}



			break;
		case HOR_PRED:                        /* horizontal prediction from left block */
			//LowPassForIntra8x8PredVer(&(P_Z), block_available_up_left, block_available_up, block_available_left);
			FILTER_LEFT(P_X)

				*(avdUInt32 *)Y4Rows0 = *(avdUInt32 *)(Y4Rows0+4)=(V_0 | (V_0<<8) | (V_0<<16) | (V_0<<24));
			*(avdUInt32 *)Y4Rows1 = *(avdUInt32 *)(Y4Rows1+4)=(V_1 | (V_1<<8) | (V_1<<16) | (V_1<<24));
			*(avdUInt32 *)Y4Rows2 = *(avdUInt32 *)(Y4Rows2+4)=(V_2 | (V_2<<8) | (V_2<<16) | (V_2<<24));
			*(avdUInt32 *)Y4Rows3 = *(avdUInt32 *)(Y4Rows3+4)=(V_3 | (V_3<<8) | (V_3<<16) | (V_3<<24));
			*(avdUInt32 *)Y4Rows4 = *(avdUInt32 *)(Y4Rows4+4)=(V_4 | (V_4<<8) | (V_4<<16) | (V_4<<24));
			*(avdUInt32 *)Y4Rows5 = *(avdUInt32 *)(Y4Rows5+4)=(V_5 | (V_5<<8) | (V_5<<16) | (V_5<<24));
			*(avdUInt32 *)Y4Rows6 = *(avdUInt32 *)(Y4Rows6+4)=(V_6 | (V_6<<8) | (V_6<<16) | (V_6<<24));
			*(avdUInt32 *)Y4Rows7 = *(avdUInt32 *)(Y4Rows7+4)=(V_7 | (V_7<<8) | (V_7<<16) | (V_7<<24));


			break;
		case DC_PRED:                         /* DC prediction */
			//LowPassForIntra8x8Pred(&(P_Z), block_available_up_left, block_available_up, block_available_left);
			if (mbBInfo){
				if (mbAInfo)
				{
					FILTER_UP(P_X)
						FILTER_LEFT(P_X)

				}
				else
				{
					FILTER_UP(P_X)
				}
				s0 = mbAInfo ?
					(H_0+ H_1+H_2+H_3+H_4+H_5+H_6+H_7+V_0+ V_1+V_2+V_3+V_4+V_5+V_6+V_7+ 8)>>4
					: (H_0+ H_1+H_2+H_3+H_4+H_5+H_6+H_7 + 4)>>3;
			}
			else{
				if (mbAInfo)
				{
					FILTER_LEFT(P_X)
				}
				s0 = mbAInfo ? (V_0+ V_1+V_2+V_3+V_4+V_5+V_6+V_7 + 4)>>3
					: 128;
			}

			s0 |= (s0<<8);
			*(avdUInt32 *)Y4Rows0 = *(avdUInt32 *)Y4Rows1
				= *(avdUInt32 *)Y4Rows2 = *(avdUInt32 *)Y4Rows3
				=*(avdUInt32 *)Y4Rows4 = *(avdUInt32 *)Y4Rows5
				= *(avdUInt32 *)Y4Rows6 = *(avdUInt32 *)Y4Rows7
				=*(avdUInt32 *)(Y4Rows0+4) = *(avdUInt32 *)(Y4Rows1+4)
				= *(avdUInt32 *)(Y4Rows2+4) = *(avdUInt32 *)(Y4Rows3+4)
				=*(avdUInt32 *)(Y4Rows4+4) = *(avdUInt32 *)(Y4Rows5+4)
				= *(avdUInt32 *)(Y4Rows6+4) = *(avdUInt32 *)(Y4Rows7+4)
				= (s0 | (s0<<16));

			break;
		case DIAG_DOWN_LEFT_PRED:
			//LowPassForIntra8x8Pred(&(P_Z), block_available_up_left, block_available_up, block_available_left);
			FILTER_UP(P_X)
				ui64 = 

				(((H_11 + H_13+ (H_12<<1) + 2)>>2)) | 
				(((H_12 + H_14 + (H_13<<1) + 2)>>2)<<8) | 
				(((H_13 + H_15 + (H_14<<1) + 2)>>2)<<16) | 
				(((H_14 + H_15 + (H_15<<1) + 2)>>2)<<24);

			ui64 = (ui64<<24)|(((H_8 + H_10 + (H_9<<1) + 2)>>2)) | 
				(((H_9 + H_11 + (H_10<<1) + 2)>>2)<<8) | 
				(((H_10 + H_12 + (H_11<<1) + 2)>>2)<<16);

			ui64 = (ui64<<8)|((H_7 + H_9 + (H_8<<1) + 2)>>2) ;
			*(avdUInt64 *)Y4Rows7 = ui64;
			ui64 = (((H_6 + H_8 + (H_7<<1) + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows6 = ui64;
			ui64 = (((H_5 + H_7 + (H_6<<1) + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows5 = ui64;
			ui64 = (((H_4 + H_6 + (H_5<<1) + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows4 = ui64;
			ui64 = (((H_3 + H_5 + (H_4<<1) + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows3 = ui64;
			ui64 = (((H_2 + H_4 + (H_3<<1) + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows2 = ui64;
			ui64 = (((H_1 + H_3 + (H_2<<1) + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows1 = ui64;
			ui64 = (((H_0 + H_2 + (H_1<<1) + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows0 = ui64;
			break;
		case DIAG_DOWN_RIGHT_PRED:
			//LowPassForIntra8x8Pred(&(P_Z), block_available_up_left, block_available_up, block_available_left);
			s0=P_X;
			FILTER_UPLEFT(s0);
			FILTER_UP(s0);
			FILTER_LEFT(s0);
			ui64 = 
				(((H_2 + H_4+ (H_3<<1) + 2)>>2)) | 
				(((H_3 + H_5 + (H_4<<1) + 2)>>2)<<8) | 
				(((H_4 + H_6 + (H_5<<1) + 2)>>2)<<16) | 
				(((H_5 + H_7 + (H_6<<1) + 2)>>2)<<24);
			ui64 =(ui64<<24)|(((P_X + H_1 + (H_0<<1) + 2)>>2)) | 
				(((H_0 + H_2 + (H_1<<1) + 2)>>2)<<8) | 
				(((H_1 + H_3 + (H_2<<1) + 2)>>2)<<16);
			ui64 = (ui64<<8)| ((V_0 + H_0 + (P_X<<1) + 2)>>2) ;
			*(avdUInt64 *)Y4Rows0 = ui64;

			ui64 = (((V_1 + (V_0<<1) + P_X + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows1 = ui64;
			ui64 = (((V_2 + (V_1<<1) + V_0 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows2 = ui64;
			ui64 = (((V_3 + (V_2<<1) + V_1 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows3 = ui64;
			ui64 = (((V_4 + (V_3<<1) + V_2 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows4 = ui64;
			ui64 = (((V_5 + (V_4<<1) + V_3 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows5 = ui64;
			ui64 = (((V_6 + (V_5<<1) + V_4 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows6 = ui64;
			ui64 = (((V_7 + (V_6<<1) + V_5 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows7 = ui64;
			break;
		case  VERT_RIGHT_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
			//LowPassForIntra8x8Pred(&(P_Z), block_available_up_left, block_available_up, block_available_left);
			s0=P_X;
			FILTER_UPLEFT(s0);
			FILTER_UP(s0);
			FILTER_LEFT(s0);
			ui64 =
				(((H_3 + H_4 + 1)>>1)) | 
				(((H_4 + H_5+ 1)>>1)<<8) | 
				(((H_5 + H_6 + 1)>>1)<<16) | 
				(((H_6 + H_7 + 1)>>1)<<24);
			ui64 = (ui64<<24)|(((H_0 + H_1 + 1)>>1)) | 
				(((H_1 + H_2 + 1)>>1)<<8) | 
				(((H_2 + H_3 + 1)>>1)<<16);
			ui64 = (ui64<<8)|((H_0 + P_X + 1)>>1) ;
			*(avdUInt64 *)Y4Rows0 = ui64;

			ui64 = (((P_X + (V_0<<1) + V_1 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows2 = ui64;
			ui64 = (((V_1 + (V_2<<1) + V_3 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows4 = ui64;
			ui64 = (((V_3 + (V_4<<1) + V_5 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows6 = ui64;

			ui64 =
				(((H_2 + H_4+ (H_3<<1) + 2)>>2)) | 
				(((H_3 + H_5 + (H_4<<1) + 2)>>2)<<8) | 
				(((H_4 + H_6 + (H_5<<1) + 2)>>2)<<16) | 
				(((H_5 + H_7 + (H_6<<1) + 2)>>2)<<24);

			ui64 = (ui64<<24)|(((P_X + H_1 + (H_0<<1) + 2)>>2)) | 
				(((H_0 + H_2 + (H_1<<1) + 2)>>2)<<8) | 
				(((H_1 + H_3 + (H_2<<1) + 2)>>2)<<16);
			ui64 = (ui64<<8)|((V_0 + H_0 + (P_X<<1) + 2)>>2);

			*(avdUInt64 *)Y4Rows1 = ui64;
			ui64=(((V_0 + (V_1<<1) + V_2 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows3 = ui64;
			ui64=(((V_2 + (V_3<<1) + V_4 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows5 = ui64;
			ui64=(((V_4 + (V_5<<1) + V_6 + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows7 = ui64;
			break;
		case  HOR_DOWN_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
			//LowPassForIntra8x8Pred(&(P_Z), block_available_up_left, block_available_up, block_available_left);
			s0=P_X;
			FILTER_UPLEFT(s0);
			FILTER_UP(s0);
			FILTER_LEFT(s0);
			ui64 =(((H_1 + (H_2<<1) + H_3 + 2)>>2)) | 
				(((H_2 + (H_3<<1) + H_4 + 2)>>2)<<8) | 
				(((H_3 + (H_4<<1) + H_5 + 2)>>2)<<16) | 
				(((H_4 + (H_5<<1) + H_6 + 2)>>2)<<24);
			ui64 = (ui64<<24)|(((V_0 + (P_X<<1) + H_0 + 2)>>2)) | 
				(((P_X + (H_0<<1) + H_1 + 2)>>2)<<8) | 
				(((H_0 + (H_1<<1) + H_2 + 2)>>2)<<16);
			ui64 = (ui64<<8)|((V_0 + P_X + 1)>>1);
			*(avdUInt64 *)Y4Rows0 = ui64;

			ui64= (((V_0 + V_1 + 1)>>1) | 
				(((P_X + (V_0<<1) + V_1 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows1 = ui64;
			ui64= (((V_1 + V_2 + 1)>>1) | 
				(((V_0 + (V_1<<1) + V_2 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows2 = ui64;
			ui64= (((V_2 + V_3 + 1)>>1) | 
				(((V_1 + (V_2<<1) + V_3 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows3 = ui64;
			ui64= (((V_3 + V_4 + 1)>>1) | 
				(((V_2 + (V_3<<1) + V_4 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows4 = ui64;
			ui64= (((V_4 + V_5 + 1)>>1) | 
				(((V_3 + (V_4<<1) + V_5 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows5 = ui64;
			ui64= (((V_5 + V_6 + 1)>>1) | 
				(((V_4 + (V_5<<1) + V_6 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows6 = ui64;
			ui64= (((V_6 + V_7 + 1)>>1) | 
				(((V_5 + (V_6<<1) + V_7 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows7 = ui64;
			break;
		case  VERT_LEFT_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
			//LowPassForIntra8x8Pred(&(P_Z), block_available_up_left, block_available_up, block_available_left);
			//DUMP_NEIGHBOR_BEFORE
			FILTER_UP(P_X)
				ui64 = (((H_7 + H_8 + 1)>>1)) | 
				(((H_8 + H_9+ 1)>>1)<<8) | 
				(((H_9 + H_10 + 1)>>1)<<16) | 
				(((H_10 + H_11 + 1)>>1)<<24);
			ui64 =(ui64<<24)|(((H_4 + H_5 + 1)>>1)) | 
				(((H_5 + H_6 + 1)>>1)<<8) | 
				(((H_6 + H_7 + 1)>>1)<<16);
			ui64 = (ui64<<8)|((H_3 + H_4 + 1)>>1);
			*(avdUInt64 *)Y4Rows6 = ui64;
			*(avdUInt64 *)Y4Rows4 =ui64= (((H_2+ H_3 + 1)>>1) | (ui64<<8));
			*(avdUInt64 *)Y4Rows2 =ui64= (((H_1+ H_2 + 1)>>1) | (ui64<<8));
			*(avdUInt64 *)Y4Rows0 =ui64= (((H_0+ H_1 + 1)>>1) | (ui64<<8));

			ui64 =	(((H_7 + H_9+ (H_8<<1) + 2)>>2)) | 
				(((H_8 + H_10 + (H_9<<1) + 2)>>2)<<8) | 
				(((H_9 + H_11 + (H_10<<1) + 2)>>2)<<16) | 
				(((H_10 + H_12 + (H_11<<1) + 2)>>2)<<24);
			ui64=(ui64<<24)|(((H_4 + H_6 + (H_5<<1) + 2)>>2)) | 
				(((H_5 + H_7 + (H_6<<1) + 2)>>2)<<8) | 
				(((H_6 + H_8 + (H_7<<1) + 2)>>2)<<16);
			ui64=(ui64<<8)|((H_3 + H_5 + (H_4<<1) + 2)>>2);
			*(avdUInt64 *)Y4Rows7 = ui64;
			*(avdUInt64 *)Y4Rows5 =ui64= (((H_2 + H_4 + (H_3<<1) + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows3 =ui64= (((H_1 + H_3 + (H_2<<1) + 2)>>2) | (ui64<<8));
			*(avdUInt64 *)Y4Rows1 =ui64= (((H_0 + H_2 + (H_1<<1) + 2)>>2) | (ui64<<8));
			break;
		case  HOR_UP_PRED:/* diagonal prediction -22.5 deg to horizontal plane */
			//LowPassForIntra8x8Pred(&(P_Z), block_available_up_left, block_available_up, block_available_left);
			FILTER_LEFT(P_X)
				ui64 =(V_7 | (V_7<<8) | (V_7<<16) | (V_7<<24));//| (V_7<<32)| (V_7<<40)| (V_7<<48)| (V_7<<56));
			ui64=(ui64<<24)|V_7 | (V_7<<8) | (V_7<<16);
			ui64=(ui64<<8)|V_7;
			*(avdUInt64 *)Y4Rows7 = ui64;
			ui64 = (((V_6+ V_7 + 1)>>1) | 
				(((V_6+ (V_7<<1) + V_7 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows6 = ui64;
			ui64 = (((V_5+ V_6 + 1)>>1) | 
				(((V_5+ (V_6<<1) + V_7 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows5 = ui64;
			ui64 = (((V_4+ V_5 + 1)>>1) | 
				(((V_4+ (V_5<<1) + V_6 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows4 = ui64;
			ui64 = (((V_3+ V_4 + 1)>>1) | 
				(((V_3+ (V_4<<1) + V_5 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows3 = ui64;
			ui64 = (((V_2+ V_3 + 1)>>1) | 
				(((V_2+ (V_3<<1) + V_4 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows2 = ui64;
			ui64 = (((V_1+ V_2 + 1)>>1) | 
				(((V_1+ (V_2<<1) + V_3 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows1 = ui64;
			ui64 = (((V_0+ V_1 + 1)>>1) | 
				(((V_0+ (V_1<<1) + V_2 + 2)>>2)<<8) | (ui64<<16));
			*(avdUInt64 *)Y4Rows0 = ui64;
			break;
		default:

			AVD_ERROR_CHECK(img,"intrapred:there is not predict mode",ERROR_IntraModeIsNULL);	
			return SEARCH_SYNC;
		}
#if (DUMP_VERSION & DUMP_SLICE)
		//Dumploop(DUMP_SLICE,"Intra8_Neighbor",25,&(P_Z),DDT_D8);
		if(DUMP_I8x8)
			Dumploop2(DUMP_SLICE, info,"Intra8_Pred",8,8,Y4Rows0,DDT_D8,img->sizeInfo->yPlnPitch);
#endif//(DUMP_VERSION & DUMP_SLICE)
		TIME_END(start,intra8x8Sum)	
		return DECODING_OK;
	}
#endif//FEATURE_T8x8
	/*!
	***********************************************************************
	* \brief
	*    invers  transform
	***********************************************************************
	*/
	void itrans_2(ImageParameters *img,TMBsProcessor *info) //!< image parameters
	{
		avdInt16 *M4;
		avdUInt32 iTransFlags;	
		avdNativeInt a0, a1, a2, a3;
		avdNativeInt i,j;
		avdNativeInt qp_per, qp_rem, dqCoeff,dqCoeff2;
		TMBBitStream	*mbBits = GetMBBits(info->currMB);
		M4 = GetMBCofBuf(info);
		// horizontal
		j = 4;
		do {
			a0 = M4[0] + M4[32];
			a1 = M4[0] - M4[32];
			a2 = M4[16] - M4[48];
			a3 = M4[16] + M4[48];

			M4[0]  = a0 + a3;
			M4[16] = a1 + a2;
			M4[32] = a1 - a2;
			M4[48] = a0 - a3;
			M4 += 64; //4*4*4;
		} while (--j);

		// vertical
		qp_per = DIVIDE6[img->qp-MIN_QP];
		qp_rem = (img->qp-MIN_QP) - qp_per * 6;
#if FEATURE_MATRIXQUANT
		if(img->initMatrixDequantDone)
		{
			TDequantArray_coef* InvLevelScale4x4_Intra	=(TDequantArray_coef*)img->dequant_coef_shift;
			dqCoeff2 =(*InvLevelScale4x4_Intra)[0][qp_rem][0][0];
		}
		else
#endif
			dqCoeff = (dequant_coef[DEQUANT_IDX(qp_rem,0,0)]<<qp_per);
		i = 4;
		M4 -= (64<<2); // go back to (avdNativeInt *)&mbBits->cof[0][0][0][0];
		iTransFlags = mbBits->iTransFlags;
		do {
			a0 = M4[0] + M4[128];
			a1 = M4[0] - M4[128];
			a2 = M4[64] - M4[192];
			a3 = M4[64] + M4[192];
#if FEATURE_MATRIXQUANT
			if(img->initMatrixDequantDone)
			{

#define rshift_rnd(x,a) ((x + (1 << (a-1) )) >> a)
				dqCoeff = ((a0+a3)*dqCoeff2)<<qp_per;
				if ((M4[0]   = rshift_rnd(dqCoeff,6)) != 0)
					iTransFlags |= (1<<(4-i));
				dqCoeff = ((a1+a2)*dqCoeff2)<<qp_per;
				if ((M4[64]  = rshift_rnd(dqCoeff,6)) != 0)
					iTransFlags |= (1<<(8-i));
				dqCoeff = ((a1-a2)*dqCoeff2)<<qp_per;
				if ((M4[128] = rshift_rnd(dqCoeff,6)) != 0)
					iTransFlags |= (1<<(12-i));
				dqCoeff = ((a0-a3)*dqCoeff2)<<qp_per;
				if ((M4[192] = rshift_rnd(dqCoeff,6)) != 0)
					iTransFlags |= (1<<(16-i));
			}
			else
#endif
			{


				if ((M4[0]   = (((a0+a3)*dqCoeff)+2)>>2) != 0)
					iTransFlags |= (1<<(4-i));
				if ((M4[64]  = (((a1+a2)*dqCoeff)+2)>>2) != 0)
					iTransFlags |= (1<<(8-i));
				if ((M4[128] = (((a1-a2)*dqCoeff)+2)>>2) != 0)
					iTransFlags |= (1<<(12-i));
				if ((M4[192] = (((a0-a3)*dqCoeff)+2)>>2) != 0)
					iTransFlags |= (1<<(16-i));
			}

			M4 += 16; //4 * 4;
		} while (--i);
		mbBits->iTransFlags = iTransFlags;
	}

	/*!
	***********************************************************************
	* \brief
	*    Inverse 4x4 transformation, transforms cof to m7
	***********************************************************************
	*/



	/*!
	***********************************************************************
	* \brief
	*    The routine performs transform,quantization,inverse transform, adds the diff.
	*    to the prediction and writes the result to the decoded luma frame. Includes the
	*    RD constrained quantization also.
	*
	* \par Input:
	*    block_x,block_y: Block position inside a macro block (0,4,8,12).
	*
	* \par Output:
	*    nonzero: 0 if no levels are nonzero.  1 if there are nonzero levels. \n
	*    coeff_cost: Counter for nonzero coefficients, used to discard expencive levels.
	************************************************************************
	*/


