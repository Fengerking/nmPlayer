
#include <stdlib.h> /* abs() */
#include <assert.h> /* pre-release check */

#include "typedef.h"
#include "basic_op.h"
#include "count.h"

#include "amr_plus_fx.h"

static void Re8_compute_base_index(Word16 x[], Word16 *ka, UWord16 *I);
static void Re8_compute_rank_of_permutation_and_sign_code(Word16  *x, Word16 *rank, Word16 *sign_code);

/*--------------------------------------------------------------------------
  RE8_cod(x, n, I, k)
  MULTI-RATE INDEXING OF A POINT y in THE LATTICE RE8 (INDEX COMPUTATION)

  (i) x: point in RE8 (8-dimensional integer vector)
  (i) n: codebook number (*n is an integer defined in {0,2,3,4,..,n_max})
  (o) I: index of c (pointer to unsigned 16-bit word)
  (o) k: index of v (8-dimensional vector of binary indices) = Voronoi index

  note: the index I is defined as a 32-bit word, but only
  16 bits are required (long can be replaced by unsigned integer)
  --------------------------------------------------------------------------
 */
void RE8_Cod(Word16 x[], Word16 *n, UWord16 *I, Word16 k[])
{
  Word16 ka, c[8];
  /* decompose x as x = 2^r c + v, where r is an integer >=0, c is an element
     of Q0, Q2, Q3 or Q4, and v is an element of a Voronoi code in RE8
     (if r=0, x=c) 
     this decomposition produces as a side-product the index k[] of v
     and the identifier ka of the absolute leader related to c

     the index of y is split into 2 parts :
     - the index I of c
     - the index k[] of v */

  RE8_Vor(x, n, k, c, &ka);

  /* compute the index I (only if c is in Q2, Q3 or Q4) */
  test();
  if (*n > 0)
  {                                       
    Re8_compute_base_index(c, &ka, I);
  }
  return;
}

/*--------------------------------------------------------------
  re8_compute_base_index(x, ka, I)
  COMPUTE THE INDEX I of A LATTICE POINT x in  RE8

  (i) x : point in RE8 (8-dimensional integer vector)
  (i) ka: identifier of the absolute leader of x (scalar integer)
  (o) I : index of x (unsigned 16-bit word)

  note: the index I is defined as a 32-bit word, but only
  16 bits are required (long can be replaced by unsigned integer)
  ---------------------------------------------------------------
 */
static void Re8_compute_base_index(Word16 x[], Word16 *ka, UWord16 *I)
{
  Word16 rank, offset, code, i, ks;
  /* - compute rank of permutation of x 
     (x is a codevector in a permutation code C,
      the code C is organized according to the lexicographic order,
      the maximum xs of C is called the signed leader (playing the role of
      a generator for C),
      the rank #include <assert.h>of the permutation is the index of x in C)

    - get also the sign code of xs as a side product
      (the sign code is obtained by concatenating the sign bits of xs) */

  Re8_compute_rank_of_permutation_and_sign_code(x, &rank, &code);

  /* compute cardinality offset in 2 steps:
     1. search for the sign code of xs in a pre-computed list of sign codes
        (the result ks is an identifier of the signed leader xs related to x,
	 the search is focused based on the identifier ka of the absolute
	 leader related to x)
     2. get the cardinality offset by table look-up
  */
  ks = -1;                            move16();/* initialization to use assert() */
  for (i=Ia_[*ka];i<NB_LDSIGN;i++)
  {
    test();
    if (sub(code,Ds_[i]) == 0)
    {
      ks = i;               move16();
      break;
    }
  }
  test();
  if (ks < 0)
  {
    AMRWBPLUS_FPRINTF(stderr, "\nre8_compute_base_index abnormal termination\n");
    exit(0);
  }

  offset = Is_[ks];      move16();

  /* compute index of x: index = cardinality offset + rank of permutation */
  *I = (UWord16)L_add(offset,rank); move16();
}

/*---------------------------------------------------------------------
  re8_compute_rank_and_sign_code(int *x, t, code)
  COMPUTE THE RANK OF THE PERMUTATION GIVEN x FROM THE SIGNED LEADER xs
  AND AS A SIDE-PRODUCT COMPUTE THE SIGN CODE OF xs

  (i) x:         point in RE8 (8-dimensional integer vector)
  (o) rank:      rank of the permutation x of xs (scalar integer)
  (o) sign_code: sign code of xs (8-bit word)

  note : the rank computation is based on Schalkwijk formula.
                 __
                \         (7-k)!
         rank =  |    ---------------   x (wk0 +...+ wk(d(k)-1))
                /__      ___
               k=0..7   |   |
                        |   |  (w_kî)!
                       i=0..q-1
   where k   =position in the vector
         q   =size of the alphabet of xs
         w_ki=number of occurences of the ith symbol from th kth position
	 d(k)=code translation a symbol of the alphabet into an index in
              the alphabet
  ---------------------------------------------------------------------
 */
static void Re8_compute_rank_of_permutation_and_sign_code(Word16  *x, Word16 *rank, Word16 *sign_code)
{
  Word16 xs[8], a[8], q, d[8], w[8], idx, tmp16, abs_i, abs_j, A, m_B16, exp_B, exp_tmp, m_tmp;
  Word16 i, j, k;
  Word32 B, Ltmp;
  /* sort the elements of x to obtain the signed leader xs of x */

  for (i=0; i<8; i++)
  {
    xs[i] = x[i];           move16();
  }
  for (k=0; k<7; k++)
  {
    j = k;                  move16();
    for (i=k+1; i<8; i++)
    {
      abs_j = abs_s(xs[j]);      
      abs_i = abs_s(xs[i]);      
                               
      test();
      if (sub(abs_i,abs_j) >= 0)
      {                   
        test();
        if (sub(abs_i,xs[j]) > 0)
        {
          j = i;      move16();
        }
      }
    }
    test();
    if (sub(j,k) > 0)
    {
      tmp16 = xs[k];        move16();
      xs[k] = xs[j];      move16();
      xs[j] = tmp16;        move16();
    }
  }

  /* compute the sign code of xs (this is a side-product of the rank
     calculation) */
  tmp16 = 0;         move16();

  for (i=0; i<8; i++)
  {
    test();
    if (xs[i]<0)
    {
      tmp16 = add(tmp16, Tab_pow2[i]); /* *sign_code += 1<<(7-i); */
    }
  }
  *sign_code = tmp16;       move16();

  /* compute the alphabet a=[a[0] ... a[q-1]] of x (q elements)
     such that a[0] != ... != a[q-1] and compute q */
  
  a[0] = xs[0];       move16();
  q = 1;              move16();
               
  for (i=1; i<8; i++)
  { 
    test();
    if (sub(xs[i],xs[i-1]) != 0)
    {
      a[q] = xs[i];         move16();
      q = add(q,1);         
    }
  }
  /* translate x into d (where 0<=d[i]<q and d[i]=j if x[i]=a[j])
     based on the alphabet a */ 
                   
  for (i=0; i<8; i++)
  {              
    for (j=0; j<q; j++)
    {
      test();
      if (sub(x[i],a[j]) == 0)
      {
        d[i] = j;           move16();
        break; /* end loop over j */
      }
    }
  }

  /* compute rank of permutation based on Schalkwijk's formula
     the rank is given by rank=sum_{k=0..7} (A_k * fac_k/B_k) */
  tmp16 = 0;      move16();
  for (j=0; j<q; j++)
  {
    w[j] = 0;       move16();
  }
  B = 2;            move32();
  for (i=7; i>=0; i--)
  {
    idx = d[i];                     move16();
    w[idx] = add(w[idx], 1);        move16();

    B = L_mult(extract_h(L_shl(B,15)), w[idx]); 
    A = 0;       move16();
          
    for (j=0; j<idx; j++)
    {
      A = add(A, w[j]); 
    }
    test();      
    if (A>0)
    {                                   
      /* A_k * fac_k/B_k */
      /**rank += A * tab_factorial[i]/B; */
      Ltmp = L_mult(A, Tab_factorial[i]);
      exp_tmp = sub(norm_l(Ltmp),1);          /* ensure num < den */
      m_tmp = extract_h(L_shl(Ltmp, exp_tmp));
      exp_tmp = sub(30,exp_tmp);

      exp_B = norm_l(B);
      m_B16 = extract_h(L_shl(B, exp_B));
      exp_B = sub(30, exp_B);
      m_B16 = div_s(m_tmp , m_B16);
      exp_B = sub(exp_tmp , exp_B);

      tmp16 = add(tmp16, shl(m_B16, sub(exp_B,15)));      /* Q15 -> Q0 */
    }
  }
  *rank = tmp16;      move16();

}

