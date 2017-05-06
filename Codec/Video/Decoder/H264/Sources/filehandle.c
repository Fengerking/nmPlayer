
/*!
 **************************************************************************************
 * \file
 *    filehandle.c
 * \brief
 *     Trace file handling and standard error handling function.
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Karsten Suehring            <suehring@hhi.de>
 ***************************************************************************************
 */
 #include "defines.h"
#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#endif

#include "contributors.h"
#include "global.h"
#include "mbuffer.h"
#include "nalucommon.h"
#include "voLog_android.h"
void ProcessDPBAfterError(ImageParameters *img)
{
	//DecodedPictureBuffer *dpb = img->dpb;

	if(img->dec_picture)
		PutStorablePic(img,img->dec_picture);

}
/*!
 ************************************************************************
 * \brief
 *    Error handling procedure. Print error message to stderr and exit
 *    with supplied code.
 * \param text
 *    Error message
 * \param code
 *    Exit code
 ************************************************************************
 */
static int warningCount=0;
static int currFrame=-1;
#define WARN_LIMIT 2

void avd_error(ImageParameters *img,char *text, int code)
{
#ifdef USE_JOBPOOL
     int i;
#endif
	if(code>=0)
	{
		AvdLog2(LL_WARN,"Frame_%d,warning %d:%s\n",img->ioInfo->sliceNum,code,text);
#if defined(_LINUX_ANDROID)
		if(code==FORCE_OUT_WARN_INFO)
		{
			voLog_264_debug("Frame_%d,warning %d:%s\n",img->ioInfo->sliceNum,code,text);
		}
#endif
		return;
	}
	img->error = code;	
#ifdef USE_JOBPOOL
#if USE_IMG_MIRROR			
    if(img->vdLibPar->multiCoreNum > 1 && img->is_cores_running)
    {
	    for(i = 0; i < MAX_SLICE_NUM; i++)
	        img->curr_slice_img[i]->error = code;
    }
#endif	
#endif
	
	
#ifdef USE_JOBPOOL    
	if(!img->is_cores_running)
	    DumpBufInfo(img,img->ori_mbsProcessor[0]);
	else
	    DumpBufInfo(img,img->mbsProcessor[0]);
#else
	DumpBufInfo(img,img->mbsProcessor[0]);
#endif
	
	AvdLog2(LL_ERROR,"error:%d, %s, frameNum:%d,poc:%d, error kind=%d,%s\n\n",img->ioInfo->sliceNum,GetStrFrameType(img->type),img->specialInfo->frame_num,img->pocInfo->currPoc, code,text);
	if (img->vdLibPar->multiCoreNum>1)
	{
#ifdef USE_JOBPOOL
        if(!img->is_cores_running)
	      DumpMbsInfo(img->ori_mbsProcessor[0],-999);//for check in
	    else
		  DumpMbsInfo(img->mbsProcessor[0],-999);//for check in
#else	
		DumpMbsInfo(img->mbsProcessor[0],-999);//for check in
#endif		
	}
#if !ENABLE_TRACE
	if(code!=-ERROR_FirstFrameIsNotIntra && !(img->vdLibPar->optFlag&OPT_DISABLE_ERRORLOG))
		voLog_264_debug("error:%d, %s, frameNum:%d,poc:%d, error kind=%d,%s\n\n",img->ioInfo->sliceNum,
			GetStrFrameType(img->type),
			img->specialInfo->frame_num,img->pocInfo->currPoc, code,text);
#endif//ENABLE_TRACE
}


