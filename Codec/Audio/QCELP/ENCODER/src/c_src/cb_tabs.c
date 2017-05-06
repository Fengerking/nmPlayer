/**********************************************************************
Each of the companies; Qualcomm, and Lucent (hereinafter 
referred to individually as "Source" or collectively as "Sources") do 
hereby state:

To the extent to which the Source(s) may legally and freely do so, the 
Source(s), upon submission of a Contribution, grant(s) a free, 
irrevocable, non-exclusive, license to the Third Generation Partnership 
Project 2 (3GPP2) and its Organizational Partners: ARIB, CCSA, TIA, TTA, 
and TTC, under the Source's copyright or copyright license rights in the 
Contribution, to, in whole or in part, copy, make derivative works, 
perform, display and distribute the Contribution and derivative works 
thereof consistent with 3GPP2's and each Organizational Partner's 
policies and procedures, with the right to (i) sublicense the foregoing 
rights consistent with 3GPP2's and each Organizational Partner's  policies 
and procedures and (ii) copyright and sell, if applicable) in 3GPP2's name 
or each Organizational Partner's name any 3GPP2 or transposed Publication 
even though this Publication may contain the Contribution or a derivative 
work thereof.  The Contribution shall disclose any known limitations on 
the Source's rights to license as herein provided.

When a Contribution is submitted by the Source(s) to assist the 
formulating groups of 3GPP2 or any of its Organizational Partners, it 
is proposed to the Committee as a basis for discussion and is not to 
be construed as a binding proposal on the Source(s).  The Source(s) 
specifically reserve(s) the right to amend or modify the material 
contained in the Contribution. Nothing contained in the Contribution 
shall, except as herein expressly provided, be construed as conferring 
by implication, estoppel or otherwise, any license or right under (i) 
any existing or later issuing patent, whether or not the use of 
information in the document necessarily employs an invention of any 
existing or later issued patent, (ii) any copyright, (iii) any 
trademark, or (iv) any other intellectual property right.

With respect to the Software necessary for the practice of any or 
all Normative portions of the QCELP-13 Variable Rate Speech Codec as 
it exists on the date of submittal of this form, should the QCELP-13 be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the QCELP-13 or the field of use of practice of the 
QCELP-13 Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/

#include "coder.h"

/* CODEBOOK_HALF[i] = CODEBOOK_HALF[i] * (1 << 11)  */
short CODEBOOK_HALF[CBLENGTH]= {
    0, -4096,     0, -3072,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,
    0, -3072, -2048,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,  5120,
    0,     0,     0,     0,     0,     0,  4096,     0,
    0,  3072,  2048,     0,  3072,  4096,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,  3072,     0,     0,
-3072,  3072,     0,     0, -2048,     0,  3072,     0,
    0,     0,     0,     0,     0,     0, -5120,     0,
    0,     0,     0,  3072,     0,     0,     0,  3072,
    0,     0,     0,     0,     0,     0,     0,  4096,
    0,     0,     0,     0,     0,     0,     0,     0,
    0,  3072,  6144, -3072, -4096,     0, -3072, -3072,
 3072, -3072,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0
}; 

/* CODEBOOK[i] = CODEBOOK[i] * (1 << 11)  */
short CODEBOOK[CBLENGTH]= {
  205, -1330, -1207,   246,  2253,   696, -2743,  3215,
 2130, -1719,  -695, -2354,   471, -2067,    61,   922,
-2067,  -327, -1207,   573,  -921,  2744, -1371,   451,
 1249,  -593,  4628,  -531, -1125, -3665,  3215, -1043,
-4505, -1904,  -757,  1229,  2417,  1516,  -982, -1945,
-3706,  2273,   737, -1064, -4402,  1597, -2293,   799,
 -347,  -962, -4566,   389,   246, -2006, -2907,  2662,
 1106, -2600,   430,  -245,   799,  -982,   246,  2621,
  123, -3419,  1679, -2088, -1617,  1126,  -900,   983,
 -409, -1084,   164, -1248,   225, -1433, -3214, -3440,
  410, -1146, -1515,  1597,   676, -1289, -3542,   -40,
-1535, -1084, -2989,  1577,  1352,  -593,   184, -1535,
 1331,  2437,  -880,  1556,  4772,  2007,  2560, -3194,
 -552,  1597,  -183,  3482,  3604,  2929, -3030,  -142,
  553, -2784,   102,   553,   369,  2847,  4178,   143,
-3767, -4034,  1065,   -60,  1597, -3870,   164, -1330
};

