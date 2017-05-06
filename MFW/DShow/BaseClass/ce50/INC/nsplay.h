
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Thu Jun 24 19:41:50 2004
 */
/* Compiler settings for .\nsplay.idl:
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

#ifndef __nsplay_h__
#define __nsplay_h__

/* Forward Declarations */ 

#ifndef __INSOPlay_FWD_DEFINED__
#define __INSOPlay_FWD_DEFINED__
typedef interface INSOPlay INSOPlay;
#endif 	/* __INSOPlay_FWD_DEFINED__ */


#ifndef __INSPlay_FWD_DEFINED__
#define __INSPlay_FWD_DEFINED__
typedef interface INSPlay INSPlay;
#endif 	/* __INSPlay_FWD_DEFINED__ */


#ifndef __INSPlay1_FWD_DEFINED__
#define __INSPlay1_FWD_DEFINED__
typedef interface INSPlay1 INSPlay1;
#endif 	/* __INSPlay1_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_nsplay_0000 */
/* [local] */ 

typedef /* [public] */ 
enum NSOStateConstants
    {	nsoStopped	= 0,
	nsoPaused	= nsoStopped + 1,
	nsoRunning	= nsoPaused + 1
    }	NSOStateConstants;

typedef /* [public] */ 
enum NSOControlConstants
    {	nsoNone	= 0,
	nsoSimple	= nsoNone + 1,
	nsoFull	= nsoSimple + 1
    }	NSOControlConstants;

typedef /* [public] */ 
enum NSODisplayConstants
    {	nsoDefaultSize	= 0,
	nsoHalfSize	= nsoDefaultSize + 1,
	nsoDoubleSize	= nsoHalfSize + 1
    }	NSODisplayConstants;

typedef /* [public] */ 
enum NSPlayStateConstants
    {	nsStopped	= 0,
	nsPaused	= nsStopped + 1,
	nsPlaying	= nsPaused + 1,
	nsWaiting	= nsPlaying + 1
    }	NSPlayStateConstants;

typedef /* [public] */ 
enum NSControlConstants
    {	nsNone	= 0,
	nsSimple	= nsNone + 1,
	nsFull	= nsSimple + 1
    }	NSControlConstants;

typedef /* [public] */ 
enum NSDisplayConstants
    {	nsDefaultSize	= 0,
	nsHalfSize	= nsDefaultSize + 1,
	nsDoubleSize	= nsHalfSize + 1
    }	NSDisplayConstants;

typedef /* [public] */ 
enum NSProtocolConstants
    {	nsMulticast	= 1,
	nsMulticastPlus	= nsMulticast + 1,
	nsUDP	= nsMulticastPlus + 1,
	nsTCP	= nsUDP + 1,
	nsDistribution	= nsTCP + 1,
	nsHTTP	= nsDistribution + 1,
	nsFile	= nsHTTP + 1
    }	NSProtocolConstants;

typedef /* [public] */ 
enum NSOpenStateConstants
    {	nsClosed	= 0,
	nsLoadingASX	= nsClosed + 1,
	nsLoadingNSC	= nsLoadingASX + 1,
	nsLocating	= nsLoadingNSC + 1,
	nsConnecting	= nsLocating + 1,
	nsOpening	= nsConnecting + 1,
	nsOpen	= nsOpening + 1
    }	NSOpenStateConstants;

typedef /* [public] */ 
enum NSWarningTypeConstants
    {	nsNoAudioDevice	= 0,
	nsUnknownStreamFormat	= nsNoAudioDevice + 1,
	nsPlaylistItemFailure	= nsUnknownStreamFormat + 1,
	nsVideoNotRendered	= nsPlaylistItemFailure + 1,
	nsAudioNotRendered	= nsVideoNotRendered + 1,
	nsMissingCodec	= nsAudioNotRendered + 1
    }	NSWarningTypeConstants;



extern RPC_IF_HANDLE __MIDL_itf_nsplay_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_nsplay_0000_v0_0_s_ifspec;

#ifndef __INSOPlay_INTERFACE_DEFINED__
#define __INSOPlay_INTERFACE_DEFINED__

/* interface INSOPlay */
/* [hidden][dual][uuid][object] */ 


EXTERN_C const IID IID_INSOPlay;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2179C5D1-EBFF-11cf-B6FD-00AA00B4E220")
    INSOPlay : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ImageSourceWidth( 
            /* [retval][out] */ long __RPC_FAR *pWidth) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ImageSourceHeight( 
            /* [retval][out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Duration( 
            /* [retval][out] */ double __RPC_FAR *pDuration) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Author( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Copyright( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Rating( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Title( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SourceLink( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSourceLink) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_MarkerCount( 
            /* [retval][out] */ long __RPC_FAR *pMarkerCount) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CanScan( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanScan) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CanSeek( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeek) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CanSeekToMarkers( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeekToMarkers) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CreationDate( 
            /* [retval][out] */ DATE __RPC_FAR *pCreationDate) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Bandwidth( 
            /* [retval][out] */ long __RPC_FAR *pBandwidth) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ErrorCorrection( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorCorrection) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoStart( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoStart) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoStart( 
            /* [in] */ VARIANT_BOOL AutoStart) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoRewind( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoRewind) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoRewind( 
            /* [in] */ VARIANT_BOOL AutoRewind) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_InvokeURLs( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pInvokeURLs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_InvokeURLs( 
            /* [in] */ VARIANT_BOOL InvokeURLs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnableContextMenu( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableContextMenu) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnableContextMenu( 
            /* [in] */ VARIANT_BOOL EnableContextMenu) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_TransparentAtStart( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pTransparentAtStart) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_TransparentAtStart( 
            /* [in] */ VARIANT_BOOL TransparentAtStart) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ClickToPlay( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pClickToPlay) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ClickToPlay( 
            /* [in] */ VARIANT_BOOL ClickToPlay) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FileName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_FileName( 
            /* [in] */ BSTR bstrFileName) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentPosition( 
            /* [retval][out] */ double __RPC_FAR *pCurrentPosition) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CurrentPosition( 
            /* [in] */ double CurrentPosition) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Rate( 
            /* [retval][out] */ double __RPC_FAR *pRate) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Rate( 
            /* [in] */ double Rate) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentMarker( 
            /* [retval][out] */ long __RPC_FAR *pCurrentMarker) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CurrentMarker( 
            /* [in] */ long CurrentMarker) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_PlayCount( 
            /* [retval][out] */ long __RPC_FAR *pPlayCount) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_PlayCount( 
            /* [in] */ long PlayCount) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentState( 
            /* [retval][out] */ long __RPC_FAR *pCurrentState) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DisplaySize( 
            /* [retval][out] */ long __RPC_FAR *pDisplaySize) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DisplaySize( 
            /* [in] */ long DisplaySize) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_MainWindow( 
            /* [retval][out] */ long __RPC_FAR *pMainWindow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowScan( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAllowScan) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowScan( 
            /* [in] */ VARIANT_BOOL AllowScan) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SendKeyboardEvents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendKeyboardEvents) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SendKeyboardEvents( 
            /* [in] */ VARIANT_BOOL SendKeyboardEvents) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SendMouseClickEvents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseClickEvents) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SendMouseClickEvents( 
            /* [in] */ VARIANT_BOOL SendMouseClickEvents) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SendMouseMoveEvents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseMoveEvents) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SendMouseMoveEvents( 
            /* [in] */ VARIANT_BOOL SendMouseMoveEvents) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SendStateChangeEvents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendStateChangeEvents) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SendStateChangeEvents( 
            /* [in] */ VARIANT_BOOL SendStateChangeEvents) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ReceivedPackets( 
            /* [retval][out] */ long __RPC_FAR *pReceivedPackets) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RecoveredPackets( 
            /* [retval][out] */ long __RPC_FAR *pRecoveredPackets) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_LostPackets( 
            /* [retval][out] */ long __RPC_FAR *pLostPackets) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ReceptionQuality( 
            /* [retval][out] */ long __RPC_FAR *pReceptionQuality) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BufferingCount( 
            /* [retval][out] */ long __RPC_FAR *pBufferingCount) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CursorType( 
            /* [retval][out] */ long __RPC_FAR *pCursorType) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CursorType( 
            /* [in] */ long CursorType) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AnimationAtStart( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationAtStart) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AnimationAtStart( 
            /* [in] */ VARIANT_BOOL AnimationAtStart) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_AnimationOnStop( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationOnStop) = 0;
        
        virtual /* [hidden][propput][id] */ HRESULT STDMETHODCALLTYPE put_AnimationOnStop( 
            /* [in] */ VARIANT_BOOL AnimationOnStop) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Play( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetMarkerTime( 
            /* [in] */ long MarkerNum,
            /* [retval][out] */ double __RPC_FAR *pMarkerTime) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetMarkerName( 
            /* [in] */ long MarkerNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMarkerName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INSOPlayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INSOPlay __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INSOPlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INSOPlay __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceWidth )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pWidth);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceHeight )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHeight);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pDuration);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Author )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Copyright )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceLink )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSourceLink);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MarkerCount )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pMarkerCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanScan )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanScan);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanSeek )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeek);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanSeekToMarkers )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeekToMarkers);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CreationDate )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pCreationDate);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bandwidth )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBandwidth);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorCorrection )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorCorrection);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoStart )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoStart);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoStart )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AutoStart);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoRewind )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoRewind);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoRewind )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AutoRewind);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InvokeURLs )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pInvokeURLs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_InvokeURLs )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL InvokeURLs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableContextMenu )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableContextMenu);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableContextMenu )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableContextMenu);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransparentAtStart )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pTransparentAtStart);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TransparentAtStart )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL TransparentAtStart);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClickToPlay )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pClickToPlay);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ClickToPlay )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL ClickToPlay);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileName )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FileName )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPosition )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pCurrentPosition);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentPosition )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ double CurrentPosition);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rate )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pRate);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rate )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ double Rate);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentMarker )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCurrentMarker);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentMarker )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ long CurrentMarker);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayCount )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pPlayCount);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayCount )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ long PlayCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentState )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCurrentState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplaySize )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDisplaySize);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplaySize )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ long DisplaySize);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MainWindow )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pMainWindow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowScan )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAllowScan);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowScan )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AllowScan);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendKeyboardEvents )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendKeyboardEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendKeyboardEvents )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendKeyboardEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendMouseClickEvents )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseClickEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendMouseClickEvents )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendMouseClickEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendMouseMoveEvents )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseMoveEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendMouseMoveEvents )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendMouseMoveEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendStateChangeEvents )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendStateChangeEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendStateChangeEvents )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendStateChangeEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceivedPackets )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pReceivedPackets);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecoveredPackets )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pRecoveredPackets);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LostPackets )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pLostPackets);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceptionQuality )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pReceptionQuality);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingCount )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBufferingCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorType )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCursorType);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorType )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ long CursorType);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AnimationAtStart )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationAtStart);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AnimationAtStart )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AnimationAtStart);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AnimationOnStop )( 
            INSOPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationOnStop);
        
        /* [hidden][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AnimationOnStop )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AnimationOnStop);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Play )( 
            INSOPlay __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            INSOPlay __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            INSOPlay __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMarkerTime )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ long MarkerNum,
            /* [retval][out] */ double __RPC_FAR *pMarkerTime);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMarkerName )( 
            INSOPlay __RPC_FAR * This,
            /* [in] */ long MarkerNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMarkerName);
        
        END_INTERFACE
    } INSOPlayVtbl;

    interface INSOPlay
    {
        CONST_VTBL struct INSOPlayVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INSOPlay_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INSOPlay_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INSOPlay_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INSOPlay_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INSOPlay_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INSOPlay_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INSOPlay_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INSOPlay_get_ImageSourceWidth(This,pWidth)	\
    (This)->lpVtbl -> get_ImageSourceWidth(This,pWidth)

#define INSOPlay_get_ImageSourceHeight(This,pHeight)	\
    (This)->lpVtbl -> get_ImageSourceHeight(This,pHeight)

#define INSOPlay_get_Duration(This,pDuration)	\
    (This)->lpVtbl -> get_Duration(This,pDuration)

#define INSOPlay_get_Author(This,pbstrAuthor)	\
    (This)->lpVtbl -> get_Author(This,pbstrAuthor)

#define INSOPlay_get_Copyright(This,pbstrCopyright)	\
    (This)->lpVtbl -> get_Copyright(This,pbstrCopyright)

#define INSOPlay_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define INSOPlay_get_Rating(This,pbstrRating)	\
    (This)->lpVtbl -> get_Rating(This,pbstrRating)

#define INSOPlay_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define INSOPlay_get_SourceLink(This,pbstrSourceLink)	\
    (This)->lpVtbl -> get_SourceLink(This,pbstrSourceLink)

#define INSOPlay_get_MarkerCount(This,pMarkerCount)	\
    (This)->lpVtbl -> get_MarkerCount(This,pMarkerCount)

#define INSOPlay_get_CanScan(This,pCanScan)	\
    (This)->lpVtbl -> get_CanScan(This,pCanScan)

#define INSOPlay_get_CanSeek(This,pCanSeek)	\
    (This)->lpVtbl -> get_CanSeek(This,pCanSeek)

#define INSOPlay_get_CanSeekToMarkers(This,pCanSeekToMarkers)	\
    (This)->lpVtbl -> get_CanSeekToMarkers(This,pCanSeekToMarkers)

#define INSOPlay_get_CreationDate(This,pCreationDate)	\
    (This)->lpVtbl -> get_CreationDate(This,pCreationDate)

#define INSOPlay_get_Bandwidth(This,pBandwidth)	\
    (This)->lpVtbl -> get_Bandwidth(This,pBandwidth)

#define INSOPlay_get_ErrorCorrection(This,pbstrErrorCorrection)	\
    (This)->lpVtbl -> get_ErrorCorrection(This,pbstrErrorCorrection)

#define INSOPlay_get_AutoStart(This,pAutoStart)	\
    (This)->lpVtbl -> get_AutoStart(This,pAutoStart)

#define INSOPlay_put_AutoStart(This,AutoStart)	\
    (This)->lpVtbl -> put_AutoStart(This,AutoStart)

#define INSOPlay_get_AutoRewind(This,pAutoRewind)	\
    (This)->lpVtbl -> get_AutoRewind(This,pAutoRewind)

#define INSOPlay_put_AutoRewind(This,AutoRewind)	\
    (This)->lpVtbl -> put_AutoRewind(This,AutoRewind)

#define INSOPlay_get_InvokeURLs(This,pInvokeURLs)	\
    (This)->lpVtbl -> get_InvokeURLs(This,pInvokeURLs)

#define INSOPlay_put_InvokeURLs(This,InvokeURLs)	\
    (This)->lpVtbl -> put_InvokeURLs(This,InvokeURLs)

#define INSOPlay_get_EnableContextMenu(This,pEnableContextMenu)	\
    (This)->lpVtbl -> get_EnableContextMenu(This,pEnableContextMenu)

#define INSOPlay_put_EnableContextMenu(This,EnableContextMenu)	\
    (This)->lpVtbl -> put_EnableContextMenu(This,EnableContextMenu)

#define INSOPlay_get_TransparentAtStart(This,pTransparentAtStart)	\
    (This)->lpVtbl -> get_TransparentAtStart(This,pTransparentAtStart)

#define INSOPlay_put_TransparentAtStart(This,TransparentAtStart)	\
    (This)->lpVtbl -> put_TransparentAtStart(This,TransparentAtStart)

#define INSOPlay_get_ClickToPlay(This,pClickToPlay)	\
    (This)->lpVtbl -> get_ClickToPlay(This,pClickToPlay)

#define INSOPlay_put_ClickToPlay(This,ClickToPlay)	\
    (This)->lpVtbl -> put_ClickToPlay(This,ClickToPlay)

#define INSOPlay_get_FileName(This,pbstrFileName)	\
    (This)->lpVtbl -> get_FileName(This,pbstrFileName)

#define INSOPlay_put_FileName(This,bstrFileName)	\
    (This)->lpVtbl -> put_FileName(This,bstrFileName)

#define INSOPlay_get_CurrentPosition(This,pCurrentPosition)	\
    (This)->lpVtbl -> get_CurrentPosition(This,pCurrentPosition)

#define INSOPlay_put_CurrentPosition(This,CurrentPosition)	\
    (This)->lpVtbl -> put_CurrentPosition(This,CurrentPosition)

#define INSOPlay_get_Rate(This,pRate)	\
    (This)->lpVtbl -> get_Rate(This,pRate)

#define INSOPlay_put_Rate(This,Rate)	\
    (This)->lpVtbl -> put_Rate(This,Rate)

#define INSOPlay_get_CurrentMarker(This,pCurrentMarker)	\
    (This)->lpVtbl -> get_CurrentMarker(This,pCurrentMarker)

#define INSOPlay_put_CurrentMarker(This,CurrentMarker)	\
    (This)->lpVtbl -> put_CurrentMarker(This,CurrentMarker)

#define INSOPlay_get_PlayCount(This,pPlayCount)	\
    (This)->lpVtbl -> get_PlayCount(This,pPlayCount)

#define INSOPlay_put_PlayCount(This,PlayCount)	\
    (This)->lpVtbl -> put_PlayCount(This,PlayCount)

#define INSOPlay_get_CurrentState(This,pCurrentState)	\
    (This)->lpVtbl -> get_CurrentState(This,pCurrentState)

#define INSOPlay_get_DisplaySize(This,pDisplaySize)	\
    (This)->lpVtbl -> get_DisplaySize(This,pDisplaySize)

#define INSOPlay_put_DisplaySize(This,DisplaySize)	\
    (This)->lpVtbl -> put_DisplaySize(This,DisplaySize)

#define INSOPlay_get_MainWindow(This,pMainWindow)	\
    (This)->lpVtbl -> get_MainWindow(This,pMainWindow)

#define INSOPlay_get_AllowScan(This,pAllowScan)	\
    (This)->lpVtbl -> get_AllowScan(This,pAllowScan)

#define INSOPlay_put_AllowScan(This,AllowScan)	\
    (This)->lpVtbl -> put_AllowScan(This,AllowScan)

#define INSOPlay_get_SendKeyboardEvents(This,pSendKeyboardEvents)	\
    (This)->lpVtbl -> get_SendKeyboardEvents(This,pSendKeyboardEvents)

#define INSOPlay_put_SendKeyboardEvents(This,SendKeyboardEvents)	\
    (This)->lpVtbl -> put_SendKeyboardEvents(This,SendKeyboardEvents)

#define INSOPlay_get_SendMouseClickEvents(This,pSendMouseClickEvents)	\
    (This)->lpVtbl -> get_SendMouseClickEvents(This,pSendMouseClickEvents)

#define INSOPlay_put_SendMouseClickEvents(This,SendMouseClickEvents)	\
    (This)->lpVtbl -> put_SendMouseClickEvents(This,SendMouseClickEvents)

#define INSOPlay_get_SendMouseMoveEvents(This,pSendMouseMoveEvents)	\
    (This)->lpVtbl -> get_SendMouseMoveEvents(This,pSendMouseMoveEvents)

#define INSOPlay_put_SendMouseMoveEvents(This,SendMouseMoveEvents)	\
    (This)->lpVtbl -> put_SendMouseMoveEvents(This,SendMouseMoveEvents)

#define INSOPlay_get_SendStateChangeEvents(This,pSendStateChangeEvents)	\
    (This)->lpVtbl -> get_SendStateChangeEvents(This,pSendStateChangeEvents)

#define INSOPlay_put_SendStateChangeEvents(This,SendStateChangeEvents)	\
    (This)->lpVtbl -> put_SendStateChangeEvents(This,SendStateChangeEvents)

#define INSOPlay_get_ReceivedPackets(This,pReceivedPackets)	\
    (This)->lpVtbl -> get_ReceivedPackets(This,pReceivedPackets)

#define INSOPlay_get_RecoveredPackets(This,pRecoveredPackets)	\
    (This)->lpVtbl -> get_RecoveredPackets(This,pRecoveredPackets)

#define INSOPlay_get_LostPackets(This,pLostPackets)	\
    (This)->lpVtbl -> get_LostPackets(This,pLostPackets)

#define INSOPlay_get_ReceptionQuality(This,pReceptionQuality)	\
    (This)->lpVtbl -> get_ReceptionQuality(This,pReceptionQuality)

#define INSOPlay_get_BufferingCount(This,pBufferingCount)	\
    (This)->lpVtbl -> get_BufferingCount(This,pBufferingCount)

#define INSOPlay_get_CursorType(This,pCursorType)	\
    (This)->lpVtbl -> get_CursorType(This,pCursorType)

#define INSOPlay_put_CursorType(This,CursorType)	\
    (This)->lpVtbl -> put_CursorType(This,CursorType)

#define INSOPlay_get_AnimationAtStart(This,pAnimationAtStart)	\
    (This)->lpVtbl -> get_AnimationAtStart(This,pAnimationAtStart)

#define INSOPlay_put_AnimationAtStart(This,AnimationAtStart)	\
    (This)->lpVtbl -> put_AnimationAtStart(This,AnimationAtStart)

#define INSOPlay_get_AnimationOnStop(This,pAnimationOnStop)	\
    (This)->lpVtbl -> get_AnimationOnStop(This,pAnimationOnStop)

#define INSOPlay_put_AnimationOnStop(This,AnimationOnStop)	\
    (This)->lpVtbl -> put_AnimationOnStop(This,AnimationOnStop)

#define INSOPlay_Play(This)	\
    (This)->lpVtbl -> Play(This)

#define INSOPlay_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define INSOPlay_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define INSOPlay_GetMarkerTime(This,MarkerNum,pMarkerTime)	\
    (This)->lpVtbl -> GetMarkerTime(This,MarkerNum,pMarkerTime)

#define INSOPlay_GetMarkerName(This,MarkerNum,pbstrMarkerName)	\
    (This)->lpVtbl -> GetMarkerName(This,MarkerNum,pbstrMarkerName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_ImageSourceWidth_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pWidth);


void __RPC_STUB INSOPlay_get_ImageSourceWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_ImageSourceHeight_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pHeight);


void __RPC_STUB INSOPlay_get_ImageSourceHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_Duration_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pDuration);


void __RPC_STUB INSOPlay_get_Duration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_Author_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor);


void __RPC_STUB INSOPlay_get_Author_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_Copyright_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);


void __RPC_STUB INSOPlay_get_Copyright_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_Description_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);


void __RPC_STUB INSOPlay_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_Rating_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);


void __RPC_STUB INSOPlay_get_Rating_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_Title_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);


void __RPC_STUB INSOPlay_get_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_SourceLink_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrSourceLink);


void __RPC_STUB INSOPlay_get_SourceLink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_MarkerCount_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pMarkerCount);


void __RPC_STUB INSOPlay_get_MarkerCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_CanScan_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanScan);


void __RPC_STUB INSOPlay_get_CanScan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_CanSeek_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeek);


void __RPC_STUB INSOPlay_get_CanSeek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_CanSeekToMarkers_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeekToMarkers);


void __RPC_STUB INSOPlay_get_CanSeekToMarkers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_CreationDate_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *pCreationDate);


void __RPC_STUB INSOPlay_get_CreationDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_Bandwidth_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBandwidth);


void __RPC_STUB INSOPlay_get_Bandwidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_ErrorCorrection_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorCorrection);


void __RPC_STUB INSOPlay_get_ErrorCorrection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_AutoStart_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoStart);


void __RPC_STUB INSOPlay_get_AutoStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_AutoStart_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL AutoStart);


void __RPC_STUB INSOPlay_put_AutoStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_AutoRewind_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoRewind);


void __RPC_STUB INSOPlay_get_AutoRewind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_AutoRewind_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL AutoRewind);


void __RPC_STUB INSOPlay_put_AutoRewind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_InvokeURLs_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pInvokeURLs);


void __RPC_STUB INSOPlay_get_InvokeURLs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_InvokeURLs_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL InvokeURLs);


void __RPC_STUB INSOPlay_put_InvokeURLs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_EnableContextMenu_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableContextMenu);


void __RPC_STUB INSOPlay_get_EnableContextMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_EnableContextMenu_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableContextMenu);


void __RPC_STUB INSOPlay_put_EnableContextMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_TransparentAtStart_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pTransparentAtStart);


void __RPC_STUB INSOPlay_get_TransparentAtStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_TransparentAtStart_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL TransparentAtStart);


void __RPC_STUB INSOPlay_put_TransparentAtStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_ClickToPlay_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pClickToPlay);


void __RPC_STUB INSOPlay_get_ClickToPlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_ClickToPlay_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL ClickToPlay);


void __RPC_STUB INSOPlay_put_ClickToPlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_FileName_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);


void __RPC_STUB INSOPlay_get_FileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_FileName_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ BSTR bstrFileName);


void __RPC_STUB INSOPlay_put_FileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_CurrentPosition_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pCurrentPosition);


void __RPC_STUB INSOPlay_get_CurrentPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_CurrentPosition_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ double CurrentPosition);


void __RPC_STUB INSOPlay_put_CurrentPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_Rate_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pRate);


void __RPC_STUB INSOPlay_get_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_Rate_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ double Rate);


void __RPC_STUB INSOPlay_put_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_CurrentMarker_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCurrentMarker);


void __RPC_STUB INSOPlay_get_CurrentMarker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_CurrentMarker_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ long CurrentMarker);


void __RPC_STUB INSOPlay_put_CurrentMarker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_PlayCount_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pPlayCount);


void __RPC_STUB INSOPlay_get_PlayCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_PlayCount_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ long PlayCount);


void __RPC_STUB INSOPlay_put_PlayCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_CurrentState_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCurrentState);


void __RPC_STUB INSOPlay_get_CurrentState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_DisplaySize_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pDisplaySize);


void __RPC_STUB INSOPlay_get_DisplaySize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_DisplaySize_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ long DisplaySize);


void __RPC_STUB INSOPlay_put_DisplaySize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_MainWindow_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pMainWindow);


void __RPC_STUB INSOPlay_get_MainWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_AllowScan_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAllowScan);


void __RPC_STUB INSOPlay_get_AllowScan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_AllowScan_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL AllowScan);


void __RPC_STUB INSOPlay_put_AllowScan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_SendKeyboardEvents_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendKeyboardEvents);


void __RPC_STUB INSOPlay_get_SendKeyboardEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_SendKeyboardEvents_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL SendKeyboardEvents);


void __RPC_STUB INSOPlay_put_SendKeyboardEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_SendMouseClickEvents_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseClickEvents);


void __RPC_STUB INSOPlay_get_SendMouseClickEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_SendMouseClickEvents_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL SendMouseClickEvents);


void __RPC_STUB INSOPlay_put_SendMouseClickEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_SendMouseMoveEvents_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseMoveEvents);


void __RPC_STUB INSOPlay_get_SendMouseMoveEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_SendMouseMoveEvents_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL SendMouseMoveEvents);


void __RPC_STUB INSOPlay_put_SendMouseMoveEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_SendStateChangeEvents_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendStateChangeEvents);


void __RPC_STUB INSOPlay_get_SendStateChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_SendStateChangeEvents_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL SendStateChangeEvents);


void __RPC_STUB INSOPlay_put_SendStateChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_ReceivedPackets_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pReceivedPackets);


void __RPC_STUB INSOPlay_get_ReceivedPackets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_RecoveredPackets_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pRecoveredPackets);


void __RPC_STUB INSOPlay_get_RecoveredPackets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_LostPackets_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pLostPackets);


void __RPC_STUB INSOPlay_get_LostPackets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_ReceptionQuality_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pReceptionQuality);


void __RPC_STUB INSOPlay_get_ReceptionQuality_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_BufferingCount_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBufferingCount);


void __RPC_STUB INSOPlay_get_BufferingCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_CursorType_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCursorType);


void __RPC_STUB INSOPlay_get_CursorType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_CursorType_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ long CursorType);


void __RPC_STUB INSOPlay_put_CursorType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_AnimationAtStart_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationAtStart);


void __RPC_STUB INSOPlay_get_AnimationAtStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_AnimationAtStart_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL AnimationAtStart);


void __RPC_STUB INSOPlay_put_AnimationAtStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_get_AnimationOnStop_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationOnStop);


void __RPC_STUB INSOPlay_get_AnimationOnStop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][propput][id] */ HRESULT STDMETHODCALLTYPE INSOPlay_put_AnimationOnStop_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL AnimationOnStop);


void __RPC_STUB INSOPlay_put_AnimationOnStop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSOPlay_Play_Proxy( 
    INSOPlay __RPC_FAR * This);


void __RPC_STUB INSOPlay_Play_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSOPlay_Pause_Proxy( 
    INSOPlay __RPC_FAR * This);


void __RPC_STUB INSOPlay_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSOPlay_Stop_Proxy( 
    INSOPlay __RPC_FAR * This);


void __RPC_STUB INSOPlay_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSOPlay_GetMarkerTime_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ long MarkerNum,
    /* [retval][out] */ double __RPC_FAR *pMarkerTime);


void __RPC_STUB INSOPlay_GetMarkerTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSOPlay_GetMarkerName_Proxy( 
    INSOPlay __RPC_FAR * This,
    /* [in] */ long MarkerNum,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrMarkerName);


void __RPC_STUB INSOPlay_GetMarkerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INSOPlay_INTERFACE_DEFINED__ */


#ifndef __INSPlay_INTERFACE_DEFINED__
#define __INSPlay_INTERFACE_DEFINED__

/* interface INSPlay */
/* [hidden][dual][uuid][object] */ 


EXTERN_C const IID IID_INSPlay;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E7C4BE80-7960-11d0-B727-00AA00B4E220")
    INSPlay : public INSOPlay
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ChannelName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelName) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ChannelDescription( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelDescription) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ChannelURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelURL) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ContactAddress( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactAddress) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ContactPhone( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactPhone) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ContactEmail( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactEmail) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowChangeDisplaySize( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAllowChangeDisplaySize) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowChangeDisplaySize( 
            /* [in] */ VARIANT_BOOL AllowChangeDisplaySize) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CodecCount( 
            /* [retval][out] */ long __RPC_FAR *pCodecCount) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IsBroadcast( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsBroadcast) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IsDurationValid( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsDurationValid) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SourceProtocol( 
            /* [retval][out] */ long __RPC_FAR *pSourceProtocol) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_OpenState( 
            /* [retval][out] */ long __RPC_FAR *pOpenState) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SendOpenStateChangeEvents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendOpenStateChangeEvents) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SendOpenStateChangeEvents( 
            /* [in] */ VARIANT_BOOL SendOpenStateChangeEvents) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SendWarningEvents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendWarningEvents) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SendWarningEvents( 
            /* [in] */ VARIANT_BOOL SendWarningEvents) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SendErrorEvents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendErrorEvents) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SendErrorEvents( 
            /* [in] */ VARIANT_BOOL SendErrorEvents) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_HasError( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pHasError) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ErrorDescription( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorDescription) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ErrorCode( 
            /* [retval][out] */ long __RPC_FAR *pErrorCode) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_PlayState( 
            /* [retval][out] */ long __RPC_FAR *pPlayState) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SendPlayStateChangeEvents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendPlayStateChangeEvents) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SendPlayStateChangeEvents( 
            /* [in] */ VARIANT_BOOL SendPlayStateChangeEvents) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BufferingTime( 
            /* [retval][out] */ double __RPC_FAR *pBufferingTime) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_BufferingTime( 
            /* [in] */ double BufferingTime) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_UseFixedUDPPort( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseFixedUDPPort) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_UseFixedUDPPort( 
            /* [in] */ VARIANT_BOOL UseFixedUDPPort) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FixedUDPPort( 
            /* [retval][out] */ long __RPC_FAR *pFixedUDPPort) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_FixedUDPPort( 
            /* [in] */ long FixedUDPPort) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_UseHTTPProxy( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseHTTPProxy) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_UseHTTPProxy( 
            /* [in] */ VARIANT_BOOL UseHTTPProxy) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnableAutoProxy( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableAutoProxy) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnableAutoProxy( 
            /* [in] */ VARIANT_BOOL EnableAutoProxy) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_HTTPProxyHost( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrHTTPProxyHost) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_HTTPProxyHost( 
            /* [in] */ BSTR bstrHTTPProxyHost) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_HTTPProxyPort( 
            /* [retval][out] */ long __RPC_FAR *pHTTPProxyPort) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_HTTPProxyPort( 
            /* [in] */ long HTTPProxyPort) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnableMulticast( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableMulticast) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnableMulticast( 
            /* [in] */ VARIANT_BOOL EnableMulticast) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnableUDP( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableUDP) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnableUDP( 
            /* [in] */ VARIANT_BOOL EnableUDP) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnableTCP( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableTCP) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnableTCP( 
            /* [in] */ VARIANT_BOOL EnableTCP) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnableHTTP( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableHTTP) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnableHTTP( 
            /* [in] */ VARIANT_BOOL EnableHTTP) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BufferingProgress( 
            /* [retval][out] */ long __RPC_FAR *pBufferingProgress) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BaseURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrBaseURL) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_BaseURL( 
            /* [in] */ BSTR bstrBaseURL) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DefaultFrame( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDefaultFrame) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DefaultFrame( 
            /* [in] */ BSTR bstrDefaultFrame) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AboutBox( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecInstalled( 
            /* [in] */ long CodecNum,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCodecInstalled) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecDescription( 
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecDescription) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecURL( 
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecURL) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ BSTR bstrFileName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INSPlayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INSPlay __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INSPlay __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INSPlay __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceWidth )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pWidth);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceHeight )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHeight);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pDuration);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Author )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Copyright )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceLink )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSourceLink);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MarkerCount )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pMarkerCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanScan )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanScan);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanSeek )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeek);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanSeekToMarkers )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeekToMarkers);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CreationDate )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pCreationDate);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bandwidth )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBandwidth);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorCorrection )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorCorrection);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoStart )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoStart);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoStart )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AutoStart);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoRewind )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoRewind);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoRewind )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AutoRewind);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InvokeURLs )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pInvokeURLs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_InvokeURLs )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL InvokeURLs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableContextMenu )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableContextMenu);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableContextMenu )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableContextMenu);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransparentAtStart )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pTransparentAtStart);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TransparentAtStart )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL TransparentAtStart);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClickToPlay )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pClickToPlay);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ClickToPlay )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL ClickToPlay);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileName )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FileName )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPosition )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pCurrentPosition);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentPosition )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ double CurrentPosition);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rate )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pRate);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rate )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ double Rate);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentMarker )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCurrentMarker);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentMarker )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long CurrentMarker);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayCount )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pPlayCount);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayCount )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long PlayCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentState )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCurrentState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplaySize )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDisplaySize);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplaySize )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long DisplaySize);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MainWindow )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pMainWindow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowScan )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAllowScan);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowScan )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AllowScan);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendKeyboardEvents )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendKeyboardEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendKeyboardEvents )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendKeyboardEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendMouseClickEvents )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseClickEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendMouseClickEvents )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendMouseClickEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendMouseMoveEvents )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseMoveEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendMouseMoveEvents )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendMouseMoveEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendStateChangeEvents )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendStateChangeEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendStateChangeEvents )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendStateChangeEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceivedPackets )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pReceivedPackets);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecoveredPackets )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pRecoveredPackets);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LostPackets )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pLostPackets);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceptionQuality )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pReceptionQuality);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingCount )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBufferingCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorType )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCursorType);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorType )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long CursorType);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AnimationAtStart )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationAtStart);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AnimationAtStart )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AnimationAtStart);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AnimationOnStop )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationOnStop);
        
        /* [hidden][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AnimationOnStop )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AnimationOnStop);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Play )( 
            INSPlay __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            INSPlay __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            INSPlay __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMarkerTime )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long MarkerNum,
            /* [retval][out] */ double __RPC_FAR *pMarkerTime);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMarkerName )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long MarkerNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMarkerName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ChannelName )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ChannelDescription )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ChannelURL )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelURL);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContactAddress )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactAddress);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContactPhone )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactPhone);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContactEmail )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactEmail);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowChangeDisplaySize )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAllowChangeDisplaySize);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowChangeDisplaySize )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AllowChangeDisplaySize);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CodecCount )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCodecCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsBroadcast )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsBroadcast);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsDurationValid )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsDurationValid);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceProtocol )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceProtocol);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OpenState )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pOpenState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendOpenStateChangeEvents )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendOpenStateChangeEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendOpenStateChangeEvents )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendOpenStateChangeEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendWarningEvents )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendWarningEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendWarningEvents )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendWarningEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendErrorEvents )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendErrorEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendErrorEvents )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendErrorEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HasError )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pHasError);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorDescription )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorCode )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pErrorCode);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayState )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pPlayState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendPlayStateChangeEvents )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendPlayStateChangeEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendPlayStateChangeEvents )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendPlayStateChangeEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingTime )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pBufferingTime);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BufferingTime )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ double BufferingTime);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseFixedUDPPort )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseFixedUDPPort);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseFixedUDPPort )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL UseFixedUDPPort);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FixedUDPPort )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pFixedUDPPort);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FixedUDPPort )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long FixedUDPPort);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseHTTPProxy )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseHTTPProxy);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseHTTPProxy )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL UseHTTPProxy);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableAutoProxy )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableAutoProxy);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableAutoProxy )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableAutoProxy);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HTTPProxyHost )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrHTTPProxyHost);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HTTPProxyHost )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ BSTR bstrHTTPProxyHost);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HTTPProxyPort )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHTTPProxyPort);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HTTPProxyPort )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long HTTPProxyPort);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableMulticast )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableMulticast);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableMulticast )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableMulticast);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableUDP )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableUDP);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableUDP )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableUDP);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableTCP )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableTCP);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableTCP )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableTCP);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableHTTP )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableHTTP);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableHTTP )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableHTTP);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingProgress )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBufferingProgress);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BaseURL )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrBaseURL);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BaseURL )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ BSTR bstrBaseURL);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultFrame )( 
            INSPlay __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDefaultFrame);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefaultFrame )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ BSTR bstrDefaultFrame);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AboutBox )( 
            INSPlay __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            INSPlay __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCodecInstalled )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long CodecNum,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCodecInstalled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCodecDescription )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecDescription);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCodecURL )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecURL);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            INSPlay __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName);
        
        END_INTERFACE
    } INSPlayVtbl;

    interface INSPlay
    {
        CONST_VTBL struct INSPlayVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INSPlay_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INSPlay_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INSPlay_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INSPlay_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INSPlay_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INSPlay_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INSPlay_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INSPlay_get_ImageSourceWidth(This,pWidth)	\
    (This)->lpVtbl -> get_ImageSourceWidth(This,pWidth)

#define INSPlay_get_ImageSourceHeight(This,pHeight)	\
    (This)->lpVtbl -> get_ImageSourceHeight(This,pHeight)

#define INSPlay_get_Duration(This,pDuration)	\
    (This)->lpVtbl -> get_Duration(This,pDuration)

#define INSPlay_get_Author(This,pbstrAuthor)	\
    (This)->lpVtbl -> get_Author(This,pbstrAuthor)

#define INSPlay_get_Copyright(This,pbstrCopyright)	\
    (This)->lpVtbl -> get_Copyright(This,pbstrCopyright)

#define INSPlay_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define INSPlay_get_Rating(This,pbstrRating)	\
    (This)->lpVtbl -> get_Rating(This,pbstrRating)

#define INSPlay_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define INSPlay_get_SourceLink(This,pbstrSourceLink)	\
    (This)->lpVtbl -> get_SourceLink(This,pbstrSourceLink)

#define INSPlay_get_MarkerCount(This,pMarkerCount)	\
    (This)->lpVtbl -> get_MarkerCount(This,pMarkerCount)

#define INSPlay_get_CanScan(This,pCanScan)	\
    (This)->lpVtbl -> get_CanScan(This,pCanScan)

#define INSPlay_get_CanSeek(This,pCanSeek)	\
    (This)->lpVtbl -> get_CanSeek(This,pCanSeek)

#define INSPlay_get_CanSeekToMarkers(This,pCanSeekToMarkers)	\
    (This)->lpVtbl -> get_CanSeekToMarkers(This,pCanSeekToMarkers)

#define INSPlay_get_CreationDate(This,pCreationDate)	\
    (This)->lpVtbl -> get_CreationDate(This,pCreationDate)

#define INSPlay_get_Bandwidth(This,pBandwidth)	\
    (This)->lpVtbl -> get_Bandwidth(This,pBandwidth)

#define INSPlay_get_ErrorCorrection(This,pbstrErrorCorrection)	\
    (This)->lpVtbl -> get_ErrorCorrection(This,pbstrErrorCorrection)

#define INSPlay_get_AutoStart(This,pAutoStart)	\
    (This)->lpVtbl -> get_AutoStart(This,pAutoStart)

#define INSPlay_put_AutoStart(This,AutoStart)	\
    (This)->lpVtbl -> put_AutoStart(This,AutoStart)

#define INSPlay_get_AutoRewind(This,pAutoRewind)	\
    (This)->lpVtbl -> get_AutoRewind(This,pAutoRewind)

#define INSPlay_put_AutoRewind(This,AutoRewind)	\
    (This)->lpVtbl -> put_AutoRewind(This,AutoRewind)

#define INSPlay_get_InvokeURLs(This,pInvokeURLs)	\
    (This)->lpVtbl -> get_InvokeURLs(This,pInvokeURLs)

#define INSPlay_put_InvokeURLs(This,InvokeURLs)	\
    (This)->lpVtbl -> put_InvokeURLs(This,InvokeURLs)

#define INSPlay_get_EnableContextMenu(This,pEnableContextMenu)	\
    (This)->lpVtbl -> get_EnableContextMenu(This,pEnableContextMenu)

#define INSPlay_put_EnableContextMenu(This,EnableContextMenu)	\
    (This)->lpVtbl -> put_EnableContextMenu(This,EnableContextMenu)

#define INSPlay_get_TransparentAtStart(This,pTransparentAtStart)	\
    (This)->lpVtbl -> get_TransparentAtStart(This,pTransparentAtStart)

#define INSPlay_put_TransparentAtStart(This,TransparentAtStart)	\
    (This)->lpVtbl -> put_TransparentAtStart(This,TransparentAtStart)

#define INSPlay_get_ClickToPlay(This,pClickToPlay)	\
    (This)->lpVtbl -> get_ClickToPlay(This,pClickToPlay)

#define INSPlay_put_ClickToPlay(This,ClickToPlay)	\
    (This)->lpVtbl -> put_ClickToPlay(This,ClickToPlay)

#define INSPlay_get_FileName(This,pbstrFileName)	\
    (This)->lpVtbl -> get_FileName(This,pbstrFileName)

#define INSPlay_put_FileName(This,bstrFileName)	\
    (This)->lpVtbl -> put_FileName(This,bstrFileName)

#define INSPlay_get_CurrentPosition(This,pCurrentPosition)	\
    (This)->lpVtbl -> get_CurrentPosition(This,pCurrentPosition)

#define INSPlay_put_CurrentPosition(This,CurrentPosition)	\
    (This)->lpVtbl -> put_CurrentPosition(This,CurrentPosition)

#define INSPlay_get_Rate(This,pRate)	\
    (This)->lpVtbl -> get_Rate(This,pRate)

#define INSPlay_put_Rate(This,Rate)	\
    (This)->lpVtbl -> put_Rate(This,Rate)

#define INSPlay_get_CurrentMarker(This,pCurrentMarker)	\
    (This)->lpVtbl -> get_CurrentMarker(This,pCurrentMarker)

#define INSPlay_put_CurrentMarker(This,CurrentMarker)	\
    (This)->lpVtbl -> put_CurrentMarker(This,CurrentMarker)

#define INSPlay_get_PlayCount(This,pPlayCount)	\
    (This)->lpVtbl -> get_PlayCount(This,pPlayCount)

#define INSPlay_put_PlayCount(This,PlayCount)	\
    (This)->lpVtbl -> put_PlayCount(This,PlayCount)

#define INSPlay_get_CurrentState(This,pCurrentState)	\
    (This)->lpVtbl -> get_CurrentState(This,pCurrentState)

#define INSPlay_get_DisplaySize(This,pDisplaySize)	\
    (This)->lpVtbl -> get_DisplaySize(This,pDisplaySize)

#define INSPlay_put_DisplaySize(This,DisplaySize)	\
    (This)->lpVtbl -> put_DisplaySize(This,DisplaySize)

#define INSPlay_get_MainWindow(This,pMainWindow)	\
    (This)->lpVtbl -> get_MainWindow(This,pMainWindow)

#define INSPlay_get_AllowScan(This,pAllowScan)	\
    (This)->lpVtbl -> get_AllowScan(This,pAllowScan)

#define INSPlay_put_AllowScan(This,AllowScan)	\
    (This)->lpVtbl -> put_AllowScan(This,AllowScan)

#define INSPlay_get_SendKeyboardEvents(This,pSendKeyboardEvents)	\
    (This)->lpVtbl -> get_SendKeyboardEvents(This,pSendKeyboardEvents)

#define INSPlay_put_SendKeyboardEvents(This,SendKeyboardEvents)	\
    (This)->lpVtbl -> put_SendKeyboardEvents(This,SendKeyboardEvents)

#define INSPlay_get_SendMouseClickEvents(This,pSendMouseClickEvents)	\
    (This)->lpVtbl -> get_SendMouseClickEvents(This,pSendMouseClickEvents)

#define INSPlay_put_SendMouseClickEvents(This,SendMouseClickEvents)	\
    (This)->lpVtbl -> put_SendMouseClickEvents(This,SendMouseClickEvents)

#define INSPlay_get_SendMouseMoveEvents(This,pSendMouseMoveEvents)	\
    (This)->lpVtbl -> get_SendMouseMoveEvents(This,pSendMouseMoveEvents)

#define INSPlay_put_SendMouseMoveEvents(This,SendMouseMoveEvents)	\
    (This)->lpVtbl -> put_SendMouseMoveEvents(This,SendMouseMoveEvents)

#define INSPlay_get_SendStateChangeEvents(This,pSendStateChangeEvents)	\
    (This)->lpVtbl -> get_SendStateChangeEvents(This,pSendStateChangeEvents)

#define INSPlay_put_SendStateChangeEvents(This,SendStateChangeEvents)	\
    (This)->lpVtbl -> put_SendStateChangeEvents(This,SendStateChangeEvents)

#define INSPlay_get_ReceivedPackets(This,pReceivedPackets)	\
    (This)->lpVtbl -> get_ReceivedPackets(This,pReceivedPackets)

#define INSPlay_get_RecoveredPackets(This,pRecoveredPackets)	\
    (This)->lpVtbl -> get_RecoveredPackets(This,pRecoveredPackets)

#define INSPlay_get_LostPackets(This,pLostPackets)	\
    (This)->lpVtbl -> get_LostPackets(This,pLostPackets)

#define INSPlay_get_ReceptionQuality(This,pReceptionQuality)	\
    (This)->lpVtbl -> get_ReceptionQuality(This,pReceptionQuality)

#define INSPlay_get_BufferingCount(This,pBufferingCount)	\
    (This)->lpVtbl -> get_BufferingCount(This,pBufferingCount)

#define INSPlay_get_CursorType(This,pCursorType)	\
    (This)->lpVtbl -> get_CursorType(This,pCursorType)

#define INSPlay_put_CursorType(This,CursorType)	\
    (This)->lpVtbl -> put_CursorType(This,CursorType)

#define INSPlay_get_AnimationAtStart(This,pAnimationAtStart)	\
    (This)->lpVtbl -> get_AnimationAtStart(This,pAnimationAtStart)

#define INSPlay_put_AnimationAtStart(This,AnimationAtStart)	\
    (This)->lpVtbl -> put_AnimationAtStart(This,AnimationAtStart)

#define INSPlay_get_AnimationOnStop(This,pAnimationOnStop)	\
    (This)->lpVtbl -> get_AnimationOnStop(This,pAnimationOnStop)

#define INSPlay_put_AnimationOnStop(This,AnimationOnStop)	\
    (This)->lpVtbl -> put_AnimationOnStop(This,AnimationOnStop)

#define INSPlay_Play(This)	\
    (This)->lpVtbl -> Play(This)

#define INSPlay_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define INSPlay_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define INSPlay_GetMarkerTime(This,MarkerNum,pMarkerTime)	\
    (This)->lpVtbl -> GetMarkerTime(This,MarkerNum,pMarkerTime)

#define INSPlay_GetMarkerName(This,MarkerNum,pbstrMarkerName)	\
    (This)->lpVtbl -> GetMarkerName(This,MarkerNum,pbstrMarkerName)


#define INSPlay_get_ChannelName(This,pbstrChannelName)	\
    (This)->lpVtbl -> get_ChannelName(This,pbstrChannelName)

#define INSPlay_get_ChannelDescription(This,pbstrChannelDescription)	\
    (This)->lpVtbl -> get_ChannelDescription(This,pbstrChannelDescription)

#define INSPlay_get_ChannelURL(This,pbstrChannelURL)	\
    (This)->lpVtbl -> get_ChannelURL(This,pbstrChannelURL)

#define INSPlay_get_ContactAddress(This,pbstrContactAddress)	\
    (This)->lpVtbl -> get_ContactAddress(This,pbstrContactAddress)

#define INSPlay_get_ContactPhone(This,pbstrContactPhone)	\
    (This)->lpVtbl -> get_ContactPhone(This,pbstrContactPhone)

#define INSPlay_get_ContactEmail(This,pbstrContactEmail)	\
    (This)->lpVtbl -> get_ContactEmail(This,pbstrContactEmail)

#define INSPlay_get_AllowChangeDisplaySize(This,pAllowChangeDisplaySize)	\
    (This)->lpVtbl -> get_AllowChangeDisplaySize(This,pAllowChangeDisplaySize)

#define INSPlay_put_AllowChangeDisplaySize(This,AllowChangeDisplaySize)	\
    (This)->lpVtbl -> put_AllowChangeDisplaySize(This,AllowChangeDisplaySize)

#define INSPlay_get_CodecCount(This,pCodecCount)	\
    (This)->lpVtbl -> get_CodecCount(This,pCodecCount)

#define INSPlay_get_IsBroadcast(This,pIsBroadcast)	\
    (This)->lpVtbl -> get_IsBroadcast(This,pIsBroadcast)

#define INSPlay_get_IsDurationValid(This,pIsDurationValid)	\
    (This)->lpVtbl -> get_IsDurationValid(This,pIsDurationValid)

#define INSPlay_get_SourceProtocol(This,pSourceProtocol)	\
    (This)->lpVtbl -> get_SourceProtocol(This,pSourceProtocol)

#define INSPlay_get_OpenState(This,pOpenState)	\
    (This)->lpVtbl -> get_OpenState(This,pOpenState)

#define INSPlay_get_SendOpenStateChangeEvents(This,pSendOpenStateChangeEvents)	\
    (This)->lpVtbl -> get_SendOpenStateChangeEvents(This,pSendOpenStateChangeEvents)

#define INSPlay_put_SendOpenStateChangeEvents(This,SendOpenStateChangeEvents)	\
    (This)->lpVtbl -> put_SendOpenStateChangeEvents(This,SendOpenStateChangeEvents)

#define INSPlay_get_SendWarningEvents(This,pSendWarningEvents)	\
    (This)->lpVtbl -> get_SendWarningEvents(This,pSendWarningEvents)

#define INSPlay_put_SendWarningEvents(This,SendWarningEvents)	\
    (This)->lpVtbl -> put_SendWarningEvents(This,SendWarningEvents)

#define INSPlay_get_SendErrorEvents(This,pSendErrorEvents)	\
    (This)->lpVtbl -> get_SendErrorEvents(This,pSendErrorEvents)

#define INSPlay_put_SendErrorEvents(This,SendErrorEvents)	\
    (This)->lpVtbl -> put_SendErrorEvents(This,SendErrorEvents)

#define INSPlay_get_HasError(This,pHasError)	\
    (This)->lpVtbl -> get_HasError(This,pHasError)

#define INSPlay_get_ErrorDescription(This,pbstrErrorDescription)	\
    (This)->lpVtbl -> get_ErrorDescription(This,pbstrErrorDescription)

#define INSPlay_get_ErrorCode(This,pErrorCode)	\
    (This)->lpVtbl -> get_ErrorCode(This,pErrorCode)

#define INSPlay_get_PlayState(This,pPlayState)	\
    (This)->lpVtbl -> get_PlayState(This,pPlayState)

#define INSPlay_get_SendPlayStateChangeEvents(This,pSendPlayStateChangeEvents)	\
    (This)->lpVtbl -> get_SendPlayStateChangeEvents(This,pSendPlayStateChangeEvents)

#define INSPlay_put_SendPlayStateChangeEvents(This,SendPlayStateChangeEvents)	\
    (This)->lpVtbl -> put_SendPlayStateChangeEvents(This,SendPlayStateChangeEvents)

#define INSPlay_get_BufferingTime(This,pBufferingTime)	\
    (This)->lpVtbl -> get_BufferingTime(This,pBufferingTime)

#define INSPlay_put_BufferingTime(This,BufferingTime)	\
    (This)->lpVtbl -> put_BufferingTime(This,BufferingTime)

#define INSPlay_get_UseFixedUDPPort(This,pUseFixedUDPPort)	\
    (This)->lpVtbl -> get_UseFixedUDPPort(This,pUseFixedUDPPort)

#define INSPlay_put_UseFixedUDPPort(This,UseFixedUDPPort)	\
    (This)->lpVtbl -> put_UseFixedUDPPort(This,UseFixedUDPPort)

#define INSPlay_get_FixedUDPPort(This,pFixedUDPPort)	\
    (This)->lpVtbl -> get_FixedUDPPort(This,pFixedUDPPort)

#define INSPlay_put_FixedUDPPort(This,FixedUDPPort)	\
    (This)->lpVtbl -> put_FixedUDPPort(This,FixedUDPPort)

#define INSPlay_get_UseHTTPProxy(This,pUseHTTPProxy)	\
    (This)->lpVtbl -> get_UseHTTPProxy(This,pUseHTTPProxy)

#define INSPlay_put_UseHTTPProxy(This,UseHTTPProxy)	\
    (This)->lpVtbl -> put_UseHTTPProxy(This,UseHTTPProxy)

#define INSPlay_get_EnableAutoProxy(This,pEnableAutoProxy)	\
    (This)->lpVtbl -> get_EnableAutoProxy(This,pEnableAutoProxy)

#define INSPlay_put_EnableAutoProxy(This,EnableAutoProxy)	\
    (This)->lpVtbl -> put_EnableAutoProxy(This,EnableAutoProxy)

#define INSPlay_get_HTTPProxyHost(This,pbstrHTTPProxyHost)	\
    (This)->lpVtbl -> get_HTTPProxyHost(This,pbstrHTTPProxyHost)

#define INSPlay_put_HTTPProxyHost(This,bstrHTTPProxyHost)	\
    (This)->lpVtbl -> put_HTTPProxyHost(This,bstrHTTPProxyHost)

#define INSPlay_get_HTTPProxyPort(This,pHTTPProxyPort)	\
    (This)->lpVtbl -> get_HTTPProxyPort(This,pHTTPProxyPort)

#define INSPlay_put_HTTPProxyPort(This,HTTPProxyPort)	\
    (This)->lpVtbl -> put_HTTPProxyPort(This,HTTPProxyPort)

#define INSPlay_get_EnableMulticast(This,pEnableMulticast)	\
    (This)->lpVtbl -> get_EnableMulticast(This,pEnableMulticast)

#define INSPlay_put_EnableMulticast(This,EnableMulticast)	\
    (This)->lpVtbl -> put_EnableMulticast(This,EnableMulticast)

#define INSPlay_get_EnableUDP(This,pEnableUDP)	\
    (This)->lpVtbl -> get_EnableUDP(This,pEnableUDP)

#define INSPlay_put_EnableUDP(This,EnableUDP)	\
    (This)->lpVtbl -> put_EnableUDP(This,EnableUDP)

#define INSPlay_get_EnableTCP(This,pEnableTCP)	\
    (This)->lpVtbl -> get_EnableTCP(This,pEnableTCP)

#define INSPlay_put_EnableTCP(This,EnableTCP)	\
    (This)->lpVtbl -> put_EnableTCP(This,EnableTCP)

#define INSPlay_get_EnableHTTP(This,pEnableHTTP)	\
    (This)->lpVtbl -> get_EnableHTTP(This,pEnableHTTP)

#define INSPlay_put_EnableHTTP(This,EnableHTTP)	\
    (This)->lpVtbl -> put_EnableHTTP(This,EnableHTTP)

#define INSPlay_get_BufferingProgress(This,pBufferingProgress)	\
    (This)->lpVtbl -> get_BufferingProgress(This,pBufferingProgress)

#define INSPlay_get_BaseURL(This,pbstrBaseURL)	\
    (This)->lpVtbl -> get_BaseURL(This,pbstrBaseURL)

#define INSPlay_put_BaseURL(This,bstrBaseURL)	\
    (This)->lpVtbl -> put_BaseURL(This,bstrBaseURL)

#define INSPlay_get_DefaultFrame(This,pbstrDefaultFrame)	\
    (This)->lpVtbl -> get_DefaultFrame(This,pbstrDefaultFrame)

#define INSPlay_put_DefaultFrame(This,bstrDefaultFrame)	\
    (This)->lpVtbl -> put_DefaultFrame(This,bstrDefaultFrame)

#define INSPlay_AboutBox(This)	\
    (This)->lpVtbl -> AboutBox(This)

#define INSPlay_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)

#define INSPlay_GetCodecInstalled(This,CodecNum,pCodecInstalled)	\
    (This)->lpVtbl -> GetCodecInstalled(This,CodecNum,pCodecInstalled)

#define INSPlay_GetCodecDescription(This,CodecNum,pbstrCodecDescription)	\
    (This)->lpVtbl -> GetCodecDescription(This,CodecNum,pbstrCodecDescription)

#define INSPlay_GetCodecURL(This,CodecNum,pbstrCodecURL)	\
    (This)->lpVtbl -> GetCodecURL(This,CodecNum,pbstrCodecURL)

#define INSPlay_Open(This,bstrFileName)	\
    (This)->lpVtbl -> Open(This,bstrFileName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_ChannelName_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelName);


void __RPC_STUB INSPlay_get_ChannelName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_ChannelDescription_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelDescription);


void __RPC_STUB INSPlay_get_ChannelDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_ChannelURL_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelURL);


void __RPC_STUB INSPlay_get_ChannelURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_ContactAddress_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrContactAddress);


void __RPC_STUB INSPlay_get_ContactAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_ContactPhone_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrContactPhone);


void __RPC_STUB INSPlay_get_ContactPhone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_ContactEmail_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrContactEmail);


void __RPC_STUB INSPlay_get_ContactEmail_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_AllowChangeDisplaySize_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAllowChangeDisplaySize);


void __RPC_STUB INSPlay_get_AllowChangeDisplaySize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_AllowChangeDisplaySize_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL AllowChangeDisplaySize);


void __RPC_STUB INSPlay_put_AllowChangeDisplaySize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_CodecCount_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCodecCount);


void __RPC_STUB INSPlay_get_CodecCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_IsBroadcast_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsBroadcast);


void __RPC_STUB INSPlay_get_IsBroadcast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_IsDurationValid_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsDurationValid);


void __RPC_STUB INSPlay_get_IsDurationValid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_SourceProtocol_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceProtocol);


void __RPC_STUB INSPlay_get_SourceProtocol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_OpenState_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pOpenState);


void __RPC_STUB INSPlay_get_OpenState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_SendOpenStateChangeEvents_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendOpenStateChangeEvents);


void __RPC_STUB INSPlay_get_SendOpenStateChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_SendOpenStateChangeEvents_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL SendOpenStateChangeEvents);


void __RPC_STUB INSPlay_put_SendOpenStateChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_SendWarningEvents_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendWarningEvents);


void __RPC_STUB INSPlay_get_SendWarningEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_SendWarningEvents_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL SendWarningEvents);


void __RPC_STUB INSPlay_put_SendWarningEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_SendErrorEvents_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendErrorEvents);


void __RPC_STUB INSPlay_get_SendErrorEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_SendErrorEvents_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL SendErrorEvents);


void __RPC_STUB INSPlay_put_SendErrorEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_HasError_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pHasError);


void __RPC_STUB INSPlay_get_HasError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_ErrorDescription_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorDescription);


void __RPC_STUB INSPlay_get_ErrorDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_ErrorCode_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pErrorCode);


void __RPC_STUB INSPlay_get_ErrorCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_PlayState_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pPlayState);


void __RPC_STUB INSPlay_get_PlayState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_SendPlayStateChangeEvents_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendPlayStateChangeEvents);


void __RPC_STUB INSPlay_get_SendPlayStateChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_SendPlayStateChangeEvents_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL SendPlayStateChangeEvents);


void __RPC_STUB INSPlay_put_SendPlayStateChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_BufferingTime_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pBufferingTime);


void __RPC_STUB INSPlay_get_BufferingTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_BufferingTime_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ double BufferingTime);


void __RPC_STUB INSPlay_put_BufferingTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_UseFixedUDPPort_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseFixedUDPPort);


void __RPC_STUB INSPlay_get_UseFixedUDPPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_UseFixedUDPPort_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL UseFixedUDPPort);


void __RPC_STUB INSPlay_put_UseFixedUDPPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_FixedUDPPort_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pFixedUDPPort);


void __RPC_STUB INSPlay_get_FixedUDPPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_FixedUDPPort_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ long FixedUDPPort);


void __RPC_STUB INSPlay_put_FixedUDPPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_UseHTTPProxy_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseHTTPProxy);


void __RPC_STUB INSPlay_get_UseHTTPProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_UseHTTPProxy_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL UseHTTPProxy);


void __RPC_STUB INSPlay_put_UseHTTPProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_EnableAutoProxy_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableAutoProxy);


void __RPC_STUB INSPlay_get_EnableAutoProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_EnableAutoProxy_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableAutoProxy);


void __RPC_STUB INSPlay_put_EnableAutoProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_HTTPProxyHost_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrHTTPProxyHost);


void __RPC_STUB INSPlay_get_HTTPProxyHost_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_HTTPProxyHost_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ BSTR bstrHTTPProxyHost);


void __RPC_STUB INSPlay_put_HTTPProxyHost_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_HTTPProxyPort_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pHTTPProxyPort);


void __RPC_STUB INSPlay_get_HTTPProxyPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_HTTPProxyPort_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ long HTTPProxyPort);


void __RPC_STUB INSPlay_put_HTTPProxyPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_EnableMulticast_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableMulticast);


void __RPC_STUB INSPlay_get_EnableMulticast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_EnableMulticast_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableMulticast);


void __RPC_STUB INSPlay_put_EnableMulticast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_EnableUDP_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableUDP);


void __RPC_STUB INSPlay_get_EnableUDP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_EnableUDP_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableUDP);


void __RPC_STUB INSPlay_put_EnableUDP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_EnableTCP_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableTCP);


void __RPC_STUB INSPlay_get_EnableTCP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_EnableTCP_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableTCP);


void __RPC_STUB INSPlay_put_EnableTCP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_EnableHTTP_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableHTTP);


void __RPC_STUB INSPlay_get_EnableHTTP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_EnableHTTP_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableHTTP);


void __RPC_STUB INSPlay_put_EnableHTTP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_BufferingProgress_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBufferingProgress);


void __RPC_STUB INSPlay_get_BufferingProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_BaseURL_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrBaseURL);


void __RPC_STUB INSPlay_get_BaseURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_BaseURL_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ BSTR bstrBaseURL);


void __RPC_STUB INSPlay_put_BaseURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INSPlay_get_DefaultFrame_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDefaultFrame);


void __RPC_STUB INSPlay_get_DefaultFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INSPlay_put_DefaultFrame_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ BSTR bstrDefaultFrame);


void __RPC_STUB INSPlay_put_DefaultFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSPlay_AboutBox_Proxy( 
    INSPlay __RPC_FAR * This);


void __RPC_STUB INSPlay_AboutBox_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSPlay_Cancel_Proxy( 
    INSPlay __RPC_FAR * This);


void __RPC_STUB INSPlay_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSPlay_GetCodecInstalled_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ long CodecNum,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCodecInstalled);


void __RPC_STUB INSPlay_GetCodecInstalled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSPlay_GetCodecDescription_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ long CodecNum,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecDescription);


void __RPC_STUB INSPlay_GetCodecDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSPlay_GetCodecURL_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ long CodecNum,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecURL);


void __RPC_STUB INSPlay_GetCodecURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INSPlay_Open_Proxy( 
    INSPlay __RPC_FAR * This,
    /* [in] */ BSTR bstrFileName);


void __RPC_STUB INSPlay_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INSPlay_INTERFACE_DEFINED__ */


#ifndef __INSPlay1_INTERFACE_DEFINED__
#define __INSPlay1_INTERFACE_DEFINED__

/* interface INSPlay1 */
/* [hidden][dual][uuid][object] */ 


EXTERN_C const IID IID_INSPlay1;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("265EC141-AE62-11d1-8500-00A0C91F9CA0")
    INSPlay1 : public INSPlay
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_MediaPlayer( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdispatch) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INSPlay1Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INSPlay1 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INSPlay1 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INSPlay1 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceWidth )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pWidth);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceHeight )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHeight);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pDuration);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Author )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Copyright )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceLink )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSourceLink);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MarkerCount )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pMarkerCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanScan )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanScan);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanSeek )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeek);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanSeekToMarkers )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCanSeekToMarkers);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CreationDate )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pCreationDate);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bandwidth )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBandwidth);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorCorrection )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorCorrection);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoStart )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoStart);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoStart )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AutoStart);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoRewind )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAutoRewind);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoRewind )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AutoRewind);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InvokeURLs )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pInvokeURLs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_InvokeURLs )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL InvokeURLs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableContextMenu )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableContextMenu);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableContextMenu )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableContextMenu);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransparentAtStart )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pTransparentAtStart);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TransparentAtStart )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL TransparentAtStart);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClickToPlay )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pClickToPlay);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ClickToPlay )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL ClickToPlay);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileName )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FileName )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPosition )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pCurrentPosition);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentPosition )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ double CurrentPosition);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rate )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pRate);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rate )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ double Rate);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentMarker )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCurrentMarker);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentMarker )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long CurrentMarker);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayCount )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pPlayCount);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayCount )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long PlayCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentState )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCurrentState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplaySize )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDisplaySize);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplaySize )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long DisplaySize);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MainWindow )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pMainWindow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowScan )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAllowScan);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowScan )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AllowScan);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendKeyboardEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendKeyboardEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendKeyboardEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendKeyboardEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendMouseClickEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseClickEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendMouseClickEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendMouseClickEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendMouseMoveEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendMouseMoveEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendMouseMoveEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendMouseMoveEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendStateChangeEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendStateChangeEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendStateChangeEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendStateChangeEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceivedPackets )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pReceivedPackets);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecoveredPackets )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pRecoveredPackets);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LostPackets )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pLostPackets);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceptionQuality )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pReceptionQuality);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingCount )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBufferingCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorType )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCursorType);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorType )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long CursorType);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AnimationAtStart )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationAtStart);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AnimationAtStart )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AnimationAtStart);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AnimationOnStop )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAnimationOnStop);
        
        /* [hidden][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AnimationOnStop )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AnimationOnStop);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Play )( 
            INSPlay1 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            INSPlay1 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            INSPlay1 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMarkerTime )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long MarkerNum,
            /* [retval][out] */ double __RPC_FAR *pMarkerTime);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMarkerName )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long MarkerNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMarkerName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ChannelName )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ChannelDescription )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ChannelURL )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelURL);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContactAddress )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactAddress);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContactPhone )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactPhone);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContactEmail )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactEmail);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowChangeDisplaySize )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pAllowChangeDisplaySize);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowChangeDisplaySize )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL AllowChangeDisplaySize);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CodecCount )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCodecCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsBroadcast )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsBroadcast);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsDurationValid )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsDurationValid);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceProtocol )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceProtocol);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OpenState )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pOpenState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendOpenStateChangeEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendOpenStateChangeEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendOpenStateChangeEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendOpenStateChangeEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendWarningEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendWarningEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendWarningEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendWarningEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendErrorEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendErrorEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendErrorEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendErrorEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HasError )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pHasError);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorDescription )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorCode )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pErrorCode);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayState )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pPlayState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SendPlayStateChangeEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSendPlayStateChangeEvents);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SendPlayStateChangeEvents )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL SendPlayStateChangeEvents);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingTime )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pBufferingTime);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BufferingTime )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ double BufferingTime);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseFixedUDPPort )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseFixedUDPPort);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseFixedUDPPort )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL UseFixedUDPPort);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FixedUDPPort )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pFixedUDPPort);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FixedUDPPort )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long FixedUDPPort);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseHTTPProxy )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseHTTPProxy);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseHTTPProxy )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL UseHTTPProxy);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableAutoProxy )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableAutoProxy);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableAutoProxy )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableAutoProxy);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HTTPProxyHost )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrHTTPProxyHost);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HTTPProxyHost )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ BSTR bstrHTTPProxyHost);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HTTPProxyPort )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHTTPProxyPort);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HTTPProxyPort )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long HTTPProxyPort);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableMulticast )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableMulticast);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableMulticast )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableMulticast);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableUDP )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableUDP);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableUDP )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableUDP);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableTCP )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableTCP);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableTCP )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableTCP);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableHTTP )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableHTTP);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableHTTP )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableHTTP);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingProgress )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBufferingProgress);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BaseURL )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrBaseURL);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BaseURL )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ BSTR bstrBaseURL);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultFrame )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDefaultFrame);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefaultFrame )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ BSTR bstrDefaultFrame);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AboutBox )( 
            INSPlay1 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            INSPlay1 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCodecInstalled )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long CodecNum,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCodecInstalled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCodecDescription )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecDescription);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCodecURL )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecURL);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            INSPlay1 __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaPlayer )( 
            INSPlay1 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdispatch);
        
        END_INTERFACE
    } INSPlay1Vtbl;

    interface INSPlay1
    {
        CONST_VTBL struct INSPlay1Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INSPlay1_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INSPlay1_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INSPlay1_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INSPlay1_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INSPlay1_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INSPlay1_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INSPlay1_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INSPlay1_get_ImageSourceWidth(This,pWidth)	\
    (This)->lpVtbl -> get_ImageSourceWidth(This,pWidth)

#define INSPlay1_get_ImageSourceHeight(This,pHeight)	\
    (This)->lpVtbl -> get_ImageSourceHeight(This,pHeight)

#define INSPlay1_get_Duration(This,pDuration)	\
    (This)->lpVtbl -> get_Duration(This,pDuration)

#define INSPlay1_get_Author(This,pbstrAuthor)	\
    (This)->lpVtbl -> get_Author(This,pbstrAuthor)

#define INSPlay1_get_Copyright(This,pbstrCopyright)	\
    (This)->lpVtbl -> get_Copyright(This,pbstrCopyright)

#define INSPlay1_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define INSPlay1_get_Rating(This,pbstrRating)	\
    (This)->lpVtbl -> get_Rating(This,pbstrRating)

#define INSPlay1_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define INSPlay1_get_SourceLink(This,pbstrSourceLink)	\
    (This)->lpVtbl -> get_SourceLink(This,pbstrSourceLink)

#define INSPlay1_get_MarkerCount(This,pMarkerCount)	\
    (This)->lpVtbl -> get_MarkerCount(This,pMarkerCount)

#define INSPlay1_get_CanScan(This,pCanScan)	\
    (This)->lpVtbl -> get_CanScan(This,pCanScan)

#define INSPlay1_get_CanSeek(This,pCanSeek)	\
    (This)->lpVtbl -> get_CanSeek(This,pCanSeek)

#define INSPlay1_get_CanSeekToMarkers(This,pCanSeekToMarkers)	\
    (This)->lpVtbl -> get_CanSeekToMarkers(This,pCanSeekToMarkers)

#define INSPlay1_get_CreationDate(This,pCreationDate)	\
    (This)->lpVtbl -> get_CreationDate(This,pCreationDate)

#define INSPlay1_get_Bandwidth(This,pBandwidth)	\
    (This)->lpVtbl -> get_Bandwidth(This,pBandwidth)

#define INSPlay1_get_ErrorCorrection(This,pbstrErrorCorrection)	\
    (This)->lpVtbl -> get_ErrorCorrection(This,pbstrErrorCorrection)

#define INSPlay1_get_AutoStart(This,pAutoStart)	\
    (This)->lpVtbl -> get_AutoStart(This,pAutoStart)

#define INSPlay1_put_AutoStart(This,AutoStart)	\
    (This)->lpVtbl -> put_AutoStart(This,AutoStart)

#define INSPlay1_get_AutoRewind(This,pAutoRewind)	\
    (This)->lpVtbl -> get_AutoRewind(This,pAutoRewind)

#define INSPlay1_put_AutoRewind(This,AutoRewind)	\
    (This)->lpVtbl -> put_AutoRewind(This,AutoRewind)

#define INSPlay1_get_InvokeURLs(This,pInvokeURLs)	\
    (This)->lpVtbl -> get_InvokeURLs(This,pInvokeURLs)

#define INSPlay1_put_InvokeURLs(This,InvokeURLs)	\
    (This)->lpVtbl -> put_InvokeURLs(This,InvokeURLs)

#define INSPlay1_get_EnableContextMenu(This,pEnableContextMenu)	\
    (This)->lpVtbl -> get_EnableContextMenu(This,pEnableContextMenu)

#define INSPlay1_put_EnableContextMenu(This,EnableContextMenu)	\
    (This)->lpVtbl -> put_EnableContextMenu(This,EnableContextMenu)

#define INSPlay1_get_TransparentAtStart(This,pTransparentAtStart)	\
    (This)->lpVtbl -> get_TransparentAtStart(This,pTransparentAtStart)

#define INSPlay1_put_TransparentAtStart(This,TransparentAtStart)	\
    (This)->lpVtbl -> put_TransparentAtStart(This,TransparentAtStart)

#define INSPlay1_get_ClickToPlay(This,pClickToPlay)	\
    (This)->lpVtbl -> get_ClickToPlay(This,pClickToPlay)

#define INSPlay1_put_ClickToPlay(This,ClickToPlay)	\
    (This)->lpVtbl -> put_ClickToPlay(This,ClickToPlay)

#define INSPlay1_get_FileName(This,pbstrFileName)	\
    (This)->lpVtbl -> get_FileName(This,pbstrFileName)

#define INSPlay1_put_FileName(This,bstrFileName)	\
    (This)->lpVtbl -> put_FileName(This,bstrFileName)

#define INSPlay1_get_CurrentPosition(This,pCurrentPosition)	\
    (This)->lpVtbl -> get_CurrentPosition(This,pCurrentPosition)

#define INSPlay1_put_CurrentPosition(This,CurrentPosition)	\
    (This)->lpVtbl -> put_CurrentPosition(This,CurrentPosition)

#define INSPlay1_get_Rate(This,pRate)	\
    (This)->lpVtbl -> get_Rate(This,pRate)

#define INSPlay1_put_Rate(This,Rate)	\
    (This)->lpVtbl -> put_Rate(This,Rate)

#define INSPlay1_get_CurrentMarker(This,pCurrentMarker)	\
    (This)->lpVtbl -> get_CurrentMarker(This,pCurrentMarker)

#define INSPlay1_put_CurrentMarker(This,CurrentMarker)	\
    (This)->lpVtbl -> put_CurrentMarker(This,CurrentMarker)

#define INSPlay1_get_PlayCount(This,pPlayCount)	\
    (This)->lpVtbl -> get_PlayCount(This,pPlayCount)

#define INSPlay1_put_PlayCount(This,PlayCount)	\
    (This)->lpVtbl -> put_PlayCount(This,PlayCount)

#define INSPlay1_get_CurrentState(This,pCurrentState)	\
    (This)->lpVtbl -> get_CurrentState(This,pCurrentState)

#define INSPlay1_get_DisplaySize(This,pDisplaySize)	\
    (This)->lpVtbl -> get_DisplaySize(This,pDisplaySize)

#define INSPlay1_put_DisplaySize(This,DisplaySize)	\
    (This)->lpVtbl -> put_DisplaySize(This,DisplaySize)

#define INSPlay1_get_MainWindow(This,pMainWindow)	\
    (This)->lpVtbl -> get_MainWindow(This,pMainWindow)

#define INSPlay1_get_AllowScan(This,pAllowScan)	\
    (This)->lpVtbl -> get_AllowScan(This,pAllowScan)

#define INSPlay1_put_AllowScan(This,AllowScan)	\
    (This)->lpVtbl -> put_AllowScan(This,AllowScan)

#define INSPlay1_get_SendKeyboardEvents(This,pSendKeyboardEvents)	\
    (This)->lpVtbl -> get_SendKeyboardEvents(This,pSendKeyboardEvents)

#define INSPlay1_put_SendKeyboardEvents(This,SendKeyboardEvents)	\
    (This)->lpVtbl -> put_SendKeyboardEvents(This,SendKeyboardEvents)

#define INSPlay1_get_SendMouseClickEvents(This,pSendMouseClickEvents)	\
    (This)->lpVtbl -> get_SendMouseClickEvents(This,pSendMouseClickEvents)

#define INSPlay1_put_SendMouseClickEvents(This,SendMouseClickEvents)	\
    (This)->lpVtbl -> put_SendMouseClickEvents(This,SendMouseClickEvents)

#define INSPlay1_get_SendMouseMoveEvents(This,pSendMouseMoveEvents)	\
    (This)->lpVtbl -> get_SendMouseMoveEvents(This,pSendMouseMoveEvents)

#define INSPlay1_put_SendMouseMoveEvents(This,SendMouseMoveEvents)	\
    (This)->lpVtbl -> put_SendMouseMoveEvents(This,SendMouseMoveEvents)

#define INSPlay1_get_SendStateChangeEvents(This,pSendStateChangeEvents)	\
    (This)->lpVtbl -> get_SendStateChangeEvents(This,pSendStateChangeEvents)

#define INSPlay1_put_SendStateChangeEvents(This,SendStateChangeEvents)	\
    (This)->lpVtbl -> put_SendStateChangeEvents(This,SendStateChangeEvents)

#define INSPlay1_get_ReceivedPackets(This,pReceivedPackets)	\
    (This)->lpVtbl -> get_ReceivedPackets(This,pReceivedPackets)

#define INSPlay1_get_RecoveredPackets(This,pRecoveredPackets)	\
    (This)->lpVtbl -> get_RecoveredPackets(This,pRecoveredPackets)

#define INSPlay1_get_LostPackets(This,pLostPackets)	\
    (This)->lpVtbl -> get_LostPackets(This,pLostPackets)

#define INSPlay1_get_ReceptionQuality(This,pReceptionQuality)	\
    (This)->lpVtbl -> get_ReceptionQuality(This,pReceptionQuality)

#define INSPlay1_get_BufferingCount(This,pBufferingCount)	\
    (This)->lpVtbl -> get_BufferingCount(This,pBufferingCount)

#define INSPlay1_get_CursorType(This,pCursorType)	\
    (This)->lpVtbl -> get_CursorType(This,pCursorType)

#define INSPlay1_put_CursorType(This,CursorType)	\
    (This)->lpVtbl -> put_CursorType(This,CursorType)

#define INSPlay1_get_AnimationAtStart(This,pAnimationAtStart)	\
    (This)->lpVtbl -> get_AnimationAtStart(This,pAnimationAtStart)

#define INSPlay1_put_AnimationAtStart(This,AnimationAtStart)	\
    (This)->lpVtbl -> put_AnimationAtStart(This,AnimationAtStart)

#define INSPlay1_get_AnimationOnStop(This,pAnimationOnStop)	\
    (This)->lpVtbl -> get_AnimationOnStop(This,pAnimationOnStop)

#define INSPlay1_put_AnimationOnStop(This,AnimationOnStop)	\
    (This)->lpVtbl -> put_AnimationOnStop(This,AnimationOnStop)

#define INSPlay1_Play(This)	\
    (This)->lpVtbl -> Play(This)

#define INSPlay1_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define INSPlay1_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define INSPlay1_GetMarkerTime(This,MarkerNum,pMarkerTime)	\
    (This)->lpVtbl -> GetMarkerTime(This,MarkerNum,pMarkerTime)

#define INSPlay1_GetMarkerName(This,MarkerNum,pbstrMarkerName)	\
    (This)->lpVtbl -> GetMarkerName(This,MarkerNum,pbstrMarkerName)


#define INSPlay1_get_ChannelName(This,pbstrChannelName)	\
    (This)->lpVtbl -> get_ChannelName(This,pbstrChannelName)

#define INSPlay1_get_ChannelDescription(This,pbstrChannelDescription)	\
    (This)->lpVtbl -> get_ChannelDescription(This,pbstrChannelDescription)

#define INSPlay1_get_ChannelURL(This,pbstrChannelURL)	\
    (This)->lpVtbl -> get_ChannelURL(This,pbstrChannelURL)

#define INSPlay1_get_ContactAddress(This,pbstrContactAddress)	\
    (This)->lpVtbl -> get_ContactAddress(This,pbstrContactAddress)

#define INSPlay1_get_ContactPhone(This,pbstrContactPhone)	\
    (This)->lpVtbl -> get_ContactPhone(This,pbstrContactPhone)

#define INSPlay1_get_ContactEmail(This,pbstrContactEmail)	\
    (This)->lpVtbl -> get_ContactEmail(This,pbstrContactEmail)

#define INSPlay1_get_AllowChangeDisplaySize(This,pAllowChangeDisplaySize)	\
    (This)->lpVtbl -> get_AllowChangeDisplaySize(This,pAllowChangeDisplaySize)

#define INSPlay1_put_AllowChangeDisplaySize(This,AllowChangeDisplaySize)	\
    (This)->lpVtbl -> put_AllowChangeDisplaySize(This,AllowChangeDisplaySize)

#define INSPlay1_get_CodecCount(This,pCodecCount)	\
    (This)->lpVtbl -> get_CodecCount(This,pCodecCount)

#define INSPlay1_get_IsBroadcast(This,pIsBroadcast)	\
    (This)->lpVtbl -> get_IsBroadcast(This,pIsBroadcast)

#define INSPlay1_get_IsDurationValid(This,pIsDurationValid)	\
    (This)->lpVtbl -> get_IsDurationValid(This,pIsDurationValid)

#define INSPlay1_get_SourceProtocol(This,pSourceProtocol)	\
    (This)->lpVtbl -> get_SourceProtocol(This,pSourceProtocol)

#define INSPlay1_get_OpenState(This,pOpenState)	\
    (This)->lpVtbl -> get_OpenState(This,pOpenState)

#define INSPlay1_get_SendOpenStateChangeEvents(This,pSendOpenStateChangeEvents)	\
    (This)->lpVtbl -> get_SendOpenStateChangeEvents(This,pSendOpenStateChangeEvents)

#define INSPlay1_put_SendOpenStateChangeEvents(This,SendOpenStateChangeEvents)	\
    (This)->lpVtbl -> put_SendOpenStateChangeEvents(This,SendOpenStateChangeEvents)

#define INSPlay1_get_SendWarningEvents(This,pSendWarningEvents)	\
    (This)->lpVtbl -> get_SendWarningEvents(This,pSendWarningEvents)

#define INSPlay1_put_SendWarningEvents(This,SendWarningEvents)	\
    (This)->lpVtbl -> put_SendWarningEvents(This,SendWarningEvents)

#define INSPlay1_get_SendErrorEvents(This,pSendErrorEvents)	\
    (This)->lpVtbl -> get_SendErrorEvents(This,pSendErrorEvents)

#define INSPlay1_put_SendErrorEvents(This,SendErrorEvents)	\
    (This)->lpVtbl -> put_SendErrorEvents(This,SendErrorEvents)

#define INSPlay1_get_HasError(This,pHasError)	\
    (This)->lpVtbl -> get_HasError(This,pHasError)

#define INSPlay1_get_ErrorDescription(This,pbstrErrorDescription)	\
    (This)->lpVtbl -> get_ErrorDescription(This,pbstrErrorDescription)

#define INSPlay1_get_ErrorCode(This,pErrorCode)	\
    (This)->lpVtbl -> get_ErrorCode(This,pErrorCode)

#define INSPlay1_get_PlayState(This,pPlayState)	\
    (This)->lpVtbl -> get_PlayState(This,pPlayState)

#define INSPlay1_get_SendPlayStateChangeEvents(This,pSendPlayStateChangeEvents)	\
    (This)->lpVtbl -> get_SendPlayStateChangeEvents(This,pSendPlayStateChangeEvents)

#define INSPlay1_put_SendPlayStateChangeEvents(This,SendPlayStateChangeEvents)	\
    (This)->lpVtbl -> put_SendPlayStateChangeEvents(This,SendPlayStateChangeEvents)

#define INSPlay1_get_BufferingTime(This,pBufferingTime)	\
    (This)->lpVtbl -> get_BufferingTime(This,pBufferingTime)

#define INSPlay1_put_BufferingTime(This,BufferingTime)	\
    (This)->lpVtbl -> put_BufferingTime(This,BufferingTime)

#define INSPlay1_get_UseFixedUDPPort(This,pUseFixedUDPPort)	\
    (This)->lpVtbl -> get_UseFixedUDPPort(This,pUseFixedUDPPort)

#define INSPlay1_put_UseFixedUDPPort(This,UseFixedUDPPort)	\
    (This)->lpVtbl -> put_UseFixedUDPPort(This,UseFixedUDPPort)

#define INSPlay1_get_FixedUDPPort(This,pFixedUDPPort)	\
    (This)->lpVtbl -> get_FixedUDPPort(This,pFixedUDPPort)

#define INSPlay1_put_FixedUDPPort(This,FixedUDPPort)	\
    (This)->lpVtbl -> put_FixedUDPPort(This,FixedUDPPort)

#define INSPlay1_get_UseHTTPProxy(This,pUseHTTPProxy)	\
    (This)->lpVtbl -> get_UseHTTPProxy(This,pUseHTTPProxy)

#define INSPlay1_put_UseHTTPProxy(This,UseHTTPProxy)	\
    (This)->lpVtbl -> put_UseHTTPProxy(This,UseHTTPProxy)

#define INSPlay1_get_EnableAutoProxy(This,pEnableAutoProxy)	\
    (This)->lpVtbl -> get_EnableAutoProxy(This,pEnableAutoProxy)

#define INSPlay1_put_EnableAutoProxy(This,EnableAutoProxy)	\
    (This)->lpVtbl -> put_EnableAutoProxy(This,EnableAutoProxy)

#define INSPlay1_get_HTTPProxyHost(This,pbstrHTTPProxyHost)	\
    (This)->lpVtbl -> get_HTTPProxyHost(This,pbstrHTTPProxyHost)

#define INSPlay1_put_HTTPProxyHost(This,bstrHTTPProxyHost)	\
    (This)->lpVtbl -> put_HTTPProxyHost(This,bstrHTTPProxyHost)

#define INSPlay1_get_HTTPProxyPort(This,pHTTPProxyPort)	\
    (This)->lpVtbl -> get_HTTPProxyPort(This,pHTTPProxyPort)

#define INSPlay1_put_HTTPProxyPort(This,HTTPProxyPort)	\
    (This)->lpVtbl -> put_HTTPProxyPort(This,HTTPProxyPort)

#define INSPlay1_get_EnableMulticast(This,pEnableMulticast)	\
    (This)->lpVtbl -> get_EnableMulticast(This,pEnableMulticast)

#define INSPlay1_put_EnableMulticast(This,EnableMulticast)	\
    (This)->lpVtbl -> put_EnableMulticast(This,EnableMulticast)

#define INSPlay1_get_EnableUDP(This,pEnableUDP)	\
    (This)->lpVtbl -> get_EnableUDP(This,pEnableUDP)

#define INSPlay1_put_EnableUDP(This,EnableUDP)	\
    (This)->lpVtbl -> put_EnableUDP(This,EnableUDP)

#define INSPlay1_get_EnableTCP(This,pEnableTCP)	\
    (This)->lpVtbl -> get_EnableTCP(This,pEnableTCP)

#define INSPlay1_put_EnableTCP(This,EnableTCP)	\
    (This)->lpVtbl -> put_EnableTCP(This,EnableTCP)

#define INSPlay1_get_EnableHTTP(This,pEnableHTTP)	\
    (This)->lpVtbl -> get_EnableHTTP(This,pEnableHTTP)

#define INSPlay1_put_EnableHTTP(This,EnableHTTP)	\
    (This)->lpVtbl -> put_EnableHTTP(This,EnableHTTP)

#define INSPlay1_get_BufferingProgress(This,pBufferingProgress)	\
    (This)->lpVtbl -> get_BufferingProgress(This,pBufferingProgress)

#define INSPlay1_get_BaseURL(This,pbstrBaseURL)	\
    (This)->lpVtbl -> get_BaseURL(This,pbstrBaseURL)

#define INSPlay1_put_BaseURL(This,bstrBaseURL)	\
    (This)->lpVtbl -> put_BaseURL(This,bstrBaseURL)

#define INSPlay1_get_DefaultFrame(This,pbstrDefaultFrame)	\
    (This)->lpVtbl -> get_DefaultFrame(This,pbstrDefaultFrame)

#define INSPlay1_put_DefaultFrame(This,bstrDefaultFrame)	\
    (This)->lpVtbl -> put_DefaultFrame(This,bstrDefaultFrame)

#define INSPlay1_AboutBox(This)	\
    (This)->lpVtbl -> AboutBox(This)

#define INSPlay1_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)

#define INSPlay1_GetCodecInstalled(This,CodecNum,pCodecInstalled)	\
    (This)->lpVtbl -> GetCodecInstalled(This,CodecNum,pCodecInstalled)

#define INSPlay1_GetCodecDescription(This,CodecNum,pbstrCodecDescription)	\
    (This)->lpVtbl -> GetCodecDescription(This,CodecNum,pbstrCodecDescription)

#define INSPlay1_GetCodecURL(This,CodecNum,pbstrCodecURL)	\
    (This)->lpVtbl -> GetCodecURL(This,CodecNum,pbstrCodecURL)

#define INSPlay1_Open(This,bstrFileName)	\
    (This)->lpVtbl -> Open(This,bstrFileName)


#define INSPlay1_get_MediaPlayer(This,ppdispatch)	\
    (This)->lpVtbl -> get_MediaPlayer(This,ppdispatch)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INSPlay1_get_MediaPlayer_Proxy( 
    INSPlay1 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdispatch);


void __RPC_STUB INSPlay1_get_MediaPlayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INSPlay1_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


