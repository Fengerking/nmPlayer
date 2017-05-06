
#ifndef __VO_SOURCEIOWRAPPER_H__
#define __VO_SOURCEIOWRAPPER_H__

#include "voYYDef_SourceIO.h"
#include "voSource2_IO.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct
{
	VO_PTR  pIOHandle;
	VO_PTR  pDrmEng;
}voSourceIOHnd;

#ifdef _VONAMESPACE
}
#endif ///< _VONAMESPACE

VO_VOID voGetSourceIOAPI( VO_SOURCE2_IO_API * ptr_api );

#endif///<__VO_SOURCEIOWRAPPER_H__