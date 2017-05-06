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

#import <Foundation/Foundation.h>

@protocol VOCommonPlayerSubtitle <NSObject>


/**
 * Set file/URL path for external subtitles (e.g. smi, srt files, etc.).
 *
 * @param   filePath [in] Full path/URL (including “http://”) to the subtitle file.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitlePath:(NSString *)filePath;


/**
 * Enable/disable subtitles display; default is disabled.
 *
 * @param   value [in] Enable/disable subtiltles; true to enable (show) subtitles, false to disable (hide) subtitles.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) enableSubtitle:(bool)value;


/**
 * Set subtitle font color.
 *
 * @param   color [in] Font color (0x00RRGGBB) of subtitle text.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontColor:(int)color;


/**
 * Set subtitle font color opacity rate.
 *
 * @param   alpha [in] Font color opacity rate. The valid range is 0 to 100, from transparent to opaque.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontOpacity:(int)alpha;


/**
 * Set subtitle font size scale.
 *
 * @param   scale [in] Font size scale for subtitle text (percent). The valid range is 50 to 200,
 * where 50 is the smallest and is half of the default size, and 200 is the largest and is twice the default size.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontSizeScale:(int)scale;


/**
 * Set subtitle font background color.
 *
 * @param   color [in] Subtitle font background color (0x00RRGGBB).
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundColor:(int)color;


/**
 * Set subtitle font background color opacity rate.
 *
 * @param   alpha [in] Subtitle font background color opacity rate. The valid range is 0 to 100, from transparent to opaque.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundOpacity:(int)alpha;


/**
 * Set window background color.
 *
 * @param   color [in] Subtitle window background color (0x00RRGGBB).
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundColor:(int)color;


/**
 * Set window background color opacity rate.
 *
 * @param   alpha [in] Subtitle window background color opacity rate. The valid range is 0 to 100, from transparent to opaque.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundOpacity:(int)alpha;


/**
 * Set enable/disable subtitle font italics.
 *
 * @param   enable [in] Enable/Disable; true to enable subtitle font italics.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontItalic:(bool)enable;


/**
 * Set enable/disable subtitle font bold.
 *
 * @param   enable [in] Enable/Disable; true to enable subtitle font bold.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontBold:(bool)enable;


/**
 * Set enable/disable subtitle font underlined.
 *
 * @param   enable [in] Enable/Disable; true to enable subtitle font underline.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontUnderline:(bool)enable;


/**
 * Set subtitle font name.
 *
 * @param   name [in] Font name for subtitle text.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontName:(NSString *)name;


/**
 * Set subtitle font edge type.
 *
 * @param   type [in] Edge type of subtitle font. Valid values are 0:NONE, 1:RAISED, 2:DEPRESSED, 3:UNIFORM, 4:LEFT_DROP_SHADOW, 5:RIGHT_DROP_SHADOW.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeType:(int)type;


/**
 * Set subtitle font edge color.
 *
 * @param   color [in] Font edge color (0x00RRGGBB) of subtitle text.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeColor:(int)color;


/**
 * Set subtitle font edge color opacity rate.
 *
 * @param   alpha [in] Edge color opacity rate of subtitle font. The valid range is 0 to 100, from transparent to opaque.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeOpacity:(int)alpha;


/**
 * Reset all parameters to their default values. Subtitles will be presented as specified in the subtitle stream.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) resetSubtitleParameter;


/**
 * Preview subtitle by sending some sample text to be rendered.
 *
 * @param sampleText [in] text for previewing current subtitle settings
 * @param view [in] view to which the sample text is being rendered
 *
 * This API must be called in main thread.
 *
 * @return {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) previewSubtitle:(NSString *)sampleText view:(id)view;

@end
