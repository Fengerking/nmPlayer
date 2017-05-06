
#include <wdrm.h>



void *  wdrm_alloc (const char *  filename, const char *  store)
{
  return NULL;
}  // end fctn



void  wdrm_release (void *  drm)
{
}  // end fctn


void  wdrm_set_iv (void *  drm, unsigned char *  aIv, int  aIvSize)
{
}  // end fctn


int  wdrm_decrypt_packet (void *  drm, unsigned char *  aPacket, int  aPacketSize)
{
  return 0;
}  // end fctn




static wdrm_object_t *  default_wdrm = NULL;

static wdrm_object_t *  get_wdrm_default ()
{
  if ( default_wdrm == NULL )  default_wdrm = (wdrm_object_t *) malloc(sizeof(wdrm_object_t));
  if ( default_wdrm == NULL )  return NULL;
  default_wdrm->_this = default_wdrm;
  default_wdrm->alloc = & wdrm_alloc ;
  default_wdrm->release = & wdrm_release ;
  default_wdrm->set_iv = & wdrm_set_iv ;
  default_wdrm->decrypt_packet = & wdrm_decrypt_packet ;
  return default_wdrm;
}  // end fctn



static wdrm_object_t *  local_wdrm = NULL;

int  set_wdrm (wdrm_object_t *  wdrm)
{
  if ( wdrm == NULL )  return -__LINE__;
  local_wdrm = wdrm;
  return 0;
}  // end fctn

wdrm_object_t *  get_wdrm ()
{
  if ( local_wdrm == NULL )  local_wdrm = get_wdrm_default();
  return local_wdrm;
}  // end fctn




// EOF!
