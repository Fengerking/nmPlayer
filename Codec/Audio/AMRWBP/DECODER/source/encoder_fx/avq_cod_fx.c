/*---------------------------------------------------------------------------*
 *         SPLIT ALGEBRAIC VECTOR QUANTIZER BASED ON RE8 LATTICE             *
 *---------------------------------------------------------------------------*
 * NOTE: a mitsmatch can occurs in some subvectors between the encoder       *
 *       and decoder, because the encoder use a bit-rate estimator to set    *
 *       the TCX global gain - this estimator is many times faster than the  *
 *       call of RE8_idx() for bits calculation.                             * 
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>


#define NQ_MAX     36
#define FAC_LOG2   3.321928095f
#define NSV_MAX    256     /* number of sub-vector max in QVAE, 256*8=2048 */

#include "typedef.h"
#include "basic_op.h"
#include "count.h"  
#include "oper_32b.h"
#include "amr_plus_fx.h"
#include "log2.h"
#include "math_op.h"

/* local function */
static Word16 Calc_bits(Word16 nq);
static void Sort(Word16 *ebits, Word16 n, Word16 *idx);
static void Split_idx_noovf(Word16 *xriq, Word16 NB_BITS, Word16 Nsv, Word16 *nq, UWord16 *I, Word16 *kv);
static void Writ_all_nq(Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 NB_BITS, Word16 Nsv, Word16 *last, Word16 **parm_ptr);
static void Chk_ovf(Word16 n_bits, Word16 n, Word16 *n1, Word16 *n2);
static void Writ_I(Word16 n, Word16 *pos_i, UWord16 *I, Word16 *parm);
static void Writ_k(Word16 n, Word16 *pos_i, Word16 *k, Word16 *parm, Word16 flag);
static void Init_pos_i_ovf(Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 last, Word16 *pos_i_ovf);
static void Writ_ovf(Word16 n_pack, Word16 *parm_ovf, Word16 n, Word16 *pos_i_ovf, Word16 *pos_n, Word16 **parm_ptr);
static void Writ_all_i(Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 last, UWord16 *I, Word16 *kv, Word16 **parm_ptr);


Word16 AVQ_Cod(   /* output: comfort noise gain factor      */ 
  Word16 *xri,    /* input:  vector to quantize             */
  Word16 *xriq,     /* output: quantized normalized vector (assuming the bit budget is enough) */ 
  Word16 NB_BITS,   /* input:  number of allocated bits          */ 
  Word16 Nsv)       /* input:  number of subvectors (lg=Nsv*8) */  
{

  Word16 i, l, n, iter, c[8];
  Word16 gain_inv, tmp, nbits, nbits_max, fac, offset;
  Word16 ebits[NSV_MAX], e_ebits, f_ebits, e_tmp,f_tmp, tmp16, l_8;
  Word32 ener, Ltmp,Lgain, x1[8],Lnbits;

  /* find energy of each subvector in log domain (scaled for bits estimation) */
  for (l=0; l<Nsv; l++)
  {
    ener = 4;     move32();/* to set ebits >= 0 */
    for (i=0;i<8;i++) 
    {
      ener = L_mac(ener, xri[l*8+i], xri[l*8+i]);
    }
    /* estimated bit consumption when gain=1 */ 

    /*ebits[l] = 5.0f*FAC_LOG2*(Word16)log10(ener*0.5);*/
    Log2(ener, &e_ebits, &f_ebits);
    e_ebits = sub(e_ebits,2);         /* *0.25 */
    Ltmp = Mpy_32_16(e_ebits, f_ebits, 40);  /* 40 = 5*8*/
    ebits[l] = extract_l(Ltmp);    move16(); /*Q4*/
  }
  /*---------------------------------------------------------------------*
   * subvector energy worst case:                                        *
   * - typically, it's a tone with maximum of amplitude (RMS=23170).     *
   * - fft length max = 1024 (N/2 is 512)                                *
   * log10(energy) = log10(23710*23710*1024*(N/2)) = 14.45               *
   * ebits --> 5.0*FAC_LOG2*14.45 = 240 bits                             *
   *---------------------------------------------------------------------*/

  /* estimate gain according to number of bits allowed */

  fac = 2048;    move16();      /* start at the middle (offset range = 0 to 255.75) Q6 */
  offset = 0;     move16();

  Ltmp = L_mult(31130, sub(NB_BITS, Nsv));  /* (1810 - 8 - 1152/8)*.95*/
  nbits_max = round(L_shl(Ltmp, 4));

  /* tree search with 10 iterations : offset with step of 0.25 bits (0.3 dB) */
  for (iter=0; iter<10; iter++)
  {
    offset = add(fac, offset);

    /* calculate the required number of bits */
    nbits = 0;          move16();

    for (l=0; l<Nsv; l++)
    {
      tmp = sub(ebits[l], offset);
      test();  
      if (tmp < 0) 
      {
        tmp = 0;        move16();
      }
      nbits = add(tmp, nbits);
    }

    /* decrease gain when no overflow occurs */
    test();
    if (sub(nbits,nbits_max) <= 0) 
    {
      offset = sub(offset, fac);
    }

    fac = mult(fac, 16384);
  } 

  Ltmp = L_shr(L_mult(offset, 13107),6); /* offset((2^21)/160 */

  L_Extract(Ltmp, &e_tmp, &f_tmp);
  tmp16 = extract_l(Pow2(14, f_tmp));
  Lgain = L_shl(tmp16, e_tmp);                        
  /*gain_inv = 1.0f / gain;*/
  e_tmp = norm_l(Lgain);
  tmp16 = extract_h(L_shl(Lgain,e_tmp));
  e_tmp = sub(31-14,e_tmp);
  gain_inv = div_s(16384, tmp16);
  e_tmp = sub(0,e_tmp);

  /* quantize all subvector using estimated gain */
  for (l=0; l<Nsv; l++)
  {
    l_8 = shl(l,3);
    for (i=0;i<8;i++) 
    {
      x1[i] = L_shl(L_mult(xri[l_8+i],gain_inv),e_tmp);     move32();
    }
    
    RE8_PPV_FX(x1, c);
    
    for (i=0;i<8;i++) 
    {
      xriq[l_8+i] = c[i];
    }
  }

  /* evaluate comfort noise level at freq. over 3200Hz (Nsv/2) */
  /* SV with ebits < 5 bits may not be quantized */
  Lnbits = 0;      move32();
  n = 10;          move16();
  for (l=Nsv/2; l<Nsv; l++)
  {
    tmp = sub(ebits[l], offset);
    test();
    if (sub(tmp,5*16) < 0)
    {
      Lnbits = L_mac(Lnbits, tmp, 2048);   /*Q16*/
      n = add(n,10);
    }
  }
  e_tmp = norm_s(n);
  n = shl(n, e_tmp);
  n = div_s(16384,n);
  n = shl(n, sub(e_tmp,14));

  Ltmp = L_mac(-32768, round(Lnbits), n); /* Q0->Q16 add -5/10 */   

  L_Extract(Ltmp, &e_tmp, &f_tmp);
  /*fac = (Word16)pow(10.0, (nbits/64-5.0) / (2.0*5.0*FAC_LOG2) );*/

  tmp16 = extract_l(Pow2(14, f_tmp));
  fac = shl(tmp16, add(e_tmp,1));  /* Q15*/      

  /* round bit allocations and save */
  for (i=0; i<Nsv; i++) 
  {
    xriq[(Nsv*8)+i] = shl(ebits[i],7-4);
  }

  return(fac);
}


/*
  AVQ_encmux(xriq, parm, n_bits, NB_BITS, Nsv, n_pack)
  ENCODE AND MULTIPLEX SUBVECTORS INTO SEVERAL PACKETS
   -> n_pack  : number of packets
   -> xriq    : rounded subvectors [0..8*Nsv-1]
                followed by rounded bit allocations [8*Nsv..8*Nsv+Nsv-1]
  <-> param   : multiplexed parameters
   -> n_bits  : size of each packet 
   -> Nsv     : number of subvectors

  note:
  Nsv MUST be multiple of n_pack

  IMPORTANT:
  it is assumed that codebook numbers in track #p do not cause bit
  budget overflow in packet #p   
  in practice this is ok if p<5 because the quantizer #n takes 5n bits
  and putting all bits in subvectors of track #p results in NB_BITS/5 bits
  for codebook numbers
*/
void AVQ_Encmux(Word16 n_pack, Word16 *xriq, Word16 *param, Word16 *n_bits, Word16 Nsv)
{
  Word16   last,i,p, n_cnt;
  Word16   kv[NSV_MAX*8], nq[NSV_MAX];
  UWord16  I[NSV_MAX];
  Word16 pos_n[N_PACK_MAX];
  /* Word16 pos_i[N_PACK_MAX]; */
  Word16 *parm;

  Word16 *parm_ptr[4];
  Word16 NB_BITS;

  NB_BITS=0;        move16();
  for (i=0; i<n_pack; i++) 
  {
    parm_ptr[i] = param;  move16();
    NB_BITS = add(NB_BITS, n_bits[i]);
    param += shr(add(n_bits[i],3),2);
  }

  /* initialize pointers and packets */
  for (p=0; p<n_pack; p++)
  {
      /* pos_i[p] = 0; */
      pos_n[p] = sub(n_bits[p],1);      move16();

      /* initialize packet to zero, i.e. all subvectors are set to zero */
      parm = parm_ptr[p];               move16();
      n_cnt = shr(sub(n_bits[p],1),2);
      for (i=0; i <= n_cnt; i++) 
      {
        parm[i] = 0;        move16();
      }
  }


  /* encode subvectors and fix possible overflows in TOTAL bit budget:
     i.e. find (i,nq) for each subvector where
          i is a codevector index split in a base codebook index (I) and
                                                  a Voronoi index (kv)
          nq is a codebook index (nq=0,2,3,4,...) */
  Split_idx_noovf(xriq, NB_BITS, Nsv, nq, I, kv);	


  /* split multiplexing of codebook numbers (by interleaved tracks) */
  Writ_all_nq(n_pack, nq, pos_n, NB_BITS, Nsv, &last, parm_ptr);	

  /* write indices
     multiplexing is done track-by-track (from track #0 to track #n_pack-1) */
  Writ_all_i(n_pack, nq, pos_n, last, I, kv, parm_ptr);

  return;
}


/*
  calc_bits(nq)
  COMPUTE (NUMBER OF BITS -1) TO DESCRIBE Q #nq
  -> nq: quantizer id (0,2,3,4...)
  <-   : bit allocation
*/
static Word16 Calc_bits(Word16 nq)
{
  test();
  if (sub(nq,2) >= 0) 
  {
    /* 4n bits + variable-length descriptor for allocation:
       descriptor -> nq
       0          -> 0
       10         -> 2
       110        -> 3
       => size of descriptor = 5n bits */
    
    return sub(add(shl(nq,2),nq),1); /* [5n-1] */
  }
  else 
  {
    return 0; /* 1-1 [1 bit to describe the allocation] */
  }
}


/*
  sort(ebits, n, idx)
  SORT SUBVECTORS BY DECREASING BIT ALLOCATIONS
  -> ebits : estimated bit allocations (table of n *positive* integers)
  -> n     : number of subvectors
  <- idx   : indices
*/
static void Sort(Word16 *ebits, Word16 n, Word16 *idx)
{
  Word16 t[NSV_MAX], i, j, ebits_max, pos;

  for (i=0; i<n;i++) 
  {
    t[i] = ebits[i];   move16();
  }
  for (i=0; i<n; i++) 
  {
      ebits_max = t[0];   move16();
      pos = 0;            move16();

      for (j=1; j<n; j++) 
      {
        test();
        if (sub(t[j],ebits_max)>0) 
        {
          ebits_max = t[j];     move16();
          pos = j;              move16();
        }
      }
      idx[i] = pos;               move16();
      t[pos] = -1;                move16();
  }
  return;
}



/*
  split_idx_noovf(xriq,NB_BITS, Nsv, nq, I, kv, last)
  COMPUTE MULTI-RATE INDICES FOR ALL SUBVECTORS AND FORCE NO BIT BUDGET OVERFLOW
  PRIOR TO MULTIPLEXING
  -> xriq    : rounded subvectors [0..8*Nsv-1]
               followed by rounded bit allocations [8*Nsv..8*Nsv+Nsv-1]
  -> NB_BITS : number of bits allocated for split multi-rate RE8 VQ
  -> Nsv     : number of subvectors
  <- nq      : codebook numbers
  <- I       : indices for base quantizers (Q2,Q3,Q4)
  <- kv      : Voronoi indices
*/
static void Split_idx_noovf(Word16 *xriq, Word16 NB_BITS, Word16 Nsv, Word16 *nq, UWord16 *I, Word16 *kv)
{
  Word16 k,l,n,n_bits,pos, pos_max;
  Word16 sort_idx[NSV_MAX];
  /* sort subvectors by estimated bit allocations in decreasing order
     (l=idx[0] is such that (rounded) ebits[l] is maximum) */
  Sort(&xriq[8*Nsv],Nsv,sort_idx);

  /* compute multi-rate indices and avoid bit budget overflow  */
  pos_max = 0;            move16();
  n_bits = 0;             move16();

  for (l=0; l<Nsv; l++) 
  {
    /* find vector to quantize (criteria: nb of estimated bits) */
    pos = sort_idx[l];    move16();
    
    /* compute multi-rate index of rounded subvector (nq,I,kv[]) */
    RE8_Cod(&xriq[pos*8], &nq[pos], &I[pos], &kv[8*pos]);
    test();
    if (nq[pos]>0) 
    {
      k = pos_max;        move16();
      if (sub(pos,k) > 0)
      {
        k = pos;          move16();
      }
        
      /* check for overflow and compute number of bits-1 (n) */
      n = Calc_bits(nq[pos]);       
      test();
      if ( sub(add(add(n_bits,n),k),NB_BITS) > 0) 
      { /* if budget overflow */
        nq[pos] = 0; /* force Q0 */   move16();
      } 
      else 
      {
        n_bits  = add(n_bits, n);
        pos_max = k;    move16(); /* update index of last described subvector (last) */
      }
    }
  }
}



/*
  writ_all_nq(n_pack, nq, pos_n, NB_BITS, last, parm_ptr)
  ENCODE AND MULTIPLEX ALL CODEBOOKS NUMBERS IN nq[] TRACK-BY-TRACK
   ->  n_pack      : number of packets
   ->  nq          : table of codebook numbers [0..Nsv-1]
   ->  pos_n       : table of pointers to write nq in packets [0..n_pack-1]
   ->  NB_BITS     : total bit allocation
  <-  last        : index of last subvector for which an index is written
  <-> parm        : bistream
*/
static void Writ_all_nq(Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 NB_BITS, Word16 Nsv, Word16 *last, Word16 **parm_ptr)
{
  Word16 p, pos, l, i, n_bits, *parm;
  
  n_bits = NB_BITS;       move16();
  *last = -1;             move16();

  /* write nq[l] for l=0...Nsv-1 in packet #p= mod(l,number_of_packets)*/
  for (l=0;l<Nsv;l++) 
  {
    /*p = l%n_pack;*/ /* alternative : apply mask 0x01 / 0x03 */ 
    p = (l & sub(n_pack,1));    logic16();
    parm = parm_ptr[p];         move16();
    pos = pos_n[p];             move16();

    /* compute the minimal bit budget to write nq[l] and the related index */
    i = Calc_bits(nq[l]);

    /* if the budget is exceeded, force nq[l] to 0
       else decrement the number of left bits */
    test();
    if (sub(i,n_bits) > 0) 
    {
      nq[l] = 0;      move16();
    } 
    else 
    {
      n_bits = sub(n_bits, i);
    }

    /* update "last" */
    test();
    if (sub(nq[l],2) >= 0) 
    {
      *last = l;              move16();
    }

    /* write the unary code (except stop bit) for nq[l] in packet #p */
    i = sub(nq[l], 1);
    test();
    while (i > 0) 
    {
      i = sub(i,1);
      parm[pos/4] =  add(parm[pos/4],shl(1,(pos&3)));     move16();logic16();
      pos = sub(pos,1);  
      test();
    }

    /* if bit budget is not empty, write stop bit of unary code */
    test();
    if (n_bits > 0) 
    {
      pos = sub(pos,1);  
      n_bits = sub(n_bits,1);
    }

    pos_n[p] = pos;           move16();
  }
  return;
}

/* check if n groups of 4 bits fit in n_bits bits */
static void Chk_ovf(Word16 n_bits, Word16 n, Word16 *n1, Word16 *n2)
{
  test();
  if (sub(shl(n,2),n_bits) <= 0) 
  {
    *n1=n;      move16();
    *n2=0;      move16();
  }
  else 
  {
    test();
    if (n_bits < 0) 
    {
      *n1 = shr_r(n_bits ,2); 
    }
    else
    {
      *n1 = shr(n_bits ,2); 
    }
    move16();    
    *n2 = sub(n, *n1);               move16();
  }
}

/* write n groups of 4-bit for base codebook index (I) */
static void Writ_I(Word16 n, Word16 *pos_i, UWord16 *I, Word16 *parm)
{
  Word16 pos;
  
  /* base codebook index */
  pos = shr_r(*pos_i,2);
  test();
  while (n > 0) 
  {
    test();
    n = sub(n,1);
    parm[pos++] = (*I & 0x0F);      logic16();    move16();
    *I = L_shr(*I,4);       move16();
  }
  *pos_i = shl(pos, 2);     move16();
}

/* write n groups of 4-bit for Voronoi index (k[]) */
static void Writ_k(Word16 n, Word16 *pos_i, Word16 *k, Word16 *parm, Word16 flag)
{
  Word16 i, ival, delta, *kv, pos;
  
  delta = shl(flag,2);
  pos = shr_r(*pos_i,2);
  test();
  while (n > 0) 
  {
    test();
    n = sub(n,1);

    kv = k + delta;     move16(); /*ptr*/
    ival = 0;             move16();
    for (i=0; i<4; i++) 
    {
      ival  = shl(ival,1);
      ival = add(ival,(kv[i] & 0x01));  logic16();
      kv[i] = shr(kv[i],1);     move16();
    }
    parm[pos++] = ival;       move16();
    delta = add(delta,4)%8; /* circular shift */   logic16();
  }
  *pos_i = shl(pos,2);      move16();
}

/* find in each packet the positions where overflow occurs */
static void Init_pos_i_ovf(Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 last, Word16 *pos_i_ovf)
{
  Word16 p, pos, n_bits, l, n1, n2;
  
  for (p=0; p<n_pack; p++) 
  {
    pos = 0;              move16();
    n_bits = add(pos_n[p],1); /* pos_n[p] - 0 +1 */ 

    for (l=p; l<=last; l+=n_pack) 
    {
      test();
      if (nq[l] > 0) 
      {
        Chk_ovf(n_bits, nq[l], &n1, &n2);
        n_bits  = sub(n_bits, shl(n1,2));
        pos = add(pos,n1);
      }
    }
    pos_i_ovf[p] = shl(pos,2);      move16();
  }
}

/* write bits in overflow */
static void Writ_ovf(Word16 n_pack, Word16 *parm_ovf, Word16 n, Word16 *pos_i_ovf, Word16 *pos_n, Word16 **parm_ptr)
{
  Word16 pos_ovf, p, n_bits, pos, *parm, moved_bit;
  
  /* initialize position in overflow packet (parm_ovf[]) */
  pos_ovf = 0;        move16();

  /* move bits from overflow packet (parm_ovf[]) to packets (parm_ptr[][])
     [write 4-bit by 4-bit] */
  for (p=0; p<n_pack; p++) 
  {
    /* compute number of bits left in packet #p */
    n_bits = add(sub(pos_n[p], pos_i_ovf[p]), 1);
    test();test();
    if ((sub(n_bits,4) >= 0) && (n>0)) 
    {
      pos = shr(pos_i_ovf[p],2);
      parm = parm_ptr[p];    move16();

      do 
      {
        parm[pos++] = parm_ovf[pos_ovf++];      move16(); /* move 4 bits */
        n_bits = sub(n_bits,4);
        n  = sub(n, 4);
        test();test();
      } 
      while ((sub(n_bits,4) >= 0) && (n>0));
      pos_i_ovf[p] = shl(pos,2);        move16();
    }
  }
  pos_ovf = shl(pos_ovf,2);
  
  /* move bits remaining in overflow packet
     [write bit-by-bit (3 bits at maximum per packet)] */

  for (p=0; p<n_pack; p++) 
  {
    /* compute number of bits left in packet #p */
    n_bits = add(sub(pos_n[p],pos_i_ovf[p]),1);

    test();test();  
    if ((n_bits > 0) && (n>0)) 
    {
      pos = pos_i_ovf[p];         move16();
      parm = parm_ptr[p];         move16();
      do 
      {
        /* write a single bit */
        moved_bit = (shr(parm_ovf[pos_ovf/4], (pos_ovf&3)) & 0x01);     logic16();logic16();
        parm[pos/4] = add(parm[pos/4], shl(moved_bit,(pos&3)));                 logic16();move16();    

        pos = add(pos,1);
        pos_ovf = add(pos_ovf, 1);
        n_bits  = sub(n_bits,1);
        n = sub(n, 1);
        test();test();
      } while ((n_bits > 0) && (n > 0));
      pos_i_ovf[p] = pos;       move16();
    }
  }
}

/*
  writ_all_i(nq, p, pos_n, n_pack, last, pos_ovf, n_bits_left, parm_ptr)
  MULTIPLEX AN INDEX (I,kv) OF 4*nq BITS INTO BITSTREAM
  (THE INDEX CORRESPONDS TO A SUBVECTOR IN TRACK #p)
  ->  nq          : codebook number (scalar)
  ->  p           : index of track
  ->  pos_i       : pointer to write i in packet [0..n_pack-1]
  ->  n_pack      : number of packets
  <-> pos_ovf     : pointers for overflow [0..n_pack-1]
  <-> n_bits_left : number of unused bits in packets [0..n_pack-1]
  ->  I           : base codebook index
  ->  kv          : Voronoi index
  <-> parm        : bistream [initialized to zero when writ_all_i is called]

  important note:
  - if the index fits completely in packet #p, multiplexing is done as
    in writ_parm from top to bottom:
    the 1st bit of I is written at the top
    the last bit of kv at the bottom
*/
static void Writ_all_i(Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 last, UWord16 *I, Word16 *kv, Word16 **parm_ptr)
{
  Word16 pos_ovf, p, l, n_bits, ni, nk, n1, i, n2, pos, k[8], *parm, l_8;
  UWord16 index;
  Word16 parm_ovf[NQ_MAX];
  Word16 pos_i_ovf[N_PACK_MAX];
  
  /* initialize overflow packet */
  for (i=0; i<NQ_MAX; i++) 
  {
    parm_ovf[i]=0;        move16();
  }

  /* find positions in each packet from which the bits in overflow can be
     written */
  Init_pos_i_ovf(n_pack, nq, pos_n, last, pos_i_ovf);	

  /* write indices */
  for (p=0; p<n_pack; p++)
  {
    pos = 0;                    move16();
    parm = parm_ptr[p];         move16();

    for (l=p; l<=last; l+=n_pack) 
    {
      test();
      if (nq[l] > 0) 
      {
        /* compute number of bits left in packet #p */
        n_bits = add(sub(pos_n[p],pos),1);
        /* compute number of 4-bit groups for base codebook index (ni)
           and Voronoi index (nk) */
        ni = nq[l];       move16();
        nk = 0;           move16();
        test();
        if (sub(ni,4) > 0) 
        {
          nk = shr(sub(ni,4-1),1); /* nkv*2 = number of 4-bit groups */
          nk = shl(nk, 1);
          ni = sub(ni, nk);
        }
        /* write base codebook index (in packet #p / overflow packet) */
        index = I[l];                   move16();
        Chk_ovf(n_bits, ni, &n1, &n2);
        Writ_I(n1, &pos, &index, parm);
        n_bits = sub(n_bits, shl(n1,2));
        test();
        if (n2>0) 
        {
          /* write 4-bit groups in overflow packet */
          pos_ovf = 0;        move16();
          Writ_I(n2, &pos_ovf, &index, parm_ovf);

          /* distribute bits from overflow packet to packets
             #0 to n_pack-1 */
          Writ_ovf(n_pack, parm_ovf, shl(n2,2), pos_i_ovf, pos_n, parm_ptr);	

          for (i=0; i<n2; i++) 
          {
            parm_ovf[i]=0;      move16();
          }
        }
        test();
        if (nk>0) 
        {
          /* write Voronoi index (in packet #p / overflow packet) */
          l_8 = shl(l,3);  
          for (i=0; i<8; i++) 
          {
            k[i] = kv[l_8+i];         move16();
          }
          Chk_ovf(n_bits, nk, &n1, &n2);
          Writ_k(n1, &pos, k, parm, 0);
          test();
          if (n2>0) 
          {
            pos_ovf = 0;      move16();
            Writ_k(n2, &pos_ovf, k, parm_ovf,(n1%2));     logic16();
            Writ_ovf(n_pack, parm_ovf, shl(n2,2), pos_i_ovf, pos_n, parm_ptr);
            for (i=0; i<n2; i++) 
            {
              parm_ovf[i]=0;      move16();
            }
          }
        }
      }
    }
  }
}
