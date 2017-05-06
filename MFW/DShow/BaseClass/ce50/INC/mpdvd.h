
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Thu Jun 24 19:41:56 2004
 */
/* Compiler settings for .\mpdvd.idl:
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

#ifndef __mpdvd_h__
#define __mpdvd_h__

/* Forward Declarations */ 

#ifndef __IMediaPlayerDvd_FWD_DEFINED__
#define __IMediaPlayerDvd_FWD_DEFINED__
typedef interface IMediaPlayerDvd IMediaPlayerDvd;
#endif 	/* __IMediaPlayerDvd_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_mpdvd_0000 */
/* [local] */ 

typedef /* [public] */ 
enum DVD_DomainConstants
    {	dvdDomain_FirstPlay	= 1,
	dvdDomain_VideoManagerMenu	= dvdDomain_FirstPlay + 1,
	dvdDomain_VideoTitleSetMenu	= dvdDomain_VideoManagerMenu + 1,
	dvdDomain_Title	= dvdDomain_VideoTitleSetMenu + 1,
	dvdDomain_Stop	= dvdDomain_Title + 1
    }	DVD_DomainConstants;

typedef /* [public] */ 
enum DVDMenuIDConstants
    {	dvdMenu_Title	= 2,
	dvdMenu_Root	= dvdMenu_Title + 1,
	dvdMenu_Subpicture	= dvdMenu_Root + 1,
	dvdMenu_Audio	= dvdMenu_Subpicture + 1,
	dvdMenu_Angle	= dvdMenu_Audio + 1,
	dvdMenu_Chapter	= dvdMenu_Angle + 1
    }	DVDMenuIDConstants;

typedef /* [public] */ 
enum DVDEventConstants
    {	dvdEvent_DomainChange	= 0x100 + 0x1,
	dvdEvent_TitleChange	= 0x100 + 0x2,
	dvdEvent_ChapterStart	= 0x100 + 0x3,
	dvdEvent_AudioStreamChange	= 0x100 + 0x4,
	dvdEvent_SubPictureStreamChange	= 0x100 + 0x5,
	dvdEvent_AngleChange	= 0x100 + 0x6,
	dvdEvent_ButtonChange	= 0x100 + 0x7,
	dvdEvent_ValidUOPSChange	= 0x100 + 0x8,
	dvdEvent_StillOn	= 0x100 + 0x9,
	dvdEvent_StillOff	= 0x100 + 0xa,
	dvdEvent_CurrentTime	= 0x100 + 0xb,
	dvdEvent_Error	= 0x100 + 0xc,
	dvdEvent_Warning	= 0x100 + 0xd,
	dvdEvent_ChapterAutoStop	= 0x100 + 0xe,
	dvdEvent_NoFirstPlayPGC	= 0x100 + 0xf,
	dvdEvent_RateChange	= 0x100 + 0x10,
	dvdEvent_ParentalLevelChange	= 0x100 + 0x11,
	dvdEvent_PlaybackStopped	= 0x100 + 0x12,
	dvdEvent_AnglesAvailable	= 0x100 + 0x13
    }	DVDEventConstants;



extern RPC_IF_HANDLE __MIDL_itf_mpdvd_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mpdvd_0000_v0_0_s_ifspec;

#ifndef __IMediaPlayerDvd_INTERFACE_DEFINED__
#define __IMediaPlayerDvd_INTERFACE_DEFINED__

/* interface IMediaPlayerDvd */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_IMediaPlayerDvd;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("746EB440-3835-11d2-9774-0000F80855E6")
    IMediaPlayerDvd : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ButtonSelectAndActivate( 
            /* [in] */ ULONG uiButton) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UpperButtonSelect( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LowerButtonSelect( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LeftButtonSelect( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RightButtonSelect( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ButtonActivate( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ForwardScan( 
            /* [in] */ double dwSpeed) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE BackwardScan( 
            /* [in] */ double dwSpeed) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PrevPGSearch( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE TopPGSearch( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE NextPGSearch( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE TitlePlay( 
            /* [in] */ ULONG uiTitle) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ChapterPlay( 
            /* [in] */ ULONG uiTitle,
            /* [in] */ ULONG uiChapter) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ChapterSearch( 
            /* [in] */ ULONG Chapter) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE MenuCall( 
            /* [in] */ DVDMenuIDConstants MenuID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ResumeFromMenu( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE TimePlay( 
            /* [in] */ ULONG uiTitle,
            /* [in] */ BSTR bstrTime) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE TimeSearch( 
            /* [in] */ BSTR bstrTime) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ChapterPlayAutoStop( 
            /* [in] */ ULONG ulTitle,
            /* [in] */ ULONG ulChapter,
            /* [in] */ ULONG ulChaptersToPlay) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE StillOff( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GoUp( void) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_TotalTitleTime( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrTime) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetNumberOfChapters( 
            /* [in] */ ULONG ulTitle,
            /* [retval][out] */ ULONG __RPC_FAR *ulNumChapters) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAudioLanguage( 
            /* [in] */ ULONG ulStream,
            /* [retval][out] */ BSTR __RPC_FAR *bstrAudioLang) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetSubpictureLanguage( 
            /* [in] */ ULONG ulStream,
            /* [retval][out] */ BSTR __RPC_FAR *bstrSubpictureLang) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAllGPRMs( 
            /* [retval][out] */ VARIANT __RPC_FAR *vtGPRM) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAllSPRMs( 
            /* [retval][out] */ VARIANT __RPC_FAR *vtSPRM) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UOPValid( 
            /* [in] */ ULONG ulUOP,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bValid) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ButtonsAvailable( 
            /* [retval][out] */ ULONG __RPC_FAR *ulButtonsAvailable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentButton( 
            /* [retval][out] */ ULONG __RPC_FAR *ulCurrentButton) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AudioStreamsAvailable( 
            /* [retval][out] */ ULONG __RPC_FAR *ulAudioStreamsAvailable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentAudioStream( 
            /* [retval][out] */ ULONG __RPC_FAR *ulAudioStream) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CurrentAudioStream( 
            /* [in] */ ULONG ulAudioStream) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentSubpictureStream( 
            /* [retval][out] */ ULONG __RPC_FAR *ulSubpictureStream) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CurrentSubpictureStream( 
            /* [in] */ ULONG ulSubpictureStream) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SubpictureStreamsAvailable( 
            /* [retval][out] */ ULONG __RPC_FAR *ulNumSubpictureStreams) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SubpictureOn( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bSubpictureON) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SubpictureOn( 
            /* [in] */ VARIANT_BOOL bSubpictureON) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AnglesAvailable( 
            /* [retval][out] */ ULONG __RPC_FAR *ulAnglesAvailable) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentAngle( 
            /* [retval][out] */ ULONG __RPC_FAR *ulAngle) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CurrentAngle( 
            /* [in] */ ULONG ulAngle) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentTitle( 
            /* [retval][out] */ ULONG __RPC_FAR *ulTitle) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentChapter( 
            /* [retval][out] */ ULONG __RPC_FAR *ulChapter) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentTime( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrTime) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Root( 
            /* [in] */ BSTR bstrPath) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Root( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPath) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FramesPerSecond( 
            /* [retval][out] */ ULONG __RPC_FAR *ulFps) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentDomain( 
            /* [retval][out] */ ULONG __RPC_FAR *ulDomain) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_TitlesAvailable( 
            /* [retval][out] */ ULONG __RPC_FAR *ulTitles) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_VolumesAvailable( 
            /* [retval][out] */ ULONG __RPC_FAR *pulVolumes) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentVolume( 
            /* [retval][out] */ ULONG __RPC_FAR *pulVolume) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentDiscSide( 
            /* [retval][out] */ ULONG __RPC_FAR *pulDiscSide) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CCActive( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bCCActive) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CCActive( 
            /* [in] */ VARIANT_BOOL bCCActive) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentCCService( 
            /* [retval][out] */ ULONG __RPC_FAR *pulService) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CurrentCCService( 
            /* [in] */ ULONG ulService) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_UniqueID( 
            /* [retval][out] */ BSTR __RPC_FAR *pvtUniqueID) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ColorKey( 
            /* [retval][out] */ ULONG __RPC_FAR *pClr) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ColorKey( 
            /* [in] */ ULONG Clr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaPlayerDvdVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ButtonSelectAndActivate )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG uiButton);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpperButtonSelect )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LowerButtonSelect )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LeftButtonSelect )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RightButtonSelect )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ButtonActivate )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ForwardScan )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ double dwSpeed);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BackwardScan )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ double dwSpeed);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrevPGSearch )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TopPGSearch )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NextPGSearch )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TitlePlay )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG uiTitle);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChapterPlay )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG uiTitle,
            /* [in] */ ULONG uiChapter);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChapterSearch )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG Chapter);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MenuCall )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ DVDMenuIDConstants MenuID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResumeFromMenu )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TimePlay )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG uiTitle,
            /* [in] */ BSTR bstrTime);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TimeSearch )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ BSTR bstrTime);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChapterPlayAutoStop )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG ulTitle,
            /* [in] */ ULONG ulChapter,
            /* [in] */ ULONG ulChaptersToPlay);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StillOff )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GoUp )( 
            IMediaPlayerDvd __RPC_FAR * This);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TotalTitleTime )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrTime);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNumberOfChapters )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG ulTitle,
            /* [retval][out] */ ULONG __RPC_FAR *ulNumChapters);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAudioLanguage )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG ulStream,
            /* [retval][out] */ BSTR __RPC_FAR *bstrAudioLang);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubpictureLanguage )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG ulStream,
            /* [retval][out] */ BSTR __RPC_FAR *bstrSubpictureLang);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAllGPRMs )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *vtGPRM);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAllSPRMs )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *vtSPRM);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UOPValid )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG ulUOP,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bValid);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ButtonsAvailable )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulButtonsAvailable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentButton )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulCurrentButton);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AudioStreamsAvailable )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulAudioStreamsAvailable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentAudioStream )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulAudioStream);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentAudioStream )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG ulAudioStream);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentSubpictureStream )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulSubpictureStream);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentSubpictureStream )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG ulSubpictureStream);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubpictureStreamsAvailable )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulNumSubpictureStreams);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubpictureOn )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bSubpictureON);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SubpictureOn )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bSubpictureON);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AnglesAvailable )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulAnglesAvailable);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentAngle )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulAngle);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentAngle )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG ulAngle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentTitle )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulTitle);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentChapter )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulChapter);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentTime )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrTime);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Root )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ BSTR bstrPath);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Root )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPath);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FramesPerSecond )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulFps);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentDomain )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulDomain);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TitlesAvailable )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *ulTitles);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VolumesAvailable )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pulVolumes);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentVolume )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pulVolume);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentDiscSide )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pulDiscSide);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CCActive )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bCCActive);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CCActive )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bCCActive);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentCCService )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pulService);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentCCService )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG ulService);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UniqueID )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pvtUniqueID);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ColorKey )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pClr);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ColorKey )( 
            IMediaPlayerDvd __RPC_FAR * This,
            /* [in] */ ULONG Clr);
        
        END_INTERFACE
    } IMediaPlayerDvdVtbl;

    interface IMediaPlayerDvd
    {
        CONST_VTBL struct IMediaPlayerDvdVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaPlayerDvd_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaPlayerDvd_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaPlayerDvd_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaPlayerDvd_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaPlayerDvd_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaPlayerDvd_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaPlayerDvd_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaPlayerDvd_ButtonSelectAndActivate(This,uiButton)	\
    (This)->lpVtbl -> ButtonSelectAndActivate(This,uiButton)

#define IMediaPlayerDvd_UpperButtonSelect(This)	\
    (This)->lpVtbl -> UpperButtonSelect(This)

#define IMediaPlayerDvd_LowerButtonSelect(This)	\
    (This)->lpVtbl -> LowerButtonSelect(This)

#define IMediaPlayerDvd_LeftButtonSelect(This)	\
    (This)->lpVtbl -> LeftButtonSelect(This)

#define IMediaPlayerDvd_RightButtonSelect(This)	\
    (This)->lpVtbl -> RightButtonSelect(This)

#define IMediaPlayerDvd_ButtonActivate(This)	\
    (This)->lpVtbl -> ButtonActivate(This)

#define IMediaPlayerDvd_ForwardScan(This,dwSpeed)	\
    (This)->lpVtbl -> ForwardScan(This,dwSpeed)

#define IMediaPlayerDvd_BackwardScan(This,dwSpeed)	\
    (This)->lpVtbl -> BackwardScan(This,dwSpeed)

#define IMediaPlayerDvd_PrevPGSearch(This)	\
    (This)->lpVtbl -> PrevPGSearch(This)

#define IMediaPlayerDvd_TopPGSearch(This)	\
    (This)->lpVtbl -> TopPGSearch(This)

#define IMediaPlayerDvd_NextPGSearch(This)	\
    (This)->lpVtbl -> NextPGSearch(This)

#define IMediaPlayerDvd_TitlePlay(This,uiTitle)	\
    (This)->lpVtbl -> TitlePlay(This,uiTitle)

#define IMediaPlayerDvd_ChapterPlay(This,uiTitle,uiChapter)	\
    (This)->lpVtbl -> ChapterPlay(This,uiTitle,uiChapter)

#define IMediaPlayerDvd_ChapterSearch(This,Chapter)	\
    (This)->lpVtbl -> ChapterSearch(This,Chapter)

#define IMediaPlayerDvd_MenuCall(This,MenuID)	\
    (This)->lpVtbl -> MenuCall(This,MenuID)

#define IMediaPlayerDvd_ResumeFromMenu(This)	\
    (This)->lpVtbl -> ResumeFromMenu(This)

#define IMediaPlayerDvd_TimePlay(This,uiTitle,bstrTime)	\
    (This)->lpVtbl -> TimePlay(This,uiTitle,bstrTime)

#define IMediaPlayerDvd_TimeSearch(This,bstrTime)	\
    (This)->lpVtbl -> TimeSearch(This,bstrTime)

#define IMediaPlayerDvd_ChapterPlayAutoStop(This,ulTitle,ulChapter,ulChaptersToPlay)	\
    (This)->lpVtbl -> ChapterPlayAutoStop(This,ulTitle,ulChapter,ulChaptersToPlay)

#define IMediaPlayerDvd_StillOff(This)	\
    (This)->lpVtbl -> StillOff(This)

#define IMediaPlayerDvd_GoUp(This)	\
    (This)->lpVtbl -> GoUp(This)

#define IMediaPlayerDvd_get_TotalTitleTime(This,bstrTime)	\
    (This)->lpVtbl -> get_TotalTitleTime(This,bstrTime)

#define IMediaPlayerDvd_GetNumberOfChapters(This,ulTitle,ulNumChapters)	\
    (This)->lpVtbl -> GetNumberOfChapters(This,ulTitle,ulNumChapters)

#define IMediaPlayerDvd_GetAudioLanguage(This,ulStream,bstrAudioLang)	\
    (This)->lpVtbl -> GetAudioLanguage(This,ulStream,bstrAudioLang)

#define IMediaPlayerDvd_GetSubpictureLanguage(This,ulStream,bstrSubpictureLang)	\
    (This)->lpVtbl -> GetSubpictureLanguage(This,ulStream,bstrSubpictureLang)

#define IMediaPlayerDvd_GetAllGPRMs(This,vtGPRM)	\
    (This)->lpVtbl -> GetAllGPRMs(This,vtGPRM)

#define IMediaPlayerDvd_GetAllSPRMs(This,vtSPRM)	\
    (This)->lpVtbl -> GetAllSPRMs(This,vtSPRM)

#define IMediaPlayerDvd_UOPValid(This,ulUOP,bValid)	\
    (This)->lpVtbl -> UOPValid(This,ulUOP,bValid)

#define IMediaPlayerDvd_get_ButtonsAvailable(This,ulButtonsAvailable)	\
    (This)->lpVtbl -> get_ButtonsAvailable(This,ulButtonsAvailable)

#define IMediaPlayerDvd_get_CurrentButton(This,ulCurrentButton)	\
    (This)->lpVtbl -> get_CurrentButton(This,ulCurrentButton)

#define IMediaPlayerDvd_get_AudioStreamsAvailable(This,ulAudioStreamsAvailable)	\
    (This)->lpVtbl -> get_AudioStreamsAvailable(This,ulAudioStreamsAvailable)

#define IMediaPlayerDvd_get_CurrentAudioStream(This,ulAudioStream)	\
    (This)->lpVtbl -> get_CurrentAudioStream(This,ulAudioStream)

#define IMediaPlayerDvd_put_CurrentAudioStream(This,ulAudioStream)	\
    (This)->lpVtbl -> put_CurrentAudioStream(This,ulAudioStream)

#define IMediaPlayerDvd_get_CurrentSubpictureStream(This,ulSubpictureStream)	\
    (This)->lpVtbl -> get_CurrentSubpictureStream(This,ulSubpictureStream)

#define IMediaPlayerDvd_put_CurrentSubpictureStream(This,ulSubpictureStream)	\
    (This)->lpVtbl -> put_CurrentSubpictureStream(This,ulSubpictureStream)

#define IMediaPlayerDvd_get_SubpictureStreamsAvailable(This,ulNumSubpictureStreams)	\
    (This)->lpVtbl -> get_SubpictureStreamsAvailable(This,ulNumSubpictureStreams)

#define IMediaPlayerDvd_get_SubpictureOn(This,bSubpictureON)	\
    (This)->lpVtbl -> get_SubpictureOn(This,bSubpictureON)

#define IMediaPlayerDvd_put_SubpictureOn(This,bSubpictureON)	\
    (This)->lpVtbl -> put_SubpictureOn(This,bSubpictureON)

#define IMediaPlayerDvd_get_AnglesAvailable(This,ulAnglesAvailable)	\
    (This)->lpVtbl -> get_AnglesAvailable(This,ulAnglesAvailable)

#define IMediaPlayerDvd_get_CurrentAngle(This,ulAngle)	\
    (This)->lpVtbl -> get_CurrentAngle(This,ulAngle)

#define IMediaPlayerDvd_put_CurrentAngle(This,ulAngle)	\
    (This)->lpVtbl -> put_CurrentAngle(This,ulAngle)

#define IMediaPlayerDvd_get_CurrentTitle(This,ulTitle)	\
    (This)->lpVtbl -> get_CurrentTitle(This,ulTitle)

#define IMediaPlayerDvd_get_CurrentChapter(This,ulChapter)	\
    (This)->lpVtbl -> get_CurrentChapter(This,ulChapter)

#define IMediaPlayerDvd_get_CurrentTime(This,bstrTime)	\
    (This)->lpVtbl -> get_CurrentTime(This,bstrTime)

#define IMediaPlayerDvd_put_Root(This,bstrPath)	\
    (This)->lpVtbl -> put_Root(This,bstrPath)

#define IMediaPlayerDvd_get_Root(This,pbstrPath)	\
    (This)->lpVtbl -> get_Root(This,pbstrPath)

#define IMediaPlayerDvd_get_FramesPerSecond(This,ulFps)	\
    (This)->lpVtbl -> get_FramesPerSecond(This,ulFps)

#define IMediaPlayerDvd_get_CurrentDomain(This,ulDomain)	\
    (This)->lpVtbl -> get_CurrentDomain(This,ulDomain)

#define IMediaPlayerDvd_get_TitlesAvailable(This,ulTitles)	\
    (This)->lpVtbl -> get_TitlesAvailable(This,ulTitles)

#define IMediaPlayerDvd_get_VolumesAvailable(This,pulVolumes)	\
    (This)->lpVtbl -> get_VolumesAvailable(This,pulVolumes)

#define IMediaPlayerDvd_get_CurrentVolume(This,pulVolume)	\
    (This)->lpVtbl -> get_CurrentVolume(This,pulVolume)

#define IMediaPlayerDvd_get_CurrentDiscSide(This,pulDiscSide)	\
    (This)->lpVtbl -> get_CurrentDiscSide(This,pulDiscSide)

#define IMediaPlayerDvd_get_CCActive(This,bCCActive)	\
    (This)->lpVtbl -> get_CCActive(This,bCCActive)

#define IMediaPlayerDvd_put_CCActive(This,bCCActive)	\
    (This)->lpVtbl -> put_CCActive(This,bCCActive)

#define IMediaPlayerDvd_get_CurrentCCService(This,pulService)	\
    (This)->lpVtbl -> get_CurrentCCService(This,pulService)

#define IMediaPlayerDvd_put_CurrentCCService(This,ulService)	\
    (This)->lpVtbl -> put_CurrentCCService(This,ulService)

#define IMediaPlayerDvd_get_UniqueID(This,pvtUniqueID)	\
    (This)->lpVtbl -> get_UniqueID(This,pvtUniqueID)

#define IMediaPlayerDvd_get_ColorKey(This,pClr)	\
    (This)->lpVtbl -> get_ColorKey(This,pClr)

#define IMediaPlayerDvd_put_ColorKey(This,Clr)	\
    (This)->lpVtbl -> put_ColorKey(This,Clr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_ButtonSelectAndActivate_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG uiButton);


void __RPC_STUB IMediaPlayerDvd_ButtonSelectAndActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_UpperButtonSelect_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_UpperButtonSelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_LowerButtonSelect_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_LowerButtonSelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_LeftButtonSelect_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_LeftButtonSelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_RightButtonSelect_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_RightButtonSelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_ButtonActivate_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_ButtonActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_ForwardScan_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ double dwSpeed);


void __RPC_STUB IMediaPlayerDvd_ForwardScan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_BackwardScan_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ double dwSpeed);


void __RPC_STUB IMediaPlayerDvd_BackwardScan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_PrevPGSearch_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_PrevPGSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_TopPGSearch_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_TopPGSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_NextPGSearch_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_NextPGSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_TitlePlay_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG uiTitle);


void __RPC_STUB IMediaPlayerDvd_TitlePlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_ChapterPlay_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG uiTitle,
    /* [in] */ ULONG uiChapter);


void __RPC_STUB IMediaPlayerDvd_ChapterPlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_ChapterSearch_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG Chapter);


void __RPC_STUB IMediaPlayerDvd_ChapterSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_MenuCall_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ DVDMenuIDConstants MenuID);


void __RPC_STUB IMediaPlayerDvd_MenuCall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_ResumeFromMenu_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_ResumeFromMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_TimePlay_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG uiTitle,
    /* [in] */ BSTR bstrTime);


void __RPC_STUB IMediaPlayerDvd_TimePlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_TimeSearch_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ BSTR bstrTime);


void __RPC_STUB IMediaPlayerDvd_TimeSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_ChapterPlayAutoStop_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG ulTitle,
    /* [in] */ ULONG ulChapter,
    /* [in] */ ULONG ulChaptersToPlay);


void __RPC_STUB IMediaPlayerDvd_ChapterPlayAutoStop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_StillOff_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_StillOff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_GoUp_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This);


void __RPC_STUB IMediaPlayerDvd_GoUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_TotalTitleTime_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrTime);


void __RPC_STUB IMediaPlayerDvd_get_TotalTitleTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_GetNumberOfChapters_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG ulTitle,
    /* [retval][out] */ ULONG __RPC_FAR *ulNumChapters);


void __RPC_STUB IMediaPlayerDvd_GetNumberOfChapters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_GetAudioLanguage_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG ulStream,
    /* [retval][out] */ BSTR __RPC_FAR *bstrAudioLang);


void __RPC_STUB IMediaPlayerDvd_GetAudioLanguage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_GetSubpictureLanguage_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG ulStream,
    /* [retval][out] */ BSTR __RPC_FAR *bstrSubpictureLang);


void __RPC_STUB IMediaPlayerDvd_GetSubpictureLanguage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_GetAllGPRMs_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *vtGPRM);


void __RPC_STUB IMediaPlayerDvd_GetAllGPRMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_GetAllSPRMs_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *vtSPRM);


void __RPC_STUB IMediaPlayerDvd_GetAllSPRMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_UOPValid_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG ulUOP,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bValid);


void __RPC_STUB IMediaPlayerDvd_UOPValid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_ButtonsAvailable_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulButtonsAvailable);


void __RPC_STUB IMediaPlayerDvd_get_ButtonsAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentButton_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulCurrentButton);


void __RPC_STUB IMediaPlayerDvd_get_CurrentButton_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_AudioStreamsAvailable_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulAudioStreamsAvailable);


void __RPC_STUB IMediaPlayerDvd_get_AudioStreamsAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentAudioStream_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulAudioStream);


void __RPC_STUB IMediaPlayerDvd_get_CurrentAudioStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_put_CurrentAudioStream_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG ulAudioStream);


void __RPC_STUB IMediaPlayerDvd_put_CurrentAudioStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentSubpictureStream_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulSubpictureStream);


void __RPC_STUB IMediaPlayerDvd_get_CurrentSubpictureStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_put_CurrentSubpictureStream_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG ulSubpictureStream);


void __RPC_STUB IMediaPlayerDvd_put_CurrentSubpictureStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_SubpictureStreamsAvailable_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulNumSubpictureStreams);


void __RPC_STUB IMediaPlayerDvd_get_SubpictureStreamsAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_SubpictureOn_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bSubpictureON);


void __RPC_STUB IMediaPlayerDvd_get_SubpictureOn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_put_SubpictureOn_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bSubpictureON);


void __RPC_STUB IMediaPlayerDvd_put_SubpictureOn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_AnglesAvailable_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulAnglesAvailable);


void __RPC_STUB IMediaPlayerDvd_get_AnglesAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentAngle_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulAngle);


void __RPC_STUB IMediaPlayerDvd_get_CurrentAngle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_put_CurrentAngle_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG ulAngle);


void __RPC_STUB IMediaPlayerDvd_put_CurrentAngle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentTitle_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulTitle);


void __RPC_STUB IMediaPlayerDvd_get_CurrentTitle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentChapter_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulChapter);


void __RPC_STUB IMediaPlayerDvd_get_CurrentChapter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentTime_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrTime);


void __RPC_STUB IMediaPlayerDvd_get_CurrentTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_put_Root_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ BSTR bstrPath);


void __RPC_STUB IMediaPlayerDvd_put_Root_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_Root_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPath);


void __RPC_STUB IMediaPlayerDvd_get_Root_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_FramesPerSecond_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulFps);


void __RPC_STUB IMediaPlayerDvd_get_FramesPerSecond_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentDomain_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulDomain);


void __RPC_STUB IMediaPlayerDvd_get_CurrentDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_TitlesAvailable_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *ulTitles);


void __RPC_STUB IMediaPlayerDvd_get_TitlesAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_VolumesAvailable_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pulVolumes);


void __RPC_STUB IMediaPlayerDvd_get_VolumesAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentVolume_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pulVolume);


void __RPC_STUB IMediaPlayerDvd_get_CurrentVolume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentDiscSide_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pulDiscSide);


void __RPC_STUB IMediaPlayerDvd_get_CurrentDiscSide_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CCActive_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bCCActive);


void __RPC_STUB IMediaPlayerDvd_get_CCActive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_put_CCActive_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bCCActive);


void __RPC_STUB IMediaPlayerDvd_put_CCActive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_CurrentCCService_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pulService);


void __RPC_STUB IMediaPlayerDvd_get_CurrentCCService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_put_CurrentCCService_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG ulService);


void __RPC_STUB IMediaPlayerDvd_put_CurrentCCService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_UniqueID_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pvtUniqueID);


void __RPC_STUB IMediaPlayerDvd_get_UniqueID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_get_ColorKey_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pClr);


void __RPC_STUB IMediaPlayerDvd_get_ColorKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IMediaPlayerDvd_put_ColorKey_Proxy( 
    IMediaPlayerDvd __RPC_FAR * This,
    /* [in] */ ULONG Clr);


void __RPC_STUB IMediaPlayerDvd_put_ColorKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaPlayerDvd_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


