#include		"voH264.h"
/*
void g1_fake_link_voH264()
{
	VO_HANDLE hCodec;
	int		returnCode;
	VO_CODECBUFFER inData;
	VO_VIDEO_BUFFER  outData;
	VO_VIDEO_OUTPUTINFO outFormat;
	VO_VIDEO_DECAPI decApi={0};
	voGetH264DecAPI(&decApi,0);
	returnCode = decApi.Init(&hCodec,0,0);
	returnCode = decApi.SetInputData(hCodec,&inData);
	decApi.GetOutputData(hCodec,&outData,&outFormat);
	decApi.Uninit(hCodec);
}
*/
VO_S32 VO_API voGetH264DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	return 	voGetH264DecAPI2(pDecHandle,uFlag);
}