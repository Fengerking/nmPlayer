#include "vo_http_md5.h"
#include "voString.h"
#include "vo_http_authentication.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_http_authentication::vo_http_authentication()
:m_auth_type( UNKNOW_AUTH )
{
	Init();
}

vo_http_authentication::~vo_http_authentication(void)
{
}

VO_VOID vo_http_authentication::Init()
{
	memset( m_challengeinfo , 0 , sizeof(m_challengeinfo) );
	memset( m_authorizationinfo , 0 , sizeof(m_authorizationinfo) );
	memset( m_pszNonce , 0 , sizeof(m_pszNonce) );
	memset( m_pszNonceCount , 0 , sizeof(m_pszNonceCount) );
	memset( m_pszCNonce , 0 , sizeof(m_pszCNonce) );
	memset( m_pszUser , 0 , sizeof(m_pszUser) );
	memset( m_pszRealm , 0 , sizeof(m_pszRealm) );
	memset( m_pszPass , 0 , sizeof(m_pszPass) );
	memset( m_pszMethod , 0 , sizeof(m_pszMethod) );
	memset( m_pszQop , 0 , sizeof(m_pszQop) );
	memset( m_pszURI , 0 , sizeof(m_pszURI) );
	memset( m_pszOpaque , 0 , sizeof(m_pszOpaque) );
	memset( m_pszStale , 0 , sizeof(m_pszStale) );
	memset( m_pszAlg , 0 , sizeof(m_pszAlg) );

	//set default value
	strcpy( m_pszURI , "/" );
	strcpy( m_pszAlg , "MD5" );
	strcpy( m_pszNonceCount , "00000001" );
	strcpy( m_pszCNonce , "56734f6e" );
	strcpy( m_pszMethod , "GET" );
}

VO_VOID vo_http_authentication::set_username( VO_CHAR* strname)
{
	if( strlen(strname) > 0 )
	{
		strcpy( m_pszUser , strname );
	}
}

VO_VOID vo_http_authentication::set_password( VO_CHAR* strpasswd)
{
	if( strlen(strpasswd) > 0 )
	{
		strcpy( m_pszPass , strpasswd );
	}
}

VO_BOOL vo_http_authentication::generate_digest_authorizationinfo()
{
	memset( m_authorizationinfo , 0 , sizeof(m_authorizationinfo) );

	if( strlen(m_pszUser) == 0 || strlen(m_pszPass) == 0 )
	{
		VOLOGE( "username or password is null " );
		return VO_FALSE;
	}

	strcpy( m_authorizationinfo , "Authorization: Digest username=\"" );
	strcat( m_authorizationinfo , m_pszUser );
	strcat( m_authorizationinfo , "\"," );

	if( strlen(m_pszRealm) > 0 )
	{
		strcat( m_authorizationinfo , "realm=\"" );
		strcat( m_authorizationinfo , m_pszRealm );
		strcat( m_authorizationinfo , "\"," );
	}

	if( strlen(m_pszNonce) > 0 )
	{
		strcat( m_authorizationinfo , "nonce=\"" );
		strcat( m_authorizationinfo , m_pszNonce );
		strcat( m_authorizationinfo , "\"," );
	}

	if( strlen(m_pszURI) > 0 )
	{
		strcat( m_authorizationinfo , "uri=\"" );
		strcat( m_authorizationinfo , m_pszURI );
		strcat( m_authorizationinfo , "\"," );
	}

	if( strlen(m_pszOpaque) > 0 )
	{
		strcat( m_authorizationinfo , "opaque=\"" );
		strcat( m_authorizationinfo , m_pszOpaque );
		strcat( m_authorizationinfo , "\"," );
	}

	if( strlen(m_pszQop) > 0 )
	{
		strcat( m_authorizationinfo , "qop=" );
		strcat( m_authorizationinfo , m_pszQop );
		strcat( m_authorizationinfo , "," );
	}

	if( strlen(m_pszNonceCount) > 0 )
	{
		strcat( m_authorizationinfo , "nc=");
		strcat( m_authorizationinfo , m_pszNonceCount );
		strcat( m_authorizationinfo , "," );
	}

	if( strlen(m_pszCNonce) > 0 )
	{
		strcat( m_authorizationinfo , "cnonce=\"" );
		strcat( m_authorizationinfo , m_pszCNonce );
		strcat( m_authorizationinfo , "\"," );
	}


	//calc response string
	HASHHEX HA1;
	HASHHEX HA2="";
	HASHHEX Response;
	DigestCalcHA1( m_pszAlg, m_pszUser , m_pszRealm , m_pszPass , m_pszNonce , m_pszCNonce , HA1 );

	DigestCalcResponse( HA1 , m_pszNonce , m_pszNonceCount , m_pszCNonce , m_pszQop ,m_pszMethod , m_pszURI, HA2, Response );

	strcat( m_authorizationinfo , "response=\"" );
	strcat( m_authorizationinfo , (const char *)Response );
	strcat( m_authorizationinfo , "\"" );

	return VO_TRUE;
}

VO_BOOL vo_http_authentication::generate_authorizationinfo()
{
	if( Is_digest_auth() )
	{
		return generate_digest_authorizationinfo();
	}
	else
	if( Is_basic_auth() )
	{
		return generate_basic_authorizationinfo();
	}

	return VO_FALSE;
}

//Authorization: Basic QWxhZGRpbjpvcGVuIHN1C2fTZQ==
VO_BOOL vo_http_authentication::generate_basic_authorizationinfo()
{
	memset( m_authorizationinfo , 0 , sizeof(m_authorizationinfo) );

	if( strlen(m_pszUser) == 0 || strlen(m_pszPass) == 0 )
	{
		VOLOGE( "username or password is null " );
		return VO_FALSE;
	}

	VO_CHAR credential[200];
	memset( credential, 0, sizeof(credential) );
	strcpy( credential , m_pszUser );
	strcat( credential, ":" );
	strcat( credential, m_pszPass );

	VO_CHAR base64encode[200];
	memset( base64encode , 0 ,sizeof(base64encode) );
	if( Base64_Encode( base64encode , sizeof(base64encode) , credential , strlen(credential) ) <= 0 )
	{
		return VO_FALSE;
	}
	//if( base64_encode( credential , strlen(credential) , base64encode , sizeof(base64encode) ) <= 0)
	//{
	//	return VO_FALSE;
	//}

	strcpy( m_authorizationinfo , "Authorization: Basic " );
	strcat( m_authorizationinfo , (const char*)base64encode );

	return VO_TRUE;
}

////WWW-Authenticate: Basic realm="WallyWorld" or
//WWW-Authenticate: Digest
//realm="testrealm@host.com",
//qop="auth,auth-int",
//nonce="dcd98dff13434defcb44545455dfecb89",
//opaque="5ccc069cedfc23decb3454545452dcb32",
//domain="/dir/index.html",
//algorithm="MD5",
//stale="true"
VO_VOID vo_http_authentication::Process_Property( VO_CHAR* ptr_info , const VO_CHAR* propname )
{
	VO_CHAR *ptr_temp = strstr( ptr_info , propname );
	if( ptr_temp )
	{
		ptr_temp = ptr_temp + strlen( propname );
	}
	else
	{
		return;
	}

	while( *ptr_temp == ' ')
	{
		ptr_temp++;
	}

	if( *ptr_temp != '"')
	{
		VOLOGE(	"property format wrong: %s " , ptr_temp );
		return;
	}
	else
	{
		ptr_temp++;
	}

	VO_CHAR	* ptr_end = ptr_temp;
	while( *ptr_end && *ptr_end != '"' && *ptr_end != '\n' && *ptr_end != '\r')
	{
		ptr_end++;
	}

	if( *ptr_end == '"' )
	{
		if( strstr( propname , "realm=" ) )
		{
			if( (VO_U32)(ptr_end - ptr_temp) < sizeof( m_pszRealm ) )
			{
				memcpy( m_pszRealm , ptr_temp , ptr_end - ptr_temp );
			}
		}
		else
		if( strstr( propname , "qop=" ) )
		{
			if( (VO_U32)(ptr_end - ptr_temp) < sizeof( m_pszQop ) )
			{
				memcpy( m_pszQop , ptr_temp , ptr_end - ptr_temp );
			}
		}
		else
		if( strstr( propname , "nonce=" ) )
		{
			if( (VO_U32)(ptr_end - ptr_temp) < sizeof( m_pszNonce ) )
			{
				memcpy( m_pszNonce , ptr_temp , ptr_end - ptr_temp );
			}
		}
		else
		if( strstr( propname , "opaque=" ) )
		{
			if( (VO_U32)(ptr_end - ptr_temp) < sizeof( m_pszOpaque ) )
			{
				memcpy( m_pszOpaque , ptr_temp , ptr_end - ptr_temp );
			}
		}
		else
		if( strstr( propname , "domain=" ) )
		{
			if( (VO_U32)(ptr_end - ptr_temp) < sizeof( m_pszURI ) )
			{
				memcpy( m_pszURI , ptr_temp , ptr_end - ptr_temp );
			}
		}
		else
		if( strstr( propname , "stale=" ) )
		{
			if( (VO_U32)(ptr_end - ptr_temp) < sizeof( m_pszStale ) )
			{
				memcpy( m_pszStale , ptr_temp , ptr_end - ptr_temp );
			}
		}
		else
		if( strstr( propname , "algorithm=" ) )
		{
			if( (VO_U32)(ptr_end - ptr_temp) < sizeof( m_pszAlg ) )
			{
				memcpy( m_pszAlg , ptr_temp , ptr_end - ptr_temp );
			}
		}
		else
		{
			VOLOGE( " properties name unrocognized :%s " , ptr_temp );
		}
	}
	else
	{
		VOLOGE(	"property format wrong: %s " , ptr_temp );
	}
}


void vo_http_authentication::process_challenge_info(VO_CHAR * ptr_challenge)
{
	strcpy( m_challengeinfo , ptr_challenge );

	VO_CHAR * ptr_chainfo = m_challengeinfo;
	VO_CHAR * ptr_autheninfo_head = strstr( ptr_chainfo , "WWW-Authenticate:" );
	if( ptr_autheninfo_head )
	{
		ptr_autheninfo_head = ptr_autheninfo_head + strlen( "WWW-Authenticate:" );
		VO_CHAR *ptr_linend = ptr_autheninfo_head;
		//reach to line end , and check the authentication type
		while( *ptr_linend  && ( *ptr_linend != '\r' || *ptr_linend != '\n') )
		{
			ptr_linend++;
		}
		
		VO_CHAR authentype[2048];
		memset( authentype , 0 , sizeof(authentype) );
		if( (VO_U32)(ptr_linend - ptr_autheninfo_head) < sizeof(authentype) )
		{
			memcpy( authentype , ptr_autheninfo_head , ptr_linend - ptr_autheninfo_head );
		}
		else
		{
			VOLOGE( "the authentication type info is too long" );
			return;
		}

		//check the type base on the first line info
		if( strstr( authentype , "Basic" ) )
		{
			m_auth_type = BASIC_AUTH;
			//if it is basic authen-type, try to get realm only.
			Process_Property( ptr_autheninfo_head , "realm=");
		}
		else
		if( strstr( authentype , "Digest" ) )
		{
			m_auth_type = DIGEST_AUTH;
			//if it is Digest authen type
			Process_Property( ptr_autheninfo_head , "realm=");
			Process_Property( ptr_autheninfo_head , "qop=");
			Process_Property( ptr_autheninfo_head , "nonce=");
			Process_Property( ptr_autheninfo_head , "opaque=");
			Process_Property( ptr_autheninfo_head , "domain=");
			Process_Property( ptr_autheninfo_head , "stale=");
			Process_Property( ptr_autheninfo_head , "algorithm=");
		}
		else
		{
			m_auth_type = UNKNOW_AUTH;
		}
	}
	//else
	//{
	//	VOLOGI("the challenge did not contain valid info");
	//}
}


void vo_http_authentication::CvtHex(
    IN HASH Bin,
    OUT HASHHEX Hex
    )
{
	unsigned short i;
	unsigned char j;

	for (i = 0; i < HASHLEN; i++) 
	{
		j = (Bin[i] >> 4) & 0xf;
		if (j <= 9)
			Hex[i*2] = (j + '0');
		else
			Hex[i*2] = (j + 'a' - 10);

		j = Bin[i] & 0xf;
		if (j <= 9)
			Hex[i*2+1] = (j + '0');
		else
			Hex[i*2+1] = (j + 'a' - 10);
	};
	Hex[HASHHEXLEN] = '\0';
}

VO_CHAR vo_http_authentication::Encode_GetChar(VO_BYTE num)
{
    return 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "+/="[num];
}


VO_S32 vo_http_authentication::Base64_Encode(VO_CHAR *pDest, VO_S32 encode_space , const VO_CHAR *pSrc, VO_S32 srclen )
{
	VO_BYTE input[3], output[4];
	VO_S32 i, index_src = 0, index_dest = 0;
	for(i = 0; i < srclen; i += 3)
	{
		if( index_dest >= encode_space - 1 )
		{
			return -1;
		}

		//char [0]
		input[0] = pSrc[index_src++];
		output[0] = (VO_BYTE)(input[0] >> 2);
		pDest[index_dest++] = Encode_GetChar(output[0]);

		//char [1]
		if(index_src < srclen)
		{
			input[1] = pSrc[index_src++];
			output[1] = (VO_BYTE)(((input[0] & 0x03) << 4) + (input[1] >> 4));
			pDest[index_dest++] = Encode_GetChar(output[1]);
		}
		else
		{
			output[1] = (VO_BYTE)((input[0] & 0x03) << 4);
			pDest[index_dest++] = Encode_GetChar(output[1]);
			pDest[index_dest++] = '=';
			pDest[index_dest++] = '=';
			break;
		}

		//char [2]
		if(index_src < srclen)
		{
			input[2] = pSrc[index_src++];
			output[2] = (VO_BYTE)(((input[1] & 0x0f) << 2) + (input[2] >> 6));
			pDest[index_dest++] = Encode_GetChar(output[2]);
		}
		else
		{
			output[2] = (VO_BYTE)((input[1] & 0x0f) << 2);
			pDest[index_dest++] = Encode_GetChar(output[2]);
			pDest[index_dest++] = '=';
			break;
		}

		//char [3]
		output[3] = (VO_BYTE)(input[2] & 0x3f);
		pDest[index_dest++] = Encode_GetChar(output[3]);
	}
	//null-terminator
	pDest[index_dest] = 0;
	return index_dest;
}

/* calculate H(A1) as per spec */
void vo_http_authentication::DigestCalcHA1(
    IN char * pszAlg,
    IN char * pszUserName,
    IN char * pszRealm,
    IN char * pszPassword,
    IN char * pszNonce,
    IN char * pszCNonce,
    OUT HASHHEX SessionKey
    )
{
	vo_http_md5 md5;
	HASH HA1;

	md5.reset();
	md5.update( (VO_BYTE*)pszUserName, strlen(pszUserName));
	md5.update( (VO_BYTE*)":", 1);
	md5.update( (VO_BYTE*)pszRealm, strlen(pszRealm));
	md5.update( (VO_BYTE*)":", 1);
	md5.update( (VO_BYTE*)pszPassword, strlen(pszPassword));
	memcpy( HA1 , md5.digest() , HASHLEN );

#ifdef WINCE 
	if (_strnicmp(pszAlg, "md5-sess", strlen("md5-sess")) == 0)
#elif WIN32
	if (stricmp(pszAlg, "md5-sess") == 0) 
#else
	if (strncasecmp(pszAlg, "md5-sess" , strlen("md5-sess")) == 0) 
#endif
	{
		md5.reset();
		md5.update( HA1, HASHLEN);
		md5.update( (VO_BYTE*)":", 1);
		md5.update( (VO_BYTE*)pszNonce, strlen(pszNonce));
		md5.update( (VO_BYTE*)":", 1);
		md5.update( (VO_BYTE*)pszCNonce, strlen(pszCNonce));
		memcpy( HA1 , md5.digest() , HASHLEN );
	};
	CvtHex(HA1, SessionKey);
}

/* calculate request-digest/response-digest as per HTTP Digest spec */
void vo_http_authentication::DigestCalcResponse(
    IN HASHHEX HA1,           /* H(A1) */
    IN char * pszNonce,       /* nonce from server */
    IN char * pszNonceCount,  /* 8 hex digits */
    IN char * pszCNonce,      /* client nonce */
    IN char * pszQop,         /* qop-value: "", "auth", "auth-int" */
    IN char * pszMethod,      /* method from the request */
    IN char * pszDigestUri,   /* requested URL */
    IN HASHHEX HEntity,       /* H(entity body) if qop="auth-int" */
    OUT HASHHEX Response      /* request-digest or response-digest */
    )
{
	vo_http_md5 md5;
	HASH HA2;
	HASH RespHash;
	HASHHEX HA2Hex;

	// calculate H(A2)
	md5.reset();
	md5.update( (VO_BYTE*)pszMethod, strlen(pszMethod));
	md5.update( (VO_BYTE*)":", 1);
	md5.update( (VO_BYTE*)pszDigestUri, strlen(pszDigestUri));

#ifdef WINCE
	if (_strnicmp(pszQop, "auth-int", strlen ("auth-int")) == 0) 
#elif WIN32
	if (stricmp(pszQop, "auth-int") == 0) 
#else
	if (strncasecmp(pszQop, "auth-int" , strlen ("auth-int")) == 0) 
#endif
	{
		md5.update( (VO_BYTE*)":", 1);
		md5.update( HEntity, HASHHEXLEN);
	};
	memcpy( HA2 , md5.digest() , HASHLEN );
	CvtHex(HA2, HA2Hex);

	// calculate response
	md5.reset();
	md5.update( HA1, HASHHEXLEN);
	md5.update( (VO_BYTE*)":", 1);
	md5.update( (VO_BYTE*)pszNonce, strlen(pszNonce));
	md5.update( (VO_BYTE*)":", 1);
	if (*pszQop) 
	{
		md5.update( (VO_BYTE*)pszNonceCount, strlen(pszNonceCount));
		md5.update( (VO_BYTE*)":", 1);
		md5.update( (VO_BYTE*)pszCNonce, strlen(pszCNonce));
		md5.update( (VO_BYTE*)":", 1);
		md5.update( (VO_BYTE*)pszQop, strlen(pszQop));
		md5.update( (VO_BYTE*)":", 1);
	};
	md5.update( HA2Hex, HASHHEXLEN);
	memcpy( RespHash , md5.digest() , HASHLEN );
	CvtHex(RespHash, Response);
}





