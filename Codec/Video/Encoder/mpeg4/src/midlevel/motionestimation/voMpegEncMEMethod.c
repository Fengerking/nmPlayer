/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include <stdio.h>

#include "../../voMpegEnc.h"
#include "voMpegEncMEMethod.h"
#include "voMpegEncME.h"
#include "../../lowlevel/voMpegEncSad.h"
#include "../../lowlevel/voMpegEncHPInterPolate.h"

// K = 4 
const VO_U32 round_tab_76[16] ={ 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1 };
// K = 1 
const VO_U32 round_tab_79[4] ={ 0, 1, 0, 0 };

const VO_S32 me_lambda_vec16[32] = 
{
	0, 1, 1 ,2, 2, 3, 4 ,4,
	5, 5, 6, 7, 7, 8, 8, 9, 
	10, 10, 11, 11, 12, 13, 13, 14,
	14, 15, 16, 16, 17, 17, 18, 19
};

VO_VOID DiamondSearch(VO_S32 x, VO_S32 y, ME_DATA * const me_data,
						 VO_S32 direction, VoCheckFunc * const DirectionCheck )
{
	//1 -- left	//2 -- right 	//4 -- up //8 -- down

	for(;;) { 
        me_data->dir = 0;
        //check four apex
        switch( direction&0xf )
        {
        case 15:
            DirectionCheck(x - VODIAMONDSIZE, y, me_data, 1 );
			DirectionCheck(x + VODIAMONDSIZE, y, me_data, 2 );
			DirectionCheck(x, y - VODIAMONDSIZE, me_data, 4 );
			DirectionCheck(x, y + VODIAMONDSIZE, me_data, 8 );
            break;
        case 14:
            DirectionCheck(x + VODIAMONDSIZE, y, me_data, 2 );
			DirectionCheck(x, y - VODIAMONDSIZE, me_data, 4 );
			DirectionCheck(x, y + VODIAMONDSIZE, me_data, 8 );
            break;
        case 13:
            DirectionCheck(x - VODIAMONDSIZE, y, me_data, 1 );
			DirectionCheck(x, y - VODIAMONDSIZE, me_data, 4 );
			DirectionCheck(x, y + VODIAMONDSIZE, me_data, 8 );
            break;
        case 12:
			DirectionCheck(x, y - VODIAMONDSIZE, me_data, 4 );
			DirectionCheck(x, y + VODIAMONDSIZE, me_data, 8 );
            break;
        case 11:
            DirectionCheck(x - VODIAMONDSIZE, y, me_data, 1 );
			DirectionCheck(x + VODIAMONDSIZE, y, me_data, 2 );
			DirectionCheck(x, y + VODIAMONDSIZE, me_data, 8 );
            break;
        case 10:
			DirectionCheck(x + VODIAMONDSIZE, y, me_data, 2 );
			DirectionCheck(x, y + VODIAMONDSIZE, me_data, 8 );
            break;
        case 9:
            DirectionCheck(x - VODIAMONDSIZE, y, me_data, 1 );
			DirectionCheck(x, y + VODIAMONDSIZE, me_data, 8 );
            break;
        case 8:
            DirectionCheck(x, y + VODIAMONDSIZE, me_data, 8 );
            break;
        case 7:
            DirectionCheck(x - VODIAMONDSIZE, y, me_data, 1 );
			DirectionCheck(x + VODIAMONDSIZE, y, me_data, 2 );
			DirectionCheck(x, y - VODIAMONDSIZE, me_data, 4 );
            break;
        case 6:
            DirectionCheck(x + VODIAMONDSIZE, y, me_data, 2 );
			DirectionCheck(x, y - VODIAMONDSIZE, me_data, 4 );
            break;
        case 5:
            DirectionCheck(x - VODIAMONDSIZE, y, me_data, 1 );
			DirectionCheck(x, y - VODIAMONDSIZE, me_data, 4 );
            break;
        case 4:
            DirectionCheck(x, y - VODIAMONDSIZE, me_data, 4 );
            break;
        case 3:
            DirectionCheck(x - VODIAMONDSIZE, y, me_data, 1 );
			DirectionCheck(x + VODIAMONDSIZE, y, me_data, 2 );
            break;
        case 2:
            DirectionCheck(x + VODIAMONDSIZE, y, me_data, 2 );
            break;
        case 1:
            DirectionCheck(x - VODIAMONDSIZE, y, me_data, 1 );
            break;
        }

        if(me_data->dir) 
        {
			direction = me_data->dir;
            me_data->dir =0;
			x = me_data->current_best_MV->x; 
			y = me_data->current_best_MV->y;

			if (direction & 3) { //( 1,3,) our candidate is left or right 
				DirectionCheck(x, y + VODIAMONDSIZE, me_data,8 );
				DirectionCheck(x, y - VODIAMONDSIZE, me_data,4 );
			} else { // what remains here is up or down 
				DirectionCheck(x + VODIAMONDSIZE, y, me_data,2 );
				DirectionCheck(x - VODIAMONDSIZE, y, me_data,1 );
			}

			if (me_data->dir) {
				direction += me_data->dir;
				x = me_data->current_best_MV->x;
                y = me_data->current_best_MV->y;
			}
		}
        else 
        {
			switch (direction)  //ckeck the  diagonal
            {
			case 2:
				DirectionCheck(x + VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,2 + 4 );
				DirectionCheck(x + VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,2 + 8 );
				break;
			case 1:
				DirectionCheck(x - VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,1 + 4 );
				DirectionCheck(x - VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,1 + 8 );
				break;
			case 2 + 4:
				DirectionCheck(x - VODIAMONDSIZE,  y - VODIAMONDSIZE, me_data,1 + 4 );
				DirectionCheck(x + VODIAMONDSIZE,  y - VODIAMONDSIZE, me_data,2 + 4 );
				DirectionCheck(x + VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,2 + 8 );
				break;
			case 4:
				DirectionCheck(x + VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,2 + 4 );
				DirectionCheck(x - VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,1 + 4 );
				break;
			case 8:
				DirectionCheck(x + VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,2 + 8 );
				DirectionCheck(x - VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,1 + 8 );
				break;
			case 1 + 4:
				DirectionCheck(x - VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,1 + 8 );
				DirectionCheck(x - VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,1 + 4 );
				DirectionCheck(x + VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,2 + 4 );
				break;
			case 2 + 8:
				DirectionCheck(x + VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,2 + 4 );
				DirectionCheck(x + VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,2 + 8 );
				DirectionCheck(x - VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,1 + 8 );
				break;
			case 1 + 8:
				DirectionCheck(x - VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,1 + 4 );
				DirectionCheck(x - VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,1 + 8 );
				DirectionCheck(x + VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,2 + 8 );
				break;
			default:		// 1+2+4+8 == we didn't find anything at all 
#ifndef SKIP_ME1
				DirectionCheck(x - VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,1 + 4 );
				DirectionCheck(x - VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,1 + 8 );
				DirectionCheck(x + VODIAMONDSIZE, y - VODIAMONDSIZE, me_data,2 + 4 );
				DirectionCheck(x + VODIAMONDSIZE, y + VODIAMONDSIZE, me_data,2 + 8 );
#endif
				break;
			}
			if (!me_data->dir) 
				break;		// ok, the end 
			direction = me_data->dir;//*ptr_direction;
			x = me_data->current_best_MV->x;
			y = me_data->current_best_MV->y;
		}
	}
}

static VO_S32 halfpel_x[8] = { 0,  0, -1,  1, -1,  1,  1, -1}; 
static VO_S32 halfpel_y[8] = {-1,  1,  0,  0, -1, -1,  1,  1}; 
static VO_S32 halfpel_start[8] = {0, 24, 24*17, 24*17+1, 24*17*2, 24*17*2+1, 24*17*2+25, 24*17*2+24};

VO_VOID HalfPixelRefine(ME_DATA * const data, VO_U32 mb_threshhold)
{
	VO_S32 x = 0, y = 0, xc = 0, yc = 0, i;
	VO_U32 t=0; 
	VECTOR * cur_mv = data->current_best_MV;
	const VECTOR fullpel_mv = cur_mv[0];	
    VO_U32 sad =0;
    VO_U32 tmp_sad[4];
	//VO_U32 pred_sad_h=0, pred_sad_v=0;
	VO_U32 sad16 = data->iMinSAD[0];
	VO_U8 *ref = data->RefY + ((fullpel_mv.y>>1) * data->iEdgedWidth + (fullpel_mv.x>>1));

	Inplace16InterpolateHP[data->rounding](data->ref_h,    
                                                          data->ref_v, 
                                                          data->ref_hv,
                                                          ref, 24, 
                                                          data->iEdgedWidth, 
                                                          data->rounding );

	for(i = 0; i < 8; i++){
        if(sad16==data->iMinSAD[0] && i>3)
            return;
        xc = x = fullpel_mv.x + halfpel_x[i];
        yc = y = fullpel_mv.y + halfpel_y[i];
		
        if (!((x>data->max_dx)||(x<data->min_dx)||(y>data->max_dy)||(y<data->min_dy))){ 
            VO_U8 *cur = data->current_mb;
            VO_U8 *ref = data->ref_v + halfpel_start[i];
			//v-0
            sad = Sad16_4MV(cur, ref, 16, 24, tmp_sad);			
			t = CalculateMvBits(x, y, data->predMV, data->iFcode);			
			sad += (data->lambda16 * t);	
			if (sad < data->iMinSAD[0]){ 				
				if (data->chroma) {
					sad += CheckChromaSAD((xc >> 1) + round_tab_79[xc & 0x3],
						(yc >> 1) + round_tab_79[yc & 0x3], data);
                    if (sad < data->iMinSAD[0]) {
                        data->iMinSAD[0] = sad;
                        cur_mv[0].x = x; cur_mv[0].y = y;
                    }
				}			

                data->iMinSAD[0] = sad;
                cur_mv[0].x = x; cur_mv[0].y = y;               
			}
            UPDATAMEDATA(data,tmp_sad,cur_mv);
		}
		//h-1
#ifdef SKIP_ME1
		if (data->iMinSAD[0] < mb_threshhold) { 
			return; 
		}
#endif
	}
}

// it's the positive max, so "32" needs fcode of 2, not 1 
VO_U32 GetMinFcode(const VO_S32 MVmax)
{
	VO_U32 fcode;
	for (fcode = 1; (16 << fcode) <= MVmax; fcode++);
	return fcode;
}

