
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Thu Jun 24 19:41:45 2004
 */
/* Compiler settings for .\amovie.idl:
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

#ifndef __amovie_h__
#define __amovie_h__

/* Forward Declarations */ 

#ifndef __IActiveMovie_FWD_DEFINED__
#define __IActiveMovie_FWD_DEFINED__
typedef interface IActiveMovie IActiveMovie;
#endif 	/* __IActiveMovie_FWD_DEFINED__ */


#ifndef __IActiveMovie2_FWD_DEFINED__
#define __IActiveMovie2_FWD_DEFINED__
typedef interface IActiveMovie2 IActiveMovie2;
#endif 	/* __IActiveMovie2_FWD_DEFINED__ */


#ifndef __IActiveMovie3_FWD_DEFINED__
#define __IActiveMovie3_FWD_DEFINED__
typedef interface IActiveMovie3 IActiveMovie3;
#endif 	/* __IActiveMovie3_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "enums.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_amovie_0000 */
/* [local] */ 

typedef /* [public] */ 
enum WindowSizeConstants
    {	amvOriginalSize	= 0,
	amvDoubleOriginalSize	= amvOriginalSize + 1,
	amvOneSixteenthScreen	= amvDoubleOriginalSize + 1,
	amvOneFourthScreen	= amvOneSixteenthScreen + 1,
	amvOneHalfScreen	= amvOneFourthScreen + 1
    }	WindowSizeConstants;

typedef /* [public] */ 
enum StateConstants
    {	amvNotLoaded	= -1,
	amvStopped	= amvNotLoaded + 1,
	amvPaused	= amvStopped + 1,
	amvRunning	= amvPaused + 1
    }	StateConstants;

typedef /* [public] */ 
enum DisplayModeConstants
    {	amvTime	= 0,
	amvFrames	= amvTime + 1
    }	DisplayModeConstants;

typedef /* [public] */ 
enum AppearanceConstants
    {	amvFlat	= 0,
	amv3D	= amvFlat + 1
    }	AppearanceConstants;

typedef /* [public] */ 
enum BorderStyleConstants
    {	amvNone	= 0,
	amvFixedSingle	= amvNone + 1
    }	BorderStyleConstants;



extern RPC_IF_HANDLE __MIDL_itf_amovie_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_amovie_0000_v0_0_s_ifspec;

#ifndef __IActiveMovie_INTERFACE_DEFINED__
#define __IActiveMovie_INTERFACE_DEFINED__

/* interface IActiveMovie */
/* [unique][hidden][dual][uuid][object] */ 


EXTERN_C const IID IID_IActiveMovie;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("05589fa2-c356-11ce-bf01-00aa0055595a")
    IActiveMovie : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AboutBox( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Run( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ImageSourceWidth( 
            /* [retval][out] */ long __RPC_FAR *pWidth) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ImageSourceHeight( 
            /* [retval][out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Author( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Title( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Copyright( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Rating( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FileName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_FileName( 
            /* [in] */ BSTR bstrFileName) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Duration( 
            /* [retval][out] */ double __RPC_FAR *pValue) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentPosition( 
            /* [retval][out] */ double __RPC_FAR *pValue) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CurrentPosition( 
            /* [in] */ double Value) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_PlayCount( 
            /* [retval][out] */ long __RPC_FAR *pPlayCount) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_PlayCount( 
            /* [in] */ long PlayCount) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SelectionStart( 
            /* [retval][out] */ double __RPC_FAR *pValue) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SelectionStart( 
            /* [in] */ double Value) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SelectionEnd( 
            /* [retval][out] */ double __RPC_FAR *pValue) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SelectionEnd( 
            /* [in] */ double Value) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentState( 
            /* [retval][out] */ StateConstants __RPC_FAR *pState) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Rate( 
            /* [retval][out] */ double __RPC_FAR *pValue) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Rate( 
            /* [in] */ double Value) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Volume( 
            /* [retval][out] */ long __RPC_FAR *pValue) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Volume( 
            /* [in] */ long Value) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Balance( 
            /* [retval][out] */ long __RPC_FAR *pValue) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Balance( 
            /* [in] */ long Value) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnableContextMenu( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnableContextMenu( 
            /* [in] */ VARIANT_BOOL Enable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ShowDisplay( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ShowDisplay( 
            /* [in] */ VARIANT_BOOL Show) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ShowControls( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ShowControls( 
            /* [in] */ VARIANT_BOOL Show) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ShowPositionControls( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ShowPositionControls( 
            /* [in] */ VARIANT_BOOL Show) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ShowSelectionControls( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ShowSelectionControls( 
            /* [in] */ VARIANT_BOOL Show) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ShowTracker( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ShowTracker( 
            /* [in] */ VARIANT_BOOL Show) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnablePositionControls( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnablePositionControls( 
            /* [in] */ VARIANT_BOOL Enable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnableSelectionControls( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnableSelectionControls( 
            /* [in] */ VARIANT_BOOL Enable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EnableTracker( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EnableTracker( 
            /* [in] */ VARIANT_BOOL Enable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowHideDisplay( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowHideDisplay( 
            /* [in] */ VARIANT_BOOL Show) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowHideControls( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowHideControls( 
            /* [in] */ VARIANT_BOOL Show) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DisplayMode( 
            /* [retval][out] */ DisplayModeConstants __RPC_FAR *pValue) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DisplayMode( 
            /* [in] */ DisplayModeConstants Value) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowChangeDisplayMode( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *fAllow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowChangeDisplayMode( 
            /* [in] */ VARIANT_BOOL Allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FilterGraph( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppFilterGraph) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_FilterGraph( 
            /* [in] */ IUnknown __RPC_FAR *pFilterGraph) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FilterGraphDispatch( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pDispatch) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DisplayForeColor( 
            /* [retval][out] */ OLE_COLOR __RPC_FAR *ForeColor) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DisplayForeColor( 
            /* [in] */ OLE_COLOR ForeColor) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DisplayBackColor( 
            /* [retval][out] */ OLE_COLOR __RPC_FAR *BackColor) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DisplayBackColor( 
            /* [in] */ OLE_COLOR BackColor) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_MovieWindowSize( 
            /* [retval][out] */ WindowSizeConstants __RPC_FAR *WindowSize) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_MovieWindowSize( 
            /* [in] */ WindowSizeConstants WindowSize) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FullScreenMode( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_FullScreenMode( 
            /* [in] */ VARIANT_BOOL Enable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoStart( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoStart( 
            /* [in] */ VARIANT_BOOL Enable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoRewind( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoRewind( 
            /* [in] */ VARIANT_BOOL Enable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_hWnd( 
            /* [retval][out] */ long __RPC_FAR *hWnd) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Appearance( 
            /* [retval][out] */ AppearanceConstants __RPC_FAR *pAppearance) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Appearance( 
            /* [in] */ AppearanceConstants Appearance) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BorderStyle( 
            /* [retval][out] */ BorderStyleConstants __RPC_FAR *pBorderStyle) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_BorderStyle( 
            /* [in] */ BorderStyleConstants BorderStyle) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL Enabled) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_Info( 
            /* [retval][out] */ long __RPC_FAR *ppInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveMovieVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IActiveMovie __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IActiveMovie __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IActiveMovie __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AboutBox )( 
            IActiveMovie __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Run )( 
            IActiveMovie __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IActiveMovie __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IActiveMovie __RPC_FAR * This);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceWidth )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pWidth);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceHeight )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHeight);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Author )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Copyright )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileName )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FileName )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPosition )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentPosition )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayCount )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pPlayCount);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayCount )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ long PlayCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectionStart )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelectionStart )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectionEnd )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelectionEnd )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentState )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ StateConstants __RPC_FAR *pState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rate )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rate )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Volume )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Volume )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ long Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Balance )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Balance )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ long Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableContextMenu )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableContextMenu )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowDisplay )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowDisplay )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowPositionControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowPositionControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowSelectionControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowSelectionControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowTracker )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowTracker )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnablePositionControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnablePositionControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableSelectionControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableSelectionControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableTracker )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableTracker )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowHideDisplay )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowHideDisplay )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowHideControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowHideControls )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayMode )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ DisplayModeConstants __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayMode )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ DisplayModeConstants Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowChangeDisplayMode )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *fAllow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowChangeDisplayMode )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterGraph )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppFilterGraph);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FilterGraph )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pFilterGraph);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterGraphDispatch )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pDispatch);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayForeColor )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *ForeColor);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayForeColor )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ OLE_COLOR ForeColor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayBackColor )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *BackColor);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayBackColor )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ OLE_COLOR BackColor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MovieWindowSize )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ WindowSizeConstants __RPC_FAR *WindowSize);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MovieWindowSize )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ WindowSizeConstants WindowSize);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullScreenMode )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FullScreenMode )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoStart )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoStart )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoRewind )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoRewind )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_hWnd )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *hWnd);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Appearance )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ AppearanceConstants __RPC_FAR *pAppearance);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Appearance )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ AppearanceConstants Appearance);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BorderStyle )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ BorderStyleConstants __RPC_FAR *pBorderStyle);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BorderStyle )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ BorderStyleConstants BorderStyle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            IActiveMovie __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enabled);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Info )( 
            IActiveMovie __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *ppInfo);
        
        END_INTERFACE
    } IActiveMovieVtbl;

    interface IActiveMovie
    {
        CONST_VTBL struct IActiveMovieVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveMovie_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveMovie_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveMovie_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveMovie_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IActiveMovie_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IActiveMovie_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IActiveMovie_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IActiveMovie_AboutBox(This)	\
    (This)->lpVtbl -> AboutBox(This)

#define IActiveMovie_Run(This)	\
    (This)->lpVtbl -> Run(This)

#define IActiveMovie_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IActiveMovie_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IActiveMovie_get_ImageSourceWidth(This,pWidth)	\
    (This)->lpVtbl -> get_ImageSourceWidth(This,pWidth)

#define IActiveMovie_get_ImageSourceHeight(This,pHeight)	\
    (This)->lpVtbl -> get_ImageSourceHeight(This,pHeight)

#define IActiveMovie_get_Author(This,pbstrAuthor)	\
    (This)->lpVtbl -> get_Author(This,pbstrAuthor)

#define IActiveMovie_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define IActiveMovie_get_Copyright(This,pbstrCopyright)	\
    (This)->lpVtbl -> get_Copyright(This,pbstrCopyright)

#define IActiveMovie_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define IActiveMovie_get_Rating(This,pbstrRating)	\
    (This)->lpVtbl -> get_Rating(This,pbstrRating)

#define IActiveMovie_get_FileName(This,pbstrFileName)	\
    (This)->lpVtbl -> get_FileName(This,pbstrFileName)

#define IActiveMovie_put_FileName(This,bstrFileName)	\
    (This)->lpVtbl -> put_FileName(This,bstrFileName)

#define IActiveMovie_get_Duration(This,pValue)	\
    (This)->lpVtbl -> get_Duration(This,pValue)

#define IActiveMovie_get_CurrentPosition(This,pValue)	\
    (This)->lpVtbl -> get_CurrentPosition(This,pValue)

#define IActiveMovie_put_CurrentPosition(This,Value)	\
    (This)->lpVtbl -> put_CurrentPosition(This,Value)

#define IActiveMovie_get_PlayCount(This,pPlayCount)	\
    (This)->lpVtbl -> get_PlayCount(This,pPlayCount)

#define IActiveMovie_put_PlayCount(This,PlayCount)	\
    (This)->lpVtbl -> put_PlayCount(This,PlayCount)

#define IActiveMovie_get_SelectionStart(This,pValue)	\
    (This)->lpVtbl -> get_SelectionStart(This,pValue)

#define IActiveMovie_put_SelectionStart(This,Value)	\
    (This)->lpVtbl -> put_SelectionStart(This,Value)

#define IActiveMovie_get_SelectionEnd(This,pValue)	\
    (This)->lpVtbl -> get_SelectionEnd(This,pValue)

#define IActiveMovie_put_SelectionEnd(This,Value)	\
    (This)->lpVtbl -> put_SelectionEnd(This,Value)

#define IActiveMovie_get_CurrentState(This,pState)	\
    (This)->lpVtbl -> get_CurrentState(This,pState)

#define IActiveMovie_get_Rate(This,pValue)	\
    (This)->lpVtbl -> get_Rate(This,pValue)

#define IActiveMovie_put_Rate(This,Value)	\
    (This)->lpVtbl -> put_Rate(This,Value)

#define IActiveMovie_get_Volume(This,pValue)	\
    (This)->lpVtbl -> get_Volume(This,pValue)

#define IActiveMovie_put_Volume(This,Value)	\
    (This)->lpVtbl -> put_Volume(This,Value)

#define IActiveMovie_get_Balance(This,pValue)	\
    (This)->lpVtbl -> get_Balance(This,pValue)

#define IActiveMovie_put_Balance(This,Value)	\
    (This)->lpVtbl -> put_Balance(This,Value)

#define IActiveMovie_get_EnableContextMenu(This,pEnable)	\
    (This)->lpVtbl -> get_EnableContextMenu(This,pEnable)

#define IActiveMovie_put_EnableContextMenu(This,Enable)	\
    (This)->lpVtbl -> put_EnableContextMenu(This,Enable)

#define IActiveMovie_get_ShowDisplay(This,Show)	\
    (This)->lpVtbl -> get_ShowDisplay(This,Show)

#define IActiveMovie_put_ShowDisplay(This,Show)	\
    (This)->lpVtbl -> put_ShowDisplay(This,Show)

#define IActiveMovie_get_ShowControls(This,Show)	\
    (This)->lpVtbl -> get_ShowControls(This,Show)

#define IActiveMovie_put_ShowControls(This,Show)	\
    (This)->lpVtbl -> put_ShowControls(This,Show)

#define IActiveMovie_get_ShowPositionControls(This,Show)	\
    (This)->lpVtbl -> get_ShowPositionControls(This,Show)

#define IActiveMovie_put_ShowPositionControls(This,Show)	\
    (This)->lpVtbl -> put_ShowPositionControls(This,Show)

#define IActiveMovie_get_ShowSelectionControls(This,Show)	\
    (This)->lpVtbl -> get_ShowSelectionControls(This,Show)

#define IActiveMovie_put_ShowSelectionControls(This,Show)	\
    (This)->lpVtbl -> put_ShowSelectionControls(This,Show)

#define IActiveMovie_get_ShowTracker(This,Show)	\
    (This)->lpVtbl -> get_ShowTracker(This,Show)

#define IActiveMovie_put_ShowTracker(This,Show)	\
    (This)->lpVtbl -> put_ShowTracker(This,Show)

#define IActiveMovie_get_EnablePositionControls(This,Enable)	\
    (This)->lpVtbl -> get_EnablePositionControls(This,Enable)

#define IActiveMovie_put_EnablePositionControls(This,Enable)	\
    (This)->lpVtbl -> put_EnablePositionControls(This,Enable)

#define IActiveMovie_get_EnableSelectionControls(This,Enable)	\
    (This)->lpVtbl -> get_EnableSelectionControls(This,Enable)

#define IActiveMovie_put_EnableSelectionControls(This,Enable)	\
    (This)->lpVtbl -> put_EnableSelectionControls(This,Enable)

#define IActiveMovie_get_EnableTracker(This,Enable)	\
    (This)->lpVtbl -> get_EnableTracker(This,Enable)

#define IActiveMovie_put_EnableTracker(This,Enable)	\
    (This)->lpVtbl -> put_EnableTracker(This,Enable)

#define IActiveMovie_get_AllowHideDisplay(This,Show)	\
    (This)->lpVtbl -> get_AllowHideDisplay(This,Show)

#define IActiveMovie_put_AllowHideDisplay(This,Show)	\
    (This)->lpVtbl -> put_AllowHideDisplay(This,Show)

#define IActiveMovie_get_AllowHideControls(This,Show)	\
    (This)->lpVtbl -> get_AllowHideControls(This,Show)

#define IActiveMovie_put_AllowHideControls(This,Show)	\
    (This)->lpVtbl -> put_AllowHideControls(This,Show)

#define IActiveMovie_get_DisplayMode(This,pValue)	\
    (This)->lpVtbl -> get_DisplayMode(This,pValue)

#define IActiveMovie_put_DisplayMode(This,Value)	\
    (This)->lpVtbl -> put_DisplayMode(This,Value)

#define IActiveMovie_get_AllowChangeDisplayMode(This,fAllow)	\
    (This)->lpVtbl -> get_AllowChangeDisplayMode(This,fAllow)

#define IActiveMovie_put_AllowChangeDisplayMode(This,Allow)	\
    (This)->lpVtbl -> put_AllowChangeDisplayMode(This,Allow)

#define IActiveMovie_get_FilterGraph(This,ppFilterGraph)	\
    (This)->lpVtbl -> get_FilterGraph(This,ppFilterGraph)

#define IActiveMovie_put_FilterGraph(This,pFilterGraph)	\
    (This)->lpVtbl -> put_FilterGraph(This,pFilterGraph)

#define IActiveMovie_get_FilterGraphDispatch(This,pDispatch)	\
    (This)->lpVtbl -> get_FilterGraphDispatch(This,pDispatch)

#define IActiveMovie_get_DisplayForeColor(This,ForeColor)	\
    (This)->lpVtbl -> get_DisplayForeColor(This,ForeColor)

#define IActiveMovie_put_DisplayForeColor(This,ForeColor)	\
    (This)->lpVtbl -> put_DisplayForeColor(This,ForeColor)

#define IActiveMovie_get_DisplayBackColor(This,BackColor)	\
    (This)->lpVtbl -> get_DisplayBackColor(This,BackColor)

#define IActiveMovie_put_DisplayBackColor(This,BackColor)	\
    (This)->lpVtbl -> put_DisplayBackColor(This,BackColor)

#define IActiveMovie_get_MovieWindowSize(This,WindowSize)	\
    (This)->lpVtbl -> get_MovieWindowSize(This,WindowSize)

#define IActiveMovie_put_MovieWindowSize(This,WindowSize)	\
    (This)->lpVtbl -> put_MovieWindowSize(This,WindowSize)

#define IActiveMovie_get_FullScreenMode(This,pEnable)	\
    (This)->lpVtbl -> get_FullScreenMode(This,pEnable)

#define IActiveMovie_put_FullScreenMode(This,Enable)	\
    (This)->lpVtbl -> put_FullScreenMode(This,Enable)

#define IActiveMovie_get_AutoStart(This,pEnable)	\
    (This)->lpVtbl -> get_AutoStart(This,pEnable)

#define IActiveMovie_put_AutoStart(This,Enable)	\
    (This)->lpVtbl -> put_AutoStart(This,Enable)

#define IActiveMovie_get_AutoRewind(This,pEnable)	\
    (This)->lpVtbl -> get_AutoRewind(This,pEnable)

#define IActiveMovie_put_AutoRewind(This,Enable)	\
    (This)->lpVtbl -> put_AutoRewind(This,Enable)

#define IActiveMovie_get_hWnd(This,hWnd)	\
    (This)->lpVtbl -> get_hWnd(This,hWnd)

#define IActiveMovie_get_Appearance(This,pAppearance)	\
    (This)->lpVtbl -> get_Appearance(This,pAppearance)

#define IActiveMovie_put_Appearance(This,Appearance)	\
    (This)->lpVtbl -> put_Appearance(This,Appearance)

#define IActiveMovie_get_BorderStyle(This,pBorderStyle)	\
    (This)->lpVtbl -> get_BorderStyle(This,pBorderStyle)

#define IActiveMovie_put_BorderStyle(This,BorderStyle)	\
    (This)->lpVtbl -> put_BorderStyle(This,BorderStyle)

#define IActiveMovie_get_Enabled(This,pEnabled)	\
    (This)->lpVtbl -> get_Enabled(This,pEnabled)

#define IActiveMovie_put_Enabled(This,Enabled)	\
    (This)->lpVtbl -> put_Enabled(This,Enabled)

#define IActiveMovie_get_Info(This,ppInfo)	\
    (This)->lpVtbl -> get_Info(This,ppInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_AboutBox_Proxy( 
    IActiveMovie __RPC_FAR * This);


void __RPC_STUB IActiveMovie_AboutBox_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_Run_Proxy( 
    IActiveMovie __RPC_FAR * This);


void __RPC_STUB IActiveMovie_Run_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_Pause_Proxy( 
    IActiveMovie __RPC_FAR * This);


void __RPC_STUB IActiveMovie_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_Stop_Proxy( 
    IActiveMovie __RPC_FAR * This);


void __RPC_STUB IActiveMovie_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_ImageSourceWidth_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pWidth);


void __RPC_STUB IActiveMovie_get_ImageSourceWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_ImageSourceHeight_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IActiveMovie_get_ImageSourceHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Author_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor);


void __RPC_STUB IActiveMovie_get_Author_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Title_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);


void __RPC_STUB IActiveMovie_get_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Copyright_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);


void __RPC_STUB IActiveMovie_get_Copyright_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Description_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);


void __RPC_STUB IActiveMovie_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Rating_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);


void __RPC_STUB IActiveMovie_get_Rating_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_FileName_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);


void __RPC_STUB IActiveMovie_get_FileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_FileName_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ BSTR bstrFileName);


void __RPC_STUB IActiveMovie_put_FileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Duration_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pValue);


void __RPC_STUB IActiveMovie_get_Duration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_CurrentPosition_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pValue);


void __RPC_STUB IActiveMovie_get_CurrentPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_CurrentPosition_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ double Value);


void __RPC_STUB IActiveMovie_put_CurrentPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_PlayCount_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pPlayCount);


void __RPC_STUB IActiveMovie_get_PlayCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_PlayCount_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ long PlayCount);


void __RPC_STUB IActiveMovie_put_PlayCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_SelectionStart_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pValue);


void __RPC_STUB IActiveMovie_get_SelectionStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_SelectionStart_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ double Value);


void __RPC_STUB IActiveMovie_put_SelectionStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_SelectionEnd_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pValue);


void __RPC_STUB IActiveMovie_get_SelectionEnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_SelectionEnd_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ double Value);


void __RPC_STUB IActiveMovie_put_SelectionEnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_CurrentState_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ StateConstants __RPC_FAR *pState);


void __RPC_STUB IActiveMovie_get_CurrentState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Rate_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pValue);


void __RPC_STUB IActiveMovie_get_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_Rate_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ double Value);


void __RPC_STUB IActiveMovie_put_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Volume_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pValue);


void __RPC_STUB IActiveMovie_get_Volume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_Volume_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ long Value);


void __RPC_STUB IActiveMovie_put_Volume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Balance_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pValue);


void __RPC_STUB IActiveMovie_get_Balance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_Balance_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ long Value);


void __RPC_STUB IActiveMovie_put_Balance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_EnableContextMenu_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);


void __RPC_STUB IActiveMovie_get_EnableContextMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_EnableContextMenu_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enable);


void __RPC_STUB IActiveMovie_put_EnableContextMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_ShowDisplay_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);


void __RPC_STUB IActiveMovie_get_ShowDisplay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_ShowDisplay_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Show);


void __RPC_STUB IActiveMovie_put_ShowDisplay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_ShowControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);


void __RPC_STUB IActiveMovie_get_ShowControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_ShowControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Show);


void __RPC_STUB IActiveMovie_put_ShowControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_ShowPositionControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);


void __RPC_STUB IActiveMovie_get_ShowPositionControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_ShowPositionControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Show);


void __RPC_STUB IActiveMovie_put_ShowPositionControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_ShowSelectionControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);


void __RPC_STUB IActiveMovie_get_ShowSelectionControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_ShowSelectionControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Show);


void __RPC_STUB IActiveMovie_put_ShowSelectionControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_ShowTracker_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);


void __RPC_STUB IActiveMovie_get_ShowTracker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_ShowTracker_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Show);


void __RPC_STUB IActiveMovie_put_ShowTracker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_EnablePositionControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);


void __RPC_STUB IActiveMovie_get_EnablePositionControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_EnablePositionControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enable);


void __RPC_STUB IActiveMovie_put_EnablePositionControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_EnableSelectionControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);


void __RPC_STUB IActiveMovie_get_EnableSelectionControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_EnableSelectionControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enable);


void __RPC_STUB IActiveMovie_put_EnableSelectionControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_EnableTracker_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);


void __RPC_STUB IActiveMovie_get_EnableTracker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_EnableTracker_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enable);


void __RPC_STUB IActiveMovie_put_EnableTracker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_AllowHideDisplay_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);


void __RPC_STUB IActiveMovie_get_AllowHideDisplay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_AllowHideDisplay_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Show);


void __RPC_STUB IActiveMovie_put_AllowHideDisplay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_AllowHideControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);


void __RPC_STUB IActiveMovie_get_AllowHideControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_AllowHideControls_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Show);


void __RPC_STUB IActiveMovie_put_AllowHideControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_DisplayMode_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ DisplayModeConstants __RPC_FAR *pValue);


void __RPC_STUB IActiveMovie_get_DisplayMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_DisplayMode_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ DisplayModeConstants Value);


void __RPC_STUB IActiveMovie_put_DisplayMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_AllowChangeDisplayMode_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *fAllow);


void __RPC_STUB IActiveMovie_get_AllowChangeDisplayMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_AllowChangeDisplayMode_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Allow);


void __RPC_STUB IActiveMovie_put_AllowChangeDisplayMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_FilterGraph_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppFilterGraph);


void __RPC_STUB IActiveMovie_get_FilterGraph_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_FilterGraph_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pFilterGraph);


void __RPC_STUB IActiveMovie_put_FilterGraph_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_FilterGraphDispatch_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pDispatch);


void __RPC_STUB IActiveMovie_get_FilterGraphDispatch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_DisplayForeColor_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ OLE_COLOR __RPC_FAR *ForeColor);


void __RPC_STUB IActiveMovie_get_DisplayForeColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_DisplayForeColor_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ OLE_COLOR ForeColor);


void __RPC_STUB IActiveMovie_put_DisplayForeColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_DisplayBackColor_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ OLE_COLOR __RPC_FAR *BackColor);


void __RPC_STUB IActiveMovie_get_DisplayBackColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_DisplayBackColor_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ OLE_COLOR BackColor);


void __RPC_STUB IActiveMovie_put_DisplayBackColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_MovieWindowSize_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ WindowSizeConstants __RPC_FAR *WindowSize);


void __RPC_STUB IActiveMovie_get_MovieWindowSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_MovieWindowSize_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ WindowSizeConstants WindowSize);


void __RPC_STUB IActiveMovie_put_MovieWindowSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_FullScreenMode_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);


void __RPC_STUB IActiveMovie_get_FullScreenMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_FullScreenMode_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enable);


void __RPC_STUB IActiveMovie_put_FullScreenMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_AutoStart_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);


void __RPC_STUB IActiveMovie_get_AutoStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_AutoStart_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enable);


void __RPC_STUB IActiveMovie_put_AutoStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_AutoRewind_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);


void __RPC_STUB IActiveMovie_get_AutoRewind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_AutoRewind_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enable);


void __RPC_STUB IActiveMovie_put_AutoRewind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_hWnd_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *hWnd);


void __RPC_STUB IActiveMovie_get_hWnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Appearance_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ AppearanceConstants __RPC_FAR *pAppearance);


void __RPC_STUB IActiveMovie_get_Appearance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_Appearance_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ AppearanceConstants Appearance);


void __RPC_STUB IActiveMovie_put_Appearance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_BorderStyle_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ BorderStyleConstants __RPC_FAR *pBorderStyle);


void __RPC_STUB IActiveMovie_get_BorderStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_BorderStyle_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ BorderStyleConstants BorderStyle);


void __RPC_STUB IActiveMovie_put_BorderStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Enabled_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnabled);


void __RPC_STUB IActiveMovie_get_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_put_Enabled_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enabled);


void __RPC_STUB IActiveMovie_put_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie_get_Info_Proxy( 
    IActiveMovie __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *ppInfo);


void __RPC_STUB IActiveMovie_get_Info_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveMovie_INTERFACE_DEFINED__ */


#ifndef __IActiveMovie2_INTERFACE_DEFINED__
#define __IActiveMovie2_INTERFACE_DEFINED__

/* interface IActiveMovie2 */
/* [unique][hidden][dual][uuid][object] */ 


EXTERN_C const IID IID_IActiveMovie2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B6CD6554-E9CB-11d0-821F-00A0C91F9CA0")
    IActiveMovie2 : public IActiveMovie
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsSoundCardEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbSoundCard) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ReadyState( 
            /* [retval][out] */ ReadyStateConstants __RPC_FAR *pValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveMovie2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IActiveMovie2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IActiveMovie2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AboutBox )( 
            IActiveMovie2 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Run )( 
            IActiveMovie2 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IActiveMovie2 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IActiveMovie2 __RPC_FAR * This);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceWidth )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pWidth);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceHeight )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHeight);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Author )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Copyright )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileName )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FileName )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPosition )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentPosition )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayCount )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pPlayCount);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayCount )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ long PlayCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectionStart )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelectionStart )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectionEnd )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelectionEnd )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentState )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ StateConstants __RPC_FAR *pState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rate )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rate )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Volume )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Volume )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ long Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Balance )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Balance )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ long Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableContextMenu )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableContextMenu )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowDisplay )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowDisplay )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowPositionControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowPositionControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowSelectionControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowSelectionControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowTracker )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowTracker )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnablePositionControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnablePositionControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableSelectionControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableSelectionControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableTracker )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableTracker )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowHideDisplay )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowHideDisplay )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowHideControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowHideControls )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayMode )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ DisplayModeConstants __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayMode )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ DisplayModeConstants Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowChangeDisplayMode )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *fAllow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowChangeDisplayMode )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterGraph )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppFilterGraph);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FilterGraph )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pFilterGraph);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterGraphDispatch )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pDispatch);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayForeColor )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *ForeColor);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayForeColor )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ OLE_COLOR ForeColor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayBackColor )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *BackColor);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayBackColor )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ OLE_COLOR BackColor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MovieWindowSize )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ WindowSizeConstants __RPC_FAR *WindowSize);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MovieWindowSize )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ WindowSizeConstants WindowSize);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullScreenMode )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FullScreenMode )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoStart )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoStart )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoRewind )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoRewind )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_hWnd )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *hWnd);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Appearance )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ AppearanceConstants __RPC_FAR *pAppearance);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Appearance )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ AppearanceConstants Appearance);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BorderStyle )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ BorderStyleConstants __RPC_FAR *pBorderStyle);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BorderStyle )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ BorderStyleConstants BorderStyle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enabled);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Info )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *ppInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsSoundCardEnabled )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbSoundCard);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReadyState )( 
            IActiveMovie2 __RPC_FAR * This,
            /* [retval][out] */ ReadyStateConstants __RPC_FAR *pValue);
        
        END_INTERFACE
    } IActiveMovie2Vtbl;

    interface IActiveMovie2
    {
        CONST_VTBL struct IActiveMovie2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveMovie2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveMovie2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveMovie2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveMovie2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IActiveMovie2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IActiveMovie2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IActiveMovie2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IActiveMovie2_AboutBox(This)	\
    (This)->lpVtbl -> AboutBox(This)

#define IActiveMovie2_Run(This)	\
    (This)->lpVtbl -> Run(This)

#define IActiveMovie2_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IActiveMovie2_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IActiveMovie2_get_ImageSourceWidth(This,pWidth)	\
    (This)->lpVtbl -> get_ImageSourceWidth(This,pWidth)

#define IActiveMovie2_get_ImageSourceHeight(This,pHeight)	\
    (This)->lpVtbl -> get_ImageSourceHeight(This,pHeight)

#define IActiveMovie2_get_Author(This,pbstrAuthor)	\
    (This)->lpVtbl -> get_Author(This,pbstrAuthor)

#define IActiveMovie2_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define IActiveMovie2_get_Copyright(This,pbstrCopyright)	\
    (This)->lpVtbl -> get_Copyright(This,pbstrCopyright)

#define IActiveMovie2_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define IActiveMovie2_get_Rating(This,pbstrRating)	\
    (This)->lpVtbl -> get_Rating(This,pbstrRating)

#define IActiveMovie2_get_FileName(This,pbstrFileName)	\
    (This)->lpVtbl -> get_FileName(This,pbstrFileName)

#define IActiveMovie2_put_FileName(This,bstrFileName)	\
    (This)->lpVtbl -> put_FileName(This,bstrFileName)

#define IActiveMovie2_get_Duration(This,pValue)	\
    (This)->lpVtbl -> get_Duration(This,pValue)

#define IActiveMovie2_get_CurrentPosition(This,pValue)	\
    (This)->lpVtbl -> get_CurrentPosition(This,pValue)

#define IActiveMovie2_put_CurrentPosition(This,Value)	\
    (This)->lpVtbl -> put_CurrentPosition(This,Value)

#define IActiveMovie2_get_PlayCount(This,pPlayCount)	\
    (This)->lpVtbl -> get_PlayCount(This,pPlayCount)

#define IActiveMovie2_put_PlayCount(This,PlayCount)	\
    (This)->lpVtbl -> put_PlayCount(This,PlayCount)

#define IActiveMovie2_get_SelectionStart(This,pValue)	\
    (This)->lpVtbl -> get_SelectionStart(This,pValue)

#define IActiveMovie2_put_SelectionStart(This,Value)	\
    (This)->lpVtbl -> put_SelectionStart(This,Value)

#define IActiveMovie2_get_SelectionEnd(This,pValue)	\
    (This)->lpVtbl -> get_SelectionEnd(This,pValue)

#define IActiveMovie2_put_SelectionEnd(This,Value)	\
    (This)->lpVtbl -> put_SelectionEnd(This,Value)

#define IActiveMovie2_get_CurrentState(This,pState)	\
    (This)->lpVtbl -> get_CurrentState(This,pState)

#define IActiveMovie2_get_Rate(This,pValue)	\
    (This)->lpVtbl -> get_Rate(This,pValue)

#define IActiveMovie2_put_Rate(This,Value)	\
    (This)->lpVtbl -> put_Rate(This,Value)

#define IActiveMovie2_get_Volume(This,pValue)	\
    (This)->lpVtbl -> get_Volume(This,pValue)

#define IActiveMovie2_put_Volume(This,Value)	\
    (This)->lpVtbl -> put_Volume(This,Value)

#define IActiveMovie2_get_Balance(This,pValue)	\
    (This)->lpVtbl -> get_Balance(This,pValue)

#define IActiveMovie2_put_Balance(This,Value)	\
    (This)->lpVtbl -> put_Balance(This,Value)

#define IActiveMovie2_get_EnableContextMenu(This,pEnable)	\
    (This)->lpVtbl -> get_EnableContextMenu(This,pEnable)

#define IActiveMovie2_put_EnableContextMenu(This,Enable)	\
    (This)->lpVtbl -> put_EnableContextMenu(This,Enable)

#define IActiveMovie2_get_ShowDisplay(This,Show)	\
    (This)->lpVtbl -> get_ShowDisplay(This,Show)

#define IActiveMovie2_put_ShowDisplay(This,Show)	\
    (This)->lpVtbl -> put_ShowDisplay(This,Show)

#define IActiveMovie2_get_ShowControls(This,Show)	\
    (This)->lpVtbl -> get_ShowControls(This,Show)

#define IActiveMovie2_put_ShowControls(This,Show)	\
    (This)->lpVtbl -> put_ShowControls(This,Show)

#define IActiveMovie2_get_ShowPositionControls(This,Show)	\
    (This)->lpVtbl -> get_ShowPositionControls(This,Show)

#define IActiveMovie2_put_ShowPositionControls(This,Show)	\
    (This)->lpVtbl -> put_ShowPositionControls(This,Show)

#define IActiveMovie2_get_ShowSelectionControls(This,Show)	\
    (This)->lpVtbl -> get_ShowSelectionControls(This,Show)

#define IActiveMovie2_put_ShowSelectionControls(This,Show)	\
    (This)->lpVtbl -> put_ShowSelectionControls(This,Show)

#define IActiveMovie2_get_ShowTracker(This,Show)	\
    (This)->lpVtbl -> get_ShowTracker(This,Show)

#define IActiveMovie2_put_ShowTracker(This,Show)	\
    (This)->lpVtbl -> put_ShowTracker(This,Show)

#define IActiveMovie2_get_EnablePositionControls(This,Enable)	\
    (This)->lpVtbl -> get_EnablePositionControls(This,Enable)

#define IActiveMovie2_put_EnablePositionControls(This,Enable)	\
    (This)->lpVtbl -> put_EnablePositionControls(This,Enable)

#define IActiveMovie2_get_EnableSelectionControls(This,Enable)	\
    (This)->lpVtbl -> get_EnableSelectionControls(This,Enable)

#define IActiveMovie2_put_EnableSelectionControls(This,Enable)	\
    (This)->lpVtbl -> put_EnableSelectionControls(This,Enable)

#define IActiveMovie2_get_EnableTracker(This,Enable)	\
    (This)->lpVtbl -> get_EnableTracker(This,Enable)

#define IActiveMovie2_put_EnableTracker(This,Enable)	\
    (This)->lpVtbl -> put_EnableTracker(This,Enable)

#define IActiveMovie2_get_AllowHideDisplay(This,Show)	\
    (This)->lpVtbl -> get_AllowHideDisplay(This,Show)

#define IActiveMovie2_put_AllowHideDisplay(This,Show)	\
    (This)->lpVtbl -> put_AllowHideDisplay(This,Show)

#define IActiveMovie2_get_AllowHideControls(This,Show)	\
    (This)->lpVtbl -> get_AllowHideControls(This,Show)

#define IActiveMovie2_put_AllowHideControls(This,Show)	\
    (This)->lpVtbl -> put_AllowHideControls(This,Show)

#define IActiveMovie2_get_DisplayMode(This,pValue)	\
    (This)->lpVtbl -> get_DisplayMode(This,pValue)

#define IActiveMovie2_put_DisplayMode(This,Value)	\
    (This)->lpVtbl -> put_DisplayMode(This,Value)

#define IActiveMovie2_get_AllowChangeDisplayMode(This,fAllow)	\
    (This)->lpVtbl -> get_AllowChangeDisplayMode(This,fAllow)

#define IActiveMovie2_put_AllowChangeDisplayMode(This,Allow)	\
    (This)->lpVtbl -> put_AllowChangeDisplayMode(This,Allow)

#define IActiveMovie2_get_FilterGraph(This,ppFilterGraph)	\
    (This)->lpVtbl -> get_FilterGraph(This,ppFilterGraph)

#define IActiveMovie2_put_FilterGraph(This,pFilterGraph)	\
    (This)->lpVtbl -> put_FilterGraph(This,pFilterGraph)

#define IActiveMovie2_get_FilterGraphDispatch(This,pDispatch)	\
    (This)->lpVtbl -> get_FilterGraphDispatch(This,pDispatch)

#define IActiveMovie2_get_DisplayForeColor(This,ForeColor)	\
    (This)->lpVtbl -> get_DisplayForeColor(This,ForeColor)

#define IActiveMovie2_put_DisplayForeColor(This,ForeColor)	\
    (This)->lpVtbl -> put_DisplayForeColor(This,ForeColor)

#define IActiveMovie2_get_DisplayBackColor(This,BackColor)	\
    (This)->lpVtbl -> get_DisplayBackColor(This,BackColor)

#define IActiveMovie2_put_DisplayBackColor(This,BackColor)	\
    (This)->lpVtbl -> put_DisplayBackColor(This,BackColor)

#define IActiveMovie2_get_MovieWindowSize(This,WindowSize)	\
    (This)->lpVtbl -> get_MovieWindowSize(This,WindowSize)

#define IActiveMovie2_put_MovieWindowSize(This,WindowSize)	\
    (This)->lpVtbl -> put_MovieWindowSize(This,WindowSize)

#define IActiveMovie2_get_FullScreenMode(This,pEnable)	\
    (This)->lpVtbl -> get_FullScreenMode(This,pEnable)

#define IActiveMovie2_put_FullScreenMode(This,Enable)	\
    (This)->lpVtbl -> put_FullScreenMode(This,Enable)

#define IActiveMovie2_get_AutoStart(This,pEnable)	\
    (This)->lpVtbl -> get_AutoStart(This,pEnable)

#define IActiveMovie2_put_AutoStart(This,Enable)	\
    (This)->lpVtbl -> put_AutoStart(This,Enable)

#define IActiveMovie2_get_AutoRewind(This,pEnable)	\
    (This)->lpVtbl -> get_AutoRewind(This,pEnable)

#define IActiveMovie2_put_AutoRewind(This,Enable)	\
    (This)->lpVtbl -> put_AutoRewind(This,Enable)

#define IActiveMovie2_get_hWnd(This,hWnd)	\
    (This)->lpVtbl -> get_hWnd(This,hWnd)

#define IActiveMovie2_get_Appearance(This,pAppearance)	\
    (This)->lpVtbl -> get_Appearance(This,pAppearance)

#define IActiveMovie2_put_Appearance(This,Appearance)	\
    (This)->lpVtbl -> put_Appearance(This,Appearance)

#define IActiveMovie2_get_BorderStyle(This,pBorderStyle)	\
    (This)->lpVtbl -> get_BorderStyle(This,pBorderStyle)

#define IActiveMovie2_put_BorderStyle(This,BorderStyle)	\
    (This)->lpVtbl -> put_BorderStyle(This,BorderStyle)

#define IActiveMovie2_get_Enabled(This,pEnabled)	\
    (This)->lpVtbl -> get_Enabled(This,pEnabled)

#define IActiveMovie2_put_Enabled(This,Enabled)	\
    (This)->lpVtbl -> put_Enabled(This,Enabled)

#define IActiveMovie2_get_Info(This,ppInfo)	\
    (This)->lpVtbl -> get_Info(This,ppInfo)


#define IActiveMovie2_IsSoundCardEnabled(This,pbSoundCard)	\
    (This)->lpVtbl -> IsSoundCardEnabled(This,pbSoundCard)

#define IActiveMovie2_get_ReadyState(This,pValue)	\
    (This)->lpVtbl -> get_ReadyState(This,pValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IActiveMovie2_IsSoundCardEnabled_Proxy( 
    IActiveMovie2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbSoundCard);


void __RPC_STUB IActiveMovie2_IsSoundCardEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IActiveMovie2_get_ReadyState_Proxy( 
    IActiveMovie2 __RPC_FAR * This,
    /* [retval][out] */ ReadyStateConstants __RPC_FAR *pValue);


void __RPC_STUB IActiveMovie2_get_ReadyState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveMovie2_INTERFACE_DEFINED__ */


#ifndef __IActiveMovie3_INTERFACE_DEFINED__
#define __IActiveMovie3_INTERFACE_DEFINED__

/* interface IActiveMovie3 */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IActiveMovie3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("265EC140-AE62-11d1-8500-00A0C91F9CA0")
    IActiveMovie3 : public IActiveMovie2
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_MediaPlayer( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveMovie3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IActiveMovie3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IActiveMovie3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AboutBox )( 
            IActiveMovie3 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Run )( 
            IActiveMovie3 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IActiveMovie3 __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IActiveMovie3 __RPC_FAR * This);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceWidth )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pWidth);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageSourceHeight )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHeight);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Author )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Copyright )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileName )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FileName )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPosition )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentPosition )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayCount )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pPlayCount);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayCount )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ long PlayCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectionStart )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelectionStart )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectionEnd )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelectionEnd )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentState )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ StateConstants __RPC_FAR *pState);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rate )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rate )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ double Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Volume )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Volume )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ long Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Balance )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Balance )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ long Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableContextMenu )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableContextMenu )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowDisplay )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowDisplay )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowPositionControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowPositionControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowSelectionControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowSelectionControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowTracker )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowTracker )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnablePositionControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnablePositionControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableSelectionControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableSelectionControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableTracker )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableTracker )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowHideDisplay )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowHideDisplay )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowHideControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Show);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowHideControls )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Show);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayMode )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ DisplayModeConstants __RPC_FAR *pValue);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayMode )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ DisplayModeConstants Value);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowChangeDisplayMode )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *fAllow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowChangeDisplayMode )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterGraph )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppFilterGraph);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FilterGraph )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pFilterGraph);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterGraphDispatch )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pDispatch);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayForeColor )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *ForeColor);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayForeColor )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ OLE_COLOR ForeColor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayBackColor )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *BackColor);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayBackColor )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ OLE_COLOR BackColor);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MovieWindowSize )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ WindowSizeConstants __RPC_FAR *WindowSize);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MovieWindowSize )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ WindowSizeConstants WindowSize);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullScreenMode )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FullScreenMode )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoStart )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoStart )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoRewind )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnable);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoRewind )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_hWnd )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *hWnd);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Appearance )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ AppearanceConstants __RPC_FAR *pAppearance);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Appearance )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ AppearanceConstants Appearance);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BorderStyle )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ BorderStyleConstants __RPC_FAR *pBorderStyle);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BorderStyle )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ BorderStyleConstants BorderStyle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enabled);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Info )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *ppInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsSoundCardEnabled )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbSoundCard);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReadyState )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ ReadyStateConstants __RPC_FAR *pValue);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaPlayer )( 
            IActiveMovie3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
        
        END_INTERFACE
    } IActiveMovie3Vtbl;

    interface IActiveMovie3
    {
        CONST_VTBL struct IActiveMovie3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveMovie3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveMovie3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveMovie3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveMovie3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IActiveMovie3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IActiveMovie3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IActiveMovie3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IActiveMovie3_AboutBox(This)	\
    (This)->lpVtbl -> AboutBox(This)

#define IActiveMovie3_Run(This)	\
    (This)->lpVtbl -> Run(This)

#define IActiveMovie3_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IActiveMovie3_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IActiveMovie3_get_ImageSourceWidth(This,pWidth)	\
    (This)->lpVtbl -> get_ImageSourceWidth(This,pWidth)

#define IActiveMovie3_get_ImageSourceHeight(This,pHeight)	\
    (This)->lpVtbl -> get_ImageSourceHeight(This,pHeight)

#define IActiveMovie3_get_Author(This,pbstrAuthor)	\
    (This)->lpVtbl -> get_Author(This,pbstrAuthor)

#define IActiveMovie3_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define IActiveMovie3_get_Copyright(This,pbstrCopyright)	\
    (This)->lpVtbl -> get_Copyright(This,pbstrCopyright)

#define IActiveMovie3_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define IActiveMovie3_get_Rating(This,pbstrRating)	\
    (This)->lpVtbl -> get_Rating(This,pbstrRating)

#define IActiveMovie3_get_FileName(This,pbstrFileName)	\
    (This)->lpVtbl -> get_FileName(This,pbstrFileName)

#define IActiveMovie3_put_FileName(This,bstrFileName)	\
    (This)->lpVtbl -> put_FileName(This,bstrFileName)

#define IActiveMovie3_get_Duration(This,pValue)	\
    (This)->lpVtbl -> get_Duration(This,pValue)

#define IActiveMovie3_get_CurrentPosition(This,pValue)	\
    (This)->lpVtbl -> get_CurrentPosition(This,pValue)

#define IActiveMovie3_put_CurrentPosition(This,Value)	\
    (This)->lpVtbl -> put_CurrentPosition(This,Value)

#define IActiveMovie3_get_PlayCount(This,pPlayCount)	\
    (This)->lpVtbl -> get_PlayCount(This,pPlayCount)

#define IActiveMovie3_put_PlayCount(This,PlayCount)	\
    (This)->lpVtbl -> put_PlayCount(This,PlayCount)

#define IActiveMovie3_get_SelectionStart(This,pValue)	\
    (This)->lpVtbl -> get_SelectionStart(This,pValue)

#define IActiveMovie3_put_SelectionStart(This,Value)	\
    (This)->lpVtbl -> put_SelectionStart(This,Value)

#define IActiveMovie3_get_SelectionEnd(This,pValue)	\
    (This)->lpVtbl -> get_SelectionEnd(This,pValue)

#define IActiveMovie3_put_SelectionEnd(This,Value)	\
    (This)->lpVtbl -> put_SelectionEnd(This,Value)

#define IActiveMovie3_get_CurrentState(This,pState)	\
    (This)->lpVtbl -> get_CurrentState(This,pState)

#define IActiveMovie3_get_Rate(This,pValue)	\
    (This)->lpVtbl -> get_Rate(This,pValue)

#define IActiveMovie3_put_Rate(This,Value)	\
    (This)->lpVtbl -> put_Rate(This,Value)

#define IActiveMovie3_get_Volume(This,pValue)	\
    (This)->lpVtbl -> get_Volume(This,pValue)

#define IActiveMovie3_put_Volume(This,Value)	\
    (This)->lpVtbl -> put_Volume(This,Value)

#define IActiveMovie3_get_Balance(This,pValue)	\
    (This)->lpVtbl -> get_Balance(This,pValue)

#define IActiveMovie3_put_Balance(This,Value)	\
    (This)->lpVtbl -> put_Balance(This,Value)

#define IActiveMovie3_get_EnableContextMenu(This,pEnable)	\
    (This)->lpVtbl -> get_EnableContextMenu(This,pEnable)

#define IActiveMovie3_put_EnableContextMenu(This,Enable)	\
    (This)->lpVtbl -> put_EnableContextMenu(This,Enable)

#define IActiveMovie3_get_ShowDisplay(This,Show)	\
    (This)->lpVtbl -> get_ShowDisplay(This,Show)

#define IActiveMovie3_put_ShowDisplay(This,Show)	\
    (This)->lpVtbl -> put_ShowDisplay(This,Show)

#define IActiveMovie3_get_ShowControls(This,Show)	\
    (This)->lpVtbl -> get_ShowControls(This,Show)

#define IActiveMovie3_put_ShowControls(This,Show)	\
    (This)->lpVtbl -> put_ShowControls(This,Show)

#define IActiveMovie3_get_ShowPositionControls(This,Show)	\
    (This)->lpVtbl -> get_ShowPositionControls(This,Show)

#define IActiveMovie3_put_ShowPositionControls(This,Show)	\
    (This)->lpVtbl -> put_ShowPositionControls(This,Show)

#define IActiveMovie3_get_ShowSelectionControls(This,Show)	\
    (This)->lpVtbl -> get_ShowSelectionControls(This,Show)

#define IActiveMovie3_put_ShowSelectionControls(This,Show)	\
    (This)->lpVtbl -> put_ShowSelectionControls(This,Show)

#define IActiveMovie3_get_ShowTracker(This,Show)	\
    (This)->lpVtbl -> get_ShowTracker(This,Show)

#define IActiveMovie3_put_ShowTracker(This,Show)	\
    (This)->lpVtbl -> put_ShowTracker(This,Show)

#define IActiveMovie3_get_EnablePositionControls(This,Enable)	\
    (This)->lpVtbl -> get_EnablePositionControls(This,Enable)

#define IActiveMovie3_put_EnablePositionControls(This,Enable)	\
    (This)->lpVtbl -> put_EnablePositionControls(This,Enable)

#define IActiveMovie3_get_EnableSelectionControls(This,Enable)	\
    (This)->lpVtbl -> get_EnableSelectionControls(This,Enable)

#define IActiveMovie3_put_EnableSelectionControls(This,Enable)	\
    (This)->lpVtbl -> put_EnableSelectionControls(This,Enable)

#define IActiveMovie3_get_EnableTracker(This,Enable)	\
    (This)->lpVtbl -> get_EnableTracker(This,Enable)

#define IActiveMovie3_put_EnableTracker(This,Enable)	\
    (This)->lpVtbl -> put_EnableTracker(This,Enable)

#define IActiveMovie3_get_AllowHideDisplay(This,Show)	\
    (This)->lpVtbl -> get_AllowHideDisplay(This,Show)

#define IActiveMovie3_put_AllowHideDisplay(This,Show)	\
    (This)->lpVtbl -> put_AllowHideDisplay(This,Show)

#define IActiveMovie3_get_AllowHideControls(This,Show)	\
    (This)->lpVtbl -> get_AllowHideControls(This,Show)

#define IActiveMovie3_put_AllowHideControls(This,Show)	\
    (This)->lpVtbl -> put_AllowHideControls(This,Show)

#define IActiveMovie3_get_DisplayMode(This,pValue)	\
    (This)->lpVtbl -> get_DisplayMode(This,pValue)

#define IActiveMovie3_put_DisplayMode(This,Value)	\
    (This)->lpVtbl -> put_DisplayMode(This,Value)

#define IActiveMovie3_get_AllowChangeDisplayMode(This,fAllow)	\
    (This)->lpVtbl -> get_AllowChangeDisplayMode(This,fAllow)

#define IActiveMovie3_put_AllowChangeDisplayMode(This,Allow)	\
    (This)->lpVtbl -> put_AllowChangeDisplayMode(This,Allow)

#define IActiveMovie3_get_FilterGraph(This,ppFilterGraph)	\
    (This)->lpVtbl -> get_FilterGraph(This,ppFilterGraph)

#define IActiveMovie3_put_FilterGraph(This,pFilterGraph)	\
    (This)->lpVtbl -> put_FilterGraph(This,pFilterGraph)

#define IActiveMovie3_get_FilterGraphDispatch(This,pDispatch)	\
    (This)->lpVtbl -> get_FilterGraphDispatch(This,pDispatch)

#define IActiveMovie3_get_DisplayForeColor(This,ForeColor)	\
    (This)->lpVtbl -> get_DisplayForeColor(This,ForeColor)

#define IActiveMovie3_put_DisplayForeColor(This,ForeColor)	\
    (This)->lpVtbl -> put_DisplayForeColor(This,ForeColor)

#define IActiveMovie3_get_DisplayBackColor(This,BackColor)	\
    (This)->lpVtbl -> get_DisplayBackColor(This,BackColor)

#define IActiveMovie3_put_DisplayBackColor(This,BackColor)	\
    (This)->lpVtbl -> put_DisplayBackColor(This,BackColor)

#define IActiveMovie3_get_MovieWindowSize(This,WindowSize)	\
    (This)->lpVtbl -> get_MovieWindowSize(This,WindowSize)

#define IActiveMovie3_put_MovieWindowSize(This,WindowSize)	\
    (This)->lpVtbl -> put_MovieWindowSize(This,WindowSize)

#define IActiveMovie3_get_FullScreenMode(This,pEnable)	\
    (This)->lpVtbl -> get_FullScreenMode(This,pEnable)

#define IActiveMovie3_put_FullScreenMode(This,Enable)	\
    (This)->lpVtbl -> put_FullScreenMode(This,Enable)

#define IActiveMovie3_get_AutoStart(This,pEnable)	\
    (This)->lpVtbl -> get_AutoStart(This,pEnable)

#define IActiveMovie3_put_AutoStart(This,Enable)	\
    (This)->lpVtbl -> put_AutoStart(This,Enable)

#define IActiveMovie3_get_AutoRewind(This,pEnable)	\
    (This)->lpVtbl -> get_AutoRewind(This,pEnable)

#define IActiveMovie3_put_AutoRewind(This,Enable)	\
    (This)->lpVtbl -> put_AutoRewind(This,Enable)

#define IActiveMovie3_get_hWnd(This,hWnd)	\
    (This)->lpVtbl -> get_hWnd(This,hWnd)

#define IActiveMovie3_get_Appearance(This,pAppearance)	\
    (This)->lpVtbl -> get_Appearance(This,pAppearance)

#define IActiveMovie3_put_Appearance(This,Appearance)	\
    (This)->lpVtbl -> put_Appearance(This,Appearance)

#define IActiveMovie3_get_BorderStyle(This,pBorderStyle)	\
    (This)->lpVtbl -> get_BorderStyle(This,pBorderStyle)

#define IActiveMovie3_put_BorderStyle(This,BorderStyle)	\
    (This)->lpVtbl -> put_BorderStyle(This,BorderStyle)

#define IActiveMovie3_get_Enabled(This,pEnabled)	\
    (This)->lpVtbl -> get_Enabled(This,pEnabled)

#define IActiveMovie3_put_Enabled(This,Enabled)	\
    (This)->lpVtbl -> put_Enabled(This,Enabled)

#define IActiveMovie3_get_Info(This,ppInfo)	\
    (This)->lpVtbl -> get_Info(This,ppInfo)


#define IActiveMovie3_IsSoundCardEnabled(This,pbSoundCard)	\
    (This)->lpVtbl -> IsSoundCardEnabled(This,pbSoundCard)

#define IActiveMovie3_get_ReadyState(This,pValue)	\
    (This)->lpVtbl -> get_ReadyState(This,pValue)


#define IActiveMovie3_get_MediaPlayer(This,ppDispatch)	\
    (This)->lpVtbl -> get_MediaPlayer(This,ppDispatch)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IActiveMovie3_get_MediaPlayer_Proxy( 
    IActiveMovie3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);


void __RPC_STUB IActiveMovie3_get_MediaPlayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveMovie3_INTERFACE_DEFINED__ */


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


