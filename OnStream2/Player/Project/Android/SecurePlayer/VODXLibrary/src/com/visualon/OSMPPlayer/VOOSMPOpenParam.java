/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

package com.visualon.OSMPPlayer;

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_DECODER_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

public class VOOSMPOpenParam {

    private long mFileSize = 0;
    private int  mDecoderType = VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_SW.getValue() | VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_SW.getValue();

    public VOOSMPOpenParam() {
        super();
    }

    
    /**
     * Get audio and video decoder types 
     *
     * @return  audio and video decoder types (bitwise-OR of values of {@link VOOSMPType.VO_OSMP_DECODER_TYPE})
     */
    public int getDecoderType() {
        return mDecoderType;
    }
    
    
    /**
     * Set audio and video decoder types
     *
     * @param  decoderType audio and video decoder types (bitwise-OR of values of {@link VOOSMPType.VO_OSMP_DECODER_TYPE}). It must have one audio and one video decoder types. Default is VO_OSMP_DEC_VIDEO_SW.getValue() | VO_OSMP_DEC_AUDIO_SW.getValue()
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setDecoderType(int decoderType) {
        
        int videoMask = VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_SW.getValue() |
                        VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_IOMX.getValue() |
                        VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_MEDIACODEC.getValue();

        int audioMask = VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_SW.getValue() |
                        VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_MEDIACODEC.getValue();

        int videoDec = decoderType & videoMask;
        int audioDec = decoderType & audioMask;

        if (videoDec != VO_OSMP_DECODER_TYPE .VO_OSMP_DEC_VIDEO_SW.getValue() &&
            videoDec != VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_IOMX.getValue() &&
            videoDec != VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_MEDIACODEC.getValue())
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;

    if (audioDec != VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_SW.getValue() &&
        audioDec != VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_MEDIACODEC.getValue())
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;

            mDecoderType = decoderType;
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
    
    /**
    * Get actual file size
    *
    * @return  file size
    */
    public long getFileSize() {
        // TODO Auto-generated method stub
        return mFileSize;
    }
    
    /**
     * Set actual file size 
     *
     * @param fileSize File size
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setFileSize(long fileSize) {
        // TODO Auto-generated method stub
        mFileSize = fileSize;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }



}
