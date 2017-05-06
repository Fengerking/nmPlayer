#include "voVP8Common.h"
#include "voVP8DecMBlock.h"
#include "voVP8DecFrame.h"
#include "voVP8Memory.h"

static const unsigned int default_coef_counts [BLOCK_TYPES] [COEF_BANDS] [PREV_COEF_CONTEXTS] [vp8_coef_tokens] =
{

    {
        // Block Type ( 0 )
        {
            // Coeff Band ( 0 )
            {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
        },
        {
            // Coeff Band ( 1 )
            {30190, 26544, 225,  24,   4,   0,   0,   0,   0,   0,   0, 4171593,},
            {26846, 25157, 1241, 130,  26,   6,   1,   0,   0,   0,   0, 149987,},
            {10484, 9538, 1006, 160,  36,  18,   0,   0,   0,   0,   0, 15104,},
        },
        {
            // Coeff Band ( 2 )
            {25842, 40456, 1126,  83,  11,   2,   0,   0,   0,   0,   0,   0,},
            {9338, 8010, 512,  73,   7,   3,   2,   0,   0,   0,   0, 43294,},
            {1047, 751, 149,  31,  13,   6,   1,   0,   0,   0,   0, 879,},
        },
        {
            // Coeff Band ( 3 )
            {26136, 9826, 252,  13,   0,   0,   0,   0,   0,   0,   0,   0,},
            {8134, 5574, 191,  14,   2,   0,   0,   0,   0,   0,   0, 35302,},
            { 605, 677, 116,   9,   1,   0,   0,   0,   0,   0,   0, 611,},
        },
        {
            // Coeff Band ( 4 )
            {10263, 15463, 283,  17,   0,   0,   0,   0,   0,   0,   0,   0,},
            {2773, 2191, 128,   9,   2,   2,   0,   0,   0,   0,   0, 10073,},
            { 134, 125,  32,   4,   0,   2,   0,   0,   0,   0,   0,  50,},
        },
        {
            // Coeff Band ( 5 )
            {10483, 2663,  23,   1,   0,   0,   0,   0,   0,   0,   0,   0,},
            {2137, 1251,  27,   1,   1,   0,   0,   0,   0,   0,   0, 14362,},
            { 116, 156,  14,   2,   1,   0,   0,   0,   0,   0,   0, 190,},
        },
        {
            // Coeff Band ( 6 )
            {40977, 27614, 412,  28,   0,   0,   0,   0,   0,   0,   0,   0,},
            {6113, 5213, 261,  22,   3,   0,   0,   0,   0,   0,   0, 26164,},
            { 382, 312,  50,  14,   2,   0,   0,   0,   0,   0,   0, 345,},
        },
        {
            // Coeff Band ( 7 )
            {   0,  26,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            {   0,  13,   0,   0,   0,   0,   0,   0,   0,   0,   0, 319,},
            {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   8,},
        },
    },
    {
        // Block Type ( 1 )
        {
            // Coeff Band ( 0 )
            {3268, 19382, 1043, 250,  93,  82,  49,  26,  17,   8,  25, 82289,},
            {8758, 32110, 5436, 1832, 827, 668, 420, 153,  24,   0,   3, 52914,},
            {9337, 23725, 8487, 3954, 2107, 1836, 1069, 399,  59,   0,   0, 18620,},
        },
        {
            // Coeff Band ( 1 )
            {12419, 8420, 452,  62,   9,   1,   0,   0,   0,   0,   0,   0,},
            {11715, 8705, 693,  92,  15,   7,   2,   0,   0,   0,   0, 53988,},
            {7603, 8585, 2306, 778, 270, 145,  39,   5,   0,   0,   0, 9136,},
        },
        {
            // Coeff Band ( 2 )
            {15938, 14335, 1207, 184,  55,  13,   4,   1,   0,   0,   0,   0,},
            {7415, 6829, 1138, 244,  71,  26,   7,   0,   0,   0,   0, 9980,},
            {1580, 1824, 655, 241,  89,  46,  10,   2,   0,   0,   0, 429,},
        },
        {
            // Coeff Band ( 3 )
            {19453, 5260, 201,  19,   0,   0,   0,   0,   0,   0,   0,   0,},
            {9173, 3758, 213,  22,   1,   1,   0,   0,   0,   0,   0, 9820,},
            {1689, 1277, 276,  51,  17,   4,   0,   0,   0,   0,   0, 679,},
        },
        {
            // Coeff Band ( 4 )
            {12076, 10667, 620,  85,  19,   9,   5,   0,   0,   0,   0,   0,},
            {4665, 3625, 423,  55,  19,   9,   0,   0,   0,   0,   0, 5127,},
            { 415, 440, 143,  34,  20,   7,   2,   0,   0,   0,   0, 101,},
        },
        {
            // Coeff Band ( 5 )
            {12183, 4846, 115,  11,   1,   0,   0,   0,   0,   0,   0,   0,},
            {4226, 3149, 177,  21,   2,   0,   0,   0,   0,   0,   0, 7157,},
            { 375, 621, 189,  51,  11,   4,   1,   0,   0,   0,   0, 198,},
        },
        {
            // Coeff Band ( 6 )
            {61658, 37743, 1203,  94,  10,   3,   0,   0,   0,   0,   0,   0,},
            {15514, 11563, 903, 111,  14,   5,   0,   0,   0,   0,   0, 25195,},
            { 929, 1077, 291,  78,  14,   7,   1,   0,   0,   0,   0, 507,},
        },
        {
            // Coeff Band ( 7 )
            {   0, 990,  15,   3,   0,   0,   0,   0,   0,   0,   0,   0,},
            {   0, 412,  13,   0,   0,   0,   0,   0,   0,   0,   0, 1641,},
            {   0,  18,   7,   1,   0,   0,   0,   0,   0,   0,   0,  30,},
        },
    },
    {
        // Block Type ( 2 )
        {
            // Coeff Band ( 0 )
            { 953, 24519, 628, 120,  28,  12,   4,   0,   0,   0,   0, 2248798,},
            {1525, 25654, 2647, 617, 239, 143,  42,   5,   0,   0,   0, 66837,},
            {1180, 11011, 3001, 1237, 532, 448, 239,  54,   5,   0,   0, 7122,},
        },
        {
            // Coeff Band ( 1 )
            {1356, 2220,  67,  10,   4,   1,   0,   0,   0,   0,   0,   0,},
            {1450, 2544, 102,  18,   4,   3,   0,   0,   0,   0,   0, 57063,},
            {1182, 2110, 470, 130,  41,  21,   0,   0,   0,   0,   0, 6047,},
        },
        {
            // Coeff Band ( 2 )
            { 370, 3378, 200,  30,   5,   4,   1,   0,   0,   0,   0,   0,},
            { 293, 1006, 131,  29,  11,   0,   0,   0,   0,   0,   0, 5404,},
            { 114, 387,  98,  23,   4,   8,   1,   0,   0,   0,   0, 236,},
        },
        {
            // Coeff Band ( 3 )
            { 579, 194,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            { 395, 213,   5,   1,   0,   0,   0,   0,   0,   0,   0, 4157,},
            { 119, 122,   4,   0,   0,   0,   0,   0,   0,   0,   0, 300,},
        },
        {
            // Coeff Band ( 4 )
            {  38, 557,  19,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            {  21, 114,  12,   1,   0,   0,   0,   0,   0,   0,   0, 427,},
            {   0,   5,   0,   0,   0,   0,   0,   0,   0,   0,   0,   7,},
        },
        {
            // Coeff Band ( 5 )
            {  52,   7,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            {  18,   6,   0,   0,   0,   0,   0,   0,   0,   0,   0, 652,},
            {   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,  30,},
        },
        {
            // Coeff Band ( 6 )
            { 640, 569,  10,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            {  25,  77,   2,   0,   0,   0,   0,   0,   0,   0,   0, 517,},
            {   4,   7,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,},
        },
        {
            // Coeff Band ( 7 )
            {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
        },
    },
    {
        // Block Type ( 3 )
        {
            // Coeff Band ( 0 )
            {2506, 20161, 2707, 767, 261, 178, 107,  30,  14,   3,   0, 100694,},
            {8806, 36478, 8817, 3268, 1280, 850, 401, 114,  42,   0,   0, 58572,},
            {11003, 27214, 11798, 5716, 2482, 2072, 1048, 175,  32,   0,   0, 19284,},
        },
        {
            // Coeff Band ( 1 )
            {9738, 11313, 959, 205,  70,  18,  11,   1,   0,   0,   0,   0,},
            {12628, 15085, 1507, 273,  52,  19,   9,   0,   0,   0,   0, 54280,},
            {10701, 15846, 5561, 1926, 813, 570, 249,  36,   0,   0,   0, 6460,},
        },
        {
            // Coeff Band ( 2 )
            {6781, 22539, 2784, 634, 182, 123,  20,   4,   0,   0,   0,   0,},
            {6263, 11544, 2649, 790, 259, 168,  27,   5,   0,   0,   0, 20539,},
            {3109, 4075, 2031, 896, 457, 386, 158,  29,   0,   0,   0, 1138,},
        },
        {
            // Coeff Band ( 3 )
            {11515, 4079, 465,  73,   5,  14,   2,   0,   0,   0,   0,   0,},
            {9361, 5834, 650,  96,  24,   8,   4,   0,   0,   0,   0, 22181,},
            {4343, 3974, 1360, 415, 132,  96,  14,   1,   0,   0,   0, 1267,},
        },
        {
            // Coeff Band ( 4 )
            {4787, 9297, 823, 168,  44,  12,   4,   0,   0,   0,   0,   0,},
            {3619, 4472, 719, 198,  60,  31,   3,   0,   0,   0,   0, 8401,},
            {1157, 1175, 483, 182,  88,  31,   8,   0,   0,   0,   0, 268,},
        },
        {
            // Coeff Band ( 5 )
            {8299, 1226,  32,   5,   1,   0,   0,   0,   0,   0,   0,   0,},
            {3502, 1568,  57,   4,   1,   1,   0,   0,   0,   0,   0, 9811,},
            {1055, 1070, 166,  29,   6,   1,   0,   0,   0,   0,   0, 527,},
        },
        {
            // Coeff Band ( 6 )
            {27414, 27927, 1989, 347,  69,  26,   0,   0,   0,   0,   0,   0,},
            {5876, 10074, 1574, 341,  91,  24,   4,   0,   0,   0,   0, 21954,},
            {1571, 2171, 778, 324, 124,  65,  16,   0,   0,   0,   0, 979,},
        },
        {
            // Coeff Band ( 7 )
            {   0,  29,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
            {   0,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0, 459,},
            {   0,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,  13,},
        },
    },
};

const unsigned int vp8_kf_default_bmode_counts [VP8_BINTRAMODES] [VP8_BINTRAMODES] [VP8_BINTRAMODES] =
{
    {
        //Above Mode :  0
        { 43438,   2195,    470,    316,    615,    171,    217,    412,    124,    160, }, // left_mode 0
        {  5722,   2751,    296,    291,     81,     68,     80,    101,    100,    170, }, // left_mode 1
        {  1629,    201,    307,     25,     47,     16,     34,     72,     19,     28, }, // left_mode 2
        {   332,    266,     36,    500,     20,     65,     23,     14,    154,    106, }, // left_mode 3
        {   450,     97,     10,     24,    117,     10,      2,     12,      8,     71, }, // left_mode 4
        {   384,     49,     29,     44,     12,    162,     51,      5,     87,     42, }, // left_mode 5
        {   495,     53,    157,     27,     14,     57,    180,     17,     17,     34, }, // left_mode 6
        {   695,     64,     62,      9,     27,      5,      3,    147,     10,     26, }, // left_mode 7
        {   230,     54,     20,    124,     16,    125,     29,     12,    283,     37, }, // left_mode 8
        {   260,     87,     21,    120,     32,     16,     33,     16,     33,    203, }, // left_mode 9
    },
    {
        //Above Mode :  1
        {  3934,   2573,    355,    137,    128,     87,    133,    117,     37,     27, }, // left_mode 0
        {  1036,   1929,    278,    135,     27,     37,     48,     55,     41,     91, }, // left_mode 1
        {   223,    256,    253,     15,     13,      9,     28,     64,      3,      3, }, // left_mode 2
        {   120,    129,     17,    316,     15,     11,      9,      4,     53,     74, }, // left_mode 3
        {   129,     58,      6,     11,     38,      2,      0,      5,      2,     67, }, // left_mode 4
        {    53,     22,     11,     16,      8,     26,     14,      3,     19,     12, }, // left_mode 5
        {    59,     26,     61,     11,      4,      9,     35,     13,      8,      8, }, // left_mode 6
        {   101,     52,     40,      8,      5,      2,      8,     59,      2,     20, }, // left_mode 7
        {    48,     34,     10,     52,      8,     15,      6,      6,     63,     20, }, // left_mode 8
        {    96,     48,     22,     63,     11,     14,      5,      8,      9,     96, }, // left_mode 9
    },
    {
        //Above Mode :  2
        {   709,    461,    506,     36,     27,     33,    151,     98,     24,      6, }, // left_mode 0
        {   201,    375,    442,     27,     13,      8,     46,     58,      6,     19, }, // left_mode 1
        {   122,    140,    417,      4,     13,      3,     33,     59,      4,      2, }, // left_mode 2
        {    36,     17,     22,     16,      6,      8,     12,     17,      9,     21, }, // left_mode 3
        {    51,     15,      7,      1,     14,      0,      4,      5,      3,     22, }, // left_mode 4
        {    18,     11,     30,      9,      7,     20,     11,      5,      2,      6, }, // left_mode 5
        {    38,     21,    103,      9,      4,     12,     79,     13,      2,      5, }, // left_mode 6
        {    64,     17,     66,      2,     12,      4,      2,     65,      4,      5, }, // left_mode 7
        {    14,      7,      7,     16,      3,     11,      4,     13,     15,     16, }, // left_mode 8
        {    36,      8,     32,      9,      9,      4,     14,      7,      6,     24, }, // left_mode 9
    },
    {
        //Above Mode :  3
        {  1340,    173,     36,    119,     30,     10,     13,     10,     20,     26, }, // left_mode 0
        {   156,    293,     26,    108,      5,     16,      2,      4,     23,     30, }, // left_mode 1
        {    60,     34,     13,      7,      3,      3,      0,      8,      4,      5, }, // left_mode 2
        {    72,     64,      1,    235,      3,      9,      2,      7,     28,     38, }, // left_mode 3
        {    29,     14,      1,      3,      5,      0,      2,      2,      5,     13, }, // left_mode 4
        {    22,      7,      4,     11,      2,      5,      1,      2,      6,      4, }, // left_mode 5
        {    18,     14,      5,      6,      4,      3,     14,      0,      9,      2, }, // left_mode 6
        {    41,     10,      7,      1,      2,      0,      0,     10,      2,      1, }, // left_mode 7
        {    23,     19,      2,     33,      1,      5,      2,      0,     51,      8, }, // left_mode 8
        {    33,     26,      7,     53,      3,      9,      3,      3,      9,     19, }, // left_mode 9
    },
    {
        //Above Mode :  4
        {   410,    165,     43,     31,     66,     15,     30,     54,      8,     17, }, // left_mode 0
        {   115,     64,     27,     18,     30,      7,     11,     15,      4,     19, }, // left_mode 1
        {    31,     23,     25,      1,      7,      2,      2,     10,      0,      5, }, // left_mode 2
        {    17,      4,      1,      6,      8,      2,      7,      5,      5,     21, }, // left_mode 3
        {   120,     12,      1,      2,     83,      3,      0,      4,      1,     40, }, // left_mode 4
        {     4,      3,      1,      2,      1,      2,      5,      0,      3,      6, }, // left_mode 5
        {    10,      2,     13,      6,      6,      6,      8,      2,      4,      5, }, // left_mode 6
        {    58,     10,      5,      1,     28,      1,      1,     33,      1,      9, }, // left_mode 7
        {     8,      2,      1,      4,      2,      5,      1,      1,      2,     10, }, // left_mode 8
        {    76,      7,      5,      7,     18,      2,      2,      0,      5,     45, }, // left_mode 9
    },
    {
        //Above Mode :  5
        {   444,     46,     47,     20,     14,    110,     60,     14,     60,      7, }, // left_mode 0
        {    59,     57,     25,     18,      3,     17,     21,      6,     14,      6, }, // left_mode 1
        {    24,     17,     20,      6,      4,     13,      7,      2,      3,      2, }, // left_mode 2
        {    13,     11,      5,     14,      4,      9,      2,      4,     15,      7, }, // left_mode 3
        {     8,      5,      2,      1,      4,      0,      1,      1,      2,     12, }, // left_mode 4
        {    19,      5,      5,      7,      4,     40,      6,      3,     10,      4, }, // left_mode 5
        {    16,      5,      9,      1,      1,     16,     26,      2,     10,      4, }, // left_mode 6
        {    11,      4,      8,      1,      1,      4,      4,      5,      4,      1, }, // left_mode 7
        {    15,      1,      3,      7,      3,     21,      7,      1,     34,      5, }, // left_mode 8
        {    18,      5,      1,      3,      4,      3,      7,      1,      2,      9, }, // left_mode 9
    },
    {
        //Above Mode :  6
        {   476,    149,     94,     13,     14,     77,    291,     27,     23,      3, }, // left_mode 0
        {    79,     83,     42,     14,      2,     12,     63,      2,      4,     14, }, // left_mode 1
        {    43,     36,     55,      1,      3,      8,     42,     11,      5,      1, }, // left_mode 2
        {     9,      9,      6,     16,      1,      5,      6,      3,     11,     10, }, // left_mode 3
        {    10,      3,      1,      3,     10,      1,      0,      1,      1,      4, }, // left_mode 4
        {    14,      6,     15,      5,      1,     20,     25,      2,      5,      0, }, // left_mode 5
        {    28,      7,     51,      1,      0,      8,    127,      6,      2,      5, }, // left_mode 6
        {    13,      3,      3,      2,      3,      1,      2,      8,      1,      2, }, // left_mode 7
        {    10,      3,      3,      3,      3,      8,      2,      2,      9,      3, }, // left_mode 8
        {    13,      7,     11,      4,      0,      4,      6,      2,      5,      8, }, // left_mode 9
    },
    {
        //Above Mode :  7
        {   376,    135,    119,      6,     32,      8,     31,    224,      9,      3, }, // left_mode 0
        {    93,     60,     54,      6,     13,      7,      8,     92,      2,     12, }, // left_mode 1
        {    74,     36,     84,      0,      3,      2,      9,     67,      2,      1, }, // left_mode 2
        {    19,      4,      4,      8,      8,      2,      4,      7,      6,     16, }, // left_mode 3
        {    51,      7,      4,      1,     77,      3,      0,     14,      1,     15, }, // left_mode 4
        {     7,      7,      5,      7,      4,      7,      4,      5,      0,      3, }, // left_mode 5
        {    18,      2,     19,      2,      2,      4,     12,     11,      1,      2, }, // left_mode 6
        {   129,      6,     27,      1,     21,      3,      0,    189,      0,      6, }, // left_mode 7
        {     9,      1,      2,      8,      3,      7,      0,      5,      3,      3, }, // left_mode 8
        {    20,      4,      5,     10,      4,      2,      7,     17,      3,     16, }, // left_mode 9
    },
    {
        //Above Mode :  8
        {   617,     68,     34,     79,     11,     27,     25,     14,     75,     13, }, // left_mode 0
        {    51,     82,     21,     26,      6,     12,     13,      1,     26,     16, }, // left_mode 1
        {    29,      9,     12,     11,      3,      7,      1,     10,      2,      2, }, // left_mode 2
        {    17,     19,     11,     74,      4,      3,      2,      0,     58,     13, }, // left_mode 3
        {    10,      1,      1,      3,      4,      1,      0,      2,      1,      8, }, // left_mode 4
        {    14,      4,      5,      5,      1,     13,      2,      0,     27,      8, }, // left_mode 5
        {    10,      3,      5,      4,      1,      7,      6,      4,      5,      1, }, // left_mode 6
        {    10,      2,      6,      2,      1,      1,      1,      4,      2,      1, }, // left_mode 7
        {    14,      8,      5,     23,      2,     12,      6,      2,    117,      5, }, // left_mode 8
        {     9,      6,      2,     19,      1,      6,      3,      2,      9,      9, }, // left_mode 9
    },
    {
        //Above Mode :  9
        {   680,     73,     22,     38,     42,      5,     11,      9,      6,     28, }, // left_mode 0
        {   113,    112,     21,     22,     10,      2,      8,      4,      6,     42, }, // left_mode 1
        {    44,     20,     24,      6,      5,      4,      3,      3,      1,      2, }, // left_mode 2
        {    40,     23,      7,     71,      5,      2,      4,      1,      7,     22, }, // left_mode 3
        {    85,      9,      4,      4,     17,      2,      0,      3,      2,     23, }, // left_mode 4
        {    13,      4,      2,      6,      1,      7,      0,      1,      7,      6, }, // left_mode 5
        {    26,      6,      8,      3,      2,      3,      8,      1,      5,      4, }, // left_mode 6
        {    54,      8,      9,      6,      7,      0,      1,     11,      1,      3, }, // left_mode 7
        {     9,     10,      4,     13,      2,      5,      4,      2,     14,      8, }, // left_mode 8
        {    92,      9,      5,     19,     15,      3,      3,      1,      6,     58, }, // left_mode 9
    },
};

const unsigned int vp8dx_bitreader_norm[256] =
{
    0, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

struct vp8_token_struct vp8_bmode_encodings   [VP8_BINTRAMODES];
struct vp8_token_struct vp8_ymode_encodings   [VP8_YMODES];
struct vp8_token_struct vp8_kf_ymode_encodings [VP8_YMODES];
struct vp8_token_struct vp8_uv_mode_encodings  [VP8_UV_MODES];
//struct vp8_token_struct vp8_mbsplit_encodings [VP8_NUMMBSPLITS];

static const unsigned int kf_y_mode_cts[VP8_YMODES] = { 1607, 915, 812, 811, 5455};
static const unsigned int y_mode_cts  [VP8_YMODES] = { 8080, 1908, 1582, 1007, 5874};

static const unsigned int uv_mode_cts  [VP8_UV_MODES] = { 59483, 13605, 16492, 4230};
static const unsigned int kf_uv_mode_cts[VP8_UV_MODES] = { 5319, 1904, 1703, 674};

static const unsigned char sub_mv_ref_prob [VP8_SUBMVREFS-1] = { 180, 162, 25};
static const unsigned int bmode_cts[VP8_BINTRAMODES] =
{
    43891, 17694, 10036, 3920, 3363, 2546, 5119, 3221, 2471, 1723
};
const vp8_tree_index vp8_ymode_tree[8] =
{
    -DC_PRED, 2,
    4, 6,
    -V_PRED, -H_PRED,
    -TM_PRED, -B_PRED
};

const vp8_tree_index vp8_kf_ymode_tree[8] =
{
    -B_PRED, 2,
    4, 6,
    -DC_PRED, -V_PRED,
    -H_PRED, -TM_PRED
};

const vp8_tree_index vp8_uv_mode_tree[6] =
{
    -DC_PRED, 2,
    -V_PRED, 4,
    -H_PRED, -TM_PRED
};

const vp8_tree_index vp8_mbsplit_tree[6] =
{
    -3, 2,
    -2, 4,
    -0, -1
};
//vp8_mbsplit vp8_mbsplits [VP8_NUMMBSPLITS] =
//{
//    {
//        0,  0,  0,  0,
//        0,  0,  0,  0,
//        1,  1,  1,  1,
//        1,  1,  1,  1,
//    },
//    {
//        0,  0,  1,  1,
//        0,  0,  1,  1,
//        0,  0,  1,  1,
//        0,  0,  1,  1,
//    },
//    {
//        0,  0,  1,  1,
//        0,  0,  1,  1,
//        2,  2,  3,  3,
//        2,  2,  3,  3,
//    },
//    {
//        0,  1,  2,  3,
//        4,  5,  6,  7,
//        8,  9,  10, 11,
//        12, 13, 14, 15,
//    },
//};
const vp8_tree_index vp8_bmode_tree[18] =     /* INTRAMODECONTEXTNODE value */
{
    -B_DC_PRED, 2,                             /* 0 = DC_NODE */
    -B_TM_PRED, 4,                            /* 1 = TM_NODE */
    -B_VE_PRED, 6,                           /* 2 = VE_NODE */
    8, 12,                                  /* 3 = COM_NODE */
    -B_HE_PRED, 10,                        /* 4 = HE_NODE */
    -B_RD_PRED, -B_VR_PRED,               /* 5 = RD_NODE */
    -B_LD_PRED, 14,                        /* 6 = LD_NODE */
    -B_VL_PRED, 16,                      /* 7 = VL_NODE */
    -B_HD_PRED, -B_HU_PRED             /* 8 = HD_NODE */
};

//extern const vp8_tree_index vp8_mv_ref_tree[8];
//extern const vp8_tree_index vp8_sub_mv_ref_tree[6];
//extern const vp8_tree_index vp8_small_mvtree [14];

//struct vp8_token_struct vp8_small_mvencodings [8];
struct vp8_token_struct vp8_coef_encodings[vp8_coef_tokens];

const vp8_tree_index vp8_coef_tree[ 22] =     /* corresponding _CONTEXT_NODEs */
{
    -DCT_EOB_TOKEN, 2,                             /* 0 = EOB */
    -ZERO_TOKEN, 4,                               /* 1 = ZERO */
    -ONE_TOKEN, 6,                               /* 2 = ONE */
    8, 12,                                      /* 3 = LOW_VAL */
    -TWO_TOKEN, 10,                            /* 4 = TWO */
    -THREE_TOKEN, -FOUR_TOKEN,                /* 5 = THREE */
    14, 16,                                    /* 6 = HIGH_LOW */
    -DCT_VAL_CATEGORY1, -DCT_VAL_CATEGORY2,   /* 7 = CAT_ONE */
    18, 20,                                   /* 8 = CAT_THREEFOUR */
    -DCT_VAL_CATEGORY3, -DCT_VAL_CATEGORY4,  /* 9 = CAT_THREE */
    -DCT_VAL_CATEGORY5, -DCT_VAL_CATEGORY6   /* 10 = CAT_FIVE */
};

//static vp8_tree_index cat1[2], cat2[4], cat3[6], cat4[8], cat5[10], cat6[22];

static void tree2tok(struct vp8_token_struct *const p, vp8_tree t, int i, int v, int L)
{
    v += v;
    ++L;

    do
    {
        const vp8_tree_index j = t[i++];

        if (j <= 0)
        {
            p[-j].value = v;
            p[-j].Len = L;
        }
        else
            tree2tok(p, t, j, v, L);
    }
    while (++v & 1);
}

void vp8_tokens_from_tree(struct vp8_token_struct *p, vp8_tree t)
{
    tree2tok(p, t, 0, 0, 0);
}

static void branch_counts(
    int n,                      /* n = size of alphabet */
    vp8_token tok               [ /* n */ ],
    vp8_tree tree,
    unsigned int branch_ct       [ /* n-1 */ ] [2],
    const unsigned int num_events[ /* n */ ]
)
{
    const int tree_len = n - 1;
    int t = 0;

    do
    {
        branch_ct[t][0] = branch_ct[t][1] = 0;
    }
    while (++t < tree_len);

    t = 0;

    do
    {
        int L = tok[t].Len;
        const int enc = tok[t].value;
        const unsigned int ct = num_events[t];

        vp8_tree_index i = 0;

        do
        {
            const int b = (enc >> --L) & 1;
            const int j = i >> 1;
            branch_ct [j] [b] += ct;
            i = tree[ i + b];
        }
        while (i > 0);
    }
    while (++t < n);
}

void vp8_tree_probs_from_distribution(
    int n,                      /* n = size of alphabet */
    vp8_token tok               [ /* n */ ],
    vp8_tree tree,
    unsigned char probs          [ /* n-1 */ ],
    unsigned int branch_ct       [ /* n-1 */ ] [2],
    const unsigned int num_events[ /* n */ ],
    unsigned int Pfac,
    int rd
)
{
    const int tree_len = n - 1;
    int t = 0;

    branch_counts(n, tok, tree, branch_ct, num_events);

    do
    {
        const unsigned int *const c = branch_ct[t];
        const unsigned int tot = c[0] + c[1];

        if (tot)
        {
            const unsigned int p = ((c[0] * Pfac) + (rd ? tot >> 1 : 0)) / tot;
            probs[t] = p < 256 ? (p ? p : 1) : 255; /* agree w/old version for now */
        }
        else
            probs[t] = vp8_prob_half;
    }
    while (++t < tree_len);
}

void vp8_init_mbmode_probs(VP8_COMMON *x)
{
    unsigned int bct [VP8_YMODES] [2];      /* num Ymodes > num UV modes */

    vp8_tree_probs_from_distribution(
        VP8_YMODES, vp8_ymode_encodings, vp8_ymode_tree,
        x->fc.ymode_prob, bct, y_mode_cts,
        256, 1
    );
    vp8_tree_probs_from_distribution(
        VP8_YMODES, vp8_kf_ymode_encodings, vp8_kf_ymode_tree,
        x->kf_ymode_prob, bct, kf_y_mode_cts,
        256, 1
    );
    vp8_tree_probs_from_distribution(
        VP8_UV_MODES, vp8_uv_mode_encodings, vp8_uv_mode_tree,
        x->fc.uv_mode_prob, bct, uv_mode_cts,
        256, 1
    );
    vp8_tree_probs_from_distribution(
        VP8_UV_MODES, vp8_uv_mode_encodings, vp8_uv_mode_tree,
        x->kf_uv_mode_prob, bct, kf_uv_mode_cts,
        256, 1
    );
	CopyMem(x->pUserData, x->nCodecIdx, (VO_U8*)(x->fc.sub_mv_ref_prob), (VO_U8*)sub_mv_ref_prob,  sizeof(sub_mv_ref_prob));
}
static void intra_bmode_probs_from_distribution(
    unsigned char p [VP8_BINTRAMODES-1],
    unsigned int branch_ct [VP8_BINTRAMODES-1] [2],
    const unsigned int events [VP8_BINTRAMODES]
)
{
    vp8_tree_probs_from_distribution(
        VP8_BINTRAMODES, vp8_bmode_encodings, vp8_bmode_tree,
        p, branch_ct, events,
        256, 1
    );
	return;
}
void vp8_default_coef_probs(VP8_COMMON *pc)
{
    int h = 0;

    do
    {
        int i = 0;
        do
        {
            int k = 0;
            do
            {
                unsigned int branch_ct [vp8_coef_tokens-1] [2];
                vp8_tree_probs_from_distribution(
                    vp8_coef_tokens, vp8_coef_encodings, vp8_coef_tree,
                    pc->fc.coef_probs [h][i][k], branch_ct, default_coef_counts [h][i][k],
                    256, 1);
            }
            while (++k < PREV_COEF_CONTEXTS);
        }
        while (++i < COEF_BANDS);
    }
    while (++h < BLOCK_TYPES);
}

void vp8_default_bmode_probs(unsigned char p [VP8_BINTRAMODES-1])
{
    unsigned int branch_ct [VP8_BINTRAMODES-1] [2];
    intra_bmode_probs_from_distribution(p, branch_ct, bmode_cts);
	return;
}

void vp8_kf_default_bmode_probs(unsigned char p [VP8_BINTRAMODES] [VP8_BINTRAMODES] [VP8_BINTRAMODES-1])
{
    unsigned int branch_ct [VP8_BINTRAMODES-1] [2];

    int i = 0;

    do
    {
        int j = 0;

        do
        {
            intra_bmode_probs_from_distribution(
                p[i][j], branch_ct, vp8_kf_default_bmode_counts[i][j]);

        }
        while (++j < VP8_BINTRAMODES);
    }
    while (++i < VP8_BINTRAMODES);
}


void vp8_entropy_mode_init()
{
    vp8_tokens_from_tree(vp8_bmode_encodings,   vp8_bmode_tree);
    vp8_tokens_from_tree(vp8_ymode_encodings,   vp8_ymode_tree);
    vp8_tokens_from_tree(vp8_kf_ymode_encodings, vp8_kf_ymode_tree);
    vp8_tokens_from_tree(vp8_uv_mode_encodings,  vp8_uv_mode_tree);

    //vp8_tokens_from_tree(vp8_mbsplit_encodings, vp8_mbsplit_tree);
    //vp8_tokens_from_tree(VP8_MVREFENCODINGS,   vp8_mv_ref_tree);
    //vp8_tokens_from_tree(VP8_SUBMVREFENCODINGS, vp8_sub_mv_ref_tree);
    //vp8_tokens_from_tree(vp8_small_mvencodings, vp8_small_mvtree);
}
int vp8dx_start_decode_c(BOOL_DECODER *br, const unsigned char *source,
                        unsigned int source_sz)
{
    br->user_buffer_end = source+source_sz;
    br->user_buffer     = source;
	br->pos               =  0;
    br->value    = 0;
    br->count    = -8;
    br->range    = 255;

    if (source_sz && !source)
        return 1;

    /* Populate the buffer */
    vp8dx_bool_decoder_fill_c(br);

    return 0;
}
int vp8dx_start_decode(BOOL_DECODER *br, const unsigned char *source, unsigned int source_sz) 
{
    return vp8dx_start_decode_c(br, source, source_sz);
}
//static void init_bit_tree(vp8_tree_index *p, int n)
//{
//    int i = 0;
//
//    while (++i < n)
//    {
//        p[0] = p[1] = i << 1;
//        p += 2;
//    }
//
//    p[0] = p[1] = 0;
//}
//
//static void init_bit_trees()
//{
//    init_bit_tree(cat1, 1);
//    init_bit_tree(cat2, 2);
//    init_bit_tree(cat3, 3);
//    init_bit_tree(cat4, 4);
//    init_bit_tree(cat5, 5);
//    init_bit_tree(cat6, 11);
//}

void vp8_coef_tree_initialize()
{
    //init_bit_trees();
    vp8_tokens_from_tree(vp8_coef_encodings, vp8_coef_tree);
}
