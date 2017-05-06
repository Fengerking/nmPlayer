

#include "global.h"
#include "memalloc.h"
//#include "mbuffer.h"



byte** new_mem2D(H264DEC_G *pDecGlobal, int dim0, int dim1)
{
  int i;
  byte **array2D;

  if((  array2D  = (byte**)voMalloc(pDecGlobal,dim0 *      sizeof(byte*))) == NULL)
    no_mem_exit("get_mem2D: array2D");
  if((*(array2D) = (byte* )voMalloc(pDecGlobal,dim0 * dim1*sizeof(byte ))) == NULL)
    no_mem_exit("get_mem2D: array2D");

  for(i = 1; i < dim0; i++)
    array2D[i] = array2D[i-1] + dim1;

  return (array2D);
}


int get_mem2D(H264DEC_G *pDecGlobal, byte ***array2D, int dim0, int dim1)
{
  int i;

  if((  *array2D  = (byte**)voMalloc(pDecGlobal,dim0 *      sizeof(byte*))) == NULL)
    no_mem_exit("get_mem2D: array2D");
  if((*(*array2D) = (byte* )voMalloc(pDecGlobal,dim0 * dim1*sizeof(byte ))) == NULL)
    no_mem_exit("get_mem2D: array2D");

  for(i = 1; i < dim0; i++)
    (*array2D)[i] = (*array2D)[i-1] + dim1;

  return dim0 * (sizeof(byte*) + dim1 * sizeof(byte));
}



int** new_mem2Dint(H264DEC_G *pDecGlobal, int dim0, int dim1)
{
  int i;
  int **array2D;

  if((array2D    = (int**)voMalloc(pDecGlobal,dim0 *        sizeof(int*))) == NULL)
    no_mem_exit("get_mem2Dint: array2D");
  if((*(array2D) = (int* )voMalloc(pDecGlobal,dim0 * dim1* sizeof(int ))) == NULL)
    no_mem_exit("get_mem2Dint: array2D");

  for(i = 1 ; i < dim0; i++)
    (array2D)[i] =  (array2D)[i-1] + dim1;

  return (array2D);
}


int get_mem2Dint(H264DEC_G *pDecGlobal, int ***array2D, int dim0, int dim1)
{
  int i;

  if((*array2D    = (int**)voMalloc(pDecGlobal,dim0 *       sizeof(int*))) == NULL)
    no_mem_exit("get_mem2Dint: array2D");
  if((*(*array2D) = (int* )voMalloc(pDecGlobal,dim0 * dim1* sizeof(int ))) == NULL)
    no_mem_exit("get_mem2Dint: array2D");

  for(i = 1 ; i < dim0; i++)
    (*array2D)[i] =  (*array2D)[i-1] + dim1;

  return dim0 * (sizeof(int*) + dim1 * sizeof(int));
}



int get_mem3D(H264DEC_G *pDecGlobal, byte ****array3D, int dim0, int dim1, int dim2)
{
  int  i, mem_size = dim0 * sizeof(byte**);

  if(((*array3D) = (byte***)voMalloc(pDecGlobal,dim0 * sizeof(byte**))) == NULL)
    no_mem_exit("get_mem3D: array3D");

  mem_size += get_mem2D(pDecGlobal,*array3D, dim0 * dim1, dim2);

  for(i = 1; i < dim0; i++)
    (*array3D)[i] =  (*array3D)[i-1] + dim1;

  return mem_size;
}


int get_mem3Dint(H264DEC_G *pDecGlobal, int ****array3D, int dim0, int dim1, int dim2)
{
  int  i, mem_size = dim0 * sizeof(int**);

  if(((*array3D) = (int***)voMalloc(pDecGlobal,dim0 * sizeof(int**))) == NULL)
    no_mem_exit("get_mem3Dint: array3D");

  mem_size += get_mem2Dint(pDecGlobal,*array3D, dim0 * dim1, dim2);

  for(i = 1; i < dim0; i++)
    (*array3D)[i] =  (*array3D)[i-1] + dim1;

  return mem_size;
}


int get_mem4Dint(H264DEC_G *pDecGlobal, int *****array4D, int dim0, int dim1, int dim2, int dim3)
{
  int  i, mem_size = dim0 * sizeof(int***);

  if(((*array4D) = (int****)voMalloc(pDecGlobal,dim0 * sizeof(int***))) == NULL)
    no_mem_exit("get_mem4Dint: array4D");

  mem_size += get_mem3Dint(pDecGlobal,*array4D, dim0 * dim1, dim2, dim3);

  for(i = 1; i < dim0; i++)
    (*array4D)[i] =  (*array4D)[i-1] + dim1;

  return mem_size;
}



void free_mem2D(H264DEC_G *pDecGlobal, byte **array2D)
{
  if (array2D)
  {
    if (*array2D)
      voFree (pDecGlobal,*array2D);
    else 
      error ("free_mem2D: trying to voFree unused memory",100);

    voFree (pDecGlobal,array2D);
  } 
  else
  {
    error ("free_mem2D: trying to voFree unused memory",100);
  }
}


void free_mem2Dint(H264DEC_G *pDecGlobal, int **array2D)
{
  if (array2D)
  {
    if (*array2D)
      voFree (pDecGlobal,*array2D);
    else 
      error ("free_mem2Dint: trying to voFree unused memory",100);

    voFree (pDecGlobal,array2D);
  } 
  else
  {
    error ("free_mem2Dint: trying to voFree unused memory",100);
  }
}



void free_mem3D(H264DEC_G *pDecGlobal, byte ***array3D)
{
  if (array3D)
  {
   free_mem2D(pDecGlobal,*array3D);
   voFree (pDecGlobal,array3D);
  } 
  else
  {
    error ("free_mem3D: trying to voFree unused memory",100);
  }
}


void free_mem4D(H264DEC_G *pDecGlobal, byte ****array4D)
{
  if (array4D)
  {
   free_mem3D(pDecGlobal,*array4D);
   voFree (pDecGlobal,array4D);
  } 
  else
  {
    error ("free_mem4D: trying to voFree unused memory",100);
  }
}


void free_mem3Dint(H264DEC_G *pDecGlobal, int ***array3D)
{
  if (array3D)
  {
   free_mem2Dint(pDecGlobal,*array3D);
   voFree (pDecGlobal,array3D);
  } 
  else
  {
    error ("free_mem3Dint: trying to voFree unused memory",100);
  }
}


void free_mem4Dint(H264DEC_G *pDecGlobal, int ****array4D)
{
  if (array4D)
  {
    free_mem3Dint(pDecGlobal, *array4D);
    voFree (pDecGlobal,array4D);
  } else
  {
    error ("free_mem4Dint: trying to voFree unused memory",100);
  }
}

void no_mem_exit(char *where)
{
   //snprintf(errortext, ET_SIZE, "Could not allocate memory: %s",where);
   error ("Could not allocate memory", 100);
}


#define H264_MEM_ALIGN 128
#define H264_MEM_ALIGN_MASK (H264_MEM_ALIGN-1)

void *voH264AlignedMalloc(H264DEC_G *pDecGlobal, int size)
{
// 	static int debugID=0;

	VO_U8 advance;
	VO_U8 *realPt, *alignedPt;
	if(size<=0)
	{
		return NULL;
	}
	size += H264_MEM_ALIGN;

	if(pDecGlobal->customMem2&&pDecGlobal->customMem2->Alloc4)
	{
		VO_MEM_INFO voMemInfo={0}; 
		voMemInfo.Size=size; 
		pDecGlobal->customMem2->Alloc4(VO_INDEX_DEC_H264, (VOMEM_INFO *)&voMemInfo); 
		realPt=(VO_U8 *)voMemInfo.VBuffer; 
		memset(realPt,0,size);
	}
    else if ((realPt = (VO_U8 *)calloc(1,size)) == NULL)
	{
		no_mem_exit("voH264AlignedMalloc");
	}
#if MEMPRINT
	printf("alloc %p:%d\n", realPt, size);
#endif
	advance = (VO_U8)(H264_MEM_ALIGN - ((VO_U32)realPt & H264_MEM_ALIGN_MASK));
	//assert(advance > 0);
	alignedPt = realPt + advance; // to aligned location;
	*(alignedPt - 1) = advance; // save real voMalloc pt at alignedPt[-1] location for voFree;
	return alignedPt;
}

void voH264AlignedFree(H264DEC_G *pDecGlobal, void *alignedPt)
{
	VO_U8 *realPt;
	if(!alignedPt || ((VO_U32)alignedPt & H264_MEM_ALIGN_MASK))
		return;
	realPt = (VO_U8 *)alignedPt - *((VO_U8 *)alignedPt - 1);
	//printf("%x,%d\r\n",realPt,*((VO_U8 *)alignedPt - 1));
    if(pDecGlobal->customMem2&&pDecGlobal->customMem2->Free4)
		pDecGlobal->customMem2->Free4(VO_INDEX_DEC_H264,realPt);
	else	
	    free(realPt); // voFree real voMalloc location;
#if MEMPRINT
	printf("free %p\n", realPt);
#endif
}

