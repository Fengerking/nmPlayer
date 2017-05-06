/*
 *  voLicenseMng.cpp
 *  vompEngn
 *
 *  Created by Lin Jun on 12/15/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "voLicenseMng.h"
#include <sys/time.h>
#include <unistd.h>

voLicenseMng::voLicenseMng()
:m_bVideoFrameBlack(false)
{
	memset(m_szKey, 0, 1024);
}

voLicenseMng::~voLicenseMng()
{
}


int	voLicenseMng::SetKey(VO_TCHAR* pszKey)
{
	memset(m_szKey, 0, 1024);
	strcpy(m_szKey, pszKey);
	return 0;
}

int voLicenseMng::GenerateLicenseFile(VO_TCHAR* pszFilePath, VO_TCHAR* pszEncryptData)
{
	printf("%s\n", pszEncryptData);
	
	char szTmp[1024];
	memset(szTmp, 0, 1024);
	
	Encrypt(pszEncryptData, szTmp);
	
	FILE* hFile = fopen(pszFilePath, "wb");
	if(!hFile)
		return -1;
	
	fwrite(szTmp, 1, strlen(szTmp), hFile);
	fclose(hFile);
	
	printf("%s\n", szTmp);
	
	return 0;
}

int voLicenseMng::CheckLicense(VO_TCHAR* pszFilePath)
{
	int iLength = strlen(m_szKey);
	// sample: VOTRUST_#20120430Tarasystem
	if ((16 <= iLength) && ('#' == m_szKey[8])) {
		char szYear[5];
		memset(szYear, 0, sizeof(szYear));
		strncpy(szYear, m_szKey+9, 4);
		int iYear = atoi(szYear);

		char szMonth[3];
		memset(szMonth, 0, sizeof(szMonth));
		strncpy(szMonth, m_szKey+13, 2);
		int iMonth = atoi(szMonth);
		
		char szDay[3];
		memset(szDay, 0, sizeof(szDay));
		strncpy(szDay, m_szKey+15, 2);
		int iDay = atoi(szDay);
		
		time_t t = time(0);
		struct tm *timeInfo = localtime(&t);
		timeInfo->tm_year += 1900;
		++(timeInfo->tm_mon);
		
		if (timeInfo->tm_year > iYear) {
			return -1;
		}
		else if (timeInfo->tm_year == iYear) {
			if (timeInfo->tm_mon > iMonth) {
				return -1;
			}
			else if (timeInfo->tm_mon == iMonth) {
				if (timeInfo->tm_mday > iDay) {
					return -1;
				}
			}
			else {}
		}
		else {}
	}
	
	char szTmp[1024], szDecrypt[1024];
	memset(szTmp, 0, 1024);
	memset(szDecrypt, 0, 1024);
	
	FILE* hFile = fopen(pszFilePath, "rb");

	if(!hFile)
		return -1;
				   
	fread(szTmp, 1, 1024, hFile);
	fclose(hFile);
	
	Decrypt(szTmp, szDecrypt);
	//printf("Decrypt %s, key %s\n", szDecrypt, m_szKey);
				   
	if(!strncmp(m_szKey, szDecrypt, strlen(m_szKey)))
		return 0;
				   
	return -1;
}


int voLicenseMng::Encrypt(char*   cSrc,char*   cDest)
{
	char   c;   
	int   i,h,l,j=0;   
	for   (i=0;i<(int)strlen(cSrc);i++)   
	{   
		c=cSrc[i];   
		h=(c>>4)&0xf;   
		l=c&0xf;   
		cDest[j]=h+'x';   
		cDest[j+1]=l+'z';   
		j+=2;   
	}   
	cDest[j]='\0';   
	return   0; 	
}

int voLicenseMng::Decrypt(char*   cSrc,char*   cDest)
{
	int   i,h,l,m,n,j=0;   
	for   (i=0;i<(int)strlen(cSrc);i=i+2)   
	{   
		h=(cSrc[i]-'x');   
		l=(cSrc[i+1]-'z');   
		m=(h<<4);   
		n=(l&0xf);   
		cDest[j]=m   +   n;   
		j++;   
	}   
	cDest[j]='\0';   
	return   0;   
}

bool voLicenseMng::IsVideoFrameBlack()
{
	return m_bVideoFrameBlack;
}