#import "npapi/npapi.h"
#import "npapi/npfunctions.h"
#import "npapi/npruntime.h"

class npapiBase
{
public:
  npapiBase(NPNetscapeFuncs* pBrowserFuncs, NPObject* pNPObj);
  ~npapiBase();
  
  virtual NPError   nppNew            (NPMIMEType pluginType, NPP pInstance, uint16_t nMode, int16_t nArgc, char* szArgn[], char* szArgv[], NPSavedData* pSaved);
  virtual NPError   nppDestroy        (NPP pInstance, NPSavedData** ppSaved);
  virtual NPError   nppSetWindow      (NPP pInstance, NPWindow* pWindow);
  virtual NPError   nppNewStream      (NPP pInstance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype);
  virtual NPError   nppDestroyStream  (NPP pInstance, NPStream* pStream, NPReason reason);
  virtual int32_t   nppWriteReady     (NPP pInstance, NPStream* pStream);
  virtual int32_t   nppWrite          (NPP pInstance, NPStream* pStream, int32_t nOffset, int32_t nLen, void* pBuffer);
  virtual void      nppStreamAsFile   (NPP pInstance, NPStream* pStream, const char* pFilename);
  virtual void      nppPrint          (NPP pInstance, NPPrint* pPlatformPrint);
  virtual int16_t   nppHandleEvent    (NPP pInstance, void* pEvent);
  virtual void      nppURLNotify      (NPP pInstance, const char* pUrl, NPReason reason, void* pNotifyData);
  virtual NPError   nppGetValue       (NPP pInstance, NPPVariable variable, void *pValue);
  virtual NPError   nppSetValue       (NPP pInstance, NPNVariable variable, void *pValue);
  
  virtual NPObject* npAllocate        (NPP pInstance, NPClass* pNPClass);
  virtual void      npDeallocate      (NPObject *pNPObj);
  virtual void      npInvalidate      (NPObject *pNPObj);
  virtual bool      npHasMethod       (NPObject *pNPObj, NPIdentifier name);
  virtual bool      npInvoke          (NPObject *pNPObj, NPIdentifier name, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult);
  virtual bool      npInvokeDefault   (NPObject *pNPObj, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult);
  virtual bool      npHasProperty     (NPObject *pNPObj, NPIdentifier name);
  virtual bool      npGetProperty     (NPObject *pNPObj, NPIdentifier name, NPVariant *pResult);
  virtual bool      npSetProperty     (NPObject *pNPObj, NPIdentifier name, const NPVariant *pValue);
  virtual bool      npRemoveProperty  (NPObject *pNPObj, NPIdentifier name);
  virtual bool      npEnumerate       (NPObject *pNPObj, NPIdentifier **ppName, uint32_t *pCount);
  virtual bool      npConstruct       (NPObject *pNPObj, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult);

protected:
  NPNetscapeFuncs*  m_pBrowserFuncs;
  NPObject*         m_pNPObj;
};
