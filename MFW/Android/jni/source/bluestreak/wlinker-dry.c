
#include <wlinker.h>
#include <dlfcn.h>


//int  set_wdrm (wdrm_object_t *  wdrm);

void *  wlinker_wdlopen (struct wlinker_s *  thiz, const char *  libname, int  mode)
{
  void *  handle = dlopen(libname,RTLD_NOW|RTLD_GLOBAL);
  if ( handle == NULL )  return NULL;
  void *  sym = NULL;
  sym = dlsym(handle,"set_wlinker");
  if ( sym != NULL )
  {
    int  (*f) (wlinker_object_t *  wlinker) = sym;
    f(thiz);
  }
  sym = dlsym(handle,"set_wdrm");
  if ( sym != NULL )
  {
    int  (*f) (wdrm_object_t *  wdrm) = sym;
    f(thiz->_wdrm);
  }
  return handle;
}  // end fctn



void *  wlinker_wdlsym (struct wlinker_s *  thiz, void *  handle, const char *  symbol)
{
  return dlsym(handle,symbol);
}  // end fctn


static wlinker_object_t *  default_wlinker = NULL;

static wlinker_object_t *  get_wlinker_default ()
{
  if ( default_wlinker == NULL )  default_wlinker = (wlinker_object_t *) malloc(sizeof(wlinker_object_t));
  if ( default_wlinker == NULL )  return NULL;
  default_wlinker->_this = default_wlinker;
  default_wlinker->wdlopen = & wlinker_wdlopen ;
  default_wlinker->wdlsym = & wlinker_wdlsym ;
  default_wlinker->_wdrm = get_wdrm();
  return default_wlinker;
} // end fctn




static wlinker_object_t *  local_wlinker = NULL;

int  set_wlinker (wlinker_object_t *  wlinker)
{
  if ( wlinker == NULL )  return -__LINE__;
  local_wlinker = wlinker;
  return 0;
}  // end fctn


wlinker_object_t *  get_wlinker ()
{
  if ( local_wlinker == NULL )  local_wlinker = get_wlinker_default();
  return local_wlinker;
}  // end fctn

// EOF!
