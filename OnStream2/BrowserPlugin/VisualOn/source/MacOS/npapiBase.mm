#import "npapiBase.h"

npapiBase::npapiBase(NPNetscapeFuncs* pBrowserFuncs, NPObject* pNPObj)
{
  m_pBrowserFuncs = pBrowserFuncs;
  m_pNPObj = pNPObj;
}

npapiBase::~npapiBase()
{
}

NPError npapiBase::nppNew(NPMIMEType pluginType, NPP pInstance, uint16_t nMode, int16_t nArgc, char* szArgn[], char* szArgv[], NPSavedData* pSaved)
{
  return NPERR_NO_ERROR;
}

NPError npapiBase::nppDestroy(NPP pInstance, NPSavedData** ppSaved)
{
  return NPERR_NO_ERROR;
}

NPError npapiBase::nppSetWindow(NPP pInstance, NPWindow* pWindow)
{
  return NPERR_NO_ERROR;
}

NPError npapiBase::nppNewStream(NPP pInstance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
  return NPERR_NO_ERROR;
}

NPError npapiBase::nppDestroyStream(NPP pInstance, NPStream* pStream, NPReason reason)
{
  return NPERR_NO_ERROR;
}

int32_t npapiBase::nppWriteReady(NPP pInstance, NPStream* pStream)
{
  return 0;
}

int32_t npapiBase::nppWrite(NPP pInstance, NPStream* pStream, int32_t nOffset, int32_t nLen, void* pBuffer)
{
  return 0;
}

void npapiBase::nppStreamAsFile(NPP pInstance, NPStream* pStream, const char* pFilename)
{
}

void npapiBase::nppPrint(NPP pInstance, NPPrint* pPlatformPrint)
{
}

int16_t npapiBase::nppHandleEvent(NPP pInstance, void* pEvent)
{
  return 0;
}

void npapiBase::nppURLNotify(NPP pInstance, const char* pUrl, NPReason reason, void* pNotifyData)
{  
}

NPError npapiBase::nppGetValue(NPP pInstance, NPPVariable variable, void *pValue)
{
  return NPERR_NO_ERROR;
}

NPError npapiBase::nppSetValue(NPP pInstance, NPNVariable variable, void *pValue)
{
  return NPERR_NO_ERROR;
}



NPObject* npapiBase::npAllocate(NPP pInst, NPClass* npclass)
{
  return 0;
}

void npapiBase::npDeallocate(NPObject *pNPObj)
{
  
}

void npapiBase::npInvalidate(NPObject *pNPObj)
{
  
}

bool npapiBase::npHasMethod(NPObject *pNPObj, NPIdentifier name)
{
  return false;
}

bool npapiBase::npInvoke(NPObject *pNPObj, NPIdentifier name, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult)
{
  return false;
}

bool npapiBase::npInvokeDefault(NPObject *pNPObj, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult)
{
  return false;
}

bool npapiBase::npHasProperty(NPObject * pNPObj, NPIdentifier name)
{
  return false;
}

bool npapiBase::npGetProperty(NPObject *pNPObj, NPIdentifier name, NPVariant *pResult)
{
  return false;
}

bool npapiBase::npSetProperty(NPObject *pNPObj, NPIdentifier name, const NPVariant *pValue)
{
  return false;
}

bool npapiBase::npRemoveProperty(NPObject *pNPObj, NPIdentifier name)
{
  return false;
}

bool npapiBase::npEnumerate(NPObject *pNPObj, NPIdentifier **ppName, uint32_t *pCount)
{
  return false;
}

bool npapiBase::npConstruct(NPObject *pNPObj, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult)
{
  return false;
}

