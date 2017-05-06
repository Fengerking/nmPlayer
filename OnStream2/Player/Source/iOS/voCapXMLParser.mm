/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003-    			*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		voCapXMLParser.cpp
 
 Contains:	VisualOn cab xml parser mm file
 
 Written by:	Jeff Huang
 
 Change History (most recent first):
 2012-06-09		Jeff			Create file
 *******************************************************************************/

#include "voCapXMLParser.h"

#import "voLog.h"

//#ifdef _VONAMESPACE
//using namespace _VONAMESPACE;
//#endif

@interface voCapXMLParser ()
// Properties that don't need to be seen by the outside world.
@property (nonatomic, retain)   NSMutableArray * m_pArrayElement;
@property (nonatomic, retain)   NSMutableArray * m_pArrayItemList;
@property (nonatomic, retain)   NSMutableDictionary * m_pItemAttribute;
@end

@implementation voCapXMLParser

@synthesize m_pArrayElement   = _pArrayElement;
@synthesize m_pArrayItemList  = _pArrayItemList;
@synthesize m_pItemAttribute  = _pItemAttribute;

- (id) init
{
    memset(&m_cvoPerfData, 0, sizeof(m_cvoPerfData));
    
    if (nil != (self = [super init])) {
        
        self.m_pArrayElement = nil;
        self.m_pArrayItemList = nil;
        self.m_pItemAttribute = nil;
    }
    
    return self;
}

-(void) dealloc
{
    self.m_pArrayElement = nil;
    self.m_pArrayItemList = nil;
    self.m_pItemAttribute = nil;
    
	[super dealloc];
}

-(BOOL) isParseringItem
{
    if (2 <= [self.m_pArrayElement count]
        && [[self.m_pArrayElement objectAtIndex:0] isEqualToString:@"CapData"]
        && [[self.m_pArrayElement objectAtIndex:1] isEqualToString:@"item"]) {
        return YES;
    }
    return NO;
}

-(void) parserItem:(NSString *)string
{
    if (3 != [self.m_pArrayElement count]
        || nil == self.m_pItemAttribute) {
        return;
    }
    
    if ([[self.m_pArrayElement objectAtIndex:2] isEqualToString:@"Core"]
            || [[self.m_pArrayElement objectAtIndex:2] isEqualToString:@"Neon"]
            || [[self.m_pArrayElement objectAtIndex:2] isEqualToString:@"Frequency"]
            || [[self.m_pArrayElement objectAtIndex:2] isEqualToString:@"CodecType"]
            || [[self.m_pArrayElement objectAtIndex:2] isEqualToString:@"BitRate"]
            || [[self.m_pArrayElement objectAtIndex:2] isEqualToString:@"VideoWidth"]
            || [[self.m_pArrayElement objectAtIndex:2] isEqualToString:@"VideoHeight"]
            || [[self.m_pArrayElement objectAtIndex:2] isEqualToString:@"ProfileLevel"]
            || [[self.m_pArrayElement objectAtIndex:2] isEqualToString:@"FPS"]) {
        [self.m_pItemAttribute setObject:string forKey:[self.m_pArrayElement objectAtIndex:2]];
    }
}

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict
{
    [self.m_pArrayElement addObject:elementName];
    
    if ((2 == [self.m_pArrayElement count])
         && YES == [self isParseringItem]) {
        self.m_pItemAttribute = [[NSMutableDictionary alloc] init];
        [self.m_pItemAttribute release];
    }
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
    NSString *strTrimmed =[string stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];

    if (YES == [self isParseringItem]) {
        
        [self parserItem:strTrimmed];
    }
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName
{
    [self.m_pArrayElement removeObject:[self.m_pArrayElement lastObject]];
    
    // end of parser input
    if ((1 == [self.m_pArrayElement count])
        && [[self.m_pArrayElement objectAtIndex:0] isEqualToString:@"CapData"]
        && [elementName isEqualToString:@"item"]) {
        
        [self.m_pArrayItemList addObject:self.m_pItemAttribute];
        self.m_pItemAttribute = nil;
    }
}

-(bool) parserXML:(const char *)pStrIn pInfo:(VOOSMP_CPU_INFO *)pInfo
{
    if (NULL == pStrIn || NULL == pInfo || 0 == pInfo->nCoreCount || 0 == pInfo->nFrequency) {
        return false;
    }
    
    NSString *pStrFile = [NSString stringWithUTF8String:pStrIn];
    
    if(![[NSFileManager defaultManager] fileExistsAtPath:pStrFile]) {
        return false;
    }
    
    self.m_pArrayElement = [[NSMutableArray alloc] init];
    [self.m_pArrayElement release];
    self.m_pArrayItemList = [[NSMutableArray alloc] init];
    [self.m_pArrayItemList release];
    
    bool bRet = true;
    
//    NSXMLParser* parser = [[NSXMLParser alloc] initWithData:[pStrIn dataUsingEncoding:NSUTF8StringEncoding]];
    
    NSData * dataXml = [[NSData alloc] initWithContentsOfURL:[NSURL fileURLWithPath:pStrFile]];
    NSXMLParser *parser = [[NSXMLParser alloc] initWithData:dataXml];
    [dataXml release];
    
    //NSXMLParser* parser = [[NSXMLParser alloc] initWithContentsOfURL:[NSURL fileURLWithPath:pStrFile]];
    
    [parser setDelegate:self];
    if (![parser parse]) {
        bRet = false;
        VOLOGE("parse error = %@", [parser parserError]);
    }
    
    [parser release];
    
    NSComparator cmptr = ^NSComparisonResult(id obj1, id obj2) {
        int nFre1 = [[obj1 objectForKey:@"Core"] intValue] * [[obj1 objectForKey:@"Frequency"] intValue];
        if ([[obj1 objectForKey:@"Neon"] intValue] == 0) {
            nFre1 = nFre1 * 0.7;
        }
        
        int nFre2 = [[obj2 objectForKey:@"Core"] intValue] * [[obj2 objectForKey:@"Frequency"] intValue];
        if ([[obj2 objectForKey:@"Neon"] intValue] == 0) {
            nFre2 = nFre2 * 0.7;
        }
        
        if (nFre1 > nFre2) {
            return NSOrderedDescending;
        }
        else if (nFre1 < nFre2) {
            return NSOrderedAscending;
        }
        else {
            return NSOrderedSame;
        }
    };
    
    NSArray *pSortItemList = [self.m_pArrayItemList sortedArrayUsingComparator:cmptr];
    
    NSMutableDictionary* pDicTmp = [[NSMutableDictionary alloc] init];
    [pDicTmp setObject:[NSString stringWithFormat:@"%d", pInfo->nCoreCount] forKey:@"Core"];
    [pDicTmp setObject:[NSString stringWithFormat:@"%d", pInfo->nCPUType] forKey:@"Neon"];
    [pDicTmp setObject:[NSString stringWithFormat:@"%d", pInfo->nFrequency] forKey:@"Frequency"];
    
    int iOk = 0;
    for (int nIndex = 1; nIndex < [pSortItemList count]; ++nIndex) {
        NSMutableDictionary* pDicIndex = [pSortItemList objectAtIndex:nIndex];
        if (nil == pDicIndex) {
            continue;
        }
        
        int cResult = cmptr(pDicTmp, pDicIndex);
        if (NSOrderedAscending == cResult) {
            break;
        }
        else {
            iOk = nIndex;
        }
    }
    
    [pDicTmp release];
    
    if (0 < [pSortItemList count]) {
        NSMutableDictionary* pDicOk = [pSortItemList objectAtIndex:iOk];
        
        m_cvoPerfData.nCodecType = [[pDicOk objectForKey:@"CodecType"] intValue];
        m_cvoPerfData.nBitRate = [[pDicOk objectForKey:@"BitRate"] intValue];
        m_cvoPerfData.nVideoWidth = [[pDicOk objectForKey:@"VideoWidth"] intValue];
        m_cvoPerfData.nVideoHeight = [[pDicOk objectForKey:@"VideoHeight"] intValue];
        m_cvoPerfData.nProfileLevel = [[pDicOk objectForKey:@"ProfileLevel"] intValue];
        m_cvoPerfData.nFPS = [[pDicOk objectForKey:@"FPS"] intValue];
        
        VOLOGI("Core is %d, Neon is %d, Frequency is %d, CodecType is %d, BitRate is %d, VideoWidth is %d, VideoHeight is %d ProfileLevel is %d, FPS is %d", [[pDicOk objectForKey:@"Core"] intValue], [[pDicOk objectForKey:@"Neon"] intValue], [[pDicOk objectForKey:@"Frequency"] intValue], m_cvoPerfData.nCodecType, m_cvoPerfData.nBitRate, m_cvoPerfData.nVideoWidth, m_cvoPerfData.nVideoHeight, m_cvoPerfData.nProfileLevel, m_cvoPerfData.nFPS);
    }
    else {
        bRet = false;
    }
    
    self.m_pArrayElement = nil;
    self.m_pArrayItemList = nil;
    self.m_pItemAttribute = nil;
    
    return bRet;
}

-(VOOSMP_PERF_DATA *) getPerfData
{
    return &m_cvoPerfData;
}

@end