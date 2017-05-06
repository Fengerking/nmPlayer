//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    encode.c

Abstract:

    Subband Codec encode file.

Author:

    Witten Wen 11-January-2010

Revision History:

*************************************************************************/

#include "sbc.h"
#include "sbc_tables.h"
#include "sbccommon.h"
/*
 * Packs the SBC frame from frame into the memory at data. At most len
 * bytes will be used, should more memory be needed an appropriate
 * error code will be returned. Returns the length of the packed frame
 * on success or a negative value on error.
 *
 * The error codes are:
 * -1 Not enough memory reserved
 * -2 Unsupported sampling rate
 * -3 Unsupported number of blocks
 * -4 Unsupported number of subbands
 * -5 Bitpool value out of bounds
 * -99 not implemented
 */

static int SBCPackFrame(unsigned char *data, struct sbc_frame *frame, VO_U32 len)
{
	int produced;
	/* Will copy the header parts for CRC-8 calculation here */
	unsigned char crc_header[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int crc_pos = 0;

	/* Sampling frequency as temporary value for table lookup */
	unsigned char  sf;
	unsigned short audio_sample;

	int ch, sb, blk, bit;	/* channel, subband, block and bit counters */
	int bits[2][8];		/* bits distribution */
	int levels[2][8];	/* levels are derived from that */

	int scalefactor[2][8];	/* derived from frame->m_ScaleFactor */

	data[0] = VOSBC_SYNCWORD;
	/* set the sampling frequency*/
	if (frame->m_SampleRate == 16000) {
		data[1] = (VOSBC_MODE_16 & 0x03) << 6;
		sf = VOSBC_MODE_16;
	} else if (frame->m_SampleRate == 32000) {
		data[1] = (VOSBC_MODE_32 & 0x03) << 6;
		sf = VOSBC_MODE_32;
	} else if (frame->m_SampleRate == 44100) {
		data[1] = (VOSBC_MODE_44 & 0x03) << 6;
		sf = VOSBC_MODE_44;
	} else if (frame->m_SampleRate == 48000) {
		data[1] = (VOSBC_MODE_48 & 0x03) << 6;
		sf = VOSBC_MODE_48;
	} else
		return -2;

	switch (frame->m_Blocks) {
	case 4:
		data[1] |= (NB4_VOSBC & 0x03) << 4;
		break;
	case 8:
		data[1] |= (NB8_VOSBC & 0x03) << 4;
		break;
	case 12:
		data[1] |= (NB12_VOSBC & 0x03) << 4;
		break;
	case 16:
		data[1] |= (NB16_VOSBC & 0x03) << 4;
		break;
	default:
		return -3;
		break;
	}

	data[1] |= (frame->channel_mode & 0x03) << 2;

	data[1] |= (frame->allocation_method & 0x01) << 1;

	switch (frame->m_Subbands) {
	case 4:
		/* Nothing to do */
		break;
	case 8:
		data[1] |= 0x01;
		break;
	default:
		return -4;
		break;
	}

	data[2] = frame->m_Bitpool;

	if ((frame->channel_mode == VOSBR_CH_MONO || frame->channel_mode == VOSBR_CH_DUAL_CHANNEL) &&
			frame->m_Bitpool > frame->m_Subbands << 4)
		return -5;

	if ((frame->channel_mode == VOSBR_CH_STEREO || frame->channel_mode == VOSBR_CH_JOINT_STEREO) &&
			frame->m_Bitpool > frame->m_Subbands << 5)
		return -5;

	/* Can't fill in crc yet */

	produced = 32;

	crc_header[0] = data[1];
	crc_header[1] = data[2];
	crc_pos = 16;

	for (ch = 0; ch < frame->m_Channels; ch++) {
		for (sb = 0; sb < frame->m_Subbands; sb++) {
			frame->m_ScaleFactor[ch][sb] = 0;
			scalefactor[ch][sb] = 2;
			for (blk = 0; blk < frame->m_Blocks; blk++) {
				while (scalefactor[ch][sb] < fabs(frame->m_SBSampleF[blk][ch][sb])) {
					frame->m_ScaleFactor[ch][sb]++;
					scalefactor[ch][sb] *= 2;
				}
			}
		}
	}

	if (frame->channel_mode == VOSBR_CH_JOINT_STEREO) {
		/* like frame->sb_sample but joint stereo */
		int sb_sample_j[16][2];
		/* scalefactor and scale_factor in joint case */
		int scalefactor_j[2];
		unsigned char scale_factor_j[2];

		frame->m_Join = 0;

		for (sb = 0; sb < frame->m_Subbands - 1; sb++) {
			scale_factor_j[0] = 0;
			scalefactor_j[0] = 2;
			scale_factor_j[1] = 0;
			scalefactor_j[1] = 2;

			for (blk = 0; blk < frame->m_Blocks; blk++) {
				/* Calculate joint stereo signal */
				sb_sample_j[blk][0] =
					(frame->m_SBSampleF[blk][0][sb] +
						frame->m_SBSampleF[blk][1][sb]) >> 1;
				sb_sample_j[blk][1] =
					(frame->m_SBSampleF[blk][0][sb] -
						frame->m_SBSampleF[blk][1][sb]) >> 1;

				/* calculate scale_factor_j and scalefactor_j for joint case */
				while (scalefactor_j[0] < fabs(sb_sample_j[blk][0])) {
					scale_factor_j[0]++;
					scalefactor_j[0] *= 2;
				}
				while (scalefactor_j[1] < fabs(sb_sample_j[blk][1])) {
					scale_factor_j[1]++;
					scalefactor_j[1] *= 2;
				}
			}

			/* decide whether to join this subband */
			if ((scalefactor[0][sb] + scalefactor[1][sb]) >
					(scalefactor_j[0] + scalefactor_j[1]) ) {
				/* use joint stereo for this subband */
				frame->m_Join |= 1 << sb;
				frame->m_ScaleFactor[0][sb] = scale_factor_j[0];
				frame->m_ScaleFactor[1][sb] = scale_factor_j[1];
				scalefactor[0][sb] = scalefactor_j[0];
				scalefactor[1][sb] = scalefactor_j[1];
				for (blk = 0; blk < frame->m_Blocks; blk++) {
					frame->m_SBSampleF[blk][0][sb] =
							sb_sample_j[blk][0];
					frame->m_SBSampleF[blk][1][sb] =
							sb_sample_j[blk][1];
				}
			}
		}

		data[4] = 0;
		for (sb = 0; sb < frame->m_Subbands - 1; sb++)
			data[4] |= ((frame->m_Join >> sb) & 0x01) << (frame->m_Subbands - 1 - sb);

		crc_header[crc_pos >> 3] = data[4];

		produced += frame->m_Subbands;
		crc_pos += frame->m_Subbands;
	}

	for (ch = 0; ch < frame->m_Channels; ch++) 
	{
		for (sb = 0; sb < frame->m_Subbands; sb++) 
		{
			data[produced >> 3] <<= 4;
			crc_header[crc_pos >> 3] <<= 4;
			data[produced >> 3] |= frame->m_ScaleFactor[ch][sb] & 0x0F;
			crc_header[crc_pos >> 3] |= frame->m_ScaleFactor[ch][sb] & 0x0F;

			produced += 4;
			crc_pos += 4;
		}
	}

	/* align the last crc byte */
	if(crc_pos % 8) 
		crc_header[crc_pos >> 3] <<= 8 - (crc_pos % 8);

	data[3] = SBCCrc8(crc_header, crc_pos);

	SBCCalculateBits(frame, bits, sf);

	for (ch = 0; ch < frame->m_Channels; ch++) {
		for (sb = 0; sb < frame->m_Subbands; sb++)
			levels[ch][sb] = (1 << bits[ch][sb]) - 1;
	}

	for (blk = 0; blk < frame->m_Blocks; blk++) {
		for (ch = 0; ch < frame->m_Channels; ch++) {
			for (sb = 0; sb < frame->m_Subbands; sb++) {
				if (levels[ch][sb] > 0) {
					audio_sample =
						(unsigned short) ((((frame->m_SBSampleF[blk][ch][sb]*levels[ch][sb]) >>
									(frame->m_ScaleFactor[ch][sb] + 1)) +
								levels[ch][sb]) >> 1);
					audio_sample <<= 16 - bits[ch][sb];
					for (bit = 0; bit < bits[ch][sb]; bit++) {
						data[produced >> 3] <<= 1;
						if(audio_sample & 0x8000)
							data[produced >> 3] |= 0x1;
						audio_sample <<= 1;
						produced++;
					}
				}
			}
		}
	}

	/* align the last byte */
	if(produced % 8) {
		data[produced >> 3] <<= 8 - (produced % 8);
	}

	return (produced + 7) >> 3;
}


static void SBCEncoderInit(struct sbc_encoder_state *state,
				const struct sbc_frame *frame)
{
	voSBCMemset(&state->m_X, 0, sizeof(state->m_X));
	state->m_Subbands = frame->m_Subbands;
}

static __inline void _SBCAnalyze4(const int *in, int *out)
{

	sbc_extended_t res;
	sbc_extended_t t[8];
	sbc_extended_t s[5];

	MUL(res, voSBR_4_Proto[0], (in[8] - in[32])); /* Q18 */
	MULA(res, voSBR_4_Proto[1], (in[16] - in[24]));
	t[0] = SCALE4_STAGE1(res); /* Q8 */

	MUL(res, voSBR_4_Proto[2], in[1]);
	MULA(res, voSBR_4_Proto[3], in[9]);
	MULA(res, voSBR_4_Proto[4], in[17]);
	MULA(res, voSBR_4_Proto[5], in[25]);
	MULA(res, voSBR_4_Proto[6], in[33]);
	t[1] = SCALE4_STAGE1(res);

	MUL(res, voSBR_4_Proto[7], in[2]);
	MULA(res, voSBR_4_Proto[8], in[10]);
	MULA(res, voSBR_4_Proto[9], in[18]);
	MULA(res, voSBR_4_Proto[10], in[26]);
	MULA(res, voSBR_4_Proto[11], in[34]);
	t[2] = SCALE4_STAGE1(res);

	MUL(res, voSBR_4_Proto[12], in[3]);
	MULA(res, voSBR_4_Proto[13], in[11]);
	MULA(res, voSBR_4_Proto[14], in[19]);
	MULA(res, voSBR_4_Proto[15], in[27]);
	MULA(res, voSBR_4_Proto[16], in[35]);
	t[3] = SCALE4_STAGE1(res);

	MUL(res, voSBR_4_Proto[17], in[4] + in[36]);
	MULA(res, voSBR_4_Proto[18], in[12] + in[28]);
	MULA(res, voSBR_4_Proto[19], in[20]);
	t[4] = SCALE4_STAGE1(res);

	MUL(res, voSBR_4_Proto[16], in[5]);
	MULA(res, voSBR_4_Proto[15], in[13]);
	MULA(res, voSBR_4_Proto[14], in[21]);
	MULA(res, voSBR_4_Proto[13], in[29]);
	MULA(res, voSBR_4_Proto[12], in[37]);
	t[5] = SCALE4_STAGE1(res);

	/* don't compute t[6]... this term always multiplies
	* with cos(pi/2) = 0 */

	MUL(res, voSBR_4_Proto[6], in[7]);
	MULA(res, voSBR_4_Proto[5], in[15]);
	MULA(res, voSBR_4_Proto[4], in[23]);
	MULA(res, voSBR_4_Proto[3], in[31]);
	MULA(res, voSBR_4_Proto[2], in[39]);
	t[7] = SCALE4_STAGE1(res);

	MUL(s[0], voSBR_M4_ANA[0], t[0] + t[4]);
	MUL(s[1], voSBR_M4_ANA[2], t[2]);
	MUL(s[2], voSBR_M4_ANA[1], t[1] + t[3]);
	MULA(s[2], voSBR_M4_ANA[3], t[5]);
	MUL(s[3], voSBR_M4_ANA[3], t[1] + t[3]);
	MULA(s[3], voSBR_M4_ANA[1], - t[5] + t[7]);
	MUL(s[4], voSBR_M4_ANA[3], t[7]);
	out[0] = (int)SCALE4_STAGE2( s[0] + s[1] + s[2] + s[4]); /* Q0 */
	out[1] = (int)SCALE4_STAGE2(-s[0] + s[1] + s[3]);
	out[2] = (int)SCALE4_STAGE2(-s[0] + s[1] - s[3]);
	out[3] = (int)SCALE4_STAGE2( s[0] + s[1] - s[2] - s[4]);
}

static __inline void SBCAnalyze4(struct sbc_encoder_state *state,
struct sbc_frame *frame, int ch, int blk)
{
	int i;
	/* Input 4 New Audio Samples */
	for (i = 39; i >= 4; i--)
		state->m_X[ch][i] = state->m_X[ch][i - 4];
	for (i = 3; i >= 0; i--)
		state->m_X[ch][i] = frame->m_PCMSample[ch][blk * 4 + (3 - i)];
	_SBCAnalyze4(state->m_X[ch], frame->m_SBSampleF[blk][ch]);
}

static __inline void _SBCAnalyze8(const int *in, int *out)
{
	sbc_extended_t res;
	sbc_extended_t t[8];
	sbc_extended_t s[8];

	MUL(res,  voSBR_8_Proto[0], (in[16] - in[64])); /* Q18 = Q18 * Q0 */
	MULA(res, voSBR_8_Proto[1], (in[32] - in[48]));
	MULA(res, voSBR_8_Proto[2], in[4]);
	MULA(res, voSBR_8_Proto[3], in[20]);
	MULA(res, voSBR_8_Proto[4], in[36]);
	MULA(res, voSBR_8_Proto[5], in[52]);
	t[0] = SCALE8_STAGE1(res); /* Q10 */

	MUL(res,   voSBR_8_Proto[6], in[2]);
	MULA(res,  voSBR_8_Proto[7], in[18]);
	MULA(res,  voSBR_8_Proto[8], in[34]);
	MULA(res,  voSBR_8_Proto[9], in[50]);
	MULA(res, voSBR_8_Proto[10], in[66]);
	t[1] = SCALE8_STAGE1(res);

	MUL(res,  voSBR_8_Proto[11], in[1]);
	MULA(res, voSBR_8_Proto[12], in[17]);
	MULA(res, voSBR_8_Proto[13], in[33]);
	MULA(res, voSBR_8_Proto[14], in[49]);
	MULA(res, voSBR_8_Proto[15], in[65]);
	MULA(res, voSBR_8_Proto[16], in[3]);
	MULA(res, voSBR_8_Proto[17], in[19]);
	MULA(res, voSBR_8_Proto[18], in[35]);
	MULA(res, voSBR_8_Proto[19], in[51]);
	MULA(res, voSBR_8_Proto[20], in[67]);
	t[2] = SCALE8_STAGE1(res);

	MUL(res,   voSBR_8_Proto[21], in[5]);
	MULA(res,  voSBR_8_Proto[22], in[21]);
	MULA(res,  voSBR_8_Proto[23], in[37]);
	MULA(res,  voSBR_8_Proto[24], in[53]);
	MULA(res,  voSBR_8_Proto[25], in[69]);
	MULA(res, -voSBR_8_Proto[15], in[15]);
	MULA(res, -voSBR_8_Proto[14], in[31]);
	MULA(res, -voSBR_8_Proto[13], in[47]);
	MULA(res, -voSBR_8_Proto[12], in[63]);
	MULA(res, -voSBR_8_Proto[11], in[79]);
	t[3] = SCALE8_STAGE1(res);

	MUL(res,   voSBR_8_Proto[26], in[6]);
	MULA(res,  voSBR_8_Proto[27], in[22]);
	MULA(res,  voSBR_8_Proto[28], in[38]);
	MULA(res,  voSBR_8_Proto[29], in[54]);
	MULA(res,  voSBR_8_Proto[30], in[70]);
	MULA(res, -voSBR_8_Proto[10], in[14]);
	MULA(res,  -voSBR_8_Proto[9], in[30]);
	MULA(res,  -voSBR_8_Proto[8], in[46]);
	MULA(res,  -voSBR_8_Proto[7], in[62]);
	MULA(res,  -voSBR_8_Proto[6], in[78]);
	t[4] = SCALE8_STAGE1(res);

	MUL(res,   voSBR_8_Proto[31], in[7]);
	MULA(res,  voSBR_8_Proto[32], in[23]);
	MULA(res,  voSBR_8_Proto[33], in[39]);
	MULA(res,  voSBR_8_Proto[34], in[55]);
	MULA(res,  voSBR_8_Proto[35], in[71]);
	MULA(res, -voSBR_8_Proto[20], in[13]);
	MULA(res, -voSBR_8_Proto[19], in[29]);
	MULA(res, -voSBR_8_Proto[18], in[45]);
	MULA(res, -voSBR_8_Proto[17], in[61]);
	MULA(res, -voSBR_8_Proto[16], in[77]);
	t[5] = SCALE8_STAGE1(res);

	MUL(res,   voSBR_8_Proto[36], in[8] + in[72]);
	MULA(res,  voSBR_8_Proto[37], in[24] + in[56]);
	MULA(res,  voSBR_8_Proto[38], in[40]);
	MULA(res, -voSBR_8_Proto[39], in[12]);
	MULA(res,  -voSBR_8_Proto[5], in[28]);
	MULA(res,  -voSBR_8_Proto[4], in[44]);
	MULA(res,  -voSBR_8_Proto[3], in[60]);
	MULA(res,  -voSBR_8_Proto[2], in[76]);
	t[6] = SCALE8_STAGE1(res);

	MUL(res,   voSBR_8_Proto[35], in[9]);
	MULA(res,  voSBR_8_Proto[34], in[25]);
	MULA(res,  voSBR_8_Proto[33], in[41]);
	MULA(res,  voSBR_8_Proto[32], in[57]);
	MULA(res,  voSBR_8_Proto[31], in[73]);
	MULA(res, -voSBR_8_Proto[25], in[11]);
	MULA(res, -voSBR_8_Proto[24], in[27]);
	MULA(res, -voSBR_8_Proto[23], in[43]);
	MULA(res, -voSBR_8_Proto[22], in[59]);
	MULA(res, -voSBR_8_Proto[21], in[75]);
	t[7] = SCALE8_STAGE1(res);

	MUL(s[0], voSBR_M8_ANA[0], t[0]); /* = Q14 * Q10 */
	MULA(s[0], voSBR_M8_ANA[1], t[6]);
	MUL(s[1], voSBR_M8_ANA[7], t[1]);
	MUL(s[2], voSBR_M8_ANA[2], t[2]);
	MULA(s[2], voSBR_M8_ANA[3], t[3]);
	MULA(s[2], voSBR_M8_ANA[4], t[5]);
	MULA(s[2], voSBR_M8_ANA[5], t[7]);
	MUL(s[3], voSBR_M8_ANA[6], t[4]);
	MUL(s[4], voSBR_M8_ANA[3], t[2]);
	MULA(s[4], -voSBR_M8_ANA[5], t[3]);
	MULA(s[4], -voSBR_M8_ANA[2], t[5]);
	MULA(s[4], -voSBR_M8_ANA[4], t[7]);
	MUL(s[5], voSBR_M8_ANA[4], t[2]);
	MULA(s[5], -voSBR_M8_ANA[2], t[3]);
	MULA(s[5], voSBR_M8_ANA[5], t[5]);
	MULA(s[5], voSBR_M8_ANA[3], t[7]);
	MUL(s[6], voSBR_M8_ANA[1], t[0]);
	MULA(s[6], -voSBR_M8_ANA[0], t[6]);
	MUL(s[7], voSBR_M8_ANA[5], t[2]);
	MULA(s[7], -voSBR_M8_ANA[4], t[3]);
	MULA(s[7], voSBR_M8_ANA[3], t[5]);
	MULA(s[7], -voSBR_M8_ANA[2], t[7]);
	out[0] = (int)SCALE8_STAGE2( s[0] + s[1] + s[2] + s[3]);
	out[1] = (int)SCALE8_STAGE2( s[1] - s[3] + s[4] + s[6]);
	out[2] = (int)SCALE8_STAGE2( s[1] - s[3] + s[5] - s[6]);
	out[3] = (int)SCALE8_STAGE2(-s[0] + s[1] + s[3] + s[7]);
	out[4] = (int)SCALE8_STAGE2(-s[0] + s[1] + s[3] - s[7]);
	out[5] = (int)SCALE8_STAGE2( s[1] - s[3] - s[5] - s[6]);
	out[6] = (int)SCALE8_STAGE2( s[1] - s[3] - s[4] + s[6]);
	out[7] = (int)SCALE8_STAGE2( s[0] + s[1] - s[2] + s[3]);
}

static __inline void SBCAnalyze8(struct sbc_encoder_state *state,
struct sbc_frame *frame, int ch,
	int blk)
{
	int i;

	/* Input 8 Audio Samples */
	for (i = 79; i >= 8; i--)
		state->m_X[ch][i] = state->m_X[ch][i - 8];
	for (i = 7; i >= 0; i--)
		state->m_X[ch][i] = frame->m_PCMSample[ch][blk * 8 + (7 - i)];
	_SBCAnalyze8(state->m_X[ch], frame->m_SBSampleF[blk][ch]);
}

static int SBCAnalyzeAudio(struct sbc_encoder_state *state,
struct sbc_frame *frame)
{
	int ch, blk;

	switch (frame->m_Subbands) {
case 4:
	for (ch = 0; ch < frame->m_Channels; ch++)
		for (blk = 0; blk < frame->m_Blocks; blk++)
			SBCAnalyze4(state, frame, ch, blk);
	return frame->m_Blocks * 4;

case 8:
	for (ch = 0; ch < frame->m_Channels; ch++)
		for (blk = 0; blk < frame->m_Blocks; blk++)
			SBCAnalyze8(state, frame, ch, blk);
	return frame->m_Blocks * 8;

default:
	return ERR_IO;
	}
}

int SBCEncode(sbc_t *sbc, VO_U8 *input, VO_U32 input_len, VO_U8 *output,
		int output_len, VO_U32 *written)
{
	struct sbc_priv *priv;
	char *ptr;
	int i, ch, framelen, samples;

	if (!sbc && !input)
		return ERR_IO;

	priv = sbc->m_pPriv;

	if (written)
		*written = 0;

	if (!priv->m_Init) {
		priv->m_Frame.m_SampleRate = sbc->m_Rate;
		priv->m_Frame.m_Channels = sbc->m_Channels;

		if (sbc->m_Channels > 1) {
			if (sbc->m_Joint)/* 2 channel*/
				priv->m_Frame.channel_mode = VOSBR_CH_JOINT_STEREO;
			else
				priv->m_Frame.channel_mode = VOSBR_CH_STEREO;
		} else /*1 mono*/
			priv->m_Frame.channel_mode = VOSBR_CH_MONO;

		priv->m_Frame.allocation_method = sbc->m_Allocation;
		priv->m_Frame.m_Subbands = sbc->m_Subbands;
		priv->m_Frame.m_Blocks = sbc->m_Blocks;
		priv->m_Frame.m_Bitpool = sbc->m_Bitpool;
		priv->m_Frame.m_Codesize = SBCGetCodesize(sbc);
		priv->m_Frame.m_Length = SBCGetFrameLength(sbc);

		SBCEncoderInit(&priv->m_EncState, &priv->m_Frame);
		priv->m_Init = 1;
	}

	/* input must be large enough to encode a complete frame */
	if (input_len < priv->m_Frame.m_Codesize)
		return 0;

	/* output must be large enough to receive the encoded frame */
	if (!output || output_len < priv->m_Frame.m_Length)
		return ERR_SPC;

	ptr = (char*)input;

	for (i = 0; i < priv->m_Frame.m_Subbands * priv->m_Frame.m_Blocks; i++) {
		for (ch = 0; ch < sbc->m_Channels; ch++) {
			unsigned short s;

			if (sbc->m_Swap)
				s = (ptr[0] & 0xff) << 8 | (ptr[1] & 0xff);
			else
				s = (ptr[0] & 0xff) | (ptr[1] & 0xff) << 8;
			ptr += 2;
			priv->m_Frame.m_PCMSample[ch][i] = s;
		}
	}

	samples = SBCAnalyzeAudio(&priv->m_EncState, &priv->m_Frame);

	framelen = SBCPackFrame(output, &priv->m_Frame, output_len);

	if (written)
		*written = framelen;

	return samples * sbc->m_Channels * 2;
}



