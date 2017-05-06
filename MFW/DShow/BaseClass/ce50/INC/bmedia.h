
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Thu Jun 24 19:41:54 2004
 */
/* Compiler settings for .\bmedia.idl:
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

#ifndef __bmedia_h__
#define __bmedia_h__

/* Forward Declarations */ 

#ifndef __IServiceProvider_FWD_DEFINED__
#define __IServiceProvider_FWD_DEFINED__
typedef interface IServiceProvider IServiceProvider;
#endif 	/* __IServiceProvider_FWD_DEFINED__ */


#ifndef __IAsyncPProt_FWD_DEFINED__
#define __IAsyncPProt_FWD_DEFINED__
typedef interface IAsyncPProt IAsyncPProt;
#endif 	/* __IAsyncPProt_FWD_DEFINED__ */


#ifndef __IAsyncMHandler_FWD_DEFINED__
#define __IAsyncMHandler_FWD_DEFINED__
typedef interface IAsyncMHandler IAsyncMHandler;
#endif 	/* __IAsyncMHandler_FWD_DEFINED__ */


#ifndef __IDirectControl_FWD_DEFINED__
#define __IDirectControl_FWD_DEFINED__
typedef interface IDirectControl IDirectControl;
#endif 	/* __IDirectControl_FWD_DEFINED__ */


#ifndef __IDirectControlView_FWD_DEFINED__
#define __IDirectControlView_FWD_DEFINED__
typedef interface IDirectControlView IDirectControlView;
#endif 	/* __IDirectControlView_FWD_DEFINED__ */


#ifndef __IDirectContainer_FWD_DEFINED__
#define __IDirectContainer_FWD_DEFINED__
typedef interface IDirectContainer IDirectContainer;
#endif 	/* __IDirectContainer_FWD_DEFINED__ */


#ifndef __IRadioView_FWD_DEFINED__
#define __IRadioView_FWD_DEFINED__
typedef interface IRadioView IRadioView;
#endif 	/* __IRadioView_FWD_DEFINED__ */


#ifndef __IRadioPlayer_FWD_DEFINED__
#define __IRadioPlayer_FWD_DEFINED__
typedef interface IRadioPlayer IRadioPlayer;
#endif 	/* __IRadioPlayer_FWD_DEFINED__ */


#ifndef __IRadioServer_FWD_DEFINED__
#define __IRadioServer_FWD_DEFINED__
typedef interface IRadioServer IRadioServer;
#endif 	/* __IRadioServer_FWD_DEFINED__ */


#ifndef __IRadioServerControl_FWD_DEFINED__
#define __IRadioServerControl_FWD_DEFINED__
typedef interface IRadioServerControl IRadioServerControl;
#endif 	/* __IRadioServerControl_FWD_DEFINED__ */


#ifndef __IRadioBand_FWD_DEFINED__
#define __IRadioBand_FWD_DEFINED__
typedef interface IRadioBand IRadioBand;
#endif 	/* __IRadioBand_FWD_DEFINED__ */


#ifndef __IMediaBindStream_FWD_DEFINED__
#define __IMediaBindStream_FWD_DEFINED__
typedef interface IMediaBindStream IMediaBindStream;
#endif 	/* __IMediaBindStream_FWD_DEFINED__ */


#ifndef __IMediaPlayerListener_FWD_DEFINED__
#define __IMediaPlayerListener_FWD_DEFINED__
typedef interface IMediaPlayerListener IMediaPlayerListener;
#endif 	/* __IMediaPlayerListener_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_bmedia_0000 */
/* [local] */ 

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

#pragma comment(lib,"uuid.lib")

//---------------------------------------------------------------------------=
// IServiceProvider Interfaces.




extern RPC_IF_HANDLE __MIDL_itf_bmedia_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bmedia_0000_v0_0_s_ifspec;

#ifndef __IServiceProvider_INTERFACE_DEFINED__
#define __IServiceProvider_INTERFACE_DEFINED__

/* interface IServiceProvider */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IServiceProvider __RPC_FAR *LPSERVICEPROVIDER;

#if (_MSC_VER >= 1100) && defined(__cplusplus) && !defined(CINTERFACE)
    EXTERN_C const IID IID_IServiceProvider;
    extern "C++"
    {
        MIDL_INTERFACE("6d5140c1-7436-11ce-8034-00aa006009fa")
        IServiceProvider : public IUnknown
        {
        public:
            virtual /* [local] */ HRESULT STDMETHODCALLTYPE QueryService( 
                /* [in] */ REFGUID guidService,
                /* [in] */ REFIID riid,
                /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject) = 0;
            
            template <class Q>
            HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService, Q** pp)
            {
                return QueryService(guidService, __uuidof(Q), (void **)pp);
            }
        };
    }

    /* [call_as] */ HRESULT STDMETHODCALLTYPE IServiceProvider_RemoteQueryService_Proxy( 
        IServiceProvider __RPC_FAR * This,
        /* [in] */ REFGUID guidService,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);

    void __RPC_STUB IServiceProvider_RemoteQueryService_Stub(
        IRpcStubBuffer *This,
        IRpcChannelBuffer *_pRpcChannelBuffer,
        PRPC_MESSAGE _pRpcMessage,
        DWORD *_pdwStubPhase);

#else // VC6 Hack

EXTERN_C const IID IID_IServiceProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6d5140c1-7436-11ce-8034-00aa006009fa")
    IServiceProvider : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE QueryService( 
            /* [in] */ REFGUID guidService,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IServiceProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IServiceProvider __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IServiceProvider __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IServiceProvider __RPC_FAR * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryService )( 
            IServiceProvider __RPC_FAR * This,
            /* [in] */ REFGUID guidService,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } IServiceProviderVtbl;

    interface IServiceProvider
    {
        CONST_VTBL struct IServiceProviderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IServiceProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IServiceProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IServiceProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IServiceProvider_QueryService(This,guidService,riid,ppvObject)	\
    (This)->lpVtbl -> QueryService(This,guidService,riid,ppvObject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IServiceProvider_RemoteQueryService_Proxy( 
    IServiceProvider __RPC_FAR * This,
    /* [in] */ REFGUID guidService,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);


void __RPC_STUB IServiceProvider_RemoteQueryService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IServiceProvider_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_bmedia_0147 */
/* [local] */ 

#endif // VC6 Hack


extern RPC_IF_HANDLE __MIDL_itf_bmedia_0147_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bmedia_0147_v0_0_s_ifspec;

#ifndef __IAsyncPProt_INTERFACE_DEFINED__
#define __IAsyncPProt_INTERFACE_DEFINED__

/* interface IAsyncPProt */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IAsyncPProt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3DA2AA3A-3D96-11D2-9BD2-204C4F4F5020")
    IAsyncPProt : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAsyncPProtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAsyncPProt __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAsyncPProt __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAsyncPProt __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAsyncPProt __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAsyncPProt __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAsyncPProt __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAsyncPProt __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IAsyncPProtVtbl;

    interface IAsyncPProt
    {
        CONST_VTBL struct IAsyncPProtVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAsyncPProt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAsyncPProt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAsyncPProt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAsyncPProt_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAsyncPProt_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAsyncPProt_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAsyncPProt_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAsyncPProt_INTERFACE_DEFINED__ */


#ifndef __IAsyncMHandler_INTERFACE_DEFINED__
#define __IAsyncMHandler_INTERFACE_DEFINED__

/* interface IAsyncMHandler */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IAsyncMHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3DA2AA3D-3D96-11D2-9BD2-204C4F4F5020")
    IAsyncMHandler : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAsyncMHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAsyncMHandler __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAsyncMHandler __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAsyncMHandler __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAsyncMHandler __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAsyncMHandler __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAsyncMHandler __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAsyncMHandler __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IAsyncMHandlerVtbl;

    interface IAsyncMHandler
    {
        CONST_VTBL struct IAsyncMHandlerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAsyncMHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAsyncMHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAsyncMHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAsyncMHandler_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAsyncMHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAsyncMHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAsyncMHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAsyncMHandler_INTERFACE_DEFINED__ */


#ifndef __IDirectControl_INTERFACE_DEFINED__
#define __IDirectControl_INTERFACE_DEFINED__

/* interface IDirectControl */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IDirectControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("39A2C2A5-4778-11D2-9BDB-204C4F4F5020")
    IDirectControl : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateView( 
            BSTR bszClsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DestroyAllViews( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDirectControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDirectControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDirectControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDirectControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IDirectControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IDirectControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IDirectControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IDirectControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateView )( 
            IDirectControl __RPC_FAR * This,
            BSTR bszClsid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DestroyAllViews )( 
            IDirectControl __RPC_FAR * This);
        
        END_INTERFACE
    } IDirectControlVtbl;

    interface IDirectControl
    {
        CONST_VTBL struct IDirectControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirectControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDirectControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDirectControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDirectControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDirectControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDirectControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDirectControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDirectControl_CreateView(This,bszClsid)	\
    (This)->lpVtbl -> CreateView(This,bszClsid)

#define IDirectControl_DestroyAllViews(This)	\
    (This)->lpVtbl -> DestroyAllViews(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDirectControl_CreateView_Proxy( 
    IDirectControl __RPC_FAR * This,
    BSTR bszClsid);


void __RPC_STUB IDirectControl_CreateView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectControl_DestroyAllViews_Proxy( 
    IDirectControl __RPC_FAR * This);


void __RPC_STUB IDirectControl_DestroyAllViews_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDirectControl_INTERFACE_DEFINED__ */


#ifndef __IDirectControlView_INTERFACE_DEFINED__
#define __IDirectControlView_INTERFACE_DEFINED__

/* interface IDirectControlView */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IDirectControlView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("39A2C2FF-4778-11D2-9BDB-204C4F4F5020")
    IDirectControlView : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsTimerNeeded( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnTimer( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Visible( 
            VARIANT_BOOL bVisible) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnFocusChange( 
            VARIANT_BOOL bFocus) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDirectControlViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDirectControlView __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDirectControlView __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDirectControlView __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IDirectControlView __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IDirectControlView __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IDirectControlView __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IDirectControlView __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsTimerNeeded )( 
            IDirectControlView __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnTimer )( 
            IDirectControlView __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Visible )( 
            IDirectControlView __RPC_FAR * This,
            VARIANT_BOOL bVisible);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnFocusChange )( 
            IDirectControlView __RPC_FAR * This,
            VARIANT_BOOL bFocus);
        
        END_INTERFACE
    } IDirectControlViewVtbl;

    interface IDirectControlView
    {
        CONST_VTBL struct IDirectControlViewVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirectControlView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDirectControlView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDirectControlView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDirectControlView_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDirectControlView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDirectControlView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDirectControlView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDirectControlView_IsTimerNeeded(This)	\
    (This)->lpVtbl -> IsTimerNeeded(This)

#define IDirectControlView_OnTimer(This)	\
    (This)->lpVtbl -> OnTimer(This)

#define IDirectControlView_put_Visible(This,bVisible)	\
    (This)->lpVtbl -> put_Visible(This,bVisible)

#define IDirectControlView_OnFocusChange(This,bFocus)	\
    (This)->lpVtbl -> OnFocusChange(This,bFocus)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDirectControlView_IsTimerNeeded_Proxy( 
    IDirectControlView __RPC_FAR * This);


void __RPC_STUB IDirectControlView_IsTimerNeeded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectControlView_OnTimer_Proxy( 
    IDirectControlView __RPC_FAR * This);


void __RPC_STUB IDirectControlView_OnTimer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectControlView_put_Visible_Proxy( 
    IDirectControlView __RPC_FAR * This,
    VARIANT_BOOL bVisible);


void __RPC_STUB IDirectControlView_put_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectControlView_OnFocusChange_Proxy( 
    IDirectControlView __RPC_FAR * This,
    VARIANT_BOOL bFocus);


void __RPC_STUB IDirectControlView_OnFocusChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDirectControlView_INTERFACE_DEFINED__ */


#ifndef __IDirectContainer_INTERFACE_DEFINED__
#define __IDirectContainer_INTERFACE_DEFINED__

/* interface IDirectContainer */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDirectContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("39A2C2A8-4778-11D2-9BDB-204C4F4F5020")
    IDirectContainer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateControl( 
            BSTR bszClsid,
            DWORD dwClsContext,
            IUnknown __RPC_FAR *__RPC_FAR *ppunk,
            DWORD dwWindowStyle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetServiceProvider( 
            IServiceProvider __RPC_FAR *pspSet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIInputObjectSite( 
            IUnknown __RPC_FAR *pios) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShowControl( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HideControl( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsControlCreated( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DestroyControl( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDirectContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDirectContainer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDirectContainer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDirectContainer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateControl )( 
            IDirectContainer __RPC_FAR * This,
            BSTR bszClsid,
            DWORD dwClsContext,
            IUnknown __RPC_FAR *__RPC_FAR *ppunk,
            DWORD dwWindowStyle);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetServiceProvider )( 
            IDirectContainer __RPC_FAR * This,
            IServiceProvider __RPC_FAR *pspSet);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetIInputObjectSite )( 
            IDirectContainer __RPC_FAR * This,
            IUnknown __RPC_FAR *pios);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowControl )( 
            IDirectContainer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HideControl )( 
            IDirectContainer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsControlCreated )( 
            IDirectContainer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DestroyControl )( 
            IDirectContainer __RPC_FAR * This);
        
        END_INTERFACE
    } IDirectContainerVtbl;

    interface IDirectContainer
    {
        CONST_VTBL struct IDirectContainerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirectContainer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDirectContainer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDirectContainer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDirectContainer_CreateControl(This,bszClsid,dwClsContext,ppunk,dwWindowStyle)	\
    (This)->lpVtbl -> CreateControl(This,bszClsid,dwClsContext,ppunk,dwWindowStyle)

#define IDirectContainer_SetServiceProvider(This,pspSet)	\
    (This)->lpVtbl -> SetServiceProvider(This,pspSet)

#define IDirectContainer_SetIInputObjectSite(This,pios)	\
    (This)->lpVtbl -> SetIInputObjectSite(This,pios)

#define IDirectContainer_ShowControl(This)	\
    (This)->lpVtbl -> ShowControl(This)

#define IDirectContainer_HideControl(This)	\
    (This)->lpVtbl -> HideControl(This)

#define IDirectContainer_IsControlCreated(This)	\
    (This)->lpVtbl -> IsControlCreated(This)

#define IDirectContainer_DestroyControl(This)	\
    (This)->lpVtbl -> DestroyControl(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDirectContainer_CreateControl_Proxy( 
    IDirectContainer __RPC_FAR * This,
    BSTR bszClsid,
    DWORD dwClsContext,
    IUnknown __RPC_FAR *__RPC_FAR *ppunk,
    DWORD dwWindowStyle);


void __RPC_STUB IDirectContainer_CreateControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectContainer_SetServiceProvider_Proxy( 
    IDirectContainer __RPC_FAR * This,
    IServiceProvider __RPC_FAR *pspSet);


void __RPC_STUB IDirectContainer_SetServiceProvider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectContainer_SetIInputObjectSite_Proxy( 
    IDirectContainer __RPC_FAR * This,
    IUnknown __RPC_FAR *pios);


void __RPC_STUB IDirectContainer_SetIInputObjectSite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectContainer_ShowControl_Proxy( 
    IDirectContainer __RPC_FAR * This);


void __RPC_STUB IDirectContainer_ShowControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectContainer_HideControl_Proxy( 
    IDirectContainer __RPC_FAR * This);


void __RPC_STUB IDirectContainer_HideControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectContainer_IsControlCreated_Proxy( 
    IDirectContainer __RPC_FAR * This);


void __RPC_STUB IDirectContainer_IsControlCreated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDirectContainer_DestroyControl_Proxy( 
    IDirectContainer __RPC_FAR * This);


void __RPC_STUB IDirectContainer_DestroyControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDirectContainer_INTERFACE_DEFINED__ */


#ifndef __IRadioView_INTERFACE_DEFINED__
#define __IRadioView_INTERFACE_DEFINED__

/* interface IRadioView */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IRadioView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("847B4DF4-4B61-11D2-9BDB-204C4F4F5020")
    IRadioView : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IRadioViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRadioView __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRadioView __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRadioView __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRadioView __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRadioView __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRadioView __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRadioView __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IRadioViewVtbl;

    interface IRadioView
    {
        CONST_VTBL struct IRadioViewVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRadioView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRadioView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRadioView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRadioView_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRadioView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRadioView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRadioView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRadioView_INTERFACE_DEFINED__ */


#ifndef __IRadioPlayer_INTERFACE_DEFINED__
#define __IRadioPlayer_INTERFACE_DEFINED__

/* interface IRadioPlayer */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IRadioPlayer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9C2263AF-3E3C-11D2-9BD3-204C4F4F5020")
    IRadioPlayer : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE BindRadioMemory( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ReleaseRadio( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RegisterEvent( 
            /* [in] */ BSTR bszEvent,
            /* [retval][out] */ LONG __RPC_FAR *plRegister) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RegisterWindow( 
            /* [in] */ LONG lHWND,
            /* [in] */ DWORD dwMessage,
            /* [in] */ DWORD dwCodeSet,
            /* [retval][out] */ LONG __RPC_FAR *plRegister) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetSection( 
            /* [retval][out] */ BSTR __RPC_FAR *bszSection) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Unregister( 
            /* [in] */ LONG lRegister) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetInstanceCount( 
            /* [retval][out] */ LONG __RPC_FAR *plInstances) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Play( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Url( 
            /* [in] */ BSTR wszUrl) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Volume( 
            /* [in] */ LONG lVolumeSet) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Mute( 
            /* [in] */ VARIANT_BOOL fMuteSet) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetStatus( 
            /* [out] */ LONG __RPC_FAR *plVolume,
            /* [out] */ LONG __RPC_FAR *pfMute,
            /* [out] */ LONG __RPC_FAR *pfPlay,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0015,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0016,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0017,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0018,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0019,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0020,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0021) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetState( 
            /* [out] */ LONG __RPC_FAR *plOpenState,
            /* [out] */ LONG __RPC_FAR *pfBuffering,
            /* [out] */ LONG __RPC_FAR *plBufferingPercent,
            /* [out] */ LONG __RPC_FAR *plQuality) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRadioPlayerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRadioPlayer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRadioPlayer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRadioPlayer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BindRadioMemory )( 
            IRadioPlayer __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReleaseRadio )( 
            IRadioPlayer __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterEvent )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ BSTR bszEvent,
            /* [retval][out] */ LONG __RPC_FAR *plRegister);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterWindow )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ LONG lHWND,
            /* [in] */ DWORD dwMessage,
            /* [in] */ DWORD dwCodeSet,
            /* [retval][out] */ LONG __RPC_FAR *plRegister);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSection )( 
            IRadioPlayer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bszSection);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Unregister )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ LONG lRegister);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetInstanceCount )( 
            IRadioPlayer __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plInstances);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Play )( 
            IRadioPlayer __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IRadioPlayer __RPC_FAR * This);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Url )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ BSTR wszUrl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Volume )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ LONG lVolumeSet);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Mute )( 
            IRadioPlayer __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fMuteSet);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetStatus )( 
            IRadioPlayer __RPC_FAR * This,
            /* [out] */ LONG __RPC_FAR *plVolume,
            /* [out] */ LONG __RPC_FAR *pfMute,
            /* [out] */ LONG __RPC_FAR *pfPlay,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0015,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0016,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0017,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0018,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0019,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0020,
            /* [out] */ BSTR __RPC_FAR *__MIDL_0021);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetState )( 
            IRadioPlayer __RPC_FAR * This,
            /* [out] */ LONG __RPC_FAR *plOpenState,
            /* [out] */ LONG __RPC_FAR *pfBuffering,
            /* [out] */ LONG __RPC_FAR *plBufferingPercent,
            /* [out] */ LONG __RPC_FAR *plQuality);
        
        END_INTERFACE
    } IRadioPlayerVtbl;

    interface IRadioPlayer
    {
        CONST_VTBL struct IRadioPlayerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRadioPlayer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRadioPlayer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRadioPlayer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRadioPlayer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRadioPlayer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRadioPlayer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRadioPlayer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRadioPlayer_BindRadioMemory(This)	\
    (This)->lpVtbl -> BindRadioMemory(This)

#define IRadioPlayer_ReleaseRadio(This)	\
    (This)->lpVtbl -> ReleaseRadio(This)

#define IRadioPlayer_RegisterEvent(This,bszEvent,plRegister)	\
    (This)->lpVtbl -> RegisterEvent(This,bszEvent,plRegister)

#define IRadioPlayer_RegisterWindow(This,lHWND,dwMessage,dwCodeSet,plRegister)	\
    (This)->lpVtbl -> RegisterWindow(This,lHWND,dwMessage,dwCodeSet,plRegister)

#define IRadioPlayer_GetSection(This,bszSection)	\
    (This)->lpVtbl -> GetSection(This,bszSection)

#define IRadioPlayer_Unregister(This,lRegister)	\
    (This)->lpVtbl -> Unregister(This,lRegister)

#define IRadioPlayer_GetInstanceCount(This,plInstances)	\
    (This)->lpVtbl -> GetInstanceCount(This,plInstances)

#define IRadioPlayer_Play(This)	\
    (This)->lpVtbl -> Play(This)

#define IRadioPlayer_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IRadioPlayer_put_Url(This,wszUrl)	\
    (This)->lpVtbl -> put_Url(This,wszUrl)

#define IRadioPlayer_put_Volume(This,lVolumeSet)	\
    (This)->lpVtbl -> put_Volume(This,lVolumeSet)

#define IRadioPlayer_put_Mute(This,fMuteSet)	\
    (This)->lpVtbl -> put_Mute(This,fMuteSet)

#define IRadioPlayer_GetStatus(This,plVolume,pfMute,pfPlay,__MIDL_0015,__MIDL_0016,__MIDL_0017,__MIDL_0018,__MIDL_0019,__MIDL_0020,__MIDL_0021)	\
    (This)->lpVtbl -> GetStatus(This,plVolume,pfMute,pfPlay,__MIDL_0015,__MIDL_0016,__MIDL_0017,__MIDL_0018,__MIDL_0019,__MIDL_0020,__MIDL_0021)

#define IRadioPlayer_GetState(This,plOpenState,pfBuffering,plBufferingPercent,plQuality)	\
    (This)->lpVtbl -> GetState(This,plOpenState,pfBuffering,plBufferingPercent,plQuality)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_BindRadioMemory_Proxy( 
    IRadioPlayer __RPC_FAR * This);


void __RPC_STUB IRadioPlayer_BindRadioMemory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_ReleaseRadio_Proxy( 
    IRadioPlayer __RPC_FAR * This);


void __RPC_STUB IRadioPlayer_ReleaseRadio_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_RegisterEvent_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [in] */ BSTR bszEvent,
    /* [retval][out] */ LONG __RPC_FAR *plRegister);


void __RPC_STUB IRadioPlayer_RegisterEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_RegisterWindow_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [in] */ LONG lHWND,
    /* [in] */ DWORD dwMessage,
    /* [in] */ DWORD dwCodeSet,
    /* [retval][out] */ LONG __RPC_FAR *plRegister);


void __RPC_STUB IRadioPlayer_RegisterWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_GetSection_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bszSection);


void __RPC_STUB IRadioPlayer_GetSection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_Unregister_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [in] */ LONG lRegister);


void __RPC_STUB IRadioPlayer_Unregister_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_GetInstanceCount_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plInstances);


void __RPC_STUB IRadioPlayer_GetInstanceCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_Play_Proxy( 
    IRadioPlayer __RPC_FAR * This);


void __RPC_STUB IRadioPlayer_Play_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_Stop_Proxy( 
    IRadioPlayer __RPC_FAR * This);


void __RPC_STUB IRadioPlayer_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_put_Url_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [in] */ BSTR wszUrl);


void __RPC_STUB IRadioPlayer_put_Url_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_put_Volume_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [in] */ LONG lVolumeSet);


void __RPC_STUB IRadioPlayer_put_Volume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_put_Mute_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fMuteSet);


void __RPC_STUB IRadioPlayer_put_Mute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_GetStatus_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [out] */ LONG __RPC_FAR *plVolume,
    /* [out] */ LONG __RPC_FAR *pfMute,
    /* [out] */ LONG __RPC_FAR *pfPlay,
    /* [out] */ BSTR __RPC_FAR *__MIDL_0015,
    /* [out] */ BSTR __RPC_FAR *__MIDL_0016,
    /* [out] */ BSTR __RPC_FAR *__MIDL_0017,
    /* [out] */ BSTR __RPC_FAR *__MIDL_0018,
    /* [out] */ BSTR __RPC_FAR *__MIDL_0019,
    /* [out] */ BSTR __RPC_FAR *__MIDL_0020,
    /* [out] */ BSTR __RPC_FAR *__MIDL_0021);


void __RPC_STUB IRadioPlayer_GetStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioPlayer_GetState_Proxy( 
    IRadioPlayer __RPC_FAR * This,
    /* [out] */ LONG __RPC_FAR *plOpenState,
    /* [out] */ LONG __RPC_FAR *pfBuffering,
    /* [out] */ LONG __RPC_FAR *plBufferingPercent,
    /* [out] */ LONG __RPC_FAR *plQuality);


void __RPC_STUB IRadioPlayer_GetState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRadioPlayer_INTERFACE_DEFINED__ */


#ifndef __IRadioServer_INTERFACE_DEFINED__
#define __IRadioServer_INTERFACE_DEFINED__

/* interface IRadioServer */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IRadioServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9C2263A0-3E3C-11D2-9BD3-204C4F4F5020")
    IRadioServer : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE BindToRadio( 
            /* [in] */ BSTR wszRadio,
            /* [retval][out] */ IRadioPlayer __RPC_FAR *__RPC_FAR *ppServer) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsRadioExists( 
            /* [in] */ BSTR wszRadio) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LaunchStandardUrl( 
            /* [in] */ BSTR bszUrl,
            /* [in] */ IUnknown __RPC_FAR *pBrowser) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRadioServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRadioServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRadioServer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRadioServer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRadioServer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRadioServer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRadioServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRadioServer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BindToRadio )( 
            IRadioServer __RPC_FAR * This,
            /* [in] */ BSTR wszRadio,
            /* [retval][out] */ IRadioPlayer __RPC_FAR *__RPC_FAR *ppServer);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsRadioExists )( 
            IRadioServer __RPC_FAR * This,
            /* [in] */ BSTR wszRadio);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LaunchStandardUrl )( 
            IRadioServer __RPC_FAR * This,
            /* [in] */ BSTR bszUrl,
            /* [in] */ IUnknown __RPC_FAR *pBrowser);
        
        END_INTERFACE
    } IRadioServerVtbl;

    interface IRadioServer
    {
        CONST_VTBL struct IRadioServerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRadioServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRadioServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRadioServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRadioServer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRadioServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRadioServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRadioServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRadioServer_BindToRadio(This,wszRadio,ppServer)	\
    (This)->lpVtbl -> BindToRadio(This,wszRadio,ppServer)

#define IRadioServer_IsRadioExists(This,wszRadio)	\
    (This)->lpVtbl -> IsRadioExists(This,wszRadio)

#define IRadioServer_LaunchStandardUrl(This,bszUrl,pBrowser)	\
    (This)->lpVtbl -> LaunchStandardUrl(This,bszUrl,pBrowser)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IRadioServer_BindToRadio_Proxy( 
    IRadioServer __RPC_FAR * This,
    /* [in] */ BSTR wszRadio,
    /* [retval][out] */ IRadioPlayer __RPC_FAR *__RPC_FAR *ppServer);


void __RPC_STUB IRadioServer_BindToRadio_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioServer_IsRadioExists_Proxy( 
    IRadioServer __RPC_FAR * This,
    /* [in] */ BSTR wszRadio);


void __RPC_STUB IRadioServer_IsRadioExists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRadioServer_LaunchStandardUrl_Proxy( 
    IRadioServer __RPC_FAR * This,
    /* [in] */ BSTR bszUrl,
    /* [in] */ IUnknown __RPC_FAR *pBrowser);


void __RPC_STUB IRadioServer_LaunchStandardUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRadioServer_INTERFACE_DEFINED__ */


#ifndef __IRadioServerControl_INTERFACE_DEFINED__
#define __IRadioServerControl_INTERFACE_DEFINED__

/* interface IRadioServerControl */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IRadioServerControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8E718889-423F-11D2-876E-00A0C9082467")
    IRadioServerControl : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IRadioServerControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRadioServerControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRadioServerControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRadioServerControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRadioServerControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRadioServerControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRadioServerControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRadioServerControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IRadioServerControlVtbl;

    interface IRadioServerControl
    {
        CONST_VTBL struct IRadioServerControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRadioServerControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRadioServerControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRadioServerControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRadioServerControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRadioServerControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRadioServerControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRadioServerControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRadioServerControl_INTERFACE_DEFINED__ */


#ifndef __IRadioBand_INTERFACE_DEFINED__
#define __IRadioBand_INTERFACE_DEFINED__

/* interface IRadioBand */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IRadioBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8E718881-423F-11D2-876E-00A0C9082467")
    IRadioBand : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Create( 
            LONG __RPC_FAR *phwnd,
            LONG hwndParent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRadioBandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRadioBand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRadioBand __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRadioBand __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRadioBand __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRadioBand __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRadioBand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRadioBand __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            IRadioBand __RPC_FAR * This,
            LONG __RPC_FAR *phwnd,
            LONG hwndParent);
        
        END_INTERFACE
    } IRadioBandVtbl;

    interface IRadioBand
    {
        CONST_VTBL struct IRadioBandVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRadioBand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRadioBand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRadioBand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRadioBand_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRadioBand_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRadioBand_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRadioBand_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRadioBand_Create(This,phwnd,hwndParent)	\
    (This)->lpVtbl -> Create(This,phwnd,hwndParent)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IRadioBand_Create_Proxy( 
    IRadioBand __RPC_FAR * This,
    LONG __RPC_FAR *phwnd,
    LONG hwndParent);


void __RPC_STUB IRadioBand_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRadioBand_INTERFACE_DEFINED__ */


#ifndef __IMediaBindStream_INTERFACE_DEFINED__
#define __IMediaBindStream_INTERFACE_DEFINED__

/* interface IMediaBindStream */
/* [object][unique][dual][uuid] */ 


EXTERN_C const IID IID_IMediaBindStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("920F0DE3-91C5-11d2-828F-00C04FC99D4E")
    IMediaBindStream : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LoadMoniker( 
            /* [in] */ BSTR bszTransferContext,
            /* [in] */ BSTR bszUrl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaBindStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaBindStream __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaBindStream __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaBindStream __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaBindStream __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaBindStream __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaBindStream __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaBindStream __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadMoniker )( 
            IMediaBindStream __RPC_FAR * This,
            /* [in] */ BSTR bszTransferContext,
            /* [in] */ BSTR bszUrl);
        
        END_INTERFACE
    } IMediaBindStreamVtbl;

    interface IMediaBindStream
    {
        CONST_VTBL struct IMediaBindStreamVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaBindStream_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaBindStream_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaBindStream_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaBindStream_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaBindStream_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaBindStream_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaBindStream_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaBindStream_LoadMoniker(This,bszTransferContext,bszUrl)	\
    (This)->lpVtbl -> LoadMoniker(This,bszTransferContext,bszUrl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IMediaBindStream_LoadMoniker_Proxy( 
    IMediaBindStream __RPC_FAR * This,
    /* [in] */ BSTR bszTransferContext,
    /* [in] */ BSTR bszUrl);


void __RPC_STUB IMediaBindStream_LoadMoniker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaBindStream_INTERFACE_DEFINED__ */


#ifndef __IMediaPlayerListener_INTERFACE_DEFINED__
#define __IMediaPlayerListener_INTERFACE_DEFINED__

/* interface IMediaPlayerListener */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IMediaPlayerListener;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33222211-5E5E-11d2-9E8E-0000F8085981")
    IMediaPlayerListener : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PlayStateChanged( 
            /* [in] */ long lNewState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Buffering( 
            /* [in] */ VARIANT_BOOL fStart) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BufferPercent( 
            /* [in] */ long lBufferPercent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OpenStateChanged( 
            /* [in] */ long lOpenState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MediaInfoChanged( 
            /* [in] */ BSTR bstrShowTitle,
            /* [in] */ BSTR bstrClipTitle,
            /* [in] */ BSTR bstrClipAuthor,
            /* [in] */ BSTR bstrClipCopyright,
            /* [in] */ BSTR bstrStationURL) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QualityChanged( 
            /* [in] */ long lQuality) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Error( 
            /* [in] */ BSTR bstrError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaPlayerListenerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaPlayerListener __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaPlayerListener __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaPlayerListener __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlayStateChanged )( 
            IMediaPlayerListener __RPC_FAR * This,
            /* [in] */ long lNewState);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Buffering )( 
            IMediaPlayerListener __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fStart);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BufferPercent )( 
            IMediaPlayerListener __RPC_FAR * This,
            /* [in] */ long lBufferPercent);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenStateChanged )( 
            IMediaPlayerListener __RPC_FAR * This,
            /* [in] */ long lOpenState);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MediaInfoChanged )( 
            IMediaPlayerListener __RPC_FAR * This,
            /* [in] */ BSTR bstrShowTitle,
            /* [in] */ BSTR bstrClipTitle,
            /* [in] */ BSTR bstrClipAuthor,
            /* [in] */ BSTR bstrClipCopyright,
            /* [in] */ BSTR bstrStationURL);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QualityChanged )( 
            IMediaPlayerListener __RPC_FAR * This,
            /* [in] */ long lQuality);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Error )( 
            IMediaPlayerListener __RPC_FAR * This,
            /* [in] */ BSTR bstrError);
        
        END_INTERFACE
    } IMediaPlayerListenerVtbl;

    interface IMediaPlayerListener
    {
        CONST_VTBL struct IMediaPlayerListenerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaPlayerListener_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaPlayerListener_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaPlayerListener_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaPlayerListener_PlayStateChanged(This,lNewState)	\
    (This)->lpVtbl -> PlayStateChanged(This,lNewState)

#define IMediaPlayerListener_Buffering(This,fStart)	\
    (This)->lpVtbl -> Buffering(This,fStart)

#define IMediaPlayerListener_BufferPercent(This,lBufferPercent)	\
    (This)->lpVtbl -> BufferPercent(This,lBufferPercent)

#define IMediaPlayerListener_OpenStateChanged(This,lOpenState)	\
    (This)->lpVtbl -> OpenStateChanged(This,lOpenState)

#define IMediaPlayerListener_MediaInfoChanged(This,bstrShowTitle,bstrClipTitle,bstrClipAuthor,bstrClipCopyright,bstrStationURL)	\
    (This)->lpVtbl -> MediaInfoChanged(This,bstrShowTitle,bstrClipTitle,bstrClipAuthor,bstrClipCopyright,bstrStationURL)

#define IMediaPlayerListener_QualityChanged(This,lQuality)	\
    (This)->lpVtbl -> QualityChanged(This,lQuality)

#define IMediaPlayerListener_Error(This,bstrError)	\
    (This)->lpVtbl -> Error(This,bstrError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaPlayerListener_PlayStateChanged_Proxy( 
    IMediaPlayerListener __RPC_FAR * This,
    /* [in] */ long lNewState);


void __RPC_STUB IMediaPlayerListener_PlayStateChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPlayerListener_Buffering_Proxy( 
    IMediaPlayerListener __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fStart);


void __RPC_STUB IMediaPlayerListener_Buffering_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPlayerListener_BufferPercent_Proxy( 
    IMediaPlayerListener __RPC_FAR * This,
    /* [in] */ long lBufferPercent);


void __RPC_STUB IMediaPlayerListener_BufferPercent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPlayerListener_OpenStateChanged_Proxy( 
    IMediaPlayerListener __RPC_FAR * This,
    /* [in] */ long lOpenState);


void __RPC_STUB IMediaPlayerListener_OpenStateChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPlayerListener_MediaInfoChanged_Proxy( 
    IMediaPlayerListener __RPC_FAR * This,
    /* [in] */ BSTR bstrShowTitle,
    /* [in] */ BSTR bstrClipTitle,
    /* [in] */ BSTR bstrClipAuthor,
    /* [in] */ BSTR bstrClipCopyright,
    /* [in] */ BSTR bstrStationURL);


void __RPC_STUB IMediaPlayerListener_MediaInfoChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPlayerListener_QualityChanged_Proxy( 
    IMediaPlayerListener __RPC_FAR * This,
    /* [in] */ long lQuality);


void __RPC_STUB IMediaPlayerListener_QualityChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPlayerListener_Error_Proxy( 
    IMediaPlayerListener __RPC_FAR * This,
    /* [in] */ BSTR bstrError);


void __RPC_STUB IMediaPlayerListener_Error_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaPlayerListener_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* [local] */ HRESULT STDMETHODCALLTYPE IServiceProvider_QueryService_Proxy( 
    IServiceProvider __RPC_FAR * This,
    /* [in] */ REFGUID guidService,
    /* [in] */ REFIID riid,
    /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IServiceProvider_QueryService_Stub( 
    IServiceProvider __RPC_FAR * This,
    /* [in] */ REFGUID guidService,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


