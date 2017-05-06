
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Thu Jun 24 19:41:39 2004
 */
/* Compiler settings for D:\macallan\private\directx\dshow\idl\.\qnetwork.odl:
    Os (OptLev=s), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
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

#ifndef __qnetwork_h__
#define __qnetwork_h__

/* Forward Declarations */ 

#ifndef __IAMNetShowConfig_FWD_DEFINED__
#define __IAMNetShowConfig_FWD_DEFINED__
typedef interface IAMNetShowConfig IAMNetShowConfig;
#endif 	/* __IAMNetShowConfig_FWD_DEFINED__ */


#ifndef __IAMChannelInfo_FWD_DEFINED__
#define __IAMChannelInfo_FWD_DEFINED__
typedef interface IAMChannelInfo IAMChannelInfo;
#endif 	/* __IAMChannelInfo_FWD_DEFINED__ */


#ifndef __IAMNetworkStatus_FWD_DEFINED__
#define __IAMNetworkStatus_FWD_DEFINED__
typedef interface IAMNetworkStatus IAMNetworkStatus;
#endif 	/* __IAMNetworkStatus_FWD_DEFINED__ */


#ifndef __IAMExtendedSeeking_FWD_DEFINED__
#define __IAMExtendedSeeking_FWD_DEFINED__
typedef interface IAMExtendedSeeking IAMExtendedSeeking;
#endif 	/* __IAMExtendedSeeking_FWD_DEFINED__ */


#ifndef __IAMNetShowExProps_FWD_DEFINED__
#define __IAMNetShowExProps_FWD_DEFINED__
typedef interface IAMNetShowExProps IAMNetShowExProps;
#endif 	/* __IAMNetShowExProps_FWD_DEFINED__ */


#ifndef __IAMExtendedErrorInfo_FWD_DEFINED__
#define __IAMExtendedErrorInfo_FWD_DEFINED__
typedef interface IAMExtendedErrorInfo IAMExtendedErrorInfo;
#endif 	/* __IAMExtendedErrorInfo_FWD_DEFINED__ */


#ifndef __IAMMediaContent_FWD_DEFINED__
#define __IAMMediaContent_FWD_DEFINED__
typedef interface IAMMediaContent IAMMediaContent;
#endif 	/* __IAMMediaContent_FWD_DEFINED__ */


#ifndef __IAMMediaContent2_FWD_DEFINED__
#define __IAMMediaContent2_FWD_DEFINED__
typedef interface IAMMediaContent2 IAMMediaContent2;
#endif 	/* __IAMMediaContent2_FWD_DEFINED__ */


#ifndef __IAMMediaContentEx_FWD_DEFINED__
#define __IAMMediaContentEx_FWD_DEFINED__
typedef interface IAMMediaContentEx IAMMediaContentEx;
#endif 	/* __IAMMediaContentEx_FWD_DEFINED__ */


#ifndef __IAMNetShowPreroll_FWD_DEFINED__
#define __IAMNetShowPreroll_FWD_DEFINED__
typedef interface IAMNetShowPreroll IAMNetShowPreroll;
#endif 	/* __IAMNetShowPreroll_FWD_DEFINED__ */


#ifndef __IDShowPlugin_FWD_DEFINED__
#define __IDShowPlugin_FWD_DEFINED__
typedef interface IDShowPlugin IDShowPlugin;
#endif 	/* __IDShowPlugin_FWD_DEFINED__ */


#ifndef __IAMSecureMediaContent_FWD_DEFINED__
#define __IAMSecureMediaContent_FWD_DEFINED__
typedef interface IAMSecureMediaContent IAMSecureMediaContent;
#endif 	/* __IAMSecureMediaContent_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __QuartzNetTypeLib_LIBRARY_DEFINED__
#define __QuartzNetTypeLib_LIBRARY_DEFINED__

/* library QuartzNetTypeLib */
/* [version][lcid][helpstring][uuid] */ 

typedef 
enum __MIDL___MIDL_itf_qnetwork_0082_0001
    {	AM_EXSEEK_CANSEEK	= 1,
	AM_EXSEEK_CANSCAN	= 2,
	AM_EXSEEK_MARKERSEEK	= 4,
	AM_EXSEEK_SCANWITHOUTCLOCK	= 8,
	AM_EXSEEK_NOSTANDARDREPAINT	= 16,
	AM_EXSEEK_BUFFERING	= 32,
	AM_EXSEEK_SENDS_VIDEOFRAMEREADY	= 64
    }	AMExtendedSeekingCapabilities;


DEFINE_GUID(LIBID_QuartzNetTypeLib,0x56a868b1,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#ifndef __IAMNetShowConfig_INTERFACE_DEFINED__
#define __IAMNetShowConfig_INTERFACE_DEFINED__

/* interface IAMNetShowConfig */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMNetShowConfig,0xFA2AA8F1,0x8B62,0x11d0,0xA5,0x20,0x00,0x00,0x00,0x00,0x00,0x00);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FA2AA8F1-8B62-11d0-A520-000000000000")
    IAMNetShowConfig : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BufferingTime( 
            /* [retval][out] */ double __RPC_FAR *pBufferingTime) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_BufferingTime( 
            /* [in] */ double BufferingTime) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UseFixedUDPPort( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseFixedUDPPort) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_UseFixedUDPPort( 
            /* [in] */ VARIANT_BOOL UseFixedUDPPort) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FixedUDPPort( 
            /* [retval][out] */ long __RPC_FAR *pFixedUDPPort) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_FixedUDPPort( 
            /* [in] */ long FixedUDPPort) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UseHTTPProxy( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseHTTPProxy) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_UseHTTPProxy( 
            /* [in] */ VARIANT_BOOL UseHTTPProxy) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableAutoProxy( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableAutoProxy) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableAutoProxy( 
            /* [in] */ VARIANT_BOOL EnableAutoProxy) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_HTTPProxyHost( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrHTTPProxyHost) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_HTTPProxyHost( 
            /* [in] */ BSTR bstrHTTPProxyHost) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_HTTPProxyPort( 
            /* [retval][out] */ long __RPC_FAR *pHTTPProxyPort) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_HTTPProxyPort( 
            /* [in] */ long HTTPProxyPort) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableMulticast( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableMulticast) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableMulticast( 
            /* [in] */ VARIANT_BOOL EnableMulticast) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableUDP( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableUDP) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableUDP( 
            /* [in] */ VARIANT_BOOL EnableUDP) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableTCP( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableTCP) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableTCP( 
            /* [in] */ VARIANT_BOOL EnableTCP) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnableHTTP( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableHTTP) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableHTTP( 
            /* [in] */ VARIANT_BOOL EnableHTTP) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMNetShowConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMNetShowConfig __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMNetShowConfig __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingTime )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pBufferingTime);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BufferingTime )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ double BufferingTime);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseFixedUDPPort )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseFixedUDPPort);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseFixedUDPPort )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL UseFixedUDPPort);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FixedUDPPort )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pFixedUDPPort);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FixedUDPPort )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ long FixedUDPPort);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseHTTPProxy )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseHTTPProxy);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseHTTPProxy )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL UseHTTPProxy);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableAutoProxy )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableAutoProxy);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableAutoProxy )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableAutoProxy);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HTTPProxyHost )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrHTTPProxyHost);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HTTPProxyHost )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ BSTR bstrHTTPProxyHost);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HTTPProxyPort )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHTTPProxyPort);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HTTPProxyPort )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ long HTTPProxyPort);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableMulticast )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableMulticast);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableMulticast )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableMulticast);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableUDP )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableUDP);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableUDP )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableUDP);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableTCP )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableTCP);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableTCP )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableTCP);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableHTTP )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableHTTP);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableHTTP )( 
            IAMNetShowConfig __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL EnableHTTP);
        
        END_INTERFACE
    } IAMNetShowConfigVtbl;

    interface IAMNetShowConfig
    {
        CONST_VTBL struct IAMNetShowConfigVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMNetShowConfig_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMNetShowConfig_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMNetShowConfig_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMNetShowConfig_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMNetShowConfig_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMNetShowConfig_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMNetShowConfig_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMNetShowConfig_get_BufferingTime(This,pBufferingTime)	\
    (This)->lpVtbl -> get_BufferingTime(This,pBufferingTime)

#define IAMNetShowConfig_put_BufferingTime(This,BufferingTime)	\
    (This)->lpVtbl -> put_BufferingTime(This,BufferingTime)

#define IAMNetShowConfig_get_UseFixedUDPPort(This,pUseFixedUDPPort)	\
    (This)->lpVtbl -> get_UseFixedUDPPort(This,pUseFixedUDPPort)

#define IAMNetShowConfig_put_UseFixedUDPPort(This,UseFixedUDPPort)	\
    (This)->lpVtbl -> put_UseFixedUDPPort(This,UseFixedUDPPort)

#define IAMNetShowConfig_get_FixedUDPPort(This,pFixedUDPPort)	\
    (This)->lpVtbl -> get_FixedUDPPort(This,pFixedUDPPort)

#define IAMNetShowConfig_put_FixedUDPPort(This,FixedUDPPort)	\
    (This)->lpVtbl -> put_FixedUDPPort(This,FixedUDPPort)

#define IAMNetShowConfig_get_UseHTTPProxy(This,pUseHTTPProxy)	\
    (This)->lpVtbl -> get_UseHTTPProxy(This,pUseHTTPProxy)

#define IAMNetShowConfig_put_UseHTTPProxy(This,UseHTTPProxy)	\
    (This)->lpVtbl -> put_UseHTTPProxy(This,UseHTTPProxy)

#define IAMNetShowConfig_get_EnableAutoProxy(This,pEnableAutoProxy)	\
    (This)->lpVtbl -> get_EnableAutoProxy(This,pEnableAutoProxy)

#define IAMNetShowConfig_put_EnableAutoProxy(This,EnableAutoProxy)	\
    (This)->lpVtbl -> put_EnableAutoProxy(This,EnableAutoProxy)

#define IAMNetShowConfig_get_HTTPProxyHost(This,pbstrHTTPProxyHost)	\
    (This)->lpVtbl -> get_HTTPProxyHost(This,pbstrHTTPProxyHost)

#define IAMNetShowConfig_put_HTTPProxyHost(This,bstrHTTPProxyHost)	\
    (This)->lpVtbl -> put_HTTPProxyHost(This,bstrHTTPProxyHost)

#define IAMNetShowConfig_get_HTTPProxyPort(This,pHTTPProxyPort)	\
    (This)->lpVtbl -> get_HTTPProxyPort(This,pHTTPProxyPort)

#define IAMNetShowConfig_put_HTTPProxyPort(This,HTTPProxyPort)	\
    (This)->lpVtbl -> put_HTTPProxyPort(This,HTTPProxyPort)

#define IAMNetShowConfig_get_EnableMulticast(This,pEnableMulticast)	\
    (This)->lpVtbl -> get_EnableMulticast(This,pEnableMulticast)

#define IAMNetShowConfig_put_EnableMulticast(This,EnableMulticast)	\
    (This)->lpVtbl -> put_EnableMulticast(This,EnableMulticast)

#define IAMNetShowConfig_get_EnableUDP(This,pEnableUDP)	\
    (This)->lpVtbl -> get_EnableUDP(This,pEnableUDP)

#define IAMNetShowConfig_put_EnableUDP(This,EnableUDP)	\
    (This)->lpVtbl -> put_EnableUDP(This,EnableUDP)

#define IAMNetShowConfig_get_EnableTCP(This,pEnableTCP)	\
    (This)->lpVtbl -> get_EnableTCP(This,pEnableTCP)

#define IAMNetShowConfig_put_EnableTCP(This,EnableTCP)	\
    (This)->lpVtbl -> put_EnableTCP(This,EnableTCP)

#define IAMNetShowConfig_get_EnableHTTP(This,pEnableHTTP)	\
    (This)->lpVtbl -> get_EnableHTTP(This,pEnableHTTP)

#define IAMNetShowConfig_put_EnableHTTP(This,EnableHTTP)	\
    (This)->lpVtbl -> put_EnableHTTP(This,EnableHTTP)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_BufferingTime_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pBufferingTime);


void __RPC_STUB IAMNetShowConfig_get_BufferingTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_BufferingTime_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ double BufferingTime);


void __RPC_STUB IAMNetShowConfig_put_BufferingTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_UseFixedUDPPort_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseFixedUDPPort);


void __RPC_STUB IAMNetShowConfig_get_UseFixedUDPPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_UseFixedUDPPort_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL UseFixedUDPPort);


void __RPC_STUB IAMNetShowConfig_put_UseFixedUDPPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_FixedUDPPort_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pFixedUDPPort);


void __RPC_STUB IAMNetShowConfig_get_FixedUDPPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_FixedUDPPort_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ long FixedUDPPort);


void __RPC_STUB IAMNetShowConfig_put_FixedUDPPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_UseHTTPProxy_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseHTTPProxy);


void __RPC_STUB IAMNetShowConfig_get_UseHTTPProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_UseHTTPProxy_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL UseHTTPProxy);


void __RPC_STUB IAMNetShowConfig_put_UseHTTPProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_EnableAutoProxy_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableAutoProxy);


void __RPC_STUB IAMNetShowConfig_get_EnableAutoProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_EnableAutoProxy_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableAutoProxy);


void __RPC_STUB IAMNetShowConfig_put_EnableAutoProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_HTTPProxyHost_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrHTTPProxyHost);


void __RPC_STUB IAMNetShowConfig_get_HTTPProxyHost_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_HTTPProxyHost_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ BSTR bstrHTTPProxyHost);


void __RPC_STUB IAMNetShowConfig_put_HTTPProxyHost_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_HTTPProxyPort_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pHTTPProxyPort);


void __RPC_STUB IAMNetShowConfig_get_HTTPProxyPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_HTTPProxyPort_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ long HTTPProxyPort);


void __RPC_STUB IAMNetShowConfig_put_HTTPProxyPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_EnableMulticast_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableMulticast);


void __RPC_STUB IAMNetShowConfig_get_EnableMulticast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_EnableMulticast_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableMulticast);


void __RPC_STUB IAMNetShowConfig_put_EnableMulticast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_EnableUDP_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableUDP);


void __RPC_STUB IAMNetShowConfig_get_EnableUDP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_EnableUDP_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableUDP);


void __RPC_STUB IAMNetShowConfig_put_EnableUDP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_EnableTCP_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableTCP);


void __RPC_STUB IAMNetShowConfig_get_EnableTCP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_EnableTCP_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableTCP);


void __RPC_STUB IAMNetShowConfig_put_EnableTCP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_get_EnableHTTP_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEnableHTTP);


void __RPC_STUB IAMNetShowConfig_get_EnableHTTP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowConfig_put_EnableHTTP_Proxy( 
    IAMNetShowConfig __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL EnableHTTP);


void __RPC_STUB IAMNetShowConfig_put_EnableHTTP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMNetShowConfig_INTERFACE_DEFINED__ */


#ifndef __IAMChannelInfo_INTERFACE_DEFINED__
#define __IAMChannelInfo_INTERFACE_DEFINED__

/* interface IAMChannelInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMChannelInfo,0xFA2AA8F2,0x8B62,0x11d0,0xA5,0x20,0x00,0x00,0x00,0x00,0x00,0x00);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FA2AA8F2-8B62-11d0-A520-000000000000")
    IAMChannelInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ChannelName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ChannelDescription( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelDescription) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ChannelURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ContactAddress( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactAddress) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ContactPhone( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactPhone) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ContactEmail( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactEmail) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMChannelInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMChannelInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMChannelInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ChannelName )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ChannelDescription )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelDescription);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ChannelURL )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContactAddress )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactAddress);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContactPhone )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactPhone);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContactEmail )( 
            IAMChannelInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContactEmail);
        
        END_INTERFACE
    } IAMChannelInfoVtbl;

    interface IAMChannelInfo
    {
        CONST_VTBL struct IAMChannelInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMChannelInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMChannelInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMChannelInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMChannelInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMChannelInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMChannelInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMChannelInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMChannelInfo_get_ChannelName(This,pbstrChannelName)	\
    (This)->lpVtbl -> get_ChannelName(This,pbstrChannelName)

#define IAMChannelInfo_get_ChannelDescription(This,pbstrChannelDescription)	\
    (This)->lpVtbl -> get_ChannelDescription(This,pbstrChannelDescription)

#define IAMChannelInfo_get_ChannelURL(This,pbstrChannelURL)	\
    (This)->lpVtbl -> get_ChannelURL(This,pbstrChannelURL)

#define IAMChannelInfo_get_ContactAddress(This,pbstrContactAddress)	\
    (This)->lpVtbl -> get_ContactAddress(This,pbstrContactAddress)

#define IAMChannelInfo_get_ContactPhone(This,pbstrContactPhone)	\
    (This)->lpVtbl -> get_ContactPhone(This,pbstrContactPhone)

#define IAMChannelInfo_get_ContactEmail(This,pbstrContactEmail)	\
    (This)->lpVtbl -> get_ContactEmail(This,pbstrContactEmail)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMChannelInfo_get_ChannelName_Proxy( 
    IAMChannelInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelName);


void __RPC_STUB IAMChannelInfo_get_ChannelName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMChannelInfo_get_ChannelDescription_Proxy( 
    IAMChannelInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelDescription);


void __RPC_STUB IAMChannelInfo_get_ChannelDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMChannelInfo_get_ChannelURL_Proxy( 
    IAMChannelInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrChannelURL);


void __RPC_STUB IAMChannelInfo_get_ChannelURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMChannelInfo_get_ContactAddress_Proxy( 
    IAMChannelInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrContactAddress);


void __RPC_STUB IAMChannelInfo_get_ContactAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMChannelInfo_get_ContactPhone_Proxy( 
    IAMChannelInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrContactPhone);


void __RPC_STUB IAMChannelInfo_get_ContactPhone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMChannelInfo_get_ContactEmail_Proxy( 
    IAMChannelInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrContactEmail);


void __RPC_STUB IAMChannelInfo_get_ContactEmail_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMChannelInfo_INTERFACE_DEFINED__ */


#ifndef __IAMNetworkStatus_INTERFACE_DEFINED__
#define __IAMNetworkStatus_INTERFACE_DEFINED__

/* interface IAMNetworkStatus */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMNetworkStatus,0xFA2AA8F3,0x8B62,0x11d0,0xA5,0x20,0x00,0x00,0x00,0x00,0x00,0x00);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FA2AA8F3-8B62-11d0-A520-000000000000")
    IAMNetworkStatus : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ReceivedPackets( 
            /* [retval][out] */ long __RPC_FAR *pReceivedPackets) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_RecoveredPackets( 
            /* [retval][out] */ long __RPC_FAR *pRecoveredPackets) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LostPackets( 
            /* [retval][out] */ long __RPC_FAR *pLostPackets) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ReceptionQuality( 
            /* [retval][out] */ long __RPC_FAR *pReceptionQuality) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BufferingCount( 
            /* [retval][out] */ long __RPC_FAR *pBufferingCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsBroadcast( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsBroadcast) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BufferingProgress( 
            /* [retval][out] */ long __RPC_FAR *pBufferingProgress) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMNetworkStatusVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMNetworkStatus __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMNetworkStatus __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceivedPackets )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pReceivedPackets);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecoveredPackets )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pRecoveredPackets);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LostPackets )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pLostPackets);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceptionQuality )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pReceptionQuality);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingCount )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBufferingCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsBroadcast )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsBroadcast);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BufferingProgress )( 
            IAMNetworkStatus __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBufferingProgress);
        
        END_INTERFACE
    } IAMNetworkStatusVtbl;

    interface IAMNetworkStatus
    {
        CONST_VTBL struct IAMNetworkStatusVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMNetworkStatus_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMNetworkStatus_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMNetworkStatus_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMNetworkStatus_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMNetworkStatus_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMNetworkStatus_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMNetworkStatus_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMNetworkStatus_get_ReceivedPackets(This,pReceivedPackets)	\
    (This)->lpVtbl -> get_ReceivedPackets(This,pReceivedPackets)

#define IAMNetworkStatus_get_RecoveredPackets(This,pRecoveredPackets)	\
    (This)->lpVtbl -> get_RecoveredPackets(This,pRecoveredPackets)

#define IAMNetworkStatus_get_LostPackets(This,pLostPackets)	\
    (This)->lpVtbl -> get_LostPackets(This,pLostPackets)

#define IAMNetworkStatus_get_ReceptionQuality(This,pReceptionQuality)	\
    (This)->lpVtbl -> get_ReceptionQuality(This,pReceptionQuality)

#define IAMNetworkStatus_get_BufferingCount(This,pBufferingCount)	\
    (This)->lpVtbl -> get_BufferingCount(This,pBufferingCount)

#define IAMNetworkStatus_get_IsBroadcast(This,pIsBroadcast)	\
    (This)->lpVtbl -> get_IsBroadcast(This,pIsBroadcast)

#define IAMNetworkStatus_get_BufferingProgress(This,pBufferingProgress)	\
    (This)->lpVtbl -> get_BufferingProgress(This,pBufferingProgress)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetworkStatus_get_ReceivedPackets_Proxy( 
    IAMNetworkStatus __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pReceivedPackets);


void __RPC_STUB IAMNetworkStatus_get_ReceivedPackets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetworkStatus_get_RecoveredPackets_Proxy( 
    IAMNetworkStatus __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pRecoveredPackets);


void __RPC_STUB IAMNetworkStatus_get_RecoveredPackets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetworkStatus_get_LostPackets_Proxy( 
    IAMNetworkStatus __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pLostPackets);


void __RPC_STUB IAMNetworkStatus_get_LostPackets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetworkStatus_get_ReceptionQuality_Proxy( 
    IAMNetworkStatus __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pReceptionQuality);


void __RPC_STUB IAMNetworkStatus_get_ReceptionQuality_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetworkStatus_get_BufferingCount_Proxy( 
    IAMNetworkStatus __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBufferingCount);


void __RPC_STUB IAMNetworkStatus_get_BufferingCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetworkStatus_get_IsBroadcast_Proxy( 
    IAMNetworkStatus __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIsBroadcast);


void __RPC_STUB IAMNetworkStatus_get_IsBroadcast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetworkStatus_get_BufferingProgress_Proxy( 
    IAMNetworkStatus __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBufferingProgress);


void __RPC_STUB IAMNetworkStatus_get_BufferingProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMNetworkStatus_INTERFACE_DEFINED__ */


#ifndef __IAMExtendedSeeking_INTERFACE_DEFINED__
#define __IAMExtendedSeeking_INTERFACE_DEFINED__

/* interface IAMExtendedSeeking */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMExtendedSeeking,0xFA2AA8F9,0x8B62,0x11d0,0xA5,0x20,0x00,0x00,0x00,0x00,0x00,0x00);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FA2AA8F9-8B62-11d0-A520-000000000000")
    IAMExtendedSeeking : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ExSeekCapabilities( 
            /* [retval][out] */ long __RPC_FAR *pExCapabilities) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MarkerCount( 
            /* [retval][out] */ long __RPC_FAR *pMarkerCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentMarker( 
            /* [retval][out] */ long __RPC_FAR *pCurrentMarker) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMarkerTime( 
            /* [in] */ long MarkerNum,
            /* [retval][out] */ double __RPC_FAR *pMarkerTime) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMarkerName( 
            /* [in] */ long MarkerNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMarkerName) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_PlaybackSpeed( 
            /* [in] */ double Speed) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PlaybackSpeed( 
            /* [retval][out] */ double __RPC_FAR *pSpeed) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMExtendedSeekingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMExtendedSeeking __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMExtendedSeeking __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ExSeekCapabilities )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pExCapabilities);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MarkerCount )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pMarkerCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentMarker )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCurrentMarker);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMarkerTime )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [in] */ long MarkerNum,
            /* [retval][out] */ double __RPC_FAR *pMarkerTime);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMarkerName )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [in] */ long MarkerNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMarkerName);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlaybackSpeed )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [in] */ double Speed);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaybackSpeed )( 
            IAMExtendedSeeking __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pSpeed);
        
        END_INTERFACE
    } IAMExtendedSeekingVtbl;

    interface IAMExtendedSeeking
    {
        CONST_VTBL struct IAMExtendedSeekingVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMExtendedSeeking_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMExtendedSeeking_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMExtendedSeeking_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMExtendedSeeking_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMExtendedSeeking_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMExtendedSeeking_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMExtendedSeeking_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMExtendedSeeking_get_ExSeekCapabilities(This,pExCapabilities)	\
    (This)->lpVtbl -> get_ExSeekCapabilities(This,pExCapabilities)

#define IAMExtendedSeeking_get_MarkerCount(This,pMarkerCount)	\
    (This)->lpVtbl -> get_MarkerCount(This,pMarkerCount)

#define IAMExtendedSeeking_get_CurrentMarker(This,pCurrentMarker)	\
    (This)->lpVtbl -> get_CurrentMarker(This,pCurrentMarker)

#define IAMExtendedSeeking_GetMarkerTime(This,MarkerNum,pMarkerTime)	\
    (This)->lpVtbl -> GetMarkerTime(This,MarkerNum,pMarkerTime)

#define IAMExtendedSeeking_GetMarkerName(This,MarkerNum,pbstrMarkerName)	\
    (This)->lpVtbl -> GetMarkerName(This,MarkerNum,pbstrMarkerName)

#define IAMExtendedSeeking_put_PlaybackSpeed(This,Speed)	\
    (This)->lpVtbl -> put_PlaybackSpeed(This,Speed)

#define IAMExtendedSeeking_get_PlaybackSpeed(This,pSpeed)	\
    (This)->lpVtbl -> get_PlaybackSpeed(This,pSpeed)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMExtendedSeeking_get_ExSeekCapabilities_Proxy( 
    IAMExtendedSeeking __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pExCapabilities);


void __RPC_STUB IAMExtendedSeeking_get_ExSeekCapabilities_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMExtendedSeeking_get_MarkerCount_Proxy( 
    IAMExtendedSeeking __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pMarkerCount);


void __RPC_STUB IAMExtendedSeeking_get_MarkerCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMExtendedSeeking_get_CurrentMarker_Proxy( 
    IAMExtendedSeeking __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCurrentMarker);


void __RPC_STUB IAMExtendedSeeking_get_CurrentMarker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAMExtendedSeeking_GetMarkerTime_Proxy( 
    IAMExtendedSeeking __RPC_FAR * This,
    /* [in] */ long MarkerNum,
    /* [retval][out] */ double __RPC_FAR *pMarkerTime);


void __RPC_STUB IAMExtendedSeeking_GetMarkerTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAMExtendedSeeking_GetMarkerName_Proxy( 
    IAMExtendedSeeking __RPC_FAR * This,
    /* [in] */ long MarkerNum,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrMarkerName);


void __RPC_STUB IAMExtendedSeeking_GetMarkerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAMExtendedSeeking_put_PlaybackSpeed_Proxy( 
    IAMExtendedSeeking __RPC_FAR * This,
    /* [in] */ double Speed);


void __RPC_STUB IAMExtendedSeeking_put_PlaybackSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IAMExtendedSeeking_get_PlaybackSpeed_Proxy( 
    IAMExtendedSeeking __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pSpeed);


void __RPC_STUB IAMExtendedSeeking_get_PlaybackSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMExtendedSeeking_INTERFACE_DEFINED__ */


#ifndef __IAMNetShowExProps_INTERFACE_DEFINED__
#define __IAMNetShowExProps_INTERFACE_DEFINED__

/* interface IAMNetShowExProps */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMNetShowExProps,0xFA2AA8F5,0x8B62,0x11d0,0xA5,0x20,0x00,0x00,0x00,0x00,0x00,0x00);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FA2AA8F5-8B62-11d0-A520-000000000000")
    IAMNetShowExProps : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SourceProtocol( 
            /* [retval][out] */ long __RPC_FAR *pSourceProtocol) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Bandwidth( 
            /* [retval][out] */ long __RPC_FAR *pBandwidth) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorCorrection( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorCorrection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CodecCount( 
            /* [retval][out] */ long __RPC_FAR *pCodecCount) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCodecInstalled( 
            /* [in] */ long CodecNum,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCodecInstalled) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCodecDescription( 
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecDescription) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCodecURL( 
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CreationDate( 
            /* [retval][out] */ DATE __RPC_FAR *pCreationDate) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SourceLink( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSourceLink) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMNetShowExPropsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMNetShowExProps __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMNetShowExProps __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceProtocol )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceProtocol);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bandwidth )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBandwidth);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorCorrection )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorCorrection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CodecCount )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCodecCount);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCodecInstalled )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [in] */ long CodecNum,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCodecInstalled);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCodecDescription )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecDescription);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCodecURL )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [in] */ long CodecNum,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CreationDate )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pCreationDate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceLink )( 
            IAMNetShowExProps __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSourceLink);
        
        END_INTERFACE
    } IAMNetShowExPropsVtbl;

    interface IAMNetShowExProps
    {
        CONST_VTBL struct IAMNetShowExPropsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMNetShowExProps_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMNetShowExProps_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMNetShowExProps_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMNetShowExProps_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMNetShowExProps_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMNetShowExProps_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMNetShowExProps_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMNetShowExProps_get_SourceProtocol(This,pSourceProtocol)	\
    (This)->lpVtbl -> get_SourceProtocol(This,pSourceProtocol)

#define IAMNetShowExProps_get_Bandwidth(This,pBandwidth)	\
    (This)->lpVtbl -> get_Bandwidth(This,pBandwidth)

#define IAMNetShowExProps_get_ErrorCorrection(This,pbstrErrorCorrection)	\
    (This)->lpVtbl -> get_ErrorCorrection(This,pbstrErrorCorrection)

#define IAMNetShowExProps_get_CodecCount(This,pCodecCount)	\
    (This)->lpVtbl -> get_CodecCount(This,pCodecCount)

#define IAMNetShowExProps_GetCodecInstalled(This,CodecNum,pCodecInstalled)	\
    (This)->lpVtbl -> GetCodecInstalled(This,CodecNum,pCodecInstalled)

#define IAMNetShowExProps_GetCodecDescription(This,CodecNum,pbstrCodecDescription)	\
    (This)->lpVtbl -> GetCodecDescription(This,CodecNum,pbstrCodecDescription)

#define IAMNetShowExProps_GetCodecURL(This,CodecNum,pbstrCodecURL)	\
    (This)->lpVtbl -> GetCodecURL(This,CodecNum,pbstrCodecURL)

#define IAMNetShowExProps_get_CreationDate(This,pCreationDate)	\
    (This)->lpVtbl -> get_CreationDate(This,pCreationDate)

#define IAMNetShowExProps_get_SourceLink(This,pbstrSourceLink)	\
    (This)->lpVtbl -> get_SourceLink(This,pbstrSourceLink)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowExProps_get_SourceProtocol_Proxy( 
    IAMNetShowExProps __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceProtocol);


void __RPC_STUB IAMNetShowExProps_get_SourceProtocol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowExProps_get_Bandwidth_Proxy( 
    IAMNetShowExProps __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBandwidth);


void __RPC_STUB IAMNetShowExProps_get_Bandwidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowExProps_get_ErrorCorrection_Proxy( 
    IAMNetShowExProps __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorCorrection);


void __RPC_STUB IAMNetShowExProps_get_ErrorCorrection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowExProps_get_CodecCount_Proxy( 
    IAMNetShowExProps __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCodecCount);


void __RPC_STUB IAMNetShowExProps_get_CodecCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAMNetShowExProps_GetCodecInstalled_Proxy( 
    IAMNetShowExProps __RPC_FAR * This,
    /* [in] */ long CodecNum,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pCodecInstalled);


void __RPC_STUB IAMNetShowExProps_GetCodecInstalled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAMNetShowExProps_GetCodecDescription_Proxy( 
    IAMNetShowExProps __RPC_FAR * This,
    /* [in] */ long CodecNum,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecDescription);


void __RPC_STUB IAMNetShowExProps_GetCodecDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAMNetShowExProps_GetCodecURL_Proxy( 
    IAMNetShowExProps __RPC_FAR * This,
    /* [in] */ long CodecNum,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCodecURL);


void __RPC_STUB IAMNetShowExProps_GetCodecURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowExProps_get_CreationDate_Proxy( 
    IAMNetShowExProps __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *pCreationDate);


void __RPC_STUB IAMNetShowExProps_get_CreationDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowExProps_get_SourceLink_Proxy( 
    IAMNetShowExProps __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrSourceLink);


void __RPC_STUB IAMNetShowExProps_get_SourceLink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMNetShowExProps_INTERFACE_DEFINED__ */


#ifndef __IAMExtendedErrorInfo_INTERFACE_DEFINED__
#define __IAMExtendedErrorInfo_INTERFACE_DEFINED__

/* interface IAMExtendedErrorInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMExtendedErrorInfo,0xFA2AA8F6,0x8B62,0x11d0,0xA5,0x20,0x00,0x00,0x00,0x00,0x00,0x00);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FA2AA8F6-8B62-11d0-A520-000000000000")
    IAMExtendedErrorInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_HasError( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pHasError) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorDescription( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorDescription) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorCode( 
            /* [retval][out] */ long __RPC_FAR *pErrorCode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMExtendedErrorInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMExtendedErrorInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMExtendedErrorInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMExtendedErrorInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMExtendedErrorInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMExtendedErrorInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMExtendedErrorInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMExtendedErrorInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HasError )( 
            IAMExtendedErrorInfo __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pHasError);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorDescription )( 
            IAMExtendedErrorInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorDescription);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorCode )( 
            IAMExtendedErrorInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pErrorCode);
        
        END_INTERFACE
    } IAMExtendedErrorInfoVtbl;

    interface IAMExtendedErrorInfo
    {
        CONST_VTBL struct IAMExtendedErrorInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMExtendedErrorInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMExtendedErrorInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMExtendedErrorInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMExtendedErrorInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMExtendedErrorInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMExtendedErrorInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMExtendedErrorInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMExtendedErrorInfo_get_HasError(This,pHasError)	\
    (This)->lpVtbl -> get_HasError(This,pHasError)

#define IAMExtendedErrorInfo_get_ErrorDescription(This,pbstrErrorDescription)	\
    (This)->lpVtbl -> get_ErrorDescription(This,pbstrErrorDescription)

#define IAMExtendedErrorInfo_get_ErrorCode(This,pErrorCode)	\
    (This)->lpVtbl -> get_ErrorCode(This,pErrorCode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMExtendedErrorInfo_get_HasError_Proxy( 
    IAMExtendedErrorInfo __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pHasError);


void __RPC_STUB IAMExtendedErrorInfo_get_HasError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMExtendedErrorInfo_get_ErrorDescription_Proxy( 
    IAMExtendedErrorInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrErrorDescription);


void __RPC_STUB IAMExtendedErrorInfo_get_ErrorDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMExtendedErrorInfo_get_ErrorCode_Proxy( 
    IAMExtendedErrorInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pErrorCode);


void __RPC_STUB IAMExtendedErrorInfo_get_ErrorCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMExtendedErrorInfo_INTERFACE_DEFINED__ */


#ifndef __IAMMediaContent_INTERFACE_DEFINED__
#define __IAMMediaContent_INTERFACE_DEFINED__

/* interface IAMMediaContent */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMMediaContent,0xFA2AA8F4,0x8B62,0x11d0,0xA5,0x20,0x00,0x00,0x00,0x00,0x00,0x00);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FA2AA8F4-8B62-11d0-A520-000000000000")
    IAMMediaContent : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AuthorName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthorName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Title( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Rating( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Copyright( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BaseURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrBaseURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LogoURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLogoURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LogoIconURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLogoURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_WatermarkURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrWatermarkURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MoreInfoURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MoreInfoBannerImage( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoBannerImage) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MoreInfoBannerURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoBannerURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MoreInfoText( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoText) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMMediaContentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMMediaContent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMMediaContent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMMediaContent __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMMediaContent __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMMediaContent __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMMediaContent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMMediaContent __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthorName )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthorName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Copyright )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BaseURL )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrBaseURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogoURL )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLogoURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogoIconURL )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLogoURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WatermarkURL )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrWatermarkURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MoreInfoURL )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MoreInfoBannerImage )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoBannerImage);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MoreInfoBannerURL )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoBannerURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MoreInfoText )( 
            IAMMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoText);
        
        END_INTERFACE
    } IAMMediaContentVtbl;

    interface IAMMediaContent
    {
        CONST_VTBL struct IAMMediaContentVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMMediaContent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMMediaContent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMMediaContent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMMediaContent_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMMediaContent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMMediaContent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMMediaContent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMMediaContent_get_AuthorName(This,pbstrAuthorName)	\
    (This)->lpVtbl -> get_AuthorName(This,pbstrAuthorName)

#define IAMMediaContent_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define IAMMediaContent_get_Rating(This,pbstrRating)	\
    (This)->lpVtbl -> get_Rating(This,pbstrRating)

#define IAMMediaContent_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define IAMMediaContent_get_Copyright(This,pbstrCopyright)	\
    (This)->lpVtbl -> get_Copyright(This,pbstrCopyright)

#define IAMMediaContent_get_BaseURL(This,pbstrBaseURL)	\
    (This)->lpVtbl -> get_BaseURL(This,pbstrBaseURL)

#define IAMMediaContent_get_LogoURL(This,pbstrLogoURL)	\
    (This)->lpVtbl -> get_LogoURL(This,pbstrLogoURL)

#define IAMMediaContent_get_LogoIconURL(This,pbstrLogoURL)	\
    (This)->lpVtbl -> get_LogoIconURL(This,pbstrLogoURL)

#define IAMMediaContent_get_WatermarkURL(This,pbstrWatermarkURL)	\
    (This)->lpVtbl -> get_WatermarkURL(This,pbstrWatermarkURL)

#define IAMMediaContent_get_MoreInfoURL(This,pbstrMoreInfoURL)	\
    (This)->lpVtbl -> get_MoreInfoURL(This,pbstrMoreInfoURL)

#define IAMMediaContent_get_MoreInfoBannerImage(This,pbstrMoreInfoBannerImage)	\
    (This)->lpVtbl -> get_MoreInfoBannerImage(This,pbstrMoreInfoBannerImage)

#define IAMMediaContent_get_MoreInfoBannerURL(This,pbstrMoreInfoBannerURL)	\
    (This)->lpVtbl -> get_MoreInfoBannerURL(This,pbstrMoreInfoBannerURL)

#define IAMMediaContent_get_MoreInfoText(This,pbstrMoreInfoText)	\
    (This)->lpVtbl -> get_MoreInfoText(This,pbstrMoreInfoText)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_AuthorName_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthorName);


void __RPC_STUB IAMMediaContent_get_AuthorName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_Title_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);


void __RPC_STUB IAMMediaContent_get_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_Rating_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);


void __RPC_STUB IAMMediaContent_get_Rating_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_Description_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);


void __RPC_STUB IAMMediaContent_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_Copyright_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);


void __RPC_STUB IAMMediaContent_get_Copyright_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_BaseURL_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrBaseURL);


void __RPC_STUB IAMMediaContent_get_BaseURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_LogoURL_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLogoURL);


void __RPC_STUB IAMMediaContent_get_LogoURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_LogoIconURL_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLogoURL);


void __RPC_STUB IAMMediaContent_get_LogoIconURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_WatermarkURL_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrWatermarkURL);


void __RPC_STUB IAMMediaContent_get_WatermarkURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_MoreInfoURL_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoURL);


void __RPC_STUB IAMMediaContent_get_MoreInfoURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_MoreInfoBannerImage_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoBannerImage);


void __RPC_STUB IAMMediaContent_get_MoreInfoBannerImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_MoreInfoBannerURL_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoBannerURL);


void __RPC_STUB IAMMediaContent_get_MoreInfoBannerURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent_get_MoreInfoText_Proxy( 
    IAMMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoText);


void __RPC_STUB IAMMediaContent_get_MoreInfoText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMMediaContent_INTERFACE_DEFINED__ */


#ifndef __IAMMediaContent2_INTERFACE_DEFINED__
#define __IAMMediaContent2_INTERFACE_DEFINED__

/* interface IAMMediaContent2 */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMMediaContent2,0x817FF170,0xC535,0x11d2,0x9C,0x67,0x00,0xA0,0xC9,0x8C,0x04,0x78);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("817FF170-C535-11d2-9C67-00A0C98C0478")
    IAMMediaContent2 : public IAMMediaContent
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MoreInfoBannerBitmap( 
            /* [retval][out] */ LONG __RPC_FAR *pMoreInfoBannerBitmap) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMMediaContent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMMediaContent2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMMediaContent2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthorName )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthorName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTitle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrRating);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Copyright )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCopyright);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BaseURL )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrBaseURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogoURL )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLogoURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogoIconURL )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLogoURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WatermarkURL )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrWatermarkURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MoreInfoURL )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MoreInfoBannerImage )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoBannerImage);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MoreInfoBannerURL )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoBannerURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MoreInfoText )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMoreInfoText);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MoreInfoBannerBitmap )( 
            IAMMediaContent2 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pMoreInfoBannerBitmap);
        
        END_INTERFACE
    } IAMMediaContent2Vtbl;

    interface IAMMediaContent2
    {
        CONST_VTBL struct IAMMediaContent2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMMediaContent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMMediaContent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMMediaContent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMMediaContent2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMMediaContent2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMMediaContent2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMMediaContent2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMMediaContent2_get_AuthorName(This,pbstrAuthorName)	\
    (This)->lpVtbl -> get_AuthorName(This,pbstrAuthorName)

#define IAMMediaContent2_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define IAMMediaContent2_get_Rating(This,pbstrRating)	\
    (This)->lpVtbl -> get_Rating(This,pbstrRating)

#define IAMMediaContent2_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define IAMMediaContent2_get_Copyright(This,pbstrCopyright)	\
    (This)->lpVtbl -> get_Copyright(This,pbstrCopyright)

#define IAMMediaContent2_get_BaseURL(This,pbstrBaseURL)	\
    (This)->lpVtbl -> get_BaseURL(This,pbstrBaseURL)

#define IAMMediaContent2_get_LogoURL(This,pbstrLogoURL)	\
    (This)->lpVtbl -> get_LogoURL(This,pbstrLogoURL)

#define IAMMediaContent2_get_LogoIconURL(This,pbstrLogoURL)	\
    (This)->lpVtbl -> get_LogoIconURL(This,pbstrLogoURL)

#define IAMMediaContent2_get_WatermarkURL(This,pbstrWatermarkURL)	\
    (This)->lpVtbl -> get_WatermarkURL(This,pbstrWatermarkURL)

#define IAMMediaContent2_get_MoreInfoURL(This,pbstrMoreInfoURL)	\
    (This)->lpVtbl -> get_MoreInfoURL(This,pbstrMoreInfoURL)

#define IAMMediaContent2_get_MoreInfoBannerImage(This,pbstrMoreInfoBannerImage)	\
    (This)->lpVtbl -> get_MoreInfoBannerImage(This,pbstrMoreInfoBannerImage)

#define IAMMediaContent2_get_MoreInfoBannerURL(This,pbstrMoreInfoBannerURL)	\
    (This)->lpVtbl -> get_MoreInfoBannerURL(This,pbstrMoreInfoBannerURL)

#define IAMMediaContent2_get_MoreInfoText(This,pbstrMoreInfoText)	\
    (This)->lpVtbl -> get_MoreInfoText(This,pbstrMoreInfoText)


#define IAMMediaContent2_get_MoreInfoBannerBitmap(This,pMoreInfoBannerBitmap)	\
    (This)->lpVtbl -> get_MoreInfoBannerBitmap(This,pMoreInfoBannerBitmap)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContent2_get_MoreInfoBannerBitmap_Proxy( 
    IAMMediaContent2 __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pMoreInfoBannerBitmap);


void __RPC_STUB IAMMediaContent2_get_MoreInfoBannerBitmap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMMediaContent2_INTERFACE_DEFINED__ */


#ifndef __IAMMediaContentEx_INTERFACE_DEFINED__
#define __IAMMediaContentEx_INTERFACE_DEFINED__

/* interface IAMMediaContentEx */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMMediaContentEx,0xCE8F78C1,0x74D9,0x11d2,0xB0,0x9D,0x00,0xA0,0xC9,0xA8,0x11,0x17);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE8F78C1-74D9-11d2-B09D-00A0C9A81117")
    IAMMediaContentEx : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MediaParameter( 
            /* [in] */ long EntryNum,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrValue) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MediaParameterName( 
            /* [in] */ long EntryNum,
            /* [in] */ long Index,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlaylistCount( 
            /* [retval][out] */ long __RPC_FAR *pNumberEntries) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMMediaContentExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMMediaContentEx __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMMediaContentEx __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMMediaContentEx __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMMediaContentEx __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMMediaContentEx __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMMediaContentEx __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMMediaContentEx __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaParameter )( 
            IAMMediaContentEx __RPC_FAR * This,
            /* [in] */ long EntryNum,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrValue);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaParameterName )( 
            IAMMediaContentEx __RPC_FAR * This,
            /* [in] */ long EntryNum,
            /* [in] */ long Index,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistCount )( 
            IAMMediaContentEx __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pNumberEntries);
        
        END_INTERFACE
    } IAMMediaContentExVtbl;

    interface IAMMediaContentEx
    {
        CONST_VTBL struct IAMMediaContentExVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMMediaContentEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMMediaContentEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMMediaContentEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMMediaContentEx_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMMediaContentEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMMediaContentEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMMediaContentEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMMediaContentEx_get_MediaParameter(This,EntryNum,bstrName,pbstrValue)	\
    (This)->lpVtbl -> get_MediaParameter(This,EntryNum,bstrName,pbstrValue)

#define IAMMediaContentEx_get_MediaParameterName(This,EntryNum,Index,pbstrName)	\
    (This)->lpVtbl -> get_MediaParameterName(This,EntryNum,Index,pbstrName)

#define IAMMediaContentEx_get_PlaylistCount(This,pNumberEntries)	\
    (This)->lpVtbl -> get_PlaylistCount(This,pNumberEntries)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContentEx_get_MediaParameter_Proxy( 
    IAMMediaContentEx __RPC_FAR * This,
    /* [in] */ long EntryNum,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrValue);


void __RPC_STUB IAMMediaContentEx_get_MediaParameter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContentEx_get_MediaParameterName_Proxy( 
    IAMMediaContentEx __RPC_FAR * This,
    /* [in] */ long EntryNum,
    /* [in] */ long Index,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB IAMMediaContentEx_get_MediaParameterName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMMediaContentEx_get_PlaylistCount_Proxy( 
    IAMMediaContentEx __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pNumberEntries);


void __RPC_STUB IAMMediaContentEx_get_PlaylistCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMMediaContentEx_INTERFACE_DEFINED__ */


#ifndef __IAMNetShowPreroll_INTERFACE_DEFINED__
#define __IAMNetShowPreroll_INTERFACE_DEFINED__

/* interface IAMNetShowPreroll */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMNetShowPreroll,0xAAE7E4E2,0x6388,0x11D1,0x8D,0x93,0x00,0x60,0x97,0xC9,0xA2,0xB2);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AAE7E4E2-6388-11D1-8D93-006097C9A2B2")
    IAMNetShowPreroll : public IDispatch
    {
    public:
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Preroll( 
            /* [in] */ VARIANT_BOOL fPreroll) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Preroll( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfPreroll) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMNetShowPrerollVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMNetShowPreroll __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMNetShowPreroll __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMNetShowPreroll __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMNetShowPreroll __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMNetShowPreroll __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMNetShowPreroll __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMNetShowPreroll __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Preroll )( 
            IAMNetShowPreroll __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fPreroll);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Preroll )( 
            IAMNetShowPreroll __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfPreroll);
        
        END_INTERFACE
    } IAMNetShowPrerollVtbl;

    interface IAMNetShowPreroll
    {
        CONST_VTBL struct IAMNetShowPrerollVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMNetShowPreroll_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMNetShowPreroll_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMNetShowPreroll_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMNetShowPreroll_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMNetShowPreroll_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMNetShowPreroll_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMNetShowPreroll_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMNetShowPreroll_put_Preroll(This,fPreroll)	\
    (This)->lpVtbl -> put_Preroll(This,fPreroll)

#define IAMNetShowPreroll_get_Preroll(This,pfPreroll)	\
    (This)->lpVtbl -> get_Preroll(This,pfPreroll)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAMNetShowPreroll_put_Preroll_Proxy( 
    IAMNetShowPreroll __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fPreroll);


void __RPC_STUB IAMNetShowPreroll_put_Preroll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IAMNetShowPreroll_get_Preroll_Proxy( 
    IAMNetShowPreroll __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfPreroll);


void __RPC_STUB IAMNetShowPreroll_get_Preroll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMNetShowPreroll_INTERFACE_DEFINED__ */


#ifndef __IDShowPlugin_INTERFACE_DEFINED__
#define __IDShowPlugin_INTERFACE_DEFINED__

/* interface IDShowPlugin */
/* [object][helpstring][uuid] */ 


DEFINE_GUID(IID_IDShowPlugin,0x4746B7C8,0x700E,0x11d1,0xBE,0xCC,0x00,0xC0,0x4F,0xB6,0xE9,0x37);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4746B7C8-700E-11d1-BECC-00C04FB6E937")
    IDShowPlugin : public IUnknown
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_URL( 
            /* [retval][out] */ BSTR __RPC_FAR *pURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UserAgent( 
            /* [retval][out] */ BSTR __RPC_FAR *pUserAgent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDShowPluginVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDShowPlugin __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDShowPlugin __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDShowPlugin __RPC_FAR * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_URL )( 
            IDShowPlugin __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UserAgent )( 
            IDShowPlugin __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pUserAgent);
        
        END_INTERFACE
    } IDShowPluginVtbl;

    interface IDShowPlugin
    {
        CONST_VTBL struct IDShowPluginVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDShowPlugin_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDShowPlugin_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDShowPlugin_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDShowPlugin_get_URL(This,pURL)	\
    (This)->lpVtbl -> get_URL(This,pURL)

#define IDShowPlugin_get_UserAgent(This,pUserAgent)	\
    (This)->lpVtbl -> get_UserAgent(This,pUserAgent)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IDShowPlugin_get_URL_Proxy( 
    IDShowPlugin __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pURL);


void __RPC_STUB IDShowPlugin_get_URL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IDShowPlugin_get_UserAgent_Proxy( 
    IDShowPlugin __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pUserAgent);


void __RPC_STUB IDShowPlugin_get_UserAgent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDShowPlugin_INTERFACE_DEFINED__ */


#ifndef __IAMSecureMediaContent_INTERFACE_DEFINED__
#define __IAMSecureMediaContent_INTERFACE_DEFINED__

/* interface IAMSecureMediaContent */
/* [object][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMSecureMediaContent,0x36F0A694,0xC536,0x11d2,0x9C,0x67,0x00,0xA0,0xC9,0x8C,0x04,0x78);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("36F0A694-C536-11d2-9C67-00A0C98C0478")
    IAMSecureMediaContent : public IUnknown
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsSecure( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsSecure) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Tooltip( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTooltip) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMSecureMediaContentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMSecureMediaContent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMSecureMediaContent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMSecureMediaContent __RPC_FAR * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsSecure )( 
            IAMSecureMediaContent __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsSecure);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Tooltip )( 
            IAMSecureMediaContent __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTooltip);
        
        END_INTERFACE
    } IAMSecureMediaContentVtbl;

    interface IAMSecureMediaContent
    {
        CONST_VTBL struct IAMSecureMediaContentVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMSecureMediaContent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMSecureMediaContent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMSecureMediaContent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMSecureMediaContent_get_IsSecure(This,pfIsSecure)	\
    (This)->lpVtbl -> get_IsSecure(This,pfIsSecure)

#define IAMSecureMediaContent_get_Tooltip(This,pbstrTooltip)	\
    (This)->lpVtbl -> get_Tooltip(This,pbstrTooltip)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMSecureMediaContent_get_IsSecure_Proxy( 
    IAMSecureMediaContent __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsSecure);


void __RPC_STUB IAMSecureMediaContent_get_IsSecure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAMSecureMediaContent_get_Tooltip_Proxy( 
    IAMSecureMediaContent __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrTooltip);


void __RPC_STUB IAMSecureMediaContent_get_Tooltip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMSecureMediaContent_INTERFACE_DEFINED__ */

#endif /* __QuartzNetTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


