
#import "VOCommonPlayerHDMIImpl.h"
#import <UIKit/UIKit.h>
#import <ExternalAccessory/ExternalAccessory.h>

static NSString *pHDMIList = @"A1388,A1422";

@interface VOCommonPlayerHDMIImpl()

@property (nonatomic, retain) EAAccessory *accessory;
@property (nonatomic, assign) BOOL enableHDMIDetection;
@property (assign) id <onHDMIConnectionChangeDelegate> delegate;
@end

@implementation VOCommonPlayerHDMIImpl
@synthesize delegate = _delegate;
@synthesize enableHDMIDetection = _enableHDMIDetection;
@synthesize accessory = _accessory;

- (bool)isHDHICable
{
    NSArray *pHDMIArray = [pHDMIList componentsSeparatedByString:@","];
    if (self.accessory == nil) {
        NSArray *pArray = [[EAAccessoryManager sharedAccessoryManager] connectedAccessories];
        int count = [pArray count];
        if (count == 0) {
            return false;
        }
        for (NSString *hdmiName in pHDMIArray) 
        {   
            for (int i = 0; i < count; i++) {
                EAAccessory *accessory = [[pArray objectAtIndex:i] retain];
                if (accessory != nil) {
                    if ([hdmiName isEqualToString:[accessory modelNumber]]) {
                        return true;
                    }
                }
                [accessory release];
            }
        } 
    }
    else {
        for (NSString *hdmiName in pHDMIArray) 
        {   
            if ([hdmiName isEqualToString:[self.accessory modelNumber]]) {
                return true;
            }
        } 
    }
    return false;
}

- (bool)isHDMIConnected
{
    if ([self isHDHICable] &&  [UIScreen screens].count > 1) {
         return true;       
    }
    return false;
}

#pragma mark Internal

- (void)accessoryDidConnect:(NSNotification *)notification {
    
    self.accessory = nil;
    self.accessory = [[notification userInfo] objectForKey:EAAccessoryKey];
    if (self.accessory == nil) {
        return;
    }
    
    if ([self isHDMIConnected]) {
        [self.delegate onHDMIStateChangeEvent:VO_OSMP_HDMISTATE_CONNECT];
    }
    
}

- (void)accessoryDidDisconnect:(NSNotification *)notification 
{
    self.accessory = nil;
    [self.delegate onHDMIStateChangeEvent:VO_OSMP_HDMISTATE_DISCONNECT];
}


- (void) screenDidConnect:(NSNotification *)aNotification{

    if ([self isHDMIConnected]) {
        [self.delegate onHDMIStateChangeEvent:VO_OSMP_HDMISTATE_CONNECT];
    }
}

- (void) screenDidDisconnect:(NSNotification *)aNotification{
    
    if ([self isHDHICable]) {
        [self.delegate onHDMIStateChangeEvent:VO_OSMP_HDMISTATE_DISCONNECT];
    }
}

#pragma mark loading
- (VO_OSMP_RETURN_CODE)enableHDMIDetection:(bool)value
{
    if (value) {
        NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
        EAAccessoryManager *accessoryMamaner  = [EAAccessoryManager sharedAccessoryManager];
        if (accessoryMamaner == nil) {
            return VO_OSMP_ERR_POINTER;
        }
        [accessoryMamaner registerForLocalNotifications];
        [notificationCenter addObserver: self  selector: @selector(accessoryDidConnect:)   name: EAAccessoryDidConnectNotification object: nil];
        [notificationCenter addObserver: self  selector: @selector(accessoryDidDisconnect:)   name: EAAccessoryDidDisconnectNotification object: nil];
        
        [notificationCenter addObserver:self selector:@selector(screenDidConnect:) name:UIScreenDidConnectNotification object:nil];
        [notificationCenter addObserver:self selector:@selector(screenDidDisconnect:) name:UIScreenDidDisconnectNotification object:nil];
        
        self.enableHDMIDetection = YES;
    }
    else
    {
        if (self.enableHDMIDetection) {
            NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
            [notificationCenter removeObserver:self name:EAAccessoryDidConnectNotification object:nil];
            [notificationCenter removeObserver:self name:EAAccessoryDidDisconnectNotification object:nil];
            [notificationCenter removeObserver:self name:UIScreenDidConnectNotification object:nil];
            [notificationCenter removeObserver:self name:UIScreenDidDisconnectNotification object:nil];
        }
        self.enableHDMIDetection = NO;
    }
    return VO_OSMP_ERR_NONE;
}

- (bool)isHDMIDetectionSupported
{
    return true;
}

- (VO_OSMP_HDMI_CONNECTION_STATUS) getHDMIStatus
{
    if ([self isHDHICable] &&  [UIScreen screens].count > 1) {
        return VO_OSMP_HDMISTATE_CONNECT;
    }
    return VO_OSMP_HDMISTATE_DISCONNECT;
}

- (VO_OSMP_RETURN_CODE)setOnHDMIConnectionChangeDelegate:(id <onHDMIConnectionChangeDelegate>)delegate
{
    if (delegate == nil) {
        return VO_OSMP_ERR_POINTER;
    }
    self.delegate = delegate;
    return VO_OSMP_ERR_NONE;
}

- (id)init
{
    self.accessory = nil;
    self.enableHDMIDetection = NO;
    
    if (nil != [super init]) {
        return self;
    }
    return nil;
}

- (void)uninit
{
    [self enableHDMIDetection:false];
    self.accessory = nil;
}

- (void) dealloc
{
    [self uninit];
	[super dealloc];
}


@end
