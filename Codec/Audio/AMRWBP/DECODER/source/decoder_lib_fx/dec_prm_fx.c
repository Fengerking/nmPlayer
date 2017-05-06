#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"



/* local function */
static Word16 Pack4bits(Word16 nbits, Word16 *ptr, Word16 *prm);


void Dec_prm(
  Word16 mod[],         /* (i) : frame mode (mode[4], 4 frames)   */
  Word16 bad_frame[],   /* (i) : bfi for 4 frames (bad_frame[4])  */
  Word16 serial[],      /* (i) : serial bits stream               */
  Word16 nbits_pack,    /* (i) : number of bits per packet of 20ms*/
  Word16 codec_mode,    /* (i) : AMR-WB+ mode (see cnst.h)        */
  Word16 param[],       /* (o) : decoded parameters               */
  Word16 nbits_AVQ[]    /* (o) : nb of bits for AVQ (4 division)  */
)                       
{
  Word16 j, k, mode, nbits, n, sfr, index0, index1, parity, n_loss, i;
  Word16 *ptr, *prm;
  /* remove bits for mode and HF extension */
  nbits = (NBITS_CORE_FX[codec_mode] >> 2) - 2;
  k = 0;
  while(k < NB_DIV)
  {
    mode = mod[k];
    prm = param + (k*DEC_NPRM_DIV);

    if ((mode == 0) || (mode == 1))
    {
      ptr = serial + shr(extract_l(L_mac(4, k, nbits_pack)),1); /* +2 because of mode */ 

      prm[0] = Bin2int(8, ptr);      ptr += 8;  
      prm[1] = Bin2int(8, ptr);      ptr += 8;  
      prm[2] = Bin2int(6, ptr);      ptr += 6;  
      prm[3] = Bin2int(7, ptr);      ptr += 7;  
      prm[4] = Bin2int(7, ptr);      ptr += 7;  
      prm[5] = Bin2int(5, ptr);      ptr += 5;  
      prm[6] = Bin2int(5, ptr);      ptr += 5;  

      if (mode == 0)
      {
        j = 7; 
        prm[j] = Bin2int(2, ptr);   ptr += 2;   j++; 
        for (sfr=0; sfr<4; sfr++)
        {
          if ((sfr==0) || (sfr == 2)) 
          {
            n=9;
          }
          else 
          {
            n=6;  
          }

          prm[j] = Bin2int(n, ptr);       ptr += n;    j++;   
          prm[j] = Bin2int(1, ptr);       ptr += 1;    j++;  

          if (codec_mode == MODE_9k6)
          {
            /* 20 bits AMR-WB codebook is used */
            prm[j] = Bin2int(5, ptr);       ptr += 5;     j++;  
            prm[j] = Bin2int(5, ptr);       ptr += 5;     j++;  
            prm[j] = Bin2int(5, ptr);       ptr += 5;     j++;  
            prm[j] = Bin2int(5, ptr);       ptr += 5;     j++;  
          }
          else if(codec_mode == MODE_11k2)
          {
            /* 28 bits AMR-WB codebook is used */
            prm[j] = Bin2int(9, ptr);       ptr += 9;     j++;  
            prm[j] = Bin2int(9, ptr);       ptr += 9;     j++;  
            prm[j] = Bin2int(5, ptr);       ptr += 5;     j++;  
            prm[j] = Bin2int(5, ptr);       ptr += 5;     j++;  
          }
          else if(codec_mode == MODE_12k8)
          {
            /* 36 bits AMR-WB codebook is used */
            prm[j] = Bin2int(9, ptr);       ptr += 9;     j++;  
            prm[j] = Bin2int(9, ptr);       ptr += 9;     j++;  
            prm[j] = Bin2int(9, ptr);       ptr += 9;     j++;  
            prm[j] = Bin2int(9, ptr);       ptr += 9;     j++;  
          }
          else if(codec_mode == MODE_14k4)
          {
            /* 44 bits AMR-WB codebook is used */
            prm[j] = Bin2int(13, ptr);      ptr += 13;    j++;  
            prm[j] = Bin2int(13, ptr);      ptr += 13;    j++;  
            prm[j] = Bin2int(9, ptr);       ptr += 9;     j++;  
            prm[j] = Bin2int(9, ptr);       ptr += 9;     j++;  
          }
          else if(codec_mode == MODE_16k)
          {
            /* 52 bits AMR-WB codebook is used */
            prm[j] = Bin2int(13, ptr);      ptr += 13;    j++;  
            prm[j] = Bin2int(13, ptr);      ptr += 13;    j++;  
            prm[j] = Bin2int(13, ptr);      ptr += 13;    j++;  
            prm[j] = Bin2int(13, ptr);      ptr += 13;    j++;  
          }
          else if(codec_mode == MODE_18k4)
          { 
            /* 64 bits AMR-WB codebook is used */
            prm[j] = Bin2int(2, ptr);       ptr += 2;     j++;  
            prm[j] = Bin2int(2, ptr);       ptr += 2;     j++;  
            prm[j] = Bin2int(2, ptr);       ptr += 2;     j++;  
            prm[j] = Bin2int(2, ptr);       ptr += 2;     j++;  
            prm[j] = Bin2int(14, ptr);      ptr += 14;    j++;  
            prm[j] = Bin2int(14, ptr);      ptr += 14;    j++;  
            prm[j] = Bin2int(14, ptr);      ptr += 14;    j++;  
            prm[j] = Bin2int(14, ptr);      ptr += 14;    j++;  
          }
          else if(codec_mode == MODE_20k)
          {
            /* 72 bits AMR-WB codebook is used */
            prm[j] = Bin2int(10, ptr);      ptr += 10;    j++;  
            prm[j] = Bin2int(10, ptr);      ptr += 10;    j++;  
            prm[j] = Bin2int(2,  ptr);      ptr += 2;     j++;  
            prm[j] = Bin2int(2,  ptr);      ptr += 2;     j++;  
            prm[j] = Bin2int(10, ptr);      ptr += 10;    j++;  
            prm[j] = Bin2int(10, ptr);      ptr += 10;    j++;  
            prm[j] = Bin2int(14, ptr);      ptr += 14;    j++;  
            prm[j] = Bin2int(14, ptr);      ptr += 14;    j++;  
          }
          else if(codec_mode == MODE_23k2)
          {
            /* 88 bits AMR-WB codebook is used */
            prm[j] = Bin2int(11, ptr);      ptr += 11;    j++;  
            prm[j] = Bin2int(11, ptr);      ptr += 11;    j++;  
            prm[j] = Bin2int(11, ptr);      ptr += 11;    j++;  
            prm[j] = Bin2int(11, ptr);      ptr += 11;    j++;  
            prm[j] = Bin2int(11, ptr);      ptr += 11;    j++;  
            prm[j] = Bin2int(11, ptr);      ptr += 11;    j++;  
            prm[j] = Bin2int(11, ptr);      ptr += 11;    j++;  
            prm[j] = Bin2int(11, ptr);      ptr += 11;    j++;  
          }

          prm[j] = Bin2int(7, ptr);       ptr += 7;    j++;  
        }
      }
      else  /* mode 1 */
      {
        prm[7] = Bin2int(3, ptr);     ptr += 3;   
        prm[8] = Bin2int(7, ptr);     ptr += 7;   

        nbits_AVQ[k] = nbits - 56;         
        Pack4bits(nbits_AVQ[k], ptr, prm+9);
      }
      k++;
    }
    else if(mode == 2)
    {
      /* decode first 20ms packet */
      ptr = serial + (extract_l(L_mac(4, k, nbits_pack)) >> 1);

      prm[0] = Bin2int(8, ptr);    ptr += 8;  
      prm[1] = Bin2int(8, ptr);    ptr += 8;  
      prm[7] = Bin2int(3, ptr);    ptr += 3;  
      prm[8] = Bin2int(7, ptr);    ptr += 7;  


      nbits_AVQ[k] = nbits - 26;            
      j = 9;                              
      j += Pack4bits(nbits_AVQ[k], ptr, prm+j);
      k++;

      /* decode second 20ms packet */
      ptr = serial + (extract_l(L_mac(4, k,nbits_pack)) >> 1);

      prm[2] = Bin2int(6, ptr);    ptr += 6;  
      prm[3] = Bin2int(7, ptr);    ptr += 7;  
      prm[4] = Bin2int(7, ptr);    ptr += 7;  
      prm[5] = Bin2int(5, ptr);    ptr += 5;  
      prm[6] = Bin2int(5, ptr);    ptr += 5;  

      if(bad_frame[k-1] == 1) 
      {
        prm[8] = (Bin2int(6, ptr) << 1);      
      }
      ptr += 6;

      nbits_AVQ[k] = nbits - 36;              /* 6 bits of redundancy for TCX gain */ 
      Pack4bits(nbits_AVQ[k], ptr, prm+j);
      k++;
    }
    else if(mode == 3)
    {
      /* decode first 20ms packet */
      ptr = serial + (extract_l(L_mac(4, k, nbits_pack)) >> 1);

      prm[0] = Bin2int(8, ptr);    ptr += 8;  
      prm[1] = Bin2int(8, ptr);    ptr += 8;  
      prm[8] = Bin2int(7, ptr);    ptr += 7;  

      nbits_AVQ[k] = nbits - 23;
      j = 9;  
      j += Pack4bits(nbits_AVQ[k], ptr, prm+j);
      k++;

      /* decode second 20ms packet */
      ptr = serial + (extract_l(L_mac(4, k, nbits_pack)) >> 1);

      prm[2] = Bin2int(6, ptr);    ptr += 6;  
      prm[7] = Bin2int(3, ptr);    ptr += 3;  

      /* read parity check bits */
      parity = Bin2int(3, ptr);    ptr += 3;

      nbits_AVQ[k] = nbits - 12; 
      j += Pack4bits(nbits_AVQ[k], ptr, prm+j);
      k++;

      /* decode third 20ms packet */
      ptr = serial + (extract_l(L_mac(4, k, nbits_pack)) >> 1);

      prm[3] = Bin2int(7, ptr);    ptr += 7;  
      prm[5] = Bin2int(5, ptr);    ptr += 5; 

      /* read parity check bits */
      index0 = Bin2int(3, ptr);  ptr += 3;

      nbits_AVQ[k] = nbits - 15;
      j += Pack4bits(nbits_AVQ[k], ptr, prm+j);
      k++;

      /* decode fourth 20ms packet */
      ptr = serial + (extract_l(L_mac(4, k, nbits_pack)) >> 1);

      prm[4] = Bin2int(7, ptr);    ptr += 7;  
      prm[6] = Bin2int(5, ptr);    ptr += 5;  

      /* read parity check bits */
      index1 = Bin2int(3, ptr);    ptr += 3;

      /* use the redundancy bits to recover the index of TCX gain in case of packet erasures */
      n_loss = 0;
      
      for (i=1; i<4; i++) 
      {
        n_loss += bad_frame[i];
      }

      if ((bad_frame[0] == 1) && (n_loss <= 1)) 
      {
        /* decode the index of TCX gain */
        if(bad_frame[2] == 1) 
        {
          index0 = (parity^index1);
        }
        if(bad_frame[3] == 1) 
        {
          index1 = (parity^index0); 
        }

        prm[8] = (index0 << 4) + (index1 << 1); 
      }

      nbits_AVQ[k] = nbits - 15;
      Pack4bits(nbits_AVQ[k], ptr, prm+j);
      k++;
    }    /*mode 3*/
  }      /* end while (k < NB_DIV) */
  return;
}



void Dec_prm_hf(
  Word16 mod[],         /* (i) : frame mode (mode[4], 4 frames)   */
  Word16 bad_frame[],   /* (i) : bfi for 4 frames (bad_frame[4])  */
  Word16 serial[],      /* (i) : serial bits stream               */
  Word16 nbits_pack,    /* (i) : number of bits per packet of 20ms*/
  Word16 param[]        /* (o) : decoded parameters               */
)                       
{
  Word16 i, k, mode, nbits, *prm;
  Word16 *ptr;
  /* number of bits for HF extension (per packet of 20 ms) */
  nbits = NBITS_BWE/4;        

  k = 0;                      
  
  while (sub(k,NB_DIV) < 0)
  {
      
    mode = mod[k];            

    /* set pointer to parameters */
    prm = param + shr(extract_l(L_mult(k, NPRM_BWE_DIV)),1);

    /* decode first 20ms packet */
    ptr = serial + sub(shr(extract_l(L_mult(add(k,1),nbits_pack)),1), nbits);

    prm[0] = Bin2int(2, ptr);      ptr += 2;      
    prm[1] = Bin2int(7, ptr);      ptr += 7;      
    prm[2] = Bin2int(7, ptr);      ptr += 7;      
    k++;

    
    if (sub(mode,2) == 0)
    {
      /* decode second 20ms packet */
      ptr = serial + sub(shr(extract_l(L_mult(add(k,1),nbits_pack)),1), nbits);


      for (i=3; i<=10; i++) 
      {
        prm[i] = Bin2int(2, ptr);    ptr += 2;      
      }
      k++;
    }
    else if (sub(mode,3) == 0)
    {
      /* decode second 20ms packet */
      ptr = serial + sub(shr(extract_l(L_mult(add(k,1),nbits_pack)),1), nbits);


      for (i=3; i<=10; i++) 
      {
        prm[i] = shl(Bin2int(2, ptr),1);   ptr += 2;      
      }
      k++;

      /* decode third 20ms packet */
      ptr = serial + sub(shr(extract_l(L_mult(add(k,1),nbits_pack)),1), nbits);


      for (i=11; i<=18; i++) 
      {
        prm[i] = shl(Bin2int(2, ptr),1);   ptr += 2;    
      }
      k++;

      /* decode fourth 20ms packet */
      ptr = serial + sub(shr(extract_l(L_mult(add(k,1),nbits_pack)),1), nbits);
        
      if (bad_frame[k] == 0)
      {

        for (i=3; i<=18; i++) 
        {
          prm[i] = add(prm[i],  Bin2int(1, ptr));     ptr += 1;      
        }
      }
      k++;
    }
  }  /* end while (k < NB_DIV) */
  return;
}



static Word16 Pack4bits(
  Word16  nbits,
  Word16 *ptr,
  Word16 *prm
)
{
  Word16 i;

  i=0;      
    
  while (sub(nbits,4) > 0)
  {
    prm[i] = Bin2int(4, ptr);   
    ptr += 4;
    nbits = sub(nbits, 4);
    i++;
  }
  prm[i] = Bin2int(nbits, ptr);     
  i++;

  return(i);
}

