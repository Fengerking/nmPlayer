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

// To be self registering, OLE servers must
// export functions named DllRegisterServer
// and DllUnregisterServer.  To allow use of
// custom and default implementations the
// defaults are named AMovieDllRegisterServer
// and AMovieDllUnregisterServer.
//
// To the use the default implementation you
// must provide stub functions.
//
// i.e. STDAPI DllRegisterServer()
//      {
//        return AMovieDllRegisterServer();
//      }
//
//      STDAPI DllUnregisterServer()
//      {
//        return AMovieDllUnregisterServer();
//      }
//
//
// AMovieDllRegisterServer   calls IAMovieSetup.Register(), and
// AMovieDllUnregisterServer calls IAMovieSetup.Unregister().

STDAPI AMovieDllRegisterServer2( BOOL );
STDAPI AMovieDllRegisterServer();
STDAPI AMovieDllUnregisterServer();

// helper functions
STDAPI EliminateSubKey( HKEY, LPTSTR );


#ifndef NO_FILTERMAPPER2
STDAPI
AMovieSetupRegisterFilter2( const AMOVIESETUP_FILTER * const psetupdata
                          , IFilterMapper2 *                 pIFM2
                          , BOOL                             bRegister  );
#endif //!NO_FILTERMAPPER2


