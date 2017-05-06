#include <stdlib.h>
#include <memory.h>

#include "typedef.h"
#include "basic_op.h"
#include "count.h"

#include"options_fx.h"
#include "cnst_wb_fx.h"


#include "acelp_fx.h"
#include "main_fx.h"
#include "cod_main_fx.h"

#include "enc_if_fx.h"
#include "if_rom_fx.h"

#define L_FRAME16k   320            /* Frame size at 16kHz     */
#define MRNO_DATA    15

/* number of bits */
#define HEADER_SIZE  6     /* real size + 1  */
#define T_NBBITS_7k  (NBBITS_7k + HEADER_SIZE)
#define T_NBBITS_9k  (NBBITS_9k + HEADER_SIZE)
#define T_NBBITS_12k (NBBITS_12k + HEADER_SIZE)
#define T_NBBITS_14k (NBBITS_14k + HEADER_SIZE)
#define T_NBBITS_16k (NBBITS_16k + HEADER_SIZE)
#define T_NBBITS_18k (NBBITS_18k + HEADER_SIZE)
#define T_NBBITS_20k (NBBITS_20k + HEADER_SIZE)
#define T_NBBITS_23k (NBBITS_23k + HEADER_SIZE)
#define T_NBBITS_24k (NBBITS_24k + HEADER_SIZE)
#define T_NBBITS_SID (NBBITS_SID + HEADER_SIZE)


typedef struct
{
   Word16 sid_update_counter;   /* Number of frames since last SID */
   Word16 sid_handover_debt;    /* Number of extra SID_UPD frames to schedule */
   Word16 prev_ft;              /* Type of the previous frame */
   void *encoder_state;         /* Points encoder state structure */
} WB_enc_if_state;


extern const Word16 voAMRWBPDecmode_7k[];
extern const Word16 mode_9k[];
extern const Word16 mode_12k[];
extern const Word16 voAMRWBPDecmode_14k[];
extern const Word16 mode_16k[];
extern const Word16 mode_18k[];
extern const Word16 voAMRWBPDecmode_20k[];
extern const Word16 mode_23k[];
extern const Word16 mode_24k[];
extern const Word16 mode_DTX[];

extern const UWord8 block_size[];
extern const Word16 Nb_of_param[];

extern const Word16 Dfh_M7k[];
extern const Word16 voAMRWBPDecDfh_M9k[];
extern const Word16 Dfh_M12k[];
extern const Word16 Dfh_M14k[];
extern const Word16 voAMRWBPDecDfh_M16k[];
extern const Word16 Dfh_M18k[];
extern const Word16 Dfh_M20k[];
extern const Word16 voAMRWBPDecDfh_M23k[];
extern const Word16 Dfh_M24k[];

/* overall table with the parameters of the
   decoder homing frames for all modes */

extern const Word16 *Dhf[10];


/*
 * E_IF_homing_frame_test
 *
 *
 * Parameters:
 *    input_frame I: input speech frame
 *
 * Function:
 *    Check 320 input samples for matching EHF_MASK
 *
 * Returns:
 *    If homing frame
 *//*
Word16 E_IF_homing_frame_test(Word16 input_frame[])
{
   Word16 i, j = 0;
                                        
   for (i = 0; i < L_FRAME16k; i++)
   {
      j = input_frame[i] ^ EHF_MASK;    
                                        
      if (j)                            
      {
         break;
      }
   }                                    
   return (Word16) (!j);
}
*/
/*
 * E_IF_homing_coding
 *
 *
 * Parameters:
 *    parms  I/O: pointer to parameter vector
 *    mode     I: speech mode
 *
 * Function:
 *    Copy decoder homing frame from memory to parameter vector
 *
 * Returns:
 *    void
 *//*
void E_IF_homing_coding(Word16 *parms, Word16 mode)
{
   memcpy(parms, dhf[mode], nb_of_param[mode] * sizeof(Word16));    
}
*/

#ifdef IF2
/*
 * E_IF_if2_conversion
 *
 *
 * Parameters:
 *  mode        I: Mode
 *  param       I: encoder output
 *  stream      O: packed octets (TS26.201)
 *  frame_type  I: TX frame type
 *  dtx         I: speech mode for mode MRDTX
 *
 * Function:
 *  Packing one frame of encoded parameters to AMR-WB IF2
 *
 * Returns:
 *    number of octets
 */
int E_IF_if2_conversion_fx(Word16 mode, Word16 *param, UWord8 *stream,
                               Word16 frame_type, Word16 speech_mode)
{
   Word16 j = 0, i;
   Word16 const *mask;

   for(i = 0; i < block_size[mode]; i++)
   {
      stream[i]  = 0;   move16();  
   }
                                            
   switch(mode)
   {
   case MRNO_DATA:
      *stream = 0xF8;          ;move16();
      j = 8;                             move16();               
      break;

   case MODE_7k:        
      mask = voAMRWBPDecmode_7k;                                  move16();                
      *stream = 0x2;    /* frame_type = 0, fqi = 1  */ move16();                                            
                                                        
      for (j = HEADER_SIZE; j < T_NBBITS_7k; j++)
      {  test();logic16();                                               
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream,0x1);                move16();             
         }
         mask += 2;                                     
         test();logic16();                                               
         if (j & 7)                     
         {
            *stream =shl(*stream, 1);     move16();                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();logic16();                                                  
      while (j & 7)
      {  test();logic16();
         *stream = shl(*stream,1);                                 
         j = add(j,1);                                                
                                                        
      }

      break;

   case MODE_9k:
      mask = mode_9k;             move16();move16();                                
      *stream = 0x6;    /* frame_type = 1, fqi = 1  */
                                                        
      for (j = HEADER_SIZE; j < T_NBBITS_9k; j++)
      {  test();logic16();                                               
         if (param[*mask] & *(mask + 1))                
         {
            *stream = add(*stream,0x1);                             
         }

         mask += 2;                                     
         test();logic16();                                               
         if (j & 7)
         {
            *stream = shl(*stream,1);                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();logic16();                                                  
      while (j & 7)
      {  test();logic16();
         *stream = shl(*stream,1);                                 
         j = add(j,1);                                           
      }

      break;

   case MODE_12k:
      mask = mode_12k;                                  move16();                
      *stream = 0xA;    /* frame_type = 2, fqi = 1  */move16();                
                                                        
      for (j = HEADER_SIZE; j < T_NBBITS_12k; j++)
      {                                                 
         test();logic16();                                               
         if (param[*mask] & *(mask + 1))                
         {
            *stream = add(*stream,0x1);                             
         }

         mask += 2;                                     
         test();logic16();                                               
         if (j & 7)
         {
            *stream = shl(*stream,1);                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();logic16();                                                  
      while (j & 7)
      {  test();logic16();
         *stream = shl(*stream,1);                                 
         j = add(j,1);                                           
      }
      break;

   case MODE_14k:
      mask = voAMRWBPDecmode_14k;                                  move16();                
      *stream = 0xE;    /* frame_type = 3, fqi = 1  */move16();                
                                                        
      for ( j = HEADER_SIZE; j < T_NBBITS_14k; j++ )
      {                                                 
         test();logic16();                                               
         if (param[*mask] & *(mask + 1))                
         {
            *stream = add(*stream,0x1);                             
         }

         mask += 2;                                     
         test();logic16();                                               
         if (j & 7)
         {
            *stream = shl(*stream,1);                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();logic16();                                                  
      while (j & 7)
      {  test();logic16();
         *stream = shl(*stream,1);                                 
         j = add(j,1);                                           
      }

      break;

   case MODE_16k:
      mask = mode_16k;                                  move16();                
      *stream = 0x12;    /* frame_type = 4, fqi = 1  */move16();                
                                                            
      for (j = HEADER_SIZE; j < T_NBBITS_16k; j++)
      {                                                 
         test();logic16();                                               
         if (param[*mask] & *(mask + 1))                
         {
            *stream = add(*stream,0x1);                             
         }

         mask += 2;                                     
         test();logic16();                                               
         if (j & 7)
         {
            *stream = shl(*stream,1);                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();logic16();                                                  
      while (j & 7)
      {  test();logic16();
         *stream = shl(*stream,1);                                 
         j = add(j,1);                                           
      }

      break;

   case MODE_18k:                                       
      mask = mode_18k;move16();                
      *stream = 0x16;    /* frame_type = 5, fqi = 1  */move16();                
                                                        
      for (j = HEADER_SIZE; j < T_NBBITS_18k; j++)
      {                                                 
         test();logic16();                                               
         if (param[*mask] & *(mask + 1))                
         {
            *stream = add(*stream,0x1);                             
         }

         mask += 2;                                     
         test();logic16();                                               
         if (j & 7)
         {
            *stream = shl(*stream,1);                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();logic16();                                                  
      while (j & 7)
      {  test();logic16();
         *stream = shl(*stream,1);                                 
         j = add(j,1);                                           
      }
      break;

   case MODE_20k:
      mask = voAMRWBPDecmode_20k;                                  move16();                
      *stream = 0x1A;    /* frame_type = 6, fqi = 1  */ move16();                
                                                        
      for (j = HEADER_SIZE; j < T_NBBITS_20k; j++)
      {                                                 
         test();logic16();                                               
         if (param[*mask] & *(mask + 1))                
         {
            *stream = add(*stream,0x1);                             
         }

         mask += 2;                                     
         test();logic16();                                               
         if (j & 7)
         {
            *stream = shl(*stream,1);                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();logic16();                                                  
      while (j & 7)
      {  test();logic16();
         *stream = shl(*stream,1);                                 
         j = add(j,1);                                           
      }

      break;

   case MODE_23k:
      mask = mode_23k;                                  move16();                
      *stream = 0x1E;    /* frame_type = 7, fqi = 1  */move16();                
                                                        
      for (j = HEADER_SIZE; j < T_NBBITS_23k; j++)
      {                                                 
         test();logic16();                                               
         if (param[*mask] & *(mask + 1))                
         {
            *stream = add(*stream,0x1);                             
         }

         mask += 2;                                     
         test();logic16();                                               
         if (j & 7)
         {
            *stream = shl(*stream,1);                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();logic16();                                                  
      while (j & 7)
      {  test();logic16();
         *stream = shl(*stream,1);                                 
         j = add(j,1);                                           
      }
      break;

   case MODE_24k:
      mask = mode_24k;                                  move16();                
      *stream = 0x22;    /* frame_type = 8, fqi = 1  */move16();                
                                                        
      for (j = HEADER_SIZE; j < T_NBBITS_24k; j++)
      {                                                 
         test();logic16();                                               
         if (param[*mask] & *(mask + 1))                
         {
            *stream = add(*stream,0x1);                             
         }

         mask += 2;                                     
         test();logic16();                                               
         if (j & 7)
         {
            *stream = shl(*stream,1);                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();logic16();                                                  
      while (j & 7)
      {  test();logic16();
         *stream = shl(*stream,1);                                 
         j = add(j,1);                                           
      }

      break;

   case MRDTX:
      mask = mode_DTX;                                  move16();                
      *stream = 0x26;    /* frame_type = 9, fqi = 1  */move16();                
                                                        
      for ( j = HEADER_SIZE; j < T_NBBITS_SID; j++ )
      {  test();logic16();                                               
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream,0x1);                             
         }

         mask += 2;                            
                                                        
         test();logic16();                                               
         if (j & 7)
         {
            *stream = shl(*stream,1);                              
         }
         else
         {
            stream++;                                   
         }
      }
      test();
      /* sid type */                                    
      if (sub(frame_type,TX_SID_UPDATE) == 0)
      {
         /* sid update */
         *stream = add(*stream,0x1);                                   
      }

      /* speech mode indicator */
      *stream = shl(*stream,4);                                    
      *stream = (UWord8)add(*stream, speech_mode);        
      /* bit stuffing */
      *stream = shl(*stream,3);                                    

      j = 48;                move16();                           

      break;

   default:
      break;

   }
                                                            
   return shr(j,3);
}

#else
//#warning "Need to add FLC!"
/*
 * E_IF_mms_conversion
 *
 *
 * Parameters:
 *  mode        I: Mode
 *  param       I: encoder output
 *  stream      O: packed octets (RFC 3267, section 5.3)
 *  frame_type  I: TX frame type
 *  dtx         I: speech mode for mode MRDTX
 *
 * Function:
 *  Packing one frame of encoded parameters to AMR-WB MMS format
 *
 * Returns:
 *    number of octets
 */
int E_IF_mms_conversion_fx(Word16 mode, Word16 *param, UWord8 *stream,
                               Word16 frame_type, Word16 speech_mode)
{
   Word32 j = 0, i;
   Word16 const *mask;

   for(i = 0; i < block_size[mode]; i++)
   {
      stream[i]  = 0;   move16();  
   }
   
   switch(mode)
   {
   case MRNO_DATA:
      *stream = 0x7C;     move16();
      j = 0;              move16();
      break;

   case MODE_7k:
      mask = voAMRWBPDecmode_7k;     move16();
      *stream = 0x04;    /* frame_type = 0, fqi = 1  */ move16();
	  stream++;

      for (j = 1; j <= NBBITS_7k; j++)
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);      move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);       move16();
         }
         else
         {
            stream++;
         }
      }
      test();logic16();
      while (j & 7)
      {
         *stream = shl(*stream,1);      move16();
         j = add(j,1);
      }

      break;

   case MODE_9k:
      mask = mode_9k;                                     move16();
      *stream = 0x0C;    /* frame_type = 1, fqi = 1  */move16();
	  stream++;

      for (j = 1; j <= NBBITS_9k; j++)
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);      move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);       move16();
         }
         else
         {
            stream++;
         }
      }
      test();logic16();
      while (j & 7)
      {
         *stream = shl(*stream,1);      move16();
         j = add(j,1);
      }

      break;

   case MODE_12k:
      mask = mode_12k;                                      move16();
      *stream = 0x14;    /* frame_type = 2, fqi = 1  */ move16();
	  stream++;

      for (j = 1; j <= NBBITS_12k; j++)
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);      move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);       move16();
         }
         else
         {
            stream++;
         }
      }
      test();logic16();
      while (j & 7)
      {
         *stream = shl(*stream,1);      move16();
         j = add(j,1);
      }
      break;

   case MODE_14k:
      mask = voAMRWBPDecmode_14k;                                  move16();
      *stream = 0x1C;    /* frame_type = 3, fqi = 1  */ move16();
	  stream++;

      for ( j = 1; j <= NBBITS_14k; j++ )
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);      move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);       move16();
         }
         else
         {
            stream++;
         }
      }
      test();logic16();
      while (j & 7)
      {
         *stream = shl(*stream,1);      move16();
         j = add(j,1);
      }

      break;

   case MODE_16k:
      mask = mode_16k;                                  move16();
      *stream = 0x24;    /* frame_type = 4, fqi = 1  */ move16();
	  stream++;

      for (j = 1; j <= NBBITS_16k; j++)
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);      move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);       move16();
         }
         else
         {
            stream++;
         }
      }
      test();logic16();
      while (j & 7)
      {
         *stream = shl(*stream,1);      move16();
         j = add(j,1);
      }

      break;

   case MODE_18k:
      mask = mode_18k;                                  move16();
      *stream = 0x2C;    /* frame_type = 5, fqi = 1  */move16();
	  stream++;

      for (j = 1; j <= NBBITS_18k; j++)
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);      move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);       move16();
         }
         else
         {
            stream++;
         }
      }
      test();logic16();
      while (j & 7)
      {
         *stream = shl(*stream,1);      move16();
         j = add(j,1);
      }

      break;

   case MODE_20k:
      mask = voAMRWBPDecmode_20k;                                  move16();
      *stream = 0x34;    /* frame_type = 6, fqi = 1  */ move16();
	  stream++;

      for (j = 1; j <= NBBITS_20k; j++)
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);      move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);       move16();
         }
         else
         {
            stream++;
         }
      }
      test();logic16();
      while (j & 7)
      {
         *stream = shl(*stream,1);      move16();
         j = add(j,1);
      }
      break;

   case MODE_23k: 
      mask = mode_23k;                                    move16();
      *stream = 0x3C;    /* frame_type = 7, fqi = 1  */   move16();  
	  stream++;

      for (j = 1; j <= NBBITS_23k; j++)
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);      move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);       move16();
         }
         else
         {
            stream++;
         }
      }
      test();logic16();
      while (j & 7)
      {
         *stream = shl(*stream,1);      move16();
         j = add(j,1);
      }

      break;

   case MODE_24k:
      mask = mode_24k;                                  move16();
      *stream = 0x44;    /* frame_type = 8, fqi = 1  */move16();
	  stream++;

      for (j = 1; j <= NBBITS_24k; j++)
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);      move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);       move16();
         }
         else
         {
            stream++;
         }
      }
      test();logic16();
      while (j & 7)
      {
         *stream = shl(*stream,1);      move16();
         j = add(j,1);
      }

      break;

   case MRDTX:
      mask = mode_DTX;                                  move16();
      *stream = 0x4C;    /* frame_type = 9, fqi = 1  */ move16();
	  stream++;

      for ( j = 1; j <= NBBITS_SID; j++ )
      {
         test();logic16();
         if (param[*mask] & *(mask + 1))
         {
            *stream = add(*stream, 0x1);              move16();
         }
         mask += 2;
         test();logic16(); 
         if (j & 7)
         {
            *stream = shl(*stream,1);                 move16();
         }
         else
         {
            stream++;
         }
      }
      test();
      /* sid type */
      if (sub(frame_type,TX_SID_UPDATE) == 0)
      {
         /* sid update */
         *stream = add(*stream, 0x1);                 move16();
      }

      /* speech mode indicator */
      *stream  = shl(*stream, 4);                     move16();
      *stream = (UWord8)add(*stream, speech_mode);    move16();
      j = 40;                                         move16();

      break;

   default:
      break;

   }

   return add(shr(j,3), 1);
}

#endif

/*
 * E_IF_sid_sync_reset
 *
 * Parameters:
 *    st                O: state structure
 *
 * Function:
 *    Initializes state memory
 *
 * Returns:
 *    void
 */
static void E_IF_sid_sync_reset(WB_enc_if_state *st)
{
   st->sid_update_counter = 3;        move16();  
   st->sid_handover_debt = 0;         move16();
   st->prev_ft = TX_SPEECH;           move16();
}

/*
 * E_IF_encode
 *
 *
 * Parameters:
 *    st                I: pointer to state structure
 *    mode              I: Speech Mode
 *    speech            I: Input speech
 *    serial            O: Output octet structure IF2 or 16-bit serial stream
 *    dtx               I: use DTX
 *
 * Function:
 *    Encoding and packing one frame of speech
 *
 * Returns:
 *    number of octets
 */
int E_IF_encode_fx(void *st, Word16 req_mode, Word16 *speech, UWord8 *serial,
                Word16 dtx)
{
   Word16 prms[NB_PARM_MAX], len;
   Word16 i;
   Word16 frame_type, mode, reset_flag;
   WB_enc_if_state *s;
     
   s = (WB_enc_if_state *)st;                               
   mode = req_mode;                                         

   /* check for homing frame */
   reset_flag = encoder_homing_frame_test(speech);             
   test();                                                                 
   if (!reset_flag)
   {                                                        
      for (i = 0; i < L_FRAME16k; i++)   /* Delete the 2 LSBs (14-bit input) */
      {
         speech[i] = (Word16) (speech[i] & 0xfffC);         
      }
                                                            
      coder(&mode, speech, prms, &len, s->encoder_state, dtx);
                                                            
      if (mode == MRDTX)
      {                                             
         s->sid_update_counter--;                           
                                                            
         if (s->prev_ft == TX_SPEECH)
         {
            frame_type = TX_SID_FIRST;                      
            s->sid_update_counter = 3;                      
         }
         else
         {                                                                      
            if ((s->sid_handover_debt > 0) && (s->sid_update_counter > 2))
            {
               /*
                * ensure extra updates are
                * properly delayed after a possible SID_FIRST
                */
               frame_type = TX_SID_UPDATE;                                      
               s->sid_handover_debt--;                                          
            }
            else
            {                                                                   
               if (s->sid_update_counter == 0)
               {
                  frame_type = TX_SID_UPDATE;                                   
                  s->sid_update_counter = 8;                                    
               }
               else
               {
                  frame_type = TX_NO_DATA;                                      
                  mode = MRNO_DATA;
               }
            }                                       
         }
      }
      else
      {
         s->sid_update_counter = 8;                                             
         frame_type = TX_SPEECH;
      }

      s->prev_ft = frame_type;                                                      

   }
   /* perform homing if homing frame was detected at encoder input */
   else
   {
      Reset_encoder(s->encoder_state, 1);                                        
      E_IF_sid_sync_reset(s);                                                   
      Copy(Dhf[mode], prms,  Nb_of_param[mode]); 
      frame_type = TX_SPEECH;                   move16();                                                 
   }

                                                                                
                                                                                
#ifdef IF2
   return E_IF_if2_conversion_fx(mode, prms, serial, frame_type, req_mode);
#else
   return E_IF_mms_conversion_fx(mode, prms, serial, frame_type, req_mode);
#endif
}

/*
 * E_IF_init
 *
 * Parameters:
 *    none
 *
 * Function:
 *    Allocates state memory and initializes state memory
 *
 * Returns:
 *    pointer to encoder interface structure
 */
void *E_IF_init_fx(void)
{
   WB_enc_if_state * s;

   /* allocate memory */
   if ((s = (WB_enc_if_state *)malloc(sizeof(WB_enc_if_state))) == NULL)
   {
      return NULL;
   }

   Init_coder(&(s->encoder_state));
   if (s->encoder_state == NULL)
   {
      free(s);
      return NULL;
   }

   E_IF_sid_sync_reset(s);

   return s;
}

/*
 * E_IF_exit
 *
 * Parameters:
 *    state             I: state structure
 *
 * Function:
 *    The memory used for state memory is freed
 *
 * Returns:
 *    Void
 */
void E_IF_exit_fx(void *state)
{
   WB_enc_if_state *s;
   s = (WB_enc_if_state *)state;

   /* free memory */
   Close_coder(s->encoder_state);
   free(s);
   state = NULL;
}

void Encode_first(Word16 speech16k[], void *spe_state)
{

   /* Float32 */
   Word16 error[M + L_SUBFR];         /* error of quantization                  */
   Word16 code[L_SUBFR];            /* Fixed codebook excitation              */
   Word16 tmp;
   Word16 *new_speech;			       /* Speech vector                          */
   /* Other */
   Coder_State *st;                    /* Coder states                           */

   /* Memory Usage eval */
                                        
   st = (Coder_State *)spe_state;       

   /*
    * Initialize pointers to speech vector.
    *
    *
    *         |-------|-------|-------|-------|-------|-------|
    *          past sp   sf1     sf2     sf3     sf4    L_NEXT
    *         <-------  Total speech buffer (L_TOTAL)   ------>
    *   old_speech
    *         <-------  LPC analysis window (L_WINDOW)  ------>
    *                 <-- present frame (L_FRAME) ---->
    *                 |       <----- new speech (L_FRAME) ---->
    *                 |       |
    *               speech    |
    *                      new_speech
    */      

                                        
   new_speech = st->old_speech + L_TOTAL - 2*L_FRAME - L_FILT + 460;

   Init_HP50_12k8(st->mem_decim);                                                                      
   /*
    * Down sampling signal from 16kHz to 12.8kHz
    */
   Decim_12k8(speech16k, L_FRAME16k, new_speech, st->mem_decim);   

   /* decimate with zero-padding to avoid delay of filter */
   Copy(st->mem_decim, code, 2 * L_FILT16k);          
   Set_zero(error, L_FILT16k);                           

   Decim_12k8(error, L_FILT16k, new_speech + L_FRAME, code);       

   /*
    * Perform 50Hz HP filtering of input signal.
    * Perform fixed preemphasis through 1 - g z^-1
    */
   HP50_12k8(new_speech, L_FRAME, st->mem_sig_in);                   

   Copy(st->mem_sig_in, code,  6);                   

   HP50_12k8(new_speech + L_FRAME, L_FILT, code);                  

   Preemph(new_speech, PREEMPH_FAC, L_FRAME, &(st->mem_preemph));  
   /* last L_FILT samples for autocorrelation window */
   tmp = st->mem_preemph;            move16();                                     
   Preemph(new_speech + L_FRAME, PREEMPH_FAC, L_FILT, &tmp);     

}


/*
 *
 */
void E_IF_encode_first_fx(void *st, Word16 *speech)
{
   Word16 i;
   WB_enc_if_state *s;

   s = (WB_enc_if_state *)st;                               

   for (i = 0; i < L_FRAME16k; i++)   /* Delete the 2 LSBs (14-bit input) */
   {
     speech[i] = (Word16) (speech[i] & 0xfffC);         
   }
                                                          
   Encode_first(speech, s->encoder_state);

}
