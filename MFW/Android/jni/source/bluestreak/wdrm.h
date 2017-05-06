


#ifndef WDRM_H
#define WDRM_H

#include <stdlib.h>



#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus



struct wdrm_s
{
    void *  (*alloc) (const char *  filename, const char *  store);
    void  (*release) (void *  drm);
    void  (*set_iv) (void *  drm, unsigned char *  aIv, int  aIvSize);
    int  (*decrypt_packet) (void *  drm, unsigned char *  aPacket, int  aPacketSize);
    char *  _store;
    void *  _ctx;
    struct wdrm_s *  _this;
};

typedef struct wdrm_s  wdrm_object_t;
typedef wdrm_object_t *  wdrm_t;


int  set_wdrm (wdrm_object_t *  wdrm);
wdrm_object_t *  get_wdrm ();


#ifdef __cplusplus
} // end extern "C"
#endif // __cplusplus


#endif // WDRM_H

// EOF!

