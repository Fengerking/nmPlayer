#ifndef _MEMALLOC_H_
#define _MEMALLOC_H_

#include "global.h"


extern byte** new_mem2D(H264DEC_G *pDecGlobal, int dim0, int dim1);
extern int  get_mem2D(H264DEC_G *pDecGlobal, byte ***array2D, int dim0, int dim1);
extern int  get_mem3D(H264DEC_G *pDecGlobal, byte ****array3D, int dim0, int dim1, int dim2);

extern int** new_mem2Dint(H264DEC_G *pDecGlobal, int dim0, int dim1);
extern int  get_mem2Dint(H264DEC_G *pDecGlobal, int ***array2D, int rows, int columns);
extern int  get_mem3Dint(H264DEC_G *pDecGlobal, int ****array3D, int frames, int rows, int columns);
extern int  get_mem4Dint(H264DEC_G *pDecGlobal, int *****array4D, int idx, int frames, int rows, int columns );


extern void free_mem2D     (H264DEC_G *pDecGlobal, byte      **array2D);
extern void free_mem3D     (H264DEC_G *pDecGlobal, byte     ***array3D);

extern void free_mem2Dint  (H264DEC_G *pDecGlobal, int       **array2D);
extern void free_mem3Dint  (H264DEC_G *pDecGlobal, int      ***array3D);
extern void free_mem4Dint  (H264DEC_G *pDecGlobal, int     ****array4D);

extern void no_mem_exit(char *where);

#define voMalloc voH264AlignedMalloc
#define voFree voH264AlignedFree

extern void *voH264AlignedMalloc(H264DEC_G *pDecGlobal, int size);
extern void voH264AlignedFree(H264DEC_G *pDecGlobal, void *alignedPt);

#endif
