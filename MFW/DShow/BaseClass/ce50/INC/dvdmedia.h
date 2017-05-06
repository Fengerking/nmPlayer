//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//
//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;
//
// dvdmedia.h
//
// Contains typedefs and #defines necessary for user mode (ring 3) DVD 
// filters and applications.
// This is should be included in the DirectShow SDK for user mode filters.
// The types defined here should be kept in synch with ksmedia.h WDM DDK for
// kernel mode filters.
//

#ifndef __DVDMEDIA_H__
#define __DVDMEDIA_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// -----------------------------------------------------------------------
// AC-3 definition for the AM_KSPROPSETID_AC3 property set
// -----------------------------------------------------------------------

typedef enum {
	AM_PROPERTY_AC3_ERROR_CONCEALMENT = 1,
	AM_PROPERTY_AC3_ALTERNATE_AUDIO,
	AM_PROPERTY_AC3_DOWNMIX,
	AM_PROPERTY_AC3_BIT_STREAM_MODE,
	AM_PROPERTY_AC3_DIALOGUE_LEVEL,
	AM_PROPERTY_AC3_LANGUAGE_CODE,
	AM_PROPERTY_AC3_ROOM_TYPE
} AM_PROPERTY_AC3;

typedef struct  {
	BOOL        fRepeatPreviousBlock;
	BOOL        fErrorInCurrentBlock;
} AM_AC3_ERROR_CONCEALMENT, *PAM_AC3_ERROR_CONCEALMENT;

typedef struct {
	BOOL    fStereo;
	ULONG   DualMode;
} AM_AC3_ALTERNATE_AUDIO, *PAM_AC3_ALTERNATE_AUDIO;

#define AM_AC3_ALTERNATE_AUDIO_1     1
#define AM_AC3_ALTERNATE_AUDIO_2     2
#define AM_AC3_ALTERNATE_AUDIO_BOTH  3

typedef struct {
	BOOL        fDownMix;
	BOOL        fDolbySurround;
} AM_AC3_DOWNMIX, *PAM_AC3_DOWNMIX;

typedef struct {
	LONG        BitStreamMode;
} AM_AC3_BIT_STREAM_MODE, *PAM_AC3_BIT_STREAM_MODE;

#define AM_AC3_SERVICE_MAIN_AUDIO            0
#define AM_AC3_SERVICE_NO_DIALOG             1
#define AM_AC3_SERVICE_VISUALLY_IMPAIRED     2
#define AM_AC3_SERVICE_HEARING_IMPAIRED      3
#define AM_AC3_SERVICE_DIALOG_ONLY           4
#define AM_AC3_SERVICE_COMMENTARY            5
#define AM_AC3_SERVICE_EMERGENCY_FLASH       6
#define AM_AC3_SERVICE_VOICE_OVER            7

typedef struct {
	ULONG   DialogueLevel;
} AM_AC3_DIALOGUE_LEVEL, *PAM_AC3_DIALOGUE_LEVEL;

typedef struct {
	BOOL    fLargeRoom;
} AM_AC3_ROOM_TYPE, *PAM_AC3_ROOM_TYPE;


// -----------------------------------------------------------------------
// subpicture definition for the AM_KSPROPSETID_DvdSubPic property set
// -----------------------------------------------------------------------

typedef enum {
	AM_PROPERTY_DVDSUBPIC_PALETTE,
	AM_PROPERTY_DVDSUBPIC_HLI,
	AM_PROPERTY_DVDSUBPIC_COMPOSIT_ON,  // TRUE for subpicture is displayed
} AM_PROPERTY_DVDSUBPIC;

typedef struct _AM_DVD_YUV {
	UCHAR   Reserved;
	UCHAR   Y;
	UCHAR   U;
	UCHAR   V;
} AM_DVD_YUV, *PAM_DVD_YUV;

typedef struct _AM_PROPERTY_SPPAL {
	AM_DVD_YUV sppal[16];
} AM_PROPERTY_SPPAL, *PAM_PROPERTY_SPPAL;

typedef struct _AM_COLCON {
	UCHAR emph1col:4;
	UCHAR emph2col:4;
	UCHAR backcol:4;
	UCHAR patcol:4;
	UCHAR emph1con:4;
	UCHAR emph2con:4;
	UCHAR backcon:4;
	UCHAR patcon:4;

} AM_COLCON, *PAM_COLCON;

typedef struct _AM_PROPERTY_SPHLI {
	USHORT     HLISS;      //
	USHORT     Reserved;
	ULONG      StartPTM;   // start presentation time in x/90000
	ULONG      EndPTM;     // end PTM in x/90000
	USHORT     StartX;
	USHORT     StartY;
	USHORT     StopX;
	USHORT     StopY;
	AM_COLCON  ColCon;     // color contrast description (4 bytes as given in HLI)
} AM_PROPERTY_SPHLI, *PAM_PROPERTY_SPHLI;

typedef BOOL AM_PROPERTY_COMPOSIT_ON, *PAM_PROPERTY_COMPOSIT_ON;



// -----------------------------------------------------------------------
// copy protection definitions
// -----------------------------------------------------------------------

// AM_UseNewCSSKey for the dwTypeSpecificFlags in IMediaSample2 to indicate
// the exact point in a stream after which to start applying a new CSS key.
// This is typically sent on an empty media sample just before attempting 
// to renegotiate a CSS key.
#define AM_UseNewCSSKey    0x1

//
// AM_KSPROPSETID_CopyProt property set definitions
//
typedef enum {
	AM_PROPERTY_DVDCOPY_CHLG_KEY = 0x01,
	AM_PROPERTY_DVDCOPY_DVD_KEY1,
	AM_PROPERTY_DVDCOPY_DEC_KEY2,
	AM_PROPERTY_DVDCOPY_TITLE_KEY,
	AM_PROPERTY_COPY_MACROVISION,
	AM_PROPERTY_DVDCOPY_REGION,
	AM_PROPERTY_DVDCOPY_SET_COPY_STATE,
	AM_PROPERTY_DVDCOPY_DISC_KEY = 0x80
} AM_PROPERTY_DVDCOPYPROT;

typedef struct _AM_DVDCOPY_CHLGKEY {
	BYTE ChlgKey[10];
	BYTE Reserved[2];
} AM_DVDCOPY_CHLGKEY, *PAM_DVDCOPY_CHLGKEY;

typedef struct _AM_DVDCOPY_BUSKEY {
	BYTE BusKey[5];
	BYTE Reserved[1];
} AM_DVDCOPY_BUSKEY, *PAM_DVDCOPY_BUSKEY;

typedef struct _AM_DVDCOPY_DISCKEY {
	BYTE DiscKey[2048];
} AM_DVDCOPY_DISCKEY, *PAM_DVDCOPY_DISCKEY;

typedef struct AM_DVDCOPY_TITLEKEY {
	ULONG KeyFlags;
	ULONG Reserved1[2];
    UCHAR TitleKey[6];
	UCHAR Reserved2[2];
} AM_DVDCOPY_TITLEKEY, *PAM_DVDCOPY_TITLEKEY;

typedef struct _AM_COPY_MACROVISION {
	ULONG MACROVISIONLevel;
} AM_COPY_MACROVISION, *PAM_COPY_MACROVISION;

typedef struct AM_DVDCOPY_SET_COPY_STATE {
	ULONG DVDCopyState;
} AM_DVDCOPY_SET_COPY_STATE, *PAM_DVDCOPY_SET_COPY_STATE;

typedef enum {
	AM_DVDCOPYSTATE_INITIALIZE,
	AM_DVDCOPYSTATE_INITIALIZE_TITLE,	// indicates we are starting a title
										// key copy protection sequence
	AM_DVDCOPYSTATE_AUTHENTICATION_NOT_REQUIRED,
	AM_DVDCOPYSTATE_AUTHENTICATION_REQUIRED,
	AM_DVDCOPYSTATE_DONE
} AM_DVDCOPYSTATE;

typedef enum {
	AM_MACROVISION_DISABLED,
	AM_MACROVISION_LEVEL1,
	AM_MACROVISION_LEVEL2,
	AM_MACROVISION_LEVEL3
} AM_COPY_MACROVISION_LEVEL, *PAM_COPY_MACROVISION_LEVEL;


// CSS region stucture
typedef struct _DVD_REGION {
    UCHAR CopySystem;
    UCHAR RegionData;
	UCHAR SystemRegion;
    UCHAR Reserved;
} DVD_REGION, *PDVD_REGION;	

//
// CGMS Copy Protection Flags
//

#define AM_DVD_CGMS_RESERVED_MASK      0x00000078

#define AM_DVD_CGMS_COPY_PROTECT_MASK  0x00000018
#define AM_DVD_CGMS_COPY_PERMITTED     0x00000000
#define AM_DVD_CGMS_COPY_ONCE          0x00000010
#define AM_DVD_CGMS_NO_COPY            0x00000018

#define AM_DVD_COPYRIGHT_MASK          0x00000040
#define AM_DVD_NOT_COPYRIGHTED         0x00000000
#define AM_DVD_COPYRIGHTED             0x00000040

#define AM_DVD_SECTOR_PROTECT_MASK     0x00000020
#define AM_DVD_SECTOR_NOT_PROTECTED    0x00000000
#define AM_DVD_SECTOR_PROTECTED        0x00000020


// -----------------------------------------------------------------------
// video format blocks
// -----------------------------------------------------------------------

enum AM_MPEG2Level {
	AM_MPEG2Level_Low = 1,
	AM_MPEG2Level_Main,
	AM_MPEG2Level_High1440,
	AM_MPEG2Level_High
};

enum AM_MPEG2Profile {
	AM_MPEG2Profile_Simple = 1,
	AM_MPEG2Profile_Main,
	AM_MPEG2Profile_SNRScalable,
	AM_MPEG2Profile_SpatiallyScalable,
	AM_MPEG2Profile_High
};


#define AMMPEG2_DoPanScan           0x00000001  //if set, the MPEG-2 video decoder should crop output image 
						//  based on pan-scan vectors in picture_display_extension 
						//  and change the picture aspect ratio accordingly.
#define AMMPEG2_DVDLine21Field1     0x00000002  //if set, the MPEG-2 decoder must be able to produce an output 
						//  pin for DVD style closed caption data found in GOP layer of field 1
#define AMMPEG2_DVDLine21Field2     0x00000004  //if set, the MPEG-2 decoder must be able to produce an output 
						//  pin for DVD style closed caption data found in GOP layer of field 2
#define AMMPEG2_SourceIsLetterboxed 0x00000008  //if set, indicates that black bars have been encoded in the top 
						//  and bottom of the video.
#define AMMPEG2_FilmCameraMode      0x00000010  //if set, indicates "film mode" used for 625/50 content.  If cleared,
						//  indicates that "camera mode" was used.
#define AMMPEG2_LetterboxAnalogOut  0x00000020  //if set and this stream is sent to an analog output, it should 
                        //  be letterboxed.  Streams sent to VGA should be letterboxed only by renderers.


typedef struct tagMPEG2VIDEOINFO {
	VIDEOINFOHEADER2    hdr;
	DWORD               dwStartTimeCode;        //  ?? not used for DVD ??
	DWORD               cbSequenceHeader;       // is 0 for DVD (no sequence header)
	DWORD               dwProfile;              // use enum MPEG2Profile   
	DWORD               dwLevel;                // use enum MPEG2Level
	DWORD               dwFlags;                // use AMMPEG2_* defines.  Reject connection if undefined bits are not 0
	DWORD               dwSequenceHeader[1];    // DWORD instead of Byte for alignment purposes
												//   For MPEG-2, if a sequence_header is included, the sequence_extension 
												//   should also be included
} MPEG2VIDEOINFO;

#define SIZE_MPEG2VIDEOINFO(pv) (FIELD_OFFSET(MPEG2VIDEOINFO, bSequenceHeader[0]) + (pv)->cbSequenceHeader)
#define MPEG1_SEQUENCE_INFO(pv) ((const BYTE *)(pv)->bSequenceHeader)


//===================================================================================
// flags for dwTypeSpecificFlags in AM_SAMPLE2_PROPERTIES which define type specific
// data in IMediaSample2
//===================================================================================

#define AM_VIDEO_FLAG_FIELD_MASK	    0x0003L	// use this mask to check whether the sample is field1 or field2 or frame
#define AM_VIDEO_FLAG_INTERLEAVED_FRAME	    0x0000L     // the sample is a frame (remember to use AM_VIDEO_FLAG_FIELD_MASK when using this)
#define AM_VIDEO_FLAG_FIELD1		    0x0001L	// the sample is field1 (remember to use AM_VIDEO_FLAG_FIELD_MASK when using this)
#define AM_VIDEO_FLAG_FIELD2		    0x0002L	// the sample is the field2 (remember to use AM_VIDEO_FLAG_FIELD_MASK when using this)
#define AM_VIDEO_FLAG_FIELD1FIRST	    0x0004L	// if set means display field1 first, else display field2 first.
							// this bit is irrelavant for 1FieldPerSample mode
#define AM_VIDEO_FLAG_WEAVE    		    0x0008L	// if set use bob display mode else weave
#define AM_VIDEO_FLAG_IPB_MASK		    0x0030L	// use this mask to check whether the sample is I, P or B
#define AM_VIDEO_FLAG_I_SAMPLE		    0x0000L     // I Sample (remember to use AM_VIDEO_FLAG_IPB_MASK when using this)
#define AM_VIDEO_FLAG_P_SAMPLE		    0x0010L	// P Sample (remember to use AM_VIDEO_FLAG_IPB_MASK when using this)
#define AM_VIDEO_FLAG_B_SAMPLE		    0x0020L	// B Sample (remember to use AM_VIDEO_FLAG_IPB_MASK when using this)
#define AM_VIDEO_FLAG_REPEAT_FIELD	    0x0040L	// if set means display the field which has been displayed first again after displaying 
							// both fields first. This bit is irrelavant for 1FieldPerSample mode
#define AM_VIDEO_FLAG_NO_OUTPUT         0x80000000 // Decode, but don't display, this frame - can't be used with codecs which require the previous frame's
                                                   // data.  Useful for WMV output from the DMO, when the video-renderer's video-window is hidden, but the
                                                   // DMO was given the primary surface (visible display-memory) to draw into.

// -----------------------------------------------------------------------
// AM_KSPROPSETID_TSRateChange property set definitions for time stamp
// rate changes.
// -----------------------------------------------------------------------

typedef enum {
	AM_RATE_SimpleRateChange=1,   // rw, use AM_SimpleRateChange
	AM_RATE_ExactRateChange=2,	// rw, use AM_ExactRateChange
	AM_RATE_MaxFullDataRate=3,	// r, use AM_MaxFullDataRate
	AM_RATE_Step=4			// w, use AM_Step
} AM_PROPERTY_TS_RATE_CHANGE;

typedef struct {
	// this is the simplest mechinism to set a time stamp rate change on
	// a filter (simplest for the person setting the rate change, harder
	// for the filter doing the rate change).  
	REFERENCE_TIME	StartTime;  //stream time at which to start this rate
	LONG		Rate;       //new rate * 10000 (decimal)
} AM_SimpleRateChange;

typedef struct {
	REFERENCE_TIME  OutputZeroTime; //input TS that maps to zero output TS
	LONG		Rate;       //new rate * 10000 (decimal)
} AM_ExactRateChange;

typedef LONG AM_MaxFullDataRate; //rate * 10000 (decimal)

typedef DWORD AM_Step; // number of frame to step


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __DVDMEDIA_H__
