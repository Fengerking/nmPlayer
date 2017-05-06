

/*
 * Description: The file contains the resampling by two functions 
 */ 
#include "ReSampleBy2.h"

#define voMul32_16(A, B, C) \
    (C + (B >> 16) * A + (((unsigned int)(0x0000FFFF & B) * A) >> 16))

// allpass filter coefficients.
static const unsigned short kResampleAllpass1[3] = {3284, 24441, 49528};
static const unsigned short kResampleAllpass2[3] = {12199, 37471, 60255};

// decimator
void voDownsampleBy2(const short* in, 
		     const short len,
                     short* out, 
		     int* filtState)
{
    int tmp1, tmp2, diff, in32, out32;
    short i;

    register int state0 = filtState[0];
    register int state1 = filtState[1];
    register int state2 = filtState[2];
    register int state3 = filtState[3];
    register int state4 = filtState[4];
    register int state5 = filtState[5];
    register int state6 = filtState[6];
    register int state7 = filtState[7];

    for (i = (len >> 1); i > 0; i--)
    {
        // lower allpass filter
        in32 = (int)(*in++) << 10;
        diff = in32 - state1;
        tmp1 = voMul32_16(kResampleAllpass2[0], diff, state0);
        state0 = in32;
        diff = tmp1 - state2;
        tmp2 = voMul32_16(kResampleAllpass2[1], diff, state1);
        state1 = tmp1;
        diff = tmp2 - state3;
        state3 = voMul32_16(kResampleAllpass2[2], diff, state2);
        state2 = tmp2;

        // upper allpass filter
        in32 = (int)(*in++) << 10;
        diff = in32 - state5;
        tmp1 = voMul32_16(kResampleAllpass1[0], diff, state4);
        state4 = in32;
        diff = tmp1 - state6;
        tmp2 = voMul32_16(kResampleAllpass1[1], diff, state5);
        state5 = tmp1;
        diff = tmp2 - state7;
        state7 = voMul32_16(kResampleAllpass1[2], diff, state6);
        state6 = tmp2;

        // add two allpass outputs, divide by two and round
        out32 = (state3 + state7 + 1024) >> 11;

        // limit amplitude to prevent wrap-around, and write to output array
        if (out32 > 32767)
            *out++ = 32767;
        else if (out32 < -32768)
            *out++ = -32768;
        else
            *out++ = (short)out32;
    }

    filtState[0] = state0;
    filtState[1] = state1;
    filtState[2] = state2;
    filtState[3] = state3;
    filtState[4] = state4;
    filtState[5] = state5;
    filtState[6] = state6;
    filtState[7] = state7;
}

void voUpsampleBy2(const short* in, 
					   short len, 
		               short* out,
                       int* filtState)
{
    int tmp1, tmp2, diff, in32, out32;
    short i;

    register int state0 = filtState[0];
    register int state1 = filtState[1];
    register int state2 = filtState[2];
    register int state3 = filtState[3];
    register int state4 = filtState[4];
    register int state5 = filtState[5];
    register int state6 = filtState[6];
    register int state7 = filtState[7];

    for (i = len; i > 0; i--)
    {
        // lower allpass filter
        in32 = (int)(*in++) << 10;
        diff = in32 - state1;
        tmp1 = voMul32_16(kResampleAllpass1[0], diff, state0);
        state0 = in32;
        diff = tmp1 - state2;
        tmp2 = voMul32_16(kResampleAllpass1[1], diff, state1);
        state1 = tmp1;
        diff = tmp2 - state3;
        state3 = voMul32_16(kResampleAllpass1[2], diff, state2);
        state2 = tmp2;

        // round; limit amplitude to prevent wrap-around; write to output array
        out32 = (state3 + 512) >> 10;
        if (out32 > 32767)
            *out++ = 32767;
        else if (out32 < -32768)
            *out++ = -32768;
        else
            *out++ = (short)out32;

        // upper allpass filter
        diff = in32 - state5;
        tmp1 = voMul32_16(kResampleAllpass2[0], diff, state4);
        state4 = in32;
        diff = tmp1 - state6;
        tmp2 = voMul32_16(kResampleAllpass2[1], diff, state5);
        state5 = tmp1;
        diff = tmp2 - state7;
        state7 = voMul32_16(kResampleAllpass2[2], diff, state6);
        state6 = tmp2;

        // round; limit amplitude to prevent wrap-around; write to output array
        out32 = (state7 + 512) >> 10;
        if (out32 > 32767)
            *out++ = 32767;
        else if (out32 < -32768)
            *out++ = -32768;
        else
            *out++ = (short)out32;
    }
    
    filtState[0] = state0;
    filtState[1] = state1;
    filtState[2] = state2;
    filtState[3] = state3;
    filtState[4] = state4;
    filtState[5] = state5;
    filtState[6] = state6;
    filtState[7] = state7;
}

