
#import "PlayerView.h"


@implementation PlayerView
+ (Class)layerClass {
    NSLog(@"-----------------return AVPlayerLayer");
    return [AVPlayerLayer class];
}

//- (AVPlayer*)player {
//    NSLog(@"-----------------return avlayer");
//    return [(AVPlayerLayer *)[self layer] player];
//}
//
//- (void)setPlayer:(AVPlayer *)player {
//    NSLog(@"------------------set avplayer");
//    [(AVPlayerLayer *)[self layer] setPlayer:player];
//}

@end