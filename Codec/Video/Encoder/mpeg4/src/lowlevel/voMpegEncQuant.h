/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _QUANT_H263_H_
#define _QUANT_H263_H_

extern VO_S32  DctQuantTrellis_c(VO_S16 *const Out,
					                        const VO_S16 *const In,
					                        VO_S32 Q,
					                        VO_S32 Non_Zero,
					                        VO_S32 Sum);

extern VO_VOID ExpandByteToShort_C(VO_S16 * const dst,
                                                    const VO_U8 * const src,
                                                    const VO_U32 stride);

extern VO_VOID ExpandByteToShort_ARMV6(VO_S16 * const dst,
                                                    const VO_U8 * const src,
                                                    const VO_U32 stride);

extern VO_VOID ExpandByteToShort_ARMV7(VO_S16 * const dst,
                                                    const VO_U8 * const src,
                                                    const VO_U32 stride);

extern VO_U32 QuantIntraH263_C(VO_S16 * coeff,
                                            const VO_S16 * data,
                                            const VO_U32 quant,
                                            const VO_U32 dcscalar);

extern VO_U32 QuantIntraH263_ARMV7(VO_S16 * coeff,
                                            const VO_S16 * data,
                                            const VO_U32 quant,
                                            const VO_U32 dcscalar);

extern VO_U32 QuantInterH263Mpeg4_C(VO_S16 * coeff,
                                                    const VO_S16 * data,
                                                    const VO_U32 quant);

extern VO_U32 QuantInter_ARMV4(VO_S16 * coeff,
                                                    const VO_S16 * data,
                                                    const VO_U32 quant);

extern VO_U32 QuantInter_ARMV7(VO_S16 * coeff,
                                                    const VO_S16 * data,
                                                    const VO_U32 quant);

extern VO_U32 QuantIntraMpeg4_C(VO_S16 * coeff,
                                                const VO_S16 * data,
                                                const VO_U32 quant,
                                                const VO_U32 dcscalar);

extern VO_U32 DequantIntraH263Mpeg4_C(VO_S16 * coeff,
                                                        const VO_S16 * data,
                                                        const VO_U32 quant,
                                                        const VO_U32 dcscalar,
                                                        const VO_U32 codec_id);

#if defined (VOWMMX)
#define ExpandByteToShort          ExpandByteToShort_C
#define QuantInterH263Mpeg4      quant_h263_inter_wmmx
#define dequant_h263_inter           dequant_h263_inter_wmmx
#define QuantIntraH263               QuantIntraH263_C
#define DequantIntraH263Mpeg4    DequantIntraH263Mpeg4_C
#elif defined (VOARMV4)
#define ExpandByteToShort              ExpandByteToShort_C
#define QuantInterH263Mpeg4          QuantInter_ARMV4
#define QuantIntraH263                   QuantIntraH263_C
#define QuantIntraMpeg4                 QuantIntraMpeg4_C
#define DequantIntraH263Mpeg4       DequantIntraH263Mpeg4_C
#elif defined (VOARMV6)
#define ExpandByteToShort              ExpandByteToShort_ARMV6
#define QuantInterH263Mpeg4          QuantInter_ARMV4
#define QuantIntraH263                   QuantIntraH263_C
#define QuantIntraMpeg4                 QuantIntraMpeg4_C
#define DequantIntraH263Mpeg4        DequantIntraH263Mpeg4_C
#elif defined (VOARMV7)
#define ExpandByteToShort              ExpandByteToShort_ARMV7
#define QuantInterH263Mpeg4          QuantInter_ARMV7 
#define QuantIntraH263                   QuantIntraH263_ARMV7
#define QuantIntraMpeg4                 QuantIntraMpeg4_C
#define DequantIntraH263Mpeg4       DequantIntraH263Mpeg4_C
#else
#define ExpandByteToShort          ExpandByteToShort_C
#define QuantInterH263Mpeg4      QuantInterH263Mpeg4_C
#define QuantIntraH263               QuantIntraH263_C
#define QuantIntraMpeg4             QuantIntraMpeg4_C
#define DequantIntraH263Mpeg4    DequantIntraH263Mpeg4_C
#endif

#endif /* _QUANT_H263_H_ */
