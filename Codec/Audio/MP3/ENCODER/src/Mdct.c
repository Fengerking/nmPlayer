#include "Frame.h"

void mdct_long(int *out, int *in)
{
	int ct,st;
	int tc1, tc2, tc3, tc4, ts5, ts6, ts7, ts8;
	int ts1, ts2, ts3, ts4, tc5, tc6, tc7, tc8;
	
	/* 1,2, 5,6, 9,10, 13,14, 17 */
	tc1 = in[17] - in[ 9];
	tc3 = in[15] - in[11];
	tc4 = in[14] - in[12];
	ts5 = in[ 0] + in[ 8];
	ts6 = in[ 1] + in[ 7];
	ts7 = in[ 2] + in[ 6];
	ts8 = in[ 3] + in[ 5];
	
	out[17] = (ts5 + ts7 - ts8) * cx[8] - (ts6 - in[4]) * cx[8];
	st      = (ts5 + ts7 - ts8) * cx[7] + (ts6 - in[4]) * cx[8];
	ct      = (tc1 - tc3 - tc4) * cx[6];
	out[5]  = ct + st;
	out[6]  = ct - st;
    
	tc2     = (in[16] - in[10]) * cx[6];
	ts6     =  ts6 * cx[7] + in[4] * cx[8];
	
	ct      =  tc1 * cx[0] + tc2 + tc3 * cx[1] + tc4 * cx[2];
	st      = -ts5 * cx[4] + ts6 - ts7 * cx[5] + ts8 * cx[3];
	out[1]  = ct + st;
	out[2]  = ct - st;
    
	ct      =  tc1 * cx[1] - tc2 - tc3 * cx[2] + tc4 * cx[0];
	st      = -ts5 * cx[5] + ts6 - ts7 * cx[3] + ts8 * cx[4];
	out[ 9] = ct + st;
	out[10] = ct - st;
    
	ct      = tc1 * cx[2] - tc2 + tc3 * cx[0] - tc4 * cx[1];
	st      = ts5 * cx[3] - ts6 + ts7 * cx[4] - ts8 * cx[5];
	out[13] = ct + st;
	out[14] = ct - st;
	
	ts1 = in[ 8] - in[ 0];
	ts3 = in[ 6] - in[ 2];
	ts4 = in[ 5] - in[ 3];
	tc5 = in[17] + in[ 9];
	tc6 = in[16] + in[10];
	tc7 = in[15] + in[11];
	tc8 = in[14] + in[12];
	
	out[0]  = (tc5 + tc7 + tc8) * cx[8] + (tc6 + in[13]) * cx[8];
	ct      = (tc5 + tc7 + tc8) * cx[7] - (tc6 + in[13]) * cx[8];
	st      = (ts1 - ts3 + ts4) * cx[6];
	out[11] = ct + st;
	out[12] = ct - st;
	
	ts2     = (in[7] - in[1]) * cx[6];
	tc6     = in[13] * cx[8] - tc6 * cx[7];
	
	ct      = tc5 * cx[3] - tc6 + tc7 * cx[4] + tc8 * cx[5];
	st      = ts1 * cx[2] + ts2 + ts3 * cx[0] + ts4 * cx[1];
	out[3]  = ct + st;
	out[4]  = ct - st;
	
	ct      =-tc5 * cx[5] + tc6 - tc7 * cx[3] - tc8 * cx[4];
	st      = ts1 * cx[1] + ts2 - ts3 * cx[2] - ts4 * cx[0];
	out[7]  = ct + st;
	out[8]  = ct - st;
	
	ct      =-tc5 * cx[4] + tc6 - tc7 * cx[5] - tc8 * cx[3];
	st      = ts1 * cx[0] - ts2 + ts3 * cx[1] - ts4 * cx[2];
	out[15] = ct + st;
	out[16] = ct - st;
}