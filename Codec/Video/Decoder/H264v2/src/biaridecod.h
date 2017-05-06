

#ifndef _BIARIDECOD_H_
#define _BIARIDECOD_H_


#define VOHALF      0x01FE  

static inline void vo_start_cabac(VOCABACContext *c, const VO_U8 *buf, int buf_size){
    c->stream_start=
    c->stream= buf;
    c->stream_end= buf + buf_size;

    c->low_bits=  (*c->stream++)<<18;
    c->low_bits+=  (*c->stream++)<<10;
    c->low_bits+= ((*c->stream++)<<2) + 2;
    c->range= VOHALF;
}


#endif  // BIARIDECOD_H_

