#ifndef _CVMAPTag_H_
#define _CVMAPTag_H_

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


///<XML Tag name

#define VMAPTagExtensions  "vmap:Extensions"
#define VMAPTagUnicornOnce   "uo:unicornOnce" 
#define VMAPTagAdBreak   "vmap:AdBreak" 
#define VMAPTagAdSource   "vmap:AdSource" 
#define VMAPTagTrackingEvents   "vmap:TrackingEvents" 
#define VMAPTagAdData   "vmap:VASTData" 
#define VMAPTagAdTagURI   "vmap:AdTagURI" 
#define VMAPTagCustomAdData   "vmap:CustomAdData" 
///<XML Attribute name
#define VMAPAttriContenturi   "contenturi" 
///<Ad Break attribute
#define VMAPAttriBreakType   "breakType" 
#define VMAPAttriBreakTypeLin   "linear" 
#define VMAPAttriBreakTypeNonLin   "nonlinear" 
#define VMAPAttriBreakTypeComp   "Companion" ///<there is no defined value in spec,so we need modify later
#define VMAPAttriBreakTypeSkippable   "Skippable" ///<there is no defined value in spec,so we need modify later
#define VMAPAttriBreakTypeAdPods   "AdPods" ///<there is no defined value in spec,so we need modify later

#define VMAPAttriBreakId   "breakId" 
#define VMAPAttriTimeOffset   "timeOffset" 
#define VMAPAttriRepeatAfter   "repeatAfter" 

///<Ad Source attribute
#define VMAPAttriSourceID   "id" 
#define VMAPAttriSourceAllowMultipleAds   "allowMultipleAds" 
#define VMAPAttriSourceFollowRedirects   "followRedirects" 
	

#ifdef _VONAMESPACE
}
#endif

#endif ///<_CVMAPTag_H_