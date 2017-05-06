      

#include <stdlib.h> 
#include "memTest.h" 

VO_MEM_VIDEO_INFO gVideoMem = {0};


VO_U32 VO_API Init(VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem)
{
	VO_S32 i;
	
	//allocate the memory
	printf("\nSharedYUVMemInit\n");

	if(pVideoMem->FrameCount > 0){
	
		VO_S32 YSize, UVSize, Stride;
		
		Stride = pVideoMem->Stride;
		Stride += 64;
		YSize = Stride * pVideoMem->Height;
		UVSize = YSize / 4;

		pVideoMem->VBuffer = (VO_VIDEO_BUFFER*)malloc(pVideoMem->FrameCount * sizeof(VO_VIDEO_BUFFER));

		if(pVideoMem->VBuffer){
		
			for (i = 0; i < pVideoMem->FrameCount; i++){ 
				//VO_U8 *pTmp1 = (VO_U8*)malloc(YSize  + 32);
				//VO_U8 *pTmp2 = (VO_U8*)malloc(UVSize + 32);
				//VO_U8 *pTmp3 = (VO_U8*)malloc(UVSize + 32);
				pVideoMem->VBuffer[i].Buffer[0] = (VO_U8*)malloc(YSize );
				pVideoMem->VBuffer[i].Buffer[1] = (VO_U8*)malloc(UVSize);
				pVideoMem->VBuffer[i].Buffer[2] = (VO_U8*)malloc(UVSize);
				pVideoMem->VBuffer[i].Stride[0] = Stride;
				pVideoMem->VBuffer[i].Stride[1] = Stride/2;
				pVideoMem->VBuffer[i].Stride[2] = Stride/2;
			}
		}
	}
	
	gVideoMem = *pVideoMem;
	return 0;
}


VO_U32 VO_API GetBufByIndex (VO_S32 uID, VO_S32 nIndex)
{
	VO_S32 locked = 0;

	return locked;
}

VO_U32 VO_API Uninit(VO_S32 uID)
{
	printf("\nSharedYUVMemUninit\n");

	//free the memory
	if(gVideoMem.VBuffer){
		VO_S32 i;
		for (i = 0 ; i <gVideoMem.FrameCount; i++){
			free(gVideoMem.VBuffer[i].Buffer[0]);
			free(gVideoMem.VBuffer[i].Buffer[1]);
			free(gVideoMem.VBuffer[i].Buffer[2]);
		}
	}

	return 0;
}

void GetVideoMemOperatorAPI(VO_MEM_VIDEO_OPERATOR * pVideoMemOperator)
{
	pVideoMemOperator->Init = Init;
	pVideoMemOperator->GetBufByIndex = GetBufByIndex;
	pVideoMemOperator->Uninit = Uninit;

}
