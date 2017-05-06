 /*---------------------------------------------------------------------------*
 *         SPLIT ALGEBRAIC VECTOR QUANTIZER BASED ON RE8 LATTICE             *
 *---------------------------------------------------------------------------*
 * NOTE: a mitsmatch can occurs in some subvectors between the encoder       *
 *       and decoder, because the encoder use a bit-rate estimator to set    *
 *       the TCX global gain - this estimator is many times faster than the  *
 *       call of RE8_idx() for bits calculation.                             * 
 *---------------------------------------------------------------------------*/

//#include <stdio.h>
//#include <stdlib.h>
#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "count.h"


#define NQ_MAX     36
#define FAC_LOG2   3.321928095
#define NSV_MAX    256     /* number of sub-vector max in QVAE, 256*8=2048 */


/* local functions */

static void Read_nq(Word16 *nq, Word16 *n_bits, Word16 *parm, Word16 *pos);
static void Read_all_nq(Word16 n_pack, Word16 NB_BITS, Word16 Nsv, Word16 *pos_n, Word16 *nq, Word16 *last, Word16  **parm_ptr);
static void Chk_ovf(Word16 n_bits, Word16 n, Word16 *n1, Word16 *n2);
static void Read_I(Word16 n, Word16 *pos_i, UWord16 *I, Word16 *parm);
static void Read_k(Word16 n, Word16 *pos_i, Word16 *k, Word16  *parm, Word16 flag);
static void Init_pos_i_ovf(Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 last, Word16 *pos_i_ovf);
static void Read_ovf(Word16 n_pack, Word16 *parm_ovf, Word16 n, Word16 *pos_i_ovf, Word16 *pos_n, Word16  **parm_ptr);
static void Read_all_i(Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 last, UWord16 *I, Word16 *kv, Word16 **parm_ptr);
static void Read_track(Word16 *pos_n, Word16 *pos_i, Word16 *nq, UWord16 *I, Word16 *kv, Word16 *parm);

/*
  AVQ_demuxdec_multi(parm, xriq, parm, NB_BITS, Nsv, n_pack, bfi)
  DEMULTIPLEX AND DECODE SUBVECTORS FROM SEVERAL PACKETS
  -> n_pack  : number of packets
  -> parm    : bitstream = table of 4-bit words [0..(NB_BITS-1)/4]
               the bitstream is divided into n_pack blocks
             parm[0]..parm[NB_BITS-1]
  <- xriq    : rounded subvectors [0..8*Nsv-1]
               followed by rounded bit allocations [8*Nsv..8*Nsv+Nsv-1]
  -> NB_BITS : number of bits allocated for split multi-rate RE8 VQ
  -> Nsv     : number of subvectors
  -> bfi     : bad frame indicator for each packet

  notes (see AVQ_encmux_multi)
*/
#if (LARGE_VAR_OPT)
static Word16 nq[NSV_MAX], kv[8*NSV_MAX];
static UWord16 I[NSV_MAX];
#endif
void AVQ_Demuxdec(Word16 n_pack,Word16 *param,Word16 *n_bits, Word16 *xriq, Word16 Nsv, Word16 *bfi)
{
#if (FUNC_AVQ_DEMUXDEC_OPT)

  Word16 p, pos_i[N_PACK_MAX], pos_n[N_PACK_MAX], l, i;
  Word16 any_loss;
  Word16 last, c[8];
#if (!LARGE_VAR_OPT)
  Word16 nq[NSV_MAX], kv[8*NSV_MAX];
  UWord16 I[NSV_MAX];
#endif

  Word16  *parm_ptr[4];
  Word16 NB_BITS;

  NB_BITS = 0;
  for (i=0; i<n_pack; i++) 
  {
    parm_ptr[i] = param;
    NB_BITS += n_bits[i];
    param += (n_bits[i] + 3) >> 2;
  }

  /* initialize pointers */
  for (p=0; p<n_pack; p++)
  {
    pos_i[p] = 0;
    pos_n[p] = n_bits[p] - 1;
  }

  /* verify if any packet is lost */
  any_loss = 0;
  for (p=0; p<n_pack; p++)
  {
    any_loss |= bfi[p];
  }

  /* demultiplex and decode */
  if (any_loss == 0)
  {

    /* read and decode encoded nq, set overflow pointers and compute
       number of unused bits in each packet and index of last described
       subvector */
    Read_all_nq(n_pack, NB_BITS, Nsv, pos_n, nq, &last, parm_ptr);
    /* read indices (i) */
    Read_all_i(n_pack,nq,pos_n,last,I,kv,parm_ptr);

    /* decode all */
    for (l=0; l<Nsv; l++)
    {
      /* multi-rate RE8 decoder */

      RE8_Dec(nq[l], I[l], &kv[8*l], c);
      /* write decoded RE8 vector to decoded subvector #l */
      for (i=0;i<8;i++)
      {
        xriq[l*8+i] = c[i];
      }
    }
  }
  else
  {
    /* do not decode bits in overflow if any packet is lost */
    for (p=0; p<n_pack; p++)
    {
      if (bfi[p] == 0)
      {
        /* read and decode encoded codebook numbers and indices jointly
           stop when oveflow or after last subvector */
        for (l=p; l<Nsv; l += n_pack)
        { 
          /* read and decode codebook number (nq), read index (i)
             note : we cannot use read_parm because of pos_i/pos_n
                    which may not be multiple of 4 */
          Read_track(pos_n+p, pos_i+p, &nq[l], &I[l], &kv[8*l], parm_ptr[p]);

          /* multi-rate RE8 decoder */
          RE8_Dec(nq[l], I[l], &kv[8*l], c);

          /* write decoded RE8 vector to subvector l */
          for (i=0;i<8;i++)
          {
            xriq[l*8+i] = c[i];
          }
        }
      }
      else
      for (l=p; l<Nsv; l+=n_pack)
      {
        for (i=0; i<8; i++)
        {
          xriq[8*l+i]=0;
        }
      }
    }
  }
  return;
  
#else


  Word16 p, pos_i[N_PACK_MAX], pos_n[N_PACK_MAX], l, i;
  Word16 any_loss;
  Word16 nq[NSV_MAX], kv[8*NSV_MAX], last, c[8];
  UWord16 I[NSV_MAX];

  Word16  *parm_ptr[4];
  Word16 NB_BITS;

  NB_BITS = 0;  
  for (i=0; i<n_pack; i++) 
  {
    parm_ptr[i] = param;      
    NB_BITS = add(NB_BITS, n_bits[i]);
    param += shr(add(n_bits[i],3),2);
  }

  /* initialize pointers */
  for (p=0; p<n_pack; p++)
  {
    pos_i[p] = 0;                     
    pos_n[p] = sub(n_bits[p],1);      
  }

  /* verify if any packet is lost */
  any_loss = 0;                       
  for (p=0; p<n_pack; p++)
  {
    any_loss |= bfi[p];               logic16();
  }

  /* demultiplex and decode */
  
  if (any_loss == 0)
  {

    /* read and decode encoded nq, set overflow pointers and compute
       number of unused bits in each packet and index of last described
       subvector */
    Read_all_nq(n_pack, NB_BITS, Nsv, pos_n, nq, &last, parm_ptr);
    /* read indices (i) */
    Read_all_i(n_pack,nq,pos_n,last,I,kv,parm_ptr);

    /* decode all */
    for (l=0; l<Nsv; l++)
    {
      /* multi-rate RE8 decoder */

      RE8_Dec(nq[l], I[l], &kv[8*l], c);
      /* write decoded RE8 vector to decoded subvector #l */
      for (i=0;i<8;i++)
      {
        xriq[l*8+i] = c[i];   
      }
    }
  }
  else
  {
    /* do not decode bits in overflow if any packet is lost */
    for (p=0; p<n_pack; p++)
    {
      
      if (bfi[p] == 0)
      {
        /* read and decode encoded codebook numbers and indices jointly
           stop when oveflow or after last subvector */
        for (l=p; l<Nsv; l += n_pack)
        { 
          /* read and decode codebook number (nq), read index (i)
             note : we cannot use read_parm because of pos_i/pos_n
                    which may not be multiple of 4 */
          Read_track(pos_n+p, pos_i+p, &nq[l], &I[l], &kv[8*l], parm_ptr[p]);

          /* multi-rate RE8 decoder */
          RE8_Dec(nq[l], I[l], &kv[8*l], c);

          /* write decoded RE8 vector to subvector l */
          for (i=0;i<8;i++)
          {
            xriq[l*8+i] = c[i];   
          }
        }
      }
      else
      for (l=p; l<Nsv; l+=n_pack)
      {
        for (i=0; i<8; i++)
        {
          xriq[8*l+i]=0;          
        }
      }
    }
  }
  return;
#endif
}

/* read a single codebook number */
static void Read_nq(Word16 *nq, Word16 *n_bits, Word16 *parm, Word16 *pos)
{
  *nq = 0;      
  
  if (sub(*n_bits,9) >= 0)
  {
    logic16();logic16();
    if (sub((shr(parm[shr(*pos,2)] ,(*pos & 3)) & 0x01),1) == 0)
    {
      *nq = 2;                      
      *n_bits = sub(*n_bits, 9);    
      *pos = sub(*pos,1);           

      /* CHECK: add a test for nq == NQ_MAX */
       logic16();logic16();
      while ( (sub((shr(parm[shr(*pos,2)] ,(*pos & 3)) & 0x01),1) == 0) &&
      (sub(*n_bits,5) >= 0) && (sub(*nq,NQ_MAX) <0))
      {
        *nq = add(*nq,1);             
        *n_bits = sub(*n_bits, 5);    
        *pos = sub(*pos, 1);          
         logic16();logic16();
      }
    }
  }
  
  if (*n_bits > 0)
  {
    *n_bits = sub(*n_bits, 1);        
    *pos = sub(*pos, 1);             
  }
}


/*
  read_all_nq(pos_n, n_pack, nq, pos_ovf, n_bits_left, last, parm)
  DE-MULTIPLEX AND DECODE AND ALL CODEBOOKS NUMBERS IN nq[] TRACK-BY-TRACK
   ->  nq         : table of codebook numbers [0..Nsv-1]
   ->  pos_n      : table of pointers to write nq in packets [0..n_pack-1]
   ->  NB_BITS    : number of bits allocated for split multi-rate RE8 VQ
   ->  n_pack     : number of packets
   ->  last       : index of last described subvector
  <-  pos_ovf     : pointers for overflow [0..n_pack-1]
  <-  n_bits_left : number of unused bits in packets [0..n_pack-1]
  <-> parm        : bistream
*/
static void Read_all_nq(Word16 n_pack, Word16 NB_BITS, Word16 Nsv, Word16 *pos_n, Word16 *nq, Word16 *last, Word16 **parm_ptr)
{
  Word16 n_bits, p, l, n_pack_m1;

  n_bits = NB_BITS;   
  *last = -1;          /* nsv : 36, 72, 144 */
  n_pack_m1 = sub(n_pack ,1);   /* npack: 1, 2, 4 */
  for (l=0; l<Nsv; l++)
  {
    p = (l & n_pack_m1);      logic16();
    Read_nq(&nq[l], &n_bits, parm_ptr[p], &pos_n[p]);
    
    if (nq[l] > 0)
    {
      *last = l;    
    }
  }

  return;
}


/* check if n groups of 4 bits (i.e. 4n bits) fit in n_bits bits */
static void Chk_ovf(Word16 n_bits, Word16 n, Word16 *n1, Word16 *n2)
{
  
  if (sub(shl(n,2),n_bits)<=0)
  {
    *n1 = n;  
    *n2 = 0;  
  }
  else
  {
    
    if (n_bits < 0) 
    {
      *n1 = shr_r(n_bits ,2); 
    }
    else
    {
      *n1 = shr(n_bits ,2); 
    }
        
    *n2 = sub(n, *n1);               
  }
}

/* read 4n bits for base codebook index (I) */
static void Read_I(Word16 n, Word16 *pos_i, UWord16 *I, Word16 *parm)
{
  Word16 pos, *pt_prm;

  /* base codebook index */
  *pos_i = add(*pos_i , shl(n,2));    
  pos = sub(shr(*pos_i,2), 1);
  pt_prm = &parm[pos];
  
  while (n > 0)
  {
    n = sub(n,1);
    *I = (UWord16)L_shl(*I, 4);                  
    *I = (UWord16)L_add(*I, (*pt_prm & 0x0F));   logic16();
    pt_prm--;
    
  }


}

/* read 4n bits for Voronoi index (k[]) */
static void Read_k(Word16 n, Word16 *pos_i, Word16 *k, Word16 *parm, Word16 flag)
{
  Word16 pos, i, ival, delta, *kv, *pt_prm, tmp16;

  *pos_i = add(*pos_i, shl(n,2));     
  pos    = sub(shr(*pos_i,2), 1);
  pt_prm = &parm[pos];              
  delta  = shl(flag,2);              
  
  while (n > 0)
  {
    n    = sub(n,1);
    kv   = k + delta;
    ival = (*pt_prm & 0x0F);  logic16(); /* optional mask */
    pt_prm--;
    for (i=3; i>=0; i--)
    {
      tmp16 = shl(kv[i], 1);   
      kv[i] = add(tmp16,(ival & 0x01));    logic16();
      ival  = shr(ival, 1);
    }
    delta = add(delta,4)%8; logic16();/* circular shift */
  }
}

/* split a codebook number (nq) into a number of bits for the base
   codebook index (4 x ni) and  the Voronoi index (4 x nk) */
static void Split_n(Word16 nq, Word16 *ni, Word16 *nk)
{

  Word16 tmp;

  *ni = nq;   
  *nk=0;      
  
  if (sub(*ni,4) > 0)
  {
    tmp = shr(sub(*ni, 3),1);
    *nk = shl(tmp,1);     
    *ni = sub(*ni, *nk);  
  }

}

/* find in each packet the positions where overflow occurs */
static void Init_pos_i_ovf(Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 last, Word16 *pos_i_ovf)
{
  Word16 p, pos, n_bits, l, n1, n2;

  /* find in each packet the positions where overflow occurs & count the number
     of bits to put in the extra packet */
  for (p=0; p<n_pack; p++)
  {
    pos = 0;        
    n_bits = add(pos_n[p],1); /* pos_n[p] - 0 +1 */ 
    for (l=p; l<=last; l+=n_pack)
    {
      
      if (nq[l] > 0)
      {      
        Chk_ovf(n_bits, nq[l], &n1, &n2);
        n_bits = sub(n_bits, shl(n1,2));
        pos = add(pos, n1);
      }
    }
    pos_i_ovf[p] = shl(pos,2);      
  }
}

/* read bits in overflow */
void Read_ovf(Word16 n_pack, Word16 *parm_ovf, Word16 n, Word16 *pos_i_ovf, Word16 *pos_n, Word16 **parm_ptr)
{
  Word16 p, n_bits, pos, pos_ovf, moved_bit, *pt_ovf, *pt_prm;
  Word16 *parm;

  /* initialize position in overflow packet (parm_ovf[]) */
  pos_ovf = 0;      

  /* read 4-bit chunks */
  for (p=0; p<n_pack; p++)
  {
    n_bits = add(sub(pos_n[p], pos_i_ovf[p]), 1);
    
    if ((sub(n_bits,4) >= 0) && (n>0))
    {
      pos = shr(pos_i_ovf[p],2);
      parm = parm_ptr[p];       /*ptr*/     
      pt_ovf =  &parm_ovf[pos_ovf];     
      pt_prm =  &parm[pos];             

      do
      {
        *pt_ovf = *pt_prm;    
        pt_ovf++; pt_prm++;
        n_bits = sub(n_bits, 4);
        n = sub(n,4);
        pos = add(pos,1);
        pos_ovf = add(pos_ovf,1);
        
      }
      while ((sub(n_bits,4) >= 0) && (n>0));
      pos_i_ovf[p] = shl(pos,2);
    }
  }
  pos_ovf = shl(pos_ovf ,2);

  /* read bit-by-bit */
  for (p=0; p<n_pack; p++)
  {
    n_bits = add(sub(pos_n[p], pos_i_ovf[p]), 1);
    
    if ((n_bits > 0) && (n>0))
    {
      pos = pos_i_ovf[p];     
      parm = parm_ptr[p];     
      do
      {
        moved_bit = (shr(parm[shr(pos,2)], (pos&3)) & 0x01);  logic16();logic16();
        parm_ovf[shr(pos_ovf,2)] = add(parm_ovf[shr(pos_ovf,2)], shl(moved_bit,(pos_ovf&3)));

        n_bits  = sub(n_bits, 1);
        n       = sub(n,1);
        pos     = add(pos,1);
        pos_ovf = add(pos_ovf, 1);
        
      }
      while ((n_bits > 0) && (n>0));
      pos_i_ovf[p] = pos;       
    }
  }
}


/*
  read_all_i(nq, p, pos_n, n_pack, last, pos_ovf, n_bits_left, I, kv, parm)
  DEMULTIPLEX AN INDEX (I,kv) OF 4*nq BITS FROM BITSTREAM
  (THE INDEX CORRESPONDS TO A SUBVECTOR IN TRACK #p)
   -> n_pack      : number for packets
   -> nq          : codebook numbers
   -> pos_n       : position in bitstream after reading nq[]
   -> last        : index of last described subvector
  <-  I           : base codebook index
  <-  kv          : Voronoi index
   -> parm_ptr    : multiple bistreams
*/
static void Read_all_i( Word16 n_pack, Word16 *nq, Word16 *pos_n, Word16 last, UWord16 *I, Word16 *kv, Word16 **parm_ptr)
{
  Word16 pos_ovf, p, l, n_bits, ni, nk, n1, i, n2, pos, pos_i_ovf[N_PACK_MAX];
  UWord16 index;
  Word16 parm_ovf[NQ_MAX];
  Word16 *parm;

  /* initialize overflow packet */
  for (i=0; i<NQ_MAX; i++)
  {
    parm_ovf[i]=0;    
  }

  /* find positions in each packet from which the bits in overflow can be read */
  Init_pos_i_ovf(n_pack, nq, pos_n, last, pos_i_ovf);  

  /* read indices */
  for (p=0; p<n_pack; p++)
  {
    pos = 0;               /* pos_i[p]/4 */
    parm = parm_ptr[p]; /* ptr*/

    for (l=p; l<=last; l+=n_pack)
    {
      
      if (nq[l] > 0)
      {      
        /* compute number of bits left for indices in packet #p */
        n_bits = add(sub(pos_n[p], pos), 1);

        Split_n(nq[l], &ni, &nk);

        /* read I in packet #p and in overflow */
        Chk_ovf(n_bits, ni, &n1, &n2);
        index = 0;      
        
        if (n2>0)
        {
          pos_ovf = 0;    

          Read_ovf(n_pack, parm_ovf, shl(n2,2), pos_i_ovf, pos_n, parm_ptr);
          Read_I(n2, &pos_ovf, &index, parm_ovf);

          for (i=0; i<n2; i++)
          {
            parm_ovf[i] = 0;      
          }
        }
        Read_I(n1, &pos, &index, parm);
        n_bits = sub(n_bits, shl(n1,2));
        I[l] = index;         

        /* read Voronoi index  */
        
        if (nk >0)
        {
          for (i=0; i<8; i++)
          {
            kv[8*l+i] = 0;
          }

          Chk_ovf(n_bits, nk, &n1, &n2);
          
          if (n2>0)
          {
            pos_ovf = 0;      
            Read_ovf(n_pack, parm_ovf, shl(n2,2), pos_i_ovf, pos_n, parm_ptr);
            Read_k(n2, &pos_ovf, &kv[8*l], parm_ovf,1);
            for (i=0; i<n2; i++)
            {
              parm_ovf[i] = 0;      
            }
          }
          Read_k(n1, &pos, &kv[8*l], parm,(Word16)((n2+1)&1));
        }
      }
    }
  }
}


/*
  read_track(pos_n, pos_i, nq, I, kv, parm)
  DEMULTIPLEX nq AND INDEX (I,kv) OF 4*nq BITS FROM BITSTREAM
  (THE INDEX CORRESPONDS TO A SUBVECTOR IN TRACK #p)
  <-> pos_n       : pointer to read nq
  <-> pos_i       : pointer to read i
  <-  nq          : codebook number (scalar)
  <-  I           : base codebook index
  <-  kv          : Voronoi index
   -> parm        : bistream
*/
static void Read_track(Word16 *pos_n, Word16 *pos_i, Word16 *nq, UWord16 *I, Word16 *kv, Word16 *parm)
{
  Word16 n_bits, i,ni,nk;

  /* compute number of bits left for indices in packet #p */
  n_bits = add(sub(*pos_n, *pos_i), 1);
      
  /* read nq */
  Read_nq(nq, &n_bits, parm, pos_n);

  /* read i and kv */
  
  if (*nq > 0)
  {
    *I=0;       
    for (i=0; i<8; i++)
    {
      kv[i] = 0;    
    }

    Split_n(*nq, &ni, &nk);
    Read_I(ni, pos_i, I, parm);
    Read_k(nk, pos_i, kv, parm,1);
  }
}

