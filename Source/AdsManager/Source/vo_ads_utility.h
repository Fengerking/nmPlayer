
#ifndef __VO_ADS_UTILITY_H__

#define __VO_ADS_UTILITY_H__

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_U32 vo_download_by_url( VO_CHAR * ptr_url , VO_BYTE ** pptr_data , VO_U32 * ptr_size , VO_TCHAR * ptr_workingpath , VO_BOOL * ptr_is_stop );

#ifdef _VONAMESPACE
}
#endif

#endif