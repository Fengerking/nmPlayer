


#ifndef WLINKER_H
#define WLINKER_H

#include <stdlib.h>
#include <wdrm.h>


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus



struct wlinker_s
{
    // shared object access...
    void *  (*wdlopen) (struct wlinker_s *  thiz, const char *  libname, int  mode);
    void *  (*wdlsym) (struct wlinker_s *  thiz, void *  handle, const char *  symbol);
    void *  _ctx;
    struct wdrm_s *  _wdrm;
    struct wlinker_s *  _this;
};


typedef struct wlinker_s  wlinker_object_t;
typedef wlinker_object_t *  wlinker_t;


int  set_wlinker (wlinker_object_t *  wlinker);
wlinker_object_t *  get_wlinker ();



#ifdef __cplusplus
} // end extern "C"
#endif // __cplusplus


#endif // WLINKER_HPP

// EOF!

