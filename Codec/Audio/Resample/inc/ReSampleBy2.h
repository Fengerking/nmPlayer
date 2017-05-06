
#ifndef __RESAMPLEBY2_H__
#define __RESAMPLEBY2_H__


/*
* Down Sample by 2
* Out length = len >> 1
*/
void voDownsampleBy2(const short* in,     /* In : input buffer */
		     const short len,     /* In : input buffer length */
		     short* out,          /* Out: output buffer */
		     int* filtState     /* I/O: filter state register */
		    );

/*
* Up Sample by 2
* Out length = len << 1
*/
void voUpsampleBy2(const short* in,       /* In : input buffer */
		   short len,             /* In : input buffer length */
		   short* out,            /* Out: output buffer */
		   int* filtState       /* I/O: filter state register */
					   );


#endif //__LPRESAMPLEBY2_H__

