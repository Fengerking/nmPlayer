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
/* at Thu Jun 24 19:41:57 2004
 */
/* Compiler settings for .\dvdata.idl:
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

#ifndef __dvdata_h__
#define __dvdata_h__

/* Forward Declarations */ 

#ifndef __IDVDROM_FWD_DEFINED__
#define __IDVDROM_FWD_DEFINED__
typedef interface IDVDROM IDVDROM;
#endif 	/* __IDVDROM_FWD_DEFINED__ */


#ifndef __IDVDParentalManagementTable_FWD_DEFINED__
#define __IDVDParentalManagementTable_FWD_DEFINED__
typedef interface IDVDParentalManagementTable IDVDParentalManagementTable;
#endif 	/* __IDVDParentalManagementTable_FWD_DEFINED__ */


#ifndef __IDVDTextManager_FWD_DEFINED__
#define __IDVDTextManager_FWD_DEFINED__
typedef interface IDVDTextManager IDVDTextManager;
#endif 	/* __IDVDTextManager_FWD_DEFINED__ */


#ifndef __IDVDStreamAttributes_FWD_DEFINED__
#define __IDVDStreamAttributes_FWD_DEFINED__
typedef interface IDVDStreamAttributes IDVDStreamAttributes;
#endif 	/* __IDVDStreamAttributes_FWD_DEFINED__ */


#ifndef __IDVDDomain_FWD_DEFINED__
#define __IDVDDomain_FWD_DEFINED__
typedef interface IDVDDomain IDVDDomain;
#endif 	/* __IDVDDomain_FWD_DEFINED__ */


#ifndef __IDVDMenu_FWD_DEFINED__
#define __IDVDMenu_FWD_DEFINED__
typedef interface IDVDMenu IDVDMenu;
#endif 	/* __IDVDMenu_FWD_DEFINED__ */


#ifndef __IEnumDVD_CELL_PIECE_FWD_DEFINED__
#define __IEnumDVD_CELL_PIECE_FWD_DEFINED__
typedef interface IEnumDVD_CELL_PIECE IEnumDVD_CELL_PIECE;
#endif 	/* __IEnumDVD_CELL_PIECE_FWD_DEFINED__ */


#ifndef __IEnumDVD_TEXT_LANGUAGE_UNIT_FWD_DEFINED__
#define __IEnumDVD_TEXT_LANGUAGE_UNIT_FWD_DEFINED__
typedef interface IEnumDVD_TEXT_LANGUAGE_UNIT IEnumDVD_TEXT_LANGUAGE_UNIT;
#endif 	/* __IEnumDVD_TEXT_LANGUAGE_UNIT_FWD_DEFINED__ */


#ifndef __IEnumDVD_TEXT_ITEM_FWD_DEFINED__
#define __IEnumDVD_TEXT_ITEM_FWD_DEFINED__
typedef interface IEnumDVD_TEXT_ITEM IEnumDVD_TEXT_ITEM;
#endif 	/* __IEnumDVD_TEXT_ITEM_FWD_DEFINED__ */


#ifndef __IDVDTitleSet_FWD_DEFINED__
#define __IDVDTitleSet_FWD_DEFINED__
typedef interface IDVDTitleSet IDVDTitleSet;
#endif 	/* __IDVDTitleSet_FWD_DEFINED__ */


#ifndef __IDVDProgramChain_FWD_DEFINED__
#define __IDVDProgramChain_FWD_DEFINED__
typedef interface IDVDProgramChain IDVDProgramChain;
#endif 	/* __IDVDProgramChain_FWD_DEFINED__ */


#ifndef __IDVDTimeMap_FWD_DEFINED__
#define __IDVDTimeMap_FWD_DEFINED__
typedef interface IDVDTimeMap IDVDTimeMap;
#endif 	/* __IDVDTimeMap_FWD_DEFINED__ */


#ifndef __IDVDVideoVolume_FWD_DEFINED__
#define __IDVDVideoVolume_FWD_DEFINED__
typedef interface IDVDVideoVolume IDVDVideoVolume;
#endif 	/* __IDVDVideoVolume_FWD_DEFINED__ */


#ifndef __IDVDDataSearchInfo_FWD_DEFINED__
#define __IDVDDataSearchInfo_FWD_DEFINED__
typedef interface IDVDDataSearchInfo IDVDDataSearchInfo;
#endif 	/* __IDVDDataSearchInfo_FWD_DEFINED__ */


#ifndef __IDVDHighlightInfo_FWD_DEFINED__
#define __IDVDHighlightInfo_FWD_DEFINED__
typedef interface IDVDHighlightInfo IDVDHighlightInfo;
#endif 	/* __IDVDHighlightInfo_FWD_DEFINED__ */


#ifndef __IDVDStreamControlInfo_FWD_DEFINED__
#define __IDVDStreamControlInfo_FWD_DEFINED__
typedef interface IDVDStreamControlInfo IDVDStreamControlInfo;
#endif 	/* __IDVDStreamControlInfo_FWD_DEFINED__ */


#ifndef __IDVDNavDataStatusSink_FWD_DEFINED__
#define __IDVDNavDataStatusSink_FWD_DEFINED__
typedef interface IDVDNavDataStatusSink IDVDNavDataStatusSink;
#endif 	/* __IDVDNavDataStatusSink_FWD_DEFINED__ */


#ifndef __DVDData_FWD_DEFINED__
#define __DVDData_FWD_DEFINED__

#ifdef __cplusplus
typedef class DVDData DVDData;
#else
typedef struct DVDData DVDData;
#endif /* __cplusplus */

#endif 	/* __DVDData_FWD_DEFINED__ */


#ifndef __DVDStreamControlInfo_FWD_DEFINED__
#define __DVDStreamControlInfo_FWD_DEFINED__

#ifdef __cplusplus
typedef class DVDStreamControlInfo DVDStreamControlInfo;
#else
typedef struct DVDStreamControlInfo DVDStreamControlInfo;
#endif /* __cplusplus */

#endif 	/* __DVDStreamControlInfo_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "dvdcom.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_dvdata_0000 */
/* [local] */ 

typedef /* [string] */ WCHAR __RPC_FAR UNICODE_PATH[ 128 ];



extern RPC_IF_HANDLE __MIDL_itf_dvdata_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dvdata_0000_v0_0_s_ifspec;

#ifndef __IDVDROM_INTERFACE_DEFINED__
#define __IDVDROM_INTERFACE_DEFINED__

/* interface IDVDROM */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDROM;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5072-9F3D-11D0-A979-00A0C9050598")
    IDVDROM : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Bind( 
            UNICODE_PATH path) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Unbind( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReadVMGI( 
            /* [out][ref] */ UINT32 __RPC_FAR *length,
            /* [out][ref][full] */ UINT8 __RPC_FAR *__RPC_FAR *bufPtr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReadVTSI( 
            UINT16 zbVTSN,
            UINT32 __RPC_FAR *length,
            UINT8 __RPC_FAR *__RPC_FAR *bufPtr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReleaseMem( 
            /* [full][in][ref] */ UINT8 __RPC_FAR *bufPtr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OpenVOB( 
            UINT16 __RPC_FAR *hVOB,
            INT16 obVTSN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReadVOB( 
            UINT16 hVOB,
            UINT8 __RPC_FAR *buf,
            UINT32 startBlock,
            UINT32 endBlock) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CloseVOB( 
            UINT16 hVOB) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDROMVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDROM __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDROM __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDROM __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Bind )( 
            IDVDROM __RPC_FAR * This,
            UNICODE_PATH path);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Unbind )( 
            IDVDROM __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadVMGI )( 
            IDVDROM __RPC_FAR * This,
            /* [out][ref] */ UINT32 __RPC_FAR *length,
            /* [out][ref][full] */ UINT8 __RPC_FAR *__RPC_FAR *bufPtr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadVTSI )( 
            IDVDROM __RPC_FAR * This,
            UINT16 zbVTSN,
            UINT32 __RPC_FAR *length,
            UINT8 __RPC_FAR *__RPC_FAR *bufPtr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReleaseMem )( 
            IDVDROM __RPC_FAR * This,
            /* [full][in][ref] */ UINT8 __RPC_FAR *bufPtr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenVOB )( 
            IDVDROM __RPC_FAR * This,
            UINT16 __RPC_FAR *hVOB,
            INT16 obVTSN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadVOB )( 
            IDVDROM __RPC_FAR * This,
            UINT16 hVOB,
            UINT8 __RPC_FAR *buf,
            UINT32 startBlock,
            UINT32 endBlock);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CloseVOB )( 
            IDVDROM __RPC_FAR * This,
            UINT16 hVOB);
        
        END_INTERFACE
    } IDVDROMVtbl;

    interface IDVDROM
    {
        CONST_VTBL struct IDVDROMVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDROM_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDROM_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDROM_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDROM_Bind(This,path)	\
    (This)->lpVtbl -> Bind(This,path)

#define IDVDROM_Unbind(This)	\
    (This)->lpVtbl -> Unbind(This)

#define IDVDROM_ReadVMGI(This,length,bufPtr)	\
    (This)->lpVtbl -> ReadVMGI(This,length,bufPtr)

#define IDVDROM_ReadVTSI(This,zbVTSN,length,bufPtr)	\
    (This)->lpVtbl -> ReadVTSI(This,zbVTSN,length,bufPtr)

#define IDVDROM_ReleaseMem(This,bufPtr)	\
    (This)->lpVtbl -> ReleaseMem(This,bufPtr)

#define IDVDROM_OpenVOB(This,hVOB,obVTSN)	\
    (This)->lpVtbl -> OpenVOB(This,hVOB,obVTSN)

#define IDVDROM_ReadVOB(This,hVOB,buf,startBlock,endBlock)	\
    (This)->lpVtbl -> ReadVOB(This,hVOB,buf,startBlock,endBlock)

#define IDVDROM_CloseVOB(This,hVOB)	\
    (This)->lpVtbl -> CloseVOB(This,hVOB)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDROM_Bind_Proxy( 
    IDVDROM __RPC_FAR * This,
    UNICODE_PATH path);


void __RPC_STUB IDVDROM_Bind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDROM_Unbind_Proxy( 
    IDVDROM __RPC_FAR * This);


void __RPC_STUB IDVDROM_Unbind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDROM_ReadVMGI_Proxy( 
    IDVDROM __RPC_FAR * This,
    /* [out][ref] */ UINT32 __RPC_FAR *length,
    /* [out][ref][full] */ UINT8 __RPC_FAR *__RPC_FAR *bufPtr);


void __RPC_STUB IDVDROM_ReadVMGI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDROM_ReadVTSI_Proxy( 
    IDVDROM __RPC_FAR * This,
    UINT16 zbVTSN,
    UINT32 __RPC_FAR *length,
    UINT8 __RPC_FAR *__RPC_FAR *bufPtr);


void __RPC_STUB IDVDROM_ReadVTSI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDROM_ReleaseMem_Proxy( 
    IDVDROM __RPC_FAR * This,
    /* [full][in][ref] */ UINT8 __RPC_FAR *bufPtr);


void __RPC_STUB IDVDROM_ReleaseMem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDROM_OpenVOB_Proxy( 
    IDVDROM __RPC_FAR * This,
    UINT16 __RPC_FAR *hVOB,
    INT16 obVTSN);


void __RPC_STUB IDVDROM_OpenVOB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDROM_ReadVOB_Proxy( 
    IDVDROM __RPC_FAR * This,
    UINT16 hVOB,
    UINT8 __RPC_FAR *buf,
    UINT32 startBlock,
    UINT32 endBlock);


void __RPC_STUB IDVDROM_ReadVOB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDROM_CloseVOB_Proxy( 
    IDVDROM __RPC_FAR * This,
    UINT16 hVOB);


void __RPC_STUB IDVDROM_CloseVOB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDROM_INTERFACE_DEFINED__ */


#ifndef __IDVDParentalManagementTable_INTERFACE_DEFINED__
#define __IDVDParentalManagementTable_INTERFACE_DEFINED__

/* interface IDVDParentalManagementTable */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDParentalManagementTable;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5074-9F3D-11D0-A979-00A0C9050598")
    IDVDParentalManagementTable : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ConvertParentalLevelToID( 
            /* [in] */ UINT8 level,
            /* [in] */ UINT16 countryCode,
            /* [in] */ UINT16 vtsn,
            /* [ref][out] */ UINT16 __RPC_FAR *parentalID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDParentalManagementTableVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDParentalManagementTable __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDParentalManagementTable __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDParentalManagementTable __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertParentalLevelToID )( 
            IDVDParentalManagementTable __RPC_FAR * This,
            /* [in] */ UINT8 level,
            /* [in] */ UINT16 countryCode,
            /* [in] */ UINT16 vtsn,
            /* [ref][out] */ UINT16 __RPC_FAR *parentalID);
        
        END_INTERFACE
    } IDVDParentalManagementTableVtbl;

    interface IDVDParentalManagementTable
    {
        CONST_VTBL struct IDVDParentalManagementTableVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDParentalManagementTable_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDParentalManagementTable_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDParentalManagementTable_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDParentalManagementTable_ConvertParentalLevelToID(This,level,countryCode,vtsn,parentalID)	\
    (This)->lpVtbl -> ConvertParentalLevelToID(This,level,countryCode,vtsn,parentalID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDParentalManagementTable_ConvertParentalLevelToID_Proxy( 
    IDVDParentalManagementTable __RPC_FAR * This,
    /* [in] */ UINT8 level,
    /* [in] */ UINT16 countryCode,
    /* [in] */ UINT16 vtsn,
    /* [ref][out] */ UINT16 __RPC_FAR *parentalID);


void __RPC_STUB IDVDParentalManagementTable_ConvertParentalLevelToID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDParentalManagementTable_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_dvdata_0084 */
/* [local] */ 





extern RPC_IF_HANDLE __MIDL_itf_dvdata_0084_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dvdata_0084_v0_0_s_ifspec;

#ifndef __IDVDTextManager_INTERFACE_DEFINED__
#define __IDVDTextManager_INTERFACE_DEFINED__

/* interface IDVDTextManager */
/* [unique][helpstring][uuid][object] */ 

typedef struct _structDVD_TEXT_LANGUAGE_UNIT
    {
    UINT16 tdlc;
    UINT8 charset;
    }	DVD_TEXT_LANGUAGE_UNIT;

typedef struct _structDVD_TEXT_LANGUAGE_UNIT __RPC_FAR *PDVD_TEXT_LANGUAGE_UNIT;

typedef struct _structDVD_TEXT_ITEM
    {
    UINT8 __RPC_FAR *pTextData;
    UINT8 itemCode;
    }	DVD_TEXT_ITEM;

typedef struct _structDVD_TEXT_ITEM __RPC_FAR *PDVD_TEXT_ITEM;


EXTERN_C const IID IID_IDVDTextManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5077-9F3D-11D0-A979-00A0C9050598")
    IDVDTextManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumerateTextLanguageUnits( 
            /* [ref][out] */ IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTextItemsCount( 
            /* [ref][in] */ DVD_TEXT_LANGUAGE_UNIT __RPC_FAR *pTLU,
            /* [in] */ UINT8 title,
            /* [ref][out] */ UINT16 __RPC_FAR *pCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumerateTitleTextItems( 
            /* [ref][in] */ DVD_TEXT_LANGUAGE_UNIT __RPC_FAR *pTLU,
            /* [in] */ UINT8 title,
            /* [ref][out] */ IEnumDVD_TEXT_ITEM __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDTextManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDTextManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDTextManager __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDTextManager __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumerateTextLanguageUnits )( 
            IDVDTextManager __RPC_FAR * This,
            /* [ref][out] */ IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR *__RPC_FAR *ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTextItemsCount )( 
            IDVDTextManager __RPC_FAR * This,
            /* [ref][in] */ DVD_TEXT_LANGUAGE_UNIT __RPC_FAR *pTLU,
            /* [in] */ UINT8 title,
            /* [ref][out] */ UINT16 __RPC_FAR *pCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumerateTitleTextItems )( 
            IDVDTextManager __RPC_FAR * This,
            /* [ref][in] */ DVD_TEXT_LANGUAGE_UNIT __RPC_FAR *pTLU,
            /* [in] */ UINT8 title,
            /* [ref][out] */ IEnumDVD_TEXT_ITEM __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } IDVDTextManagerVtbl;

    interface IDVDTextManager
    {
        CONST_VTBL struct IDVDTextManagerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDTextManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDTextManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDTextManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDTextManager_EnumerateTextLanguageUnits(This,ppEnum)	\
    (This)->lpVtbl -> EnumerateTextLanguageUnits(This,ppEnum)

#define IDVDTextManager_GetTextItemsCount(This,pTLU,title,pCount)	\
    (This)->lpVtbl -> GetTextItemsCount(This,pTLU,title,pCount)

#define IDVDTextManager_EnumerateTitleTextItems(This,pTLU,title,ppEnum)	\
    (This)->lpVtbl -> EnumerateTitleTextItems(This,pTLU,title,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDTextManager_EnumerateTextLanguageUnits_Proxy( 
    IDVDTextManager __RPC_FAR * This,
    /* [ref][out] */ IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IDVDTextManager_EnumerateTextLanguageUnits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTextManager_GetTextItemsCount_Proxy( 
    IDVDTextManager __RPC_FAR * This,
    /* [ref][in] */ DVD_TEXT_LANGUAGE_UNIT __RPC_FAR *pTLU,
    /* [in] */ UINT8 title,
    /* [ref][out] */ UINT16 __RPC_FAR *pCount);


void __RPC_STUB IDVDTextManager_GetTextItemsCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTextManager_EnumerateTitleTextItems_Proxy( 
    IDVDTextManager __RPC_FAR * This,
    /* [ref][in] */ DVD_TEXT_LANGUAGE_UNIT __RPC_FAR *pTLU,
    /* [in] */ UINT8 title,
    /* [ref][out] */ IEnumDVD_TEXT_ITEM __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IDVDTextManager_EnumerateTitleTextItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDTextManager_INTERFACE_DEFINED__ */


#ifndef __IDVDStreamAttributes_INTERFACE_DEFINED__
#define __IDVDStreamAttributes_INTERFACE_DEFINED__

/* interface IDVDStreamAttributes */
/* [unique][helpstring][uuid][object] */ 

typedef struct _structDVD_AUDIO_STREAM_ATTR
    {
    UINT16 attr;
    UINT16 reserved[ 3 ];
    }	DVD_AUDIO_STREAM_ATTR;

typedef struct _structDVD_AUDIO_STREAM_ATTR __RPC_FAR *PDVD_AUDIO_STREAM_ATTR;

typedef struct _structDVD_AUDIO_MU_ATTR
    {
    UINT8 atr1[ 8 ];
    struct 
        {
        UINT8 alpha;
        UINT8 beta;
        }	atr2[ 8 ];
    }	DVD_AUDIO_MU_ATTR;

typedef struct _structDVD_AUDIO_MU_ATTR __RPC_FAR *PDVD_AUDIO_MU_ATTR;

typedef struct _structDVD_SUBPICTURE_STREAM_ATTR
    {
    UINT16 modetype;
    UINT16 specificCode;
    UINT16 extension;
    }	DVD_SUBPICTURE_STREAM_ATTR;

typedef struct _structDVD_SUBPICTURE_STREAM_ATTR __RPC_FAR *PDVD_SUBPICTURE_STREAM_ATTR;


EXTERN_C const IID IID_IDVDStreamAttributes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5078-9F3D-11D0-A979-00A0C9050598")
    IDVDStreamAttributes : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetVideoAttr( 
            /* [ref][out] */ UINT16 __RPC_FAR *pVideoAttr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAudioCount( 
            /* [ref][out] */ UINT8 __RPC_FAR *pStreamCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAudioAttr( 
            /* [in] */ UINT nStreamNo,
            /* [ref][out] */ DVD_AUDIO_STREAM_ATTR __RPC_FAR *pAudioAttr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAudioMultichannelUseAttr( 
            /* [in] */ UINT8 nStreamNo,
            /* [ref][out] */ DVD_AUDIO_MU_ATTR __RPC_FAR *pMuse) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSubpictureCount( 
            /* [ref][out] */ UINT8 __RPC_FAR *pStreamCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSubpictureAttr( 
            /* [in] */ UINT8 nStreamNo,
            /* [ref][out] */ DVD_SUBPICTURE_STREAM_ATTR __RPC_FAR *pSPAttr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDStreamAttributesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDStreamAttributes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDStreamAttributes __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDStreamAttributes __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoAttr )( 
            IDVDStreamAttributes __RPC_FAR * This,
            /* [ref][out] */ UINT16 __RPC_FAR *pVideoAttr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAudioCount )( 
            IDVDStreamAttributes __RPC_FAR * This,
            /* [ref][out] */ UINT8 __RPC_FAR *pStreamCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAudioAttr )( 
            IDVDStreamAttributes __RPC_FAR * This,
            /* [in] */ UINT nStreamNo,
            /* [ref][out] */ DVD_AUDIO_STREAM_ATTR __RPC_FAR *pAudioAttr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAudioMultichannelUseAttr )( 
            IDVDStreamAttributes __RPC_FAR * This,
            /* [in] */ UINT8 nStreamNo,
            /* [ref][out] */ DVD_AUDIO_MU_ATTR __RPC_FAR *pMuse);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubpictureCount )( 
            IDVDStreamAttributes __RPC_FAR * This,
            /* [ref][out] */ UINT8 __RPC_FAR *pStreamCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubpictureAttr )( 
            IDVDStreamAttributes __RPC_FAR * This,
            /* [in] */ UINT8 nStreamNo,
            /* [ref][out] */ DVD_SUBPICTURE_STREAM_ATTR __RPC_FAR *pSPAttr);
        
        END_INTERFACE
    } IDVDStreamAttributesVtbl;

    interface IDVDStreamAttributes
    {
        CONST_VTBL struct IDVDStreamAttributesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDStreamAttributes_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDStreamAttributes_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDStreamAttributes_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDStreamAttributes_GetVideoAttr(This,pVideoAttr)	\
    (This)->lpVtbl -> GetVideoAttr(This,pVideoAttr)

#define IDVDStreamAttributes_GetAudioCount(This,pStreamCount)	\
    (This)->lpVtbl -> GetAudioCount(This,pStreamCount)

#define IDVDStreamAttributes_GetAudioAttr(This,nStreamNo,pAudioAttr)	\
    (This)->lpVtbl -> GetAudioAttr(This,nStreamNo,pAudioAttr)

#define IDVDStreamAttributes_GetAudioMultichannelUseAttr(This,nStreamNo,pMuse)	\
    (This)->lpVtbl -> GetAudioMultichannelUseAttr(This,nStreamNo,pMuse)

#define IDVDStreamAttributes_GetSubpictureCount(This,pStreamCount)	\
    (This)->lpVtbl -> GetSubpictureCount(This,pStreamCount)

#define IDVDStreamAttributes_GetSubpictureAttr(This,nStreamNo,pSPAttr)	\
    (This)->lpVtbl -> GetSubpictureAttr(This,nStreamNo,pSPAttr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDStreamAttributes_GetVideoAttr_Proxy( 
    IDVDStreamAttributes __RPC_FAR * This,
    /* [ref][out] */ UINT16 __RPC_FAR *pVideoAttr);


void __RPC_STUB IDVDStreamAttributes_GetVideoAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamAttributes_GetAudioCount_Proxy( 
    IDVDStreamAttributes __RPC_FAR * This,
    /* [ref][out] */ UINT8 __RPC_FAR *pStreamCount);


void __RPC_STUB IDVDStreamAttributes_GetAudioCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamAttributes_GetAudioAttr_Proxy( 
    IDVDStreamAttributes __RPC_FAR * This,
    /* [in] */ UINT nStreamNo,
    /* [ref][out] */ DVD_AUDIO_STREAM_ATTR __RPC_FAR *pAudioAttr);


void __RPC_STUB IDVDStreamAttributes_GetAudioAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamAttributes_GetAudioMultichannelUseAttr_Proxy( 
    IDVDStreamAttributes __RPC_FAR * This,
    /* [in] */ UINT8 nStreamNo,
    /* [ref][out] */ DVD_AUDIO_MU_ATTR __RPC_FAR *pMuse);


void __RPC_STUB IDVDStreamAttributes_GetAudioMultichannelUseAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamAttributes_GetSubpictureCount_Proxy( 
    IDVDStreamAttributes __RPC_FAR * This,
    /* [ref][out] */ UINT8 __RPC_FAR *pStreamCount);


void __RPC_STUB IDVDStreamAttributes_GetSubpictureCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamAttributes_GetSubpictureAttr_Proxy( 
    IDVDStreamAttributes __RPC_FAR * This,
    /* [in] */ UINT8 nStreamNo,
    /* [ref][out] */ DVD_SUBPICTURE_STREAM_ATTR __RPC_FAR *pSPAttr);


void __RPC_STUB IDVDStreamAttributes_GetSubpictureAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDStreamAttributes_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_dvdata_0088 */
/* [local] */ 

#define	DVD_SYSTEM_SPACE_BIT	1
#define	DVD_MENU_SPACE_BIT		(DVD_SYSTEM_SPACE_BIT << 1)
#define	DVD_VMG_SPACE_BIT		(DVD_MENU_SPACE_BIT << 1)
#define	DVD_VTS_SPACE_BIT		(DVD_VMG_SPACE_BIT << 1)







extern RPC_IF_HANDLE __MIDL_itf_dvdata_0088_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dvdata_0088_v0_0_s_ifspec;

#ifndef __IDVDDomain_INTERFACE_DEFINED__
#define __IDVDDomain_INTERFACE_DEFINED__

/* interface IDVDDomain */
/* [unique][helpstring][uuid][object] */ 

typedef /* [public][public] */ 
enum __MIDL_IDVDDomain_0001
    {	DVD_FP_DOM	= 0,
	DVD_VMGM_DOM	= DVD_FP_DOM + 1,
	DVD_VTSM_DOM	= DVD_VMGM_DOM + 1,
	DVD_TT_DOM	= DVD_VTSM_DOM + 1
    }	EDVDDomainType;


EXTERN_C const IID IID_IDVDDomain;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5079-9F3D-11D0-A979-00A0C9050598")
    IDVDDomain : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [ref][out] */ EDVDDomainType __RPC_FAR *pDType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSpace( 
            /* [ref][out] */ UINT8 __RPC_FAR *pSpaceSet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStreamAttributes( 
            /* [ref][out] */ IDVDStreamAttributes __RPC_FAR *__RPC_FAR *ppAttr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPGCCount( 
            /* [ref][out] */ UINT16 __RPC_FAR *pCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPGC( 
            /* [in] */ UINT16 pgcn,
            /* [ref][out] */ IDVDProgramChain __RPC_FAR *__RPC_FAR *ppPGC,
            /* [ref][out] */ UINT32 __RPC_FAR *pPGCCat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTotalVideoObjects( 
            /* [ref][out] */ UINT16 __RPC_FAR *pTotalVobs,
            /* [ref][out] */ UINT32 __RPC_FAR *pCellPieceCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCellPieceEnumerator( 
            /* [ref][out] */ IEnumDVD_CELL_PIECE __RPC_FAR *__RPC_FAR *ppEnumPieces,
            /* [ref][out] */ UINT32 __RPC_FAR *pPieceCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTotalVOBUnits( 
            /* [ref][out] */ UINT32 __RPC_FAR *pVobus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVOBUTablePtr( 
            /* [ref][out] */ UINT32 __RPC_FAR *__RPC_FAR *ppVOBUAddressArray) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTimeMap( 
            /* [in] */ UINT16 pgcn,
            /* [ref][out] */ IDVDTimeMap __RPC_FAR *__RPC_FAR *ppTimeMap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVOBFileId( 
            INT16 __RPC_FAR *pFileId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVolume( 
            IDVDVideoVolume __RPC_FAR *__RPC_FAR *ppVolume) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTitleSet( 
            IDVDTitleSet __RPC_FAR *__RPC_FAR *ppTitleSet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDDomainVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDDomain __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDDomain __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDDomain __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetType )( 
            IDVDDomain __RPC_FAR * This,
            /* [ref][out] */ EDVDDomainType __RPC_FAR *pDType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSpace )( 
            IDVDDomain __RPC_FAR * This,
            /* [ref][out] */ UINT8 __RPC_FAR *pSpaceSet);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetStreamAttributes )( 
            IDVDDomain __RPC_FAR * This,
            /* [ref][out] */ IDVDStreamAttributes __RPC_FAR *__RPC_FAR *ppAttr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPGCCount )( 
            IDVDDomain __RPC_FAR * This,
            /* [ref][out] */ UINT16 __RPC_FAR *pCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPGC )( 
            IDVDDomain __RPC_FAR * This,
            /* [in] */ UINT16 pgcn,
            /* [ref][out] */ IDVDProgramChain __RPC_FAR *__RPC_FAR *ppPGC,
            /* [ref][out] */ UINT32 __RPC_FAR *pPGCCat);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTotalVideoObjects )( 
            IDVDDomain __RPC_FAR * This,
            /* [ref][out] */ UINT16 __RPC_FAR *pTotalVobs,
            /* [ref][out] */ UINT32 __RPC_FAR *pCellPieceCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCellPieceEnumerator )( 
            IDVDDomain __RPC_FAR * This,
            /* [ref][out] */ IEnumDVD_CELL_PIECE __RPC_FAR *__RPC_FAR *ppEnumPieces,
            /* [ref][out] */ UINT32 __RPC_FAR *pPieceCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTotalVOBUnits )( 
            IDVDDomain __RPC_FAR * This,
            /* [ref][out] */ UINT32 __RPC_FAR *pVobus);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVOBUTablePtr )( 
            IDVDDomain __RPC_FAR * This,
            /* [ref][out] */ UINT32 __RPC_FAR *__RPC_FAR *ppVOBUAddressArray);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTimeMap )( 
            IDVDDomain __RPC_FAR * This,
            /* [in] */ UINT16 pgcn,
            /* [ref][out] */ IDVDTimeMap __RPC_FAR *__RPC_FAR *ppTimeMap);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVOBFileId )( 
            IDVDDomain __RPC_FAR * This,
            INT16 __RPC_FAR *pFileId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVolume )( 
            IDVDDomain __RPC_FAR * This,
            IDVDVideoVolume __RPC_FAR *__RPC_FAR *ppVolume);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTitleSet )( 
            IDVDDomain __RPC_FAR * This,
            IDVDTitleSet __RPC_FAR *__RPC_FAR *ppTitleSet);
        
        END_INTERFACE
    } IDVDDomainVtbl;

    interface IDVDDomain
    {
        CONST_VTBL struct IDVDDomainVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDDomain_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDDomain_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDDomain_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDDomain_GetType(This,pDType)	\
    (This)->lpVtbl -> GetType(This,pDType)

#define IDVDDomain_GetSpace(This,pSpaceSet)	\
    (This)->lpVtbl -> GetSpace(This,pSpaceSet)

#define IDVDDomain_GetStreamAttributes(This,ppAttr)	\
    (This)->lpVtbl -> GetStreamAttributes(This,ppAttr)

#define IDVDDomain_GetPGCCount(This,pCount)	\
    (This)->lpVtbl -> GetPGCCount(This,pCount)

#define IDVDDomain_GetPGC(This,pgcn,ppPGC,pPGCCat)	\
    (This)->lpVtbl -> GetPGC(This,pgcn,ppPGC,pPGCCat)

#define IDVDDomain_GetTotalVideoObjects(This,pTotalVobs,pCellPieceCount)	\
    (This)->lpVtbl -> GetTotalVideoObjects(This,pTotalVobs,pCellPieceCount)

#define IDVDDomain_GetCellPieceEnumerator(This,ppEnumPieces,pPieceCount)	\
    (This)->lpVtbl -> GetCellPieceEnumerator(This,ppEnumPieces,pPieceCount)

#define IDVDDomain_GetTotalVOBUnits(This,pVobus)	\
    (This)->lpVtbl -> GetTotalVOBUnits(This,pVobus)

#define IDVDDomain_GetVOBUTablePtr(This,ppVOBUAddressArray)	\
    (This)->lpVtbl -> GetVOBUTablePtr(This,ppVOBUAddressArray)

#define IDVDDomain_GetTimeMap(This,pgcn,ppTimeMap)	\
    (This)->lpVtbl -> GetTimeMap(This,pgcn,ppTimeMap)

#define IDVDDomain_GetVOBFileId(This,pFileId)	\
    (This)->lpVtbl -> GetVOBFileId(This,pFileId)

#define IDVDDomain_GetVolume(This,ppVolume)	\
    (This)->lpVtbl -> GetVolume(This,ppVolume)

#define IDVDDomain_GetTitleSet(This,ppTitleSet)	\
    (This)->lpVtbl -> GetTitleSet(This,ppTitleSet)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDDomain_GetType_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [ref][out] */ EDVDDomainType __RPC_FAR *pDType);


void __RPC_STUB IDVDDomain_GetType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetSpace_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [ref][out] */ UINT8 __RPC_FAR *pSpaceSet);


void __RPC_STUB IDVDDomain_GetSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetStreamAttributes_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [ref][out] */ IDVDStreamAttributes __RPC_FAR *__RPC_FAR *ppAttr);


void __RPC_STUB IDVDDomain_GetStreamAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetPGCCount_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [ref][out] */ UINT16 __RPC_FAR *pCount);


void __RPC_STUB IDVDDomain_GetPGCCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetPGC_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [in] */ UINT16 pgcn,
    /* [ref][out] */ IDVDProgramChain __RPC_FAR *__RPC_FAR *ppPGC,
    /* [ref][out] */ UINT32 __RPC_FAR *pPGCCat);


void __RPC_STUB IDVDDomain_GetPGC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetTotalVideoObjects_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [ref][out] */ UINT16 __RPC_FAR *pTotalVobs,
    /* [ref][out] */ UINT32 __RPC_FAR *pCellPieceCount);


void __RPC_STUB IDVDDomain_GetTotalVideoObjects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetCellPieceEnumerator_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [ref][out] */ IEnumDVD_CELL_PIECE __RPC_FAR *__RPC_FAR *ppEnumPieces,
    /* [ref][out] */ UINT32 __RPC_FAR *pPieceCount);


void __RPC_STUB IDVDDomain_GetCellPieceEnumerator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetTotalVOBUnits_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [ref][out] */ UINT32 __RPC_FAR *pVobus);


void __RPC_STUB IDVDDomain_GetTotalVOBUnits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetVOBUTablePtr_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [ref][out] */ UINT32 __RPC_FAR *__RPC_FAR *ppVOBUAddressArray);


void __RPC_STUB IDVDDomain_GetVOBUTablePtr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetTimeMap_Proxy( 
    IDVDDomain __RPC_FAR * This,
    /* [in] */ UINT16 pgcn,
    /* [ref][out] */ IDVDTimeMap __RPC_FAR *__RPC_FAR *ppTimeMap);


void __RPC_STUB IDVDDomain_GetTimeMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetVOBFileId_Proxy( 
    IDVDDomain __RPC_FAR * This,
    INT16 __RPC_FAR *pFileId);


void __RPC_STUB IDVDDomain_GetVOBFileId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetVolume_Proxy( 
    IDVDDomain __RPC_FAR * This,
    IDVDVideoVolume __RPC_FAR *__RPC_FAR *ppVolume);


void __RPC_STUB IDVDDomain_GetVolume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDomain_GetTitleSet_Proxy( 
    IDVDDomain __RPC_FAR * This,
    IDVDTitleSet __RPC_FAR *__RPC_FAR *ppTitleSet);


void __RPC_STUB IDVDDomain_GetTitleSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDDomain_INTERFACE_DEFINED__ */


#ifndef __IDVDMenu_INTERFACE_DEFINED__
#define __IDVDMenu_INTERFACE_DEFINED__

/* interface IDVDMenu */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDMenu;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB507A-9F3D-11D0-A979-00A0C9050598")
    IDVDMenu : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDomain( 
            /* [in] */ UINT16 languageCode,
            /* [ref][out] */ IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain,
            /* [ref][out] */ UINT8 __RPC_FAR *pExistFlag) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMenuLanguageInfo( 
            /* [ref][out][in] */ UINT16 __RPC_FAR *pLangCount,
            /* [full] */ UINT16 __RPC_FAR *pLCBuffer,
            /* [full] */ UINT8 __RPC_FAR *pFlagsBuffer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDMenuVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDMenu __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDMenu __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDMenu __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDomain )( 
            IDVDMenu __RPC_FAR * This,
            /* [in] */ UINT16 languageCode,
            /* [ref][out] */ IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain,
            /* [ref][out] */ UINT8 __RPC_FAR *pExistFlag);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMenuLanguageInfo )( 
            IDVDMenu __RPC_FAR * This,
            /* [ref][out][in] */ UINT16 __RPC_FAR *pLangCount,
            /* [full] */ UINT16 __RPC_FAR *pLCBuffer,
            /* [full] */ UINT8 __RPC_FAR *pFlagsBuffer);
        
        END_INTERFACE
    } IDVDMenuVtbl;

    interface IDVDMenu
    {
        CONST_VTBL struct IDVDMenuVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDMenu_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDMenu_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDMenu_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDMenu_GetDomain(This,languageCode,ppDomain,pExistFlag)	\
    (This)->lpVtbl -> GetDomain(This,languageCode,ppDomain,pExistFlag)

#define IDVDMenu_GetMenuLanguageInfo(This,pLangCount,pLCBuffer,pFlagsBuffer)	\
    (This)->lpVtbl -> GetMenuLanguageInfo(This,pLangCount,pLCBuffer,pFlagsBuffer)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDMenu_GetDomain_Proxy( 
    IDVDMenu __RPC_FAR * This,
    /* [in] */ UINT16 languageCode,
    /* [ref][out] */ IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain,
    /* [ref][out] */ UINT8 __RPC_FAR *pExistFlag);


void __RPC_STUB IDVDMenu_GetDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDMenu_GetMenuLanguageInfo_Proxy( 
    IDVDMenu __RPC_FAR * This,
    /* [ref][out][in] */ UINT16 __RPC_FAR *pLangCount,
    /* [full] */ UINT16 __RPC_FAR *pLCBuffer,
    /* [full] */ UINT8 __RPC_FAR *pFlagsBuffer);


void __RPC_STUB IDVDMenu_GetMenuLanguageInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDMenu_INTERFACE_DEFINED__ */


#ifndef __IEnumDVD_CELL_PIECE_INTERFACE_DEFINED__
#define __IEnumDVD_CELL_PIECE_INTERFACE_DEFINED__

/* interface IEnumDVD_CELL_PIECE */
/* [unique][helpstring][uuid][object] */ 

typedef /* [public][public] */ struct __MIDL_IEnumDVD_CELL_PIECE_0001
    {
    UINT16 vob_id;
    UINT8 c_idn;
    UINT32 start;
    UINT32 end;
    }	DVD_CELL_PIECE;

typedef struct __MIDL_IEnumDVD_CELL_PIECE_0001 __RPC_FAR *PDVD_CELL_PIECE;


EXTERN_C const IID IID_IEnumDVD_CELL_PIECE;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB507B-9F3D-11D0-A979-00A0C9050598")
    IEnumDVD_CELL_PIECE : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [full] */ DVD_CELL_PIECE __RPC_FAR *rgelt,
            /* [in] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG eltCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [ref][out] */ IEnumDVD_CELL_PIECE __RPC_FAR *__RPC_FAR *ppClone) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDVD_CELL_PIECEVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumDVD_CELL_PIECE __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumDVD_CELL_PIECE __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumDVD_CELL_PIECE __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumDVD_CELL_PIECE __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [full] */ DVD_CELL_PIECE __RPC_FAR *rgelt,
            /* [in] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumDVD_CELL_PIECE __RPC_FAR * This,
            /* [in] */ ULONG eltCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumDVD_CELL_PIECE __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumDVD_CELL_PIECE __RPC_FAR * This,
            /* [ref][out] */ IEnumDVD_CELL_PIECE __RPC_FAR *__RPC_FAR *ppClone);
        
        END_INTERFACE
    } IEnumDVD_CELL_PIECEVtbl;

    interface IEnumDVD_CELL_PIECE
    {
        CONST_VTBL struct IEnumDVD_CELL_PIECEVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDVD_CELL_PIECE_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDVD_CELL_PIECE_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDVD_CELL_PIECE_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDVD_CELL_PIECE_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDVD_CELL_PIECE_Skip(This,eltCount)	\
    (This)->lpVtbl -> Skip(This,eltCount)

#define IEnumDVD_CELL_PIECE_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDVD_CELL_PIECE_Clone(This,ppClone)	\
    (This)->lpVtbl -> Clone(This,ppClone)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDVD_CELL_PIECE_Next_Proxy( 
    IEnumDVD_CELL_PIECE __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [full] */ DVD_CELL_PIECE __RPC_FAR *rgelt,
    /* [in] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumDVD_CELL_PIECE_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDVD_CELL_PIECE_Skip_Proxy( 
    IEnumDVD_CELL_PIECE __RPC_FAR * This,
    /* [in] */ ULONG eltCount);


void __RPC_STUB IEnumDVD_CELL_PIECE_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDVD_CELL_PIECE_Reset_Proxy( 
    IEnumDVD_CELL_PIECE __RPC_FAR * This);


void __RPC_STUB IEnumDVD_CELL_PIECE_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDVD_CELL_PIECE_Clone_Proxy( 
    IEnumDVD_CELL_PIECE __RPC_FAR * This,
    /* [ref][out] */ IEnumDVD_CELL_PIECE __RPC_FAR *__RPC_FAR *ppClone);


void __RPC_STUB IEnumDVD_CELL_PIECE_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDVD_CELL_PIECE_INTERFACE_DEFINED__ */


#ifndef __IEnumDVD_TEXT_LANGUAGE_UNIT_INTERFACE_DEFINED__
#define __IEnumDVD_TEXT_LANGUAGE_UNIT_INTERFACE_DEFINED__

/* interface IEnumDVD_TEXT_LANGUAGE_UNIT */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumDVD_TEXT_LANGUAGE_UNIT;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB507C-9F3D-11D0-A979-00A0C9050598")
    IEnumDVD_TEXT_LANGUAGE_UNIT : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [full] */ DVD_TEXT_LANGUAGE_UNIT __RPC_FAR *rgelt,
            /* [in] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG eltCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [ref][out] */ IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR *__RPC_FAR *ppClone) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDVD_TEXT_LANGUAGE_UNITVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [full] */ DVD_TEXT_LANGUAGE_UNIT __RPC_FAR *rgelt,
            /* [in] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This,
            /* [in] */ ULONG eltCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This,
            /* [ref][out] */ IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR *__RPC_FAR *ppClone);
        
        END_INTERFACE
    } IEnumDVD_TEXT_LANGUAGE_UNITVtbl;

    interface IEnumDVD_TEXT_LANGUAGE_UNIT
    {
        CONST_VTBL struct IEnumDVD_TEXT_LANGUAGE_UNITVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDVD_TEXT_LANGUAGE_UNIT_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDVD_TEXT_LANGUAGE_UNIT_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDVD_TEXT_LANGUAGE_UNIT_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDVD_TEXT_LANGUAGE_UNIT_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDVD_TEXT_LANGUAGE_UNIT_Skip(This,eltCount)	\
    (This)->lpVtbl -> Skip(This,eltCount)

#define IEnumDVD_TEXT_LANGUAGE_UNIT_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDVD_TEXT_LANGUAGE_UNIT_Clone(This,ppClone)	\
    (This)->lpVtbl -> Clone(This,ppClone)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDVD_TEXT_LANGUAGE_UNIT_Next_Proxy( 
    IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [full] */ DVD_TEXT_LANGUAGE_UNIT __RPC_FAR *rgelt,
    /* [in] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumDVD_TEXT_LANGUAGE_UNIT_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDVD_TEXT_LANGUAGE_UNIT_Skip_Proxy( 
    IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This,
    /* [in] */ ULONG eltCount);


void __RPC_STUB IEnumDVD_TEXT_LANGUAGE_UNIT_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDVD_TEXT_LANGUAGE_UNIT_Reset_Proxy( 
    IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This);


void __RPC_STUB IEnumDVD_TEXT_LANGUAGE_UNIT_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDVD_TEXT_LANGUAGE_UNIT_Clone_Proxy( 
    IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR * This,
    /* [ref][out] */ IEnumDVD_TEXT_LANGUAGE_UNIT __RPC_FAR *__RPC_FAR *ppClone);


void __RPC_STUB IEnumDVD_TEXT_LANGUAGE_UNIT_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDVD_TEXT_LANGUAGE_UNIT_INTERFACE_DEFINED__ */


#ifndef __IEnumDVD_TEXT_ITEM_INTERFACE_DEFINED__
#define __IEnumDVD_TEXT_ITEM_INTERFACE_DEFINED__

/* interface IEnumDVD_TEXT_ITEM */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumDVD_TEXT_ITEM;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB507D-9F3D-11D0-A979-00A0C9050598")
    IEnumDVD_TEXT_ITEM : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [full] */ DVD_TEXT_ITEM __RPC_FAR *rgelt,
            /* [in] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG eltCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [ref][out] */ IEnumDVD_TEXT_ITEM __RPC_FAR *__RPC_FAR *ppClone) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDVD_TEXT_ITEMVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumDVD_TEXT_ITEM __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumDVD_TEXT_ITEM __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumDVD_TEXT_ITEM __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumDVD_TEXT_ITEM __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [full] */ DVD_TEXT_ITEM __RPC_FAR *rgelt,
            /* [in] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumDVD_TEXT_ITEM __RPC_FAR * This,
            /* [in] */ ULONG eltCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumDVD_TEXT_ITEM __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumDVD_TEXT_ITEM __RPC_FAR * This,
            /* [ref][out] */ IEnumDVD_TEXT_ITEM __RPC_FAR *__RPC_FAR *ppClone);
        
        END_INTERFACE
    } IEnumDVD_TEXT_ITEMVtbl;

    interface IEnumDVD_TEXT_ITEM
    {
        CONST_VTBL struct IEnumDVD_TEXT_ITEMVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDVD_TEXT_ITEM_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDVD_TEXT_ITEM_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDVD_TEXT_ITEM_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDVD_TEXT_ITEM_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDVD_TEXT_ITEM_Skip(This,eltCount)	\
    (This)->lpVtbl -> Skip(This,eltCount)

#define IEnumDVD_TEXT_ITEM_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDVD_TEXT_ITEM_Clone(This,ppClone)	\
    (This)->lpVtbl -> Clone(This,ppClone)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDVD_TEXT_ITEM_Next_Proxy( 
    IEnumDVD_TEXT_ITEM __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [full] */ DVD_TEXT_ITEM __RPC_FAR *rgelt,
    /* [in] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumDVD_TEXT_ITEM_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDVD_TEXT_ITEM_Skip_Proxy( 
    IEnumDVD_TEXT_ITEM __RPC_FAR * This,
    /* [in] */ ULONG eltCount);


void __RPC_STUB IEnumDVD_TEXT_ITEM_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDVD_TEXT_ITEM_Reset_Proxy( 
    IEnumDVD_TEXT_ITEM __RPC_FAR * This);


void __RPC_STUB IEnumDVD_TEXT_ITEM_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDVD_TEXT_ITEM_Clone_Proxy( 
    IEnumDVD_TEXT_ITEM __RPC_FAR * This,
    /* [ref][out] */ IEnumDVD_TEXT_ITEM __RPC_FAR *__RPC_FAR *ppClone);


void __RPC_STUB IEnumDVD_TEXT_ITEM_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDVD_TEXT_ITEM_INTERFACE_DEFINED__ */


#ifndef __IDVDTitleSet_INTERFACE_DEFINED__
#define __IDVDTitleSet_INTERFACE_DEFINED__

/* interface IDVDTitleSet */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDTitleSet;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB507E-9F3D-11D0-A979-00A0C9050598")
    IDVDTitleSet : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSpecVersion( 
            UINT16 __RPC_FAR *pVersion) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCategory( 
            UINT32 __RPC_FAR *pCategory) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMenu( 
            IDVDMenu __RPC_FAR *__RPC_FAR *ppMenu) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDomain( 
            IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTotalTitleUnitCount( 
            UINT16 __RPC_FAR *pTtuCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTitlePart( 
            UINT16 ttu,
            UINT16 ptt,
            UINT16 __RPC_FAR *pPgcn,
            UINT8 __RPC_FAR *pPgn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVTSN( 
            UINT16 __RPC_FAR *pVTSN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVolume( 
            IDVDVideoVolume __RPC_FAR *__RPC_FAR *pVolume) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDTitleSetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDTitleSet __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDTitleSet __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDTitleSet __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSpecVersion )( 
            IDVDTitleSet __RPC_FAR * This,
            UINT16 __RPC_FAR *pVersion);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCategory )( 
            IDVDTitleSet __RPC_FAR * This,
            UINT32 __RPC_FAR *pCategory);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMenu )( 
            IDVDTitleSet __RPC_FAR * This,
            IDVDMenu __RPC_FAR *__RPC_FAR *ppMenu);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDomain )( 
            IDVDTitleSet __RPC_FAR * This,
            IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTotalTitleUnitCount )( 
            IDVDTitleSet __RPC_FAR * This,
            UINT16 __RPC_FAR *pTtuCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTitlePart )( 
            IDVDTitleSet __RPC_FAR * This,
            UINT16 ttu,
            UINT16 ptt,
            UINT16 __RPC_FAR *pPgcn,
            UINT8 __RPC_FAR *pPgn);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVTSN )( 
            IDVDTitleSet __RPC_FAR * This,
            UINT16 __RPC_FAR *pVTSN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVolume )( 
            IDVDTitleSet __RPC_FAR * This,
            IDVDVideoVolume __RPC_FAR *__RPC_FAR *pVolume);
        
        END_INTERFACE
    } IDVDTitleSetVtbl;

    interface IDVDTitleSet
    {
        CONST_VTBL struct IDVDTitleSetVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDTitleSet_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDTitleSet_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDTitleSet_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDTitleSet_GetSpecVersion(This,pVersion)	\
    (This)->lpVtbl -> GetSpecVersion(This,pVersion)

#define IDVDTitleSet_GetCategory(This,pCategory)	\
    (This)->lpVtbl -> GetCategory(This,pCategory)

#define IDVDTitleSet_GetMenu(This,ppMenu)	\
    (This)->lpVtbl -> GetMenu(This,ppMenu)

#define IDVDTitleSet_GetDomain(This,ppDomain)	\
    (This)->lpVtbl -> GetDomain(This,ppDomain)

#define IDVDTitleSet_GetTotalTitleUnitCount(This,pTtuCount)	\
    (This)->lpVtbl -> GetTotalTitleUnitCount(This,pTtuCount)

#define IDVDTitleSet_GetTitlePart(This,ttu,ptt,pPgcn,pPgn)	\
    (This)->lpVtbl -> GetTitlePart(This,ttu,ptt,pPgcn,pPgn)

#define IDVDTitleSet_GetVTSN(This,pVTSN)	\
    (This)->lpVtbl -> GetVTSN(This,pVTSN)

#define IDVDTitleSet_GetVolume(This,pVolume)	\
    (This)->lpVtbl -> GetVolume(This,pVolume)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDTitleSet_GetSpecVersion_Proxy( 
    IDVDTitleSet __RPC_FAR * This,
    UINT16 __RPC_FAR *pVersion);


void __RPC_STUB IDVDTitleSet_GetSpecVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTitleSet_GetCategory_Proxy( 
    IDVDTitleSet __RPC_FAR * This,
    UINT32 __RPC_FAR *pCategory);


void __RPC_STUB IDVDTitleSet_GetCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTitleSet_GetMenu_Proxy( 
    IDVDTitleSet __RPC_FAR * This,
    IDVDMenu __RPC_FAR *__RPC_FAR *ppMenu);


void __RPC_STUB IDVDTitleSet_GetMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTitleSet_GetDomain_Proxy( 
    IDVDTitleSet __RPC_FAR * This,
    IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain);


void __RPC_STUB IDVDTitleSet_GetDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTitleSet_GetTotalTitleUnitCount_Proxy( 
    IDVDTitleSet __RPC_FAR * This,
    UINT16 __RPC_FAR *pTtuCount);


void __RPC_STUB IDVDTitleSet_GetTotalTitleUnitCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTitleSet_GetTitlePart_Proxy( 
    IDVDTitleSet __RPC_FAR * This,
    UINT16 ttu,
    UINT16 ptt,
    UINT16 __RPC_FAR *pPgcn,
    UINT8 __RPC_FAR *pPgn);


void __RPC_STUB IDVDTitleSet_GetTitlePart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTitleSet_GetVTSN_Proxy( 
    IDVDTitleSet __RPC_FAR * This,
    UINT16 __RPC_FAR *pVTSN);


void __RPC_STUB IDVDTitleSet_GetVTSN_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTitleSet_GetVolume_Proxy( 
    IDVDTitleSet __RPC_FAR * This,
    IDVDVideoVolume __RPC_FAR *__RPC_FAR *pVolume);


void __RPC_STUB IDVDTitleSet_GetVolume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDTitleSet_INTERFACE_DEFINED__ */


#ifndef __IDVDProgramChain_INTERFACE_DEFINED__
#define __IDVDProgramChain_INTERFACE_DEFINED__

/* interface IDVDProgramChain */
/* [unique][helpstring][uuid][object] */ 

typedef /* [public][public][public] */ struct __MIDL_IDVDProgramChain_0001
    {
    UINT8 cmd[ 8 ];
    }	DVD_NAV_CMD;

typedef /* [public][public][public] */ 
enum __MIDL_IDVDProgramChain_0002
    {	DVD_PRE_CMD	= 0,
	DVD_POST_CMD	= 1,
	DVD_CELL_CMD	= 2
    }	ENavCmdArea;

typedef /* [public][public] */ struct __MIDL_IDVDProgramChain_0003
    {
    UINT32 cellCategory;
    UINT32 cellPlaybackTime;
    UINT32 startFirstVobu;
    UINT32 endFirstIlvu;
    UINT32 startLastVobu;
    UINT32 endLastVobu;
    }	DVD_CELL_PLAYBACK;

typedef /* [public][public] */ 
enum __MIDL_IDVDProgramChain_0004
    {	DVD_NEXT_PGC	= 0,
	DVD_PREVIOUS_PGC	= DVD_NEXT_PGC + 1,
	DVD_UP_PGC	= DVD_PREVIOUS_PGC + 1
    }	EPGCLinkType;

typedef /* [public][public] */ 
enum __MIDL_IDVDProgramChain_0005
    {	DVD_PLAYMODE_SEQUENTIAL	= 0,
	DVD_PLAYMODE_RANDOM	= DVD_PLAYMODE_SEQUENTIAL + 1,
	DVD_PLAYMODE_SHUFFLE	= DVD_PLAYMODE_RANDOM + 1
    }	EPGCPlaybackMode;


EXTERN_C const IID IID_IDVDProgramChain;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB507F-9F3D-11D0-A979-00A0C9050598")
    IDVDProgramChain : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDomain( 
            IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContents( 
            UINT32 __RPC_FAR *pContents) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPlaybackTime( 
            UINT32 __RPC_FAR *pbcdTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUserOperationControlMask( 
            UINT32 __RPC_FAR *pmask) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAudioStreamSelector( 
            UINT8 streamN,
            UINT16 __RPC_FAR *pSelector) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSubpictureStreamSelector( 
            UINT8 streamN,
            UINT32 __RPC_FAR *pSPSel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLink( 
            EPGCLinkType eType,
            UINT16 __RPC_FAR *pgcn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPlaybackMode( 
            EPGCPlaybackMode __RPC_FAR *pMode,
            UINT8 __RPC_FAR *pRepeatCount,
            UINT8 __RPC_FAR *pStillTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSubpicturePalette( 
            UINT32 __RPC_FAR *pBuf) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNavCommandsCount( 
            ENavCmdArea eArea,
            UINT16 __RPC_FAR *pCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNavCommand( 
            ENavCmdArea area,
            UINT16 commandN,
            DVD_NAV_CMD __RPC_FAR *pCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProgramEntryCell( 
            UINT16 pgno,
            UINT8 __RPC_FAR *pEntryCN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCellPlaybackInfo( 
            UINT8 cn,
            DVD_CELL_PLAYBACK __RPC_FAR *pInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCellPosition( 
            UINT8 cn,
            UINT16 __RPC_FAR *pVOB_ID,
            UINT8 __RPC_FAR *pC_IDN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPGCN( 
            UINT16 __RPC_FAR *pPGCN) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDProgramChainVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDProgramChain __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDProgramChain __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDProgramChain __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDomain )( 
            IDVDProgramChain __RPC_FAR * This,
            IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetContents )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT32 __RPC_FAR *pContents);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPlaybackTime )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT32 __RPC_FAR *pbcdTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetUserOperationControlMask )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT32 __RPC_FAR *pmask);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAudioStreamSelector )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT8 streamN,
            UINT16 __RPC_FAR *pSelector);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubpictureStreamSelector )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT8 streamN,
            UINT32 __RPC_FAR *pSPSel);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLink )( 
            IDVDProgramChain __RPC_FAR * This,
            EPGCLinkType eType,
            UINT16 __RPC_FAR *pgcn);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPlaybackMode )( 
            IDVDProgramChain __RPC_FAR * This,
            EPGCPlaybackMode __RPC_FAR *pMode,
            UINT8 __RPC_FAR *pRepeatCount,
            UINT8 __RPC_FAR *pStillTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubpicturePalette )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT32 __RPC_FAR *pBuf);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNavCommandsCount )( 
            IDVDProgramChain __RPC_FAR * This,
            ENavCmdArea eArea,
            UINT16 __RPC_FAR *pCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNavCommand )( 
            IDVDProgramChain __RPC_FAR * This,
            ENavCmdArea area,
            UINT16 commandN,
            DVD_NAV_CMD __RPC_FAR *pCmd);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetProgramEntryCell )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT16 pgno,
            UINT8 __RPC_FAR *pEntryCN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCellPlaybackInfo )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT8 cn,
            DVD_CELL_PLAYBACK __RPC_FAR *pInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCellPosition )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT8 cn,
            UINT16 __RPC_FAR *pVOB_ID,
            UINT8 __RPC_FAR *pC_IDN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPGCN )( 
            IDVDProgramChain __RPC_FAR * This,
            UINT16 __RPC_FAR *pPGCN);
        
        END_INTERFACE
    } IDVDProgramChainVtbl;

    interface IDVDProgramChain
    {
        CONST_VTBL struct IDVDProgramChainVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDProgramChain_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDProgramChain_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDProgramChain_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDProgramChain_GetDomain(This,ppDomain)	\
    (This)->lpVtbl -> GetDomain(This,ppDomain)

#define IDVDProgramChain_GetContents(This,pContents)	\
    (This)->lpVtbl -> GetContents(This,pContents)

#define IDVDProgramChain_GetPlaybackTime(This,pbcdTime)	\
    (This)->lpVtbl -> GetPlaybackTime(This,pbcdTime)

#define IDVDProgramChain_GetUserOperationControlMask(This,pmask)	\
    (This)->lpVtbl -> GetUserOperationControlMask(This,pmask)

#define IDVDProgramChain_GetAudioStreamSelector(This,streamN,pSelector)	\
    (This)->lpVtbl -> GetAudioStreamSelector(This,streamN,pSelector)

#define IDVDProgramChain_GetSubpictureStreamSelector(This,streamN,pSPSel)	\
    (This)->lpVtbl -> GetSubpictureStreamSelector(This,streamN,pSPSel)

#define IDVDProgramChain_GetLink(This,eType,pgcn)	\
    (This)->lpVtbl -> GetLink(This,eType,pgcn)

#define IDVDProgramChain_GetPlaybackMode(This,pMode,pRepeatCount,pStillTime)	\
    (This)->lpVtbl -> GetPlaybackMode(This,pMode,pRepeatCount,pStillTime)

#define IDVDProgramChain_GetSubpicturePalette(This,pBuf)	\
    (This)->lpVtbl -> GetSubpicturePalette(This,pBuf)

#define IDVDProgramChain_GetNavCommandsCount(This,eArea,pCount)	\
    (This)->lpVtbl -> GetNavCommandsCount(This,eArea,pCount)

#define IDVDProgramChain_GetNavCommand(This,area,commandN,pCmd)	\
    (This)->lpVtbl -> GetNavCommand(This,area,commandN,pCmd)

#define IDVDProgramChain_GetProgramEntryCell(This,pgno,pEntryCN)	\
    (This)->lpVtbl -> GetProgramEntryCell(This,pgno,pEntryCN)

#define IDVDProgramChain_GetCellPlaybackInfo(This,cn,pInfo)	\
    (This)->lpVtbl -> GetCellPlaybackInfo(This,cn,pInfo)

#define IDVDProgramChain_GetCellPosition(This,cn,pVOB_ID,pC_IDN)	\
    (This)->lpVtbl -> GetCellPosition(This,cn,pVOB_ID,pC_IDN)

#define IDVDProgramChain_GetPGCN(This,pPGCN)	\
    (This)->lpVtbl -> GetPGCN(This,pPGCN)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetDomain_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain);


void __RPC_STUB IDVDProgramChain_GetDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetContents_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT32 __RPC_FAR *pContents);


void __RPC_STUB IDVDProgramChain_GetContents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetPlaybackTime_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT32 __RPC_FAR *pbcdTime);


void __RPC_STUB IDVDProgramChain_GetPlaybackTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetUserOperationControlMask_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT32 __RPC_FAR *pmask);


void __RPC_STUB IDVDProgramChain_GetUserOperationControlMask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetAudioStreamSelector_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT8 streamN,
    UINT16 __RPC_FAR *pSelector);


void __RPC_STUB IDVDProgramChain_GetAudioStreamSelector_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetSubpictureStreamSelector_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT8 streamN,
    UINT32 __RPC_FAR *pSPSel);


void __RPC_STUB IDVDProgramChain_GetSubpictureStreamSelector_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetLink_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    EPGCLinkType eType,
    UINT16 __RPC_FAR *pgcn);


void __RPC_STUB IDVDProgramChain_GetLink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetPlaybackMode_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    EPGCPlaybackMode __RPC_FAR *pMode,
    UINT8 __RPC_FAR *pRepeatCount,
    UINT8 __RPC_FAR *pStillTime);


void __RPC_STUB IDVDProgramChain_GetPlaybackMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetSubpicturePalette_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT32 __RPC_FAR *pBuf);


void __RPC_STUB IDVDProgramChain_GetSubpicturePalette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetNavCommandsCount_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    ENavCmdArea eArea,
    UINT16 __RPC_FAR *pCount);


void __RPC_STUB IDVDProgramChain_GetNavCommandsCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetNavCommand_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    ENavCmdArea area,
    UINT16 commandN,
    DVD_NAV_CMD __RPC_FAR *pCmd);


void __RPC_STUB IDVDProgramChain_GetNavCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetProgramEntryCell_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT16 pgno,
    UINT8 __RPC_FAR *pEntryCN);


void __RPC_STUB IDVDProgramChain_GetProgramEntryCell_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetCellPlaybackInfo_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT8 cn,
    DVD_CELL_PLAYBACK __RPC_FAR *pInfo);


void __RPC_STUB IDVDProgramChain_GetCellPlaybackInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetCellPosition_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT8 cn,
    UINT16 __RPC_FAR *pVOB_ID,
    UINT8 __RPC_FAR *pC_IDN);


void __RPC_STUB IDVDProgramChain_GetCellPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDProgramChain_GetPGCN_Proxy( 
    IDVDProgramChain __RPC_FAR * This,
    UINT16 __RPC_FAR *pPGCN);


void __RPC_STUB IDVDProgramChain_GetPGCN_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDProgramChain_INTERFACE_DEFINED__ */


#ifndef __IDVDTimeMap_INTERFACE_DEFINED__
#define __IDVDTimeMap_INTERFACE_DEFINED__

/* interface IDVDTimeMap */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDTimeMap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5080-9F3D-11D0-A979-00A0C9050598")
    IDVDTimeMap : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetTableHeader( 
            UINT8 __RPC_FAR *pTMU,
            UINT16 __RPC_FAR *pnEntries) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVOBUAddress( 
            UINT16 nEntryIndex,
            UINT32 __RPC_FAR *pVobuRlbn) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDTimeMapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDTimeMap __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDTimeMap __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDTimeMap __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTableHeader )( 
            IDVDTimeMap __RPC_FAR * This,
            UINT8 __RPC_FAR *pTMU,
            UINT16 __RPC_FAR *pnEntries);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVOBUAddress )( 
            IDVDTimeMap __RPC_FAR * This,
            UINT16 nEntryIndex,
            UINT32 __RPC_FAR *pVobuRlbn);
        
        END_INTERFACE
    } IDVDTimeMapVtbl;

    interface IDVDTimeMap
    {
        CONST_VTBL struct IDVDTimeMapVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDTimeMap_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDTimeMap_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDTimeMap_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDTimeMap_GetTableHeader(This,pTMU,pnEntries)	\
    (This)->lpVtbl -> GetTableHeader(This,pTMU,pnEntries)

#define IDVDTimeMap_GetVOBUAddress(This,nEntryIndex,pVobuRlbn)	\
    (This)->lpVtbl -> GetVOBUAddress(This,nEntryIndex,pVobuRlbn)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDTimeMap_GetTableHeader_Proxy( 
    IDVDTimeMap __RPC_FAR * This,
    UINT8 __RPC_FAR *pTMU,
    UINT16 __RPC_FAR *pnEntries);


void __RPC_STUB IDVDTimeMap_GetTableHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDTimeMap_GetVOBUAddress_Proxy( 
    IDVDTimeMap __RPC_FAR * This,
    UINT16 nEntryIndex,
    UINT32 __RPC_FAR *pVobuRlbn);


void __RPC_STUB IDVDTimeMap_GetVOBUAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDTimeMap_INTERFACE_DEFINED__ */


#ifndef __IDVDVideoVolume_INTERFACE_DEFINED__
#define __IDVDVideoVolume_INTERFACE_DEFINED__

/* interface IDVDVideoVolume */
/* [unique][helpstring][uuid][object] */ 

typedef struct _structDVD_PROVIDER_ID
    {
    UINT8 providerID[ 32 ];
    }	DVD_PROVIDER_ID;

typedef struct _structDVD_POS_CODE
    {
    UINT8 posCode[ 8 ];
    }	DVD_POS_CODE;

typedef struct _structDVD_TITLE_SET_INFO
    {
    UINT16 nParts;
    UINT16 parentalID;
    UINT8 titlePlaybackType;
    UINT8 nAngles;
    UINT8 vtsn;
    UINT8 vts_ttn;
    }	DVD_TITLE_SET_INFO;


EXTERN_C const IID IID_IDVDVideoVolume;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5073-9F3D-11D0-A979-00A0C9050598")
    IDVDVideoVolume : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitData( 
            IDVDROM __RPC_FAR *pDisk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Lock( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Unlock( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSpecVersion( 
            /* [out][ref] */ UINT16 __RPC_FAR *pVersion) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCategory( 
            /* [out][ref] */ UINT32 __RPC_FAR *pCat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVolumeSetInfo( 
            /* [out][ref] */ UINT16 __RPC_FAR *pVolumeSetSize,
            /* [out][ref] */ UINT16 __RPC_FAR *pVolumeNo,
            /* [out][ref] */ UINT8 __RPC_FAR *pSide) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoTitleSetsCount( 
            /* [out][ref] */ UINT16 __RPC_FAR *pCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProviderID( 
            /* [ref][out] */ DVD_PROVIDER_ID __RPC_FAR *pProvider) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPOSCode( 
            /* [ref][out] */ DVD_POS_CODE __RPC_FAR *pPOSCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstPlay( 
            /* [ref][out] */ IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMenu( 
            /* [ref][out] */ IDVDMenu __RPC_FAR *__RPC_FAR *ppMenu) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTotalTitleUnitCount( 
            /* [ref][out] */ UINT16 __RPC_FAR *pTTUCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTitleUnitInfo( 
            /* [in] */ UINT16 ttu,
            /* [ref][out] */ DVD_TITLE_SET_INFO __RPC_FAR *pTUInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTitleSet( 
            UINT16 zbVtsn,
            IDVDTitleSet __RPC_FAR *__RPC_FAR *ppTitleSet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTitleSetAttributes( 
            /* [in] */ UINT16 vtsn,
            /* [ref][out] */ UINT32 __RPC_FAR *pVTSCategory,
            /* [ref][out] */ IDVDStreamAttributes __RPC_FAR *__RPC_FAR *ppVideoAttr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParentalManagementTable( 
            /* [ref][out] */ IDVDParentalManagementTable __RPC_FAR *__RPC_FAR *ppTable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTextDataManager( 
            /* [ref][out] */ IDVDTextManager __RPC_FAR *__RPC_FAR *ppTxtMgr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDVideoVolumeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDVideoVolume __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDVideoVolume __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitData )( 
            IDVDVideoVolume __RPC_FAR * This,
            IDVDROM __RPC_FAR *pDisk);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Lock )( 
            IDVDVideoVolume __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Unlock )( 
            IDVDVideoVolume __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSpecVersion )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [out][ref] */ UINT16 __RPC_FAR *pVersion);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCategory )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [out][ref] */ UINT32 __RPC_FAR *pCat);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVolumeSetInfo )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [out][ref] */ UINT16 __RPC_FAR *pVolumeSetSize,
            /* [out][ref] */ UINT16 __RPC_FAR *pVolumeNo,
            /* [out][ref] */ UINT8 __RPC_FAR *pSide);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoTitleSetsCount )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [out][ref] */ UINT16 __RPC_FAR *pCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetProviderID )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [ref][out] */ DVD_PROVIDER_ID __RPC_FAR *pProvider);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPOSCode )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [ref][out] */ DVD_POS_CODE __RPC_FAR *pPOSCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFirstPlay )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [ref][out] */ IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMenu )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [ref][out] */ IDVDMenu __RPC_FAR *__RPC_FAR *ppMenu);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTotalTitleUnitCount )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [ref][out] */ UINT16 __RPC_FAR *pTTUCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTitleUnitInfo )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [in] */ UINT16 ttu,
            /* [ref][out] */ DVD_TITLE_SET_INFO __RPC_FAR *pTUInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTitleSet )( 
            IDVDVideoVolume __RPC_FAR * This,
            UINT16 zbVtsn,
            IDVDTitleSet __RPC_FAR *__RPC_FAR *ppTitleSet);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTitleSetAttributes )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [in] */ UINT16 vtsn,
            /* [ref][out] */ UINT32 __RPC_FAR *pVTSCategory,
            /* [ref][out] */ IDVDStreamAttributes __RPC_FAR *__RPC_FAR *ppVideoAttr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetParentalManagementTable )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [ref][out] */ IDVDParentalManagementTable __RPC_FAR *__RPC_FAR *ppTable);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTextDataManager )( 
            IDVDVideoVolume __RPC_FAR * This,
            /* [ref][out] */ IDVDTextManager __RPC_FAR *__RPC_FAR *ppTxtMgr);
        
        END_INTERFACE
    } IDVDVideoVolumeVtbl;

    interface IDVDVideoVolume
    {
        CONST_VTBL struct IDVDVideoVolumeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDVideoVolume_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDVideoVolume_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDVideoVolume_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDVideoVolume_InitData(This,pDisk)	\
    (This)->lpVtbl -> InitData(This,pDisk)

#define IDVDVideoVolume_Lock(This)	\
    (This)->lpVtbl -> Lock(This)

#define IDVDVideoVolume_Unlock(This)	\
    (This)->lpVtbl -> Unlock(This)

#define IDVDVideoVolume_GetSpecVersion(This,pVersion)	\
    (This)->lpVtbl -> GetSpecVersion(This,pVersion)

#define IDVDVideoVolume_GetCategory(This,pCat)	\
    (This)->lpVtbl -> GetCategory(This,pCat)

#define IDVDVideoVolume_GetVolumeSetInfo(This,pVolumeSetSize,pVolumeNo,pSide)	\
    (This)->lpVtbl -> GetVolumeSetInfo(This,pVolumeSetSize,pVolumeNo,pSide)

#define IDVDVideoVolume_GetVideoTitleSetsCount(This,pCount)	\
    (This)->lpVtbl -> GetVideoTitleSetsCount(This,pCount)

#define IDVDVideoVolume_GetProviderID(This,pProvider)	\
    (This)->lpVtbl -> GetProviderID(This,pProvider)

#define IDVDVideoVolume_GetPOSCode(This,pPOSCode)	\
    (This)->lpVtbl -> GetPOSCode(This,pPOSCode)

#define IDVDVideoVolume_GetFirstPlay(This,ppDomain)	\
    (This)->lpVtbl -> GetFirstPlay(This,ppDomain)

#define IDVDVideoVolume_GetMenu(This,ppMenu)	\
    (This)->lpVtbl -> GetMenu(This,ppMenu)

#define IDVDVideoVolume_GetTotalTitleUnitCount(This,pTTUCount)	\
    (This)->lpVtbl -> GetTotalTitleUnitCount(This,pTTUCount)

#define IDVDVideoVolume_GetTitleUnitInfo(This,ttu,pTUInfo)	\
    (This)->lpVtbl -> GetTitleUnitInfo(This,ttu,pTUInfo)

#define IDVDVideoVolume_GetTitleSet(This,zbVtsn,ppTitleSet)	\
    (This)->lpVtbl -> GetTitleSet(This,zbVtsn,ppTitleSet)

#define IDVDVideoVolume_GetTitleSetAttributes(This,vtsn,pVTSCategory,ppVideoAttr)	\
    (This)->lpVtbl -> GetTitleSetAttributes(This,vtsn,pVTSCategory,ppVideoAttr)

#define IDVDVideoVolume_GetParentalManagementTable(This,ppTable)	\
    (This)->lpVtbl -> GetParentalManagementTable(This,ppTable)

#define IDVDVideoVolume_GetTextDataManager(This,ppTxtMgr)	\
    (This)->lpVtbl -> GetTextDataManager(This,ppTxtMgr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDVideoVolume_InitData_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    IDVDROM __RPC_FAR *pDisk);


void __RPC_STUB IDVDVideoVolume_InitData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_Lock_Proxy( 
    IDVDVideoVolume __RPC_FAR * This);


void __RPC_STUB IDVDVideoVolume_Lock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_Unlock_Proxy( 
    IDVDVideoVolume __RPC_FAR * This);


void __RPC_STUB IDVDVideoVolume_Unlock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetSpecVersion_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [out][ref] */ UINT16 __RPC_FAR *pVersion);


void __RPC_STUB IDVDVideoVolume_GetSpecVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetCategory_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [out][ref] */ UINT32 __RPC_FAR *pCat);


void __RPC_STUB IDVDVideoVolume_GetCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetVolumeSetInfo_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [out][ref] */ UINT16 __RPC_FAR *pVolumeSetSize,
    /* [out][ref] */ UINT16 __RPC_FAR *pVolumeNo,
    /* [out][ref] */ UINT8 __RPC_FAR *pSide);


void __RPC_STUB IDVDVideoVolume_GetVolumeSetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetVideoTitleSetsCount_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [out][ref] */ UINT16 __RPC_FAR *pCount);


void __RPC_STUB IDVDVideoVolume_GetVideoTitleSetsCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetProviderID_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [ref][out] */ DVD_PROVIDER_ID __RPC_FAR *pProvider);


void __RPC_STUB IDVDVideoVolume_GetProviderID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetPOSCode_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [ref][out] */ DVD_POS_CODE __RPC_FAR *pPOSCode);


void __RPC_STUB IDVDVideoVolume_GetPOSCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetFirstPlay_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [ref][out] */ IDVDDomain __RPC_FAR *__RPC_FAR *ppDomain);


void __RPC_STUB IDVDVideoVolume_GetFirstPlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetMenu_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [ref][out] */ IDVDMenu __RPC_FAR *__RPC_FAR *ppMenu);


void __RPC_STUB IDVDVideoVolume_GetMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetTotalTitleUnitCount_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [ref][out] */ UINT16 __RPC_FAR *pTTUCount);


void __RPC_STUB IDVDVideoVolume_GetTotalTitleUnitCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetTitleUnitInfo_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [in] */ UINT16 ttu,
    /* [ref][out] */ DVD_TITLE_SET_INFO __RPC_FAR *pTUInfo);


void __RPC_STUB IDVDVideoVolume_GetTitleUnitInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetTitleSet_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    UINT16 zbVtsn,
    IDVDTitleSet __RPC_FAR *__RPC_FAR *ppTitleSet);


void __RPC_STUB IDVDVideoVolume_GetTitleSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetTitleSetAttributes_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [in] */ UINT16 vtsn,
    /* [ref][out] */ UINT32 __RPC_FAR *pVTSCategory,
    /* [ref][out] */ IDVDStreamAttributes __RPC_FAR *__RPC_FAR *ppVideoAttr);


void __RPC_STUB IDVDVideoVolume_GetTitleSetAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetParentalManagementTable_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [ref][out] */ IDVDParentalManagementTable __RPC_FAR *__RPC_FAR *ppTable);


void __RPC_STUB IDVDVideoVolume_GetParentalManagementTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDVideoVolume_GetTextDataManager_Proxy( 
    IDVDVideoVolume __RPC_FAR * This,
    /* [ref][out] */ IDVDTextManager __RPC_FAR *__RPC_FAR *ppTxtMgr);


void __RPC_STUB IDVDVideoVolume_GetTextDataManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDVideoVolume_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_dvdata_0104 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_dvdata_0104_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dvdata_0104_v0_0_s_ifspec;

#ifndef __IDVDDataSearchInfo_INTERFACE_DEFINED__
#define __IDVDDataSearchInfo_INTERFACE_DEFINED__

/* interface IDVDDataSearchInfo */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDDataSearchInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5081-9F3D-11D0-A979-00A0C9050598")
    IDVDDataSearchInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetControlInfo( 
            IDVDStreamControlInfo __RPC_FAR *__RPC_FAR *ppStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSCRBase( 
            UINT32 __RPC_FAR *pSCR) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUnitEndAddress( 
            UINT32 __RPC_FAR *pRLBN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRefPictureAddress( 
            UINT8 refNo,
            UINT32 __RPC_FAR *pRLBN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVOB_ID( 
            UINT16 __RPC_FAR *pVOB_ID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetC_IDN( 
            UINT8 __RPC_FAR *pC_IDN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSeamlessCategory( 
            UINT16 __RPC_FAR *pCat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInterleavedUnitEndAddress( 
            UINT32 __RPC_FAR *pRLBN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextInterleavedUnit( 
            UINT32 __RPC_FAR *pRLBN,
            UINT16 __RPC_FAR *pLBSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAudioGapInfo( 
            UINT8 streamNo,
            UINT8 gapNo,
            UINT32 __RPC_FAR *pMpegStart,
            UINT32 __RPC_FAR *pMpegEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSeamlessAngleChangeAddress( 
            UINT8 angle,
            UINT32 __RPC_FAR *pRLBN,
            UINT32 __RPC_FAR *pLBSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUnitStepAddress( 
            UINT8 dir,
            INT16 stepIndex,
            UINT32 __RPC_FAR *pAddress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSyncAudioAddress( 
            UINT8 streamN,
            UINT16 __RPC_FAR *pRLBN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSyncSPAddress( 
            UINT8 streamN,
            UINT32 __RPC_FAR *pRLBN) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDDataSearchInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDDataSearchInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDDataSearchInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetControlInfo )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            IDVDStreamControlInfo __RPC_FAR *__RPC_FAR *ppStream);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSCRBase )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *pSCR);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetUnitEndAddress )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *pRLBN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRefPictureAddress )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT8 refNo,
            UINT32 __RPC_FAR *pRLBN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVOB_ID )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT16 __RPC_FAR *pVOB_ID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetC_IDN )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT8 __RPC_FAR *pC_IDN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSeamlessCategory )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT16 __RPC_FAR *pCat);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetInterleavedUnitEndAddress )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *pRLBN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNextInterleavedUnit )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *pRLBN,
            UINT16 __RPC_FAR *pLBSize);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAudioGapInfo )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT8 streamNo,
            UINT8 gapNo,
            UINT32 __RPC_FAR *pMpegStart,
            UINT32 __RPC_FAR *pMpegEnd);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSeamlessAngleChangeAddress )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT8 angle,
            UINT32 __RPC_FAR *pRLBN,
            UINT32 __RPC_FAR *pLBSize);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetUnitStepAddress )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT8 dir,
            INT16 stepIndex,
            UINT32 __RPC_FAR *pAddress);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSyncAudioAddress )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT8 streamN,
            UINT16 __RPC_FAR *pRLBN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSyncSPAddress )( 
            IDVDDataSearchInfo __RPC_FAR * This,
            UINT8 streamN,
            UINT32 __RPC_FAR *pRLBN);
        
        END_INTERFACE
    } IDVDDataSearchInfoVtbl;

    interface IDVDDataSearchInfo
    {
        CONST_VTBL struct IDVDDataSearchInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDDataSearchInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDDataSearchInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDDataSearchInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDDataSearchInfo_GetControlInfo(This,ppStream)	\
    (This)->lpVtbl -> GetControlInfo(This,ppStream)

#define IDVDDataSearchInfo_GetSCRBase(This,pSCR)	\
    (This)->lpVtbl -> GetSCRBase(This,pSCR)

#define IDVDDataSearchInfo_GetUnitEndAddress(This,pRLBN)	\
    (This)->lpVtbl -> GetUnitEndAddress(This,pRLBN)

#define IDVDDataSearchInfo_GetRefPictureAddress(This,refNo,pRLBN)	\
    (This)->lpVtbl -> GetRefPictureAddress(This,refNo,pRLBN)

#define IDVDDataSearchInfo_GetVOB_ID(This,pVOB_ID)	\
    (This)->lpVtbl -> GetVOB_ID(This,pVOB_ID)

#define IDVDDataSearchInfo_GetC_IDN(This,pC_IDN)	\
    (This)->lpVtbl -> GetC_IDN(This,pC_IDN)

#define IDVDDataSearchInfo_GetSeamlessCategory(This,pCat)	\
    (This)->lpVtbl -> GetSeamlessCategory(This,pCat)

#define IDVDDataSearchInfo_GetInterleavedUnitEndAddress(This,pRLBN)	\
    (This)->lpVtbl -> GetInterleavedUnitEndAddress(This,pRLBN)

#define IDVDDataSearchInfo_GetNextInterleavedUnit(This,pRLBN,pLBSize)	\
    (This)->lpVtbl -> GetNextInterleavedUnit(This,pRLBN,pLBSize)

#define IDVDDataSearchInfo_GetAudioGapInfo(This,streamNo,gapNo,pMpegStart,pMpegEnd)	\
    (This)->lpVtbl -> GetAudioGapInfo(This,streamNo,gapNo,pMpegStart,pMpegEnd)

#define IDVDDataSearchInfo_GetSeamlessAngleChangeAddress(This,angle,pRLBN,pLBSize)	\
    (This)->lpVtbl -> GetSeamlessAngleChangeAddress(This,angle,pRLBN,pLBSize)

#define IDVDDataSearchInfo_GetUnitStepAddress(This,dir,stepIndex,pAddress)	\
    (This)->lpVtbl -> GetUnitStepAddress(This,dir,stepIndex,pAddress)

#define IDVDDataSearchInfo_GetSyncAudioAddress(This,streamN,pRLBN)	\
    (This)->lpVtbl -> GetSyncAudioAddress(This,streamN,pRLBN)

#define IDVDDataSearchInfo_GetSyncSPAddress(This,streamN,pRLBN)	\
    (This)->lpVtbl -> GetSyncSPAddress(This,streamN,pRLBN)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetControlInfo_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    IDVDStreamControlInfo __RPC_FAR *__RPC_FAR *ppStream);


void __RPC_STUB IDVDDataSearchInfo_GetControlInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetSCRBase_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *pSCR);


void __RPC_STUB IDVDDataSearchInfo_GetSCRBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetUnitEndAddress_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *pRLBN);


void __RPC_STUB IDVDDataSearchInfo_GetUnitEndAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetRefPictureAddress_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT8 refNo,
    UINT32 __RPC_FAR *pRLBN);


void __RPC_STUB IDVDDataSearchInfo_GetRefPictureAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetVOB_ID_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT16 __RPC_FAR *pVOB_ID);


void __RPC_STUB IDVDDataSearchInfo_GetVOB_ID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetC_IDN_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT8 __RPC_FAR *pC_IDN);


void __RPC_STUB IDVDDataSearchInfo_GetC_IDN_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetSeamlessCategory_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT16 __RPC_FAR *pCat);


void __RPC_STUB IDVDDataSearchInfo_GetSeamlessCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetInterleavedUnitEndAddress_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *pRLBN);


void __RPC_STUB IDVDDataSearchInfo_GetInterleavedUnitEndAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetNextInterleavedUnit_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *pRLBN,
    UINT16 __RPC_FAR *pLBSize);


void __RPC_STUB IDVDDataSearchInfo_GetNextInterleavedUnit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetAudioGapInfo_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT8 streamNo,
    UINT8 gapNo,
    UINT32 __RPC_FAR *pMpegStart,
    UINT32 __RPC_FAR *pMpegEnd);


void __RPC_STUB IDVDDataSearchInfo_GetAudioGapInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetSeamlessAngleChangeAddress_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT8 angle,
    UINT32 __RPC_FAR *pRLBN,
    UINT32 __RPC_FAR *pLBSize);


void __RPC_STUB IDVDDataSearchInfo_GetSeamlessAngleChangeAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetUnitStepAddress_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT8 dir,
    INT16 stepIndex,
    UINT32 __RPC_FAR *pAddress);


void __RPC_STUB IDVDDataSearchInfo_GetUnitStepAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetSyncAudioAddress_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT8 streamN,
    UINT16 __RPC_FAR *pRLBN);


void __RPC_STUB IDVDDataSearchInfo_GetSyncAudioAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDDataSearchInfo_GetSyncSPAddress_Proxy( 
    IDVDDataSearchInfo __RPC_FAR * This,
    UINT8 streamN,
    UINT32 __RPC_FAR *pRLBN);


void __RPC_STUB IDVDDataSearchInfo_GetSyncSPAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDDataSearchInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_dvdata_0105 */
/* [local] */ 

typedef /* [public][public] */ struct __MIDL___MIDL_itf_dvdata_0105_0001
    {
    UINT16 xStart;
    UINT16 xEnd;
    UINT16 yStart;
    UINT16 yEnd;
    }	DVD_RECT;



extern RPC_IF_HANDLE __MIDL_itf_dvdata_0105_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dvdata_0105_v0_0_s_ifspec;

#ifndef __IDVDHighlightInfo_INTERFACE_DEFINED__
#define __IDVDHighlightInfo_INTERFACE_DEFINED__

/* interface IDVDHighlightInfo */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDHighlightInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5082-9F3D-11D0-A979-00A0C9050598")
    IDVDHighlightInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetStreamingInfo( 
            IDVDStreamControlInfo __RPC_FAR *__RPC_FAR *pControlInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPTMs( 
            UINT32 __RPC_FAR *puiStart,
            UINT32 __RPC_FAR *puiEnd,
            UINT32 __RPC_FAR *puiSelectionEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetButtonMode( 
            UINT16 __RPC_FAR *pMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetButtonOffset( 
            UINT8 __RPC_FAR *pbyteOffset) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetButtonCount( 
            UINT8 __RPC_FAR *pbyteCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNumberedButtonCount( 
            UINT8 __RPC_FAR *pbyteCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInitialSelectionButton( 
            UINT8 __RPC_FAR *pInitialSelection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultActionButton( 
            UINT8 __RPC_FAR *pActionButton) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetButtonColorInfo( 
            UINT8 byteButtonColor,
            UINT32 __RPC_FAR *pSelectionColors,
            UINT32 __RPC_FAR *pActionColors) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetButtonPositionInfo( 
            UINT8 byteButton,
            BYTE __RPC_FAR *pbyteButtonColor,
            UINT8 __RPC_FAR *pbyteActionMode,
            DVD_RECT __RPC_FAR *pArea) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAdjacentButton( 
            UINT8 byteButton,
            UINT8 byteDir,
            UINT8 __RPC_FAR *pbyteButtonNumber) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetButtonCommand( 
            UINT8 byteButtonNumber,
            DVD_NAV_CMD __RPC_FAR *pCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHLIStatus( 
            UINT16 __RPC_FAR *pStatus) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDHighlightInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDHighlightInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDHighlightInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDHighlightInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetStreamingInfo )( 
            IDVDHighlightInfo __RPC_FAR * This,
            IDVDStreamControlInfo __RPC_FAR *__RPC_FAR *pControlInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPTMs )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *puiStart,
            UINT32 __RPC_FAR *puiEnd,
            UINT32 __RPC_FAR *puiSelectionEnd);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetButtonMode )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT16 __RPC_FAR *pMode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetButtonOffset )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT8 __RPC_FAR *pbyteOffset);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetButtonCount )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT8 __RPC_FAR *pbyteCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNumberedButtonCount )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT8 __RPC_FAR *pbyteCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetInitialSelectionButton )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT8 __RPC_FAR *pInitialSelection);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDefaultActionButton )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT8 __RPC_FAR *pActionButton);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetButtonColorInfo )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT8 byteButtonColor,
            UINT32 __RPC_FAR *pSelectionColors,
            UINT32 __RPC_FAR *pActionColors);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetButtonPositionInfo )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT8 byteButton,
            BYTE __RPC_FAR *pbyteButtonColor,
            UINT8 __RPC_FAR *pbyteActionMode,
            DVD_RECT __RPC_FAR *pArea);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAdjacentButton )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT8 byteButton,
            UINT8 byteDir,
            UINT8 __RPC_FAR *pbyteButtonNumber);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetButtonCommand )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT8 byteButtonNumber,
            DVD_NAV_CMD __RPC_FAR *pCmd);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHLIStatus )( 
            IDVDHighlightInfo __RPC_FAR * This,
            UINT16 __RPC_FAR *pStatus);
        
        END_INTERFACE
    } IDVDHighlightInfoVtbl;

    interface IDVDHighlightInfo
    {
        CONST_VTBL struct IDVDHighlightInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDHighlightInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDHighlightInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDHighlightInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDHighlightInfo_GetStreamingInfo(This,pControlInfo)	\
    (This)->lpVtbl -> GetStreamingInfo(This,pControlInfo)

#define IDVDHighlightInfo_GetPTMs(This,puiStart,puiEnd,puiSelectionEnd)	\
    (This)->lpVtbl -> GetPTMs(This,puiStart,puiEnd,puiSelectionEnd)

#define IDVDHighlightInfo_GetButtonMode(This,pMode)	\
    (This)->lpVtbl -> GetButtonMode(This,pMode)

#define IDVDHighlightInfo_GetButtonOffset(This,pbyteOffset)	\
    (This)->lpVtbl -> GetButtonOffset(This,pbyteOffset)

#define IDVDHighlightInfo_GetButtonCount(This,pbyteCount)	\
    (This)->lpVtbl -> GetButtonCount(This,pbyteCount)

#define IDVDHighlightInfo_GetNumberedButtonCount(This,pbyteCount)	\
    (This)->lpVtbl -> GetNumberedButtonCount(This,pbyteCount)

#define IDVDHighlightInfo_GetInitialSelectionButton(This,pInitialSelection)	\
    (This)->lpVtbl -> GetInitialSelectionButton(This,pInitialSelection)

#define IDVDHighlightInfo_GetDefaultActionButton(This,pActionButton)	\
    (This)->lpVtbl -> GetDefaultActionButton(This,pActionButton)

#define IDVDHighlightInfo_GetButtonColorInfo(This,byteButtonColor,pSelectionColors,pActionColors)	\
    (This)->lpVtbl -> GetButtonColorInfo(This,byteButtonColor,pSelectionColors,pActionColors)

#define IDVDHighlightInfo_GetButtonPositionInfo(This,byteButton,pbyteButtonColor,pbyteActionMode,pArea)	\
    (This)->lpVtbl -> GetButtonPositionInfo(This,byteButton,pbyteButtonColor,pbyteActionMode,pArea)

#define IDVDHighlightInfo_GetAdjacentButton(This,byteButton,byteDir,pbyteButtonNumber)	\
    (This)->lpVtbl -> GetAdjacentButton(This,byteButton,byteDir,pbyteButtonNumber)

#define IDVDHighlightInfo_GetButtonCommand(This,byteButtonNumber,pCmd)	\
    (This)->lpVtbl -> GetButtonCommand(This,byteButtonNumber,pCmd)

#define IDVDHighlightInfo_GetHLIStatus(This,pStatus)	\
    (This)->lpVtbl -> GetHLIStatus(This,pStatus)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetStreamingInfo_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    IDVDStreamControlInfo __RPC_FAR *__RPC_FAR *pControlInfo);


void __RPC_STUB IDVDHighlightInfo_GetStreamingInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetPTMs_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *puiStart,
    UINT32 __RPC_FAR *puiEnd,
    UINT32 __RPC_FAR *puiSelectionEnd);


void __RPC_STUB IDVDHighlightInfo_GetPTMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetButtonMode_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT16 __RPC_FAR *pMode);


void __RPC_STUB IDVDHighlightInfo_GetButtonMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetButtonOffset_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT8 __RPC_FAR *pbyteOffset);


void __RPC_STUB IDVDHighlightInfo_GetButtonOffset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetButtonCount_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT8 __RPC_FAR *pbyteCount);


void __RPC_STUB IDVDHighlightInfo_GetButtonCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetNumberedButtonCount_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT8 __RPC_FAR *pbyteCount);


void __RPC_STUB IDVDHighlightInfo_GetNumberedButtonCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetInitialSelectionButton_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT8 __RPC_FAR *pInitialSelection);


void __RPC_STUB IDVDHighlightInfo_GetInitialSelectionButton_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetDefaultActionButton_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT8 __RPC_FAR *pActionButton);


void __RPC_STUB IDVDHighlightInfo_GetDefaultActionButton_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetButtonColorInfo_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT8 byteButtonColor,
    UINT32 __RPC_FAR *pSelectionColors,
    UINT32 __RPC_FAR *pActionColors);


void __RPC_STUB IDVDHighlightInfo_GetButtonColorInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetButtonPositionInfo_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT8 byteButton,
    BYTE __RPC_FAR *pbyteButtonColor,
    UINT8 __RPC_FAR *pbyteActionMode,
    DVD_RECT __RPC_FAR *pArea);


void __RPC_STUB IDVDHighlightInfo_GetButtonPositionInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetAdjacentButton_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT8 byteButton,
    UINT8 byteDir,
    UINT8 __RPC_FAR *pbyteButtonNumber);


void __RPC_STUB IDVDHighlightInfo_GetAdjacentButton_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetButtonCommand_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT8 byteButtonNumber,
    DVD_NAV_CMD __RPC_FAR *pCmd);


void __RPC_STUB IDVDHighlightInfo_GetButtonCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDHighlightInfo_GetHLIStatus_Proxy( 
    IDVDHighlightInfo __RPC_FAR * This,
    UINT16 __RPC_FAR *pStatus);


void __RPC_STUB IDVDHighlightInfo_GetHLIStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDHighlightInfo_INTERFACE_DEFINED__ */


#ifndef __IDVDStreamControlInfo_INTERFACE_DEFINED__
#define __IDVDStreamControlInfo_INTERFACE_DEFINED__

/* interface IDVDStreamControlInfo */
/* [unique][helpstring][uuid][object] */ 

typedef /* [public][public][public][public] */ struct __MIDL_IDVDStreamControlInfo_0001
    {
    UINT8 valid;
    UINT8 countryCode[ 2 ];
    UINT8 copyrightHolder[ 3 ];
    UINT8 bcdYear;
    UINT8 bcdNumber[ 3 ];
    }	DVD_ISRC;


EXTERN_C const IID IID_IDVDStreamControlInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5083-9F3D-11D0-A979-00A0C9050598")
    IDVDStreamControlInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetNavigationPack( 
            UINT8 __RPC_FAR *pPack) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBlockAddress( 
            UINT32 __RPC_FAR *pLBN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCategory( 
            UINT16 __RPC_FAR *pCat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUserOperationControlMask( 
            UINT32 __RPC_FAR *pMask) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPresentationTimes( 
            UINT32 __RPC_FAR *pMpegStart,
            UINT32 __RPC_FAR *pMpegEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEndSequencePTM( 
            UINT32 __RPC_FAR *pMpegTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCellRelativeStartTime( 
            UINT32 __RPC_FAR *pBCDTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNonSeamlessAngleChangeAddress( 
            UINT8 angle,
            UINT32 __RPC_FAR *pRLBN) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHighlightInfo( 
            IDVDHighlightInfo __RPC_FAR *__RPC_FAR *ppHLI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDataSearchInfo( 
            IDVDDataSearchInfo __RPC_FAR *__RPC_FAR *ppDSI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRecordingVideoInfo( 
            DVD_ISRC __RPC_FAR *pVideo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRecordingAudioInfo( 
            UINT8 stream,
            DVD_ISRC __RPC_FAR *pAudio) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRecordingSubpictureGroup( 
            UINT8 __RPC_FAR *pSelector) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRecordingSubpictureInfo( 
            UINT8 bin,
            DVD_ISRC __RPC_FAR *pSubInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReaderLock( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReaderUnlock( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDStreamControlInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDStreamControlInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDStreamControlInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetNavigationPack )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT8 __RPC_FAR *pPack);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBlockAddress )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *pLBN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCategory )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT16 __RPC_FAR *pCat);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetUserOperationControlMask )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *pMask);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPresentationTimes )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *pMpegStart,
            UINT32 __RPC_FAR *pMpegEnd);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEndSequencePTM )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *pMpegTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCellRelativeStartTime )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT32 __RPC_FAR *pBCDTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNonSeamlessAngleChangeAddress )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT8 angle,
            UINT32 __RPC_FAR *pRLBN);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHighlightInfo )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            IDVDHighlightInfo __RPC_FAR *__RPC_FAR *ppHLI);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDataSearchInfo )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            IDVDDataSearchInfo __RPC_FAR *__RPC_FAR *ppDSI);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRecordingVideoInfo )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            DVD_ISRC __RPC_FAR *pVideo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRecordingAudioInfo )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT8 stream,
            DVD_ISRC __RPC_FAR *pAudio);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRecordingSubpictureGroup )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT8 __RPC_FAR *pSelector);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRecordingSubpictureInfo )( 
            IDVDStreamControlInfo __RPC_FAR * This,
            UINT8 bin,
            DVD_ISRC __RPC_FAR *pSubInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReaderLock )( 
            IDVDStreamControlInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReaderUnlock )( 
            IDVDStreamControlInfo __RPC_FAR * This);
        
        END_INTERFACE
    } IDVDStreamControlInfoVtbl;

    interface IDVDStreamControlInfo
    {
        CONST_VTBL struct IDVDStreamControlInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDStreamControlInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDStreamControlInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDStreamControlInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDStreamControlInfo_SetNavigationPack(This,pPack)	\
    (This)->lpVtbl -> SetNavigationPack(This,pPack)

#define IDVDStreamControlInfo_GetBlockAddress(This,pLBN)	\
    (This)->lpVtbl -> GetBlockAddress(This,pLBN)

#define IDVDStreamControlInfo_GetCategory(This,pCat)	\
    (This)->lpVtbl -> GetCategory(This,pCat)

#define IDVDStreamControlInfo_GetUserOperationControlMask(This,pMask)	\
    (This)->lpVtbl -> GetUserOperationControlMask(This,pMask)

#define IDVDStreamControlInfo_GetPresentationTimes(This,pMpegStart,pMpegEnd)	\
    (This)->lpVtbl -> GetPresentationTimes(This,pMpegStart,pMpegEnd)

#define IDVDStreamControlInfo_GetEndSequencePTM(This,pMpegTime)	\
    (This)->lpVtbl -> GetEndSequencePTM(This,pMpegTime)

#define IDVDStreamControlInfo_GetCellRelativeStartTime(This,pBCDTime)	\
    (This)->lpVtbl -> GetCellRelativeStartTime(This,pBCDTime)

#define IDVDStreamControlInfo_GetNonSeamlessAngleChangeAddress(This,angle,pRLBN)	\
    (This)->lpVtbl -> GetNonSeamlessAngleChangeAddress(This,angle,pRLBN)

#define IDVDStreamControlInfo_GetHighlightInfo(This,ppHLI)	\
    (This)->lpVtbl -> GetHighlightInfo(This,ppHLI)

#define IDVDStreamControlInfo_GetDataSearchInfo(This,ppDSI)	\
    (This)->lpVtbl -> GetDataSearchInfo(This,ppDSI)

#define IDVDStreamControlInfo_GetRecordingVideoInfo(This,pVideo)	\
    (This)->lpVtbl -> GetRecordingVideoInfo(This,pVideo)

#define IDVDStreamControlInfo_GetRecordingAudioInfo(This,stream,pAudio)	\
    (This)->lpVtbl -> GetRecordingAudioInfo(This,stream,pAudio)

#define IDVDStreamControlInfo_GetRecordingSubpictureGroup(This,pSelector)	\
    (This)->lpVtbl -> GetRecordingSubpictureGroup(This,pSelector)

#define IDVDStreamControlInfo_GetRecordingSubpictureInfo(This,bin,pSubInfo)	\
    (This)->lpVtbl -> GetRecordingSubpictureInfo(This,bin,pSubInfo)

#define IDVDStreamControlInfo_ReaderLock(This)	\
    (This)->lpVtbl -> ReaderLock(This)

#define IDVDStreamControlInfo_ReaderUnlock(This)	\
    (This)->lpVtbl -> ReaderUnlock(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_SetNavigationPack_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT8 __RPC_FAR *pPack);


void __RPC_STUB IDVDStreamControlInfo_SetNavigationPack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetBlockAddress_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *pLBN);


void __RPC_STUB IDVDStreamControlInfo_GetBlockAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetCategory_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT16 __RPC_FAR *pCat);


void __RPC_STUB IDVDStreamControlInfo_GetCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetUserOperationControlMask_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *pMask);


void __RPC_STUB IDVDStreamControlInfo_GetUserOperationControlMask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetPresentationTimes_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *pMpegStart,
    UINT32 __RPC_FAR *pMpegEnd);


void __RPC_STUB IDVDStreamControlInfo_GetPresentationTimes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetEndSequencePTM_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *pMpegTime);


void __RPC_STUB IDVDStreamControlInfo_GetEndSequencePTM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetCellRelativeStartTime_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT32 __RPC_FAR *pBCDTime);


void __RPC_STUB IDVDStreamControlInfo_GetCellRelativeStartTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetNonSeamlessAngleChangeAddress_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT8 angle,
    UINT32 __RPC_FAR *pRLBN);


void __RPC_STUB IDVDStreamControlInfo_GetNonSeamlessAngleChangeAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetHighlightInfo_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    IDVDHighlightInfo __RPC_FAR *__RPC_FAR *ppHLI);


void __RPC_STUB IDVDStreamControlInfo_GetHighlightInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetDataSearchInfo_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    IDVDDataSearchInfo __RPC_FAR *__RPC_FAR *ppDSI);


void __RPC_STUB IDVDStreamControlInfo_GetDataSearchInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetRecordingVideoInfo_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    DVD_ISRC __RPC_FAR *pVideo);


void __RPC_STUB IDVDStreamControlInfo_GetRecordingVideoInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetRecordingAudioInfo_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT8 stream,
    DVD_ISRC __RPC_FAR *pAudio);


void __RPC_STUB IDVDStreamControlInfo_GetRecordingAudioInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetRecordingSubpictureGroup_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT8 __RPC_FAR *pSelector);


void __RPC_STUB IDVDStreamControlInfo_GetRecordingSubpictureGroup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_GetRecordingSubpictureInfo_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This,
    UINT8 bin,
    DVD_ISRC __RPC_FAR *pSubInfo);


void __RPC_STUB IDVDStreamControlInfo_GetRecordingSubpictureInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_ReaderLock_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This);


void __RPC_STUB IDVDStreamControlInfo_ReaderLock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVDStreamControlInfo_ReaderUnlock_Proxy( 
    IDVDStreamControlInfo __RPC_FAR * This);


void __RPC_STUB IDVDStreamControlInfo_ReaderUnlock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDStreamControlInfo_INTERFACE_DEFINED__ */


#ifndef __IDVDNavDataStatusSink_INTERFACE_DEFINED__
#define __IDVDNavDataStatusSink_INTERFACE_DEFINED__

/* interface IDVDNavDataStatusSink */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDVDNavDataStatusSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77FB5085-9F3D-11D0-A979-00A0C9050598")
    IDVDNavDataStatusSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE NotifyNewPCI( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVDNavDataStatusSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDVDNavDataStatusSink __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDVDNavDataStatusSink __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDVDNavDataStatusSink __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NotifyNewPCI )( 
            IDVDNavDataStatusSink __RPC_FAR * This);
        
        END_INTERFACE
    } IDVDNavDataStatusSinkVtbl;

    interface IDVDNavDataStatusSink
    {
        CONST_VTBL struct IDVDNavDataStatusSinkVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVDNavDataStatusSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVDNavDataStatusSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVDNavDataStatusSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVDNavDataStatusSink_NotifyNewPCI(This)	\
    (This)->lpVtbl -> NotifyNewPCI(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVDNavDataStatusSink_NotifyNewPCI_Proxy( 
    IDVDNavDataStatusSink __RPC_FAR * This);


void __RPC_STUB IDVDNavDataStatusSink_NotifyNewPCI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVDNavDataStatusSink_INTERFACE_DEFINED__ */



#ifndef __DVDATALib_LIBRARY_DEFINED__
#define __DVDATALib_LIBRARY_DEFINED__

/* library DVDATALib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DVDATALib;

EXTERN_C const CLSID CLSID_DVDData;

#ifdef __cplusplus

class DECLSPEC_UUID("77FB5076-9F3D-11D0-A979-00A0C9050598")
DVDData;
#endif

EXTERN_C const CLSID CLSID_DVDStreamControlInfo;

#ifdef __cplusplus

class DECLSPEC_UUID("77FB5084-9F3D-11D0-A979-00A0C9050598")
DVDStreamControlInfo;
#endif
#endif /* __DVDATALib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


