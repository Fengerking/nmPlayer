


#ifndef WSERVICE_H
#define WSERVICE_H

#include <stdlib.h>



#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


struct wvideo_object_s
{
    // surface access...
    //void *  (*surface_new) ();  // returns a surface context, e.g. "ctx" below
    //void  (*surface_delete) (void *  ctx);
    int  (*surface_prepare) (void *  ctx);
    int  (*surface_push) (void *  ctx, const unsigned char *  buffer, int  nelem);
    int  (*surface_release) (void *  ctx);
    void *  _ctx;
};

typedef struct wvideo_object_s  wvideo_object_t;

typedef wvideo_object_t *  wvideo_t;


struct waudio_object_s
{
    // audio access...
    //void *  (*audio_new) (); // returns an audio context, e.g. "ctx" below
    //void  (*audio_delete) (void *  ctx);
    int  (*audio_set_volume) (void *  ctx, float  left, float  right);
    int  (*audio_prepare) (void *  ctx, int  sampleRateInHz, int  nchannels, int  sampleFormatInBits, int  bufferSizeInBytes);
    int  (*audio_start) (void *  ctx);
    int  (*audio_write) (void *  ctx, const unsigned char *  data, int  size); // returns # of data bytes pushed to the device
    int  (*audio_stop) (void *  ctx);
    int  (*audio_release) (void *  ctx);
    void *  _ctx;
};

typedef struct waudio_object_s  waudio_object_t;

typedef waudio_object_t *  waudio_t;



struct wservice_object_s
{
    // shared object access...
    void *  (*wdlopen) (const char *  libname);
    void *  (*dlsym) (void *  handle, const char *  symbol);

    // surface access...
    //void *  (*surface_new) ();  // returns a surface context, e.g. "ctx" below
    //void  (*surface_delete) (void *  ctx);
    int  (*surface_prepare) (void *  ctx);
    int  (*surface_push) (void *  ctx, const unsigned char *  buffer, int  nelem);
    int  (*surface_release) (void *  ctx);
    void *  _surface_ctx;
    int  _surface_width;
    int  _surface_height;

    // audio access...
    //void *  (*audio_new) (); // returns an audio context, e.g. "ctx" below
    //void  (*audio_delete) (void *  ctx);
    int  (*audio_set_volume) (void *  ctx, float  left, float  right);
    int  (*audio_prepare) (void *  ctx, int  sampleRateInHz, int  nchannels, int  sampleFormatInBits, int  bufferSizeInBytes);
    int  (*audio_start) (void *  ctx);
    int  (*audio_write) (void *  ctx, const unsigned char *  data, int  size); // returns # of data bytes pushed to the device
    int  (*audio_stop) (void *  ctx);
    int  (*audio_release) (void *  ctx);
    void *  _audio_ctx;
};


typedef struct wservice_object_s  wservice_object_t;

typedef wservice_object_t *  wservice_t;


// to be implemented by media player library:
int  set_wservice (wservice_object_t *  wservice);
extern wservice_object_t *  gwserv;

typedef int (*set_wservice_fctn_t) (wservice_object_t *  wservice);


// with empty functions, for development:
void *  default_wserv_wdlopen (const char *  libname);
void *  default_wserv_dlsym (void *  handle, const char *  symbol);
wservice_object_t *  get_wserv_default ();


#ifdef __cplusplus
} // end extern "C"
#endif // __cplusplus


#endif // WSERVICE_H

// EOF!

