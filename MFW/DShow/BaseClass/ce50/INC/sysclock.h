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
// System Clock implementation of IReferenceClock
// SYSCLOCK.H

#ifndef __SYSTEMCLOCK__
#define __SYSTEMCLOCK__

//
// Base clock.  Uses timeGetTime ONLY
// Uses most of the code in the base reference clock.
// Provides GetTime
//

class CSystemClock : public CBaseReferenceClock, public IAMClockAdjust, public IPersist
{
public:
    // We must be able to create an instance of ourselves
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
    CSystemClock(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr);

    DECLARE_IUNKNOWN

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void ** ppv);

    // Yield up our class id so that we can be persisted
    // Implement required Ipersist method
    STDMETHODIMP GetClassID(CLSID *pClsID);

    //  IAMClockAdjust methods
    STDMETHODIMP SetClockDelta(REFERENCE_TIME rtDelta);
}; //CSystemClock

#endif /* __SYSTEMCLOCK__ */
