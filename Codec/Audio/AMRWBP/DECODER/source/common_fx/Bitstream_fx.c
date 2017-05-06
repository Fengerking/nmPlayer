//#include <stdio.h>
//#include <stdlib.h>
#include "amr_plus_fx.h"
#include "typedef.h"

extern const UWord8 voAMRWBPDecBlock_size[];
#define NO_DATA  -3
/*-------------------------------------------------------------------------------------*
 * Funtion  WriteHeader()                                 
 * ~~~~~~~~~~~~~~~~~~                                            
 * Write AMR-WB and AMR-WB+ header according to payload selected (see 3gpp ts26.304)
 *
 *                            AMR-WB+ 
 * Raw File Format (FRAW)
 *  | M.Index (7 bits) | T.F. Index (2 bits) | I.S.F Index (5 bits) | AMR-WB+ bitstream...
 *
 *                            AMR-WB 
 *
 *  | Mono Rate (8 bits) | .........AMR-WB bitstream in IF2 Format (without IF2 Header)...
 *--------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------*
 * Funtion  WriteBitstreamPlus()                                 
 * ~~~~~~~~~~~~~~~~~~                                            
 * Write AMR-WB+ bitstream 
 *-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*
 * Funtion  WriteBitstream()                                 
 * ~~~~~~~~~~~~~~~~~~                                            
 * Write AMR-WB bitstream 
 *-----------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------*
 * Funtion  ReadHeader()                                 
 * ~~~~~~~~~~~~~~~~~~                                            
 * Read AMR-WB and AMR-WB+ header according to payload selected (see 3gpp ts26.304)
 *
 *                            AMR-WB+ 
 * Raw File Format (FRAW)
 *  | M.Index (7 bits) | T.F. Index (2 bits) | I.S.F Index (5 bits) | AMR-WB+ bitstream...
 *
 *                            AMR-WB 
 *
 *  | Mono Rate (8 bits) | .........AMR-WB bitstream in IF2 Format (without IF2 Header)...
 *--------------------------------------------------------------------------------------*/

Word16 ReadHeader_buffer(
Word16 *tfi,
Word16 *bfi,
Word16 *extension,
Word16 *mode,
Word16 *st_mode,
Word16 *fst,
UWord8 *buffer_in
)
{
    Word16 mode_index, fst_index, index, nb_read = 0;
    UWord8 byte;
    
    //  nb_read += fread(&byte, sizeof(unsigned char), 1, f_serial);
    //  if(nb_read ==0)
    //    return nb_read;
    byte = *buffer_in++;
    nb_read++;
    
    mode_index = (byte & 127);
    
    //If frame ereased : don't change old conf just modify mode
    if( mode_index > 47 || mode_index < 0 ||        /* mode unknown */
    mode_index == 14 || mode_index == 15 ||     /* Frame lost or ereased */
    (mode_index == 9 && *extension == 1))       /* WB SID in WB+ frame  not supported case so declare a NO_DATA*/    
    {
        //nb_read += fread(&byte, sizeof(unsigned char), 1, f_serial); /* read one more byte to ensure empty header */
        byte = *buffer_in++;
        nb_read++;
        
        *tfi = (byte & 0xc0)>>6;   /* tfi extrapolated by RTP packetizer */
        fst_index = (byte & 0x1F);    
        *fst = isfIndex_fx[fst_index];
        
        if(mode_index == 14)  /* frame lost WB or WB+*/
        {
            bfi[*tfi] = 1;
        }
        else if (mode_index == 15)
        {
            bfi[*tfi] = 0;    /* DTX in WB reset BFI vector */ 
        }
        *mode  = mode_index;
        return NO_DATA;         /* There is no more data to read */
    }
    
    *st_mode  = -1;
    
    if(mode_index >15)      /* wb+ */
    {
        if(mode_index < 24)    /* Mono mode only */
        {
            *mode =   mode_index - 16;
        }
        else
        {
            index = mode_index - 24;
            *mode = miMode_fx[2*index];
            *st_mode = miMode_fx[2*index+1];
        }
        *extension = 1;
        //nb_read += fread(&byte, sizeof(unsigned char), 1, f_serial);
        byte = *buffer_in++;
        nb_read++;
        *tfi = (byte & 0xc0)>>6;
        fst_index = (byte & 0x1F);    
        if(fst_index < 1)
        fst_index = 1;        /* prevent isf < 0.5 */
        *fst = isfIndex_fx[fst_index];
    }
    else      /* WB and caracterize WB+*/
    {
        if(mode_index == 10)
        {
            *extension = 1;
            *mode = 2;        /* 14m */        
        }
        else if (mode_index == 11)
        {
            *extension = 1;
            *mode = 2;        /* 18s */
            *st_mode = 6;
        }
        else if (mode_index == 12)
        {
            *extension = 1;
            *mode = 7;        /* 24m */
        }
        else if (mode_index == 13)
        {
            *extension = 1;
            *mode = 5;        /* 24s */
            *st_mode = 7;
        }
        else
        {
            *extension = 0;
            *mode =   mode_index;
        }
        //nb_read += fread(&byte, sizeof(unsigned char), 1, f_serial);
        byte = *buffer_in++;
        nb_read++;
        *tfi = (byte & 0xc0)>>6;
        fst_index = (byte & 0x1F);    
        //if(fst_index != 0 && fst_index != 8)
        //{
        //    //AMRWBPLUS_FPRINTF( stderr, "Internal Sampling Frequency not supported with AMW WB and caracterized WB+ modes " );
        //    exit(0);
        //}
        *fst = isfIndex_fx[fst_index];
    }
    bfi[*tfi] = 0;  /* Good frame */  
    
    return nb_read;
}

/*-----------------------------------------------------------------*
 * Funtion  ReadBitstreamPlus_buffer()                                 
 * ~~~~~~~~~~~~~~~~~~                                            
 * Read AMR-WB+ bitstream 
 *-----------------------------------------------------------------*/
Word16 ReadBitstreamPlus_buffer(
Word16 nb_bits,
Word16 nb_byte,
Word16 *serial,
UWord8 *buffer_in,
Word16 offset
)
{
    UWord8 byte;
    Word16 j, k, n, *ptr;
    UWord8 *buffer_cur = buffer_in;
    
    ptr = &serial[offset * (nb_bits / 4)];
    n = 0;
    for (j = 0; j < nb_byte; j++)
    {
        //n += fread(&byte, sizeof(unsigned char), 1, f_serial);
        byte = *buffer_cur++;
        n++;

        for (k = 0; k < 8; k++, ptr++)
        {
            *ptr = (byte & (short) 128) == (short) 128;
            byte <<= 1;
        }
    }
    
    return n;
}


/*-----------------------------------------------------------------*
 * Funtion  ReadBitstream_buffer()                                 
 * ~~~~~~~~~~~~~~~~~~                                            
 * Read AMR-WB bitstream + creation of IF2 Header
 *-----------------------------------------------------------------*/
Word16 ReadBitstream_buffer(
Word16 nb_byte,
UWord8 *serialAmrwb,
Word16 mode,
UWord8 *buffer_in
)
{
    unsigned char *ptc_serial;
    unsigned char *ptc_serial_new;
    unsigned char ctemp;
    short i;
    
    ptc_serial = buffer_in + nb_byte - 2;
    ptc_serial_new = serialAmrwb + nb_byte - 1;
    for (i = nb_byte - 1; i > 0; --i)
    {
        *ptc_serial_new = *ptc_serial;
        ptc_serial--;
        ptc_serial_new--;
    }
    /* add IF2 Header */
    ctemp = (unsigned char)(1<<2);               /* Add FQI        */ 
    ctemp +=  (unsigned char)(mode <<3);         /* Add Frame Type */     
    
    *ptc_serial_new = ctemp;
    
    return (nb_byte - 1);
}


Word16 Read_buffer(
Word16 *tfi,
Word16 *bfi,
DecoderConfig *conf,
Word16 *extension,
Word16 *mode,
Word16 *st_mode,
Word16 *fst,
UWord8 *buffer_in,
void   *serial
)
{
    Word16 i, n = 0, nb_bits, nb_byte, *pt_serial, *ptr;
    UWord8 *buffer_cur = NULL;
    Word16 counter = 0;
    
	buffer_cur = buffer_in;
    pt_serial = (Word16 *) serial;
	
    n = ReadHeader_buffer(tfi, bfi, extension, mode, st_mode, fst, (buffer_cur + counter));
	counter += n;
	for (i = 0; i < 4; i++)
    {
        //n = ReadHeader_buffer(tfi, bfi, extension, mode, st_mode, fst, (buffer_cur + counter));
		//counter += n;
		if(!n) break;
        //assume there is no amrwb -> wb+ switching inside superframe
        if (n != NO_DATA)
        {   
            //update mode and st_mode only if mode_index != (14 ||15)
            conf->mode = *mode;           
            conf->st_mode = *st_mode;
            if(*extension >0)
            {
                nb_bits = get_nb_bits(*extension, *mode, *st_mode);
                ptr = &pt_serial[i * (nb_bits / 4)];
                
                nb_byte = ((nb_bits / 4) + 7 ) / 8;
                //n = ReadBitstreamPlus_buffer(nb_bits, nb_byte, pt_serial, buffer_in, i);
                n = ReadBitstreamPlus_buffer(nb_bits, nb_byte, pt_serial, (buffer_cur + counter), i);
                counter += n;
                if (n != nb_byte)
                {
                    break;
                }
            }
            else
            {
                nb_byte = voAMRWBPDecBlock_size[*mode];
                n = ReadBitstream_buffer(nb_byte,(unsigned char*) serial, *mode, (buffer_cur + counter));
                counter += n;
                break;
            }
        }
        else if (*extension == 0)
        {
            if (*mode == 15)
            {   //DTX FRAME (NO DATA)
                ((unsigned char*)serial)[0] = 0x7C; //need in AMR WB
            }
            else if (*mode == 14)
            {   //Frame lost
                ((unsigned char*)serial)[0] = 0x74; //need in AMR WB
            }
            break;
        }
    }
    
    return counter;
}
//end

