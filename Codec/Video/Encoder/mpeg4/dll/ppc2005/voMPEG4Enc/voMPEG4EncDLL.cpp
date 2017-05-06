
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LICENSE_CHECK
void extern * voMPEG4EnchInst;
#endif
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
     {

         case DLL_PROCESS_ATTACH:

         case DLL_THREAD_ATTACH:
#ifdef LICENSE_CHECK
              voMPEG4EnchInst = (HINSTANCE) hModule;
#endif
              break;

         case DLL_THREAD_DETACH:

         case DLL_PROCESS_DETACH:

//            g_hVidDecInst = NULL;

              break;

    }

    return TRUE;
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
