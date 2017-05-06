
/*!
 ************************************************************************
 * \file  memalloc.c
 *
 * \brief
 *    Memory allocation and free helper funtions
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 ************************************************************************
 */
#include "defines.h"

#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#endif
#include "memalloc.h"

//#include "voMem.h"
#define H264_MODULE_ID VO_INDEX_DEC_H264
#define H264_MEM_ALIGN 32
#define H264_MEM_ALIGN_MASK 31 
#define H264_MEM_ALIGN16 16
#define H264_MEM_ALIGN_MASK16 15
#if !USE_GLOBALE_MEM
void *voH264AlignedMalloc(ImageParameters *img,int id2, int size)
{
	static int debugID=0;
	
	avdUInt8 advance;
	avdUInt8 *realPt, *alignedPt;
	H264VdLibParam* params = img->vdLibPar;
	if(size<=0)
	{
		AvdLog2(LL_INFO,"voH264AlignedMalloc error");
		return NULL;
	}
	size += H264_MEM_ALIGN + PADDING_LEN;

	if(params->customMem2&&params->customMem2->Alloc4)
	{
		VO_MEM_INFO voMemInfo={0}; 
			voMemInfo.Size=size; 
			params->customMem2->Alloc4(H264_MODULE_ID, (VOMEM_INFO *)&voMemInfo); 
			realPt=(avdUInt8 *)voMemInfo.VBuffer; 
			memset(realPt,0,size);
	}
	else if ((realPt = (avdUInt8 *)calloc(1,size)) == NULL)
	{
    	no_mem_exit("voH264AlignedMalloc");
		AVD_ERROR_CHECKNULL(img,"Could not allocate memory", ERROR_MEMExit);
	}

	

	advance = (avdUInt8)(H264_MEM_ALIGN - ((avdInt32)realPt & H264_MEM_ALIGN_MASK));
#ifdef TRACE_MEM
	trace_new(img,debugID,realPt,size,id2);
#endif//TRACE_MEM
	//assert(advance > 0);
	alignedPt = realPt + advance; // to aligned location;
	*(alignedPt - 1) = advance; // save real malloc pt at alignedPt[-1] location for free;
#if (DUMP_VERSION & DUMP_MEMORY)
	_totalMemory += size;
	AvdLog(DUMP_MEMORY, "\n id = %d, size = %d, total = %d, mem = 0x%x", 
		id, size, _totalMemory, realPt);
#endif //(DUMP_VERSION & DUMP_MEMORY)
	AvdLog2(LL_INFO,"mem:id = %d, size = %d,%X/%X\n",debugID++,size,alignedPt,realPt);
	return alignedPt;
}

void voH264AlignedFree(ImageParameters *img,void *alignedPt)
{
	avdUInt8 *realPt;
	H264VdLibParam* params = img->vdLibPar;
	if(!IS_VALID_POINTER(alignedPt))
		return;
	realPt = (avdUInt8 *)alignedPt - *((avdUInt8 *)alignedPt - 1);
#ifdef TRACE_MEM
	trace_delete(img,realPt);
#endif//TRACE_MEM
	if(params->customMem2&&params->customMem2->Free4)
		params->customMem2->Free4(H264_MODULE_ID,realPt);
	else
		free(realPt); // free real malloc location;
#if (DUMP_VERSION & DUMP_MEMORY)
	AvdLog(DUMP_MEMORY, "\n free:                           mem = 0x%x", realPt);
#endif //(DUMP_VERSION & DUMP_MEMORY)
}
#else//USE_GLOBALE_MEM
void *voH264AlignedMalloc(ImageParameters *img,int id, int size)
{
	GMemData* mem;
	int adjust;
	avdUInt8* relPt=NULL;
	if(id!=YUV_BUF_ID)
	{
		mem = &(img->gMem);
	}
	else
	{
		mem = &(img->gMem2);
	}
	adjust=(((avdInt32)((avdUInt8*)mem->start+mem->offset) & H264_MEM_ALIGN_MASK));
	if(adjust)
		adjust=H264_MEM_ALIGN -adjust;
	mem->offset+=adjust;

	relPt=(avdUInt8*)mem->start+mem->offset;
	mem->offset+=size;
	if(mem->offset>mem->size)
	{
		AVD_ERROR_CHECK(img,"mem->offset>mem->size",-1);
	}
	return relPt;
}

void voH264AlignedFree(ImageParameters *img,void *alignedPt)
{
	
}
#endif//USE_GLOBALE_MEM
#if 1//£¡FEATURE_BFRAME//ndef VOI_INPLACE_DEBLOCKING
/*!
 ************************************************************************
 * \brief
 *    Allocate 2D memory array -> unsigned char array2D[rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************/
// Change 9-Aug-2001 P. List: dont allocate independant row arrays anymore
// but one complete array and move row-pointers to array. Now you can step
// to the next line with an offset of sizeInfo->width
avdInt32 get_mem2D(ImageParameters *img,avdUInt8 ***array2D, avdNativeInt rows, avdNativeInt columns)
{
	avdNativeInt i;
	avdInt32 size = ((avdInt32)columns) * ((avdInt32)rows) * sizeof(avdUInt8);
	if(size<=0)
	{
		AvdLog2(LL_INFO,"get_mem2D error");
		return 0;
	}


	*array2D      = (avdUInt8**)voH264AlignedMalloc(img,50, rows * sizeof(avdUInt8*));CHECK_ERR_RET_INT
	(*array2D)[0] = (avdUInt8* )voH264AlignedMalloc(img,51, size);CHECK_ERR_RET_INT

	for(i=1;i<rows;i++)
		(*array2D)[i] = (*array2D)[i-1] + columns ;

	return size;
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 3D memory array -> unsigned char array3D[frames][rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
// same change as in get_mem2Dint
avdInt32 get_mem3D(ImageParameters *img,avdUInt8 ****array3D, avdNativeInt frames, avdNativeInt rows, avdNativeInt columns)
{
	avdNativeInt i, j;
	avdInt32 size = frames * rows * columns * sizeof(avdUInt8);
	avdUInt8 *buf;
	if(size<=0)
	{
		AvdLog2(LL_INFO,"get_mem3D");
		return NULL;
	}
	buf = (avdUInt8 *)voH264AlignedMalloc(img,52, size);
	if (!buf)
		no_mem_exit("voH264AlignedMalloc: array3D");
	CHECK_ERR_RET_INT
#ifdef _DEBUG
	if (columns&3){
		printf("\nWarning: 3D array not aligned");
	}
#endif
	
	*array3D  = (avdUInt8***)voH264AlignedMalloc(img,53, frames * sizeof(avdUInt8**));
	if (!array3D)
		no_mem_exit("voH264AlignedMalloc: array3D");
	CHECK_ERR_RET_INT
	for(i = 0; i < frames; i++){
		if(((*array3D)[i] = (avdUInt8**)voH264AlignedMalloc(img,54, rows * sizeof(avdUInt8*))) == NULL)
			no_mem_exit("voH264AlignedMalloc: array3D");
		CHECK_ERR_RET_INT
		for (j = 0; j < rows; j++){
			(*array3D)[i][j] = buf;
			buf += columns;
		}
	}
	return size;
}

/*!
 ************************************************************************
 * \brief
 *    free 2D memory array
 *    which was alocated with get_mem2D(img,)
 ************************************************************************
 */
void free_mem2D(ImageParameters *img,avdUInt8 **array2D)
{
	if (array2D){
		SafevoH264AlignedFree(img,array2D[0]);
		SafevoH264AlignedFree(img,array2D);
	} 
}

/*!
 ************************************************************************
 * \brief
 *    free 3D memory array
 *    which was alocated with get_mem3D(img,)
 ************************************************************************
 */
void free_mem3D(ImageParameters *img,avdUInt8 ***array3D, avdNativeInt frames)
{
	avdNativeInt i;
	if (array3D)
	{
		SafevoH264AlignedFree(img,array3D[0][0]);
		for(i = 0; i < frames; i++)
		{
			SafevoH264AlignedFree(img,array3D[i]);
		}
		SafevoH264AlignedFree(img,array3D);
	} 
}
#endif//!VOI_INPLACE_DEBLOCKING
/*!
 ************************************************************************
 * \brief
 *    Exit program if memory allocation failed (using error())
 * \param where
 *    string indicating which memory allocation failed
 ************************************************************************
 */
void no_mem_exit(char *where)
{
    //char errortext[ET_SIZE];
	//snprintf(errortext, ET_SIZE, "Could not allocate memory: %s",where);
	//AVD_ERROR_CHECK(img,"Could not allocate memory", ERROR_MEMExit);
	AvdLog2(LL_INFO,"no_mem_exit");
}

