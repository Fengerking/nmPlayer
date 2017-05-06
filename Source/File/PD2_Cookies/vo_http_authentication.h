#pragma once

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define HASHLEN 16
typedef unsigned char HASH[HASHLEN];
#define HASHHEXLEN 32
typedef unsigned char HASHHEX[HASHHEXLEN+1];
#define IN
#define OUT

enum AUTHENTICATION_TYPE
{
	BASIC_AUTH,
	DIGEST_AUTH,
	UNKNOW_AUTH,
};

class vo_http_authentication
{
public:
	vo_http_authentication();
	virtual ~vo_http_authentication(void);

	void process_challenge_info(VO_CHAR * ptr_challenge);
	VO_BOOL generate_authorizationinfo();
	VO_CHAR* get_authorizationinfo(){ return m_authorizationinfo; }

	VO_VOID set_username( VO_CHAR* strname);
	VO_VOID set_password( VO_CHAR* strpasswd);

private:
	VO_VOID Process_Property( VO_CHAR* ptr_info , const VO_CHAR* propname );
	VO_VOID Init();

	VO_CHAR Encode_GetChar(VO_BYTE num);

	VO_S32 Base64_Encode(VO_CHAR *pDest, VO_S32 encode_space , const VO_CHAR *pSrc, VO_S32 srclen );

	VO_BOOL generate_basic_authorizationinfo();

	VO_BOOL generate_digest_authorizationinfo();

	VO_BOOL Is_basic_auth(){ return (m_auth_type == BASIC_AUTH) ? VO_TRUE : VO_FALSE; }

	VO_BOOL Is_digest_auth(){ return (m_auth_type == DIGEST_AUTH) ? VO_TRUE : VO_FALSE; }

private:
	void CvtHex(
		IN HASH Bin,
		OUT HASHHEX Hex
		);

	/* calculate H(A1) as per HTTP Digest spec */
	void DigestCalcHA1(
		IN char * pszAlg,
		IN char * pszUserName,
		IN char * pszRealm,
		IN char * pszPassword,
		IN char * pszNonce,
		IN char * pszCNonce,
		OUT HASHHEX SessionKey
		);

	/* calculate request-digest/response-digest as per HTTP Digest spec */
	void DigestCalcResponse(
		IN HASHHEX HA1,           /* H(A1) */
		IN char * pszNonce,       /* nonce from server */
		IN char * pszNonceCount,  /* 8 hex digits */
		IN char * pszCNonce,      /* client nonce */
		IN char * pszQop,         /* qop-value: "", "auth", "auth-int" */
		IN char * pszMethod,      /* method from the request */
		IN char * pszDigestUri,   /* requested URL */
		IN HASHHEX HEntity,       /* H(entity body) if qop="auth-int" */
		OUT HASHHEX Response      /* request-digest or response-digest */
		);

private:
	VO_CHAR m_challengeinfo[2048];
	VO_CHAR m_authorizationinfo[2048];

	VO_CHAR m_pszNonce[50];
	VO_CHAR m_pszNonceCount[10];
	VO_CHAR m_pszCNonce[10];
	VO_CHAR m_pszUser[50];
	VO_CHAR m_pszRealm[50];
	VO_CHAR m_pszPass[100];
	VO_CHAR m_pszMethod[10];
	VO_CHAR m_pszQop[20];
	VO_CHAR m_pszURI[1024];
	VO_CHAR m_pszOpaque[50];
	VO_CHAR m_pszStale[10];
	VO_CHAR m_pszAlg[10];

	AUTHENTICATION_TYPE m_auth_type;
};

#ifdef _VONAMESPACE
}
#endif



