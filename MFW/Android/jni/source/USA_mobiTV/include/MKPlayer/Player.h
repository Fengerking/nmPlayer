#ifndef __MKPlayer_Player_h__
#define __MKPlayer_Player_h__

#include <MKCommon/MediaInfo.h>
#include <MKCommon/State.h>
#include <MKCommon/Event.h>
#include <MKCommon/Chain.h>

#include <MKPlayer/Result.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* TEMPORARY                                                                  */
/******************************************************************************/
/*                                                                            */
/* Contains temporary dummy placeholders for planned additions. Will be       */
/* removed once a real implementation is available.                           */
/*                                                                            */
/******************************************************************************/

typedef MK_U32 MK_RTPTransport;

/******************************************************************************/
/* CREATION & DESTRUCTION                                                     */
/******************************************************************************/
/*                                                                            */
/* Defines basic type, creation & destruction APIs for MKPlayer.              */
/*                                                                            */
/******************************************************************************/

/*
 * The MK_Player datatype. The type is incomplete and may only be instansiated
 * through the Create() function.
 */
typedef struct _MK_Player MK_Player;

/******************************************************************************/

/*
 * Allocate and initialize a MK_Player instance. Returns NULL on failure.
 */
MK_Player* MK_Player_Create(void);

/*
 * Destroy the provided MK_Player instance and free all related resources.
 */
void MK_Player_Destroy(MK_Player* aPlay);

/******************************************************************************/
/* LOGGING INTERFACE                                                          */
/******************************************************************************/
/*                                                                            */
/* Defines APIs to influence logging done in the player (e.g. level-filter    */
/* and log message destination).                                              */
/*                                                                            */
/* NOTE: This API is not defined as a MKPlayer interface at this time so the  */
/* recommendation for now is to use the APIs defined in MKBase/Log.h (which   */
/* uses a single global logger for the process) if the logging needs to be    */
/* controlled. Be aware that the MKBase API may not be available in future    */
/* versions of MKPlayer.                                                      */
/*                                                                            */
/******************************************************************************/

/* TODO: Define MKPlayer API for control of logging. */

/******************************************************************************/
/* CONFIGURATION INTERFACE                                                    */
/******************************************************************************/
/*                                                                            */
/* The API below defines a host of functions used to configure the different  */
/* features available in MKPlayer. The configuration functions MUST be called */
/* before MK_Player_Open in order to have any effect.                         */
/******************************************************************************/

/*
 * Set the interface to use when communicating over the network. The default is
 * to use a wildcard interface (which basically leaves the choice to the network
 * stack). This function currently only accepts IP-numbers as interface names.
 *
 * To return to the default behavior of using a wildcard interface just use an
 * empty string (or NULL) as argument.
 * 
 * aMaxBitrate puts a limitation on the bit rate used for the interface. There
 * will be no limitation set if zero is passed as argument.
 *
 * NOTE: This function is used dynamically during a session, i.e. MKPlayer can
 * use this information directly without having to wait for the next MK_Player_Open to 
 * be called. 
 */
MK_Result MK_Player_Net_SetLocalIF(MK_Player* aPlay, const MK_Char* aName, MK_U32 aMaxBitrate);

/*
 * Set the range of UDP port numbers from which ports are selected when creating
 * listening UDP sockets (like in RTP over UDP transport).
 *
 * The default range is the IANA open range: 49152-65535.
 */
MK_Result MK_Player_Net_SetUDPPortRange(MK_Player* aPlay, MK_U16 aFrom, MK_U16 aTo);

/*
 * Set the timeout value for network wait-operations (like connection
 * establishment and waiting for incoming data).
 *
 * Default value is 5 seconds.
 */
MK_Result MK_Player_Net_SetTimeout(MK_Player* aPlay, MK_U32 aMSec);

/******************************************************************************/

/*
 * Set the MobiDRM private key. The key is reset if a NULL key and/or a 0 size
 * is passed.
 *
 * NOTE: Subject to change due to planned DRM updates.
 */
MK_Result MK_Player_MobiDRM_SetKey(MK_Player* aPlay, const void* aKey, MK_U32 aSz);

/*
 * Set the MobiDRM device identification values. The identification is reset if
 * NULL IDs and/or 0 sizes are passed.
 *
 * NOTE: Subject to change due to planned DRM updates.
 */
MK_Result MK_Player_MobiDRM_SetDeviceIDs(MK_Player* aPlay, const void* aDID, MK_U32 aDSz, const void* aXID, MK_U32 aXSz);

/*
 * Set the MobiDRM user identification value. The identification is reset if a
 * NULL ID and/or a 0 size is passed.
 *
 * NOTE: Subject to change due to planned DRM updates.
 */
MK_Result MK_Player_MobiDRM_SetUserID(MK_Player* aPlay, const void* aUID, MK_U32 aSz);

/*
 * Set the active MobiDRM license. To disable license pass a NULL license and/or
 * a 0 size. The time in aNow should be a securely acquired UNIX timestamp
 * representing the current time (for expiration validation).
 *
 * NOTE: Subject to change due to planned DRM updates.
 */
MK_Result MK_Player_MobiDRM_SetLicense(MK_Player* aPlay, const void* aLic, MK_U32 aSz, MK_U32 aNow);

/******************************************************************************/

/*
 * Set HTTP proxy to use by providing a hostname/IP and a port. To disable proxy
 * pass a NULL host and/or a 0 port.
 */
MK_Result MK_Player_HTTP_SetProxy(MK_Player* aPlay, const MK_Char* aHost, MK_U16 aPort);

/*
 * Set some values to modify the User-Agent string provided by the HTTP stack
 * using this format:
 *
 * User-Agent: <aName>/<aVer> (MediaKit/<rev>/<build-id>/<platform>/<arch>; Device/<aDev>; <aExtra>)
 *
 * If NULL is passed in any argument the currently set value will be used.
 */
MK_Result MK_Player_HTTP_SetUserAgentVals(MK_Player* aPlay, const MK_Char* aName, const MK_Char* aVer, const MK_Char* aDev, const MK_Char* aExtra);

/*
 * Set the usage of persistent connection.
 */
MK_Result MK_Player_HTTP_SetUseKeepAlive(MK_Player* aPlay, MK_Bool aUse);

/*
 * Set if bandwidth adaptation is used. (Default: MK_TRUE) 
 */
MK_Result MK_Player_HTTP_SetUseBA(MK_Player* aPlay, MK_Bool aUse);

/*
 * Setting limits used for triggering down- and up-shift of bit rates.
 * 
 * aLower = 110 means that a down shift will be triggered when the download
 * rate is below 110 percent of the current bit rate. 
 *
 * aUpper = 120 means that a up shift will be trigger when the download rate
 * is above 120 percent of the next (higher) bit rate.
 * 
 */
MK_Result MK_Player_HTTP_SetBALimits(MK_Player* aPlay, MK_U32 aLower, MK_U32 aUpper);

/******************************************************************************/

/*
 * Toggle initial hole-punching on or off (default is on) for RTP over UDP.
 */
MK_Result MK_Player_RTP_ToggleInitialHolePunch(MK_Player* aPlay, MK_Bool aOn);

/*
 * Set the initial RTCP RR interval (the default is 5000). The value actually
 * used by the RTP stack may wary based on the number of participants in some
 * cases (as per RFC3550). A value of 0 disables periodic RTCP RR.
 */
MK_Result MK_Player_RTCP_SetInitialRRInterval(MK_Player* aPlay, MK_U32 aMSec);

/*
 * Toggle periodic hole-punching on RTP channel when sending RTCP on or off
 * (default is on).
 */
MK_Result MK_Player_RTCP_ToggleHolePunchWithRR(MK_Player* aPlay, MK_Bool aOn);

/******************************************************************************/

/*
 * Set how often the RTSP stack will ping the server to keep the connection
 * alive (default is 30000ms). A value of 0 disables keep-alive.
 */
MK_Result MK_Player_RTSP_SetPingInterval(MK_Player* aPlay, MK_U32 aMSec);

/*
 * Set RTSP proxy to use by providing a hostname/IP and a port. To disable proxy
 * pass a NULL host and/or a 0 port.
 */
MK_Result MK_Player_RTSP_SetProxy(MK_Player* aPlay, const MK_Char* aHost, MK_U16 aPort);

/*
 * Set the RTP transport prioritization preferences (i.e. provide a list of
 * enabled RTP transports ordered by preference). To revert to the default
 * behavior pass NULL and/or 0 as argument values (default is: UDP, IRTP).
 *
 * NOTE: This function is currently unsupported.
 */
MK_Result MK_Player_RTSP_SetRTPTransports(MK_Player* aPlay, const MK_RTPTransport aTpt[], MK_U32 aN);

/*
 * Set some values to modify the User-Agent string provided by the RTSP stack
 * using this format:
 *
 * User-Agent: <aName>/<aVer> (MediaKit/<rev>/<build-id>/<platform>/<arch>; Device/<aDev>; <aExtra>)
 *
 * If NULL is passed in any argument the currently set value will be used.
 */
MK_Result MK_Player_RTSP_SetUserAgentVals(MK_Player* aPlay, const MK_Char* aName, const MK_Char* aVer, const MK_Char* aDev, const MK_Char* aExtra);

/******************************************************************************/

/*
 * Set the buffer limits for the internal media buffers. The Min value guides
 * how much buffer should be accumulated before starting playback, the Low value
 * guides when a playing buffer should be considered starved and the Max value
 * guides when a buffer should be considered full/overflowing.
 *
 * NOTE: This API is subject to change pending discussions.
 */
MK_Result MK_Player_Media_SetBufferLimits(MK_Player* aPlay, MK_U32 aMinMSec, MK_U32 aLowMSec, MK_U32 aMaxMSec);

/*
* Set the timeout value used for generating a end event (timeout). 
* Event will be sent when the buffers have been empty for aMSec msecs.
* Default value is 20 seconds.
*/
MK_Result MK_Player_Media_SetBufferTimeout(MK_Player* aPlay, MK_U32 aMSec);

/************************************************************************/

/******************************************************************************/
/* CONTROL INTERFACE                                                          */
/******************************************************************************/
/*                                                                            */
/* The API below is what is used to control and monitor the MediaKit player   */
/* instance. It contains functionality to acquire a media resource, control   */
/* the playback and fetch/monitor information about the media resource and    */
/* the playback state.                                                        */
/*                                                                            */
/******************************************************************************/

/*
 * Open the specified URL for playback. After the call MediaInfo and TrackInfo
 * will be available and track selection should be done (by default all tracks
 * are disabled) before issuing Play(). Returns a negative error-code on
 * failure.
 *
 * If the player state is not Idle an implicit Close() (or equivalent) will be
 * executed before the new URL is opened (which allows MediaKit to apply
 * clip-switching optimizations in certain cases).
 *
 * The resulting state on success is Stopped and on failure Idle. State changes
 * will be reported through the StateFunc callback and events will be reported
 * through the EventFunc callback.
 */
MK_Result MK_Player_Open(MK_Player* aPlay, const MK_Char* aURL);

/*
 * Close the current media resource. If aFull is MK_TRUE a more complete close
 * operation is performed and the currently active media manager is released
 * instead of (possibly) being reused.
 *
 * If the player is in Playing state a Stop() (or equivalent) will be executed
 * before the resource is closed.
 *
 * The resulting state is always Idle. State changes will be reported through
 * the StateFunc callback and events will be reported through the EventFunc
 * callback.
 *
 * NOTE: Close() does not reset the configuraion parameters. For a complete
 *       reset the MK_Player instance should be destroyed and re-created.
 */
void MK_Player_Close(MK_Player* aPlay, MK_Bool aFull);

/******************************************************************************/

/*
 * Get the current player state. 
 *
 * See the documentation of the MK_State enum in MKCommon/State.h for additional
 * information.
 */
MK_State MK_Player_GetState(const MK_Player* aPlay);

/*
 * Set the state-change callback function. The aObj pointer is a user provided
 * value which will be available in the callback to aFunc.
 *
 * This callback is preserved for the lifetime of the player instance or until a
 * new state-change callback is set.
 */
MK_Result MK_Player_SetStateFunc(MK_Player* aPlay, MK_StateFunc aFunc, void* aObj);

/*
 * Get the playback start time for the current media resource. After a Seek()
 * completes this will return the actual position seeked to.
 */
MK_Time MK_Player_GetStartTime(const MK_Player* aPlay);

/*
 * Set the event callback function. The aObj pointer is a user provided value
 * which will be available in the callback to aFunc.
 *
 * See the documentation of the MK_Event enum in MKCommon/Event.h for additional
 * information.
 *
 * This callback is preserved for the lifetime of the player instance or until a
 * new state-change callback is set.
 */
MK_Result MK_Player_SetEventFunc(MK_Player* aPlay, MK_EventFunc aFunc, void* aObj);

/*
 * Get an estimate of the current buffer fullness in percent (the value may
 * exceed 100 depending on buffer configuration).
 */
MK_U32 MK_Player_GetBufferFullness(const MK_Player* aPlay);

/*
 * Set the buffer callback function (which reports buffering state and
 * fullness). The aObj pointer is a user provided value which will be available
 * in the callback to aFunc.
 *
 * See the documentation of MK_BufferFunc in MKCommon/Event.h for additional
 * information.
 *
 * This callback is preserved for the lifetime of the player instance or until a
 * new state-change callback is set.
 */
MK_Result MK_Player_SetBufferFunc(MK_Player* aPlay, MK_BufferFunc aFunc, void* aObj);

/******************************************************************************/

/*
 * Get the current media information. This information will be available if a
 * media resource has been successfully opened. If no media information is
 * avaliable NULL will be returned.
 *
 * The pointer returned should not be kept around after the desired information
 * has been read since other calls to MK_Player may invalidate it.
 */
const MK_MediaInfo* MK_Player_GetMediaInfo(const MK_Player* aPlay);

/*
 * Get the number of track in the currently opened media resource. Will return 0
 * if no track information is available.
 */
MK_U32 MK_Player_GetTrackCount(const MK_Player* aPlay);

/*
 * Get the track information for the track indicated by aIdx. If no information
 * is available for the given aIdx NULL will bre returned.
 *
 * The pointer returned should not be kept around after the desired information
 * has been read since other calls to MK_Player may invalidate it.
 */
const MK_TrackInfo* MK_Player_GetTrackInfo(const MK_Player* aPlay, MK_U32 aIdx);

/******************************************************************************/

/*
 * Enable playback of the track indicated by aIdx. Returns a negative error code
 * on failure.
 *
 * This is call is currently only allowed between an Open() and a Play() call
 * (to select which tracks to play).
 */
MK_Result MK_Player_EnableTrack(MK_Player* aPlay, MK_U32 aIdx);

/*
 * Get the next sample for a track. May return NULL if the track is in a
 * non-playing state, is buffer-starved or is ended.
 */
MK_Sample* MK_Player_GetTrackSample(MK_Player* aPlay, MK_U32 aIdx);

/******************************************************************************/

/*
 * Start/resume playback of the enabled tracks for the current media resource. 
 * If the state is already Playing this function does nothing. Returns a
 * negative error-code on failure.
 *
 * This call may succeed directly, returning MK_S_OK, or return MK_S_ASYNC to
 * signal asynchronous completion (e.g. because of buffering needs). In both
 * cases events will be reported through the EventFunc callback, buffering
 * progress through the BufferFunc callback and state changes through the
 * StateFunc callback, but in the asynchronous case the state transition to
 * Playing will not occur until after the function has returned (i.e. the
 * transition is triggered in the event-loop).
 */
MK_Result MK_Player_Play(MK_Player* aPlay);

/*
 * Stop playback of the current media resource. If the state is already Stopped 
 * this function does nothing. Returns a negative error-code on failure.
 *
 * This function may succeed with resumability, returning MK_S_OK, or return
 * MK_S_COND to signal that resuming at the current position is not possible
 * (e.g. for a live resource). In both cases the resulting state is Stopped and
 * state changes and events are reported through the StateFunc and EventFunc
 * callbacks.
 */
MK_Result MK_Player_Stop(MK_Player* aPlay);

/*
 * Seek to a given position in the current media resource. Seeking is currently
 * supported only in Stopped state.
 *
 * This function may succeed directly, returning MK_S_OK, or return MK_S_ASYNC
 * to signal asynchronous completion (e.g. in the event-loop or the next Play()
 * call). The precision of the seek may vary from resource to resource but the
 * actual position seeked to will be reported through the EventFunc callback or
 * may be acquired after the seek has completed with MK_Player_GetStartTime().
 */
MK_Result MK_Player_Seek(MK_Player* aPlay, MK_Time aTime);

/******************************************************************************/
/* DRIVER/EVENT-LOOP INTERFACE                                                */
/******************************************************************************/
/*                                                                            */
/* The API below is what is used to drive and control the event-loop used by  */
/* the MediaKit player. It is designed to allow use in both single-threaded   */
/* and multi-threaded player models. The recommended model is to spin of a    */
/* single thread to loop around Wait() and use one or more separate threads   */
/* to fetch samples from the media tracks (e.g. one thread per active track). */
/*                                                                            */
/******************************************************************************/

/*
 * Callback function for event-loop errors. The provided arguments are:
 *
 *     aObj   - A user defined object (set with the callback)
 *     aErr   - A result code
 */
typedef void (*MK_ErrorFunc)(void* aObj, MK_Result aErr);

/*
 * Set the error callback function for errors which occur in the event-loop. The
 * aObj pointer is a user provided value which will be available in the callback
 * to aFunc.
 *
 * This callback is preserved for the lifetime of the player instance or until a
 * new error callback is set.
 */
MK_Result MK_Player_SetErrorFunc(MK_Player* aPlay, MK_ErrorFunc aFunc, void* aObj);

/*
 * Wait a maximum of aMSec (0 for a poll) milliseconds for one or more internal
 * events to occur (I/O, periodic timers). Returns a value > 0 if events were
 * processed, 0 if the timeout expired and < 0 if an error occured:
 *
 *   MK_E_INTR    - The wait was interrupted
 *   MK_E_FAIL    - Unspecified error (triggers LOGERR)
 *
 * May trigger callbacks to: ErrorFunc, StateFunc, EventFunc, BufferFunc
 */
MK_S32 MK_Player_Wait(MK_Player* aPlay, MK_U32 aMSec);

/*
 * If a Wait() call is in progress, interrupt it and make it return with
 * MK_E_INTR as soon as possible (the same error is also used for system induced
 * interrupts).
 */
void MK_Player_Interrupt(MK_Player* aPlay);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
