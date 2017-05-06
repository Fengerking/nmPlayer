//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 2000 - 2001  Microsoft Corporation

Module Name:

    wmvdec_api.h

Abstract:

    Decoder API's for WMV.
        decodes WMV3, WMV2, WMV1, MP43, MP42, MP4S

Author:


Revision History:

*************************************************************************/

#ifndef __WMVDEC_API_H_
#define __WMVDEC_API_H_

#include "voVC1.h"
#include "xplatform_wmv.h" //sw add

#include "voVC1DID.h"

// basic types
typedef VO_HANDLE HWMVDecoder; 
//typedef void* HWMVDecoder; 

typedef void Void_WMV;
//typedef long I32_WMV;
typedef int I32_WMV;
typedef unsigned long U32_WMV;
typedef signed short I16_WMV;
typedef unsigned short U16_WMV;
typedef unsigned short U16Char_WMV;
typedef signed char I8_WMV;
typedef unsigned char U8_WMV;
typedef signed long Bool_WMV;
typedef double Double_WMV;
typedef float Float_WMV;

#define TRUE_WMV    1
#define FALSE_WMV   0
#define NULL_WMV   0

/**
 * WMV9 Decoder specific return code 
 */
typedef enum tagWMVDecodeStatus
{
    WMV_Succeeded = 0,
    WMV_Failed,                         // non-specific error; WMVideoDecReset() and try again at next keyframe.
    WMV_BadMemory,                      // Catastrophic error; close codec and reinit before playing more content.
    WMV_NoKeyFrameDecoded,              // A keyframe must be the first frame after starting or a reset.  WMVideoDecReset(), seek to a keyframe & continue.
    WMV_CorruptedBits,                  // Corrupt bitstream;  WMVideoDecReset() then seek to the next keyframe and try again.
    WMV_UnSupportedOutputPixelFormat,   // Try another color space (we generally like YV12, YUY2, and some RGB formats but not all WMV profiles map to all color spaces)
    WMV_UnSupportedCompressedFormat,    // Either FourCC or internal stream info indicates we can't play this clip.
    WMV_InValidArguments,               // Bad Arguement (can also occur when memory is corrupt).
    WMV_BadSource,                      // N/A in a production decoder -- treat as a catastrophic error.

    WMV_NoMoreOutput,                   // WMVideoDecGetOutput called when no output is available.  Don't update screen but OK to continue to decode
    WMV_EndOfFrame,                     // WMVDecCBGetData returns this when there is no more data available for this frame.
    WMV_BrokenFrame,                    // Decoder thinks more data is needed but no more is available, treat as WMV_CorruptedBits

    WMV_UnSupportedTransform            // Returned by the CallBack if an indicated external output transform is not supported in hardware
                                        // this is a request to the Codec to do the output transform inside the codec as part of GetOutput.
} tWMVDecodeStatus;

typedef enum tagWMVDecodeDispRotateDegree
{
    WMV_DispRotate0 = 0,
    WMV_DispRotate90,
    WMV_DispFlip,
    WMV_DispRotate270
} tWMVDecodeDispRotateDegree;


// *** *** *** *** *** ***
// Handle_WMV, tWMVAsyncRect, tagWMVDecodeState are needed only for the Asynchronous Interface extension
// However, they can be used with a synchronous interface

typedef void* Handle_WMV;

typedef struct tagWMVRect {
   I32_WMV left;
   I32_WMV top;
   I32_WMV right;
   I32_WMV bottom;
} tWMVRect;

typedef enum tagWMVAsyncDecodeState {
    CodecState_None,            // The codec is uninitialized or in an undefined state.
    CodecState_AsyncError,      // Please notice an asynchronously reported error.
    CodecState_ReadyToDecode,   // The codec is ready and waiting for us to give it a compressed buffer.
    CodecState_Decoding,        // The codec is asynchronous, and is still processing the buffer.
    CodecState_Decoded,         // The buffer has been decoded (synchronously or asynchronously), and the finished frame is ready for us to get from the codec.  We should only see this state if the codec doesn't do its own rendering and expects the player to render the result instead.
    CodecState_ReadyToRender,   // The buffer has been decoded and is being held internally for rendering by the codec.
    CodecState_Rendering        // The codec renderer is asynchronous, and is still rendering.
} tWMVAsyncDecodeState;


// *** *** *** *** *** ***
// HurryUp flags

typedef enum tagWMVAVSyncState {
    WMVSyncAV_Ignore = -1,      // legacy value, the codec will ignore this field
    WMVSyncAV_Unknown = 0,      // legacy value, the codec will ignore this field
    WMVSyncAV_AllOK,            // A/V sync in good shape, video being decoded in time for correct A/V display
    WMVSyncAV_BehindOK,         // Video is behind audio timeline but dropping a frame is not imminent
    WMVSyncAV_DropNext,         // Video is seriously behind audio timeline.  We predict not this frame but the next one will be dropped.
    WMVSyncAV_DropThis          // Video is seriously behind audio timeline.  This frame will not be displaqy, do the minimum decode possible.
} tWMVAVSyncState;

typedef enum tagWMVPostLevel {
    WMVPostTest = -2,           // test mode for wmvdiff: no post processing for WMV, but allow it for WMVP (Photostory).
    WMVPostAuto = -1,           // let the codec deside based on content and available CPU (Recommended)
    WMVPostNone = 0,            // disable post proceessing
    WMVPostFastDeblock,         // complex
    WMVPostFullDelock,          // pretty complex
    WMVPostDeringFastDeblock,   // quite expensive
    WMVPostDeringFullDeblock    // very expensive
} tWMVPostLevel;


// historically, iHurryUp was supported in the codec but not in the players.  So it is replaced by the WMV_AV_Sync bitfields.
// iHurryUp had many meanings:
//    In some v8 codebases, it could range from -2 to 2 and was the negative increment to change the post processing level.  Some test apps implemented this
//      from the command line, but ended up passing the same value to every call to decode data which just quickly forced the Post Processing level to 0 or 4.
//    In other v9 codebases, it was -1 or 0..4 and set the post processing level or let the codec decide.  No known implementation of this (only -1 was seen)
//    No known player app has used this feature.  Some may be passing in 0 and some may be passing in -1.
//    Since these are incompatible meanings (one has a default value of -1 and the other has a default value of 0), change completely.
// This is being replaced by passing more information about the player's A/V timeline so the codec can shed processing if things get behind
// and resume higher quality if CPU time become available.
// A player that wants the best user experience will implement WMVSyncAV in the call to DecodeData in the following way:
//    pass in m_MSOffset which is the millisecond offset between the desired render time and the time the last frame was ready to be rendered
//    pass in m_eAVSyncState implemented as completely as possible
//        WMVSyncAV_DropNext and WMVSyncAV_DropThis gives the codec the chance to drop this frame if it is a B-Frame which will save a lot of time.
//        WMVSyncAV_DropThis gives the codec the chance to avoid some processing it might otherwise do.
//        If a frame does not need to be rendered (because it is dropped or for any other reason), the Codec state exiting DecodeData will be
//        CodecState_ReadyToDecode instead of CodecState_ReadyToRender.  If one ignores this and calls GetOutput, the previous frame will be output.
//    pass in m_ePostLevel==WMVPostAuto.
//        The codec may automatically adapt post processing level based on internal reg keys and m_iMSOffset.  A particular post processing level
//        can be forced by use of this field.  Used mainly to test post processing or to force post processing off in underpowered implementations.

// *** *** *** *** *** ***


// GUID definiton
//// SHOULD MERGE WITH WMF_TYPES.H

#ifndef MAKEFOURCC_WMV
#define MAKEFOURCC_WMV(ch0, ch1, ch2, ch3) \
        ((U32_WMV)(U8_WMV)(ch0) | ((U32_WMV)(U8_WMV)(ch1) << 8) |   \
        ((U32_WMV)(U8_WMV)(ch2) << 16) | ((U32_WMV)(U8_WMV)(ch3) << 24 ))

#define mmioFOURCC_WMV(ch0, ch1, ch2, ch3)  MAKEFOURCC_WMV(ch0, ch1, ch2, ch3)
#endif


/******* video output type guids, in preference order  *****/
#ifndef _BIG_ENDIAN_
//Little Endian
#define FOURCC_WMVA_WMV     mmioFOURCC_WMV('W','M','V','A')
#define FOURCC_wmva_WMV     mmioFOURCC_WMV('w','m','v','a')
#define FOURCC_WMVP_WMV     mmioFOURCC_WMV('W','M','V','P')
#define FOURCC_wmvp_WMV     mmioFOURCC_WMV('w','m','v','p')
#define FOURCC_WVP2_WMV     mmioFOURCC_WMV('W','V','P','2')
#define FOURCC_wvp2_WMV     mmioFOURCC_WMV('w','v','p','2')
#define FOURCC_WMV3_WMV     mmioFOURCC_WMV('W','M','V','3')
#define FOURCC_wmv3_WMV     mmioFOURCC_WMV('w','m','v','3')
#define FOURCC_WMV2_WMV     mmioFOURCC_WMV('W','M','V','2')
#define FOURCC_wmv2_WMV     mmioFOURCC_WMV('w','m','v','2')
#define FOURCC_WMV1_WMV     mmioFOURCC_WMV('W','M','V','1')
#define FOURCC_wmv1_WMV     mmioFOURCC_WMV('w','m','v','1')
#define FOURCC_M4S2_WMV     mmioFOURCC_WMV('M','4','S','2')
#define FOURCC_m4s2_WMV     mmioFOURCC_WMV('m','4','s','2')
#define FOURCC_MP43_WMV     mmioFOURCC_WMV('M','P','4','3')
#define FOURCC_mp43_WMV     mmioFOURCC_WMV('m','p','4','3')
#define FOURCC_MP4S_WMV     mmioFOURCC_WMV('M','P','4','S')
#define FOURCC_mp4s_WMV     mmioFOURCC_WMV('m','p','4','s')
#define FOURCC_MP42_WMV     mmioFOURCC_WMV('M','P','4','2')
#define FOURCC_mp42_WMV     mmioFOURCC_WMV('m','p','4','2')
#define FOURCC_WVC1_WMV     mmioFOURCC_WMV('W','V','C','1')
#define FOURCC_wvc1_WMV     mmioFOURCC_WMV('w','v','c','1')

#define FOURCC_ASyncVer_Win32   mmioFOURCC_WMV('a','v','0','1')

#else
//Big Endian
#define FOURCC_WMVA_WMV     'WMVA'
#define FOURCC_wmva_WMV     'wmva'
#define FOURCC_WMVP_WMV     'WMVP'
#define FOURCC_wmvp_WMV     'wmvp'
#define FOURCC_WVP2_WMV     'WVP2'
#define FOURCC_wvp2_WMV     'wvp2'
#define FOURCC_WMV3_WMV     'WMV3'
#define FOURCC_wmv3_WMV     'wmv3'
#define FOURCC_WMV2_WMV     'WMV2'
#define FOURCC_wmv2_WMV     'wmv2'
#define FOURCC_WMV1_WMV     'WMV1'
#define FOURCC_wmv1_WMV     'wmv1'
#define FOURCC_M4S2_WMV     'M4S2'
#define FOURCC_m4s2_WMV     'm4s2'
#define FOURCC_MP43_WMV     'MP43'
#define FOURCC_mp43_WMV     'mp43'
#define FOURCC_MP4S_WMV     'MP4S'
#define FOURCC_mp4s_WMV     'mp4s'
#define FOURCC_MP42_WMV     'MP42'
#define FOURCC_mp42_WMV     'mp42'
#define FOURCC_WVC1_WMV     'WVC1'
#define FOURCC_wvc1_WMV     'wvc1'

#define FOURCC_ASyncVer_Win32   'av01'

#endif //macintosh

/***********************************************************/


/******* video intput type guids, in preference order  *****/

#define FOURCC_I420_WMV             0x30323449
#define FOURCC_IYUV_WMV             0x56555949
#define FOURCC_YV12_WMV             0x32315659
#define FOURCC_YUY2_WMV             0x32595559
#define FOURCC_NV12_WMV             0x3231564E
#define FOURCC_UYVY_WMV             0x59565955
#define FOURCC_YVYU_WMV             0x55595659
#define FOURCC_YVU9_WMV             0x39555659
#define FOURCC_BI_RGB_WMV           0x00000000
#define FOURCC_BI_BITFIELDS_WMV     0x00000003
#define FOURCC_P422_WMV     mmioFOURCC_WMV('P','4','2','2')
#define FOURCC_P411_WMV             0x31313450


/***********************************************************/

/******* frame types ***************************************/
#define WMVIDEO_FRAMETYPE_I  0
#define WMVIDEO_FRAMETYPE_P  1


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


tWMVDecodeStatus WMVideoDecInit (
    HWMVDecoder*                phWMVDecoder,
    U32_WMV                     uiUserData,
    U32_WMV                     uiFOURCCCompressed,
    Float_WMV                   fltFrameRate,
    Float_WMV                   fltBitRate,
    I32_WMV                     iWidthSource,
    I32_WMV                     iHeightSource,
    I32_WMV                     iPostFilterLevel,
    U8_WMV                      *pSequenceHeader, 
    U32_WMV                     uipSequenceHeaderLength,
    Bool_WMV                    bHostDeinterlace 
);

tWMVDecodeStatus WMVideoDecClose (HWMVDecoder hWMVDecoder);


tWMVDecodeStatus voVC1_WMVideoDecDecodeSequenceHeader (HWMVDecoder* phWMVDecoder);

//tWMVDecodeStatus WMVideoDecDecodeData (HWMVDecoder hWMVDecoder, U16_WMV* puiNumDecodedFrames );

// uiWidthBMPOutput allows the wourse width to be output into a larger BMP of width uiWidthBMPOutput.
// passing 0 causes the default source width to be used.

tWMVDecodeStatus WMVideoDecReset (HWMVDecoder hWMVDecoder);


// *** *** *** *** *** ***
// WMVDecCBGetData is an Application provided callback function to return data from the file.
// [in]  uiUserData is value provided to WMVideoDecInit
// [in]  uintPadBeginning is normally 0.    
// [out] **ppchBuffer gets a pointer to where the data is so you do not have to copy it
// [ign] uintUserDataLength is ignored -- you can return as much data as you want.  Typically the more at once the better.
// [out] *puintActualBufferLength get the length of the data returned
// [out] *pbNotEndOfFrame is true if more data will be available for this frame
extern tWMVDecodeStatus WMVDecCBGetData (HWMVDecoder hWMVDecoder,
    U32_WMV     uintPadBeginning,
    U8_WMV    **ppchBuffer, 
    U32_WMV     uintUserDataLength,
    U32_WMV    *puintActualBufferLength,
    Bool_WMV   *pbNotEndOfFrame
);


// *** *** *** *** *** ***
// This function should be called before a seek to the next key frame is performed. It will reset the internal state of the decoder to the beginning of the stream
tWMVDecodeStatus  WMVideoDecResetBState( HWMVDecoder hWMVDecoder) ;


// *** *** *** *** *** ***
// This functiuon returns the internal frame rate extracted from the raw bitstream.  Not a substitute for timestamps.  Callable after WMVideoDecDecodeSequenceHeader().
I32_WMV WMVideoDecGetFrameRate (HWMVDecoder hWMVDecoder);

// *** *** *** *** *** ***


// *** *** *** *** *** ***
// Advanced Profile Bitstream Info Callback
// The following interface is used by the codec to communicate back to the App during the decoding process.
// It is only implemented for Advanced Profile, but is designed to be extensible.
// The App must implement one functions WMVSCInfoCallBack(...) which can be an empty stub returning WMV_Succeeded if the app does not want to implement this feature.
// The WMVSCInfoCallBack passes a untion of structures to the app depneding on the nature of the information to be communicated
// The App may call a codec function to enable particular types of callbacks.  By default, all callbacks are disabled.


// tWMVSequenceHeaderInfo is available when any WMVA Sequence Header is decoded
// The App should return WMV_Succeeded if it is OK with the App to decode this Profile and Level.
// if the App returns any other value, the decoder will return from Decode with an error return
typedef struct tagWMVSequenceHeaderInfo {
    I32_WMV   iProfile;                 // See WMV Spec for details
    I32_WMV   iLevel;                   // See WMV Spec for details
    Bool_WMV  bSourceInterlaced;        // 

    I32_WMV   iFrameRate4PostProc;      // coarse approx to frame rate to aid selection of external post processing algorithms
    I32_WMV   iBitRate4PostProc;        // coarse approx to bit rate to aid selection of external post processing algorithms
} tWMVSequenceHeaderInfo;



// This sturct is used by tWMVDimensions
typedef struct tagWMVHRD {
    U16_WMV   ui16HrdRate;              // See spec.
    U16_WMV   ui16HrdBuffer;            // See spec
} tWMVHRD;



// tWMVDimensions is available when any WMVA Sequence Header is decoded which includes any of this information 
// This callback occurs after the tWMVSequenceHeaderInfo callback.
// The App should return WMV_Succeeded if the App can handle any resizing implied by the difference between MaxCoded size and Display Size.
// If the App returns WMV_UnSupportedTransform, the decoder will assume it must software resize in GetOutput (there is a performance cost)
typedef struct tagWMVDimensions {
    Bool_WMV bMaxCodedSizeChanged;      // The maximum coded size has changed since the previous GOP.
    I32_WMV  iMaxCodedHorizSize;        // Horizontal Maximum Coded Size in Pixels
    I32_WMV  iMaxCodedVertSize;         // Vertical Maximum Coded Size in Pixels

    Bool_WMV bDispSizeFlag;             // non-zero if iDispHorizSize && iDispVertSize are valid (otherwise, assume == MAxCodedSize)
    I32_WMV  iDispHorizSize;            // Horizontal Display Size
    I32_WMV  iDispVertSize;             // Vertical Display Size

    Bool_WMV bAspectRatioFlag;          // non-zero if iAspectRatio is valid
    I32_WMV  iAspectRatio;              // from bitstream, see spec
    I32_WMV  iAspect15HorizSize;        // from bitstream, see spec
    I32_WMV  iAspect15VertSize;         // from bitstream, see spec

    Bool_WMV bFrameRateFlag;            // non-zero if FrameRate info is valid
    Bool_WMV bFrameRateInd;             // As coded in spec
    I32_WMV  iFrameRateNR;              // As coded in spec
    I32_WMV  iFrameRateDR;              // As coded in spec
    I32_WMV  iFrameRateExp;             // As coded in spec

    Bool_WMV bColorFormatFlag;          // non-zero if ColorFormat info is valid
    I32_WMV  iColorPrim;                // As coded in spec
    I32_WMV  iTransferChar;             // As coded in spec
    I32_WMV  iMatrixCoef;               // As coded in spec

    I32_WMV  iNumLeakyBuckets;          // range 1..32 or 0 is HRD info is not present in this entry point
    I32_WMV  iBitRateExponent;          // range 6..32
    I32_WMV  iBufferSizeExponent;       // range 4..19
    tWMVHRD* pHRD;                      // Array of size iNumLeakyBuckets

} tWMVDimensions;




// tWMVEntryPoint is available when the decoder encounters a Entry Point Header which signals a random access point
// The App should return WMV_Succeeded if the App can handle any Resizing and Range Remapping required
// if the App returns WMV_UnSupportedTransform, the decoder will resize and remap YUV in GetOutput (at a performance cost).
typedef struct tagWMVEntryPoint {

    I32_WMV  iNumLeakyBuckets;          // range 1..32 or 0 is HRD info is not present in this entry point
    U8_WMV*  pucHrdFullness;            // HRD_FULLNESS[iNumLeakyBuckets]

    Bool_WMV bCodedSizeChanged;         // true if coded size for this GOP is different than the coded size for the previous GOP.
    I32_WMV  iCodedHorizSize;           // Coded (Picture) Horizontal Size
    I32_WMV  iCodedVertSize;            // Coded (Picture) Verticle Size

    // The following four bits are for information but are not generally needed for A/V playback.
    Bool_WMV bBrokenLink;               // Approximately: Stream has been editted and some follwoing frames may be invalid.  See spec. 
    Bool_WMV bClosedEntryPoint;         // Approximately: Start of follwoing GOP will create a broken link if editted.  See spec.
    Bool_WMV bPanScanPresent;           // Scan and Pan Info may be present in Picture info
    Bool_WMV bRefDistPresent;           // Approximately: There may be B frames in the following GOP.  See Spec.

    // One can use the following bit to know that there is a one frame delay in the decode vs presentaton time.  
    // So timing of an audio track should be delayed by one frame time.
    Bool_WMV bBFramesPresent;           // There may be B frames in the overall sequence, not necessarily the following GOP

    Bool_WMV bRangeMapYFlag;            // non-zero if iRangeMapY is valid
    I32_WMV  iRangeMapY;                // RANGE_MAPY, see spec

    Bool_WMV bRangeMapUVFlag;           // non-zero if iRangeMapUV is valid
    I32_WMV  iRangeMapUV;               // RANGE_MAPUV, see spec

} tWMVEntryPoint;



typedef struct tagPanSscanInfo {
    I32_WMV  iHorizOffset;              // signed horizontal offset in 1/16th of pixel units
    I32_WMV  iVertOffset;               // signed vertical offset in 1/16th of pixel units
    U32_WMV  iWidth;                    // unsigned width of window in pixel units
    U32_WMV  iHeight;                   // unsigned height of window in pixel units
} tPanScanInfo;



// The tWMVDisplayInfo struct is available when these parameters are encountered in the bitstream.
typedef struct tagWMVPictureInfo {

    I32_WMV   iPictureType;             // See Spec for details

    Bool_WMV  bInterlacedSource;        // always same as available in tWMVSequenceHeaderInfo
    Bool_WMV  bTopFieldFirst;           // display the top field first (meaning only in interlace)
    Bool_WMV  bRepeatFirstField;        // display first field
    I32_WMV   iRepeatFrameCount;        // number of times to repeat a frame

    I32_WMV   iNumPanScanWindows;       // 0..3  number of pan and scan windows.  Number present varies by frame/field type.  See spec.
    tPanScanInfo* pPSI;                 // Position and size of each Pan and Scan window in pPSI[0], pPSI[1], ...

    Bool_WMV  bUVSamplingFormat;        // See Spec for details

    Bool_WMV  bFrameInterpolationFlag;  // non-zero if bFrameInterpolationHint is valid
    Bool_WMV  bFrameInterpolationHint;  // See Spec for details.

    Bool_WMV  bPostProcHint;            // non-zero if iPostProcHint is valid
    I32_WMV   iPostProcHint;            // Recommended post processing, see spec.

} tWMVPictureInfo;


typedef enum tagMoreUserData {
    eMiddleChunkUserData = 0,           // Long Userdata - this is the middle part, expect more
    eStartChunkUserData,                // Long userdata - this is just the first part, expect more
    eEndChunkUserData,                  // Long userdata - this is the final part
    eCompleteChunkUserData              // Normal case   - full length of user data
} eWMVMoreUserData;


// tWMVUserData is available whenever a StartCode containing UserData is encountered in the bitstream
// UserData will trigger a callback after other callbacks are complete
// This data must be copied or consumed during this callback as it is not gaurenteed to remain stable
typedef struct tagWMVUserData {
    U32_WMV  uiUDLength;                // uiUDLength is the length of the userdata (without any trailing zero or trailing Flushing byte)
    U8_WMV*  pucUserData;               // picUserData is the user data.  
    eWMVMoreUserData eMoreUserData;     // Normally eCompleteChunkUserData, but when the UserData has been broken up by input chunking
                                        // (streaming mode), a single UserData block in the bitstream will be returned in parts.
} tWMVUserData;



// index which signals which struct is present in tWMVStartCodeInfo 
typedef enum tagWMVStartCodeIndex {
    idxSequenceInfo = 0,
    idxDimensions,
    idxEntryPoint,
    idxPictureInfo,
    idxSequenceLevelUserData,
    idxEntryPointUserData,
    idxFrameLevelUserData,
    idxFieldLevelUserData,
    idxSliceLevelUserData,
    idxEndOfSequence
    // Additional values may be added in the future.  Ignore and do not enable any other value
} tWMVStartCodeIndex;



// Union of structs which are available for different callbacks.
typedef union _tagWMVStartCodeInfo {
    tWMVSequenceHeaderInfo  tSequenceInfo;
    tWMVDimensions          tDimensions;
    tWMVEntryPoint          tEntryPoint;
    tWMVPictureInfo         tPictureInfo;
    tWMVUserData            tSequencelevelUserData;
    tWMVUserData            tEntryPointLevelUserData;
    tWMVUserData            tFrameLevelUserData;
    tWMVUserData            tFieldLevelUserData;
    tWMVUserData            tSliceLevelUserData;
    I32_WMV                 iEndOfSequence;         // End of Sequence being called is only info
} tWMVStartCodeInfo;



// start code suffix from the bitstream
typedef enum tagWMVStartCodeSuffix {
    StartCode_Slice = 0x0b,
    StartCode_Field,
    StartCode_Frame,
    StartCode_EntryPoint,
    StartCode_SequenceHeader,
    StartCode_SliceLevelUserData = 0x1b,
    StartCode_FieldLevelUserData,
    StartCode_FrameLevelUserData,
    StartCode_EntryPointLevelUserData,
    StartCode_SequenceLevelUserData
} tWMVStartCodeSuffix;



// Main struct returned by CallBack.
typedef struct _tagWMVCallBackInfo {
    Void_WMV *          pApp;               // value passed in by App in last call to WMVSCInfoMask
    HWMVDecoder         hWMVDecoder;        // handle from WMVideoDecInit
    tWMVStartCodeIndex  idxSCInfo;          // An index specifying which struct in the union tWMVStartCodeInfo is being returned
    tWMVStartCodeInfo   tSCInfo;            // The union containing info from the bitstream
 } tWMVCallBackInfo;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // __WMVDEC_API_H_
