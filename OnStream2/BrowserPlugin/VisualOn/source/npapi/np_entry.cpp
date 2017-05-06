/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010		*
*																		*
************************************************************************/
/*******************************************************************************
File:		np_PluginEntry.cpp

Contains:	this is  plugin entry point

Written by:	Yuqing Wu

Change History (most recent first):
2010-12-09		YuqingWu			Create file

*******************************************************************************/

#include "npapi.h"
#include "npfunctions.h"
#include "voLog.h"
#ifndef HIBYTE
#define HIBYTE(x) ((((uint32_t)(x)) & 0xff00) >> 8)
#endif

NPNetscapeFuncs NPNFuncs;

extern TCHAR g_lpszPluginPath[MAX_PATH];

#ifdef XP_WIN

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
	if (pFuncs == NULL)
		return NPERR_INVALID_FUNCTABLE_ERROR;

	if (pFuncs->size < sizeof(NPPluginFuncs))
		return NPERR_INVALID_FUNCTABLE_ERROR;

	pFuncs->version       = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
	pFuncs->newp          = NPP_New;
	pFuncs->destroy       = NPP_Destroy;
	pFuncs->setwindow     = NPP_SetWindow;
	pFuncs->newstream     = NPP_NewStream;
	pFuncs->destroystream = NPP_DestroyStream;
	pFuncs->asfile        = NPP_StreamAsFile;
	pFuncs->writeready    = NPP_WriteReady;
	pFuncs->write         = NPP_Write;
	pFuncs->print         = NPP_Print;
	pFuncs->event         = NPP_HandleEvent;
	pFuncs->urlnotify     = NPP_URLNotify;
	pFuncs->getvalue      = NPP_GetValue;
	pFuncs->setvalue      = NPP_SetValue;
	pFuncs->javaClass     = NULL;
	//pFuncs->didComposite  = NPP_DidComposite;
	return NPERR_NO_ERROR;
}

#endif /* XP_WIN */

NPError OSCALL NP_Initialize(NPNetscapeFuncs* pFuncs)
{
	if (pFuncs == NULL)
		return NPERR_INVALID_FUNCTABLE_ERROR;

	if (HIBYTE(pFuncs->version) > NP_VERSION_MAJOR)
		return NPERR_INCOMPATIBLE_VERSION_ERROR;

	if (pFuncs->size < sizeof(NPNetscapeFuncs))
		return NPERR_INVALID_FUNCTABLE_ERROR;

	NPNFuncs.size                    = pFuncs->size;
	NPNFuncs.version                 = pFuncs->version;
	NPNFuncs.geturlnotify            = pFuncs->geturlnotify;
	NPNFuncs.geturl                  = pFuncs->geturl;
	NPNFuncs.posturlnotify           = pFuncs->posturlnotify;
	NPNFuncs.posturl                 = pFuncs->posturl;
	NPNFuncs.requestread             = pFuncs->requestread;
	NPNFuncs.newstream               = pFuncs->newstream;
	NPNFuncs.write                   = pFuncs->write;
	NPNFuncs.destroystream           = pFuncs->destroystream;
	NPNFuncs.status                  = pFuncs->status;
	NPNFuncs.uagent                  = pFuncs->uagent;
	NPNFuncs.memalloc                = pFuncs->memalloc;
	NPNFuncs.memfree                 = pFuncs->memfree;
	NPNFuncs.memflush                = pFuncs->memflush;
	NPNFuncs.reloadplugins           = pFuncs->reloadplugins;
	NPNFuncs.getJavaEnv              = NULL;
	NPNFuncs.getJavaPeer             = NULL;
	NPNFuncs.getvalue                = pFuncs->getvalue;
	NPNFuncs.setvalue                = pFuncs->setvalue;
	NPNFuncs.invalidaterect          = pFuncs->invalidaterect;
	NPNFuncs.invalidateregion        = pFuncs->invalidateregion;
	NPNFuncs.forceredraw             = pFuncs->forceredraw;
	NPNFuncs.getstringidentifier     = pFuncs->getstringidentifier;
	NPNFuncs.getstringidentifiers    = pFuncs->getstringidentifiers;
	NPNFuncs.getintidentifier        = pFuncs->getintidentifier;
	NPNFuncs.identifierisstring      = pFuncs->identifierisstring;
	NPNFuncs.utf8fromidentifier      = pFuncs->utf8fromidentifier;
	NPNFuncs.intfromidentifier       = pFuncs->intfromidentifier;
	NPNFuncs.createobject            = pFuncs->createobject;
	NPNFuncs.retainobject            = pFuncs->retainobject;
	NPNFuncs.releaseobject           = pFuncs->releaseobject;
	NPNFuncs.invoke                  = pFuncs->invoke;
	NPNFuncs.invokeDefault           = pFuncs->invokeDefault;
	NPNFuncs.evaluate                = pFuncs->evaluate;
	NPNFuncs.getproperty             = pFuncs->getproperty;
	NPNFuncs.setproperty             = pFuncs->setproperty;
	NPNFuncs.removeproperty          = pFuncs->removeproperty;
	NPNFuncs.hasproperty             = pFuncs->hasproperty;
	NPNFuncs.hasmethod               = pFuncs->hasmethod;
	NPNFuncs.releasevariantvalue     = pFuncs->releasevariantvalue;
	NPNFuncs.setexception            = pFuncs->setexception;
	NPNFuncs.pluginthreadasynccall   = pFuncs->pluginthreadasynccall;

	//NPNFuncs.initasyncsurface   = pFuncs->initasyncsurface;
	//NPNFuncs.finalizeasyncsurface = pFuncs->finalizeasyncsurface;
	//NPNFuncs.setcurrentasyncsurface = pFuncs->setcurrentasyncsurface;

	VOLOGINIT(g_lpszPluginPath);
	return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown()
{
	VOLOGI("Here totally shutdown.");
	VOLOGUNINIT();
  return NPERR_NO_ERROR;
}


NPError NP_GetValue(void* future, NPPVariable variable, void *value)
{
	return NPP_GetValue((NPP_t *)future, variable, value);
}

