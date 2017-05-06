#include "typedef.h"
#include "count.h"
#include "basic_op.h"
#include "oper_32b.h"

#include "amr_plus_fx.h"


static void Nearest_neighbor_2D8(Word32 x[], Word16 y[]);

/*--------------------------------------------------------------
  RE8_PPV(x,y)
  NEAREST NEIGHBOR SEARCH IN INFINITE LATTICE RE8
  the algorithm is based on the definition of RE8 as
      RE8 = (2D8) U (2D8+[1,1,1,1,1,1,1,1])
  it applies the coset decoding of Sloane and Conway

  (i) x: point in R^8
  (o) y: point in RE8 (8-dimensional integer vector)
  --------------------------------------------------------------
*/
void RE8_PPV_FX(Word32 x[] /*Q8*/, Word16 y[])
{
#if (FUNC_RE8_PPV_FX_OPT)
  Word16 i,y0[8],y1[8], hi, lo;
  Word32 e0,e1,x1[8], Ltmp;

  /* find the nearest neighbor y0 of x in 2D8 */
  Nearest_neighbor_2D8(x, y0);              

  /* find the nearest neighbor y1 of x in 2D8+(1,...,1) (by coset decoding) */

  {
//    x1[0] = L_sub(x[0],QR);
//    x1[1] = L_sub(x[1],QR);
//    x1[2] = L_sub(x[2],QR);
//    x1[3] = L_sub(x[3],QR);
//    x1[4] = L_sub(x[4],QR);
//    x1[5] = L_sub(x[5],QR);
//    x1[6] = L_sub(x[6],QR);
//    x1[7] = L_sub(x[7],QR);
    x1[0] = x[0] - QR;
    x1[1] = x[1] - QR;
    x1[2] = x[2] - QR;
    x1[3] = x[3] - QR;
    x1[4] = x[4] - QR;
    x1[5] = x[5] - QR;
    x1[6] = x[6] - QR;
    x1[7] = x[7] - QR;
  }
  Nearest_neighbor_2D8(x1, y1);             

  {
    y1[0]++;
    y1[1]++;
    y1[2]++;
    y1[3]++;
    y1[4]++;
    y1[5]++;
    y1[6]++;
    y1[7]++;
  }

  /* compute e0=||x-y0||^2 and e1=||x-y1||^2 */
  
  e1 = 0;
  e0 = 0;
  for (i=0;i<8;i++)
  {
    /*tmp = x[i]-y0[i];*/                     
    Ltmp = x[i] - (y0[i] <<15);

//    hi   = extract_h(L_shl(Ltmp, 1));
//    lo   = extract_l(L_msu(Ltmp, hi, 16384));
//    Ltmp = L_mult(hi,hi); 
//    Ltmp = L_shl(Ltmp,14);     
//    Ltmp = L_mac(Ltmp,hi,lo);
//    Ltmp = L_add(Ltmp,mult(lo,lo));
    hi   = Ltmp >> 15;
    lo   = (Word16)(Ltmp - (hi << 15));
    Ltmp = (hi * hi) << 1;
    Ltmp <<= 14;
    Ltmp += (hi * lo) << 1;
    Ltmp += (lo * lo) >> 15;
  
    /*e0+=tmp*tmp;     */
    e0  += Ltmp;

//    Ltmp = L_sub(x[i], L_shl(y1[i],15)); 
//    hi = extract_h(L_shl(Ltmp, 1));
//    lo = extract_l(L_msu(Ltmp, hi, 16384));
//    Ltmp = L_mult(hi,hi); 
//    Ltmp = L_shl(Ltmp,14);
//    Ltmp = L_mac(Ltmp,hi,lo);
//    Ltmp = L_add(Ltmp,mult(lo,lo));
    Ltmp = x[i] - (y1[i] << 15);
    hi = (Word16)(Ltmp >> 15);
    lo = (Word16)(Ltmp - (hi << 15));
    Ltmp = (hi * hi) << 1;
    Ltmp <<= 14;
    Ltmp += (hi * lo) << 1;
    Ltmp += (lo * lo) >> 15;
  

    /*e1+=tmp*tmp;     */
    e1  += Ltmp;
  }

  /* select best candidate y0 or y1 to minimize distortion */
  if (e0 < e1)
  {
    y[0] = y0[0];
    y[1] = y0[1];
    y[2] = y0[2];
    y[3] = y0[3];
    y[4] = y0[4];
    y[5] = y0[5];
    y[6] = y0[6];
    y[7] = y0[7];
  }
  else
  {
    y[0] = y1[0];
    y[1] = y1[1];
    y[2] = y1[2];
    y[3] = y1[3];
    y[4] = y1[4];
    y[5] = y1[5];
    y[6] = y1[6];
    y[7] = y1[7];
  }

  return;
  
#else

  Word16 i,y0[8],y1[8], hi, lo;
  Word32 e0,e1,x1[8], Ltmp;

  /* find the nearest neighbor y0 of x in 2D8 */
  Nearest_neighbor_2D8(x, y0);              

  /* find the nearest neighbor y1 of x in 2D8+(1,...,1) (by coset decoding) */

  for (i=0;i<8;i++)
  {
    x1[i]= L_sub(x[i],QR);                      
    
  }
  Nearest_neighbor_2D8(x1, y1);             
                                        
  for (i=0;i<8;i++)
  {
    y1[i] = add(y1[i], 1);                             
  }


  /* compute e0=||x-y0||^2 and e1=||x-y1||^2 */
  
  e1 = 0; move32();
  e0 = 0; move32();
  for (i=0;i<8;i++)
  {
    /*tmp = x[i]-y0[i];*/                     
    Ltmp = L_sub(x[i], L_shl(y0[i],15)); 

    hi   = extract_h(L_shl(Ltmp, 1));
    lo   = extract_l(L_msu(Ltmp, hi, 16384));
    Ltmp = L_mult(hi,hi); 
    Ltmp = L_shl(Ltmp,14);     
    Ltmp = L_mac(Ltmp,hi,lo);
    Ltmp = L_add(Ltmp,mult(lo,lo));
  
    /*e0+=tmp*tmp;     */
    e0  = L_add(Ltmp,e0);

    Ltmp = L_sub(x[i], L_shl(y1[i],15)); 
    hi = extract_h(L_shl(Ltmp, 1));
    lo = extract_l(L_msu(Ltmp, hi, 16384));
    Ltmp = L_mult(hi,hi); 
    Ltmp = L_shl(Ltmp,14);
    Ltmp = L_mac(Ltmp,hi,lo);
    Ltmp = L_add(Ltmp,mult(lo,lo));
  

    /*e1+=tmp*tmp;     */
    e1  = L_add(Ltmp,e1);
  }

  /* select best candidate y0 or y1 to minimize distortion */
                                        
  if (L_sub(e0,e1)<0)
  {                                       
    for (i=0;i<8;i++)
    {
      y[i] = y0[i];                       
    }
  }
  else
  {                                       ;
    for (i=0;i<8;i++)
    {
      y[i] = y1[i];                       
    }
  }
                                        
  return;
#endif
}


/*--------------------------------------------------------------
  nearest_neighbor_2D8(x,y)
  NEAREST NEIGHBOR SEARCH IN INFINITE LATTICE 2D8
  algorithm: nn_2D8(x) = 2*nn_D8(x/2)
             nn_D8 = decoding of Z^8 with Wagner rule
  (see Conway and Sloane's paper in IT-82)

  (i) x: point in R^8
  (o) y: point in 2D8 (8-dimensional integer vector)
  --------------------------------------------------------------
*/
void Nearest_neighbor_2D8(Word32 x[] /*Q8*/, Word16 y[])
{
  Word16 i,j;
  Word16 sum, tmp16;
  Word32 s,e[8],em;

                                      
  /* round x into 2Z^8 i.e. compute y=(y1,...,y8) such that yi = 2[xi/2]
  where [.] is the nearest integer operator
  in the mean time, compute sum = y1+...+y8
  */
  sum=0;                                          
                                        ;
  for (i=0;i<8;i++)
  {
    /* round to ..., -2, 0, 2, ... ([-1..1[ --> 0) */
                                          
    if (x[i] < 0)
    {
      /*y[i] = -2*(((int)(1.0-x[i]))>>1);   */
      tmp16 = extract_l(L_shr(L_sub(QR, x[i]),15+1));
      y[i] = negate(shl(tmp16,1));   
                
    }
    else
    {
      /*y[i] = 2*(((int)(1.0+x[i]))>>1);    */
      tmp16 = extract_l(L_shr(L_add(QR, x[i]),15+1));
      y[i] = shl(tmp16,1);   

    }
    /*sum += y[i];                            */
    sum  =add(sum, y[i]);
  }

  /* check if y1+...+y8 is a multiple of 4
  if not, y is not round xj in the wrong way where j is defined by
    j = arg max_i | xi -yi|
  (this is called the Wagner rule)
  */
  logic16();                                    
  if (sum&3)
  {
    /* find j = arg max_i | xi -yi| */
    em=0;   move32();                              
    j=0;                                  
                                  
    for (i=0;i<8;i++)
    {
      /* compute ei = xi-yi */
      /*e[i]=x[i]-y[i];     */
      e[i] = L_msu(x[i], y[i], QR/2);       move32();

      /* compute |ei| = | xi-yi | */
      s = L_abs(e[i]);                      
      
      /* check if |ei| is maximal, if so, set j=i */
                                  
      if (L_sub(em,s) <0)
      {
        em=s;   move32();                         
        j=i;                          
      }
    }

    /* round xj in the "wrong way" */
                                      
    if (e[j]<0)
    {
      y[j] = sub(y[j], 2);                              
    }
    else
    {
      y[j] = add(y[j], 2);                         
    }
  }

  return;
}



