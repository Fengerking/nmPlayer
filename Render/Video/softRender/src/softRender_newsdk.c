#include "ccConstant.h"
#include "voCCRRR.h"
#include "stdlib.h"
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#  include "voChkLcsLib.h"
#elif defined(LINUX)
#  include <string.h>
#endif 
#ifdef NEW_SDK
#include "ccrrrender.h"
#endif//NEW_SDK
extern int *y_tab;
extern int *x_tab;

#ifdef NEW_SDK
typedef struct
{
	int in_width;
	int out_width;
	int in_height;
	int out_height;
	int ini_flg;
	void* CC_HND;
	INTYPE in_type;
	INTYPE out_type;
	RTTYPE rt;
}
CC_RRR;

VO_S32 VO_API voGetVideoCCRRRAPI (VO_VIDEO_CCRRRAPI * pCCRRR, VO_U32 uFlag)
{
	pCCRRR->Init = voCCRRInit;
	pCCRRR->Uninit = voCCRRUninit;
	pCCRRR->GetProperty = voCCRRGetProperty;
	pCCRRR->GetInputType = voCCRRGetInputType;
	pCCRRR->GetOutputType = voCCRRGetOutputType;				
	pCCRRR->SetColorType = voCCRRSetVideoType;
	pCCRRR->SetCCRRSize = voCCRRSetVideoSize;
	pCCRRR->Process = voCCRRProcess;
	pCCRRR->WaitDone = voCCRRWaitDone;
	pCCRRR->SetCallBack = voCCRRSetCallBack;
	pCCRRR->GetVideoMemOP = voCCRRGetMemoryOperator;
	pCCRRR->SetParam = voCCRRSetParameter;
	pCCRRR->GetParam = voCCRRGetParameter;	
}
#endif //NEW_SDK