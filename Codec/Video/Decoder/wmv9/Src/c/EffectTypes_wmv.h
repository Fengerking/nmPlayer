//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 2000

Module Name:

	EffectTypes.hpp

Abstract:

    Defines the transition and motion effect types for PhotoMotion.

*************************************************************************/

#ifndef _EFFECTTYPES_HPP
#define _EFFECTTYPES_HPP

// Motion Effects
#define WMVP_MOTION_BASE				0
#define WMVP_MOTION_PAN					WMVP_MOTION_BASE + 1
#define WMVP_MOTION_ROTATE				WMVP_MOTION_BASE + 2
#define WMVP_MOTION_ZOOM				WMVP_MOTION_BASE + 3

// Transition Effects
#define WMVP_TRANSITION_BASE			10
#define WMVP_TRANSITION_BOW_TIE			WMVP_TRANSITION_BASE + 1
#define WMVP_TRANSITION_CIRCLE			WMVP_TRANSITION_BASE + 2
#define WMVP_TRANSITION_CROSS_FADE		WMVP_TRANSITION_BASE + 3
#define WMVP_TRANSITION_DIAGONAL		WMVP_TRANSITION_BASE + 4
#define WMVP_TRANSITION_DIAMOND			WMVP_TRANSITION_BASE + 5
#define WMVP_TRANSITION_FADE_TO_COLOR	WMVP_TRANSITION_BASE + 6
#define WMVP_TRANSITION_FILLED_V		WMVP_TRANSITION_BASE + 7
#define WMVP_TRANSITION_FLIP			WMVP_TRANSITION_BASE + 8
#define WMVP_TRANSITION_INSET			WMVP_TRANSITION_BASE + 9
#define WMVP_TRANSITION_IRIS			WMVP_TRANSITION_BASE + 10
#define WMVP_TRANSITION_PAGE_ROLL		WMVP_TRANSITION_BASE + 11
#define WMVP_TRANSITION_PUSH			WMVP_TRANSITION_BASE + 12
#define WMVP_TRANSITION_RECTANGLE		WMVP_TRANSITION_BASE + 13
#define WMVP_TRANSITION_REVEAL			WMVP_TRANSITION_BASE + 14
#define WMVP_TRANSITION_ROTATE			WMVP_TRANSITION_BASE + 15
#define WMVP_TRANSITION_SHATTER			WMVP_TRANSITION_BASE + 16
#define WMVP_TRANSITION_SLIDE			WMVP_TRANSITION_BASE + 17
#define WMVP_TRANSITION_SPIN			WMVP_TRANSITION_BASE + 18
#define WMVP_TRANSITION_SPLIT			WMVP_TRANSITION_BASE + 19
#define WMVP_TRANSITION_STAR			WMVP_TRANSITION_BASE + 20
#define WMVP_TRANSITION_WHEEL			WMVP_TRANSITION_BASE + 21
#define WMVP_TRANSITION_WIPE			WMVP_TRANSITION_BASE + 22

// Directions
#define WMVP_HORIZONTAL					0
#define WMVP_VERTICAL					1

#define WMVP_CLOCKWISE					2
#define WMVP_COUNTER_CLOCKWISE			3

#define WMVP_INWARD						4
#define WMVP_OUTWARD					5

#define WMVP_NORMAL					    0
#define WMVP_REVERSED					1

#define WMVP_RIGHT						6
#define WMVP_LEFT						7
#define WMVP_UP							8
#define WMVP_DOWN						9

#define WMVP_UP_RIGHT					10
#define WMVP_UP_LEFT					11
#define WMVP_DOWN_RIGHT					12
#define WMVP_DOWN_LEFT					13


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Effect parameters uiNumAffineCoefficients
#define WMVP_MAX_NUM_EFFECT_PARAMETERS	100
#define WMVP_MAX_NUM_AFFINE_COFFICIENTS	14

typedef struct _WMVIMAGE_EFFECTS{
    I32_WMV m_iImageWidth, m_iImageHeight;
    I32_WMV m_iCenterX, m_iCenterY;
    I32_WMV m_iCompositionOrder;
    I32_WMV *m_pOuterLeftX, *m_pInnerLeftX;
    I32_WMV *m_pOuterRightX, *m_pInnerRightX;

    I32_WMV m_iBackgroundRed, m_iBackgroundGreen, m_iBackgroundBlue;
    Float_WMV m_fBackgroundWeight;

    Float_WMV m_fFlipAngle, m_fFocalLength;
    Bool_WMV *m_pFlipFlag;
    I32_WMV *m_pFlipX;
    I32_WMV *m_pFlipYRatio;
}WMVIMAGE_EFFECTS;


#endif  // _EFFECTTYPES_HPP
