
#include <jni.h>
#include "vomeplayer.h"
#include "player_interface.h"

#if defined __cplusplus
extern "C" {
#endif

namespace android {

void* com_VisualOn_VomePlayer_new()
{
	vomeplayer* p = new vomeplayer;
  return (void*)p;
}  // end fctn


void  com_VisualOn_VomePlayer_delete (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  if ( p != NULL )  delete p;
}  // end fctn


int  com_VisualOn_VomePlayer_set_surface (void *  ctx, void *  surface)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  ::android::sp< ::android::Surface >  s = (::android::Surface *)  surface;
  return  p->setVideoSurface(s);
}  // end fctn



int  com_VisualOn_VomePlayer_initialize (void *  ctx)
{
  return  0;  // function not used
}  // end fctn


int  com_VisualOn_VomePlayer_reset (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  p->reset();
}  // end fctn


int  com_VisualOn_VomePlayer_open (void *  ctx, const char *  uri)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  p->setDataSource(uri);
}  // end fctn


int  com_VisualOn_VomePlayer_prepare (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  p->prepare();
}  // end fctn


int  com_VisualOn_VomePlayer_start (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  p->start();
}  // end fctn


int  com_VisualOn_VomePlayer_stop (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  p->stop();
}  // end fctn


int  com_VisualOn_VomePlayer_pause (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  p->pause();
}  // end fctn


int  com_VisualOn_VomePlayer_is_playing (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  ((p->isPlaying())?1:0);
}  // end fctn


int  com_VisualOn_VomePlayer_is_looping (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  ((p->isLooping())?1:0);
}  // end fctn


int  com_VisualOn_VomePlayer_seek (void *  ctx, int  pos)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  p->seekTo(pos);
}  // end fctn


int  com_VisualOn_VomePlayer_set_looping (void *  ctx, int  looping)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  jboolean l = (looping)?true:false;
  return  p->setLooping(l);
}  // end fctn


int  com_VisualOn_VomePlayer_set_volume (void *  ctx, float  left, float  right)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  return  p->setVolume(left,right);
}  // end fctn


int  com_VisualOn_VomePlayer_close (void *  ctx)
{
  return  0;  // function not used
}  // end fctn


int  com_VisualOn_VomePlayer_release (void *  ctx)
{
  if ( ctx == NULL )  return -__LINE__;
  vomeplayer *  p = (vomeplayer *)  ctx;
  p->setListener(0);
  p->disconnect();
  return  0;
}  // end fctn


int  com_VisualOn_VomePlayer_get_video_width (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  int  w = 0;
  if ( p->getVideoWidth(&w) < 0 )  w = 0;
  return  w;
}  // end fctn


int  com_VisualOn_VomePlayer_get_video_height (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  int  h = 0;
  if ( p->getVideoHeight(&h) < 0 )  h = 0;
  return  h;
}  // end fctn


int  com_VisualOn_VomePlayer_get_position (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  int  r = 0;
  if ( p->getCurrentPosition(&r) < 0 )  r = 0;
  return  r;
}  // end fctn


int  com_VisualOn_VomePlayer_get_duration (void *  ctx)
{
  vomeplayer *  p = (vomeplayer *)  ctx;
  int  r = 0;
  if ( p->getDuration(&r) < 0 )  r = 0;
  return  r;
}  // end fctn



static mp_object_t  _g_vvwmv;

mp_object_t *  get_vvwmv ()
{
  // mp_object_t *  r = (mp_object_t *) calloc(1,sizeof(mp_object_t));
  mp_object_t *  r = & _g_vvwmv;
  if ( r == NULL )  return  NULL;
  r->alloc = & com_VisualOn_VomePlayer_new ;
  r->free = & com_VisualOn_VomePlayer_delete ;
  r->set_surface = & com_VisualOn_VomePlayer_set_surface ;
  r->initialize = & com_VisualOn_VomePlayer_initialize ;
  r->reset = & com_VisualOn_VomePlayer_reset ;
  r->open = & com_VisualOn_VomePlayer_open ;
  r->prepare = & com_VisualOn_VomePlayer_prepare ;
  r->start = & com_VisualOn_VomePlayer_start ;
  r->pause = & com_VisualOn_VomePlayer_pause ;
  r->seek = & com_VisualOn_VomePlayer_seek ;
  r->set_looping = & com_VisualOn_VomePlayer_set_looping ;
  r->set_volume = & com_VisualOn_VomePlayer_set_volume ;
  r->stop = & com_VisualOn_VomePlayer_stop ;
  r->close = & com_VisualOn_VomePlayer_close ;
  r->release = & com_VisualOn_VomePlayer_release ;
  r->get_video_width = & com_VisualOn_VomePlayer_get_video_width ;
  r->get_video_height = & com_VisualOn_VomePlayer_get_video_height ;
  r->get_position = & com_VisualOn_VomePlayer_get_position ;
  r->get_duration = & com_VisualOn_VomePlayer_get_duration ;
  r->is_playing = & com_VisualOn_VomePlayer_is_playing ;
  r->is_looping = & com_VisualOn_VomePlayer_is_looping ;
  return  r;
}  // end fctn



mp_object_t *  get_mp ()
{
  return get_vvwmv();
}

};

#if defined __cplusplus
}
#endif
// EOF!

