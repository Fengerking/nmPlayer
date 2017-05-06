/*
 *  CEventDelegate.h
 *  Player event delegate for main thread
 *
 *  Created by Jeff Huang on 09/13/2011.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

@protocol CEventDelegate<NSObject>

-(int)handleEvent:(int)nID withParam1:(void *)pParam1 withParam2:(void * )pParam2;

@end

struct EventInfo
{
	int		nEventID;
	void*	pParam1;
	void*	pParam2;
};

@interface CEventSender : NSObject 
{
	id <CEventDelegate>		delegate;
	EventInfo				m_cEventInfo;
	int						m_iRet;
}

@property(nonatomic,assign)   id <CEventDelegate>   delegate;

- (int)sendEvent:(int)nID withParam1:(void *)pParam1 withParam2:(void * )pParam2;

@end