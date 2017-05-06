/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    decode.c

Abstract:

    Subband Codec decode file.

Author:

    Witten Wen 11-January-2010

Revision History:

*************************************************************************/

#include "sbc.h"
#include "sbc_tables.h"

static void SBCDecoderInit(struct sbc_decoder_state *state,
				const struct sbc_frame *frame)
{
	int i, ch;

	voSBCMemset(state->m_V, 0, sizeof(state->m_V));
	state->m_Subbands = frame->m_Subbands;

	for (ch = 0; ch < 2; ch++)
		for (i = 0; i < frame->m_Subbands * 2; i++)
			state->m_offset[ch][i] = (10 * i + 10);
}

/*
 * Unpacks a SBC frame at the beginning of the stream in data,
 * which has at most len bytes into frame.
 * Returns the length in bytes of the packed frame, or a negative
 * value on error. The error codes are:
 *
 *  -1   Data stream too short
 *  -2   Sync byte incorrect
 *  -3   CRC8 incorrect
 *  -4   Bitpool value out of bounds
 */
static int SBCUnpackFrame(const unsigned char *data, struct sbc_frame *frame,
				VO_U32 len)
{
	VO_U32 consumed;
	/* Will copy the parts of the header that are relevant to crc
	 * calculation here */
	unsigned char crc_header[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int crc_pos = 0;
	int temp;

	unsigned char sf;		/* sampling_frequency, temporarily needed as
				   array index */

	int ch, sb, blk, bit;	/* channel, subband, block and bit standard
				   counters */
	int bits[2][8];		/* bits distribution */
	int levels[2][8];	/* levels derived from that */

	if (len < 4)
		return -1;

	if (data[0] != VOSBC_SYNCWORD)
		return -2;

	sf = (data[1] >> 6) & 0x03;
	switch (sf) {
	case VOSBC_MODE_16:
		frame->m_SampleRate = 16000;
		break;
	case VOSBC_MODE_32:
		frame->m_SampleRate = 32000;
		break;
	case VOSBC_MODE_44:
		frame->m_SampleRate = 44100;
		break;
	case VOSBC_MODE_48:
		frame->m_SampleRate = 48000;
		break;
	}

	switch ((data[1] >> 4) & 0x03) {
	case NB4_VOSBC:
		frame->m_Blocks = 4;
		break;
	case NB8_VOSBC:
		frame->m_Blocks = 8;
		break;
	case NB12_VOSBC:
		frame->m_Blocks = 12;
		break;
	case NB16_VOSBC:
		frame->m_Blocks = 16;
		break;
	}

	frame->channel_mode = (data[1] >> 2) & 0x03;
	switch (frame->channel_mode) {
	case VOSBR_CH_MONO:
		frame->m_Channels = 1;
		break;
	case VOSBR_CH_DUAL_CHANNEL:	/* fall-through */
	case VOSBR_CH_STEREO:
	case VOSBR_CH_JOINT_STEREO:
		frame->m_Channels = 2;
		break;
	}

	frame->allocation_method = (data[1] >> 1) & 0x01;

	frame->m_Subbands = (data[1] & 0x01) ? 8 : 4;

	frame->m_Bitpool = data[2];

	if ((frame->channel_mode == VOSBR_CH_MONO || frame->channel_mode == VOSBR_CH_DUAL_CHANNEL) &&
			frame->m_Bitpool > 16 * frame->m_Subbands)
		return -4;

	if ((frame->channel_mode == VOSBR_CH_STEREO || frame->channel_mode == VOSBR_CH_JOINT_STEREO) &&
			frame->m_Bitpool > 32 * frame->m_Subbands)
		return -4;

	/* data[3] is crc, we're checking it later */

	consumed = 32;
//	psbcdec->
	crc_header[0] = data[1];
	crc_header[1] = data[2];
	crc_pos = 16;

	if (frame->channel_mode == VOSBR_CH_JOINT_STEREO) {
		if (len * 8 < consumed + frame->m_Subbands)
			return -1;

		frame->m_Join = 0x00;
		for (sb = 0; sb < frame->m_Subbands - 1; sb++)
			frame->m_Join |= ((data[4] >> (7 - sb)) & 0x01) << sb;
		if (frame->m_Subbands == 4)
			crc_header[crc_pos / 8] = data[4] & 0xf0;
		else
			crc_header[crc_pos / 8] = data[4];

		consumed += frame->m_Subbands;
		crc_pos += frame->m_Subbands;
	}

	if (len * 8 < consumed + (4 * frame->m_Subbands * frame->m_Channels))
		return -1;

	for (ch = 0; ch < frame->m_Channels; ch++) {
		for (sb = 0; sb < frame->m_Subbands; sb++) {
			/* FIXME assert(consumed % 4 == 0); */
			frame->m_ScaleFactor[ch][sb] =
				(data[consumed >> 3] >> (4 - (consumed & 0x7))) & 0x0F;
			crc_header[crc_pos >> 3] |=
				frame->m_ScaleFactor[ch][sb] << (4 - (crc_pos & 0x7));

			consumed += 4;
			crc_pos += 4;
		}
	}

	if (data[3] != SBCCrc8(crc_header, crc_pos))
		return -3;

	SBCCalculateBits(frame, bits, sf);

	for (blk = 0; blk < frame->m_Blocks; blk++) {
		for (ch = 0; ch < frame->m_Channels; ch++) {
			for (sb = 0; sb < frame->m_Subbands; sb++) {
				frame->m_AudioSample[blk][ch][sb] = 0;
				if (bits[ch][sb] == 0)
					continue;

				for (bit = 0; bit < bits[ch][sb]; bit++) {
					int b;	/* A bit */
					if (consumed > len * 8)
						return -1;

					b = (data[consumed >> 3] >> (7 - (consumed & 0x7))) & 0x01;
					frame->m_AudioSample[blk][ch][sb] |=
							b << (bits[ch][sb] - bit - 1);

					consumed++;
				}
			}
		}
	}

	for (ch = 0; ch < frame->m_Channels; ch++) {
		for (sb = 0; sb < frame->m_Subbands; sb++)
			levels[ch][sb] = (1 << bits[ch][sb]) - 1;
	}

	for (blk = 0; blk < frame->m_Blocks; blk++) {
		for (ch = 0; ch < frame->m_Channels; ch++) {
			for (sb = 0; sb < frame->m_Channels; sb++) {
				if (levels[ch][sb] > 0) {
					frame->m_SBSample[blk][ch][sb] =
						(((frame->m_AudioSample[blk][ch][sb] << 16) | 0x8000) /
							levels[ch][sb]) - 0x8000;

					frame->m_SBSample[blk][ch][sb] >>= 3;

					/* Q13 */
					frame->m_SBSample[blk][ch][sb] =
						(frame->m_SBSample[blk][ch][sb] <<
							(frame->m_ScaleFactor[ch][sb] + 1));

				} else
					frame->m_SBSample[blk][ch][sb] = 0;
			}
		}
	}

	if (frame->channel_mode == VOSBR_CH_JOINT_STEREO) {
		for (blk = 0; blk < frame->m_Blocks; blk++) {
			for (sb = 0; sb < frame->m_Subbands; sb++) {
				if (frame->m_Join & (0x01 << sb)) {
					temp = frame->m_SBSample[blk][0][sb] +
						frame->m_SBSample[blk][1][sb];
					frame->m_SBSample[blk][1][sb] =
						frame->m_SBSample[blk][0][sb] -
						frame->m_SBSample[blk][1][sb];
					frame->m_SBSample[blk][0][sb] = temp;
				}
			}
		}
	}

	if ((consumed & 0x7) != 0)
		consumed += 8 - (consumed & 0x7);

	return consumed >> 3;
}


static __inline void SBCSynthesize4(struct sbc_decoder_state *state,
				struct sbc_frame *frame, int ch, int blk)
{
	int i, j, k, idx;
	sbc_extended_t res;

	for (i = 0; i < 8; i++) {
		/* Shifting */
		state->m_offset[ch][i]--;
		if (state->m_offset[ch][i] < 0) {
			state->m_offset[ch][i] = 79;
			for (j = 0; j < 9; j++)
				state->m_V[ch][j+80] = state->m_V[ch][j];
		}
	}

	for (i = 0; i < 8; i++) {
		/* Distribute the new matrix value to the shifted position */
		FIXED_0(res);
		for (j = 0; j < 4; j++)
			MULA(res, voSyn4Table[i][j], frame->m_SBSample[blk][ch][j]);
		state->m_V[ch][state->m_offset[ch][i]] = (int)SCALE4_STAGED1(res);
	}

	/* Compute the samples */
	for (idx = 0, i = 0; i < 4; i++) {
		k = (i + 4) & 0xf;
		FIXED_0(res);
		for (j = 0; j < 10; idx++) {
			MULA(res, state->m_V[ch][state->m_offset[ch][i]+j++],
					voSBC4_m0_proto[idx]);
			MULA(res, state->m_V[ch][state->m_offset[ch][k]+j++],
					voSBC4_m1_proto[idx]);
		}

		/* Store in output, Q0 */
		frame->m_PCMSample[ch][blk * 4 + i] = (short)SCALE4_STAGED2(res);
	}
}

static __inline void SBCSynthesize8(struct sbc_decoder_state *state,
				struct sbc_frame *frame, int ch, int blk)
{
	int i, j, k, idx;
	sbc_extended_t res;

	for (i = 0; i < 16; i++) {
		/* Shifting */
		state->m_offset[ch][i]--;
		if (state->m_offset[ch][i] < 0) {
			state->m_offset[ch][i] = 159;
			for (j = 0; j < 9; j++)
				state->m_V[ch][j+160] = state->m_V[ch][j];
		}
	}

	for (i = 0; i < 16; i++) {
		/* Distribute the new matrix value to the shifted position */
		FIXED_0(res);
		for (j = 0; j < 8; j++) {
			/* Q28 = Q15 * Q13 */
			MULA(res, voSyn8Table[i][j], frame->m_SBSample[blk][ch][j]);
		}
		/* Q10 */
		state->m_V[ch][state->m_offset[ch][i]] = (int)SCALE8_STAGED1(res);
	}

	/* Compute the samples */
	for (idx = 0, i = 0; i < 8; i++) {
		k = (i + 8) & 0xf;
		FIXED_0(res);
		for (j = 0; j < 10; idx++) {
			MULA(res, state->m_V[ch][state->m_offset[ch][i]+j++], voSBC8_m0_proto[idx]);
			MULA(res, state->m_V[ch][state->m_offset[ch][k]+j++], voSBC8_m1_proto[idx]);
		}
		/* Store in output */
		frame->m_PCMSample[ch][blk * 8 + i] = (short)SCALE8_STAGED2(res); // Q0

	}
}


static int SBCSynthesizeAudio(struct sbc_decoder_state *state,
				struct sbc_frame *frame)
{
	int ch, blk;

	switch (frame->m_Subbands) {
	case 4:
		for (ch = 0; ch < frame->m_Channels; ch++) {
			for (blk = 0; blk < frame->m_Blocks; blk++)
				SBCSynthesize4(state, frame, ch, blk);
		}
		return frame->m_Blocks * 4;

	case 8:
		for (ch = 0; ch < frame->m_Channels; ch++) {
			for (blk = 0; blk < frame->m_Blocks; blk++)
				SBCSynthesize8(state, frame, ch, blk);
		}
		return frame->m_Blocks * 8;

	default:
		return ERR_IOPARAMETERS;
	}
}

int SBCDecode(sbc_t *psbc, VO_U8 *input, VO_U32 input_len, VO_U8 *output,
		      int output_len, VO_U32 *written)
{
	struct sbc_priv *priv;
	char *ptr;
	int i, ch, framelen, samples;

	if (!psbc && !input)
		return ERR_IOPARAMETERS;

	priv = psbc->m_pPriv;

	framelen = SBCUnpackFrame(input, &priv->m_Frame, input_len);

	if (!priv->m_Init) {
		SBCDecoderInit(&priv->m_DecState, &priv->m_Frame);
		priv->m_Init = 1;

		psbc->m_Rate		= priv->m_Frame.m_SampleRate;
		psbc->m_Channels	= priv->m_Frame.m_Channels;
		psbc->m_Subbands	= priv->m_Frame.m_Subbands;
		psbc->m_Blocks		= priv->m_Frame.m_Blocks;
		psbc->m_Bitpool		= priv->m_Frame.m_Bitpool;

		priv->m_Frame.m_CodeSize = SBCGetCodesize(psbc);
		psbc->m_FrameSize = priv->m_Frame.m_Length = SBCGetFrameLength(psbc);
		voSBCSetInit(psbc);
	}

	if (!output)/**/
		return framelen;

	if (written)/*SET LEN ZERO*/
		*written = 0;

	samples = SBCSynthesizeAudio(&priv->m_DecState, &priv->m_Frame);

	ptr = (char*)output;//÷∏œÚBUFF µÿ÷∑

	if (output_len < samples * priv->m_Frame.m_Channels * 2)
		samples = output_len / (priv->m_Frame.m_Channels * 2);

	for (i = 0; i < samples; i++) {
		for (ch = 0; ch < priv->m_Frame.m_Channels; ch++) {
			short s;
			s = priv->m_Frame.m_PCMSample[ch][i];		//PCM Data
			

			if (psbc->m_Swap) {
				*ptr++ = (s & 0xff00) >> 8;
				*ptr++ = (s & 0x00ff);
			} else {
				*ptr++ = (s & 0x00ff);
				*ptr++ = (s & 0xff00) >> 8;
			}
		}
	}

	if (written)
		*written = samples * priv->m_Frame.m_Channels * 2;

	return framelen;
}

