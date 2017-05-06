/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __voMIDI_H__
#define __voMIDI_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//#include <voAudio.h>
#include "voSource.h"
#include "cmnFile.h"
#include "cmnMemory.h"

/* MIDI Param ID */
#define VO_PID_MIDI_Mdoule				0x422B1000 
#define VO_PID_MIDI_PATCHDIR			VO_PID_MIDI_Mdoule | 0x0001 /*!< MIDI Parameter ID for setting the midi config file direction and name*/

/**
 * Get MIDI API interface
 * \param pDecHandle [out] Return the MIDI handle.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voGetMidiReadAPI(VO_SOURCE_READAPI * pDecHandle);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voMIDI_H__

