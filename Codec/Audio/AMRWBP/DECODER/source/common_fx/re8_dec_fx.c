#include <assert.h>

#include "amr_plus_fx.h"

#include "typedef.h"
#include "count.h"
#include "basic_op.h"


static void Re8_decode_base_index(Word16 *n, UWord16 *I, Word16 *y);
static void Re8_decode_rank_of_permutation(Word16 rank, Word16 *xs, Word16 *x);

/*--------------------------------------------------------------------------
  RE8_dec(n, I, k, y)
  MULTI-RATE INDEXING OF A POINT y in THE LATTICE RE8 (INDEX DECODING)

  (i) n: codebook number (*n is an integer defined in {0,2,3,4,..,n_max})
  (i) I: index of c (pointer to unsigned 16-bit word)
  (i) k: index of v (8-dimensional vector of binary indices) = Voronoi index
  (o) y: point in RE8 (8-dimensional integer vector)

  note: the index I is defined as a 32-bit word, but only
  16 bits are required (long can be replaced by unsigned integer)
  --------------------------------------------------------------------------
 */
void RE8_Dec(Word16 n, UWord16 I, Word16 k[], Word16 y[])
{
#if (FUNC_RE8_DEC_OPT)

  Word16 i, m, v[8];

  /* decode the sub-indices I and kv[] according to the codebook number n:
     if n=0,2,3,4, decode I (no Voronoi extension)
     if n>4, Voronoi extension is used, decode I and kv[] */
  if (n <= 4)
  {
    Re8_decode_base_index(&n, &I, y);     
  }
  else
  {
    /* compute the Voronoi modulo m = 2^r where r is extension order */
    m = 0;
                                            
    while (n > 4)
    {                                     
      m++;
      n -= 2;
    }

    /* decode base codebook index I into c (c is an element of Q3 or Q4)
       [here c is stored in y to save memory] */
    Re8_decode_base_index(&n, &I, y);     
    

    /* decode Voronoi index k[] into v */
    Re8_k2y(k, m, v);                       
    /* reconstruct y as y = m c + v (with m=2^r, r integer >=1) */
                                            
    for (i=0;i<8;i++)
    {
      /*y[i] = m*y[i] + v[i];*/
      y[i] = (y[i] << m) + v[i];
    }
  }
  
  return;
  
#else

  Word16 i, m, v[8];

                                            
  /* decode the sub-indices I and kv[] according to the codebook number n:
     if n=0,2,3,4, decode I (no Voronoi extension)
     if n>4, Voronoi extension is used, decode I and kv[] */
  
  if (sub(n,4) <= 0)
  {
    Re8_decode_base_index(&n, &I, y);     
  }
  else
  {
    /* compute the Voronoi modulo m = 2^r where r is extension order */
    m = 0;                                          
                                            
    while (sub(n,4) > 0)
    {                                     
      m = add(m,1);                             
      n = sub(n,2);                             
    }

    /* decode base codebook index I into c (c is an element of Q3 or Q4)
       [here c is stored in y to save memory] */
    Re8_decode_base_index(&n, &I, y);     
    

    /* decode Voronoi index k[] into v */
    Re8_k2y(k, m, v);                       
    /* reconstruct y as y = m c + v (with m=2^r, r integer >=1) */
                                            
    for (i=0;i<8;i++)
    {
      /*y[i] = m*y[i] + v[i];*/
      y[i] = add(shl(y[i],m),v[i]);         
    }
  }
                                      
  return;
#endif
}


/*--------------------------------------------------------------------------
  re8_decode_base_index(n, I, y)
  DECODING OF AN INDEX IN Qn (n=0,2,3 or 4)

  (i) n: codebook number (*n is an integer defined in {0,2,3,4})
  (i) I: index of c (pointer to unsigned 16-bit word)
  (o) y: point in RE8 (8-dimensional integer vector)

  note: the index I is defined as a 32-bit word, but only
  16 bits are required (long can be replaced by unsigned integer)
  --------------------------------------------------------------------------
 */
static void Re8_decode_base_index(Word16 *n, UWord16 *I, Word16 *y)
{
  Word16 i,im,t,sign_code,ka,ks, rank, leader[8];
  UWord16 offset;

  ka = 0;    
  ks = 0;    


                                     
  if ((sub(*n,4)==0) && (L_sub(*I,65519)>0))
  {
    *I=0;          /* force the index to be valid*/
    
  }
  
  if (sub(*n,2) < 0)
  {                                       
    for (i=0;i<8;i++)
    {
      y[i]=0;                             
    }
  }
  else
  {
    /* search for the identifier ka of the absolute leader (table-lookup)
       Q2 is a subset of Q3 - the two cases are considered in the same branch
     */
                                            
    /* switch <=> if (n==4)... else ... end */
    
    switch (*n)
    {
      case 2:
      case 3:
                                            
        for (i=1;i<NB_LDQ3;i++)
        {                                 
          if (L_sub(*I,I3_[i]) < 0)
          break;
        }
        ka = A3_[i-1];                 
      break;
      case 4:
                                    
        for (i=1;i<NB_LDQ4;i++)
        {                                 
          if (L_sub(*I,I4_[i]) < 0)
          break;
        }
        ka = A4_[i-1];                   
      break;
    }

    /* reconstruct the absolute leader */
                                            
    /* Da[8*ka+i] -> pointer to Da[ka<<3] =>  */
    
    for (i=0;i<8;i++)
    {
      leader[i] = Da_[ka][i];                      
    }

    /* search for the identifier ks of the signed leader (table look-up)
       (this search is focused based on the identifier ka of the absolute
        leader)*/

    t=Ia_[ka];                               
    im=sub(Ns_[ka],1);                              
                      
    for (i=im; i>=0; i--)
    {                                     
      if (L_sub(*I,Is_[add(t,i)]) >=0)
      {
        ks=i;                           
        break;
      }
    }
    /* reconstruct the signed leader from its sign code */
    sign_code = Ds_[add(t,ks)];                           
                                    
    for (i=0;i<8;i++)
    {
                                            
      if (sub(sign_code,Tab_pow2[i]) >= 0)
      {
        sign_code = sub(sign_code,Tab_pow2[i]);         
        leader[i] = negate(leader[i]);           
      }
    }

    /* compute the cardinality offset */
    offset = Is_[add(t,ks)];                  

    /* compute and decode the rank of the permutation */
    rank = extract_l(L_sub(*I, offset));                         

    Re8_decode_rank_of_permutation(rank, leader, y);
  }
                                      
  return;
}


/*--------------------------------------------------------------------------
  re8_decode_rank_of_permutation(rank, xs, x)
  DECODING OF THE RANK OF THE PERMUTATION OF xs

  (i) rank: index (rank) of a permutation
  (i) xs:   signed leader in RE8 (8-dimensional integer vector)
  (o) x:    point in RE8 (8-dimensional integer vector)

  note: the definition of the signed leader is different from Adoul and
        Lamblin's definition to save complexity
  --------------------------------------------------------------------------
 */
static void Re8_decode_rank_of_permutation(Word16 rank, Word16 *xs, Word16 *x)
{
  Word16 a[8], q, w[8], fac, *ptr_w, *ptr_a;
  Word16 div_table[] = {32767,16384,10923,8192, 6556, 5461, 4681, 4096};
  const Word16 *ptr_factorial;
  Word16 expA, expB, mantA, mantB;
  Word16 i, j, tmp16;
  Word32 B, A;
  Word32 target;
// @shanrong added
#if (FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT)
  Word32 temp_temp_value;
#endif
// end

                                      

  /* --- pre-processing based on the signed leader xs ---
     - compute the alphabet a=[a[0] ... a[q-1]] of x (q elements)
       such that a[0]!=...!=a[q-1]
       it is assumed that xs is sorted in the form of a signed leader
       which can be summarized in 2 requirements:
          a) |xs[0]| >= |xs[1]| >= |xs[2]| >= ... >= |xs[7]|
          b) if |xs[i]|=|xs[i-1]|, xs[i]>=xs[i+1]
       where |.| indicates the absolute value operator
     - compute q (the number of symbols in the alphabet)
     - compute w[0..q-1] where w[j] counts the number of occurences of
       the symbol a[j] in xs
     - compute B = prod_j=0..q-1 (w[j]!) where .! is the factorial */
    /* xs[i], xs[i-1] and ptr_w/a*/
  ptr_w = w;        
  ptr_a = a;        
  *ptr_w = 1;                                 
  *ptr_a = xs[0];                           
  q = 1;                                    
  tmp16 = 1;                                
                                            
  for (i=1; i<7; i++)
  {
// @shanrong modified
#if (!FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT)
                                          
    if (sub(xs[i],xs[i-1]) != 0)
    {
      ptr_w++;                          
      ptr_a++;                          
      *ptr_w = 0;                       
      *ptr_a = xs[i];                   
      q = add(q,1);                              
    }
    *ptr_w = add(*ptr_w,1);                           
    tmp16 = extract_l(L_shr(L_mult(tmp16, *ptr_w),1));                          
#else
    if (xs[i] != xs[i-1])
    {
      ptr_w++;                          
      ptr_a++;                          
      *ptr_w = 0;                       
      *ptr_a = xs[i];                   
      q += 1;
    }
    *ptr_w += 1;
    tmp16 = (Word16)((((Word32)tmp16 * (*ptr_w)) << 1) >> 1);
#endif
// end
  }
  
// @shanrong modified
#if (!FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT)
  
  if (sub(xs[i],xs[i-1]) != 0)
  {
    ptr_w++;                          
    ptr_a++;                          
    *ptr_w = 0;                       
    *ptr_a = xs[i];                   
    q = add(q,1);                              
  }
  *ptr_w = add(*ptr_w,1);                           
  B = L_mult(tmp16, *ptr_w);                          
  B = L_shr(B, 1);
#else
  
  if (xs[i] != xs[i-1])
  {
    ptr_w++;                          
    ptr_a++;                          
    *ptr_w = 0;                       
    *ptr_a = xs[i];                   
    q += 1;
  }
  *ptr_w = add(*ptr_w,1);                           
  B = (tmp16 * (*ptr_w)) << 1;
  B >>= 1;
#endif
// end
  /* --- actual rank decoding ---
     the rank of x (where x is a permutation of xs) is based on
     Schalkwijk's formula
     it is given by rank=sum_{k=0..7} (A_k * fac_k/B_k)
     the decoding of this rank is sequential and reconstructs x[0..7]
     element by element from x[0] to x[7]
     [the tricky part is the inference of A_k for each k...]
   */

  /* decode x element by element */
                                            
  ptr_factorial = Tab_factorial;
                                            
  for (i=0; i<8; i++)
  {
    /* infere A (A_k): search j such that x[i] = a[j]
    A = sum_{i=0...j-1} w[j] with 0<=j<q and sum_{i=0..-1} . = 0
    j can be found by accumulating w[j] until the sum is superior to A
       [note: no division in search for j, but 32-bit arithmetic required] */
    fac = *ptr_factorial;                             
    /*target = -(UWord16)rank*(UWord16)B;         */
// @shanrong modified
#if (!FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT)
    
    if (L_sub(B,MAX_16)>0)
    {
      target = L_shl(L_mult(negate(rank), extract_l(L_shr(B,1))),1);
    }
    else
    {
      target = L_mult(negate(rank), extract_l(B));
    }
#else
    if (B > MAX_16)
    {
      //target = L_shl(L_mult(negate(rank), extract_l(L_shr(B,1))),1);
      target = (((Word16)(B >> 1) * (-rank)) << 1) >> 1;
    }
    else
    {
      //target = L_mult(negate(rank), extract_l(B));
      target = ((Word16)B * (-rank) << 1);
    }
#endif
// end

    A = 0;   move32();                              
    j=0;                                  
    do
    {
// @shanrong modified
#if (!FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT)
      /*target += w[j]*fac;*/
      target = L_mac(target, w[j], fac);
      /*A += w[j]*fac;                    */
      A = L_mac(A, w[j], fac);
      j++;     /*pointer*/           
      
#else
      temp_temp_value = (w[j] * fac) << 1;
      target += temp_temp_value;
      A += temp_temp_value;
      j++;     /*pointer*/           
#endif
// end
    }
    while (target<=0);
    
    j--; /* pointer w and a */                                 

    assert(j<q);

    /*A -= w[j]*fac;*/
// @shanrong modified
#if (!FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT)
    A = L_msu(A, w[j], fac);
#else
    A -= (w[j] * fac) << 1;
#endif
// end
    
    x[i] = a[j];                          

    /* update rank, denominator B (B_k) and counter w[j] */
                                           
    if (A>0)
    {
// @shanrong modified
#if (!FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT)
      /*rank -= A/B;*/
      expA = sub(norm_l(A),1);  /* ensure A<B */
      mantA = extract_h(L_shl(A,expA));
      expB = norm_l(B);
      mantB = extract_h(L_shl(B,expB));

      expA = add(15+1,sub(expA,expB)); /* A /= 2 and Ouput Q0 */
      mantA = div_s(mantA, mantB);
      rank = sub(rank, shr(mantA,expA));
#else
      expA = norm_l(A) - 1;
      mantA = (A << expA) >> 16;
      expB = norm_l(B);
      mantB = (B << expB) >> 16;

      //expA = add(15+1,sub(expA,expB)); /* A /= 2 and Ouput Q0 */
      expA = 16 + expA - expB;
      mantA = div_s(mantA, mantB);
      rank -= mantA >> expA;
#endif
// end
    }
    
                                        
    if (sub(w[j],1)>0)
    {
// @shanrong modified
#if (!FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT)
      /*B = B/w[j];*/
      
      if (L_sub(B,MAX_16) > 0)
      {
        tmp16 = extract_l(L_shr(B,1));
        B = round16(L_shl(L_mult(tmp16,div_table[sub(w[j],1)]),1));   
      }
      else
      {
        tmp16 = extract_l(B);
        B = round16(L_mult(tmp16,div_table[sub(w[j],1)]));   
      }
#else
      if (B > MAX_16)
      {
        tmp16 = (Word16)(B >> 1);
        //B = round16(L_shl(L_mult(tmp16,div_table[sub(w[j],1)]),1));   
        B = ((((tmp16 * div_table[w[j] - 1]) << 1) >> 1) + 0x00008000) >> 16;
      }
      else
      {
        tmp16 = (Word16)B;
        //B = round16(L_mult(tmp16,div_table[sub(w[j],1)]));   
        B = ((tmp16 * div_table[w[j] - 1] << 1) + 0x00008000) >> 16;
        //round16(L_mult(tmp16,div_table[sub(w[j],1)]));   
      }
#endif
// end      
    }
    
// @shanrong modified
#if (!FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT)
    w[j] = sub(w[j],1);                               
#else
    w[j] -= 1;
#endif
// end

    ptr_factorial++;                      
  }
                                  
}


