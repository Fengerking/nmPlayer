


#ifndef PLAYER_INTERFACE_H
#define PLAYER_INTERFACE_H

#include <stdlib.h>
#include <wservice.h>
#include <wdrm.h>

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// C Definitions

// In this section, we show a C interface style, just as an example...

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus



// when using a wrapper structure, quite convenient to have one dlsym() call to
// get_mp() symbol...

struct  mp_s
{
    struct mp_s *  _this;
    void *  _ctx; // private context set by framework
    void *  _mp;
    void *  (*alloc) ();
    void  (*free) (void *  mp);

    int  (*set_wservice) (void *  mp, wservice_object_t *  wserv); // not used, please ignore
    int  (*set_wdrm) (void *  mp, wdrm_object_t *  wdrm); // not used, please ignore

    int  (*set_surface) (void *  mp, void *  surface);

    // we do not need callback, if I understand correctly...
    int  (*setcb_on_opened) (struct mp_s *  thiz, void *  mp, int (*__on_opened) (void *  mp) );
    int  (*setcb_on_prepared) (struct mp_s *  thiz, void *  mp, int (*__on_prepared) (void *  mp) );
    int  (*setcb_on_started) (struct mp_s *  thiz, void *  mp, int (*__on_started) (void *  mp) );
    int  (*setcb_on_paused) (struct mp_s *  thiz, void *  mp, int (*__on_paused) (void *  mp) );
    int  (*setcb_on_stopped) (struct mp_s *  thiz, void *  mp, int (*__on_stopped) (void *  mp) );
    int  (*setcb_on_close) (struct mp_s *  thiz, void *  mp, int (*__on_close) (void *  mp) );

    int  (*initialize) (void *  mp);
    int  (*reset) (void *  mp);
    int  (*open) (void *  mp, const char *  uri);
    int  (*prepare) (void *  mp);
    int  (*start) (void *  mp);
    int  (*pause) (void *  mp);
    int  (*seek) (void *  mp, int  position);
    int  (*set_looping) (void *  mp, int  looping);
    int  (*set_volume) (void *  mp, float  left, float  right);
    int  (*stop) (void *  mp);
    int  (*close) (void *  mp);
    int  (*release) (void *  mp);

    int  (*get_video_width) (void *  mp);
    int  (*get_video_height) (void *  mp);
    int  (*get_position) (void *  mp);
    int  (*get_duration) (void *  mp);
    int  (*is_playing) (void *  mp);
    int  (*is_looping) (void *  mp);

};

typedef struct mp_s  mp_object_t;

// and we still need a function to retrieve the structure with dlsym():

mp_object_t *  get_mp ();

typedef mp_object_t *  (*get_mp_fctn_t) ();


#ifdef __cplusplus
} // end extern "C"
#endif // __cplusplus


#endif // PLAYER_INTERFACE_H

// EOF!

