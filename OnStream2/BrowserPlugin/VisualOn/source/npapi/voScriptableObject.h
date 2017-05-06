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


#if !defined __voScriptableObject_H__
#define __voScriptableObject_H__

#include "npfunctions.h"
#include "COSMPEngnWrap.h"
#include "vonpplugin.h"


#define BIG_BUF_LEN 2048

class voScriptableObject : NPObject /*: public ScriptableObjectBase*/
{
public:
	voScriptableObject(NPP npp);
	~voScriptableObject();

	void Invalidate() {};
	bool HasMethod(NPIdentifier name);
	bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool HasProperty(NPIdentifier name);
	bool GetProperty(NPIdentifier name, NPVariant *result);
	bool SetProperty(NPIdentifier name, const NPVariant *value) { return false; };
	bool RemoveProperty(NPIdentifier name) { return false; };
	bool Enumerate(NPIdentifier **identifier, uint32_t *count) { return false; };
	bool Construct(const NPVariant *args, uint32_t argc, NPVariant *result) { return false; } 

public:
	static NPObject* _Allocate(NPP instance, NPClass* npclass);
	static void _Deallocate(NPObject *npobj);
	static void _Invalidate(NPObject *npobj);
	static bool _HasMethod(NPObject *npobj, NPIdentifier name);
	static bool _Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
	static bool _InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
	static bool _HasProperty(NPObject * npobj, NPIdentifier name);
	static bool _GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result);
	static bool _SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value);
	static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
	static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count);
	static bool _Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);

private: 
	char* getStrVal(NPVariant&);
	int   getIntVal(NPVariant&);
	bool  getBoolVal(NPVariant&);
	int   getVersion(LPTSTR lpszVersion, const int nlen);
private:
	NPP					m_pnpp;
	voNPPlugin*			m_pplgn;
	COSMPEngnWrap*		m_pOsmpEngn;
	int					m_nOpenFlag;
  int         m_OpenType;
	char				m_szval[BIG_BUF_LEN];
	TCHAR				m_szURL[BIG_BUF_LEN];
};

#endif