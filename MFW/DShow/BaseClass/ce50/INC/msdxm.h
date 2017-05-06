
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Thu Jun 24 19:41:49 2004
 */
/* Compiler settings for .\msdxm.idl:
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

#ifndef __msdxm_h__
#define __msdxm_h__

/* Forward Declarations */ 

#ifndef ___IDirectControlEvents_FWD_DEFINED__
#define ___IDirectControlEvents_FWD_DEFINED__
typedef interface _IDirectControlEvents _IDirectControlEvents;
#endif 	/* ___IDirectControlEvents_FWD_DEFINED__ */


#ifndef __DirectControl_FWD_DEFINED__
#define __DirectControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class DirectControl DirectControl;
#else
typedef struct DirectControl DirectControl;
#endif /* __cplusplus */

#endif 	/* __DirectControl_FWD_DEFINED__ */


#ifndef ___IDirectContainerEvents_FWD_DEFINED__
#define ___IDirectContainerEvents_FWD_DEFINED__
typedef interface _IDirectContainerEvents _IDirectContainerEvents;
#endif 	/* ___IDirectContainerEvents_FWD_DEFINED__ */


#ifndef __DirectContainer_FWD_DEFINED__
#define __DirectContainer_FWD_DEFINED__

#ifdef __cplusplus
typedef class DirectContainer DirectContainer;
#else
typedef struct DirectContainer DirectContainer;
#endif /* __cplusplus */

#endif 	/* __DirectContainer_FWD_DEFINED__ */


#ifndef ___MediaPlayerEvents_FWD_DEFINED__
#define ___MediaPlayerEvents_FWD_DEFINED__
typedef interface _MediaPlayerEvents _MediaPlayerEvents;
#endif 	/* ___MediaPlayerEvents_FWD_DEFINED__ */


#ifndef __MediaPlayer_FWD_DEFINED__
#define __MediaPlayer_FWD_DEFINED__

#ifdef __cplusplus
typedef class MediaPlayer MediaPlayer;
#else
typedef struct MediaPlayer MediaPlayer;
#endif /* __cplusplus */

#endif 	/* __MediaPlayer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "dxmplay.h"
#include "nsplay.h"
#include "amovie.h"
#include "bmedia.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __MediaPlayer_LIBRARY_DEFINED__
#define __MediaPlayer_LIBRARY_DEFINED__

/* library MediaPlayer */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_MediaPlayer;

#ifndef ___IDirectControlEvents_DISPINTERFACE_DEFINED__
#define ___IDirectControlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IDirectControlEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__IDirectControlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("39A2C2A7-4778-11D2-9BDB-204C4F4F5020")
    _IDirectControlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IDirectControlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IDirectControlEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IDirectControlEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IDirectControlEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _IDirectControlEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _IDirectControlEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _IDirectControlEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _IDirectControlEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _IDirectControlEventsVtbl;

    interface _IDirectControlEvents
    {
        CONST_VTBL struct _IDirectControlEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IDirectControlEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IDirectControlEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IDirectControlEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IDirectControlEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IDirectControlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IDirectControlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IDirectControlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IDirectControlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_DirectControl;

#ifdef __cplusplus

class DECLSPEC_UUID("39A2C2A6-4778-11D2-9BDB-204C4F4F5020")
DirectControl;
#endif

#ifndef ___IDirectContainerEvents_DISPINTERFACE_DEFINED__
#define ___IDirectContainerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IDirectContainerEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__IDirectContainerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("39A2C2AA-4778-11D2-9BDB-204C4F4F5020")
    _IDirectContainerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IDirectContainerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IDirectContainerEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IDirectContainerEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IDirectContainerEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _IDirectContainerEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _IDirectContainerEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _IDirectContainerEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _IDirectContainerEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _IDirectContainerEventsVtbl;

    interface _IDirectContainerEvents
    {
        CONST_VTBL struct _IDirectContainerEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IDirectContainerEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IDirectContainerEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IDirectContainerEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IDirectContainerEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IDirectContainerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IDirectContainerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IDirectContainerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IDirectContainerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_DirectContainer;

#ifdef __cplusplus

class DECLSPEC_UUID("39A2C2A9-4778-11D2-9BDB-204C4F4F5020")
DirectContainer;
#endif

#ifndef ___MediaPlayerEvents_DISPINTERFACE_DEFINED__
#define ___MediaPlayerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _MediaPlayerEvents */
/* [hidden][uuid] */ 


EXTERN_C const IID DIID__MediaPlayerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("2D3A4C40-E711-11d0-94AB-0080C74C7E95")
    _MediaPlayerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _MediaPlayerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _MediaPlayerEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _MediaPlayerEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _MediaPlayerEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _MediaPlayerEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _MediaPlayerEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _MediaPlayerEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _MediaPlayerEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _MediaPlayerEventsVtbl;

    interface _MediaPlayerEvents
    {
        CONST_VTBL struct _MediaPlayerEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _MediaPlayerEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _MediaPlayerEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _MediaPlayerEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _MediaPlayerEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _MediaPlayerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _MediaPlayerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _MediaPlayerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___MediaPlayerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_MediaPlayer;

#ifdef __cplusplus

class DECLSPEC_UUID("22D6F312-B0F6-11D0-94AB-0080C74C7E95")
MediaPlayer;
#endif
#endif /* __MediaPlayer_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


