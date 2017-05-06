
#ifndef __VOADAPTIVESTREAMINGCLASSFACTORY_H__

#define __VOADAPTIVESTREAMINGCLASSFACTORY_H__

#include "voAdaptiveStreamingController.h"
#include "voChannelItemThread.h"
#include "voAdaptiveStreamingFileParser.h"
#include "voSourceBufferManager_AI.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_U32 CreateAdaptiveStreamingController( voAdaptiveStreamingController ** ppController );
VO_U32 DestroyAdaptiveStreamingController( voAdaptiveStreamingController * pController );

VO_U32 CreateChannelItemThread( VO_ADAPTIVESTREAMPARSER_STREAMTYPE StreamType , voChannelItemThread ** ppThread );
VO_U32 DestroyChannelItemThread( voChannelItemThread * pThread );

VO_U32 CreateAdaptiveStreamingFileParser( voAdaptiveStreamingFileParser ** ppParser , VO_ADAPTIVESTREAMPARSER_STREAMTYPE StreamType , VO_FILE_FORMAT fFormat
											, VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp 
											, VO_TCHAR *pWorkPath, bool bForceCreate );

VO_U32 DestroyAdaptiveStreamingFileParser( voAdaptiveStreamingFileParser * pParser );

VO_U32 CreateAdaptiveStreamingBuffer( voSourceBufferManager_AI ** ppBuffer , VO_S32 nBufferingTime , VO_S32 nMaxBufferTime , VO_S32 nStartBufferTime );
VO_U32 DestroyAdaptiveStreamingBuffer( voSourceBufferManager_AI * pBuffer );

#ifdef _VONAMESPACE
}
#endif

#endif