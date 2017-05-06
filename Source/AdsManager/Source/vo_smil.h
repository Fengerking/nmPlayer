
#ifndef __VO_SMIL_H__

#define __VO_SMIL_H__


#include "voAdsManager.h"
#include "voSmil.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


VO_U32 vo_smil_open( VO_PTR * ptr_h , VO_TCHAR * ptr_workingpath );
VO_U32 vo_smil_close( VO_PTR h );
VO_U32 vo_smil_parse_smilinfo( VO_PTR h , VO_CHAR * ptr_smil , VO_U32 size , VO_ADSMANAGER_PLAYBACKINFOEX ** pptr_info );


#ifdef _VONAMESPACE
}
#endif

#endif