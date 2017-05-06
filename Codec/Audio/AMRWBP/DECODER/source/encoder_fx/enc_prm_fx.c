#include "typedef.h"
#include "basic_op.h"
#include "count.h"

#include "amr_plus_fx.h"
extern const Word16 NBITS_CORE_FX[];


/*-----------------------------------------------------------------*
 * Funtion  enc_prm()                                              *
 * ~~~~~~~~~~~~~~~~~~                                              *
 * encode AMR-WB+ parameters according to selected mode            *
 *-----------------------------------------------------------------*/
Word16 Unpack4bits(Word16 nbits, Word16 *prm, Word16 *ptr)
{
  Word16 i;
  i = 0;                    move16();
  test();
  while (sub(nbits,4) > 0) 
  {
    Int2bin(prm[i], 4, ptr);
    ptr += 4;
    nbits = sub(nbits, 4);
    i ++;
    test();
  }
  Int2bin(prm[i], nbits, ptr);
  i ++;
  return(i);
}

void Enc_prm(
  Word16 mod[],         /* (i) : frame mode (mode[4], 4 division) */
  Word16 codec_mode,    /* (i) : AMR-WB+ mode (see cnst.h)        */
  Word16 sparam[],      /* (i) : Parameters to encode             */  
  Word16 serial[],      /* (o) : serial bits stream               */
  Word16 nbits_pack     /* (i) : number of bits per packet of 20ms*/
)
{
  Word16 j, k, n, sfr, mode, n_pack, nbits, *sprm, parity, bit;
  Word16 nbits_AVQ[N_PACK_MAX], prm_AVQ[(NBITS_MAX/4)+N_PACK_MAX];
  Word16 *ptr;

  /* remove bits for mode (2 bits per 20ms packet) */
  nbits = sub(shr(NBITS_CORE_FX[codec_mode],2), 2);

  k = 0;                  move16();
  test();
  while (sub(k,NB_DIV) < 0) 
  {
    mode = mod[k];        move16();
 
    /* set pointer to parameters */
    sprm = sparam + (k*NPRM_DIV);    move16();
    test();test();  test();test();
    if ((mode == 0) || (sub(mode,1) == 0))
    {
      ptr = serial + extract_l(L_shr(L_mac(4, k, nbits_pack),1));     move16();/* +2 because of mode */			

      /* encode LPC parameters (46 bits) */

      Int2bin(sprm[0], 8, ptr);      ptr += 8;
      Int2bin(sprm[1], 8, ptr);      ptr += 8;
      Int2bin(sprm[2], 6, ptr);      ptr += 6;
      Int2bin(sprm[3], 7, ptr);      ptr += 7;
      Int2bin(sprm[4], 7, ptr);      ptr += 7;
      Int2bin(sprm[5], 5, ptr);      ptr += 5;
      Int2bin(sprm[6], 5, ptr);      ptr += 5;

      test();
      if (mode == 0) 
      {
       /*---------------------------------------------------------*
        * encode 20ms ACELP frame                                 *
        * acelp bits: 2+(9+6+9+6)+4+(4xICB_NBITS])+(4x7)          *
        *---------------------------------------------------------*/
        j = 7;                                move16();

        /* mean energy : 2 bits */
        Int2bin(sprm[j], 2, ptr);       ptr += 2;    j++;
        for (sfr=0; sfr<4; sfr++) 
        {
          test();test();
          if ((sfr == 0) || (sub(sfr,2) == 0)) 
          {
            n=9;        move16();
          } 
          else 
          {
            n=6;      move16();
          }

          /* AMR-WB closed-loop pitch lag */
          Int2bin(sprm[j], n, ptr);       ptr += n;    j++;
          Int2bin(sprm[j], 1, ptr);       ptr += 1;    j++;
  
          test();test();test();test();test();test();test();test();
          if (sub(codec_mode,MODE_9k6) == 0) 
          {
            /* 20 bits AMR-WB codebook is used */
            Int2bin(sprm[j], 5, ptr);       ptr += 5;     j++;
            Int2bin(sprm[j], 5, ptr);       ptr += 5;     j++;
            Int2bin(sprm[j], 5, ptr);       ptr += 5;     j++;
            Int2bin(sprm[j], 5, ptr);       ptr += 5;     j++;
          } 
          else if (sub(codec_mode,MODE_11k2) == 0) 
          {
            /* 28 bits AMR-WB codebook is used */
            Int2bin(sprm[j], 9, ptr);       ptr += 9;     j++;
            Int2bin(sprm[j], 9, ptr);       ptr += 9;     j++;
            Int2bin(sprm[j], 5, ptr);       ptr += 5;     j++;
            Int2bin(sprm[j], 5, ptr);       ptr += 5;     j++;
          } 
          else if (sub(codec_mode,MODE_12k8) == 0) 
          {
            /* 36 bits AMR-WB codebook is used */
            Int2bin(sprm[j], 9, ptr);       ptr += 9;     j++;
            Int2bin(sprm[j], 9, ptr);       ptr += 9;     j++;
            Int2bin(sprm[j], 9, ptr);       ptr += 9;     j++;
            Int2bin(sprm[j], 9, ptr);       ptr += 9;     j++;
          }
          else if (sub(codec_mode,MODE_14k4) == 0) 
          {
            /* 44 bits AMR-WB codebook is used */
            Int2bin(sprm[j], 13, ptr);      ptr += 13;    j++;
            Int2bin(sprm[j], 13, ptr);      ptr += 13;    j++;
            Int2bin(sprm[j], 9, ptr);       ptr += 9;     j++;
            Int2bin(sprm[j], 9, ptr);       ptr += 9;     j++;
          }
          else if (sub(codec_mode,MODE_16k) == 0) 
          {
            /* 52 bits AMR-WB codebook is used */
            Int2bin(sprm[j], 13, ptr);      ptr += 13;    j++;
            Int2bin(sprm[j], 13, ptr);      ptr += 13;    j++;
            Int2bin(sprm[j], 13, ptr);      ptr += 13;    j++;
            Int2bin(sprm[j], 13, ptr);      ptr += 13;    j++;
          }
          else if (sub(codec_mode,MODE_18k4) == 0) 
          {
            /* 64 bits AMR-WB codebook is used */
            Int2bin(sprm[j], 2, ptr);       ptr += 2;     j++;
            Int2bin(sprm[j], 2, ptr);       ptr += 2;     j++;
            Int2bin(sprm[j], 2, ptr);       ptr += 2;     j++;
            Int2bin(sprm[j], 2, ptr);       ptr += 2;     j++;
            Int2bin(sprm[j], 14, ptr);      ptr += 14;    j++;
            Int2bin(sprm[j], 14, ptr);      ptr += 14;    j++;
            Int2bin(sprm[j], 14, ptr);      ptr += 14;    j++;
            Int2bin(sprm[j], 14, ptr);      ptr += 14;    j++;
          }
          else if (sub(codec_mode,MODE_20k) == 0) 
          {
            /* 72 bits AMR-WB codebook is used */
            Int2bin(sprm[j], 10, ptr);      ptr += 10;    j++;
            Int2bin(sprm[j], 10, ptr);      ptr += 10;    j++;
            Int2bin(sprm[j], 2,  ptr);      ptr += 2;     j++;
            Int2bin(sprm[j], 2,  ptr);      ptr += 2;     j++;
            Int2bin(sprm[j], 10, ptr);      ptr += 10;    j++;
            Int2bin(sprm[j], 10, ptr);      ptr += 10;    j++;
            Int2bin(sprm[j], 14, ptr);      ptr += 14;    j++;
            Int2bin(sprm[j], 14, ptr);      ptr += 14;    j++;
          }
          else if (sub(codec_mode,MODE_23k2) == 0) 
          {
            /* 88 bits AMR-WB codebook is used */
            Int2bin(sprm[j], 11, ptr);      ptr += 11;    j++;
            Int2bin(sprm[j], 11, ptr);      ptr += 11;    j++;
            Int2bin(sprm[j], 11, ptr);      ptr += 11;    j++;
            Int2bin(sprm[j], 11, ptr);      ptr += 11;    j++;
            Int2bin(sprm[j], 11, ptr);      ptr += 11;    j++;
            Int2bin(sprm[j], 11, ptr);      ptr += 11;    j++;
            Int2bin(sprm[j], 11, ptr);      ptr += 11;    j++;
            Int2bin(sprm[j], 11, ptr);      ptr += 11;    j++;
          }

          /* AMR-WB 7 bits gains codebook */
          Int2bin(sprm[j], 7, ptr);       ptr += 7;    j++;
        }
      } /* end of mode 0 */
      else  /* mode 1 */
      {
        /* encode 20ms TCX */
        n_pack = 1;       move16();
        nbits_AVQ[0] = sub(nbits,56);   move16();
        AVQ_Encmux(n_pack, sprm+9, prm_AVQ, nbits_AVQ, 288/8);
        
        Int2bin(sprm[7], 3, ptr);     ptr += 3;
        Int2bin(sprm[8], 7, ptr);     ptr += 7;

        Unpack4bits(nbits_AVQ[0], prm_AVQ, ptr);
      } /* end of mode 1 */
      k ++;
    } /* end of mode 0/1 */
    else if (sub(mode,2) == 0)
    {
      /* encode and multiplex 40ms TCX */
      n_pack = 2;                           move16();
      nbits_AVQ[0] = sub(nbits,26);         move16();
      nbits_AVQ[1] = sub(nbits,30+6);       move16();

      AVQ_Encmux(n_pack, sprm+9, prm_AVQ, nbits_AVQ, 576/8);

      /* encode first 20ms packet */
      ptr = serial + extract_l(L_shr(L_mac(4, k, nbits_pack),1));     move16();/* +2 because of mode */

      Int2bin(sprm[0], 8, ptr);      ptr += 8;
      Int2bin(sprm[1], 8, ptr);      ptr += 8;
      Int2bin(sprm[7], 3, ptr);      ptr += 3;
      Int2bin(sprm[8], 7, ptr);      ptr += 7;

      j = Unpack4bits(nbits_AVQ[0], prm_AVQ, ptr);
      k ++;

      /* encode second 20ms packet */
      ptr = serial + extract_l(L_shr(L_mac(4, k, nbits_pack),1));;    move16();


      Int2bin(sprm[2], 6, ptr);      ptr += 6;
      Int2bin(sprm[3], 7, ptr);      ptr += 7;
      Int2bin(sprm[4], 7, ptr);      ptr += 7;
      Int2bin(sprm[5], 5, ptr);      ptr += 5;
      Int2bin(sprm[6], 5, ptr);      ptr += 5;
      Int2bin(shr(sprm[8],1), 6, ptr);   ptr += 6; /* 6 bits redundancy for TCX gain */	 

      Unpack4bits(nbits_AVQ[1], prm_AVQ+j, ptr);
      k ++;
    } /* end of mode 2 */
    else if (sub(mode,3) == 0)
    {
      /* encode and multiplex 80ms TCX */
      n_pack = 4;                       move16();
      nbits_AVQ[0] = sub(nbits,23);     move16();
      nbits_AVQ[1] = sub(nbits,9+3);    move16();
      nbits_AVQ[2] = sub(nbits, 12+3);  move16();
      nbits_AVQ[3] = sub(nbits, 12+3);  move16();

      AVQ_Encmux(n_pack, sprm+9, prm_AVQ, nbits_AVQ, 1152/8);
          
      /* encode first 20ms packet */
      ptr = serial + extract_l(L_shr(L_mac(4, k, nbits_pack),1));     move16();/* +2 because of mode */

      Int2bin(sprm[0], 8, ptr);      ptr += 8;
      Int2bin(sprm[1], 8, ptr);      ptr += 8;
      Int2bin(sprm[8], 7, ptr);      ptr += 7;

      j = Unpack4bits(nbits_AVQ[0], prm_AVQ, ptr);
      k ++;

      /* encode second 20ms packet */
      ptr = serial + extract_l(L_shr(L_mac(4, k, nbits_pack),1));    move16();

      Int2bin(sprm[2], 6, ptr);      ptr += 6;
      Int2bin(sprm[7], 3, ptr);      ptr += 3;

      /* write 3 parity check bits */
      bit = ((shr(sprm[8],6)) & 0x01) ^ ((shr(sprm[8],3)) & 0x01);   logic16();logic16();logic16();
      parity = shl(bit,2);
      bit = ((shr(sprm[8],5)) & 0x01) ^ ((shr(sprm[8],2)) & 0x01);   logic16();logic16();logic16();
      parity = add(parity, shl(bit,1));
      bit = ((shr(sprm[8],4)) & 0x01) ^ ((shr(sprm[8],1)) & 0x01);   logic16();logic16();logic16();
      parity = add(parity, bit);
      Int2bin(parity, 3, ptr);      ptr += 3;
      
      j = add(j,Unpack4bits(nbits_AVQ[1], &prm_AVQ[j], ptr));
      k ++;

      /* encode third 20ms packet */
      ptr = serial + extract_l(L_shr(L_mac(4, k, nbits_pack),1));    move16();

      Int2bin(sprm[3], 7, ptr);      ptr += 7;
      Int2bin(sprm[5], 5, ptr);      ptr += 5;

      /* 3 bits of the TCX gain */
      Int2bin(shr(sprm[8],4), 3, ptr);      ptr += 3;

      j = add(j, Unpack4bits(nbits_AVQ[2], &prm_AVQ[j], ptr));
      k ++;

      /* encode fourth 20ms packet */
      ptr = serial + extract_l(L_shr(L_mac(4, k, nbits_pack),1));    move16();

      Int2bin(sprm[4], 7, ptr);      ptr += 7;
      Int2bin(sprm[6], 5, ptr);      ptr += 5;

      /* 3 bits of the TCX gain */
      Int2bin((shr(sprm[8],1)) & 0x07, 3, ptr);      ptr += 3;    logic16();

      Unpack4bits(nbits_AVQ[3], &prm_AVQ[j], ptr);
      k++;
    }  /* end of mode 3 */

  } /* end of while k < NB_DIV */

  return;
}  



void Enc_prm_hf(
  Word16 mod[],         /* (i) : frame mode (mode[4], 4 division) */
  Word16 param[],       /* (i) : parameters                       */
  Word16 serial[],    /* (o) : serial bits stream               */
  Word16 nbits_pack     /* (i) : number of bits per packet of 20ms*/
)
{
  Word16 i, k, mode, nbits, *prm;
  Word16 *ptr;
  /* bits per 20ms packet */
  nbits = NBITS_BWE/4;            move16();

  k = 0;      move16();

  test();
  while (sub(k,NB_DIV) < 0) 
  {
    test();
    mode = mod[k];                  move16();
 
    /* set pointer to parameters */
    prm = param + (k*NPRM_BWE_DIV);    move16();

    /* encode first 20ms packet */
    ptr = serial + sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)), nbits);     move16();

    Int2bin(prm[0], 2, ptr);      ptr += 2;
    Int2bin(prm[1], 7, ptr);      ptr += 7;
    Int2bin(prm[2], 7, ptr);      ptr += 7;
    k ++;

    test();test();
    if (sub(mode,2) == 0) 
    {
      /* encode second 20ms packet */
      ptr = serial + sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)), nbits);     move16();


      for (i=3; i<=10; i++) 
      {
        Int2bin(prm[i], 2, ptr);    ptr += 2;
      }
      k ++;
    } 
    else if (sub(mode,3) == 0)
    {
      /* encode second 20ms packet */
      ptr = serial + sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)), nbits);     move16();

      for (i=3; i<=10; i++) 
      {
        Int2bin(shr(prm[i],1), 2, ptr);   ptr += 2;
      }
      k++;

      /* encode third 20ms packet */
      ptr = serial + sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)), nbits);     move16();

      for (i=11; i<=18; i++) 
      {
        Int2bin(shr(prm[i],1), 2, ptr);   ptr += 2;
      }
      k ++;

      /* encode fourth 20ms packet */
      ptr = serial + sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)), nbits);     move16();

      for (i=3; i<=18; i++) 
      {
        Int2bin(prm[i], 1, ptr);      ptr += 1;
      }
      k ++;
    }
  }  /* end while (k < NB_DIV) */
  return;
}


