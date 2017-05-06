#include "voSubTitleFormatSettingImpl.h"
voSubTitleFormatSettingImpl::voSubTitleFormatSettingImpl(void)
{
	edgeType = 0;

	edgeColor = 0;
	fontColor = 0;
	windowBackgroundColor = 0;
	backgroundColor = 0;
	edgeColorOpacityRate = 0;
	fontColorOpacityRate = 0;
	windowBackgroundColorOpacityRate = 0;
	backgroundColorOpacityRate = 0;

	fontSizeMedium = 0.75f;
	fontItalic=0;
	fontUnderline=0;
	fontBold=1;
	reset();
}

voSubTitleFormatSettingImpl::~voSubTitleFormatSettingImpl(void)
{
}

/**
 * set font color.
 *
 * @param nFontColor the font color for text
 */
void voSubTitleFormatSettingImpl::setFontColor(COLORREF nFontColor) {
	fontColor = nFontColor;
	fontColorEnable = true;
}
/**
 * set font color opacity rate.
 *
 * @param nAlpha the font color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
 */
void voSubTitleFormatSettingImpl::setFontOpacity(int nAlpha)
{
	fontColorOpacityRateEnable = true;
	nAlpha = checkAlpha( nAlpha);
	fontColorOpacityRate = nAlpha;
	
}
/**
 * set font size, it will be delete, and it will be replaced with setFontSize(float nFontSize).
 *
 * @param nSmallFontSize the small font size for text, the parameter value is between 0.5 and 2.0, 0.5 is the smallest, 2.0 is the biggest font.
 * @param nMediumFontSize the medium font size for text, the parameter value is between 0.5 and 2.0, 0.5 is the smallest, 2.0 is the biggest font.
 * @param nLargeFontSzie the large font size for text, the parameter value is between 0.5 and 2.0, 0.5 is the smallest, 2.0 is the biggest font.
 */
void voSubTitleFormatSettingImpl::setFontSize(float nSmallFontSize, float nMediumFontSize, float nLargeFontSzie) {
	fontSizeMedium = nMediumFontSize;
	fontSizeEnable = true;
}
/**
 * set font size scale.
 *
 * @param nFontSize the font size scale for text, the parameter value is between 50 and 200, 50 is the smallest and is the 0.5 times of default font size, 200 is the biggest font and is the 2.0 times of default font size.
 */
void voSubTitleFormatSettingImpl::setFontSizeScale(int nFontSize){
	if(nFontSize>200)
		nFontSize = 200;
	if(nFontSize<50)
		nFontSize = 50;
	setFontSize(nFontSize/100.0f);
}
/**
 * set font size scale, it will replaced with setFontSizeScale.
 *
 * @param nFontSize the font size for text, the parameter value is between 0.5 and 2.0, 0.5 is the smallest, 2.0 is the biggest font.
 */
void voSubTitleFormatSettingImpl::setFontSize(float nFontSize) {
	setFontSize(nFontSize, nFontSize, nFontSize);
}
/**
 * set background color.
 *
 * @param nBackgroundColor the background color for text
 */
void voSubTitleFormatSettingImpl::setBackgroundColor(COLORREF nBackgroundColor) {
	backgroundColor = nBackgroundColor;
	backgroundColorEnable = true;
}
/**
 * set background color opacity rate.
 *
 * @param nAlpha the background color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
 */
void voSubTitleFormatSettingImpl::setBackgroundOpacity(int nAlpha)
{
	backgroundColorOpacityRateEnable = true;
	nAlpha = checkAlpha( nAlpha);
	backgroundColorOpacityRate = nAlpha;
	
}
/**
 * set window background color.
 *
 * @param nBackgroundColor the window background color for text
 */
void voSubTitleFormatSettingImpl::setWindowColor(COLORREF nBackgroundColor) {
	windowBackgroundColor = nBackgroundColor;
	windowBackgroundColorEnable = true;
}
/**
 * set window background color opacity rate.
 *
 * @param nAlpha the window background color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
 */
void voSubTitleFormatSettingImpl::setWindowOpacity(int nAlpha){
	windowBackgroundColorOpacityRateEnable = true;
	nAlpha = checkAlpha( nAlpha);
	windowBackgroundColorOpacityRate = nAlpha;
	
}
/**
 * set font as italic or not.
 *
 * @param nFontItalic if it is true, the font will be italic
 */
void voSubTitleFormatSettingImpl::setFontItalic(bool nFontItalic) {
	fontItalic = nFontItalic?1:0;
	fontItalicEnable = true;
}
/**
 * set font as bold or not.
 *
 * @param nBold if it is true, the font will be bold
 */
void voSubTitleFormatSettingImpl::setFontBold(bool nBold) {
	fontBold = nBold?1:0;
	fontBoldEnable = true;
}
/**
 * set font as underline or not.
 *
 * @param nFontUnderline if it is true, the font will be underline
 */
void voSubTitleFormatSettingImpl::setFontUnderline(bool nFontUnderline) {
	fontUnderline = nFontUnderline?1:0;
	fontUnderlineEnable = true;
}
/**
 * set font name.
 *
 * @param strFontName the font name for text, will be deleted next version
 */
void voSubTitleFormatSettingImpl::setFontName(TCHAR* strFontName) {
	if(strFontName!=NULL)
		_tcscpy(fontName ,  strFontName);
}
/**
 * set font style, same as setFontName.
 *
 * @param style the font style for text, style is value of below:
 * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default}
 * <li> {@link voSubTitleFormatSetting#VOOSMP_Monospaced_with_serifs}
 * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Proportionally_spaced_with_serifs}
 * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Monospaced_without_serifs}
 * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Proportionally_spaced_without_serifs}
 * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Casual}
 * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Cursive}
 * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Small_capitals}
 */
void voSubTitleFormatSettingImpl::setFontStyle(int style) {
	ConvertFontName((TCHAR*) fontName, style);
}
void voSubTitleFormatSettingImpl::ConvertFontName(TCHAR* fnm, int style)
{
	if(style == FontStyle_Monospaced_with_serifs)
		_tcscpy(fnm , _T("Courier"));
	if(style == FontStyle_Default_Proportionally_spaced_with_serifs)
		_tcscpy(fnm , _T("Times New Roman"));
	if(style == FontStyle_Default_Monospaced_without_serifs)
		_tcscpy(fnm , _T("Helvetica"));
	if(style == FontStyle_Default_Proportionally_spaced_without_serifs)
		_tcscpy(fnm , _T("Arial"));
	if(style == FontStyle_Default_Casual)
		_tcscpy(fnm , _T("Dom"));
	if(style == FontStyle_Default_Cursive)
		_tcscpy(fnm , _T("Coronet"));
	if(style == FontStyle_Default_Small_capitals)
		_tcscpy(fnm , _T("Gothic"));
}
TCHAR* voSubTitleFormatSettingImpl::GetFontName()
{
	if(_tcslen(fontName)>0)
		return fontName;
	else
		return NULL;
}
/**
 * set the edge type of font.
 *
 * @param edgeType the edge type of font, 0:NONE,1:RAISED,2:DEPRESSED,3:UNIFORM,4:LEFT_DROP_SHADOW,5:RIGHT_DROP_SHADOW
 */
void voSubTitleFormatSettingImpl::setEdgeType(int edgeType) {
	edgeType = edgeType;
	edgeTypeEnable = true;
}
/**
 * set font edge color.
 *
 * @param edgeColor the font edge color for text
 */
void voSubTitleFormatSettingImpl::setEdgeColor(COLORREF edgeColor) {
	edgeColor = edgeColor;
	edgeColorEnable = true;
}
/**
 * set edge color opacity rate.
 *
 * @param nAlpha the edge color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
 */
void voSubTitleFormatSettingImpl::setEdgeOpacity(int nAlpha){
	edgeColorOpacityRateEnable = true;
	nAlpha = checkAlpha( nAlpha);
	edgeColorOpacityRate = nAlpha;
	
}
int voSubTitleFormatSettingImpl::checkAlpha(int nAlpha)
{
	if(nAlpha<0)
		nAlpha = 0;
	if(nAlpha>100)
		nAlpha = 100;
	nAlpha = (int)(nAlpha*2.55);
	return nAlpha;
}
/**
 * Reset all parameters to default status. Subtitle will be presented as subtitle stream specified, instead of user specified
 */
void voSubTitleFormatSettingImpl::reset()
{
	edgeTypeEnable = false;
	
	fontColorEnable = false;
	edgeColorEnable = false;
	backgroundColorEnable = false;
	windowBackgroundColorEnable = false;
	fontColorOpacityRateEnable = false;
	edgeColorOpacityRateEnable = false;
	backgroundColorOpacityRateEnable = false;
	windowBackgroundColorOpacityRateEnable = false;
	
	fontSizeEnable = false;
	fontItalicEnable = false;
	fontUnderlineEnable = false;
	fontBoldEnable = false;
	_tcscpy(fontName , _T(""));
	
}
voSubtitleRGBAColor voSubTitleFormatSettingImpl::converColor(voSubtitleRGBAColor oldColor, COLORREF newColor, bool enableColor){
	if(enableColor)
	{
		oldColor.nRed = GetRValue(newColor);
		oldColor.nBlue = GetBValue(newColor);
		oldColor.nGreen = GetGValue(newColor);
	}
	return oldColor;
}
voSubtitleRGBAColor voSubTitleFormatSettingImpl::converColorOpacityRate(voSubtitleRGBAColor oldColor, int newColorOpacityRate, bool enableColorOpacityRate){
	if(enableColorOpacityRate)
	{
		oldColor.nTransparency = (newColorOpacityRate&0x00ff);
	}
	return oldColor;
}
//
