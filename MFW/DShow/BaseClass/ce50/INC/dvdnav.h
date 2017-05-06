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

#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Thu Jun 24 19:41:58 2004
 */
/* Compiler settings for .\dvdnav.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __dvdnav_h__
#define __dvdnav_h__

/* Forward Declarations */ 

#ifndef __IDVDUserOperation_FWD_DEFINED__
#define __IDVDUserOperation_FWD_DEFINED__
typedef interface IDVDUserOperation IDVDUserOperation;
#endif 	/* __IDVDUserOperation_FWD_DEFINED__ */


#ifndef __IDVDNavigationManager_FWD_DEFINED__
#define __IDVDNavigationManager_FWD_DEFINED__
typedef interface IDVDNavigationManager IDVDNavigationManager;
#endif 	/* __IDVDNavigationManager_FWD_DEFINED__ */


#ifndef __IDVDNavigatorSink_FWD_DEFINED__
#define __IDVDNavigatorSink_FWD_DEFINED__
typedef interface IDVDNavigatorSink IDVDNavigatorSink;
#endif 	/* __IDVDNavigatorSink_FWD_DEFINED__ */


#ifndef __DVDNavigationManager_FWD_DEFINED__
#define __DVDNavigationManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class DVDNavigationManager DVDNavigationManager;
#else
typedef struct DVDNavigationManager DVDNavigationManager;
#endif /* __cplusplus */

#endif 	/* __DVDNavigationManager_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "dvdcom.h"
#include "dvdata.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_dvdnav_0000 */
/* [local] */ 

#define	DVD_PROGRAM_PREVIOUS 0
#define	DVD_PROGRAM_NEXT 1
#define	DVD_PROGRAM_TOP 2
#define	DVD_MENU_TITLE 0
#define	DVD_MENU_ROOT 1
#define	DVD_MENU_SUBPICTURE 2
#define	DVD_MENU_AUDIO 3
#define	DVD_MENU_ANGLE 4
#define	DVD_MENU_PTT 5
#define	SPRM_M_LCD 0
#define	SPRM_ASTN 1
#define	SPRM_SPSTN 2
#define	SPRM_AGLN 3
#define	SPRM_TTN 4
#define	SPRM_VTS_TTN 5
#define	SPRM_TT_PGCN 6
#define	SPRM_PTTN 7
#define	SPRM_HL_BTNN 8
#define	SPRM_NV_TMR 9
#define	SPRM_NV_TMR_PGCN 10
#define	SPRM_P_AMXMD 11
#define	SPRM_CTY_CD 12
#define	SPRM_PTL_LVL 13
#define	SPRM_P_CFG_VIDEO 14
#define	SPRM_P_CFG_AUDIO 15
#define	SPRM_INI_LCD_AST 16
#define	SPRM_INI_LCD_EXT_AST 17
#define	SPRM_INI_LCD_SPST 18
#define	SPRM_INI_LCD_EXT_SPST 19
#define	SPRM_REGION 20
#ifndef _DVD_SCANCAPS_DEFINED_
#define _DVD_SCANCAPS_DEFINED_
#define DVD_SCANCAP_FORWARD_SCAN		0x1
#define DVD_SCANCAP_FORWARD_SLOW		0x2
#define DVD_SCANCAP_FORWARD_SINGLE	0x4
#define DVD_SCANCAP_BACKWARD_SCAN	0x8
#define DVD_SCANCAP_BACKWARD_SLOW	0x10
#define DVD_SCANCAP_BACKWARD_SINGLE	0x20
#endif
typedef /* [public][public] */ struct __MIDL___MIDL_itf_dvdnav_0000_0001
    {
    UINT8 userData[ 201 ];
    }	DVDUserGOPData;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_dvdnav_0000_0002
    {	DVD_STOPPED	= 0,
	DVD_PAUSED	= DVD_STOPPED + 1,
	DVD_STILLED	= DVD_PAUSED + 1,
	DVD_NORMAL_PLAY	= DVD_STILLED + 1,
	DVD_FORWARD_SCAN	= DVD_NORMAL_PLAY + 1,
	DVD_FORWARD_SLOW	= DVD_FORWARD_SCAN + 1,
	DVD_FORWARD_SINGLE	= DVD_FORWARD_SLOW + 1,
	DVD_BACKWARD_SCAN	= DVD_FORWARD_SINGLE + 1,
	DVD_BACKWARD_SLOW	= DVD_BACKWARD_SCAN + 1,
	DVD_BACKWARD_SINGLE	= DVD_BACKWARD_SLOW + 1
    }	EDDVDPlaybackState;




extern RPC_IF_HANDLE __MIDL_itf_dvdnav_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dvdnav_0000_v0_0_s_ifspec;

#ifndef __IDVDUserOperation_INTERFACE_DEFINED__
#define __IDVDUserOperation_INTERFACE_DEFINED__

/* interface IDVDUserOperation */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDUserOperation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("07C89F52-C72E-11D0-A985-00A0C9050598")
    IDVDUserOperation : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE TitlePlay( 
            BYTE byteTitleNumber) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PartPlay( 
            BYTE byteTitleNumber,
            USHORT uhPartOfTheTitle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TimePlay( 
            BYTE byteTitleNumber,
            UINT uiBCDTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GoUp( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TimeSearch( 
            UINT uiBCDTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PTTSearch( 
            USHORT uhPartOfTheTitle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ProgramSearch( 
            BYTE byteProgramType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ForwardScan( 
            double dblSpeed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BackwardScan( 
            double dblSpeed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MenuCall( 
            BYTE byteMenu_ID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ButtonSelect( 
            BYTE byteDirection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ButtonActivate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ButtonSelectAndActivate( 
            BYTE byteButtonNumber) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StillOff( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( 
            BOOL bOnOff) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MenuLanguageSelect( 
            USHORT uhLanguageCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangeAudioStream( 
            BYTE byteStreamNumber) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangeSubpictureStream( 
            BYTE streamNumber,
            BOOL bDisplayFlag) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangeAngle( 
            BYTE byteAngleNumber) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParentalLevelSelect( 
            BYTE byteLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParentalCountrySelect( 
            USHORT uhCountryCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE KaraokeAudioModePresentationChange( 
            USHORT uhKaraokeMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE VideoPresentationModeChange( 
            USHORT uhVideoMode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDUserOperationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDUserOperation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDUserOperation __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDUserOperation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TitlePlay )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteTitleNumber);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PartPlay )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteTitleNumber,
            USHORT uhPartOfTheTitle);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TimePlay )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteTitleNumber,
            UINT uiBCDTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IDVDUserOperation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GoUp )( 
            IDVDUserOperation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TimeSearch )( 
            IDVDUserOperation __RPC_FAR * This,
            UINT uiBCDTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PTTSearch )( 
            IDVDUserOperation __RPC_FAR * This,
            USHORT uhPartOfTheTitle);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ProgramSearch )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteProgramType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ForwardScan )( 
            IDVDUserOperation __RPC_FAR * This,
            double dblSpeed);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BackwardScan )( 
            IDVDUserOperation __RPC_FAR * This,
            double dblSpeed);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MenuCall )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteMenu_ID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resume )( 
            IDVDUserOperation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ButtonSelect )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteDirection);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ButtonActivate )( 
            IDVDUserOperation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ButtonSelectAndActivate )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteButtonNumber);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StillOff )( 
            IDVDUserOperation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IDVDUserOperation __RPC_FAR * This,
            BOOL bOnOff);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MenuLanguageSelect )( 
            IDVDUserOperation __RPC_FAR * This,
            USHORT uhLanguageCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeAudioStream )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteStreamNumber);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeSubpictureStream )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE streamNumber,
            BOOL bDisplayFlag);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeAngle )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteAngleNumber);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ParentalLevelSelect )( 
            IDVDUserOperation __RPC_FAR * This,
            BYTE byteLevel);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ParentalCountrySelect )( 
            IDVDUserOperation __RPC_FAR * This,
            USHORT uhCountryCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *KaraokeAudioModePresentationChange )( 
            IDVDUserOperation __RPC_FAR * This,
            USHORT uhKaraokeMode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VideoPresentationModeChange )( 
            IDVDUserOperation __RPC_FAR * This,
            USHORT uhVideoMode);
        
        END_INTERFACE
    } IDVDUserOperationVtbl;

    interface IDVDUserOperation
    {
        CONST_VTBL struct IDVDUserOperationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDUserOperation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDUserOperation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDUserOperation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDUserOperation_TitlePlay(This,byteTitleNumber)	\
    (This)->lpVtbl -> TitlePlay(This,byteTitleNumber)

#define IDVDUserOperation_PartPlay(This,byteTitleNumber,uhPartOfTheTitle)	\
    (This)->lpVtbl -> PartPlay(This,byteTitleNumber,uhPartOfTheTitle)

#define IDVDUserOperation_TimePlay(This,byteTitleNumber,uiBCDTime)	\
    (This)->lpVtbl -> TimePlay(This,byteTitleNumber,uiBCDTime)

#define IDVDUserOperation_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IDVDUserOperation_GoUp(This)	\
    (This)->lpVtbl -> GoUp(This)

#define IDVDUserOperation_TimeSearch(This,uiBCDTime)	\
    (This)->lpVtbl -> TimeSearch(This,uiBCDTime)

#define IDVDUserOperation_PTTSearch(This,uhPartOfTheTitle)	\
    (This)->lpVtbl -> PTTSearch(This,uhPartOfTheTitle)

#define IDVDUserOperation_ProgramSearch(This,byteProgramType)	\
    (This)->lpVtbl -> ProgramSearch(This,byteProgramType)

#define IDVDUserOperation_ForwardScan(This,dblSpeed)	\
    (This)->lpVtbl -> ForwardScan(This,dblSpeed)

#define IDVDUserOperation_BackwardScan(This,dblSpeed)	\
    (This)->lpVtbl -> BackwardScan(This,dblSpeed)

#define IDVDUserOperation_MenuCall(This,byteMenu_ID)	\
    (This)->lpVtbl -> MenuCall(This,byteMenu_ID)

#define IDVDUserOperation_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#define IDVDUserOperation_ButtonSelect(This,byteDirection)	\
    (This)->lpVtbl -> ButtonSelect(This,byteDirection)

#define IDVDUserOperation_ButtonActivate(This)	\
    (This)->lpVtbl -> ButtonActivate(This)

#define IDVDUserOperation_ButtonSelectAndActivate(This,byteButtonNumber)	\
    (This)->lpVtbl -> ButtonSelectAndActivate(This,byteButtonNumber)

#define IDVDUserOperation_StillOff(This)	\
    (This)->lpVtbl -> StillOff(This)

#define IDVDUserOperation_Pause(This,bOnOff)	\
    (This)->lpVtbl -> Pause(This,bOnOff)

#define IDVDUserOperation_MenuLanguageSelect(This,uhLanguageCode)	\
    (This)->lpVtbl -> MenuLanguageSelect(This,uhLanguageCode)

#define IDVDUserOperation_ChangeAudioStream(This,byteStreamNumber)	\
    (This)->lpVtbl -> ChangeAudioStream(This,byteStreamNumber)

#define IDVDUserOperation_ChangeSubpictureStream(This,streamNumber,bDisplayFlag)	\
    (This)->lpVtbl -> ChangeSubpictureStream(This,streamNumber,bDisplayFlag)

#define IDVDUserOperation_ChangeAngle(This,byteAngleNumber)	\
    (This)->lpVtbl -> ChangeAngle(This,byteAngleNumber)

#define IDVDUserOperation_ParentalLevelSelect(This,byteLevel)	\
    (This)->lpVtbl -> ParentalLevelSelect(This,byteLevel)

#define IDVDUserOperation_ParentalCountrySelect(This,uhCountryCode)	\
    (This)->lpVtbl -> ParentalCountrySelect(This,uhCountryCode)

#define IDVDUserOperation_KaraokeAudioModePresentationChange(This,uhKaraokeMode)	\
    (This)->lpVtbl -> KaraokeAudioModePresentationChange(This,uhKaraokeMode)

#define IDVDUserOperation_VideoPresentationModeChange(This,uhVideoMode)	\
    (This)->lpVtbl -> VideoPresentationModeChange(This,uhVideoMode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDUserOperation_TitlePlay_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteTitleNumber);


void __RPC_STUB IDVDUserOperation_TitlePlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_PartPlay_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteTitleNumber,
    USHORT uhPartOfTheTitle);


void __RPC_STUB IDVDUserOperation_PartPlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_TimePlay_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteTitleNumber,
    UINT uiBCDTime);


void __RPC_STUB IDVDUserOperation_TimePlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_Stop_Proxy( 
    IDVDUserOperation __RPC_FAR * This);


void __RPC_STUB IDVDUserOperation_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_GoUp_Proxy( 
    IDVDUserOperation __RPC_FAR * This);


void __RPC_STUB IDVDUserOperation_GoUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_TimeSearch_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    UINT uiBCDTime);


void __RPC_STUB IDVDUserOperation_TimeSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_PTTSearch_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    USHORT uhPartOfTheTitle);


void __RPC_STUB IDVDUserOperation_PTTSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ProgramSearch_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteProgramType);


void __RPC_STUB IDVDUserOperation_ProgramSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ForwardScan_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    double dblSpeed);


void __RPC_STUB IDVDUserOperation_ForwardScan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_BackwardScan_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    double dblSpeed);


void __RPC_STUB IDVDUserOperation_BackwardScan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_MenuCall_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteMenu_ID);


void __RPC_STUB IDVDUserOperation_MenuCall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_Resume_Proxy( 
    IDVDUserOperation __RPC_FAR * This);


void __RPC_STUB IDVDUserOperation_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ButtonSelect_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteDirection);


void __RPC_STUB IDVDUserOperation_ButtonSelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ButtonActivate_Proxy( 
    IDVDUserOperation __RPC_FAR * This);


void __RPC_STUB IDVDUserOperation_ButtonActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ButtonSelectAndActivate_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteButtonNumber);


void __RPC_STUB IDVDUserOperation_ButtonSelectAndActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_StillOff_Proxy( 
    IDVDUserOperation __RPC_FAR * This);


void __RPC_STUB IDVDUserOperation_StillOff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_Pause_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BOOL bOnOff);


void __RPC_STUB IDVDUserOperation_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_MenuLanguageSelect_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    USHORT uhLanguageCode);


void __RPC_STUB IDVDUserOperation_MenuLanguageSelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ChangeAudioStream_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteStreamNumber);


void __RPC_STUB IDVDUserOperation_ChangeAudioStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ChangeSubpictureStream_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE streamNumber,
    BOOL bDisplayFlag);


void __RPC_STUB IDVDUserOperation_ChangeSubpictureStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ChangeAngle_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteAngleNumber);


void __RPC_STUB IDVDUserOperation_ChangeAngle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ParentalLevelSelect_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    BYTE byteLevel);


void __RPC_STUB IDVDUserOperation_ParentalLevelSelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_ParentalCountrySelect_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    USHORT uhCountryCode);


void __RPC_STUB IDVDUserOperation_ParentalCountrySelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_KaraokeAudioModePresentationChange_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    USHORT uhKaraokeMode);


void __RPC_STUB IDVDUserOperation_KaraokeAudioModePresentationChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDUserOperation_VideoPresentationModeChange_Proxy( 
    IDVDUserOperation __RPC_FAR * This,
    USHORT uhVideoMode);


void __RPC_STUB IDVDUserOperation_VideoPresentationModeChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDUserOperation_INTERFACE_DEFINED__ */


#ifndef __IDVDNavigationManager_INTERFACE_DEFINED__
#define __IDVDNavigationManager_INTERFACE_DEFINED__

/* interface IDVDNavigationManager */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDNavigationManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("07C89F53-C72E-11D0-A985-00A0C9050598")
    IDVDNavigationManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetVolume( 
            IDVDVideoVolume __RPC_FAR *pVolume) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSParameter( 
            UINT8 no,
            UINT16 __RPC_FAR *pValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetGParameter( 
            UINT8 no,
            UINT16 __RPC_FAR *pValue,
            BOOL __RPC_FAR *pbCounterMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSParameter( 
            UINT8 no,
            UINT16 wValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetScanCaps( 
            UINT16 __RPC_FAR *pMask) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPlaybackState( 
            EDDVDPlaybackState __RPC_FAR *peState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UseRenderer( 
            WCHAR __RPC_FAR *pszRendererName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ButtonSelectDirect( 
            BYTE byteButtonNumber) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDNavigationManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDNavigationManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDNavigationManager __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDNavigationManager __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetVolume )( 
            IDVDNavigationManager __RPC_FAR * This,
            IDVDVideoVolume __RPC_FAR *pVolume);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSParameter )( 
            IDVDNavigationManager __RPC_FAR * This,
            UINT8 no,
            UINT16 __RPC_FAR *pValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetGParameter )( 
            IDVDNavigationManager __RPC_FAR * This,
            UINT8 no,
            UINT16 __RPC_FAR *pValue,
            BOOL __RPC_FAR *pbCounterMode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetSParameter )( 
            IDVDNavigationManager __RPC_FAR * This,
            UINT8 no,
            UINT16 wValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetScanCaps )( 
            IDVDNavigationManager __RPC_FAR * This,
            UINT16 __RPC_FAR *pMask);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPlaybackState )( 
            IDVDNavigationManager __RPC_FAR * This,
            EDDVDPlaybackState __RPC_FAR *peState);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UseRenderer )( 
            IDVDNavigationManager __RPC_FAR * This,
            WCHAR __RPC_FAR *pszRendererName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ButtonSelectDirect )( 
            IDVDNavigationManager __RPC_FAR * This,
            BYTE byteButtonNumber);
        
        END_INTERFACE
    } IDVDNavigationManagerVtbl;

    interface IDVDNavigationManager
    {
        CONST_VTBL struct IDVDNavigationManagerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDNavigationManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDNavigationManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDNavigationManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDNavigationManager_SetVolume(This,pVolume)	\
    (This)->lpVtbl -> SetVolume(This,pVolume)

#define IDVDNavigationManager_GetSParameter(This,no,pValue)	\
    (This)->lpVtbl -> GetSParameter(This,no,pValue)

#define IDVDNavigationManager_GetGParameter(This,no,pValue,pbCounterMode)	\
    (This)->lpVtbl -> GetGParameter(This,no,pValue,pbCounterMode)

#define IDVDNavigationManager_SetSParameter(This,no,wValue)	\
    (This)->lpVtbl -> SetSParameter(This,no,wValue)

#define IDVDNavigationManager_GetScanCaps(This,pMask)	\
    (This)->lpVtbl -> GetScanCaps(This,pMask)

#define IDVDNavigationManager_GetPlaybackState(This,peState)	\
    (This)->lpVtbl -> GetPlaybackState(This,peState)

#define IDVDNavigationManager_UseRenderer(This,pszRendererName)	\
    (This)->lpVtbl -> UseRenderer(This,pszRendererName)

#define IDVDNavigationManager_ButtonSelectDirect(This,byteButtonNumber)	\
    (This)->lpVtbl -> ButtonSelectDirect(This,byteButtonNumber)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDNavigationManager_SetVolume_Proxy( 
    IDVDNavigationManager __RPC_FAR * This,
    IDVDVideoVolume __RPC_FAR *pVolume);


void __RPC_STUB IDVDNavigationManager_SetVolume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigationManager_GetSParameter_Proxy( 
    IDVDNavigationManager __RPC_FAR * This,
    UINT8 no,
    UINT16 __RPC_FAR *pValue);


void __RPC_STUB IDVDNavigationManager_GetSParameter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigationManager_GetGParameter_Proxy( 
    IDVDNavigationManager __RPC_FAR * This,
    UINT8 no,
    UINT16 __RPC_FAR *pValue,
    BOOL __RPC_FAR *pbCounterMode);


void __RPC_STUB IDVDNavigationManager_GetGParameter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigationManager_SetSParameter_Proxy( 
    IDVDNavigationManager __RPC_FAR * This,
    UINT8 no,
    UINT16 wValue);


void __RPC_STUB IDVDNavigationManager_SetSParameter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigationManager_GetScanCaps_Proxy( 
    IDVDNavigationManager __RPC_FAR * This,
    UINT16 __RPC_FAR *pMask);


void __RPC_STUB IDVDNavigationManager_GetScanCaps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigationManager_GetPlaybackState_Proxy( 
    IDVDNavigationManager __RPC_FAR * This,
    EDDVDPlaybackState __RPC_FAR *peState);


void __RPC_STUB IDVDNavigationManager_GetPlaybackState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigationManager_UseRenderer_Proxy( 
    IDVDNavigationManager __RPC_FAR * This,
    WCHAR __RPC_FAR *pszRendererName);


void __RPC_STUB IDVDNavigationManager_UseRenderer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigationManager_ButtonSelectDirect_Proxy( 
    IDVDNavigationManager __RPC_FAR * This,
    BYTE byteButtonNumber);


void __RPC_STUB IDVDNavigationManager_ButtonSelectDirect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDNavigationManager_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_dvdnav_0115 */
/* [local] */ 

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_dvdnav_0115_0001
    {	InsufficientParentalLevel	= 0,
	FatalNavigationError	= InsufficientParentalLevel + 1
    }	EDVDNavException;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_dvdnav_0115_0002
    {	InitialAccess	= 0,
	TitlePartTimePlay	= InitialAccess + 1,
	MenuCallToVmgmDom	= TitlePartTimePlay + 1,
	MenuCallToVtsmDom	= MenuCallToVmgmDom + 1
    }	EDVDSParamEvent;



extern RPC_IF_HANDLE __MIDL_itf_dvdnav_0115_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dvdnav_0115_v0_0_s_ifspec;

#ifndef __IDVDNavigatorSink_INTERFACE_DEFINED__
#define __IDVDNavigatorSink_INTERFACE_DEFINED__

/* interface IDVDNavigatorSink */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDNavigatorSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("212CD034-45B4-11d5-84F2-00C04F68EB0F")
    IDVDNavigatorSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnSPRMChange( 
            UINT8 no,
            UINT16 wValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnGPRMChange( 
            UINT8 no,
            UINT16 wValue,
            BOOL bCounter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnNewProgramChain( 
            IDVDProgramChain __RPC_FAR *pChain) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnNewProgram( 
            UINT8 progNo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnNewCell( 
            UINT8 cellNo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnGOPUserData( 
            DVDUserGOPData __RPC_FAR *pData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnUOPChange( 
            UINT32 newUOPMask) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnPlaybackStateChange( 
            EDDVDPlaybackState eState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnDiskReadError( 
            DWORD osCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConfirmTemporaryParentalLevel( 
            UINT8 requestedLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnNavigationException( 
            EDVDNavException excCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeSystemParameters( 
            EDVDSParamEvent eventCode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDNavigatorSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDNavigatorSink __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDNavigatorSink __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDNavigatorSink __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnSPRMChange )( 
            IDVDNavigatorSink __RPC_FAR * This,
            UINT8 no,
            UINT16 wValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnGPRMChange )( 
            IDVDNavigatorSink __RPC_FAR * This,
            UINT8 no,
            UINT16 wValue,
            BOOL bCounter);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnNewProgramChain )( 
            IDVDNavigatorSink __RPC_FAR * This,
            IDVDProgramChain __RPC_FAR *pChain);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnNewProgram )( 
            IDVDNavigatorSink __RPC_FAR * This,
            UINT8 progNo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnNewCell )( 
            IDVDNavigatorSink __RPC_FAR * This,
            UINT8 cellNo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnGOPUserData )( 
            IDVDNavigatorSink __RPC_FAR * This,
            DVDUserGOPData __RPC_FAR *pData);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnUOPChange )( 
            IDVDNavigatorSink __RPC_FAR * This,
            UINT32 newUOPMask);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnPlaybackStateChange )( 
            IDVDNavigatorSink __RPC_FAR * This,
            EDDVDPlaybackState eState);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnDiskReadError )( 
            IDVDNavigatorSink __RPC_FAR * This,
            DWORD osCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConfirmTemporaryParentalLevel )( 
            IDVDNavigatorSink __RPC_FAR * This,
            UINT8 requestedLevel);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnNavigationException )( 
            IDVDNavigatorSink __RPC_FAR * This,
            EDVDNavException excCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitializeSystemParameters )( 
            IDVDNavigatorSink __RPC_FAR * This,
            EDVDSParamEvent eventCode);
        
        END_INTERFACE
    } IDVDNavigatorSinkVtbl;

    interface IDVDNavigatorSink
    {
        CONST_VTBL struct IDVDNavigatorSinkVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDNavigatorSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDNavigatorSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDNavigatorSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDNavigatorSink_OnSPRMChange(This,no,wValue)	\
    (This)->lpVtbl -> OnSPRMChange(This,no,wValue)

#define IDVDNavigatorSink_OnGPRMChange(This,no,wValue,bCounter)	\
    (This)->lpVtbl -> OnGPRMChange(This,no,wValue,bCounter)

#define IDVDNavigatorSink_OnNewProgramChain(This,pChain)	\
    (This)->lpVtbl -> OnNewProgramChain(This,pChain)

#define IDVDNavigatorSink_OnNewProgram(This,progNo)	\
    (This)->lpVtbl -> OnNewProgram(This,progNo)

#define IDVDNavigatorSink_OnNewCell(This,cellNo)	\
    (This)->lpVtbl -> OnNewCell(This,cellNo)

#define IDVDNavigatorSink_OnGOPUserData(This,pData)	\
    (This)->lpVtbl -> OnGOPUserData(This,pData)

#define IDVDNavigatorSink_OnUOPChange(This,newUOPMask)	\
    (This)->lpVtbl -> OnUOPChange(This,newUOPMask)

#define IDVDNavigatorSink_OnPlaybackStateChange(This,eState)	\
    (This)->lpVtbl -> OnPlaybackStateChange(This,eState)

#define IDVDNavigatorSink_OnDiskReadError(This,osCode)	\
    (This)->lpVtbl -> OnDiskReadError(This,osCode)

#define IDVDNavigatorSink_ConfirmTemporaryParentalLevel(This,requestedLevel)	\
    (This)->lpVtbl -> ConfirmTemporaryParentalLevel(This,requestedLevel)

#define IDVDNavigatorSink_OnNavigationException(This,excCode)	\
    (This)->lpVtbl -> OnNavigationException(This,excCode)

#define IDVDNavigatorSink_InitializeSystemParameters(This,eventCode)	\
    (This)->lpVtbl -> InitializeSystemParameters(This,eventCode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnSPRMChange_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    UINT8 no,
    UINT16 wValue);


void __RPC_STUB IDVDNavigatorSink_OnSPRMChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnGPRMChange_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    UINT8 no,
    UINT16 wValue,
    BOOL bCounter);


void __RPC_STUB IDVDNavigatorSink_OnGPRMChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnNewProgramChain_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    IDVDProgramChain __RPC_FAR *pChain);


void __RPC_STUB IDVDNavigatorSink_OnNewProgramChain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnNewProgram_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    UINT8 progNo);


void __RPC_STUB IDVDNavigatorSink_OnNewProgram_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnNewCell_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    UINT8 cellNo);


void __RPC_STUB IDVDNavigatorSink_OnNewCell_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnGOPUserData_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    DVDUserGOPData __RPC_FAR *pData);


void __RPC_STUB IDVDNavigatorSink_OnGOPUserData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnUOPChange_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    UINT32 newUOPMask);


void __RPC_STUB IDVDNavigatorSink_OnUOPChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnPlaybackStateChange_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    EDDVDPlaybackState eState);


void __RPC_STUB IDVDNavigatorSink_OnPlaybackStateChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnDiskReadError_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    DWORD osCode);


void __RPC_STUB IDVDNavigatorSink_OnDiskReadError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_ConfirmTemporaryParentalLevel_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    UINT8 requestedLevel);


void __RPC_STUB IDVDNavigatorSink_ConfirmTemporaryParentalLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_OnNavigationException_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    EDVDNavException excCode);


void __RPC_STUB IDVDNavigatorSink_OnNavigationException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDNavigatorSink_InitializeSystemParameters_Proxy( 
    IDVDNavigatorSink __RPC_FAR * This,
    EDVDSParamEvent eventCode);


void __RPC_STUB IDVDNavigatorSink_InitializeSystemParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDNavigatorSink_INTERFACE_DEFINED__ */



#ifndef __DVDNAVLib_LIBRARY_DEFINED__
#define __DVDNAVLib_LIBRARY_DEFINED__

/* library DVDNAVLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DVDNAVLib;

EXTERN_C const CLSID CLSID_DVDNavigationManager;

#ifdef __cplusplus

class DECLSPEC_UUID("07C89F56-C72E-11D0-A985-00A0C9050598")
DVDNavigationManager;
#endif
#endif /* __DVDNAVLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


