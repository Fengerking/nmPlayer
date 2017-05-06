/**********************************************************************/
/* QCELP Variable Rate Speech Codec - Simulation of TIA IS96-A, service */
/*     option one for TIA IS95, North American Wideband CDMA Digital  */
/*     Cellular Telephony.                                            */
/*                                                                    */
/* (C) Copyright 1993, QUALCOMM Incorporated                          */
/* QUALCOMM Incorporated                                              */
/* 10555 Sorrento Valley Road                                         */
/* San Diego, CA 92121                                                */
/*                                                                    */
/* Note:  Reproduction and use of this software for the design and    */
/*     development of North American Wideband CDMA Digital            */
/*     Cellular Telephony Standards is authorized by                  */
/*     QUALCOMM Incorporated.  QUALCOMM Incorporated does not         */
/*     authorize the use of this software for any other purpose.      */
/*                                                                    */
/*     The availability of this software does not provide any license */
/*     by implication, estoppel, or otherwise under any patent rights */
/*     of QUALCOMM Incorporated or others covering any use of the     */
/*     contents herein.                                               */
/*                                                                    */
/*     Any copies of this software or derivative works must include   */
/*     this and all other proprietary notices.                        */
/**********************************************************************/
/* pack.c - pack various parameters into the qc8_packet */

/*****************************************************************************
* File:           pack.cpp
*
* Version:        DSPSE Release    2.0
*
* Description:    pack various parameters into the qc13_packet
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include"qcelp.h"
#include"qcelp13.h"

//#include"qc13_deb.h"

extern Int16  CBSF   [NUMRATES];
extern Int16  PITCHSF[NUMRATES];

extern Int16 NUMBITS[NUMRATES];
extern Int16 QC13_BIT_DATA3[54][3];
extern Int16 QC13_BIT_DATA2[266][3];
extern Int16 QC13_BIT_DATA1[124][3]; /* Rate 1/2 */
extern Int16 QC13_BIT_DATA0[20][3];

//extern Int16 erasure_count;
//extern Int16 frame_num;

extern Int16 LSPVQ0_16[][2];

extern Int16 LSPVQ1_16[][2];

extern Int16 LSPVQ2_16[][2];

extern Int16 LSPVQ3_16[][2];

extern Int16 LSPVQ4_16[][2];

//extern Float FRL_B_SAT[5];



Void  unpack_cb13(QC13_PACKET *qc13_packet,  CBPARAMS *cb_params, Int16 cbsf_number)
{
    cb_params->qcode_G = qc13_packet->G[cbsf_number];

    cb_params->qcode_Gsign = qc13_packet->Gsign[cbsf_number];

    if (cb_params->qcode_Gsign == POSITIVE)
        cb_params->qcode_i = qc13_packet->i[cbsf_number];
    else
        cb_params->qcode_i = (Int16)((qc13_packet->i[cbsf_number] + CBLENGTH - 89) % CBLENGTH);

}

Void  unpack_frame13( QC13_PACKET *qc13_packet)
{
    Int16 i, j;
    Int16 *data_ptr = NULL;
    Int16 bit[QC13_WORDS_PER_PACKET * 16];
    Int16 cnt;
    Int16 frame_byte_num;
    Char char_temp;

    qc13_packet->rate = qc13_packet->data[0];

    switch (qc13_packet->rate)
    {
        case 4 :                       /* rate 1                              */
            data_ptr = &QC13_BIT_DATA2[0][0];
            break;
        case 3 :                       /* rate 1/2                            */
            data_ptr = &QC13_BIT_DATA1[0][0];
            break;
        case 2 :                       /*  rate 1/4                           */
            data_ptr = &QC13_BIT_DATA3[0][0];
            break;
        case 1 :                       /* rate 1/8                            */
            data_ptr = &QC13_BIT_DATA0[0][0];
            break;
        case 0 :                       /* BLANK                               */
            break;
//        case 14 :                      /* ERASURE                             */
//            break;
        default :                      /* ERASURE                             */
            break;
    }


    if (qc13_packet->rate == EIGHTH)
        qc13_packet->sd_dec = (unsigned char)qc13_packet->data[2] | ((unsigned char)qc13_packet->data[1] << 8);

    if (qc13_packet->rate != ERASURE)
    {
        frame_byte_num = QC13_byte_num_per_frame[qc13_packet->rate];
        cnt = 0;
        for (i = 1; i < frame_byte_num; i++)
        {
            char_temp = qc13_packet->data[i];
            for (j = 8; j > 0; j--)
            {
                bit[cnt] = (char_temp & 0x80) ? 1 : 0;
                char_temp <<= 1;
                cnt++;
            }
        }
        clear_qc13_packet_params(qc13_packet);
        if (qc13_packet->rate != BLANK)
        {
            for (i = 0; i < NUMBITS[qc13_packet->rate + QC13_RATE_OFFSET]; i++)
            {
                putbit13(qc13_packet, data_ptr[i * 3], data_ptr[i * 3 + 1], data_ptr[i * 3 + 2], bit[i]);
            }
        }

        /* begin erasure checking */
        if (qc13_packet->rate != EIGHTH)
        {
            if (lsp_erasure_check(qc13_packet->rate, qc13_packet->lpc))
            {
            }
        }
        if (qc13_packet->rate == QUARTER)
        {
            if (G_erasure_check(qc13_packet->G))
            {
 //               erasure_count++;
                qc13_packet->rate = ERASURE;
            }
        }
        /* end erasure checking */
    }
}                                      /* end of unpack_frame()               */

Void  clear_qc13_packet_params(QC13_PACKET *packet)
{
    Int16 i;

    for (i = 0; i < LPCORDER; i++)
    {
        packet->lpc[i] = 0;
    }

    for (i = 0; i < QC13_MAX_SF; i++)
    {
        packet->b[i] = 0;
        packet->lag[i] = 0;
        packet->frac[i] = 0;
        packet->G[i] = 0;
        packet->i[i] = 0;
        packet->Gsign[i] = 0;
    }
}                                      /* end of clear_packet_params()        */

Void  putbit13(QC13_PACKET *packet,Int16 type,Int16 number,Int16 loc, Int16 bit)
{

    if (bit != 0)
    {
        switch (type)
        {
            case QC13_LSPs :
                packet->lpc[number] = (Int16)(packet->lpc[number] | (1 << loc));
                break;
            case QC13_PGAIN :
                packet->b[number] = (Int16)(packet->b[number] | (1 << loc));
                break;
            case QC13_PLAG :
                packet->lag[number] = (Int16)(packet->lag[number] | (1 << loc));
                break;
            case QC13_PFRAC :
                packet->frac[number] = (Int16)(packet->frac[number] | (1 << loc));
                break;
            case QC13_CBGAIN :
                packet->G[number] = (Int16)(packet->G[number] | (1 << loc));
                break;
            case QC13_LSPVs :
                packet->lpc[number] = (Int16)(packet->lpc[number] | (1 << loc));
                break;
            case QC13_CBSIGN :
                packet->Gsign[number] = (Int16)(packet->Gsign[number] | (1 << loc));
                break;
            case QC13_CBINDEX :
                packet->i[number] = (Int16)(packet->i[number] | (1 << loc));
                break;
            case QC13_CBSEED :
                /*packet->sd=packet->sd|(1<<loc);*/ /* never used in coder */
                /* only for debugging  */
                break;
            case QC13_RESERVE :
                packet->rate = ERASURE;
                break;
        }
    }

}                                      /* end of putbit()                     */

Int16  getbit13(QC13_PACKET * QC13_packet, Int16 type, Int16 number, Int16 loc)
{

    switch (type)
    {
        case QC13_LSPs :
            return (truefalse(QC13_packet->lpc[number], loc));
        case QC13_PGAIN :
            return (truefalse(QC13_packet->b[number], loc));
        case QC13_PLAG :
            return (truefalse(QC13_packet->lag[number], loc));
        case QC13_PFRAC :
            return (truefalse(QC13_packet->frac[number], loc));
        case QC13_LSPVs :
            return (truefalse(QC13_packet->lpc[number], loc));
        case QC13_CBGAIN :
            return (truefalse(QC13_packet->G[number], loc));
        case QC13_CBSIGN :
            return (truefalse(QC13_packet->Gsign[number], loc));
        case QC13_CBINDEX :
            return (truefalse(QC13_packet->i[number], loc));
        case QC13_CBSEED :
            return (truefalse(QC13_packet->sd_enc, loc));
        case QC13_RESERVE :
            return (0);
        default :
            return(0);
//            fprintf(stderr, "getbit13: Unrecognized Packing field\n");
//            exit(-2);
    }
}                                      /* end of getbit13()                     */

Int16  lsp_erasure_check(Int16 rate, Int16 *qcode)
{
    Int16  i;
    Int16 *VQ=NULL;
    Int16  qlsp[LPCORDER];
    Int32  ar_temp;

//    for (i = 0; i < LPCORDER / 2; i++)
    for (i = 0; i < (LPCORDER>>1); i++)
    {
        switch (i)
        {
            case 0 :
                VQ = &LSPVQ0_16[0][0];
                break;
            case 1 :
                VQ = &LSPVQ1_16[0][0];
                break;
            case 2 :
                VQ = &LSPVQ2_16[0][0];
                break;
            case 3 :
                VQ = &LSPVQ3_16[0][0];
                break;
            case 4 :
                VQ = &LSPVQ4_16[0][0];
                break;
        }
        if (i == 0)
        {
            qlsp[0] = VQ[2 * qcode[i]];
        }
        else
        {
            qlsp[2 * i] = (Int16)(qlsp[2 * i - 1] + VQ[2 * qcode[i]]);
        }
        qlsp[2 * i + 1] = (Int16)(qlsp[2 * i] + VQ[2 * qcode[i] +1]);
    }

    if (rate == FULL || rate == HALF)
    {
        if (qlsp[9] <= 10813 || qlsp[9] >= 16138)
        {
            return (1);
        }
        for (i = 4; i < 10; i++)
        {
            ar_temp = (Int32)qlsp[i];
            ar_temp -= qlsp[i - 4];
            if (ar_temp < 0) ar_temp = -ar_temp;
            if (ar_temp < 1525l)
            {
                return (1);
            }
        }
    }
    else if (rate == QUARTER)
    {
        if (qlsp[9] <= 11468 || qlsp[9] >= 15892)
        {
            return (1);
        }
        for (i = 3; i < 10; i++)
        {
            ar_temp = qlsp[i];
            ar_temp -= qlsp[i - 3];
            if (ar_temp < 0) ar_temp = -ar_temp;
            if (ar_temp < 1311l)
            {
                return (1);
            }
        }
    }

    return (0);                        /* Do Not Erase Packet                 */
}

Int16  G_erasure_check(Int16 *qcode_G)
{
    Int16 j;

    for (j=0; j < 4; j++)
    {
        if (abs((Int16)(qcode_G[j + 1] - qcode_G[j])) > 10)
            return(1);
    }

    for (j=0; j < 3; j++)
    {
        if (abs((Int16)(qcode_G[j + 2] - qcode_G[j])) > 12)
            return(1);
    }
    return (0);
}

#ifndef TRUEFALSE_MACRO
Int16  truefalse(Int16 word, Int16 bitloc)
{
    if ((word & (1 << bitloc)) == 0)
    {
        return (0);
    }
    else
    {
        return (1);
    }
}
#endif

