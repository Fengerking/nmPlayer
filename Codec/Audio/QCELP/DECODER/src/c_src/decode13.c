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
/* decode.c - main QCELP decoder */

/*****************************************************************************
* File:           decode13.cpp
*
* Version:        DSPSE Release    2.0
*
* Description:    main QCELP13 decoder
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

#include"qcelp.h"
#include"qcelp13.h"

extern Int16 CBSF   [NUMRATES];
extern Int16 PITCHSF[NUMRATES];
extern Int16 GA_16[73];
extern Int16 ONE_DIV_EIGHT[9];

extern Int16 ERA_B_SAT_16[4];
extern Int16 ONE_DIV_PITCHSF_ERASURE[5];

// QC13_DObj	     Decode;

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  QC13_Decode(QC13_PACKET *qc13_packet, I16 *out_speech)
int  QC13_Decode(QCP13_DecObj *qc13_dec, Int16 *out_speech)
{
	Int16 rate, i, k, length;
	Int16 outvect[FSIZE];
	Int16 * outptr;
	int out;
	QC13_PACKET *qc13_packet;
	int  result;

	qc13_packet = &qc13_dec->q13_packet;

	unpack_frame13(qc13_packet);
	rate = qc13_packet->rate + QC13_RATE_OFFSET;

	switch(rate - QC13_RATE_OFFSET)
	{
	case BLANK:
		QC13_DecBlank(qc13_dec, rate, outvect);
		length=FSIZE;
		outptr = out_speech;
		for (k = 0; k < length; k++)
		{
			out = outvect[k] << 2;
			*outptr++ = saturate(out);
		}
		break;
	case ERASURE:
		QC13_DecErase(qc13_dec, rate, outvect);
		length=FSIZE;
		outptr = out_speech;
		for (k = 0; k < length; k++)
		{
			out = outvect[k] << 2;
			*outptr++ = saturate(out);
		}
		break;
	case EIGHTH:
		if (0 != QC13_DecEighth(qc13_dec, rate, outvect))
		{
			return 1;
		}
		length=FSIZE;
		outptr = out_speech;
		for (k = 0; k < length; k++)
		{
			out = outvect[k] << 2;
			*outptr++ = saturate(out);
		}
		break;

	case QUARTER:
		QC13_DecQuarter(qc13_dec, rate, outvect);
		length=FSIZE;
		outptr = out_speech;
		for (k = 0; k < length; k++)
		{
			out = outvect[k] << 2;
			*outptr++ = saturate(out);
		}
		break;

	default:
		length=(Int16)(FSIZE / PITCHSF[rate]);
		outptr = out_speech;
		for (i = 0; i < PITCHSF[rate]; i++)
		{
			result = QC13_DecPSF(qc13_dec, i, rate, outvect);
			if (0 != result)
			{
				return 1;
			}

			for (k = 0; k < length; k++)
			{
				out = outvect[k] << 2;
				*outptr++ = saturate(out);
			}
		}
		break;
	}

	return VO_ERR_NONE;
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  QC13_DecBlank(Int16 rate, I16 *outvect)
Void  QC13_DecBlank(QCP13_DecObj *qc13_dec, Int16 rate, Int16 *outvect)
{
	Int16  j, k;
	Int16 length;
	Int16 agc_flag=0;
	Int16 *Pvect;
	Int32 energy;
	Int16 tmp_qlsp[LPCORDER];
	QC13_DObj* Decode;
	Int32 ar;

	PITCHPARAMS pitch_params;
	PITCHPARAMS pitch_prefilt_params;
	CBPARAMS cb_params;

	Decode = &qc13_dec->Decoder;

	cb_params.G_16 = 0;
	cb_params.i = 0;               /* arbitrary                           */
	pitch_params.b_16 = Decode->last_b_16;
	pitch_params.frac = Decode->last_frac;

	if (pitch_params.b_16 > 16384)
	{
		pitch_params.b_16 = 16384;
	}

	pitch_params.lag = Decode->last_lag;

	for (k = 0; k < LPCORDER; k++)
	{
		Decode->qlsp_16[k] = Decode->last_qlsp_16[k];
		tmp_qlsp[k] = Decode->qlsp_16[k];
	}

	//    lsp2lpc(tmp_qlsp, Decode.lpc_16[0][NOT_WGHTED], LPCORDER);
	lsp2lpc(tmp_qlsp, (Decode->lpc_16[0][NOT_WGHTED]), LPCORDER);

	//    wght_lpcs(Decode.lpc_16[0][NOT_WGHTED], BWE_FACTOR_16);
	wght_lpcs(Decode->lpc_16[0][NOT_WGHTED], BWE_FACTOR_16);

	/*******************************************************************/
	Pvect = outvect;
	length = (Int16)(FSIZE/QC13_PITCHSF_BLANK);
	for (j = 0; j < QC13_PITCHSF_BLANK; j++)
	{
		make_qc13_full_cw(Pvect, length, &cb_params);  // Full rate voiced

		pitch_filter_13(Decode->adptv_cbk_16, &pitch_params, Pvect, length);

		update_acbk_16(Decode->adptv_cbk_16, Pvect, length);

		if (Decode->pitch_post_flag == YES)
		{
			if (pitch_params.b_16 > 16384)
				pitch_prefilt_params.b_16 = 16384;
			else
				pitch_prefilt_params.b_16 = pitch_params.b_16;

			ar = (Int32)pitch_prefilt_params.b_16 * QC13_SCALE_B_16 << 1; // Q15 * Q14 = Q30
			ar = L_add(ar, 0x00008000l);
			pitch_prefilt_params.b_16 = (Int16)(ar >> 16);

			pitch_prefilt_params.frac = pitch_params.frac;
			pitch_prefilt_params.lag = pitch_params.lag;

			ar = 0l;
			for (k = 0; k < length; k++)
			{
				//                ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k]); /* Q15 * Q15 = Q31 (with left shift) */
				ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k] >> 2); 
			}
			//            ar = L_shl(ar, 1);
			//            ar = L_add(ar, 0x41); // rounding
			//            ar >>= 3; // bring down to Q28
			//ar = L_add(ar, 0x2); // rounding
			//ar += 0x2; // rounding
			//ar >>= 2; // bring down to Q28

			energy = ar;

			pitch_filter_13(Decode->prefilt_acbk_16, &pitch_prefilt_params, Pvect, length);

			update_acbk_16(Decode->prefilt_acbk_16, Pvect, length);

			agc_prefilter(energy, Pvect, length);
		}

#if DEC_DP_SYNTH_FILTER  /* double precision synthesis filter */
		dp_polefilt(Pvect, Decode->lpc_16[0][NOT_WGHTED], Decode->ring_pole_hist_32, length);
#else
		polefilt(Pvect, Decode->lpc_16[0][NOT_WGHTED], Decode->ring_pole_hist_16, length);
#endif //DEC_DP_SYNTH_FILTER

		Pvect += length;
	}
	/*******************************************************************/

	Pvect = outvect;
	length=FSIZE / AGCRATE;
	for (j = 0; j < AGCRATE; j++)
	{
		if (Decode->pf_flag == PF_ON)
		{
			energy = 0l;
			agc_flag = NO;
			ar = 0l;
			for (k = 0; k < length; k++)
			{
				//            ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k]); /* Q15 * Q15 = Q31 (with left shift) */
				ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k]); /* Q15 * Q15 = Q31 (with left shift) */
			}

			if (ar < 0x00800000l) // 0.0078125 in Q31
			{
				energy = ar << 1;
				agc_flag = YES;
			}
			else
			{
				energy = ar >> 6;
			} 
			Decode->bright_hist_16=postfilt((Int16)QC13, Pvect, Decode->lpc_16[0][NOT_WGHTED],
				Decode->pf_hist_16, Decode->bright_16[0], Decode->bright_hist_16, length);

			agc(energy, Pvect, length, &(Decode->agc_factor_16), agc_flag);
		}

		Pvect += length;
	}

	/* Update parameters for next frame */
	for (k = 0; k < LPCORDER; k++)
		Decode->last_qlsp_16[k] = Decode->qlsp_16[k];

	Decode->last_b_16 = pitch_params.b_16;
	Decode->last_lag = pitch_params.lag;
	Decode->last_frac = pitch_params.frac;
	Decode->last_G_8th_rate_16 = cb_params.G_16;
	Decode->last_rate = rate;
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  QC13_DecErase(Int16 rate, I16 *outvect)
Void  QC13_DecErase(QCP13_DecObj *qc13_dec, Int16 rate, Int16 *outvect)
{
	Int16  i, j, k;
	Int16 length;
	Int16 agc_flag;
	Int16 *Pvect;
	Int32 energy;
	Int16 tmp_qlsp[LPCORDER];
	Int16 interp_G[QC13_PITCHSF_ERASURE];
	Int32 ar;
	QC13_DObj* Decode;
	PITCHPARAMS pitch_params;
	PITCHPARAMS pitch_prefilt_params;
	CBPARAMS cb_params;

	Decode = &qc13_dec->Decoder;

	gen_qlsp(Decode->qlsp_16, Decode->lsp_pred_16, Decode->err_cnt, Decode->last_qlsp_16);

	for (i = 0; i < LPCORDER; i++)
		tmp_qlsp[i] = Decode->qlsp_16[i];

	lsp2lpc(tmp_qlsp, Decode->lpc_16[0][NOT_WGHTED], LPCORDER);

	/* Bandwidth Expansion */
	wght_lpcs(Decode->lpc_16[0][NOT_WGHTED], (Int16)BWE_FACTOR_16);

	gen_params(Decode, &pitch_params, &cb_params, interp_G);

	/*******************************************************************/
	Pvect = outvect;
	length = (Int16)(FSIZE/QC13_PITCHSF_ERASURE);
	for (j = 0; j < QC13_PITCHSF_ERASURE; j++)
	{
		cb_params.G_16 = interp_G[j];

		make_qc13_full_cw(Pvect, length, &cb_params);  // Full rate voiced

		cb_params.i = (Int16)((cb_params.i - 40) & 0x7f);

		pitch_filter_13(Decode->adptv_cbk_16, &pitch_params, Pvect, length);

		update_acbk_16(Decode->adptv_cbk_16, Pvect, length);

		if (Decode->pitch_post_flag == YES)
		{
			if (pitch_params.b_16 > 16384)
				pitch_prefilt_params.b_16 = 16384;
			else
				pitch_prefilt_params.b_16 = pitch_params.b_16;

			ar = (Int32)pitch_prefilt_params.b_16 * (Int32)QC13_SCALE_B_16 << 1; // Q15 * Q14 = Q30
			ar = L_add(ar, 0x00008000);
			pitch_prefilt_params.b_16 = (Int16)(ar >> 16);

			pitch_prefilt_params.frac = pitch_params.frac;
			pitch_prefilt_params.lag = pitch_params.lag;

			ar = 0l;
			for (k = 0; k < length; k++)
			{
				//can't be modified--shrchen
				ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k] >> 2); /* Q15 * Q15 = Q31 (with left shift) */
			}
			//            ar = L_shl(ar, 1);            
			//            ar = L_add(ar, 0x4l); // rounding
			//            ar >>= 3; // bring down to Q28
			//ar = L_add(ar, 0x2); // rounding    //can't be modified--shrchen
			//ar >>= 2; // bring down to Q28
			energy = (Int32)ar;

			pitch_filter_13(Decode->prefilt_acbk_16, &pitch_prefilt_params, Pvect, length);

			update_acbk_16(Decode->prefilt_acbk_16, Pvect, length);

			agc_prefilter(energy, Pvect, length);
		}

#if DEC_DP_SYNTH_FILTER  /* double precision synthesis filter */
		dp_polefilt(Pvect, Decode->lpc_16[0][NOT_WGHTED], Decode->ring_pole_hist_32, length);
#else
		polefilt(Pvect, Decode->lpc_16[0][NOT_WGHTED], Decode->ring_pole_hist_16, length);
#endif //DEC_DP_SYNTH_FILTER

		Pvect += length;
	}
	/*******************************************************************/

	Pvect = outvect;
	length=FSIZE / AGCRATE;
	for (j = 0; j < AGCRATE; j++)
	{
		if (Decode->pf_flag == PF_ON)
		{
			energy = 0l;
			agc_flag = NO;
			ar = 0l;
			for (k = 0; k < length; k++)
			{
				//can't be modified--shrchen
				ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k]); /* Q15 * Q15 = Q31 (with left shift) */
			}
			//ar = L_shl(ar, 1);

			if (ar < 0x00800000l) // 0.0078125 in Q31
			{
				energy = ar << 1;
				agc_flag = YES;
			}
			else
			{
				energy = ar >> 6;
			}

			Decode->bright_hist_16=postfilt(QC13, Pvect, Decode->lpc_16[0][NOT_WGHTED],
				Decode->pf_hist_16, Decode->bright_16[0], Decode->bright_hist_16, length);

			agc(energy, Pvect, length, &(Decode->agc_factor_16), agc_flag);
		}

		Pvect += length;
	}

	Decode->err_cnt += (Int16) 1;
	if (Decode->err_cnt > 3)
	{
		Decode->err_cnt = 3;
	}

	/* Update parameters for next frame */
	for (k = 0; k < LPCORDER; k++)
		Decode->last_qlsp_16[k] = Decode->qlsp_16[k];

	Decode->last_b_16 = pitch_params.b_16;
	Decode->last_lag = pitch_params.lag;
	Decode->last_frac = pitch_params.frac;
	Decode->last_rate = rate;
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  QC13_DecEighth(Int16 rate, QC13_PACKET *qc13_packet, I16 *outvect)
int  QC13_DecEighth(QCP13_DecObj *qc13_dec, Int16 rate, Int16 *outvect)
{
	Int16 j, k;
	Int16 length;
	Int16 loops;
	Int16 agc_flag;
	Int16 * Pvect;
	Int16 temp_G;
	Int32 energy;
	Int32 ar;
	QC13_PACKET *qc13_packet;
	QC13_DObj	*Decode;
	PITCHPARAMS pitch_params;
	PITCHPARAMS pitch_prefilt_params;
	CBPARAMS cb_params;

	qc13_packet = &qc13_dec->q13_packet;
	Decode		= &qc13_dec->Decoder;

	pitch_params.b_16 = 0;
	pitch_params.lag = 0;
	pitch_params.frac = 0;

	/* Unpack LPC paramaters */

	unquantize_lsp(rate, Decode->qlsp_16, Decode->lsp_pred_16, qc13_packet->lpc);

	loops = smooth_lsps(rate, Decode->last_qlsp_16, Decode->qlsp_16, &Decode->low_rate_cnt);

	interp_lpcs(0, Decode->last_qlsp_16, Decode->qlsp_16, Decode->lpc_16[0][NOT_WGHTED], 
		(Int16 *)Decode->lpc_16[0][WGHTED], (Int16 *)Decode->bright_16, loops, QC13_PERCEPT_WGHT_FACTOR_16);

	/* Unpack cb paramaters */
	cb_params.qcode_G = *qc13_packet->G;
	cb_params.qcode_Gsign = POSITIVE;

	//QCELP13_DEC_RESULT_T  unquantize_G_8th(Int16 * q_G,Int16 * qcode_G, Int16 *G_pred)
	if (0 != unquantize_G_8th(&cb_params.G_16, &cb_params.qcode_G, Decode->G_pred))
	{
		return 1;
	}

	ar = (Int32)cb_params.G_16 << 16; // bring up to Q20
	if (ar < 0) ar = -ar;
	cb_params.G_16 = (Int16)(ar >> 16);

	cb_params.seed = qc13_packet->sd_dec;

	ar = (Int32)cb_params.G_16 << 15; // bring up to Q19
	ar = L_add(ar, (Int32)Decode->last_G_8th_rate_16 << 15);
	ar = L_add(ar, 0x00008000l); // rounding
	temp_G = (Int16)(ar >> 16);

	Pvect = outvect;
	length=FSIZE/CBRATE8;
	for (j = 0; j < CBRATE8; j++)
	{
		//        ar = (Int32)Decode.last_G_8th_rate_16 * (Int32)ONE_DIV_EIGHT[7-j] << 1; /* Q4 * Q15 = Q20 (with left shift) */
		//        ar = L_add(ar, (Int32)temp_G * (Int32)ONE_DIV_EIGHT[j+1] << 1); /* Q4 * Q15 = Q20 (with left shift) */
		//        ar = L_add(ar, 0x00008000l); // rounding
		//        cb_params.G_16 = (Int16)(ar >> 16);
		ar = (Int32)Decode->last_G_8th_rate_16 * (Int32)ONE_DIV_EIGHT[7-j];
		ar += (Int32)temp_G * (Int32)ONE_DIV_EIGHT[j+1];
		ar += 0x00004000l;
		cb_params.G_16 = (Int16)(ar >> 15);

		/***********************************************************/

		make_cw(Pvect, length, &cb_params, NOISE);

		Decode->seed = cb_params.seed;

		Pvect += length;
		/***********************************************************/
	}

	Pvect = outvect;
	length=FSIZE / QC13_PITCHSF8TH;
	for (j = 0; j < QC13_PITCHSF8TH; j++)
	{
		update_acbk_16(Decode->adptv_cbk_16, Pvect, length);

		if (Decode->pitch_post_flag == YES)
		{
			pitch_prefilt_params.lag = pitch_params.lag;
			pitch_prefilt_params.b_16 = pitch_params.b_16 >> 1;
			pitch_prefilt_params.frac = pitch_params.frac;

			ar = 0l;
			for (k = 0; k < length; k++)
			{
				ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k] >> 2);
			}
			//ar += 0x2;
			//ar >>= 2; // bring down to Q28

			energy = ar;

			pitch_filter_13(Decode->prefilt_acbk_16, &pitch_prefilt_params, Pvect, length);

			update_acbk_16(Decode->prefilt_acbk_16, Pvect, length);

			agc_prefilter(energy, Pvect, length);

		}

#if DEC_DP_SYNTH_FILTER  /* double precision synthesis filter */
#if 0
		dp_polefilt_asm(Pvect, Decode->lpc_16[0][NOT_WGHTED], Decode->ring_pole_hist_32, length);
#else
		dp_polefilt(Pvect, Decode->lpc_16[0][NOT_WGHTED], Decode->ring_pole_hist_32, length);
#endif
#else
		polefilt(Pvect, Decode->lpc_16[0][NOT_WGHTED], Decode->ring_pole_hist_16, length);
#endif //DEC_DP_SYNTH_FILTER

		if (Decode->pf_flag == PF_ON)
		{
			energy = 0l;
			agc_flag = NO;
			ar = 0l;
			for (k = 0; k < length; k++)
			{
				//can't be modified--shrchen
				ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k]); /* Q15 * Q15 = Q31 (with left shift) */
			}

			if (ar < 0x00800000l) // 0.0078125 in Q31
			{
				energy = ar << 1;
				agc_flag = YES;
			}
			else
			{
				energy = ar >> 6;
			}

			Decode->bright_hist_16=postfilt(QC13, Pvect, Decode->lpc_16[0][NOT_WGHTED],
				Decode->pf_hist_16, (Int16)Decode->bright_16[0], Decode->bright_hist_16, length);

			agc(energy, Pvect, length, &(Decode->agc_factor_16), agc_flag);
		}

		Pvect += length;
	}

	/* Update parameters for next frame */
	for (k = 0; k < LPCORDER; k++)
		Decode->last_qlsp_16[k] = Decode->qlsp_16[k];

	Decode->last_b_16 = 0;
	Decode->last_frac = pitch_params.frac;
	Decode->last_lag = 0;
	Decode->last_G_8th_rate_16 = cb_params.G_16;
	ar = (Int32)Decode->last_G_8th_rate_16 << 16;
	if (ar < 0) ar = -ar;
	Decode->last_G_8th_rate_16 = (Int16)(ar >> 16);
	Decode->err_cnt = (Int16)0;
	Decode->last_rate = rate;

	return VO_ERR_NONE;
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  QC13_DecQuarter(Int16 rate, QC13_PACKET *qc13_packet, I16 *outvect)
Void  QC13_DecQuarter(QCP13_DecObj *qc13_dec, Int16 rate, Int16 *outvect)
{
	Int16 i, j, k;
	Int16 length;
	Int16 loops;
	Int16 agc_flag;
	Int16 *Pvect;
	Int32 energy;
	Int16 gains[QC13_CBRATE4];
	Int16 interp_gains[CBRATE8] = {0};
	Int32 ar;
	QC13_PACKET *qc13_packet;
	QC13_DObj	*Decode;
	PITCHPARAMS pitch_params;
	PITCHPARAMS pitch_prefilt_params;
	CBPARAMS cb_params;

	qc13_packet = &qc13_dec->q13_packet; 
	Decode		= &qc13_dec->Decoder;

	/* Unpack LPC paramaters */

	//   unvq_lsp(Decode.qlsp_16, Decode.lsp_pred_16, qc13_packet->lpc);
	unvq_lsp(Decode->qlsp_16, Decode->lsp_pred_16, qc13_packet->lpc);

	//   loops = smooth_lsps(rate, Decode.last_qlsp_16, Decode.qlsp_16, & (Decode.low_rate_cnt));
	loops = smooth_lsps(rate, Decode->last_qlsp_16, Decode->qlsp_16, & (Decode->low_rate_cnt));

	for (i=0; i < loops; i++)
	{
		//       interp_lpcs(i, Decode.last_qlsp_16, Decode.qlsp_16, Decode.lpc_16[i][NOT_WGHTED], Decode.lpc_16[i][WGHTED], Decode.bright_16, loops, QC13_PERCEPT_WGHT_FACTOR_16);
		interp_lpcs(i, Decode->last_qlsp_16, Decode->qlsp_16, Decode->lpc_16[i][NOT_WGHTED], (Int16 *)Decode->lpc_16[i][WGHTED], Decode->bright_16, loops, QC13_PERCEPT_WGHT_FACTOR_16);

	}

	/* Unpack cb paramaters */
	for (j = 0; j < QC13_CBRATE4; j++)
	{
		cb_params.qcode_G = qc13_packet->G[j];
		cb_params.qcode_Gsign = POSITIVE;
		//       unquantize_G_13(rate, &gains[j], &cb_params.qcode_G, cb_params.qcode_Gsign, Decode.last_G_16, j, Decode.G_pred);
		unquantize_G_13(rate, &gains[j], &cb_params.qcode_G, cb_params.qcode_Gsign, Decode->last_G_16, j, Decode->G_pred);
	}

	for (j=0; j < CBRATE8; j++)
	{
		switch (j)
		{
		case 0 :
			interp_gains[j] = gains[0];
			break;
		case 1 :
			ar = (Int32)gains[0] * 19661 << 1; // Q4 * Q15 = Q20 (with left shift)
			ar = L_add(ar, (Int32)gains[1] * 13107 << 1); // Q4 * Q15 = Q20
			ar = L_add(ar, 0x00008000l);
			interp_gains[j] = (Int16)(ar >> 16);
			break;
		case 2 :
			interp_gains[j] = gains[1];
			break;
		case 3 :
			ar = (Int32)gains[1] * 6554 << 1; // Q4 * Q15 = Q20 (with left shift)
			ar = L_add(ar, (Int32)gains[2] * 26214 << 1); // Q4 * Q15 = Q20
			ar = L_add(ar, 0x00008000l); // rounding
			interp_gains[j] = (Int16)(ar >> 16);
			break;
		case 4:
			ar = (Int32)gains[2] * 26214 << 1; // Q4 * Q15 = Q20 (with left shift)
			ar = L_add(ar, (Int32)gains[3] * 6554 << 1); // Q4 * Q15 = Q20
			ar = L_add(ar, 0x00008000l); // rounding
			interp_gains[j] = (Int16)(ar >> 16);
			break;
		case 5:
			interp_gains[j] = gains[3];
			break;
		case 6:
			ar = (Int32)gains[3] * 13107 << 1; // Q4 * Q15 = Q20 (with left shift)
			ar = L_add(ar, (Int32)gains[4] * 19661 << 1); // Q4 * Q15 = Q20
			ar = L_add(ar, 0x00008000l); // rounding
			interp_gains[j] = (Int16)(ar >> 16);
			break;
		case 7:
			interp_gains[j] = gains[4];
			break;
		}
	}

	cb_params.seed = ((qc13_packet->data[2] & 0xffc) << 4) |
		((qc13_packet->data[1] & 0x1f8) >> 3);

	pitch_params.b_16 = 0;
	pitch_params.lag = 0;
	pitch_params.frac = 0;

	Pvect=outvect;
	length=FSIZE/CBRATE8;
	for (j = 0; j < CBRATE8; j++)
	{

		/***********************************************************/

		cb_params.G_16 = interp_gains[j];

		//       make_qc13_qtr_cw(Pvect, length, &cb_params, Decode.bpf_hist_16);
		make_qc13_qtr_cw(Pvect, length, &cb_params, Decode->bpf_hist_16);

		Decode->seed = cb_params.seed;

		Pvect += length;
		/***********************************************************/
	}

	Pvect=outvect;
	length=FSIZE / PITCHSF[rate];
	for (j = 0; j < PITCHSF[rate]; j++)
	{
		//       update_acbk_16(Decode.adptv_cbk_16, Pvect, length);
		update_acbk_16(Decode->adptv_cbk_16, Pvect, length);

		if (Decode->pitch_post_flag == YES)
		{
			pitch_prefilt_params.lag = pitch_params.lag;
			pitch_prefilt_params.b_16 = pitch_params.b_16 >> 1;
			pitch_prefilt_params.frac = pitch_params.frac;

			ar = 0l;
			for (k = 0; k < length; k++)
			{
				//               ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k]); /* Q15 * Q15 = Q31 (with left shift) */
				ar += (Int32)Pvect[k] * (Int32)Pvect[k] >> 2;
			}
			//ar += 0x2; // rounding
			//ar >>= 2; // bring down to Q28
			energy = ar;

			pitch_filter_13(Decode->prefilt_acbk_16, &pitch_prefilt_params, Pvect, length);

			update_acbk_16(Decode->prefilt_acbk_16, Pvect, length);

			agc_prefilter(energy, Pvect, length);
		}

#if DEC_DP_SYNTH_FILTER  /* double precision synthesis filter */
		dp_polefilt(Pvect, (Decode->lpc_16[j][NOT_WGHTED]), Decode->ring_pole_hist_32, length);
#else
		polefilt(Pvect, (Decode->lpc_16[j][NOT_WGHTED]), Decode->ring_pole_hist_16, length);
#endif //DEC_DP_SYNTH_FILTER

		if (Decode->pf_flag == PF_ON)
		{
			energy = 0l;
			agc_flag = NO;
			ar = 0l;
			for (k = 0; k < length; k++)
			{
				//can't be modified--shrchen
				ar = L_add(ar, (Int32)Pvect[k] * (Int32)Pvect[k]);
			}

			if (ar < 0x01000000l) // 0.0078125 in Q31
			{
				energy = ar << 1;
				agc_flag = YES;
			}
			else
			{
				ar >>= 6;
				//SAT(ar);
				energy = ar;
			}

			Decode->bright_hist_16=postfilt(QC13, Pvect, (Decode->lpc_16[j][NOT_WGHTED]),
				(Decode->pf_hist_16), Decode->bright_16[j], Decode->bright_hist_16, length);

			agc(energy, Pvect, length, &(Decode->agc_factor_16), agc_flag);
		}


		Pvect += length;
	}

	/* Update parameters for next frame */
	for (k = 0; k < LPCORDER; k++)
		Decode->last_qlsp_16[k] = Decode->qlsp_16[k];

	Decode->last_b_16 = 0;
	Decode->last_frac = pitch_params.frac;
	Decode->last_lag = 0;
	ar = (Int32)gains[4] << 16;
	if (ar < 0) ar = -ar;
	Decode->last_G_8th_rate_16 = (Int16)(ar >> 16);
	Decode->err_cnt = (Int16)0;
	Decode->last_rate = rate;
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  QC13_DecPSF(Int16 psf, Int16 rate, QC13_PACKET *qc13_packet, I16 *outvect)
int  QC13_DecPSF(QCP13_DecObj *qc13_dec, Int16 psf, Int16 rate, Int16 *outvect)
{
	Int16 i, j, k;
	Int16 cbsf_number, length;
	Int16 loops;
	Int16 agc_flag;
	Int16 * Ovect;
	Int32 energy;
	Int32 ar;
	QC13_PACKET *qc13_packet;
	QC13_DObj	*Decode;
	PITCHPARAMS pitch_params;
	PITCHPARAMS pitch_prefilt_params;
	CBPARAMS cb_params;

	int  result;
	qc13_packet = &qc13_dec->q13_packet; 
	Decode		= &qc13_dec->Decoder;

	if(psf==0)
	{
		//        unvq_lsp(Decode.qlsp_16, Decode.lsp_pred_16, qc13_packet->lpc);
		unvq_lsp(Decode->qlsp_16, Decode->lsp_pred_16, qc13_packet->lpc);

		//        loops = smooth_lsps(rate, Decode.last_qlsp_16, Decode.qlsp_16, & (Decode.low_rate_cnt));
		loops = smooth_lsps(rate, Decode->last_qlsp_16, Decode->qlsp_16, & (Decode->low_rate_cnt));

		for (i=0; i < loops; i++)
		{
			//            interp_lpcs(i, Decode.last_qlsp_16, Decode.qlsp_16, Decode.lpc_16[i][NOT_WGHTED], Decode.lpc_16[i][WGHTED], Decode.bright_16, loops, QC13_PERCEPT_WGHT_FACTOR_16);
			interp_lpcs(i, Decode->last_qlsp_16, Decode->qlsp_16, Decode->lpc_16[i][NOT_WGHTED], (Int16 *)Decode->lpc_16[i][WGHTED], Decode->bright_16, loops, QC13_PERCEPT_WGHT_FACTOR_16);
		}

		cb_params.sd = qc13_packet->sd_dec;
	}

	pitch_params.qcode_b = qc13_packet->b[psf];
	pitch_params.qcode_lag = qc13_packet->lag[psf];
	pitch_params.qcode_frac = qc13_packet->frac[psf];

	result = unquantize_lag(&(pitch_params.lag), & (pitch_params.qcode_lag),
		& (pitch_params.frac),
		& (pitch_params.qcode_frac));
	if (VO_ERR_NONE != result)
	{
		return result;
	}

	result = unquantize_b(&(pitch_params.b_16), & (pitch_params.qcode_b),
		pitch_params.qcode_lag);
	if (VO_ERR_NONE != result)
	{
		return result;
	}

	Ovect = outvect;
	length=(Int16)(FSIZE / CBSF[rate]);

	for (j = 0; j < (Int16)(CBSF[rate] / PITCHSF[rate]); j++)
	{
		cbsf_number = (Int16)(psf * (CBSF[rate] / PITCHSF[rate]) + j);
		unpack_cb13(qc13_packet, &cb_params, cbsf_number);

		//        unquantize_G_13(rate, & (cb_params.G_16), & (cb_params.qcode_G),
		//            cb_params.qcode_Gsign, Decode.last_G_16, cbsf_number, Decode.G_pred);
		unquantize_G_13(rate, &(cb_params.G_16), & (cb_params.qcode_G),
			cb_params.qcode_Gsign, Decode->last_G_16, cbsf_number, Decode->G_pred);

		unquantize_i( & (cb_params.i), & (cb_params.qcode_i));

		/***********************************************************/

		if (rate == QC13_FULL)
		{
			//            make_qc13_full_cw(Ovect, length, &cb_params);  // Full rate voiced
			make_qc13_full_cw(Ovect, length, &cb_params);  // Full rate voiced^
		}
		else
		{
			//            make_cw(Ovect, length, &cb_params, TABLE);  // Half rate voiced
			make_cw(Ovect, length, &cb_params, TABLE);  // Half rate voiced
		}

		Ovect += length;
		/***********************************************************/
	}
	Ovect = outvect;
	length=FSIZE / PITCHSF[rate];

	pitch_filter_13(Decode->adptv_cbk_16, &pitch_params, Ovect, length);

	update_acbk_16(Decode->adptv_cbk_16, Ovect, length);

	if (Decode->pitch_post_flag == YES)
	{
		if (pitch_params.b_16 > 16384)
			pitch_prefilt_params.b_16 = 16384;
		else
			pitch_prefilt_params.b_16 = pitch_params.b_16;

		ar = (Int32)pitch_prefilt_params.b_16 * (Int32)QC13_SCALE_B_16 << 1; // Q15 * Q14 = Q30
		ar = L_add(ar, 0x00008000l);
		pitch_prefilt_params.b_16 = (Int16)(ar >> 16);

		pitch_prefilt_params.frac = pitch_params.frac;
		pitch_prefilt_params.lag = pitch_params.lag;

		ar = 0l;
		for (k = 0; k < length; k++)
		{
			ar = L_add(ar, (Int32)Ovect[k] * (Int32)Ovect[k] >> 2); /* Q15 * Q15 = Q31 (with left shift) */
		}
		//        ar <<= 1;
		//ar = L_add(ar, 0x2l); // rounding
		//ar >>= 2; // bring down to Q28
		energy = ar;

		pitch_filter_13(Decode->prefilt_acbk_16, &pitch_prefilt_params, Ovect, length);

		update_acbk_16(Decode->prefilt_acbk_16, Ovect, length);

		agc_prefilter(energy, Ovect, length);
	}

#if DEC_DP_SYNTH_FILTER  /* double precision synthesis filter */
#if 0
	dp_polefilt_asm(Ovect, (Decode->lpc_16[psf][NOT_WGHTED]), Decode->ring_pole_hist_32, length);
#else
	dp_polefilt(Ovect, (Decode->lpc_16[psf][NOT_WGHTED]), Decode->ring_pole_hist_32, length);
#endif
#else
	polefilt(Ovect, (Decode->lpc_16[psf][NOT_WGHTED]), Decode->ring_pole_hist_16, length);
#endif //DEC_DP_SYNTH_FILTER

	if (Decode->pf_flag == PF_ON)
	{
		energy = 0l;
		agc_flag = NO;
		ar = 0l;
		for (k = 0; k < length; k++)
		{
			ar = L_add(ar, (Int32)Ovect[k] * (Int32)Ovect[k]); /* Q15 * Q15 = Q31 (with left shift) */
		}

		if (ar < 0x00800000l) // 0.0078125 in Q31
		{
			energy = ar << 1;
			agc_flag = YES;
		}
		else
		{
			energy = ar >> 6;
		}

		Decode->bright_hist_16=postfilt(QC13, Ovect, (Decode->lpc_16[psf][NOT_WGHTED]),
			(Decode->pf_hist_16), Decode->bright_16[psf], Decode->bright_hist_16, length);

		agc(energy, Ovect, length, &(Decode->agc_factor_16), agc_flag);
	}

	if(psf==PITCHSF[rate]-1)
	{
		/* Update parameters for next frame */
		for (k = 0; k < LPCORDER; k++)
			Decode->last_qlsp_16[k] = Decode->qlsp_16[k];

		Decode->last_b_16 = pitch_params.b_16;
		Decode->last_frac = pitch_params.frac;
		Decode->last_lag = pitch_params.lag;

		ar = (Int32)cb_params.G_16 << 16;
		if (ar < 0) ar = -ar;
		Decode->last_G_8th_rate_16 = (Int16)(ar >> 16);
		Decode->err_cnt = (Int16)0;
		Decode->last_rate = rate;
	}

	return VO_ERR_NONE;
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  agc_prefilter(I32 input_energy, I16 *output, Int16 length)
Void  agc_prefilter(Int32 input_energy, Int16 *output, Int16 length)
{
	Int16 i;
	Int32 output_energy;
	Int32 acc;
	Int16 norm;
	Int16 divisor;
	Int32 divisor_temp;

	output_energy = 0L;
	for (i = 0; i < length; i++)
	{
		output_energy = L_add(output_energy, ((Int32)output[i] * (Int32)output[i]) >> 2);
	}
	//    output_energy >>= 1; // bring down to Q28

	if ((input_energy != 0l) && (output_energy != 0l))
	{
		//        norm = sqrt((Float) (input_energy / output_energy));

		acc = (Int32)Sqrt(input_energy) << 15;
		divisor = Sqrt(output_energy);

		if (acc < ((Int32)divisor << 16))
		{

			if (divisor != 0)
			{
				divisor_temp = ((Int32)divisor) << 15;
				for (i = 0; i < 15; i++)
				{
					if (acc >= divisor_temp)
					{
						acc = ((acc - divisor_temp) << 1) + 1;
					}
					else
					{
						acc <<= 1;
					}
				}

				acc &= 0x7fff;
			}
			else
			{
				acc = 0l;
			}
		}
		else
			acc = 0x2000l;

		norm=(Int16)acc;

		for (i = 0; i < length; i++)
		{
			//          output[i] *= norm;
			acc = ((Int32)output[i] * (Int32)norm);
			//            acc = L_add(acc, acc);
			//acc <<= 2;
			acc = L_add(acc, 0x00002000l);
			output[i] = (Int16)(acc >> 14);			
		}
	}
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  gen_params(PITCHPARAMS *pitch_params, CBPARAMS *cb_params, I16* interp_G)
Void  gen_params(QC13_DObj *Decode, PITCHPARAMS *pitch_params, CBPARAMS *cb_params, Int16* interp_G)
{
	Int16 i, j;
	Int16 last_G;
	Int32 ar;

	cb_params->seed = ((521 * (Decode->seed) + 259) & 0xffff);
	cb_params->i = (Int16)(cb_params->seed & 0x7f);

	pitch_params->lag = Decode->last_lag;
	pitch_params->frac = Decode->last_frac;
	pitch_params->b_16 = Decode->last_b_16;

	if (pitch_params->b_16 > ERA_B_SAT_16[Decode->err_cnt])
	{
		pitch_params->b_16 = ERA_B_SAT_16[Decode->err_cnt];
	}

	for (i = QC13_GPRED_ORDER - 1; i > 0; i--)
	{
		Decode->G_pred[i] = Decode->G_pred[i - 1];
	}

	if (!(Decode->last_rate == QC13_EIGHTH && Decode->G_pred[0] < 14))
	{
		if (Decode->err_cnt < 3)
			Decode->G_pred[0] -= Decode->err_cnt;
		else
			Decode->G_pred[0] -= (Int16) (2 * Decode->err_cnt);
	}

	if (Decode->G_pred[0] < 0)
		Decode->G_pred[0] = 0;

	cb_params->G_16 = GA_16[Decode->G_pred[0] + 6];

	ar = (Int32)cb_params->G_16 << 16; // Q20
	ar = L_add(ar, (Int32)Decode->last_G_8th_rate_16 << 16); // Q20
	ar >>= 1; // multiply by 0.5
	ar = L_add(ar, 0x00008000l);
	last_G = (Int16)(ar >> 16);

	for (j = 0; j < QC13_PITCHSF_ERASURE; j++)
	{
		ar = (Int32)Decode->last_G_8th_rate_16 * (Int32)ONE_DIV_PITCHSF_ERASURE[QC13_PITCHSF_ERASURE - 1 -j] << 1; // Q4 * Q15 = Q20 (with left shift)

		ar = L_add(ar, (Int32)last_G * (Int32)ONE_DIV_PITCHSF_ERASURE[j + 1] << 1); // Q20
		ar = L_add(ar, 0x00008000l); // rounding
		interp_G[j] = (Int16)(ar >> 16); // Q4 number
	}

	Decode->last_G_8th_rate_16 = cb_params->G_16;
	Decode->seed = cb_params->seed;
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  agc(I32 input_energy, I16 * output, Int16 length, I16 *factor, Int16 agc_flag)
#if 1 //C_OPT
Void  agc(Int32 input_energy, Int16 * output, Int16 length, Int16 *factor, Int16 agc_flag)
{
	Int16 i;
	Int32 output_energy;
	Int32 acc;
	Int16 norm;
	Int16 divisor;
	Int32 divisor_temp;   
	output_energy = 0L;
	if (agc_flag == NO)
	{
		for (i = length - 1; i >= 0; i--)
		{
			output_energy = L_add(output_energy, (output[i] * output[i] >> 4));
		}
		output_energy >>= 2;
	}
	else
	{
		for (i = length - 1; i >= 0; i--)
		{
			output_energy = L_add(output_energy, output[i] * output[i]);
		}
		output_energy = L_shl2(output_energy, 1);
	}

	if (output_energy > 0L )
	{
		//       norm = sqrt((double) (input_energy / output_energy));
		acc = (Int32)Sqrt(input_energy) << 14;
		divisor = Sqrt(output_energy);		
		if (acc < ((Int32)divisor << 16))
		{
			divisor_temp = ((Int32)divisor) << 15;
			for (i = 0; i < 15; i++)
			{
				if (acc >= divisor_temp)
				{
					acc = ((acc - divisor_temp) << 1) + 1;
				}
				else
				{
					acc <<= 1;
				}
			}			
			acc &= 0x7fff;
		}
		else
			acc = 0x7fffl;	
		norm = (Int16)acc;
	}
	else
	{
		norm = *factor;
	}	
	//  *factor = AGC_FACTOR * (*factor) + (1 - AGC_FACTOR) * norm;
	acc = (Int32)(*factor) << 16;
	acc = L_sub(acc, (acc >> 4));
	acc = L_add(acc, (Int32)norm << 12);
	*factor=(Int16)(acc >> 16);

	for (i = 0; i < length; i++)
	{
		acc = (Int32)(*factor) * (Int32)output[i];
		acc = L_shl2(acc, 3);        
		output[i] = (Int16)(acc >> 16);
	}
}
#else
Void  agc(Int32 input_energy, Int16 * output, Int16 length, Int16 *factor, Int16 agc_flag)
{
	Int16 i;
	Int32 output_energy;
	Int32 acc;
	Int16 norm;
	Int16 divisor;
	Int32 divisor_temp;

	output_energy = 0L;
	if (agc_flag == NO)
	{
		for (i = 0; i < length; i++)
		{
			output_energy = L_add(output_energy, (output[i] * output[i] >> 4));
		}
		output_energy >>= 2;
	}
	else
	{
		for (i = 0; i < length; i++)
		{
			output_energy = L_add(output_energy, output[i] * output[i]);
		}
		output_energy = L_shl2(output_energy, 1);
	}

	if (output_energy > 0L )
	{
		//       norm = sqrt((double) (input_energy / output_energy));
		acc = (Int32)Sqrt(input_energy) << 14;
		divisor = Sqrt(output_energy);

		if (acc < ((Int32)divisor << 16))
		{
			if (divisor != 0)
			{
				divisor_temp = ((Int32)divisor) << 15;
				for (i = 0; i < 15; i++)
				{
					if (acc >= divisor_temp)
					{
						acc = ((acc - divisor_temp) << 1) + 1;
					}
					else
					{
						acc <<= 1;
					}
				}

				acc &= 0x7fff;
			}
			else
				acc = *factor;
		}
		else
			acc = 0x7fffl;

		norm = (Int16)acc;
	}
	else
	{
		norm = *factor;
	}

	//  *factor = AGC_FACTOR * (*factor) + (1 - AGC_FACTOR) * norm;
	acc = (Int32)(*factor) << 16;
	acc = L_sub(acc, (acc >> 4));
	acc = L_add(acc, (Int32)norm << 12);
	*factor=(Int16)(acc >> 16);

	for (i = 0; i < length; i++)
	{
		acc = (Int32)(*factor) * (Int32)output[i];
		acc = L_shl2(acc, 3);        
		output[i] = (Int16)(acc >> 16);
	}
}
#endif

#define step(shift) \
	if ((0x40000000l >> shift) + root <= value)       \
{                                                 \
	value -= (0x40000000l >> shift) + root;       \
	root = (root >> 1) | (0x40000000l >> shift);  \
} else {                                          \
	root = root >> 1;                             \
}

Int16 Sqrt(Int32 value)
{
	int root = 0;

	step( 0); step( 2); step( 4); step( 6);
	step( 8); step(10); step(12); step(14);
	step(16); step(18); step(20); step(22);
	step(24); step(26); step(28); step(30);

	if (root < value)
		++root;

	return root;
}

