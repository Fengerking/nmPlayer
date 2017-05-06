#include "voPDPort.h"
#include "VOUtility.h"
#include "commonheader.h"
//#include "ipnetwork.h"
#include "netstrength.h"
#include "netdownbase.h"
#include "CThroughputer.h"

DUMPLOGFUNC		g_pfDumpLogFunc = NULL;
CThroughputer*	g_Throughputer;

VONETDOWNRETURNCODE VONDAPI voNetDownInit()
{
	//return InitWinsock() ? VORC_NETDOWN_OK : VORC_NETDOWN_NETWORK_ERROR;
	g_Throughputer=new(MEM_CHECK)CThroughputer;
	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE VONDAPI voNetDownCleanup()
{
	//CleanupWinsock();
	delete g_Throughputer;
	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE VONDAPI voNetDownOpen(HVONETDOWN* ppNetDown, const char* szUrl, const char* szProxyName, VONETDOWNTYPE type)
{
	CNetStrength * pNetStrength = new(MEM_CHECK) CNetStrength;
	if(pNetStrength == NULL)
		return VORC_NETDOWN_OUTOF_MEMORY;
	VONETDOWNRETURNCODE rc = pNetStrength->Open((LPCTSTR)szUrl, (LPCTSTR)szProxyName, type);
	if(rc != VORC_NETDOWN_OK)
	{
		pNetStrength->Close();
		SAFE_DELETE(pNetStrength);
		return rc;
	}

	*ppNetDown = pNetStrength;
	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE VONDAPI voNetDownStart(HVONETDOWN pNetDown, bool* pbCancel)
{
	CNetStrength * pNetStrength = (CNetStrength *)pNetDown;
	if(pNetStrength == NULL)
		return VORC_NETDOWN_NULL_POINTER;
	
	return pNetStrength->Start(pbCancel);
}

VONETDOWNRETURNCODE VONDAPI voNetDownStop(HVONETDOWN pNetDown)
{
	CNetStrength * pNetStrength = (CNetStrength *)pNetDown;
	if(pNetStrength == NULL)
		return VORC_NETDOWN_NULL_POINTER;

	return pNetStrength->Stop();
}

VONETDOWNRETURNCODE VONDAPI voNetDownClose(HVONETDOWN pNetDown)
{
	CNetStrength * pNetStrength = (CNetStrength *)pNetDown;
	if(pNetStrength == NULL)
		return VORC_NETDOWN_NULL_POINTER;

	VONETDOWNRETURNCODE rc = pNetStrength->Close();
	SAFE_DELETE(pNetStrength);

	return rc;
}

VONETDOWNRETURNCODE VONDAPI voNetDownSetParameter(HVONETDOWN pNetDown, LONG lID, LONG lValue)
{
	if(VOID_NETDOWN_DUMP_LOG_FUNC == lID)
	{
		g_pfDumpLogFunc = (DUMPLOGFUNC)lValue;
		return VORC_NETDOWN_OK;
	}

	CNetStrength * pNetStrength = (CNetStrength *)pNetDown;
	if(pNetStrength == NULL)
		return VORC_NETDOWN_NULL_POINTER;

	return pNetStrength->SetParam(lID, lValue);
}

VONETDOWNRETURNCODE VONDAPI voNetDownGetParameter(HVONETDOWN pNetDown, LONG lID, LONG* plValue)
{
	if(VOID_NETDOWN_BYTES_PER_SEC == lID)
	{
		DWORD dwThroughput = 0;
		if(g_Throughputer->GetThroughput(dwThroughput))
		{
			*((DWORD*)plValue) = dwThroughput;
			return VORC_NETDOWN_OK;
		}

		return VORC_NETDOWN_UNKNOWN_ERROR;
	}

	CNetStrength * pNetStrength = (CNetStrength *)pNetDown;
	if(pNetStrength == NULL)
		return VORC_NETDOWN_NULL_POINTER;

	return pNetStrength->GetParam(lID, plValue);
}
