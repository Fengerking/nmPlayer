//
//  voRenderObjectInfo.m
//  voOSEng
//
//  Created by qian_siyao on 2/28/13.
//
//

#import "voRenderObjectInfo.h"

@implementation voRenderObjectInfo
@synthesize viewNumber;
@synthesize parentViewNumber;
@synthesize viewBorderType;
@synthesize viewEffect;
@synthesize viewType;
@synthesize viewZOrder;
@synthesize viewFrame;
@synthesize viewColor;
@synthesize viewBorderColor;
@synthesize imageData;
@synthesize textViewString = _textViewString;
@synthesize textEdgeViewString = _textEdgeViewString;
- (id)init
{
    if (nil != (self = [super init]))
    {
        viewNumber = 0;
        parentViewNumber = 0;
        viewType = BACKGROUN_VIEW;
        viewZOrder = 0;
        memset(&viewFrame, 0,sizeof(voViewRect));
        memset(&viewColor, 0,sizeof(voSubtitleRGBAColor));
        memset(&imageData, 0,sizeof(voSubtitleImageInfoData));
        self.textViewString = nil;
        self.textEdgeViewString = nil;
    }
    return self;
}

//- (BOOL)setImageData:(voSubtitleImageInfoData)image
//{
//    imageData = image;
//    memset(&imageData, 0,sizeof(voSubtitleImageInfoData));
//    
//    imageData.nHeight = image.nHeight;
//    imageData.nSize = image.nSize;
//    imageData.nType = image.nType;
//    imageData.nWidth = image.nWidth;
//    if (image.pPicData == NULL) {
//        return false;
//    }
//    if (imageData.pPicData != NULL) {
//        free(imageData.pPicData);
//    }
//    imageData.pPicData = (VO_PBYTE) malloc (imageData.nSize);
//    memset(imageData.pPicData, 0, imageData.nSize);
//    memcpy(imageData.pPicData, image.pPicData,imageData.nSize);
//    return true;
//}
//
//- (voSubtitleImageInfoData)getImageData
//{
//    return imageData;
//}

-(void)dealloc
{
    self.textViewString = nil;
    self.textEdgeViewString = nil;
    //free(imageData.pPicData);
    
    [super dealloc];
}
@end
