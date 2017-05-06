//
//  voRenderObjectInfo.h
//  voOSEng
//
//  Created by qian_siyao on 2/28/13.
//
//

#import <Foundation/Foundation.h>
#import "voSubtitleType.h"

typedef enum
{
    BACKGROUN_VIEW = 0,
    TEXT_VIEW = 1,
    IMAGE_VIEW = 2
}VIEW_TYPE;

typedef struct ViewRect
{
	float 	top;
	float	left;
	float 	bottom;
	float	right;
}voViewRect;

@interface voRenderObjectInfo : NSObject
{
@private
    int                            viewNumber;
    int                      parentViewNumber;
    int                            viewBorderType;
    int                            viewEffect;
    int                            viewZOrder;
    VIEW_TYPE                        viewType;
	voViewRect                            viewFrame;
	voSubtitleRGBAColor             viewColor;
    voSubtitleRGBAColor             viewBorderColor;
    NSMutableAttributedString *_textViewString;
    NSMutableAttributedString *_textEdgeViewString;
    voSubtitleImageInfoData         imageData;
}
@property(nonatomic, assign)int                  viewNumber;
@property(nonatomic, assign)int                  parentViewNumber;
@property(nonatomic, assign)int                  viewBorderType;
@property(nonatomic, assign)int                  viewEffect;
@property(nonatomic, assign)int                  viewZOrder;
@property(nonatomic, assign)VIEW_TYPE            viewType;
@property(nonatomic, assign)voViewRect                 viewFrame;
@property(nonatomic, assign)voSubtitleRGBAColor  viewColor;
@property(nonatomic, assign)voSubtitleRGBAColor  viewBorderColor;
@property(nonatomic, assign)voSubtitleImageInfoData    imageData;
@property(nonatomic, retain)NSMutableAttributedString *textViewString;
@property(nonatomic, retain)NSMutableAttributedString *textEdgeViewString;

- (id)init;
//- (BOOL)setImageData:(voSubtitleImageInfoData)image;
//- (voSubtitleImageInfoData)getImageData;
@end
