
/* md5hl.c
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@login.dkuug.dk> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *
 */

#include "md5.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


char* MD5End(md5_state_t *ctx, char *p)
{
    int i;
    unsigned char digest[16]; 
    static const char hex[]="0123456789abcdef";

    md5_finish(digest,ctx);
    for(i=0;i<16;i++) {
	p[i+i] = hex[digest[i] >> 4];
	p[i+i+1] = hex[digest[i] & 0x0f];
    }
    p[i+i] = '\0';
    return p;
}

char* MD5Data(char *buf, char *data, unsigned int len)
{
	unsigned char *dataEx = (unsigned char *)data;
    md5_state_t ctx;
    md5_init(&ctx);
    md5_append(&ctx,(md5_byte_t *)dataEx,len);
    return MD5End(&ctx, buf);
}
#ifdef _VONAMESPACE
}
#endif