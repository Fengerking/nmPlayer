//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    window.c

Abstract:

    window functions c file.

Author:

    Witten Wen 10-October-2009

Revision History:

*************************************************************************/

#include "window.h"
#include "tables.h"


const void *VorbisWindow(int type, int left)
{

	if(type==0)
	{
	switch(left)
		{
		case 32:
			return voWin64;
		case 64:
			return voWin128;
		case 128:
			return voWin256;
		case 256:
			return voWin512;
		case 512:
			return voWin1024;
		case 1024:
			return voWin2048;
		case 2048:
			return voWin4096;
		case 4096:
			return voWin8192;
		default:
			return(0);
		}
	}

	return(0);
}

void VorbisApplyWindow(OGG_S32 *d, const void *window_p[2],
			  long *blocksizes,
			  int lW, int W, int nW)
{

	LOOKUP_T *window[2] = {window_p[0], window_p[1]};  
	int i=0, p=0;

	long n  = blocksizes[W];
	long ln = blocksizes[lW];
	long rn = blocksizes[nW];

	long leftbegin = (n - ln)>>2;
	long leftend   = leftbegin + (ln>>1);

	

	if(lW==W&&W==nW){
		//symmetrical window case	
		for(i=0; i<n/2; i++){
			d[i] = MULT31(d[i], window[W][i]);
			d[n-i-1] = MULT31(d[n-i-1], window[W][i]);
		}

	}else
	{

		long rightbegin = (n>>1) + ((n - rn)>>2);
		long rightend   = rightbegin + (rn>>1);

		while(i<leftbegin){
			d[i++] = 0;    
		}

		for(p=0; i<leftend; i++,p++)
			d[i] = MULT31(d[i], window[lW][p]);

		for(i=rightbegin,p=rn/2-1; i<rightend; i++,p--)
			d[i] = MULT31(d[i], window[nW][p]);

		while(i<n){
			d[i++] = 0;    
		}
	}
}

