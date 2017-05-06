#include <windows.h>
#include <winhttp.h>
#include <tchar.h>
#include <stdio.h>
#include "voWinHttpsCertVerify.h"

#pragma comment(lib, "Winhttp.lib")


struct callback_param_t
{
	HINTERNET hInet;
	DWORD dwErrCert;
};

static VOID CALLBACK SyncCallback(HINTERNET, DWORD_PTR, DWORD, PVOID, DWORD);

static DWORD internalHttpsCertVerify(LPCWSTR pswzServerName, LPCWSTR pswzObjectName, LPDWORD lpdwErrCert)
{
	DWORD dwErr = ERROR_SUCCESS;
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;

	if(NULL == lpdwErrCert)
		*lpdwErrCert = 0;

	hSession = ::WinHttpOpen(0,WINHTTP_ACCESS_TYPE_NO_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0);

	if (NULL == hSession)
		return ::GetLastError();

	hConnect = ::WinHttpConnect(hSession, pswzServerName, INTERNET_DEFAULT_HTTPS_PORT, 0);
	if (NULL == hConnect) {
		::WinHttpCloseHandle(hSession);	
		return ::GetLastError();
	}

	// Use WINHTTP_FLAG_SECURE flag to verify CRL
	hRequest = ::WinHttpOpenRequest(hConnect, NULL, pswzObjectName, 0, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if(NULL == hRequest)
	{
		::WinHttpCloseHandle(hConnect);
		::WinHttpCloseHandle(hSession);	
		return ::GetLastError();
	}

	DWORD dwOpt = WINHTTP_ENABLE_SSL_REVOCATION;
	const BOOL bSetOptionResults = ::WinHttpSetOption(hRequest, WINHTTP_OPTION_ENABLE_FEATURE, &dwOpt, sizeof(dwOpt));
	if(!bSetOptionResults)
	{
		::WinHttpCloseHandle(hConnect);
		::WinHttpCloseHandle(hSession);	
		return ::GetLastError();
	}

	callback_param_t param;
	param.hInet = hRequest;
	param.dwErrCert = 0;
	const WINHTTP_STATUS_CALLBACK isCallback= ::WinHttpSetStatusCallback(hRequest,SyncCallback,WINHTTP_CALLBACK_FLAG_SECURE_FAILURE,0);
	if (WINHTTP_INVALID_STATUS_CALLBACK == isCallback)
	{
		::WinHttpCloseHandle(hConnect);
		::WinHttpCloseHandle(hSession);	
		return ::GetLastError();
	}

	const BOOL bSendResults = ::WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA, 0, 0, reinterpret_cast<DWORD_PTR>(&param));
	if(!bSendResults)
	{
		dwErr = ::GetLastError();
		// Value is set to lpdwErrCert, if an error occurred in CRL check.
		if(lpdwErrCert)
			*lpdwErrCert = param.dwErrCert;
	}
	else
	{
		// Place additional code here.
		// For instance, receive response
	}
	

	::WinHttpCloseHandle(hConnect);
	::WinHttpCloseHandle(hSession);

	return dwErr;
}

static VOID CALLBACK SyncCallback(HINTERNET inet, DWORD_PTR context, DWORD status, PVOID information, DWORD informationLength)
{

	callback_param_t &p = *reinterpret_cast<callback_param_t*>(context);
	const DWORD flag = reinterpret_cast<DWORD>(information);
	if ((0 != context) && (inet == p.hInet) && (WINHTTP_CALLBACK_STATUS_SECURE_FAILURE == status) && (sizeof(DWORD) == informationLength))
	{
		p.dwErrCert = flag;
	}
}

int voWinHttpsCertVerify(void* pswzURI2Open)
{
	if (pswzURI2Open == NULL)
		return -1;

	TCHAR pswzUrl[1024];
	memset(pswzUrl, 0, sizeof(pswzUrl));
	int len = _tcslen((LPCTSTR)pswzURI2Open);
	if (len > 1024)
		len = 1024;

	_tcsncpy(pswzUrl, (LPCTSTR)pswzURI2Open, len);
	_tcsupr(pswzUrl);
	LPCTSTR https = _tcsstr(pswzUrl, L"HTTPS://");
	if (https == NULL)
		return VOCERT_ERROR_NOT_HTTPS;

	TCHAR pswzTargetDomain[256];
	memset(pswzTargetDomain, 0, sizeof(pswzTargetDomain));
	len = _tcslen(L"https://");
	https = _tcschr((LPCTSTR)pswzURI2Open + len, L'/');
	_tcsncpy(pswzTargetDomain, (LPCTSTR)pswzURI2Open + len, _tcschr((LPCTSTR)pswzURI2Open + len, L'/') - (LPCTSTR)pswzURI2Open - len);
	DWORD dwErrCert = 0;
	DWORD dwErr = internalHttpsCertVerify(pswzTargetDomain, L"/", &dwErrCert);
	if (ERROR_SUCCESS == dwErr || 0 == dwErrCert)
		return 0;

	//if (dwErrCert & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REV_FAILED)
	//{
	//	puts("Certification revocation checking has been enabled, "
	//		"but the revocation check failed to verify whether "
	//		"a certificate has been revoked. The server used "
	//		"to check for revocation might be unreachable.");
	//}

	//if(dwErrCert & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CERT)
	//{
	//	puts("SSL certificate is invalid.");
	//}

	//if(dwErrCert & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REVOKED)
	//{
	//	puts("SSL certificate was revoked.");
	//}

	//if(dwErrCert & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CA)
	//{
	//	puts("The function is unfamiliar with the Certificate "
	//		"Authority that generated the server's certificate.");
	//}

	//if(dwErrCert & WINHTTP_CALLBACK_STATUS_FLAG_CERT_CN_INVALID)
	//{
	//	puts("SSL certificate common name (host name field) "
	//		"is incorrect, for example, if you entered "
	//		"www.microsoft.com and the common name on the "
	//		"certificate says www.msn.com.");
	//}

	//if (dwErrCert & WINHTTP_CALLBACK_STATUS_FLAG_CERT_DATE_INVALID)
	//{
	//	puts("SSL certificate date that was received from the "
	//		"server is bad. The certificate is expired.");
	//}

	//if (dwErrCert & WINHTTP_CALLBACK_STATUS_FLAG_SECURITY_CHANNEL_ERROR)
	//{
	//	puts("The application experienced an internal error "
	//		"loading the SSL libraries.");

	//}

	//if (dwErrCert & WINHTTP_CALLBACK_STATUS_FLAG_CERT_WRONG_USAGE)
	//{
	//	puts("WINHTTP_CALLBACK_STATUS_FLAG_CERT_WRONG_USAGE");
	//}

	return dwErr;
}