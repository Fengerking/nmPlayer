#include "struct.h"
#include "decoder.h"

int LatmGetValue(BitStream *bs)
{
	int bytesForValue = BitStreamGetBits(bs, 2);
	int value = 0;
	int i;

	for (i=0; i<=bytesForValue; i++) {
		value <<= 8;
		value |= BitStreamGetBits(bs, 8);
	}

	return value;
}

int GASpecificConfig(BitStream *bs, int profile)
{
	int ret=0;

	int framelen_flag;
	int dependsOnCoder;
	int ext_flag;
	int delay;
	int layerNr;

	framelen_flag = BitStreamGetBit(bs);		ret += 1;
	dependsOnCoder = BitStreamGetBit(bs);		ret += 1;

	if (dependsOnCoder) {
		delay = BitStreamGetBits(bs, 14);			ret += 14;
	}

	ext_flag = BitStreamGetBit(bs);				ret += 1;

	if (profile == 6 || profile == 20) {
		layerNr = BitStreamGetBits(bs, 3);			ret += 3;
	}

	if (ext_flag) {
		if (profile == 22) {
			BitStreamGetBits(bs, 5);				ret += 5;	// numOfSubFrame
			BitStreamGetBits(bs, 11);				ret += 11;	// layer_length
		}
		if (profile== 17  ||
			profile == 19 ||
			profile == 20 ||
			profile == 23) {

			BitStreamGetBits(bs, 3);				ret += 3;	// stuff
		}

		BitStreamGetBit(bs);						ret += 1;	// extflag3
	}
	return ret;
}

int	ReadAudioSpecConfig(AACDecoder*	decoder, BitStream *bs)
{
	int n, ret = 0;
	int profile,sampIdx,chanNum,sampFreq;
	int sbr_present = -1;

	profile = BitStreamGetBits(bs,5); ret += 5;
	if(profile==31)
	{
		profile = BitStreamGetBits(bs,6); ret += 6;
		profile +=32;
	}

	sampIdx = BitStreamGetBits(bs,4);  ret += 4;
	if(sampIdx==0x0f)
	{
		sampFreq = BitStreamGetBits(bs,24); ret += 24;
	}
	else
	{
		if(sampIdx < NUM_SAMPLE_RATES)
			sampFreq = sampRateTab[sampIdx];
		else
			return VO_ERR_AUDIO_UNSSAMPLERATE;
	}

	chanNum = BitStreamGetBits(bs,4);						ret += 4;

	decoder->sampleRate = sampFreq;
	decoder->profile = profile;
	decoder->channelNum = chanNum;

	if (profile == 5) {
		sbr_present = 1;
		sampIdx = BitStreamGetBits(bs, 4);					ret += 4;
		if (sampIdx == 0x0f) {
			sampFreq = BitStreamGetBits(bs, 24);			ret += 24;	
		}

		profile = BitStreamGetBits(bs, 5);					ret += 5;
		if (profile == 31) {
			n = BitStreamGetBits(bs, 6);
			profile = 32 + n;						        ret += 6;
		}
	}

	switch (profile) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 6:
	case 7:
	case 17:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
		ret += GASpecificConfig(bs, profile);
		break;
	}

	return ret;	
}

int PayloadLengthInfo(latm_header *latm, BitStream *bs)
{
	unsigned char tmp;
	if (latm->all_same_framing) {
		if (latm->frameLengthTypes[0] == 0) {
			latm->muxSlotLengthBytes[0] = 0;
			do {
				tmp = (unsigned char)BitStreamGetBits(bs, 8);
				latm->muxSlotLengthBytes[0] += tmp;
			} while (tmp == 255);
		} else {
			if (latm->frameLengthTypes[0] == 5 ||
				latm->frameLengthTypes[0] == 7 ||
				latm->frameLengthTypes[0] == 3) {
					latm->muxSlotLengthCoded[0] = (VO_U8)BitStreamGetBits(bs, 2);
			}
		}
	} else {
		return -1;
	}
	
	return 0;
}



int ReadMUXConfig(AACDecoder*	decoder, BitStream *bs)
{
	int use_same_mux;
	int frame_length_type, profile;
	latm_header *latm;

	latm = decoder->latm;

	use_same_mux = BitStreamGetBit(bs);
	
	if(use_same_mux)
	{		
		return 0;
	}

	latm->numSubFrames = 0;
	latm->audio_mux_version_A = 0;
	latm->audio_mux_version = BitStreamGetBit(bs);
	if (latm->audio_mux_version == 1) {				
		latm->audio_mux_version_A = BitStreamGetBit(bs);
	}

	if (latm->audio_mux_version_A == 0) {
		if (latm->audio_mux_version == 1) {			
			latm->taraFullness = LatmGetValue(bs);
		}

		latm->all_same_framing = BitStreamGetBit(bs);
		latm->numSubFrames = BitStreamGetBits(bs, 6);

		latm->numProgram = BitStreamGetBits(bs, 4);			
		if(latm->numProgram > 0) 
		{
			latm->numSubFrames = -1;
			return -1;
		}

		latm->numLayer = (VO_U8)BitStreamGetBits(bs, 3);
		if(latm->numLayer > 0) 
		{
			latm->numSubFrames = -1;
			return -1;
		}				

		if (latm->audio_mux_version == 0) {
			// audio specific config.
			if(ReadAudioSpecConfig(decoder, bs) < 0)
				return -1;
		} else {
			int ascLen = LatmGetValue(bs);

			int conlen = ReadAudioSpecConfig(decoder, bs);
			if(conlen < 0) return -1;

			ascLen -= conlen;							
			// fill bits
			BitStreamSkip(bs, ascLen);
		}

		profile = decoder->profile;
		// these are not needed... perhaps
		frame_length_type = BitStreamGetBits(bs, 3);
		latm->frameLengthTypes[0] = frame_length_type;
		if (frame_length_type == 0) {
			latm->latmBufferFullness[0] = (VO_U8)BitStreamGetBits(bs, 8);
		} else {
			if (frame_length_type == 1) {
				latm->frameLength[0] = (VO_U16)BitStreamGetBits(bs, 9);
			} else if (frame_length_type == 3 || frame_length_type == 4 ||frame_length_type == 5)  {
				BitStreamGetBits(bs, 6);
			} else if (frame_length_type == 6 || frame_length_type == 7) {
					BitStreamGetBit(bs);
			}
		}

		// other data
		latm->other_data_bits = 0;
		if (BitStreamGetBit(bs)) {
			// other data present
			if (latm->audio_mux_version == 1) {
				latm->other_data_bits = LatmGetValue(bs);
			} else {
				// other data not present
				int esc, tmp;
				latm->other_data_bits = 0;
				do {
					latm->other_data_bits <<= 8;
					esc = BitStreamGetBit(bs);
					tmp = BitStreamGetBits(bs, 8);
					latm->other_data_bits |= tmp;
				} while (esc);
			}
		}
		// CRC
		if (BitStreamGetBit(bs)) {
			latm->config_crc = (VO_U8)BitStreamGetBits(bs, 8);
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

int ParserLatm(AACDecoder*	decoder)
{
	int muxlength;
	BitStream *bs;
	latm_header *latm;

	bs = &(decoder->bs);
	latm = decoder->latm;

	if(BitStreamGetBits(bs, 11) != 0x2b7) 
		return VO_ERR_AAC_UNSPROFILE;		// just support AudioSyncStream() now

	muxlength = BitStreamGetBits(bs, 13);

	decoder->frame_length = muxlength + 3;
	if(muxlength + 3 > (int)decoder->Fstream.length)
		return VO_ERR_INPUT_BUFFER_SMALL;

	if(ReadMUXConfig(decoder, bs) < 0)
		return VO_ERR_AAC_UNSPROFILE;

	if(PayloadLengthInfo(latm, bs) < 0)
		return VO_ERR_AAC_UNSPROFILE;

	return 0;
}