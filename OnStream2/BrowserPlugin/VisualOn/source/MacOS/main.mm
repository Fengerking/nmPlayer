#import "npapi/npapi.h"
#import "npapi/npfunctions.h"
#import "npapi/npruntime.h"
#import <sys/types.h>
#import <sys/sysctl.h>
#import "npapiBase.h"
#import "PlayerInst.h"

@interface NPPInstance : NSObject
{
  @public
  npapiBase*  m_pPlugin;
  NPP         m_instance;
  NPObject*   m_pNPObj;
}
@end

@implementation NPPInstance
@end

NPNetscapeFuncs* g_pBrowserFuncs = 0;
NSMutableArray* g_pInstArray = 0;

NPPInstance* FindInstance(NPP pInstance)
{
  for (id obj in g_pInstArray)
  {
    if (((NPPInstance*)obj)->m_instance == pInstance)
    {
      return obj;
    }
  }
  return nil;
}

NPPInstance* FindInstance(NPObject* pObj)
{
  for (id obj in g_pInstArray)
  {
    if (((NPPInstance*)obj)->m_pNPObj == pObj)
    {
      return obj;
    }
  }
  return nil;
}

NPObject* NP_Allocate(NPP pInst, NPClass* pNPClass)
{
  NSLog(@"[voBrowserPlugin] NP_Allocate");
  return ((npapiBase*)(((NPPInstance*)pInst)->m_pPlugin))->npAllocate(pInst, pNPClass);
}

void NP_Deallocate(NPObject *pNPObj)
{
  NSLog(@"[voBrowserPlugin] NP_Deallocate");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npDeallocate(pNPObj);
}

void NP_Invalidate(NPObject *pNPObj)
{
  NSLog(@"[voBrowserPlugin] NP_Invalidate");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npInvalidate(pNPObj);
}

bool NP_HasMethod(NPObject *pNPObj, NPIdentifier name)
{
  //NSLog(@"[voBrowserPlugin] NP_HasMethod");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npHasMethod(pNPObj, name);
}

bool NP_Invoke(NPObject *pNPObj, NPIdentifier name, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult)
{
  //NSLog(@"[voBrowserPlugin] NP_Invoke");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npInvoke(pNPObj, name, pArgs, nArgCount, pResult);
}

bool NP_InvokeDefault(NPObject *pNPObj, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult)
{
  NSLog(@"[voBrowserPlugin] NP_InvokeDefault");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npInvokeDefault(pNPObj, pArgs, nArgCount, pResult);
}

bool NP_HasProperty(NPObject * pNPObj, NPIdentifier name)
{	
//  NSLog(@"[voBrowserPlugin] NP_HasProperty");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npHasProperty(pNPObj, name);
}

bool NP_GetProperty(NPObject *pNPObj, NPIdentifier name, NPVariant *pResult)
{
  NSLog(@"[voBrowserPlugin] NP_GetProperty");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npGetProperty(pNPObj, name, pResult);
}

bool NP_SetProperty(NPObject *pNPObj, NPIdentifier name, const NPVariant *pValue)
{
  NSLog(@"[voBrowserPlugin] NP_SetProperty");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npSetProperty(pNPObj, name, pValue);
}

bool NP_RemoveProperty(NPObject *pNPObj, NPIdentifier name)
{
  NSLog(@"[voBrowserPlugin] NP_RemoveProperty");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npRemoveProperty(pNPObj, name);
}

bool NP_Enumerate(NPObject *pNPObj, NPIdentifier **ppName, uint32_t *pCount)
{
  NSLog(@"[voBrowserPlugin] NP_Enumerate");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npEnumerate(pNPObj, ppName, pCount);
}

bool NP_Construct(NPObject *pNPObj, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult)
{
  NSLog(@"[voBrowserPlugin] NP_Construct");
  NPPInstance* pInst = FindInstance(pNPObj);
  return ((npapiBase*)(pInst->m_pPlugin))->npConstruct(pNPObj, pArgs, nArgCount, pResult);
}

static NPClass g_NPClass = {
  NP_CLASS_STRUCT_VERSION,
  0,
  0,
  0,
  NP_HasMethod,
  NP_Invoke,
  0,
  NP_HasProperty,
  NP_GetProperty,
  0,
  0,
  0,
  0,
};

NPError NPP_New(NPMIMEType pluginType, NPP pInstance, uint16_t nMode, int16_t nArgc, char* szArgn[], char* szArgv[], NPSavedData* pSaved)
{
  NSLog(@"[voBrowserPlugin] NPP_New");
  NPPInstance* pInst = [[NPPInstance alloc] init];
  pInst->m_pNPObj = g_pBrowserFuncs->createobject(pInstance, &g_NPClass);
  pInst->m_pPlugin = new PlayerInst(g_pBrowserFuncs, pInst->m_pNPObj);
  pInst->m_instance = pInstance;

  [g_pInstArray addObject: pInst];
  [pInst release];
  
  return pInst->m_pPlugin->nppNew(pluginType, pInstance, nMode, nArgc, szArgn, szArgv, pSaved);
}

NPError NPP_Destroy(NPP pInstance, NPSavedData** ppSaved)
{
  NSLog(@"[voBrowserPlugin] NPP_Destroy");
  NPPInstance* pInst = FindInstance(pInstance);
  NPError err = ((npapiBase*)(pInst->m_pPlugin))->nppDestroy(pInstance, ppSaved);
  [g_pInstArray removeObject: pInst];
  return err;
}

NPError NPP_SetWindow(NPP pInstance, NPWindow* pWindow)
{
//  NSLog(@"[voBrowserPlugin] NPP_SetWindow");
  NPPInstance* pInst = FindInstance(pInstance);
  return ((npapiBase*)(pInst->m_pPlugin))->nppSetWindow(pInstance, pWindow);
}

NPError NPP_NewStream(NPP pInstance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
  NSLog(@"[voBrowserPlugin] NPP_NewStream");
  NPPInstance* pInst = FindInstance(pInstance);
  return ((npapiBase*)(pInst->m_pPlugin))->nppNewStream(pInstance, type, stream, seekable, stype);
}

NPError NPP_DestroyStream(NPP pInstance, NPStream* pStream, NPReason reason)
{
  NSLog(@"[voBrowserPlugin] NPP_DestroyStream");
  NPPInstance* pInst = FindInstance(pInstance);
  return ((npapiBase*)(pInst->m_pPlugin))->nppDestroyStream(pInstance, pStream, reason);
}

int32_t NPP_WriteReady(NPP pInstance, NPStream* pStream)
{
  NSLog(@"[voBrowserPlugin] NPP_WriteReady");
  NPPInstance* pInst = FindInstance(pInstance);
  return ((npapiBase*)(pInst->m_pPlugin))->nppWriteReady(pInstance, pStream);
}

int32_t NPP_Write(NPP pInstance, NPStream* pStream, int32_t nOffset, int32_t nLen, void* pBuffer)
{
  NSLog(@"[voBrowserPlugin] NPP_Write");
  NPPInstance* pInst = FindInstance(pInstance);
  return ((npapiBase*)(pInst->m_pPlugin))->nppWrite(pInstance, pStream, nOffset, nLen, pBuffer);
}

void NPP_StreamAsFile(NPP pInstance, NPStream* pStream, const char* pFilename)
{
  NSLog(@"[voBrowserPlugin] NPP_StreamAsFile");
  NPPInstance* pInst = FindInstance(pInstance);
  ((npapiBase*)(pInst->m_pPlugin))->nppStreamAsFile(pInstance, pStream, pFilename);
}

void NPP_Print(NPP pInstance, NPPrint* pPlatformPrint)
{
  NSLog(@"[voBrowserPlugin] NPP_Print");
  NPPInstance* pInst = FindInstance(pInstance);
  ((npapiBase*)(pInst->m_pPlugin))->nppPrint(pInstance, pPlatformPrint);
}

int16_t NPP_HandleEvent(NPP pInstance, void* pEvent)
{
  //NSLog(@"[voBrowserPlugin] NPP_HandleEvent");
  NPPInstance* pInst = FindInstance(pInstance);
  return ((npapiBase*)(pInst->m_pPlugin))->nppHandleEvent(pInstance, pEvent);
}

void NPP_URLNotify(NPP pInstance, const char* pUrl, NPReason reason, void* pNotifyData)
{
  NSLog(@"[voBrowserPlugin] NPP_URLNotify");
  NPPInstance* pInst = FindInstance(pInstance);
  ((npapiBase*)(pInst->m_pPlugin))->nppURLNotify(pInstance, pUrl, reason, pNotifyData);
}

NPError NPP_GetValue(NPP pInstance, NPPVariable variable, void *pValue)
{
  NSLog(@"[voBrowserPlugin] NPP_SetWindow");
  NPPInstance* pInst = FindInstance(pInstance);
  return ((npapiBase*)(pInst->m_pPlugin))->nppGetValue(pInstance, variable, pValue);
}

NPError NPP_SetValue(NPP pInstance, NPNVariable variable, void *pValue)
{
  NSLog(@"[voBrowserPlugin] NPP_SetWindow");
  NPPInstance* pInst = FindInstance(pInstance);
  return ((npapiBase*)(pInst->m_pPlugin))->nppSetValue(pInstance, variable, pValue);
}

static bool beingDebugged() {
  int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid()}; size_t mib_size = 4;
  struct kinfo_proc kp; size_t kp_size = sizeof(kp);
  int result = sysctl(mib, mib_size, &kp, &kp_size, NULL, 0);
  return (0 == result) ? (P_TRACED & kp.kp_proc.p_flag) : false;
}

extern "C" NPError NP_Initialize(NPNetscapeFuncs* pBrowserFuncs)
{
//  while (!beingDebugged())
//    sleep(1);
  NSLog(@"[voBrowserPlugin] NP_Initialize");
  g_pBrowserFuncs = pBrowserFuncs;
  g_pInstArray = [[NSMutableArray alloc] init];
  return NPERR_NO_ERROR;
}

extern "C" NPError NP_GetEntryPoints(NPPluginFuncs* pPluginFuncs)
{
  NSLog(@"[voBrowserPlugin] NP_GetEntryPoints");
  
  if (pPluginFuncs->size < (offsetof(NPPluginFuncs, setvalue) + sizeof(void*)))
    return NPERR_INVALID_FUNCTABLE_ERROR;
  
  pPluginFuncs->version       = 11;
  pPluginFuncs->size          = sizeof(NPPluginFuncs);
  pPluginFuncs->newp          = NPP_New;
  pPluginFuncs->destroy       = NPP_Destroy;
  pPluginFuncs->setwindow     = NPP_SetWindow;
  pPluginFuncs->newstream     = NPP_NewStream;
  pPluginFuncs->destroystream = NPP_DestroyStream;
  pPluginFuncs->asfile        = NPP_StreamAsFile;
  pPluginFuncs->writeready    = NPP_WriteReady;
  pPluginFuncs->write         = NPP_Write;
  pPluginFuncs->print         = NPP_Print;
  pPluginFuncs->event         = NPP_HandleEvent;
  pPluginFuncs->urlnotify     = NPP_URLNotify;
  pPluginFuncs->getvalue      = NPP_GetValue;
  pPluginFuncs->setvalue      = NPP_SetValue;
  
  return NPERR_NO_ERROR;
}

extern "C" void NP_Shutdown(void)
{
  NSLog(@"[voBrowserPlugin] NP_Shutdown");
  
  for (id obj in g_pInstArray)
  {
    if (((NPPInstance*)obj)->m_pPlugin)
    {
      delete ((NPPInstance*)obj)->m_pPlugin;
    }
  }
  [g_pInstArray release];
}

