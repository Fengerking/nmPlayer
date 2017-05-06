/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010		*
*																		*
************************************************************************/
/*******************************************************************************
File:		npp_Interface.cpp

Contains:	this is  browser call plugin interface

Written by:	Yuqing Wu

Change History (most recent first):
2010-12-09		YuqingWu			Create file

*******************************************************************************/
#include "afxwin.h"
#include "vonpplugin.h"
#include "voLog.h"

char* NPP_GetMIMEDescription(void)
{
  return "application/x-visualon-osmp:VisualOn Browser Plugin MIMETYPE is application/x-visualon-osmp";
}

NPError NPP_Initialize(void)
{
  return NPERR_NO_ERROR;
}

void NPP_Shutdown(void)
{
}

// here the plugin creates an instance of our voNPPlugin object which 
// will be associated with this newly created plugin instance and 
// will do all the neccessary job
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved)
{   
	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	VOPluginParam pp;
	pp.bWindowless = false;
	memset(&pp, 0, sizeof(VOPluginParam));
	for (int i = 0; i < argc; i++) {
		if (stricmp(argn[i], "windowless") == 0) {
			if (stricmp(argv[i], "true") == 0)
				pp.bWindowless = true;

		} else if (stricmp(argn[i], "wmode") == 0) {
			if (stricmp(argv[i], "transparent") == 0)
				pp.bWindowless = true;
		}else if (stricmp(argn[i], "type") == 0) {
			strncpy(pp.szmime, argv[i], 32);
		} else if (stricmp(argn[i], "width") == 0) {
			pp.width = atoi(argv[i]);
		} else if (stricmp(argn[i], "height") == 0) {
			pp.height = atoi(argv[i]);
		} else if (stricmp(argn[i], "style") == 0) {
			char szStyle[1024];
			memset(szStyle, 0, 1024);
			strcpy(szStyle, argv[i]);
			bool bxfound = false, byfound = false;
			char* psz = strtok(szStyle, " :;");
			while(psz) {
				if (stricmp(psz, "top") == 0)
					bxfound = true;

				if (stricmp(psz, "left") == 0)
					byfound = true;

				if (bxfound || byfound) {
					char szval[128];
					memset(szval, 0, 128);
					strcpy(szval, psz);
					char* pos = strstr(szval, "px");
					if (pos) {
						szval[pos - szval] = 0;

						if (bxfound) {
							bxfound = false;
							pp.x = atoi(szval);
						}

						if (byfound) {
							byfound = false;
							pp.y = atoi(szval);
						}
					}
				}
				psz = strtok(NULL, " :;");
			} // end of while
		} // end of else
	} // end of for

	voNPPlugin* pplgn = new voNPPlugin(instance);

	if (pplgn == NULL)
		return NPERR_OUT_OF_MEMORY_ERROR;

	VOLOGI("npp: %p, mode: %d, savedData: %p, npplgn: %p, windowless: %d, x: %d, y: %d, w: %d, h: %d", instance, mode, saved, pplgn, pp.bWindowless, pp.x, pp.y, pp.width, pp.height);
	pplgn->init(&pp);

	instance->pdata = static_cast<void *>(pplgn);

	if (saved) {

		VOLOGI("jscallback: %p", saved->buf);
	}

	return NPERR_NO_ERROR;
}

// here is the place to clean up and destroy the voNPPlugin object
NPError NPP_Destroy (NPP instance, NPSavedData** saved)
{
	VOLOGI("Gonna destroy whole plugin %p...", instance);
	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	NPError rv = NPERR_NO_ERROR;

	voNPPlugin * pp = (voNPPlugin *)instance->pdata;

	if (pp == NULL) 
		return rv;

	//NPSavedData* sd = (NPSavedData*)NPN_MemAlloc(sizeof(NPSavedData));
	//if (sd) {
	//	sd->buf = pp->getJSCallbackOjbect();
	//	sd->len = 4;
	//	*saved = sd;
	//}

	delete pp;
	pp = NULL;
	instance->pdata = NULL;

	return rv;
}

// during this call we know when the plugin window is ready or
// is about to be destroyed so we can do some gui specific
// initialization and shutdown
NPError NPP_SetWindow (NPP instance, NPWindow* pNPWindow)
{
  if (instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;

  if (pNPWindow == NULL)
    return NPERR_GENERIC_ERROR;

  voNPPlugin * pPlugin = (voNPPlugin *)instance->pdata;

  if (pPlugin == NULL) 
    return NPERR_GENERIC_ERROR;


  // window goes away
  if (pNPWindow->window == NULL)
    return NPERR_NO_ERROR;

  // window just created
  if (!pPlugin->setWindow(pNPWindow)) {
	  delete pPlugin;
	  pPlugin = NULL;
	  return NPERR_MODULE_LOAD_FAILED_ERROR;
  }

  return rv;
}


NPError	NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	NPError rv = NPERR_NO_ERROR;

	if (instance == NULL)
		return NPERR_GENERIC_ERROR;

	voNPPlugin * plugin = (voNPPlugin *)instance->pdata;
	if (plugin == NULL)
		return NPERR_GENERIC_ERROR;

	switch (variable) {
	  case NPPVpluginNameString:
		  *((char **)value) = "Visualon Browser Plugin";
		  break;
	  case NPPVpluginDescriptionString:
		  *((char **)value) = "Visualon Browser Plugin";
		  break;
	  case NPPVpluginScriptableNPObject:
		  *(NPObject **)value = plugin->getScriptableObject();
		  break;
	  default:
		  rv = NPERR_GENERIC_ERROR;
  }

  return rv;
}

NPError NPP_NewStream(NPP instance,
                      NPMIMEType type,
                      NPStream* stream, 
                      NPBool seekable,
                      uint16_t* stype)
{
  if (instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;
  return rv;
}

int32_t NPP_WriteReady (NPP instance, NPStream *stream)
{
  if (instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  int32_t rv = 0x0fffffff;
  return rv;
}

int32_t NPP_Write (NPP instance, NPStream *stream, int32_t offset, int32_t len, void *buffer)
{   
  if (instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  int32_t rv = len;
  return rv;
}

NPError NPP_DestroyStream (NPP instance, NPStream *stream, NPError reason)
{
  if (instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;
  return rv;
}

void NPP_StreamAsFile (NPP instance, NPStream* stream, const char* fname)
{}

void NPP_Print (NPP instance, NPPrint* printInfo)
{}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
  if (instance == NULL)
    return;

  VOLOGI("Got a URL notification!");
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
  if (instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;
  return rv;
}

int16_t	NPP_HandleEvent(NPP instance, void* event)
{
  if (instance == NULL)
    return 0;

  int16_t rv = 0;
  voNPPlugin * pPlugin = (voNPPlugin *)instance->pdata;
  if (pPlugin)
    rv = pPlugin->handleEvent(event);

  return rv;
}

void  NPP_DidComposite(NPP instance) {

	if (instance == NULL)
		return;

	voNPPlugin* pp = (voNPPlugin *)instance->pdata;
	if (pp) {
		int n = 0;
		//pp->drawFrame();
	}
}
