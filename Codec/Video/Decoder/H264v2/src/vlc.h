

#ifndef _VLC_H_
#define _VLC_H_
typedef struct VLC {
    int bits;
    VO_S16 (*table)[2]; ///< code, bits
    int table_size, table_allocated;
} VLC;
extern const VO_U8 VO_BIG_LEN_TAB[256];

static inline VO_S32 vo_big_len(VO_U32 val)
{
    int len = 0;
    if (val & 0xffff0000) {
        val >>= 16;
        len += 16;
    }
    if (val & 0xff00) {
        val >>= 8;
        len += 8;
    }
    len += VO_BIG_LEN_TAB[val];

    return len;
}
extern const VO_U8 VO_VLC_LEN[512];
extern const VO_U8 VO_UE_VAL[512];
extern const VO_S8 VO_SE_VAL[512];

extern int se_v (H264DEC_G *pDecGlobal);
extern int ue_v (H264DEC_G *pDecGlobal);
extern int ue_v_31 (H264DEC_G *pDecGlobal);

//extern Boolean u_1 (H264DEC_G *pDecGlobal, Bitstream *bitstream);
extern int u_v (int LenInBits, H264DEC_G *pDecGlobal);
extern int i_v (int LenInBits, H264DEC_G *pDecGlobal);

// CAVLC mapping
extern int  uvlc_startcode_follows(H264DEC_G *pDecGlobal,Slice *currSlice, int dummy);

extern int  readSyntaxElement_Intra4x4PredictionMode(H264DEC_G *pDecGlobal);

extern int more_rbsp_data (H264DEC_G *pDecGlobal);

#endif

