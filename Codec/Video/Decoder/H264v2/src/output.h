

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

extern void output_one_picture(H264DEC_G *pDecGlobal, StorablePicture *p);

extern void write_stored_frame(H264DEC_G *pDecGlobal, FrameStore *fs);
extern void direct_output     (H264DEC_G *pDecGlobal, StorablePicture *p);

extern VO_S32 GetOutputPic(H264DEC_G *pDecGlobal, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo);


#endif //_OUTPUT_H_
