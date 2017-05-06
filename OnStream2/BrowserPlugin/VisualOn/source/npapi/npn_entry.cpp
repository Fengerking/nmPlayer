/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010		*
*																		*
************************************************************************/
/*******************************************************************************
File:		npp_Interface.cpp

Contains:	this is  plugin call browser interface

Written by:	Yuqing Wu

Change History (most recent first):
2010-12-09		YuqingWu			Create file

*******************************************************************************/

#include "npapi.h"
#include "npfunctions.h"

#ifndef HIBYTE
#define HIBYTE(x) ((((uint32_t)(x)) & 0xff00) >> 8)
#endif

#ifndef LOBYTE
#define LOBYTE(W) ((W) & 0xFF)
#endif

extern NPNetscapeFuncs NPNFuncs;

void NPN_Version(int* plugin_major, int* plugin_minor, int* netscape_major, int* netscape_minor)
{
  if(plugin_major)
  {
    *plugin_major   = NP_VERSION_MAJOR;
  }

  if(plugin_minor)
  {
    *plugin_minor   = NP_VERSION_MINOR;
  }

  if(netscape_major)
  {
    *netscape_major = HIBYTE(NPNFuncs.version);
  }

  if(netscape_minor)
  {
    *netscape_minor = LOBYTE(NPNFuncs.version);
  }
}

NPError NPN_GetURLNotify(NPP instance, const char *url, const char *target, void* notifyData)
{
  if (NULL == NPNFuncs.geturlnotify)
  {
    return NPERR_GENERIC_ERROR;
  }

	int navMinorVers = NPNFuncs.version & 0xFF;
  NPError rv = NPERR_NO_ERROR;

  if ( navMinorVers >= NPVERS_HAS_NOTIFICATION )
		rv = NPNFuncs.geturlnotify(instance, url, target, notifyData);
	else
		rv = NPERR_INCOMPATIBLE_VERSION_ERROR;

  return rv;
}

NPError NPN_GetURL(NPP instance, const char *url, const char *target)
{
  if (NULL == NPNFuncs.geturl)
  {
    return NPERR_GENERIC_ERROR;
  }

  NPError rv = NPNFuncs.geturl(instance, url, target);
  return rv;
}

NPError NPN_PostURLNotify(NPP instance, const char* url, const char* window, uint32_t len, const char* buf, NPBool file, void* notifyData)
{
  if (NULL == NPNFuncs.posturlnotify)
  {
    return NPERR_GENERIC_ERROR;
  }

	int navMinorVers = NPNFuncs.version & 0xFF;
    NPError rv = NPERR_NO_ERROR;

	if ( navMinorVers >= NPVERS_HAS_NOTIFICATION )
		rv = NPNFuncs.posturlnotify(instance, url, window, len, buf, file, notifyData);
	else
		rv = NPERR_INCOMPATIBLE_VERSION_ERROR;

  return rv;
}

NPError NPN_PostURL(NPP instance, const char* url, const char* window, uint32_t len, const char* buf, NPBool file)
{
  if (NULL == NPNFuncs.posturl)
  {
    return NPERR_GENERIC_ERROR;
  }

  NPError rv = NPNFuncs.posturl(instance, url, window, len, buf, file);
  return rv;
} 

NPError NPN_RequestRead(NPStream* stream, NPByteRange* rangeList)
{
  if (NULL == NPNFuncs.requestread)
  {
    return NPERR_GENERIC_ERROR;
  }

  NPError rv = NPNFuncs.requestread(stream, rangeList);
  return rv;
}

NPError NPN_NewStream(NPP instance, NPMIMEType type, const char* target, NPStream** stream)
{
  if (NULL == NPNFuncs.newstream)
  {
    return NPERR_GENERIC_ERROR;
  }

	int navMinorVersion = NPNFuncs.version & 0xFF;

  NPError rv = NPERR_NO_ERROR;

	if ( navMinorVersion >= NPVERS_HAS_STREAMOUTPUT )
		rv = NPNFuncs.newstream(instance, type, target, stream);
	else
		rv = NPERR_INCOMPATIBLE_VERSION_ERROR;

  return rv;
}

int32_t NPN_Write(NPP instance, NPStream *stream, int32_t len, void *buffer)
{
  if (NULL == NPNFuncs.write)
  {
    return -1;
  }

	int navMinorVersion = NPNFuncs.version & 0xFF;
  int32_t rv = 0;

  if ( navMinorVersion >= NPVERS_HAS_STREAMOUTPUT )
		rv = NPNFuncs.write(instance, stream, len, buffer);
	else
		rv = -1;

  return rv;
}

NPError NPN_DestroyStream(NPP instance, NPStream* stream, NPError reason)
{
  if (NULL == NPNFuncs.destroystream)
  {
    return NPERR_GENERIC_ERROR;
  }

	int navMinorVersion = NPNFuncs.version & 0xFF;
  NPError rv = NPERR_NO_ERROR;

  if ( navMinorVersion >= NPVERS_HAS_STREAMOUTPUT )
		rv = NPNFuncs.destroystream(instance, stream, reason);
	else
		rv = NPERR_INCOMPATIBLE_VERSION_ERROR;

  return rv;
}

void NPN_Status(NPP instance, const char *message)
{
  if (NULL == NPNFuncs.status)
  {
    return;
  }

  NPNFuncs.status(instance, message);
}

const char* NPN_UserAgent(NPP instance)
{
  if (NULL == NPNFuncs.uagent)
  {
    return NULL;
  }

  const char * rv = NULL;
  rv = NPNFuncs.uagent(instance);
  return rv;
}

void* NPN_MemAlloc(uint32_t size)
{
  if (NULL == NPNFuncs.memalloc)
  {
    return NULL;
  }

  void * rv = NULL;
  rv = NPNFuncs.memalloc(size);
  return rv;
}

void NPN_MemFree(void* ptr)
{
  if (NULL == NPNFuncs.memfree)
  {
    return;
  }

  NPNFuncs.memfree(ptr);
}

uint32_t NPN_MemFlush(uint32_t size)
{
  if (NULL == NPNFuncs.memflush)
  {
    return -1;
  }

  uint32_t rv = NPNFuncs.memflush(size);
  return rv;
}

void NPN_ReloadPlugins(NPBool reloadPages)
{
  if (NULL == NPNFuncs.reloadplugins)
  {
    return;
  }

  NPNFuncs.reloadplugins(reloadPages);
}

NPError NPN_GetValue(NPP instance, NPNVariable variable, void *value)
{
  if (NULL == NPNFuncs.getvalue)
  {
    return NPERR_GENERIC_ERROR;
  }

  NPError rv = NPNFuncs.getvalue(instance, variable, value);
  return rv;
}

NPError NPN_SetValue(NPP instance, NPPVariable variable, void *value)
{
  if (NULL == NPNFuncs.setvalue)
  {
    return NPERR_GENERIC_ERROR;
  }

  NPError rv = NPNFuncs.setvalue(instance, variable, value);
  return rv;
}

void NPN_InvalidateRect(NPP instance, NPRect *invalidRect)
{
  if (NULL == NPNFuncs.invalidaterect)
  {
    return;
  }

  NPNFuncs.invalidaterect(instance, invalidRect);
}

void NPN_InvalidateRegion(NPP instance, NPRegion invalidRegion)
{
  if (NULL == NPNFuncs.invalidateregion)
  {
    return;
  }

  NPNFuncs.invalidateregion(instance, invalidRegion);
}

void NPN_ForceRedraw(NPP instance)
{
  if (NULL == NPNFuncs.forceredraw)
  {
    return;
  }

  NPNFuncs.forceredraw(instance);
}

NPIdentifier NPN_GetStringIdentifier(const NPUTF8 *name)
{
  if (NULL == NPNFuncs.getstringidentifier)
  {
    return NULL;
  }

  return NPNFuncs.getstringidentifier(name);
}

void NPN_GetStringIdentifiers(const NPUTF8 **names, uint32_t nameCount,
                              NPIdentifier *identifiers)
{
  if (NULL == NPNFuncs.getstringidentifiers)
  {
    return;
  }
  return NPNFuncs.getstringidentifiers(names, nameCount, identifiers);
}

NPIdentifier NPN_GetStringIdentifier(int32_t intid)
{
  if (NULL == NPNFuncs.getintidentifier)
  {
    return NULL;
  }

  return NPNFuncs.getintidentifier(intid);
}

bool NPN_IdentifierIsString(NPIdentifier identifier)
{
  if (NULL == NPNFuncs.identifierisstring)
  {
    return false;
  }

  return NPNFuncs.identifierisstring(identifier);
}

NPUTF8 *NPN_UTF8FromIdentifier(NPIdentifier identifier)
{
  if (NULL == NPNFuncs.utf8fromidentifier)
  {
    return NULL;
  }

  return NPNFuncs.utf8fromidentifier(identifier);
}

int32_t NPN_IntFromIdentifier(NPIdentifier identifier)
{
  if (NULL == NPNFuncs.intfromidentifier)
  {
    return -1;
  }

  return NPNFuncs.intfromidentifier(identifier);
}

NPObject *NPN_CreateObject(NPP npp, NPClass *aClass)
{
  if (NULL == NPNFuncs.createobject)
  {
    return NULL;
  }

  return NPNFuncs.createobject(npp, aClass);
}

NPObject *NPN_RetainObject(NPObject *obj)
{
  if (NULL == NPNFuncs.retainobject)
  {
    return NULL;
  }

  return NPNFuncs.retainobject(obj);
}

void NPN_ReleaseObject(NPObject *obj)
{
  if (NULL == NPNFuncs.releaseobject)
  {
    return;
  }

  return NPNFuncs.releaseobject(obj);
}

bool NPN_Invoke(NPP npp, NPObject* obj, NPIdentifier methodName,
                const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  if (NULL == NPNFuncs.invoke)
  {
    return false;
  }

  return NPNFuncs.invoke(npp, obj, methodName, args, argCount, result);
}

bool NPN_InvokeDefault(NPP npp, NPObject* obj, const NPVariant *args,
                       uint32_t argCount, NPVariant *result)
{
  if (NULL == NPNFuncs.invokeDefault)
  {
    return false;
  }

  return NPNFuncs.invokeDefault(npp, obj, args, argCount, result);
}

bool NPN_Evaluate(NPP npp, NPObject* obj, NPString *script,
                  NPVariant *result)
{
  if (NULL == NPNFuncs.evaluate)
  {
    return false;
  }

  return NPNFuncs.evaluate(npp, obj, script, result);
}

bool NPN_GetProperty(NPP npp, NPObject* obj, NPIdentifier propertyName,
                     NPVariant *result)
{
  if (NULL == NPNFuncs.getproperty)
  {
    return false;
  }

  return NPNFuncs.getproperty(npp, obj, propertyName, result);
}

bool NPN_SetProperty(NPP npp, NPObject* obj, NPIdentifier propertyName,
                     const NPVariant *value)
{
  if (NULL == NPNFuncs.setproperty)
  {
    return false;
  }

  return NPNFuncs.setproperty(npp, obj, propertyName, value);
}

bool NPN_RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
{
  if (NULL == NPNFuncs.removeproperty)
  {
    return false;
  }

  return NPNFuncs.removeproperty(npp, obj, propertyName);
}

bool NPN_Enumerate(NPP npp, NPObject *obj, NPIdentifier **identifier,
                   uint32_t *count)
{
  if (NULL == NPNFuncs.enumerate)
  {
    return false;
  }

  return NPNFuncs.enumerate(npp, obj, identifier, count);
}

bool NPN_Construct(NPP npp, NPObject *obj, const NPVariant *args,
                   uint32_t argCount, NPVariant *result)
{
  if (NULL == NPNFuncs.construct)
  {
    return false;
  }

  return NPNFuncs.construct(npp, obj, args, argCount, result);
}

bool NPN_HasProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
{
  if (NULL == NPNFuncs.hasproperty)
  {
    return false;
  }

  return NPNFuncs.hasproperty(npp, obj, propertyName);
}

bool NPN_HasMethod(NPP npp, NPObject* obj, NPIdentifier methodName)
{
  if (NULL == NPNFuncs.hasmethod)
  {
    return false;
  }

  return NPNFuncs.hasmethod(npp, obj, methodName);
}

void NPN_ReleaseVariantValue(NPVariant *variant)
{
  if (NULL == NPNFuncs.releasevariantvalue)
  {
    return;
  }

  NPNFuncs.releasevariantvalue(variant);
}

void NPN_SetException(NPObject* obj, const NPUTF8 *message)
{
  if (NULL == NPNFuncs.setexception)
  {
    return;
  }

  NPNFuncs.setexception(obj, message);
}

void NPN_PluginThreadAsyncCall(NPP npp, void (*func)(void *), void * puserdata)
{
	if (NPNFuncs.pluginthreadasynccall)
		NPNFuncs.pluginthreadasynccall(npp, func, puserdata);
}

//NPError NPN_InitAsyncSurface(NPP instance, NPSize *size, NPImageFormat format, void *initData, NPAsyncSurface *surface)
//{
//
//	if (NPNFuncs.initasyncsurface)
//		return NPNFuncs.initasyncsurface(instance, size, format, initData, surface);
//
//	return NPERR_INVALID_FUNCTABLE_ERROR;
//}
//
//NPError NPN_FinalizeAsyncSurface(NPP instance, NPAsyncSurface *surface)
//{
//	if (NPNFuncs.finalizeasyncsurface)
//		return NPNFuncs.finalizeasyncsurface(instance, surface);
//
//	return NPERR_INVALID_FUNCTABLE_ERROR;
//}
//
//void NPN_SetCurrentAsyncSurface(NPP instance, NPAsyncSurface *surface, NPRect *changed)
//{
//	if (NPNFuncs.setcurrentasyncsurface)
//		NPNFuncs.setcurrentasyncsurface(instance, surface, changed);
//}