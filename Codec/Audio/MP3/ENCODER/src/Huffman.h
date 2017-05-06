#ifndef ___HUFFMAN_H____
#define ___HUFFMAN_H____

#include "config.h"
#include "Bits.h"

struct huffcodetab {
  int          len;				/* max. index                   */
  const unsigned char *table;   /* pointer to array[len][len]   */
  const unsigned char *hlen;   /* pointer to array[len][len]   */
};

struct huffcodebig {
  int          len;				 /* max. index                   */
  int          linbits;			 /* number of linbits            */
  int          linmax;			 /* max number stored in linbits */
};

struct Huffbigdv
{
  unsigned int region0_cnt;
  unsigned int region1_cnt;
};

int  HuffmanCode(Bitstream *stream, short *ix, int *xr, unsigned int begin, unsigned int end, int table);
int  HuffmanCod1(Bitstream *stream, short *ix, int *xr, unsigned int begin, unsigned int end, int table);
int  find_best_2( short *ix, unsigned int start, unsigned int end, const unsigned int *table,
                  unsigned int len, int *bits);
int  find_best_3( short *ix, unsigned int start, unsigned int end, const unsigned int *table,
                  unsigned int len, int *bits);
int  count_bit1 ( short *ix, unsigned int start, unsigned int end, int *bits );
int  count_bigv ( short *ix, unsigned int start, unsigned int end, int table0, int table1,
                  int *bits);

extern const unsigned char     ht_count[2][2][16];
extern const unsigned int	   tab01[ 16];     
extern const unsigned int      tab23[  9];       
extern const unsigned int      tab56[ 16];          
extern const unsigned int      tab1315[256];          
extern const unsigned int      tab1624[256];           
extern const unsigned int      tab789[ 36];            
extern const unsigned int      tabABC[ 64];        
extern const unsigned char     t1HB[  4];            
extern const unsigned char     t2HB[  9];       
extern const unsigned char     t3HB[  9];        
extern const unsigned char     t5HB[ 16];             
extern const unsigned char     t6HB[ 16];          
extern const unsigned char     t7HB[ 36];          
extern const unsigned char     t8HB[ 36];              
extern const unsigned char     t9HB[ 36];            
extern const unsigned char     t10HB[ 64];              
extern const unsigned char     t11HB[ 64];              
extern const unsigned char     t12HB[ 64];            
extern const unsigned char     t13HB[256];             
extern const unsigned char     t15HB[256];           
extern const unsigned short    t16HB[256];          
extern const unsigned short    t24HB[256];            
extern const unsigned char     t1l[  8];          
extern const unsigned char     t2l[  9];          
extern const unsigned char     t3l[  9];            
extern const unsigned char     t5l[ 16];             
extern const unsigned char     t6l[ 16];             
extern const unsigned char     t7l[ 36];            
extern const unsigned char     t8l[ 36];           
extern const unsigned char     t9l[ 36];         
extern const unsigned char     t10l[ 64];          
extern const unsigned char     t11l[ 64];            
extern const unsigned char     t12l[ 64];          
extern const unsigned char     t13l[256];           
extern const unsigned char     t15l[256];           
extern const unsigned char     t16l[256];            
extern const unsigned char     t24l[256];            
extern const struct huffcodetab ht[HTN];
extern const struct huffcodebig ht_big[HTN]; 
extern const struct Huffbigdv	subdv_table[23];       

#endif  