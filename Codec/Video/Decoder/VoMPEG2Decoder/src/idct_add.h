
#ifdef REF_DCT
VO_VOID Initialize_Decoder();

VO_VOID Reference_IDCT(short *block);
VO_VOID Add_Block(short *src, VO_U8 *dst, int dst_stride, int addflag, VO_U8 *src1, VO_S32 src_stride);
//TBD
extern VO_U8 *Clip;

#endif