//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    sbccommon.c

Abstract:

    Subband Codec Common function and define file.

Author:

    Witten Wen 11-January-2010

Revision History:

*************************************************************************/

#include "voSBC.h"
#include "sbc.h"
#include "sbc_tables.h"
/*
 * Calculates the CRC-8 of the first len bits in data
 */
static const unsigned char crc_table[256] = {
	0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
	0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
	0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
	0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
	0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
	0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
	0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
	0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
	0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
	0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
	0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
	0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
	0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
	0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
	0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
	0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
	0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
	0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
	0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
	0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
	0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
	0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
	0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
	0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
	0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
	0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
	0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
	0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
	0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
	0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
	0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
	0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};


/* A2DP specification: Appendix B, page 69 */
static const int offset4_table[4][4] = {
	{ -1, 0, 0, 0 },
	{ -2, 0, 0, 1 },
	{ -2, 0, 0, 1 },
	{ -2, 0, 0, 1 }
};

/* A2DP specification: Appendix B, page 69 */
static const int offset8_table[4][8] = {
	{ -2, 0, 0, 0, 0, 0, 0, 1 },
	{ -3, 0, 0, 0, 0, 0, 1, 2 },
	{ -4, 0, 0, 0, 0, 0, 1, 2 },
	{ -4, 0, 0, 0, 0, 0, 1, 2 }
};

unsigned char SBCCrc8(const unsigned char *data, VO_U32 len)
{
	unsigned char crc = 0x0f;
	VO_U32 i;
	unsigned char octet;

	for (i = 0; i < len / 8; i++)
		crc = crc_table[crc ^ data[i]];

	octet = data[i];
	for (i = 0; i < len % 8; i++) {
		char bit = ((octet ^ crc) & 0x80) >> 7;

		crc = ((crc & 0x7f) << 1) ^ (bit ? 0x1d : 0);

		octet = octet << 1;
	}

	return crc;
}

/*
 * Code straight from the spec to calculate the bits array
 * Takes a pointer to the frame in question, a pointer to the bits array and
 * the sampling frequency (as 2 bit integer)
 */
void SBCCalculateBits(const struct sbc_frame *frame, int (*bits)[8], unsigned char sf)
{
	if (frame->channel_mode == VOSBR_CH_MONO || frame->channel_mode == VOSBR_CH_DUAL_CHANNEL) {
		int bitneed[2][8], loudness, max_bitneed, bitcount, slicecount, bitslice;
		int ch, sb;

		for (ch = 0; ch < frame->m_Channels; ch++) {
			max_bitneed = 0;
			if (frame->allocation_method == VOSBC_AM_SNR) {
				for (sb = 0; sb < frame->m_Subbands; sb++) {
					bitneed[ch][sb] = frame->m_ScaleFactor[ch][sb];
					if (bitneed[ch][sb] > max_bitneed)
						max_bitneed = bitneed[ch][sb];
				}
			} else {
				for (sb = 0; sb < frame->m_Subbands; sb++) {
					if (frame->m_ScaleFactor[ch][sb] == 0)
						bitneed[ch][sb] = -5;
					else {
						if (frame->m_Subbands == 4)//subband is 4
							loudness = frame->m_ScaleFactor[ch][sb] - offset4_table[sf][sb];
						else//subband is 8
							loudness = frame->m_ScaleFactor[ch][sb] - offset8_table[sf][sb];
						if (loudness > 0)
							bitneed[ch][sb] = loudness / 2;
						else
							bitneed[ch][sb] = loudness;
					}
					if (bitneed[ch][sb] > max_bitneed)
						max_bitneed = bitneed[ch][sb];
				}
			}

			bitcount = 0;
			slicecount = 0;
			bitslice = max_bitneed + 1;
			do {
				bitslice--;
				bitcount += slicecount;
				slicecount = 0;
				for (sb = 0; sb < frame->m_Subbands; sb++) {
					if ((bitneed[ch][sb] > bitslice + 1) && (bitneed[ch][sb] < bitslice + 16))
						slicecount++;
					else if (bitneed[ch][sb] == bitslice + 1)
						slicecount += 2;
				}
			} while (bitcount + slicecount < frame->m_Bitpool);

			if (bitcount + slicecount == frame->m_Bitpool) {
				bitcount += slicecount;
				bitslice--;
			}

			for (sb = 0; sb < frame->m_Subbands; sb++) {
				if (bitneed[ch][sb] < bitslice + 2)
					bits[ch][sb] = 0;
				else {
					bits[ch][sb] = bitneed[ch][sb] - bitslice;
					if (bits[ch][sb] > 16)
						bits[ch][sb] = 16;
				}
			}

			for (sb = 0; bitcount < frame->m_Bitpool && sb < frame->m_Subbands; sb++) {
				if ((bits[ch][sb] >= 2) && (bits[ch][sb] < 16)) {
					bits[ch][sb]++;
					bitcount++;
				} else if ((bitneed[ch][sb] == bitslice + 1) && (frame->m_Bitpool > bitcount + 1)) {
					bits[ch][sb] = 2;
					bitcount += 2;
				}
			}

			for (sb = 0; bitcount < frame->m_Bitpool && sb < frame->m_Subbands; sb++) {
				if (bits[ch][sb] < 16) {
					bits[ch][sb]++;
					bitcount++;
				}
			}

		}

	} else if (frame->channel_mode == VOSBR_CH_STEREO || frame->channel_mode == VOSBR_CH_JOINT_STEREO) {
		int bitneed[2][8], loudness, max_bitneed, bitcount, slicecount, bitslice;
		int ch, sb;

		max_bitneed = 0;
		if (frame->allocation_method == VOSBC_AM_SNR) {
			for (ch = 0; ch < 2; ch++) {
				for (sb = 0; sb < frame->m_Subbands; sb++) {
					bitneed[ch][sb] = frame->m_ScaleFactor[ch][sb];
					if (bitneed[ch][sb] > max_bitneed)
						max_bitneed = bitneed[ch][sb];
				}
			}
		} else {
			for (ch = 0; ch < 2; ch++) {
				for (sb = 0; sb < frame->m_Subbands; sb++) {
					if (frame->m_ScaleFactor[ch][sb] == 0)
						bitneed[ch][sb] = -5;
					else {
						if (frame->m_Subbands == 4)
							loudness = frame->m_ScaleFactor[ch][sb] - offset4_table[sf][sb];
						else
							loudness = frame->m_ScaleFactor[ch][sb] - offset8_table[sf][sb];
						if (loudness > 0)
							bitneed[ch][sb] = loudness / 2;
						else
							bitneed[ch][sb] = loudness;
					}
					if (bitneed[ch][sb] > max_bitneed)
						max_bitneed = bitneed[ch][sb];
				}
			}
		}

		bitcount = 0;
		slicecount = 0;
		bitslice = max_bitneed + 1;
		do {
			bitslice--;
			bitcount += slicecount;
			slicecount = 0;
			for (ch = 0; ch < 2; ch++) {
				for (sb = 0; sb < frame->m_Subbands; sb++) {
					if ((bitneed[ch][sb] > bitslice + 1) && (bitneed[ch][sb] < bitslice + 16))
						slicecount++;
					else if (bitneed[ch][sb] == bitslice + 1)
						slicecount += 2;
				}
			}
		} while (bitcount + slicecount < frame->m_Bitpool);

		if (bitcount + slicecount == frame->m_Bitpool) {
			bitcount += slicecount;
			bitslice--;
		}

		for (ch = 0; ch < 2; ch++) {
			for (sb = 0; sb < frame->m_Subbands; sb++) {
				if (bitneed[ch][sb] < bitslice + 2) {
					bits[ch][sb] = 0;
				} else {
					bits[ch][sb] = bitneed[ch][sb] - bitslice;
					if (bits[ch][sb] > 16)
						bits[ch][sb] = 16;
				}
			}
		}

		ch = 0;
		sb = 0;
		while (bitcount < frame->m_Bitpool) {
			if ((bits[ch][sb] >= 2) && (bits[ch][sb] < 16)) {
				bits[ch][sb]++;
				bitcount++;
			} else if ((bitneed[ch][sb] == bitslice + 1) && (frame->m_Bitpool > bitcount + 1)) {
				bits[ch][sb] = 2;
				bitcount += 2;
			}
			if (ch == 1) {
				ch = 0;
				sb++;
				if (sb >= frame->m_Subbands) break;
			} else
				ch = 1;
		}

		ch = 0;
		sb = 0;
		while (bitcount < frame->m_Bitpool) {
			if (bits[ch][sb] < 16) {
				bits[ch][sb]++;
				bitcount++;
			}
			if (ch == 1) {
				ch = 0;
				sb++;
				if (sb >= frame->m_Subbands) break;
			} else
				ch = 1;
		}

	}

}

static void SBCSetDefaults(sbc_t *sbc, unsigned long flags)
{
	sbc->m_Rate = 44100;
	sbc->m_Channels = 2;
	sbc->m_Joint = 0;
	sbc->m_Allocation = VOSBC_AM_LOUDNESS;
	sbc->m_Subbands = 8;
	sbc->m_Blocks = 16;
	sbc->m_Bitpool = 32;
	sbc->m_Swap = 0;
}

int SBCInit(sbc_t *psbc, unsigned long flags)
{
	if (!psbc)
		return ERR_IO;

	psbc->m_pPriv = voSBCMalloc(sizeof(struct sbc_priv));
	if (!psbc->m_pPriv)
		return ERR_MEMORY;

	voSBCMemset(psbc->m_pPriv, 0, sizeof(struct sbc_priv));

	SBCSetDefaults(psbc, flags);

	return 0;
}

void SBCFinish(sbc_t *psbc)
{
	if (!psbc)
		return;

	if (psbc->m_pPriv)
		voSBCFree(psbc->m_pPriv);

	voSBCMemset(psbc, 0, sizeof(sbc_t));
}

int SBCGetFrameLength(sbc_t *sbc)
{
	int ret;

	ret = 4 + (4 * sbc->m_Subbands * sbc->m_Channels) / 8;

	/* This term is not always evenly divide so we round it up */
	if (sbc->m_Channels == 1)
		ret += ((sbc->m_Blocks * sbc->m_Channels * sbc->m_Bitpool) + 7) / 8;
	else
		ret += (((sbc->m_Joint ? sbc->m_Subbands : 0) + sbc->m_Blocks * sbc->m_Bitpool)
			+ 7) / 8;

	return ret;
}

int SBCGetFrameDuration(sbc_t *sbc)
{
	return (1000000 * sbc->m_Blocks * sbc->m_Subbands) / sbc->m_Rate;
}

int SBCGetCodesize(sbc_t *sbc)
{
	return sbc->m_Subbands * sbc->m_Blocks * sbc->m_Channels * 2;
}

int SBCReinit(sbc_t *sbc, unsigned long flags)
{
	struct sbc_priv *priv;

	if (!sbc || !sbc->m_pPriv)
		return ERR_IO;

	priv = sbc->m_pPriv;

	if (priv->m_Init == 1)
		voSBCMemset(sbc->m_pPriv, 0, sizeof(struct sbc_priv));

	SBCSetDefaults(sbc, flags);

	return 0;
}


VO_U32 voSBCSetInit(sbc_t *psbc)
{
	if(!psbc)
		return VO_ERR_INVALID_ARG;

	if((psbc->m_pFramBuf = voSBCMalloc(psbc->m_FrameSize)) ==NULL)
		return VO_ERR_OUTOF_MEMORY;

	psbc->m_pNextBegin = psbc->m_pFramBuf;
	return VO_ERR_NONE;
}