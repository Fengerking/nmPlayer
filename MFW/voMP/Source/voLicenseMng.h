/*
 *  voLicenseMng.h
 *  vompEngn
 *
 *  Created by Lin Jun on 12/15/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */
#ifndef _voLicenseMng_H_
#define _voLicenseMng_H_

#include "voString.h"

class voLicenseMng
{
public:
	voLicenseMng();
	virtual ~voLicenseMng(void);
	
public:
	int	SetKey(VO_TCHAR* pszKey);
	int GenerateLicenseFile(VO_TCHAR* pszFilePath, VO_TCHAR* pszEncryptData);
	int CheckLicense(VO_TCHAR* pszFilePath);
	
	bool IsVideoFrameBlack();
	
private:
	int Encrypt(char*   cSrc,char*   cDest);
	int Decrypt(char*   cSrc,char*   cDest);
	
private:
	VO_TCHAR	m_szKey[1024];
	bool		m_bVideoFrameBlack;
};

#endif //_voLicenseMng_H_