#ifndef __VOFILEREADER_H__ 

#define __VOFILEREADER_H__

#include "voYYDef_filcmn.h"

#ifdef _AVI_READER
#ifdef LIB
#define MODULENAME AVI
#else
#define MODULENAME 
#endif
#endif	//_AVI_READER

#ifdef _ASF_READER
#ifdef LIB
#define MODULENAME ASF
#else
#define MODULENAME 
#endif
#endif	//_ASF_READER

#ifdef _MP4_READER
#ifdef LIB
#define MODULENAME MP4
#else
#define MODULENAME 
#endif
#endif	//_MP4_READER

#ifdef _AUDIO_READER
#ifdef LIB
#define MODULENAME AUDIO
#else
#define MODULENAME 
#endif
#endif	//_AUDIO_READER

#ifdef _RM_READER
#ifdef LIB
#define MODULENAME RM
#else
#define MODULENAME 
#endif
#endif	//_RM_READER

#ifdef _FLV_READER
#ifdef LIB
#define MODULENAME FLV
#else
#define MODULENAME 
#endif
#endif	//_FLV_READER

#ifdef _OGG_READER
#ifdef LIB
#define MODULENAME OGG
#else
#define MODULENAME 
#endif
#endif  //_OGG_READER

#ifdef _MKV_READER
#ifdef LIB
#define MODULENAME MKV
#else
#define MODULENAME 
#endif
#endif  //_MKV_READER

#ifdef _MPEG_READER
#ifdef LIB
#define MODULENAME MPEG
#else
#define MODULENAME 
#endif
#endif	//_MPEG_READER


#define MODULEID( name ) g_dwFR##name##ModuleID
#define MODULEINSTANCE_ID( name ) g_h##name##Inst
#define FILEREADER_NAMESPACE( name ) namespace_##name

#define g_dwFRModuleID MODULEID(MODULENAME)
#define g_hInst MODULEINSTANCE_ID(MODULENAME)

#define MODULEOpen( name ) voFR##name##Open
#define MODULEClose( name ) voFR##name##Close
#define MODULEGetFileInfo( name ) voFR##name##GetFileInfo
#define MODULEGetTrackInfo( name ) voFR##name##GetTrackInfo
#define MODULEGetSample( name ) voFR##name##GetSample
#define MODULESetPos( name ) voFR##name##SetPos
#define MODULESetFileParam( name ) voFR##name##SetFileParam
#define MODULEGetFileParam( name ) voFR##name##GetFileParam
#define MODULESetTrackParam( name ) voFR##name##SetTrackParam
#define MODULEGetTrackParam( name ) voFR##name##GetTrackParam

#define voFROpen MODULEOpen(MODULENAME)
#define voFRClose MODULEClose(MODULENAME)
#define voFRGetFileInfo MODULEGetFileInfo(MODULENAME)
#define voFRGetTrackInfo MODULEGetTrackInfo(MODULENAME)
#define voFRGetSample MODULEGetSample(MODULENAME)
#define voFRSetPos MODULESetPos(MODULENAME)
#define voFRSetFileParam MODULESetFileParam(MODULENAME)
#define voFRGetFileParam MODULEGetFileParam(MODULENAME)
#define voFRSetTrackParam MODULESetTrackParam(MODULENAME)
#define voFRGetTrackParam MODULEGetTrackParam(MODULENAME)

#define MODULECBASEREADER(name) CBase##name##Reader
#define CBaseReader MODULECBASEREADER(MODULENAME)

#define MODULECBASETRACK(name) CBase##name##Track
#define CBaseTrack MODULECBASETRACK(MODULENAME)

#define MODULECBASEDRMCALLBACK(name) CvoBase##name##DrmCallback
#define CvoBaseDrmCallback MODULECBASEDRMCALLBACK(MODULENAME)

#define MODULECGFILECHUNK(name) CGFileChunk_##name##_
#define CGFileChunk MODULECGFILECHUNK(MODULENAME)


#define MODULECVOBASEFILEOPR(name) CvoBaseFileOpr_##name##_
#define CvoBaseFileOpr MODULECVOBASEFILEOPR(MODULENAME)


#define MODULECVOBASEMEMOPR(name) CvoBaseMemOpr_##name##_
#define CvoBaseMemOpr MODULECVOBASEMEMOPR(MODULENAME)


#define MODULECVOBASEOBJECT(name) CvoBaseObject_##name##_
#define CvoBaseObject MODULECVOBASEOBJECT(MODULENAME)


#define MODULEVO_THREAD(name) vo_thread_##name##_
#define vo_thread MODULEVO_THREAD(MODULENAME)


#define MODULEVOCMUTEX(name) voCMutex_##name##_
#define voCMutex MODULEVOCMUTEX(MODULENAME)


#define MODULEVOCAUTOLOCK(name) voCAutoLock_##name##_
#define voCAutoLock MODULEVOCAUTOLOCK(MODULENAME)


#define MODULEVOCSEMAPHORE(name) voCSemaphore_##name##_
#define voCSemaphore MODULEVOCSEMAPHORE(MODULENAME)


#define MODULECMNFILEOPEN(name) cmnFileOpen_##name##_
#define cmnFileOpen MODULECMNFILEOPEN(MODULENAME)

#define MODULECMNFILEREAD(name) cmnFileRead_##name##_
#define cmnFileRead MODULECMNFILEREAD(MODULENAME)

#define MODULECMNFILEWRITE(name) cmnFileWrite_##name##_
#define cmnFileWrite MODULECMNFILEWRITE(MODULENAME)

#define MODULECMNFILEFLUSH(name) cmnFileFlush_##name##_
#define cmnFileFlush MODULECMNFILEFLUSH(MODULENAME)

#define MODULECMNFILESEEK(name) cmnFileSeek_##name##_
#define cmnFileSeek MODULECMNFILESEEK(MODULENAME)

#define MODULECMNFILESIZE(name) cmnFileSize_##name##_
#define cmnFileSize MODULECMNFILESIZE(MODULENAME)

#define MODULECMNFILESAVE(name) cmnFileSave_##name##_
#define cmnFileSave MODULECMNFILESAVE(MODULENAME)

#define MODULECMNFILECLOSE(name) cmnFileClose_##name##_
#define cmnFileClose MODULECMNFILECLOSE(MODULENAME)

#define MODULECMNFILEFILLPOINTER(name) cmnFileFillPointer_##name##_
#define cmnFileFillPointer MODULECMNFILEFILLPOINTER(MODULENAME)

#define MODULECMNFILEDELETE(name) cmnFileDelete_##name##_
#define cmnFileDelete MODULECMNFILEDELETE(MODULENAME)

#define MODULECMNFOLDERCREATE(name) cmnFolderCreate_##name##_
#define cmnFolderCreate MODULECMNFOLDERCREATE(MODULENAME)

#define MODULECMNFOLDERDELETE(name) cmnFolderDelete_##name##_
#define cmnFolderDelete MODULECMNFOLDERDELETE(MODULENAME)


#define MODULECMNMEMALLOC(name) cmnMemAlloc_##name##_
#define cmnMemAlloc MODULECMNMEMALLOC(MODULENAME)

#define MODULECMNMEMFREE(name) cmnMemFree_##name##_
#define cmnMemFree MODULECMNMEMFREE(MODULENAME)

#define MODULECMNMEMSET(name) cmnMemSet_##name##_
#define cmnMemSet MODULECMNMEMSET(MODULENAME)

#define MODULECMNMEMCOPY(name) cmnMemCopy_##name##_
#define cmnMemCopy MODULECMNMEMCOPY(MODULENAME)

#define MODULECMNMEMCHECK(name) cmnMemCheck_##name##_
#define cmnMemCheck MODULECMNMEMCHECK(MODULENAME)

#define MODULECMNMEMCOMPARE(name) cmnMemCompare_##name##_
#define cmnMemCompare MODULECMNMEMCOMPARE(MODULENAME)

#define MODULECMNMEMMOVE(name) cmnMemMove_##name##_
#define cmnMemMove MODULECMNMEMMOVE(MODULENAME)

#define MODULECMNMEMFILLPOINTER(name) cmnMemFillPointer_##name##_
#define cmnMemFillPointer MODULECMNMEMFILLPOINTER(MODULENAME)

#define MODULECMNMEMSHOWSTATUS(name) cmnMemShowStatus_##name##_
#define cmnMemShowStatus MODULECMNMEMSHOWSTATUS(MODULENAME)


#define MODULEFCODECGETVIDEOCODEC(name) fCodecGetVideoCodec_##name##_
#define fCodecGetVideoCodec MODULEFCODECGETVIDEOCODEC(MODULENAME)

#define MODULEFCODECGETAUDIOCODEC(name) fCodecGetAudioCodec_##name##_
#define fCodecGetAudioCodec MODULEFCODECGETAUDIOCODEC(MODULENAME)

#define MODULEFCODECISDIVX(name) fCodecIsDivX_##name##_
#define fCodecIsDivX MODULEFCODECISDIVX(MODULENAME)

#define MODULEFCODECISXVID(name) fCodecIsXviD_##name##_
#define fCodecIsXviD MODULEFCODECISXVID(MODULENAME)



#define MODULEFILE_OPEN(name) file_open_##name##_
#define file_open MODULEFILE_OPEN(MODULENAME)

#define MODULEFILE_READ(name) file_read_##name##_
#define file_read MODULEFILE_READ(MODULENAME)

#define MODULEFILE_WRITE(name) file_write_##name##_
#define file_write MODULEFILE_WRITE(MODULENAME)

#define MODULEFILE_FLUSH(name) file_flush_##name##_
#define file_flush MODULEFILE_FLUSH(MODULENAME)

#define MODULEFILE_SEEK(name) file_seek_##name##_
#define file_seek MODULEFILE_SEEK(MODULENAME)

#define MODULEFILE_SIZE(name) file_size_##name##_
#define file_size MODULEFILE_SIZE(MODULENAME)

#define MODULEFILE_SAVE(name) file_save_##name##_
#define file_save MODULEFILE_SAVE(MODULENAME)

#define MODULEFILE_CLOSE(name) file_close_##name##_
#define file_close MODULEFILE_CLOSE(MODULENAME)

#define MODULEVO_FILE_IO(name) vo_file_io_##name##_
#define vo_file_io MODULEVO_FILE_IO(MODULENAME)


#define MODULEVO_FILE_PARSER(name) vo_file_parser_##name##_
#define vo_file_parser MODULEVO_FILE_PARSER(MODULENAME)



#define MODULEGETRESOLUTION_H264(name) getResolution_H264_##name##_
#define getResolution_H264 MODULEGETRESOLUTION_H264(MODULENAME)

#define MODULEGETPROFILELEVEL_H264(name) getProfileLevel_H264_##name##_
#define getProfileLevel_H264 MODULEGETPROFILELEVEL_H264(MODULENAME)

#define MODULEGETOMX_VIDEO_PARAM_AVCTYPE_H264(name) getOMX_VIDEO_PARAM_AVCTYPE_H264_##name##_
#define getOMX_VIDEO_PARAM_AVCTYPE_H264 MODULEGETOMX_VIDEO_PARAM_AVCTYPE_H264(MODULENAME)

#define MODULEISINTERLACE_H264(name) IsInterlace_H264_##name##_
#define IsInterlace_H264 MODULEISINTERLACE_H264(MODULENAME)

#define MODULEPROCESSSPS_AVC(name) ProcessSPS_AVC_##name##_
#define ProcessSPS_AVC MODULEPROCESSSPS_AVC(MODULENAME)

#define MODULEGETRESOLUTION_H263(name) getResolution_H263_##name##_
#define getResolution_H263 MODULEGETRESOLUTION_H263(MODULENAME)

#define MODULEGETRESOLUTION_MPEG4(name) getResolution_Mpeg4_##name##_
#define getResolution_Mpeg4 MODULEGETRESOLUTION_MPEG4(MODULENAME)

#define MODULEGETPROFILELEVEL_MPEG4(name) getProfileLevel_Mpeg4_##name##_
#define getProfileLevel_Mpeg4 MODULEGETPROFILELEVEL_MPEG4(MODULENAME)

#define MODULEISINTERLACE_MPEG4(name) IsInterlace_Mpeg4_##name##_
#define IsInterlace_Mpeg4 MODULEISINTERLACE_MPEG4(MODULENAME)

#define MODULEMPEG4VIDEOINFO(name) Mpeg4VideoInfo_##name##_
#define Mpeg4VideoInfo MODULEMPEG4VIDEOINFO(MODULENAME)


#define MODULEINITBITS(name) InitBits_##name##_
#define InitBits MODULEINITBITS(MODULENAME)

#define MODULEFLUSHBITS(name) FlushBits_##name##_
#define FlushBits MODULEFLUSHBITS(MODULENAME)

#define MODULEREADBITS(name) ReadBits_##name##_
#define ReadBits MODULEREADBITS(MODULENAME)

#define MODULEENDOFBITS(name) EndOfBits_##name##_
#define EndOfBits MODULEENDOFBITS(MODULENAME)

#define MODULEPARSERVOL(name) ParserVOL_##name##_
#define ParserVOL MODULEPARSERVOL(MODULENAME)

#define MODULEPARSERH263HEADER(name) ParserH263Header_##name##_
#define ParserH263Header MODULEPARSERH263HEADER(MODULENAME)

#define MODULEMPEG4PROFILELEVEL(name) Mpeg4ProfileLevel_##name##_
#define Mpeg4ProfileLevel MODULEMPEG4PROFILELEVEL(MODULENAME)

#define MODULEVOSEARCHMPEG4VOLSC(name) voSearchMpeg4VOLSC_##name##_
#define voSearchMpeg4VOLSC MODULEVOSEARCHMPEG4VOLSC(MODULENAME)

#define MODULEVOSEARCHMPEG4PROFILE(name) voSearchMpeg4ProFile_##name##_
#define voSearchMpeg4ProFile MODULEVOSEARCHMPEG4PROFILE(MODULENAME)

#define MODULEVOSEARCHH263PSC(name) voSearchH263PSC_##name##_
#define voSearchH263PSC MODULEVOSEARCHH263PSC(MODULENAME)

#define MODULEADJUSTBITSTREAM(name) AdjustBitstream_##name##_
#define AdjustBitstream MODULEADJUSTBITSTREAM(MODULENAME)

#define MODULEGETVLCSYMBOL(name) GetVLCSymbol_##name##_
#define GetVLCSymbol MODULEGETVLCSYMBOL(MODULENAME)

#define MODULEGETBITS(name) GetBits_##name##_
#define GetBits MODULEGETBITS(MODULENAME)

#define MODULEU(name) u_##name##_
#define u MODULEU(MODULENAME)

#define MODULEUE(name) ue_##name##_
#define ue MODULEUE(MODULENAME)

#define MODULESE(name) se_##name##_
#define se MODULESE(MODULENAME)

#define MODULECLEANTHEBITSTREAM(name) CleanTheBitStream_##name##_
#define CleanTheBitStream MODULECLEANTHEBITSTREAM(MODULENAME)

#define MODULEPROCESSSPS(name) ProcessSPS_##name##_
#define ProcessSPS MODULEPROCESSSPS(MODULENAME)

#define MODULEPROCESSSPS_AVC(name) ProcessSPS_AVC_##name##_
#define ProcessSPS_AVC MODULEPROCESSSPS_AVC(MODULENAME)

#define MODULEPROCESSSPS_H264(name) ProcessSPS_H264_##name##_
#define ProcessSPS_H264 MODULEPROCESSSPS_H264(MODULENAME)

#define MODULEISINTERLACE_MPEG4(name) IsInterlace_Mpeg4_##name##_
#define IsInterlace_Mpeg4 MODULEISINTERLACE_MPEG4(MODULENAME)

#define MODULEGETRESOLUTION_H264(name) getResolution_H264_##name##_
#define getResolution_H264 MODULEGETRESOLUTION_H264(MODULENAME)

#define MODULEGETPROFILELEVEL_H264(name) getProfileLevel_H264_##name##_
#define getProfileLevel_H264 MODULEGETPROFILELEVEL_H264(MODULENAME)

#define MODULEGETRESOLUTION_H263(name) getResolution_H263_##name##_
#define getResolution_H263 MODULEGETRESOLUTION_H263(MODULENAME)

#define MODULEGETRESOLUTION_MPEG4(name) getResolution_Mpeg4_##name##_
#define getResolution_Mpeg4 MODULEGETRESOLUTION_MPEG4(MODULENAME)

#define MODULEMPEG4VIDEOINFO(name) Mpeg4VideoInfo_##name##_
#define Mpeg4VideoInfo MODULEMPEG4VIDEOINFO(MODULENAME)

#define MODULEGETPROFILELEVEL_MPEG4(name) getProfileLevel_Mpeg4_##name##_
#define getProfileLevel_Mpeg4 MODULEGETPROFILELEVEL_MPEG4(MODULENAME)

#define MODULEGETOMX_VIDEO_PARAM_AVCTYPE_H264(name) getOMX_VIDEO_PARAM_AVCTYPE_H264_##name##_
#define getOMX_VIDEO_PARAM_AVCTYPE_H264 MODULEGETOMX_VIDEO_PARAM_AVCTYPE_H264(MODULENAME)

#define MODULEISINTERLACE_H264(name) IsInterlace_H264_##name##_
#define IsInterlace_H264 MODULEISINTERLACE_H264(MODULENAME)


#endif