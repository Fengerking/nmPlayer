//
//  CiOSApiWrapper.mm
//
//  Created by Jim Lin on 1/14/13.
//  Copyright (c) 2013 VisualOn. All rights reserved.
//

#include "iOSApiWrapper.h"
#include "voLog.h"

#import <UIKit/UIKit.h>

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


const char* GetiOSVersion()
{
    VOLOGI("[NPW]System Version: %s", [[[UIDevice currentDevice] systemVersion] UTF8String]);
    
    return [[[UIDevice currentDevice] systemVersion] UTF8String];
}


#ifdef _VONAMESPACE
}
#endif
