/**
*	Define Some OMX Macros  
*
*	David@2011/12/2
*/


#if !defined __VOOMX_DEFS__
#define __VOOMX_DEFS__

template<class T>
static void InitOMXParams(T *params) {
    params->nSize = sizeof(T);
    params->nVersion.s.nVersionMajor = 1;
    params->nVersion.s.nVersionMinor = 0;
    params->nVersion.s.nRevision = 0;
    params->nVersion.s.nStep = 0;
}

#endif