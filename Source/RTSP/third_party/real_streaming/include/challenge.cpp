/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: challenge.c,v 1.1.1.1.2.1 2005/05/04 18:22:04 hubbe Exp $
 * 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc.
 * All Rights Reserved.
 * 
 * The contents of this file, and the files included with this file,
 * are subject to the current version of the Real Format Source Code
 * Porting and Optimization License, available at
 * https://helixcommunity.org/2005/license/realformatsource (unless
 * RealNetworks otherwise expressly agrees in writing that you are
 * subject to a different license).  You may also obtain the license
 * terms directly from RealNetworks.  You may not use this file except
 * in compliance with the Real Format Source Code Porting and
 * Optimization License. There are no redistribution rights for the
 * source code of this file. Please see the Real Format Source Code
 * Porting and Optimization License for the rights, obligations and
 * limitations governing use of the contents of the file.
 * 
 * RealNetworks is the developer of the Original Code and owns the
 * copyrights in the portions it created.
 * 
 * This file, and the files included with this file, is distributed and
 * made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL
 * SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT
 * OR NON-INFRINGEMENT.
 * 
 * Technology Compatibility Kit Test Suite(s) Location:
 * https://rarvcode-tck.helixcommunity.org
 * 
 * Contributor(s):
 * 
 * ***** END LICENSE BLOCK ***** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "challenge.h"
#include "rdtpck.h" /* for packing funcs */
#include "md5.h"
#include "rm_memory_default.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

// This needs to be exactly 64 bytes + terminating NULL
static const char s_szRnCopy[] =
    "   Copyright (c) 2007 RealNetworks, Inc., All Rights Reserved   ";

const INT32 G2_BETA_EXPIRATION = 921484909;

#define RC_COPYTRAP1 "01d0a8e3"
#define RC_COPYTRAP2 "4f213d09"

const INT32 RC_MAGIC1 = 0xa1e9149d;
const INT32 RC_MAGIC2 = 0x0e6b3b59;
const INT32 RC_MAGIC3 = 0xf2788ef2;
const INT32 RC_MAGIC4 = 0x7cac10d7;

const unsigned char HX_MAGIC_TXT_1[] = {
                            0x05, 0x18, 0x74, 0xd0, 0x0d,
                            0x09, 0x02, 0x53, 0xc0, 0x01,
                            0x05, 0x05, 0x67,
                            0x03, 0x19, 0x70,
                            0x08, 0x27, 0x66,
                            0x10, 0x10, 0x72,
                            0x08, 0x09, 0x63,
                            0x11, 0x03, 0x71,
                            0x08, 0x08, 0x70,
                            0x02, 0x10, 0x57,
                            0x05, 0x18, 0x54,
                            0x0
                       };



const unsigned char pRCMagic1[] = {
    0x05, 0x18, 0x74, 0xd0, 0x0d,
    0x09, 0x02, 0x53, 0xc0, 0x01,
    0x05, 0x05, 0x67,
    0x03, 0x19, 0x70,
    0x08, 0x27, 0x66,
    0x10, 0x10, 0x72, 
    0x08, 0x09, 0x63,
    0x11, 0x03, 0x71,
    0x08, 0x08, 0x70,
    0x02, 0x10, 0x57,
    0x05, 0x18, 0x54,
    0x0
};
const unsigned char pRCMagic2[] = {
    0x08, 0x08, 0x70,
    0x05, 0x05, 0x67,
    0x11, 0x03, 0x71,
    0x05, 0x18, 0x54,
    0x03, 0x19, 0x70,
    0x05, 0x18, 0x06, 0x74, 0xd0, 0x0d, 0x5,
    0x08, 0x27, 0x66,
    0x10, 0x10, 0x72,
    0x09, 0x02, 0x53, 0xc0, 0x01,
    0x08, 0x09, 0x63,
    0x02, 0x10, 0x57,
    0x0
};

const char pMagic2[] = "Copyright(c) Progressive Networks, Inc 1995-2007";

/*
 * Both the server and the player create their own challenge.
 * Player sends its challenge to the server.  Server runs
 * response1() on it and sends back response1 as well as its own
 * challenge.  Player runs response1 on its origial challenge
 * with some new parameters (obtained from the server) and compares
 * the two responses.  If same, it prepares response2() and sends
 * it back.  The server will run response2 on its challenge
 * and compare the responses.  If either fails the connection
 * is broken.
 */


/* 
 * The user is responsible for free'ing the challenge struct
 */
struct Challenge* _createChallenge(INT32 k1, INT32 k2, BYTE* k3, BYTE* k4,
                                   rm_malloc_func_ptr fpMalloc,
                                   void* pMemoryPool)
{
    struct Challenge* pTmp = (Challenge*)fpMalloc(pMemoryPool, sizeof(struct Challenge));
    assert(pTmp);
    if( pTmp )
    {
        BYTE key[64];
        memset(key, 0, sizeof key);
        putlong(key, k1);
        putlong(key+4, k1);
        if(k3)
        {
            int len = HX_MIN(56,strlen((char*)k3));
            memcpy(key+8, k3, len); 
        }
        if(k4)
        {
            int len = HX_MIN(56,strlen((char*)k4));
            BYTE * bp = key+8;
            int i = 0;
            for (i = 0; i < len; i++)
                *bp++ ^= k4[i];
        }
        MD5Data((char*)pTmp->text, (char *)key, 64);
    }
    return pTmp;
}

struct Challenge* CreateChallenge(INT32 k1, INT32 k2, BYTE* k3, BYTE* k4)
{
    return _createChallenge(k1, k2, k3, k4, rm_memory_default_malloc, NULL);
}

struct Challenge* CreateChallengeFromPool(INT32 k1,
                                          INT32 k2,
                                          BYTE* k3,
                                          BYTE* k4,
                                          rm_malloc_func_ptr fpMalloc,
                                          void* pMemoryPool)
{
    return _createChallenge(k1, k2, k3, k4, fpMalloc, pMemoryPool);    
}


BYTE* ChallangeResponse1(BYTE* k1, BYTE* k2, INT32 k3, INT32 k4,
                         struct Challenge* ch)
{
    BYTE* pRet = NULL;
    assert(ch);
    
    if( ch )
    {
        BYTE key[64];
        memset(key, 0, sizeof key);
        putlong(key, k3);
        putlong(key+4, k4);
        if(k1)
        {
            int len = HX_MIN(56,strlen((char*)k1));
            memcpy(key+8, k1, len);
        }
        if(k2)
        {
            int len = HX_MIN(56,strlen((char*)k2));
            BYTE * bp = key+8;
            int i = 0;
            for (i = 0; i < len; i++)
                *bp++ ^= k2[i];
        }
        MD5Data((char*)ch->response, (char *)key, 64);
        pRet = ch->response;
    }
    return pRet;
}

BYTE* ChallangeResponse2(BYTE* k1, BYTE* k2, INT32 k3, INT32 k4,
                         struct Challenge* ch)
{
    BYTE* pRet = NULL;
    assert(ch);
    
    if( ch )
    {
        BYTE key[64];
        memset(key, 0, sizeof key);
        putlong(key, k3);
        putlong(key+4, k4);
        if(k1)
        {
            int len = HX_MIN(56,strlen((char*)k1));
            memcpy(key+8, k1, len); 
        }
        if(k2)
        {
            int len = HX_MIN(56,strlen((char*)k2));
            BYTE * bp = key+8;
            int i=0;
            for (i = 0; i < len; i++)
                *bp++ ^= k2[i];
        }
        MD5Data((char*)ch->response, (char *)key, 64);
        pRet = ch->response;
    }
    return pRet;
}


/*
 * caller must free struct 
 */
struct RealChallenge* _createRealChallenge(rm_malloc_func_ptr fpMalloc,
                                          void* pMemoryPool)
{
    HX_DATETIME d;
    UINT32      ulRand = 0;
    BYTE        key[64];
    
    struct RealChallenge* pTmp = (RealChallenge*)fpMalloc( pMemoryPool, sizeof(struct RealChallenge));
    assert(pTmp);
    if( pTmp )
    {
        memset(key, 0, sizeof key);
        
        // Obtain the current time
        d = HX_GET_DATETIME();

        ulRand = (rand() + (d.second << 21)) ^ d.minute << 4;
        
        // Format time string
        sprintf((char*)key, "%.2d%.2d%.4d%.2d%.2d%.2d%ld",
                    d.dayofmonth, d.month, d.year + HX_YEAR_OFFSET,
                    d.hour, d.minute, d.second, ulRand);
        
        MD5Data((char*)pTmp->challenge, (char *)key, 64);
    }
    
    return pTmp;
}

struct RealChallenge* CreateRealChallenge()
{
    return _createRealChallenge(rm_memory_default_malloc, NULL);
}

struct RealChallenge* CreateRealChallengeFromPool(rm_malloc_func_ptr fpMalloc,
                                                  void* pMemoryPool)
{
    return _createRealChallenge(fpMalloc, pMemoryPool );
}



BYTE* RealChallengeResponse1(BYTE* k1, BYTE* k2, INT32 k3, INT32 k4,
                             struct RealChallenge* rch)
{
    BYTE* pRet = NULL;
    int   len  = 0;
    BYTE* bp   = NULL;
    int   i,j  = 0;
    
    assert(rch);
    if( rch )
    {
        BYTE key[64];
        memset(key, 0, sizeof key);
        putlong(key, k3);
        putlong(key+4, k4);
        if(k1)
        {
            if (strlen((char*)k1) == 40)
            {
                k1[32] = 0;
            }
            len = HX_MIN(56,strlen((char*)k1));
            memcpy(key+8, k1, len);
        }
        if(k2)
        {
            len = HX_MIN(56,strlen((char*)k2));
            bp = key+8;
            i = 0;
            for(i = 0; i < len; i++)
                *bp++ ^= k2[i];
        }
        MD5Data((char*)rch->response, (char *)key, 64);
        sprintf((char*)&rch->response[32], RC_COPYTRAP1);

        memset(rch->trap, 0, sizeof(rch->trap));
        i = 0;
        j = 0;
        for(j = 0; j < 32; j += 4)
        {
            rch->trap[i] = rch->response[j];
            i++;
        }

        pRet = rch->response;
    }
    return pRet;
}

BYTE* RealChallengeResponse2(BYTE* k1, BYTE* k2, INT32 k3, INT32 k4,
                             struct RealChallenge* rch)
{
    BYTE* pRet = NULL, *bp = NULL;
    BYTE key[64];
    int i,j,len = 0;
    
    
    assert(rch);
    if( rch )
    {
        memset(key, 0, sizeof key);
        putlong(key, k3);
        putlong(key+4, k4);
        if(k1)
        {
            if (strlen((char*)k1) == 40)
            {
                k1[32] = 0;
            }
            len = HX_MIN(56,strlen((char*)k1));
            memcpy(key+8, k1, len); 
        }
        if(k2)
        {
            len = HX_MIN(56,strlen((char*)k2));
            bp = key+8;
            i = 0;
            for(i = 0; i < len; i++)
                *bp++ ^= k2[i];
        }
        MD5Data((char*)rch->response, (char *)key, 64);
        sprintf((char*)&rch->response[32], RC_COPYTRAP2);

        memset(rch->trap, 0, sizeof(rch->trap));
        i = 0;
        j = 0;
        for(j = 0; j < 32; j += 4)
        {
            rch->trap[i] = rch->response[j];
            i++;
        }

        pRet = rch->response;
    }
    return pRet;
}



void CalcCompanyIDKey(const char* companyID,
		      const char* starttime,
		      const char* VOGUID,
		      const char* challenge,
		      const char* copyright,
		      UCHAR* outputKey)
{
    // calculate the MD5 fingerprint 
    md5_state_t ctx;

    if( !companyID || !starttime || !VOGUID || !challenge || !copyright )
    {
        return;
    }
    md5_init(&ctx);

    // use the password
    md5_append(&ctx, (const UCHAR*)companyID, strlen(companyID));

    // use the username
    md5_append(&ctx, (const UCHAR*)starttime, strlen(starttime));

    // use the VOGUID
    md5_append(&ctx, (const UCHAR*)VOGUID, strlen(VOGUID));
    
    // use the challenge
    md5_append(&ctx, (const UCHAR*)challenge, strlen(challenge));

    // use the copyright
    md5_append(&ctx, (const UCHAR*)copyright, strlen(copyright));

    md5_finish((UCHAR*)outputKey,&ctx);
}



static const char Base64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void Output64Chunk( int c1,
                           int c2,
                           int c3,
                           int pads,
                           char* pBuf,
                           INT32 bufOffset
                           )
{
    pBuf[bufOffset++] = Base64[c1>>2];
    pBuf[bufOffset++] = Base64[((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4)];
    if (pads == 2) 
    {
        pBuf[bufOffset++] = '=';
        pBuf[bufOffset++] = '=';
    }
    else if (pads) 
    {
        pBuf[bufOffset++] = Base64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
        pBuf[bufOffset++] = '=';
    } 
    else 
    {
        pBuf[bufOffset++] = Base64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
        pBuf[bufOffset++] = Base64[c3 & 0x3F];
    }
}


INT32 
BinTo64(const BYTE* pInBuf, INT32 len, char* pOutBuf)
{
    int c1, c2, c3;
    INT32 inOffset = 0;
    INT32 outOffset = 0;

    while(inOffset < len)
    {
	c1 = pInBuf[inOffset++];
	if(inOffset == len)
	{
	    Output64Chunk(c1, 0, 0, 2, pOutBuf, outOffset);
	    outOffset += 4;
	}
	else
	{
	    c2 = pInBuf[inOffset++];
	    if(inOffset == len)
	    {
		Output64Chunk(c1, c2, 0, 1, pOutBuf, outOffset);
		outOffset += 4;
	    }
	    else
	    {
		c3 = pInBuf[inOffset++];
		Output64Chunk(c1, c2, c3, 0, pOutBuf, outOffset);
		outOffset += 4;
	    }
	}
    }
    pOutBuf[outOffset++] = '\0';
    return outOffset;
}
#ifdef _VONAMESPACE
}
#endif