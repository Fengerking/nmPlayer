/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _CONST_TAB_H
#define _CONST_TAB_H

#include "../voMpegProtect.h"
#include "voType.h"

/************************************** zigzag start ********************/
extern VO_U16 const scan_tables[3][64];
//VO_VOID InitContTab();
/************************************** zigzag end ********************/

/************************************** VLC Start ********************/
#define VLC_ERROR	(-1)

#define ESCAPE1 6
#define ESCAPE2 14
#define ESCAPE3 15

/******************************************************************
 * common tables of encoder											*
 ******************************************************************/
extern VO_U8	const *Clip255_table;

extern VO_U16 const mcbpc_intra_table[15];
extern VO_U16 const mcbpc_inter_table[29];
extern VO_U16 const cbp_lunina_table[16];
extern VO_U16 const dc_lumina_table[511];
extern VO_U16 const dc_chroma_table[511];
extern VO_U16 const mb_motion_table[65];
extern VO_U16 const h263_motion_table[65];

/************************************** VLC End ********************/
#endif