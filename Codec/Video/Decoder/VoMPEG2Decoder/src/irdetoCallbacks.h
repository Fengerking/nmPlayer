/******************************************************************************
        Irdeto Callbacks for video processing
******************************************************************************/
#ifndef __IRDETO_CALLBACKS_H__
#define __IRDETO_CALLBACKS_H__

#ifdef _WIN32
#ifdef IRDETO_CALLBACK_EXPORTS
#define IRDETO_CALLBACK_API __declspec(dllexport)
#else
#define IRDETO_CALLBACK_API __declspec(dllimport)
#endif
#else
#define IRDETO_CALLBACK_API 
#endif


// Processes macroblocks and fixes them if required
// Arguments:
//   gopTimecode         The 25-bit time code value as obtained from last GOP header
//   temporalReference   The 10-bit temporal reference from the current picture layer
//   pictureCodingType   Type of picture (I,B,P frame) as returned in MPEG2 header
//   horMacroBlockNum    Horizontal location of macroblock, base 1
//   verMacroBlockNum    Vertical location of macroblock, base 1
//   int blockNumber     Which block is this?  For 4:2:0, 0-3 == Y component, 4 == U, 5 == V
//   coefs               Post IDCT pixel values, but before clipping.  Should be 64 values.
extern IRDETO_CALLBACK_API VO_VOID irdeto_postIDCTCallback(
    VO_S32 gopTimecode, 
    VO_S32 temporalReference,
    VO_S32 pictureCodingType,
    VO_S32 horMacroBlockNum, 
    VO_S32 verMacroBlockNum, 
    VO_S32 blockNumber, 
    VO_S16 *coefs);

// Flushes and clears our internal buffers
extern IRDETO_CALLBACK_API VO_VOID iredto_flush();


#endif
