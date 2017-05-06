
#include "voutf8_tools.h"

int vowcslen(const TCHAR* str) {   
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	return len;
} 

int VO_UCS2ToUTF8(const TCHAR *szSrc, const int nSrcLen, char *strDst, const int nDstLen) {

	int is = 0, id = 0;
	const TCHAR *ps = szSrc;
	unsigned char *pd = (unsigned char*)strDst;

	if (nDstLen <= 0)
		return 0;

	for (is = 0; is < nSrcLen; is++) {
		if (BYTE_1_REP > ps[is]) { /* 1 byte utf8 representation */
			if (id + 1 < nDstLen) {
				pd[id++] = (unsigned char)ps[is];
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_2_REP > ps[is]) {
			if (id + 2 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 6 | 0xc0);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_3_REP > ps[is]) { /* 3 byte utf8 representation */
			if (id + 3 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 12 | 0xe0);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_4_REP > ps[is]) { /* 4 byte utf8 representation */
			if (id + 4 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 18 | 0xf0);
				pd[id++] = (unsigned char)(((ps[is] >> 12)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} /* @todo Handle surrogate pairs */
	}

	pd[id] = '\0'; /* Terminate string */

	return id; /* This value is in bytes */
}


int VO_UTF8ToUCS2(const char *szSrc, const int nSrcLen, TCHAR *strDst, const int nDstLen) {

	int is = 0, id = 0;
	int itemsize = sizeof(TCHAR);

	if (nDstLen <= 0) 
		return 0;

	const unsigned char *ps = (const unsigned char *)szSrc;
	TCHAR *pd = strDst;

	while (is < nSrcLen) {
		if (SIGMASK_1_1 > ps[is]) {

			if (is < nSrcLen && id + itemsize < nDstLen) {
				pd[id++] = (TCHAR)ps[is] & 0x7f;
				++is;
			} else {
				pd[id] = 0; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}

		} else if (SIGMASK_2_1 > ps[is]) {

			if (is + 1 < nSrcLen && id + itemsize < nDstLen) {
				pd[id++] = (TCHAR)(((TCHAR)ps[is] & 0x1f) << 6 | ((TCHAR)ps[is+1] & 0x3f));
				is += 2;
			} else {
				pd[id] = 0; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}

		} else if (SIGMASK_3_1 > ps[is]) {

			if (is + 2 < nSrcLen && id + itemsize < nDstLen) {
				pd[id++] = (TCHAR)((((TCHAR)ps[is] & 0x0f) << 12) | (((TCHAR)ps[is+1] & 0x3f) << 6) | ((TCHAR)ps[is+2] & 0x3f));
				is += 3;
			} else {
				pd[id] = 0; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (SIGMASK_4_1 > ps[is]) {

			if (is + 3 < nSrcLen && id + itemsize < nDstLen) {
				pd[id++] = (TCHAR)((((TCHAR)ps[is] & 0x07) << 18) 
									| (((TCHAR)ps[is+1] & 0x3f) << 12) 
									| (((TCHAR)ps[is+2] & 0x3f) << 6) 
									| ((TCHAR)ps[is+3] & 0x3f));
				is += 4;
			} else {
				pd[id] = 0; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		}
	}

	pd[id] = 0; /* Terminate string */
	return id;
}

