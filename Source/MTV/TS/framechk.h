#ifndef _FRAME_CHECKER_H
#define _FRAME_CHECKER_H

#include "voYYDef_TS.h"
#include "vobstype.h"
#include "LatmParser.h"




#if defined(LINUX)
//#   include "vocrstypes.h"
#include <string.h>
#endif


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MAX_SPLIT_FRAME_COUNT_IN_PES   126
class FrameChecker
{
public:
	//virtual bool IsValid(uint8* pData, int cData) { return true; }
	virtual bool IsSync(uint8* pData, int cData) { return true; }
	virtual int Split(uint8* pData, int cData, int* sizes, uint8*& pStart);

	FrameChecker () {m_lSyncWord = 0X01000000;}
	virtual ~FrameChecker() { }

	static FrameChecker* CreateChecker(int dcdObjectType, uint8* dsiData, int dsiSize);

	static FrameChecker* CreateCheckerDefault();
	static FrameChecker* CreateCheckerH264();
    static FrameChecker* CreateCheckerAdtsAAC();
	static FrameChecker* CreateCheckerAVC(int naluLengthSize=4);
	static FrameChecker* CreateCheckerBSAC();
	static FrameChecker* CreateCheckerMPEG2();
	static FrameChecker* CreateCheckerAC3();
	static FrameChecker* CreateCheckerMPEG4();
    static FrameChecker* CreateCheckerH265();

	static FrameChecker* CreateCheckerLATM(CLATMParser* parser);

protected:
	long	m_lSyncWord;
};

#ifdef _VONAMESPACE
}
#endif

#endif //._FRAME_CHECKER_H
