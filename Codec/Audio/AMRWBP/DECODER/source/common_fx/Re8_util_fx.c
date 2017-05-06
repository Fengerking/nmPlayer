
//#include <stdio.h>

#include "typedef.h"
#include "count.h"
#include "basic_op.h"


#include "amr_plus_fx.h"

static Word16 Re8_identify_absolute_leader(Word16 y[]);
static void Re8_coord(Word16 *y, Word16 *k);

/*--------------------------------------------------------------
  RE8_vor(y,n,k,c,ka)
  MULTI-RATE RE8 INDEXING BY VORONOI EXTENSION

  (i) y : point in RE8 (8-dimensional integer vector)
  (o) n : codebook number n=0,2,3,4,... (scalar integer)
  (o) k : Voronoi index (integer vector of dimension 8)
          used only if n>4
  (o) c : codevector in Q0, Q2, Q3, or Q4
          if n<=4, y=c
  (o) ka: identifier of absolute leader (needed to index c)
  ---------------------------------------------------------------
 */
void RE8_Vor(Word16 y[], Word16 *n, Word16 k[], Word16 c[], Word16 *ka)
{
  Word16 i, r, m, iter, ka_tmp, n_tmp, mask;
  Word16 k_tmp[8], v[8], c_tmp[8], k_mod[8];
  Word32 Ltmp, Lsphere;


  /* verify if y is in Q0, Q2, Q3 or Q4
     (a fast search is used here:
      the codebooks Q0, Q2, Q3 or Q4 are specified in terms of RE8 absolute leaders
      (see for instance Xie and Adoul's paper in ICASSP 96)
      - a unique code identifying the absolute leader related to y is computed
        in re8_identify_absolute_leader()
        this code is searched for in a pre-defined list which specifies Q0, Q2, Q3 or Q4)
        the absolute leader is identified by ka
      - a translation table maps ka to the codebook number n) */
  *ka = Re8_identify_absolute_leader(y);          

  /* compute codebook number n of Qn (by table look-up)
     at this stage, n=0,2,3,4 or out=100 */
  *n = Da_nq_[*ka];                               
 
  /* decompose y into :
       (if n<=4:)
       y = c        where c is in Q0, Q2, Q3 or Q4 
     or
       (if n>4:)
       y = m c + v  where c is in Q3 or Q4, v is a Voronoi codevector
                          m=2^r (r integer >=2) 

     in the latter case (if n>4), as a side-product, compute the (Voronoi) index k[] of v
     and replace n by n = n' + 2r where n' = 3 or 4 (c is in Qn') and r is defined above
  */
  
  if (sub(*n,4) <= 0)
  {

    for (i=0;i<8;i++)
    {
      c[i] = y[i];                            
    }
  }
  else
  {
    /* initialize r and m=2^r based on || y ||^2/8 */
    Ltmp  = 0;         move32();                  

    for (i=0;i<8;i++)
    {
      Ltmp = L_mac(Ltmp, y[i], y[i]);  
    }
    Lsphere = L_shr(Ltmp,5+1);     /* *0.125*0.25  / 2 to remove L_mac effect*/

    r = 1;                                      
    
    while (L_sub(Lsphere,11) > 0)
    {                       
      
      r = add(r,1);                                
      Lsphere = L_shr(Lsphere, 2);        /* *= 0.25 */             
    }
    /* compute the coordinates of y in the RE8 basis */
    Re8_coord(y, k_mod);                    


    /* compute m and the mask needed for modulo m (for Voronoi coding) */
    m = r;                               
    mask = sub(shl(1,r),1); /* 0x0..011...1 */          
    /* find the minimal value of r (or equivalently of m) in 2 iterations */

    for (iter=0; iter<2; iter++)     
    {
      /* compute v such that y is in m RE_8 +v (by Voronoi coding) */

      for (i=0;i<8;i++)
      {
        k_tmp[i] = k_mod[i] & mask;     logic16(); 
      }

      Re8_k2y(k_tmp, m, v);                 

      /* compute c = (y-v)/m
      (y is in RE8, c is also in RE8 by definition of v) */

      for (i=0;i<8;i++)
      {
        c_tmp[i] = shr(sub(y[i],v[i]), m);             
      }

      /*  verify if c_tmp is in Q2, Q3 or Q4 */

      ka_tmp = Re8_identify_absolute_leader(c_tmp);

      n_tmp = Da_nq_[ka_tmp];       /* at this stage, n_tmp=2,3,4 or out = 100 -- n=0 is not possible */

        
      if (sub(n_tmp,4) > 0)
      {
        /* if c is not in Q2, Q3, or Q4 (i.e. n_tmp>4), use m = 2^(r+1) instead of 2^r */
        r = add(r,1);                             
        m = add(m,1);                        
        mask = add(shl(mask,1),1);            /* mask = m-1; <- this is less complex */
      }
      else
      {
        /* c is in Q2, Q3, or Q4 -> the decomposition of y as y = m c + v is valid

        since Q2 is a subset of Q3, indicate n=3 instead of n=2 (this is because
        for n>4, n=n'+2r with n'=3 or 4, so n'=2 is not valid) */
        
        if (sub(n_tmp,3) < 0)
        {
          n_tmp = 3;                   
        }

        /* save current values into ka, n, k and c */
        *ka = ka_tmp;                         
        *n = add(n_tmp, shl(r,1));                

        for (i=0; i<8; i++)
        {
          k[i] = k_tmp[i];             
        }

        for (i=0; i<8; i++)
        {
          c[i] = c_tmp[i];             
        }

        /* try  m = 2^(r-1) instead of 2^r to be sure that m is minimal */
        r = sub(r, 1);                             
        m = sub(m, 1);                        
        mask = shr(mask, 1);                  
      }
    }
  }

  return;
}


/*-------------------------------------------------------------------------
  re8_identify_absolute_leader(y)
  IDENTIFY THE ABSOLUTE LEADER RELATED TO y USING A PRE-DEFINED TABLE WHICH
  SPECIFIES THE CODEBOOKS Q0, Q2, Q3 and Q4

  (i) y : point in RE8 (8-dimensional integer vector)
  (o) returned value = integer indicating if y if in Q0, Q2, Q3 or Q4 (or
      if y is an outlier)
  ---------------------------------------------------------------
 */
static Word16 Re8_identify_absolute_leader(Word16 y[])
{
  Word16 i,s,id,nb,pos,ka, tmp16;
  Word32 Ltmp, C[8], Ls;


  /* compute the RE8 shell number s = (y1^2+...+y8^2)/8 and C=(y1^2, ..., y8^2) */
  Ls = 0;                                  move32();      
  for (i=0;i<8;i++)
  {
    C[i] = L_mult(y[i], y[i]);                     move32();
    Ls = L_mac(Ls, y[i], y[i]);                            
  }
  s = extract_h(L_shl(Ls,16-(3+1)));        /* s can saturate here */                           

  /* compute the index 0<= ka <= NB_LEADER+1 which identifies an absolute leader of Q0, Q2, Q3 or Q4 */

  ka = NB_LEADER+1;     /* by default, ka=index of last element of the table (to indicate an outlier) */
  
  if (s == 0)
  {
    /* if s=0, y=0 i.e. y is in Q0 -> ka=index of element indicating Q0 */
    ka = NB_LEADER;                       
  }
  else
  {
    /* the maximal value of s for y in  Q0, Q2, Q3 or Q4 is NB_SPHERE
    if s> NB_SPHERE, y is an outlier (the value of ka is set correctly) */
    
    if (sub(s,NB_SPHERE) <= 0)
    {
      /* compute the unique identifier id of the absolute leader related to y:
      s = (y1^4 + ... + y8^4)/8 */
      Ltmp = 0;          move32();                  
      for (i=0;i<8;i++)
      {
        tmp16 = extract_h(L_shl(C[i], 16-1));
        Ltmp = L_mac(Ltmp, tmp16, tmp16);             
      }
      id = extract_h(L_shl(Ltmp ,16-(3+1)));            /* i can saturate to 8192 */ 

      /* search for id in table Da_id
      (containing all possible values of id if y is in Q2, Q3 or Q4)
      this search is focused based on the shell number s so that
      only the id's related to the shell of number s are checked */

      nb = Da_nb_[sub(s,1)]; /* get the number of absolute leaders used on the shell of number s */

      pos = Da_pos_[sub(s,1)]; /* get the position of the first absolute leader of shell s in Da_id */

      for (i=0; i<nb; i++)
      {
        
        if (sub(id,Da_id_[pos]) == 0)
        {
          ka = pos;       /* get ka */    
          break;
        }
        pos++;   /* ptr */
      }
    }
  }

  return(ka);
}



/* re8_coord(y,k)
   COMPUTATION OF RE8 COORDINATES

   (i) y: 8-dimensional point y[0..7] in RE8
   (o) k: coordinates k[0..7]
 */
static void Re8_coord(Word16 *y, Word16 *k)
{
  Word16 i, tmp, sum;

                                      
  /* compute k = y M^-1 
     M = 1/4 [ 1          ]
             [-1  2       ]
             [ |    \     ]
             [-1       2  ]
             [ 5 -2 _ -2 4]
  */
  k[7]=y[7];                                
  tmp = y[7];                                                    
  
  sum = extract_l(L_mac(y[7], y[7], 2));                             

  for (i=6; i>=1; i--)
  {
    /* apply factor 2/4 from M^-1 */
    k[i] = shr(sub(y[i],tmp),1);                 
    sum  = sub(sum, y[i]);                          
  }
  /* apply factor 1/4 from M^-1 */
  k[0]= shr(add(y[0], sum),2);                       

                                        
}


/* re8_y2k(y,m,k)
   VORONOI INDEXING (INDEX DECODING) k -> y

   (i) k: Voronoi index k[0..7]
   (i) m: Voronoi modulo (m = 2^r = 1<<r, where r is integer >=2)
   (o) y: 8-dimensional point y[0..7] in RE8
 */
void Re8_k2y(
  Word16 *k,
  Word16 m,
  Word16 *y
)
{
  Word16 i, v[8], tmp, sum, *ptr1, *ptr2, m_tmp, mm;
  Word32 z[8], Ltmp;
  UWord16 ytp[8];
                                            
  /* compute y = k M and z=(y-a)/m, where
     M = [4        ]
         [2 2      ]
         [|   \    ]
         [2     2  ]
         [1 1 _ 1 1]
     a=(2,0,...,0)
  */
  m_tmp = sub(15, m);

  for (i=0; i<8; i++)
  {
    ytp[i] = (UWord16)k[7];                          
  }
  z[7] = L_shl(ytp[7], m_tmp); move32();/*(int)(floor(y[7]*QR+0.5))>>m;*/                     
  
  sum=0;                                    
                                            
  for (i=6; i>=1; i--)
  {
    tmp   = shl(k[i], 1);                       
    sum   = add(tmp,sum);                          
    ytp[i]  = (UWord16)L_add(ytp[i], tmp);                          
    
    z[i] = L_shl(ytp[i], m_tmp); move32();/*(int)(floor((y[i]*QR+0.5)))>>m;*/                 
  }

  ytp[0] = (UWord16)L_add(ytp[0] , L_add(sum, shl(k[0],2)));   

  z[0] = L_shl(L_sub(ytp[0], 2), m_tmp);  move32(); /*(int)(floor((y[0]-2)*QR+0.5))>>m;*/                 

  /* find nearest neighbor v of z in infinite RE8 */
  
  RE8_PPV_FX(z, v);
  /* compute y -= m v */
                                            
  ptr1=y;       
  ptr2=v;       
  mm = shr(shl(1,m),1);  /* shr to remove effect of L_mult in L_msu */

  for (i=0; i<8; i++)
  {
    Ltmp = L_msu(ytp[i], *ptr2, mm);
    *ptr1 = extract_l(Ltmp);    
    ptr2++;ptr1++;
  }

}

