

#ifndef _CABAC_H_
#define _CABAC_H_

#include "global.h"

MotionInfoContexts* create_contexts_MotionInfo(ImageParameters *img);
TextureInfoContexts* create_contexts_TextureInfo(ImageParameters *img);
void init_contexts_MotionInfo(ImageParameters *img,TMBsProcessor *info,MotionInfoContexts *enco_ctx);
void init_contexts_TextureInfo(ImageParameters *img,TMBsProcessor *info,TextureInfoContexts *enco_ctx);
void delete_contexts_MotionInfo(ImageParameters *img,MotionInfoContexts *enco_ctx);
void delete_contexts_TextureInfo(ImageParameters *img,TextureInfoContexts *enco_ctx);

void cabac_new_slice();

void readMB_typeInfo_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se, Bitstream *dep_dp);
void readB8_typeInfo_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se, Bitstream *dep_dp);
void readIntraPredMode_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se, Bitstream *dep_dp);
void readRefFrame_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se, Bitstream *dep_dp);
void readMVD_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se, Bitstream *dep_dp);
void readCBP_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se, Bitstream *dep_dp);
void readDquant_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,Bitstream *dep_dp);
void readCIPredMode_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,Bitstream *dep_dp);
void readMB_skip_flagInfo_CABAC(ImageParameters *img,TMBsProcessor *info, SyntaxElement *se, Bitstream *dep_dp);
void readFieldModeInfo_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,Bitstream *dep_dp); 
void readFieldModeInfo_CABAC2(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,Bitstream *dep_dp,MacroBlock* left,MacroBlock* up); 

avdNativeInt  check_next_mb_and_get_field_mode_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,Bitstream *dep_dp);
void GetNeighborAB(ImageParameters *img,TMBsProcessor *info,avdNativeInt x4x4Offset, avdNativeInt y4x4Offset, MacroBlock *mbAB[], avdNativeInt y4x4AB[]);


#endif  // _CABAC_H_

