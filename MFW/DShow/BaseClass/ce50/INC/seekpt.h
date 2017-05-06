//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//
//--------------------------------------------------------------------------;
#ifndef __seekpt_h__
#define __seekpt_h__


class CSeekingPassThru : public ISeekingPassThru, public CUnknown
{
public:
    static CUnknown *CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
    CSeekingPassThru(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr);
    ~CSeekingPassThru();

    DECLARE_IUNKNOWN;
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    STDMETHODIMP Init(BOOL bSupportRendering, IPin *pPin);

private:
    CPosPassThru              *m_pPosPassThru;
};

#endif
