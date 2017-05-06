/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __VODLFCN_H__
#define __VODLFCN_H__

#if defined LINUX || defined _IOS
#if defined EXPORT
#undef EXPORT
#endif
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Open the shared object FILE and map it in; return a handle that can be passed to 'voGetAddressSymbol' to get symbol values from it.  
 * szlibname: the library name to be loaded
 * mode: [Linux like] RTLD_LAZY, RTLD_NOW, RTLD_GLOBAL, RTLD_LOCAL
 *		 [Windows]  0
 * return value: a handle which the caller may use on subsequent calls to voGetAddressSymbol() and voFreeLibrary().
 * */
EXPORT void *voLoadLibrary(const char *szlibname, int mode);


/* Open the shared object FILE and map it in; return a handle that can be passed to 'voGetAddressSymbol' to get symbol values from it.  
 * szlibname: the library name to be loaded
 *
 * NOTE: it just loads the input file name without correcting it
 *
 * mode: [Linux like] RTLD_LAZY, RTLD_NOW, RTLD_GLOBAL, RTLD_LOCAL
 *		 [Windows]  0
 * return value: a handle which the caller may use on subsequent calls to voGetAddressSymbol() and voFreeLibrary().
 * */
EXPORT void *voLoadLibraryNoSuffix(const char *szlibname, int mode);


/* Unmap and close a shared object opened by `voLoadLibrary'.The handle cannot be used again after calling `voFreeLibrary'.  
 * handle: returned from voLoadLibrary, referring to the libraries mapped in
 * return value: 0, closed successfully; non-zero, close failure
 * */
EXPORT int voFreeLibrary(void *handle);



/* Find the run-time address in the shared object HANDLE refers to the symbol called NAME.  
 * handle: returned from voLoadLibrary, referring to the libraries mapped in
 * szsymbol: the symbol's name as a character string
 * return value: the address of a symbol defined within handle
 * */

EXPORT void *voGetAddressSymbol(void *handle,	const char *szsymbol); 


/* Get the last error that occurred during dynamic linking processing.  
 * return value: a null-terminated character string that describes the last error that occurred during dynamic linking processing. 
 * If no dynamic linking errors have occurred since the last invocation of voErrorLibrary(), it shall return NULL.
 * */
EXPORT char *voErrorLibrary();


/**
 * Set the param for library operator.
 * param uParamID [IN] The param ID.
 * param pData [IN] The param value depend on the ID>
 * retval 0 Succeeded.
 *
 * */
EXPORT int voSetLibraryParam(const unsigned int uParamID, void* pData);

/**
 * *the lib operator
 * */
typedef struct
{
	void *  pUserData;
	void *  (* LoadLib)(void * pUserData, char* pLibName, int nFlag);
	void *  (* GetAddress)(void * pUserData, void * hLib, char * pFuncName);
	int     (* FreeLib)(void * pUserData, void * hLib);
} VO_LIB_FUNC;

#define VO_PID_LIB_FUNC          0X00000001  /*!< Set Lib function set. VO_LIB_FUNC* It will be supported in next build */
#define VO_PID_LIB_SUFFIX        0X00000002  /*!< Set the suffix for all the libraries handled by VisualOn lib operator */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VODLFCN_H__


