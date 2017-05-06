#ifndef __JEXIF_H__
#define __JEXIF_H__

#include "jconfig.h"


/**<
 * Tag data type information.
 *
 * Note: RATIONALs are the ratio of two 32-bit integer values.
 */
typedef	enum {
	IDF_NOTYPE		= 0,		/**< placeholder */
	IDF_BYTE		= 1,		/**< 8-bit unsigned integer */
	IDF_ASCII		= 2,		/**< 8-bit bytes w/ last byte null */
	IDF_SHORT		= 3,		/**< 16-bit unsigned integer */
	IDF_LONG		= 4,		/**< 32-bit unsigned integer */
	IDF_RATIONAL	= 5,		/**< 64-bit unsigned fraction */
	IDF_SBYTE		= 6,		/**< !8-bit signed integer */
	IDF_UNDEFINED	= 7,		/**< !8-bit untyped data */
	IDF_SSHORT		= 8,		/**< !16-bit signed integer */
	IDF_SLONG		= 9,		/**< !32-bit signed integer */
	IDF_SRATIONAL	= 10,		/**< !64-bit signed fraction */
	IDF_FLOAT		= 11,		/**< !32-bit IEEE floating point */
	IDF_DOUBLE		= 12,		/**< !64-bit IEEE floating point */
	IDF_IFD			= 13		/**< %32-bit unsigned integer (offset) */
} IDFDataType;

/**<
 * TIFF Tag Definitions.
 */
#define	TIFFTAG_SUBFILETYPE			254		/**< subfile data descriptor */
#define	    FILETYPE_REDUCEDIMAGE	0x1		/**< reduced resolution version */
#define	    FILETYPE_PAGE			0x2		/**< one page of many */
#define	    FILETYPE_MASK			0x4		/**< transparency mask */
#define	TIFFTAG_OSUBFILETYPE		255		/**< +kind of data in subfile */
#define	    OFILETYPE_IMAGE				1	/**< full resolution image data */
#define	    OFILETYPE_REDUCEDIMAGE		2	/**< reduced size image data */
#define	    OFILETYPE_PAGE				3	/**< one page of many */
#define	TIFFTAG_IMAGEWIDTH			256		/**< image width in pixels */
#define	TIFFTAG_IMAGELENGTH			257		/**< image height in pixels */
#define	TIFFTAG_BITSPERSAMPLE		258		/**< bits per channel (sample) */
#define	TIFFTAG_COMPRESSION			259		/**< data compression technique */
#define	    COMPRESSION_NONE			1	/**< dump mode */
#define	    COMPRESSION_CCITTRLE		2	/**< CCITT modified Huffman RLE */
#define	    COMPRESSION_CCITTFAX3		3	/**< CCITT Group 3 fax encoding */
#define     COMPRESSION_CCITT_T4		3	/**< CCITT T.4 (TIFF 6 name) */
#define	    COMPRESSION_CCITTFAX4		4	/**< CCITT Group 4 fax encoding */
#define     COMPRESSION_CCITT_T6		4	/**< CCITT T.6 (TIFF 6 name) */
#define	    COMPRESSION_LZW				5	/**< Lempel-Ziv  & Welch */
#define	    COMPRESSION_OJPEG			6	/**< !6.0 JPEG */
#define	    COMPRESSION_JPEG			7	/**< %JPEG DCT compression */
#define	TIFFTAG_PHOTOMETRIC			262		/**< photometric interpretation */
#define	    PHOTOMETRIC_MINISWHITE		0	/**< min value is white */
#define	    PHOTOMETRIC_MINISBLACK		1	/**< min value is black */
#define	    PHOTOMETRIC_RGB				2	/**< RGB color model */
#define	    PHOTOMETRIC_PALETTE			3	/**< color map indexed */
#define	    PHOTOMETRIC_MASK			4	/**< $holdout mask */
#define	    PHOTOMETRIC_SEPARATED		5	/**< !color separations */
#define	    PHOTOMETRIC_YCBCR			6	/**< !CCIR 601 */
#define	    PHOTOMETRIC_CIELAB			8	/**< !1976 CIE L*a*b* */
#define	    PHOTOMETRIC_ICCLAB			9	/**< ICC L*a*b* [Adobe TIFF Technote 4] */
#define	    PHOTOMETRIC_ITULAB			10	/**< ITU L*a*b* */
#define	TIFFTAG_THRESHHOLDING		263		/**< +thresholding used on data */
#define	    THRESHHOLD_BILEVEL			1	/**< b&w art scan */
#define	    THRESHHOLD_HALFTONE			2	/**< or dithered scan */
#define	    THRESHHOLD_ERRORDIFFUSE		3	/**< usually floyd-steinberg */
#define	TIFFTAG_CELLWIDTH			264		/**< +dithering matrix width */
#define	TIFFTAG_CELLLENGTH			265		/**< +dithering matrix height */
#define	TIFFTAG_FILLORDER			266		/**< data order within a byte */
#define	    FILLORDER_MSB2LSB			1	/**< most significant -> least */
#define	    FILLORDER_LSB2MSB			2	/**< least significant -> most */
#define	TIFFTAG_DOCUMENTNAME		269		/**< name of doc. image is from */
#define	TIFFTAG_IMAGEDESCRIPTION	270		/**< info about image */
#define	TIFFTAG_MAKE				271		/**< scanner manufacturer name */
#define	TIFFTAG_MODEL				272		/**< scanner model name/number */
#define	TIFFTAG_STRIPOFFSETS		273		/**< offsets to data strips */
#define	TIFFTAG_ORIENTATION			274		/**< +image orientation */
#define	    ORIENTATION_TOPLEFT			1	/**< row 0 top, col 0 lhs */
#define	    ORIENTATION_TOPRIGHT		2	/**< row 0 top, col 0 rhs */
#define	    ORIENTATION_BOTRIGHT		3	/**< row 0 bottom, col 0 rhs */
#define	    ORIENTATION_BOTLEFT			4	/**< row 0 bottom, col 0 lhs */
#define	    ORIENTATION_LEFTTOP			5	/**< row 0 lhs, col 0 top */
#define	    ORIENTATION_RIGHTTOP		6	/**< row 0 rhs, col 0 top */
#define	    ORIENTATION_RIGHTBOT		7	/**< row 0 rhs, col 0 bottom */
#define	    ORIENTATION_LEFTBOT			8	/**< row 0 lhs, col 0 bottom */
#define	TIFFTAG_SAMPLESPERPIXEL		277		/**< samples per pixel */
#define	TIFFTAG_ROWSPERSTRIP		278		/**< rows per strip of data */
#define	TIFFTAG_STRIPBYTECOUNTS		279		/**< bytes counts for strips */
#define	TIFFTAG_MINSAMPLEVALUE		280		/**< +minimum sample value */
#define	TIFFTAG_MAXSAMPLEVALUE		281		/**< +maximum sample value */
#define	TIFFTAG_XRESOLUTION			282		/**< pixels/resolution in x */
#define	TIFFTAG_YRESOLUTION			283		/**< pixels/resolution in y */
#define	TIFFTAG_PLANARCONFIG		284		/**< storage organization */
#define	    PLANARCONFIG_CONTIG			1	/**< single image plane */
#define	    PLANARCONFIG_SEPARATE		2	/**< separate planes of data */
#define	TIFFTAG_PAGENAME			285		/**< page name image is from */
#define	TIFFTAG_XPOSITION			286		/**< x page offset of image lhs */
#define	TIFFTAG_YPOSITION			287		/**< y page offset of image lhs */
#define	TIFFTAG_FREEOFFSETS			288		/**< +byte offset to free block */
#define	TIFFTAG_FREEBYTECOUNTS		289		/**< +sizes of free blocks */
#define	TIFFTAG_GRAYRESPONSEUNIT	290		/**< $gray scale curve accuracy */
#define	    GRAYRESPONSEUNIT_10S		1	/**< tenths of a unit */
#define	    GRAYRESPONSEUNIT_100S		2	/**< hundredths of a unit */
#define	    GRAYRESPONSEUNIT_1000S		3	/**< thousandths of a unit */
#define	    GRAYRESPONSEUNIT_10000S		4	/**< ten-thousandths of a unit */
#define	    GRAYRESPONSEUNIT_100000S	5	/**< hundred-thousandths */
#define	TIFFTAG_GRAYRESPONSECURVE	291		/**< $gray scale response curve */
#define	TIFFTAG_GROUP3OPTIONS		292		/**< 32 flag bits */
#define	TIFFTAG_T4OPTIONS			292		/**< TIFF 6.0 proper name alias */
#define	    GROUP3OPT_2DENCODING	0x1		/**< 2-dimensional coding */
#define	    GROUP3OPT_UNCOMPRESSED	0x2		/**< data not compressed */
#define	    GROUP3OPT_FILLBITS		0x4		/**< fill to byte boundary */
#define	TIFFTAG_GROUP4OPTIONS		293		/**< 32 flag bits */
#define TIFFTAG_T6OPTIONS           293     /**< TIFF 6.0 proper name */
#define	    GROUP4OPT_UNCOMPRESSED	0x2		/**< data not compressed */
#define	TIFFTAG_RESOLUTIONUNIT		296		/**< units of resolutions */
#define	    RESUNIT_NONE				1	/**< no meaningful units */
#define	    RESUNIT_INCH				2	/**< english */
#define	    RESUNIT_CENTIMETER			3	/**< metric */
#define	TIFFTAG_PAGENUMBER			297		/**< page numbers of multi-page */
#define	TIFFTAG_COLORRESPONSEUNIT	300		/**< $color curve accuracy */
#define	    COLORRESPONSEUNIT_10S		1	/**< tenths of a unit */
#define	    COLORRESPONSEUNIT_100S		2	/**< hundredths of a unit */
#define	    COLORRESPONSEUNIT_1000S		3	/**< thousandths of a unit */
#define	    COLORRESPONSEUNIT_10000S	4	/**< ten-thousandths of a unit */
#define	    COLORRESPONSEUNIT_100000S	5	/**< hundred-thousandths */
#define	TIFFTAG_TRANSFERFUNCTION	301		/**< !colorimetry info */
#define	TIFFTAG_SOFTWARE			305		/**< name & release */
#define	TIFFTAG_DATETIME			306		/**< creation date and time */
#define	TIFFTAG_ARTIST				315		/**< creator of image */
#define	TIFFTAG_HOSTCOMPUTER		316		/**< machine where created */
#define	TIFFTAG_PREDICTOR			317		/**< prediction scheme w/ LZW */
#define     PREDICTOR_NONE				1	/**< no prediction scheme used */
#define     PREDICTOR_HORIZONTAL		2	/**< horizontal differencing */
#define     PREDICTOR_FLOATINGPOINT		3	/**< floating point predictor */
#define	TIFFTAG_WHITEPOINT			318		/**< image white point */
#define	TIFFTAG_PRIMARYCHROMATICITIES	319	/**< !primary chromaticities */
#define	TIFFTAG_COLORMAP			320		/**< RGB map for pallette image */
#define	TIFFTAG_HALFTONEHINTS		321		/**< !highlight+shadow info */
#define	TIFFTAG_TILEWIDTH			322		/**< !tile width in pixels */
#define	TIFFTAG_TILELENGTH			323		/**< !tile height in pixels */
#define TIFFTAG_TILEOFFSETS			324		/**< !offsets to data tiles */
#define TIFFTAG_TILEBYTECOUNTS		325		/**< !byte counts for tiles */
#define	TIFFTAG_BADFAXLINES			326		/**< lines w/ wrong pixel count */
#define	TIFFTAG_CLEANFAXDATA		327		/**< regenerated line info */
#define	    CLEANFAXDATA_CLEAN			0	/**< no errors detected */
#define	    CLEANFAXDATA_REGENERATED	1	/**< receiver regenerated lines */
#define	    CLEANFAXDATA_UNCLEAN		2	/**< uncorrected errors exist */
#define	TIFFTAG_CONSECUTIVEBADFAXLINES	328	/**< max consecutive bad lines */
#define	TIFFTAG_SUBIFD				330		/**< subimage descriptors */
#define	TIFFTAG_INKSET				332		/**< !inks in separated image */
#define	    INKSET_CMYK					1	/**< !cyan-magenta-yellow-black color */
#define	    INKSET_MULTIINK				2	/**< !multi-ink or hi-fi color */
#define	TIFFTAG_INKNAMES			333		/**< !ascii names of inks */
#define	TIFFTAG_NUMBEROFINKS		334		/**< !number of inks */
#define	TIFFTAG_DOTRANGE			336		/**< !0% and 100% dot codes */
#define	TIFFTAG_TARGETPRINTER		337		/**< !separation target */
#define	TIFFTAG_EXTRASAMPLES		338		/**< !info about extra samples */
#define	    EXTRASAMPLE_UNSPECIFIED		0	/**< !unspecified data */
#define	    EXTRASAMPLE_ASSOCALPHA		1	/**< !associated alpha data */
#define	    EXTRASAMPLE_UNASSALPHA		2	/**< !unassociated alpha data */
#define	TIFFTAG_SAMPLEFORMAT		339		/**< !data sample format */
#define	    SAMPLEFORMAT_UINT			1	/**< !unsigned integer data */
#define	    SAMPLEFORMAT_INT			2	/**< !signed integer data */
#define	    SAMPLEFORMAT_IEEEFP			3	/**< !IEEE floating point data */
#define	    SAMPLEFORMAT_VOID			4	/**< !untyped data */
#define	    SAMPLEFORMAT_COMPLEXINT		5	/**< !complex signed int */
#define	    SAMPLEFORMAT_COMPLEXIEEEFP	6	/**< !complex ieee floating */
#define	TIFFTAG_SMINSAMPLEVALUE		340		/**< !variable MinSampleValue */
#define	TIFFTAG_SMAXSAMPLEVALUE		341		/**< !variable MaxSampleValue */
#define	TIFFTAG_CLIPPATH			343		/**< %ClipPath [Adobe TIFF technote 2] */
#define	TIFFTAG_XCLIPPATHUNITS		344		/**< %XClipPathUnits [Adobe TIFF technote 2] */
#define	TIFFTAG_YCLIPPATHUNITS		345		/**< %YClipPathUnits [Adobe TIFF technote 2] */
#define	TIFFTAG_INDEXED				346		/**< %Indexed [Adobe TIFF Technote 3] */
#define	TIFFTAG_JPEGTABLES			347		/**< %JPEG table stream */
#define	TIFFTAG_OPIPROXY			351		/**< %OPI Proxy [Adobe TIFF technote] */
/**<
 * Tags 512-521 are obsoleted by Technical Note #2 which specifies a
 * revised JPEG-in-TIFF scheme.
 */
#define	TIFFTAG_JPEGPROC			512		/**< !JPEG processing algorithm */
#define	    JPEGPROC_BASELINE			1	/**< !baseline sequential */
#define	    JPEGPROC_LOSSLESS			14	/**< !Huffman coded lossless */
#define	TIFFTAG_JPEGIFOFFSET		513		/**< !pointer to SOI marker */
#define	TIFFTAG_JPEGIFBYTECOUNT		514		/**< !JFIF stream length */
#define	TIFFTAG_JPEGRESTARTINTERVAL	515		/**< !restart interval length */
#define	TIFFTAG_JPEGLOSSLESSPREDICTORS	517	/**< !lossless proc predictor */
#define	TIFFTAG_JPEGPOINTTRANSFORM	518		/**< !lossless point transform */
#define	TIFFTAG_JPEGQTABLES			519		/**< !Q matrice offsets */
#define	TIFFTAG_JPEGDCTABLES		520		/**< !DCT table offsets */
#define	TIFFTAG_JPEGACTABLES		521		/**< !AC coefficient offsets */
#define	TIFFTAG_YCBCRCOEFFICIENTS	529		/**< !RGB -> YCbCr transform */
#define	TIFFTAG_YCBCRSUBSAMPLING	530		/**< !YCbCr subsampling factors */
#define	TIFFTAG_YCBCRPOSITIONING	531		/**< !subsample positioning */
#define	    YCBCRPOSITION_CENTERED		1	/**< !as in PostScript Level 2 */
#define	    YCBCRPOSITION_COSITED		2	/**< !as in CCIR 601-1 */
#define	TIFFTAG_REFERENCEBLACKWHITE	532		/**< !colorimetry info */
#define	TIFFTAG_XMLPACKET			700		/**< %XML packet [Adobe XMP Specification, January 2004 */
#define TIFFTAG_OPIIMAGEID			32781	/**< %OPI ImageID  [Adobe TIFF technote] */
#define TIFFTAG_REFPTS				32953	/**< image reference points */
#define TIFFTAG_REGIONTACKPOINT		32954	/**< region-xform tack point */
#define TIFFTAG_REGIONWARPCORNERS	32955	/**< warp quadrilateral */
#define TIFFTAG_REGIONAFFINE		32956	/**< affine transformation mat */
#define	TIFFTAG_MATTEING			32995	/**< $use ExtraSamples */
#define	TIFFTAG_DATATYPE			32996	/**< $use SampleFormat */
#define	TIFFTAG_IMAGEDEPTH			32997	/**< z depth of image */
#define	TIFFTAG_TILEDEPTH			32998	/**< z depth/data tile */

/**< tags 33300-33309 are private tags registered to Pixar */
/**<
 * TIFFTAG_PIXAR_IMAGEFULLWIDTH and TIFFTAG_PIXAR_IMAGEFULLLENGTH
 * are set when an image has been cropped out of a larger image.  
 * They reflect the size of the original uncropped image.
 * The TIFFTAG_XPOSITION and TIFFTAG_YPOSITION can be used
 * to determine the position of the smaller image in the larger one.
 */
#define TIFFTAG_PIXAR_IMAGEFULLWIDTH    33300   /**< full image size in x */
#define TIFFTAG_PIXAR_IMAGEFULLLENGTH   33301   /**< full image size in y */
#define TIFFTAG_PIXAR_TEXTUREFORMAT		33302	/**< texture map format */
#define TIFFTAG_PIXAR_WRAPMODES			33303	/**< s & t wrap modes */
#define TIFFTAG_PIXAR_FOVCOT			33304	/**< cotan(fov) for env. maps */
#define TIFFTAG_PIXAR_MATRIX_WORLDTOSCREEN 33305
#define TIFFTAG_PIXAR_MATRIX_WORLDTOCAMERA 33306
#define TIFFTAG_WRITERSERIALNUMBER		33405   /**< device serial number, a private tag registered to Eastman Kodak */
#define	TIFFTAG_COPYRIGHT				33432	/**< copyright string */
#define TIFFTAG_RICHTIFFIPTC			33723	/**< IPTC TAG from RichTIFF specifications */
#define TIFFTAG_IT8SITE					34016	/**< site name */
#define TIFFTAG_IT8COLORSEQUENCE		34017	/**< color seq. [RGB,CMYK,etc] */
#define TIFFTAG_IT8HEADER				34018	/**< DDES Header */
#define TIFFTAG_IT8RASTERPADDING		34019	/**< raster scanline padding */
#define TIFFTAG_IT8BITSPERRUNLENGTH		34020	/**< # of bits in short run */
#define TIFFTAG_IT8BITSPEREXTENDEDRUNLENGTH 34021/**< # of bits in long run */
#define TIFFTAG_IT8COLORTABLE			34022	/**< LW colortable */
#define TIFFTAG_IT8IMAGECOLORINDICATOR	34023	/**< BP/BL image color switch */
#define TIFFTAG_IT8BKGCOLORINDICATOR	34024	/**< BP/BL bg color switch */
#define TIFFTAG_IT8IMAGECOLORVALUE		34025	/**< BP/BL image color value */
#define TIFFTAG_IT8BKGCOLORVALUE		34026	/**< BP/BL bg color value */
#define TIFFTAG_IT8PIXELINTENSITYRANGE	34027	/**< MP pixel intensity value */
#define TIFFTAG_IT8TRANSPARENCYINDICATOR 34028	/**< HC transparency switch */
#define TIFFTAG_IT8COLORCHARACTERIZATION 34029	/**< color character. table */
#define TIFFTAG_IT8HCUSAGE				34030	/**< HC usage indicator */
#define TIFFTAG_IT8TRAPINDICATOR		34031	/**< Trapping indicator (untrapped=0, trapped=1) */
#define TIFFTAG_IT8CMYKEQUIVALENT		34032	/**< CMYK color equivalents */
#define TIFFTAG_FRAMECOUNT				34232   /**< Sequence Frame Count, private tags registered to Texas Instruments*/
#define TIFFTAG_PHOTOSHOP				34377	/**< private tag registered to Adobe for PhotoShop */
#define TIFFTAG_EXIFIFD					34665	/**< Pointer to EXIF private directory */
#define TIFFTAG_ICCPROFILE				34675	/**< ICC profile data */
#define	TIFFTAG_JBIGOPTIONS				34750	/**< JBIG options */
#define TIFFTAG_GPSIFD					34853	/**< Pointer to GPS private directory */
#define	TIFFTAG_FAXRECVPARAMS			34908	/**< encoded Class 2 ses. parms */
#define	TIFFTAG_FAXSUBADDRESS			34909	/**< received SubAddr string */
#define	TIFFTAG_FAXRECVTIME				34910	/**< receive time (secs) */
#define	TIFFTAG_FAXDCS					34911	/**< encoded fax ses. params, Table 2/T.30 */
#define TIFFTAG_STONITS					37439	/**< Sample value to Nits */
#define	TIFFTAG_FEDEX_EDR				34929	/**< unknown use */
#define TIFFTAG_INTEROPERABILITYIFD		40965	/**< Pointer to Interoperability private directory */
#define TIFFTAG_DNGVERSION				50706	/**< &DNG version number */
#define TIFFTAG_DNGBACKWARDVERSION		50707	/**< &DNG compatibility version */
#define TIFFTAG_UNIQUECAMERAMODEL		50708	/**< &name for the camera model */
#define TIFFTAG_LOCALIZEDCAMERAMODEL	50709	/**< &localized camera model name */
#define TIFFTAG_CFAPLANECOLOR			50710	/**< &CFAPattern->LinearRaw space mapping */
#define TIFFTAG_CFALAYOUT				50711	/**< &spatial layout of the CFA */
#define TIFFTAG_LINEARIZATIONTABLE		50712	/**< &lookup table description */
#define TIFFTAG_BLACKLEVELREPEATDIM		50713	/**< &repeat pattern size for the BlackLevel tag */
#define TIFFTAG_BLACKLEVEL				50714	/**< &zero light encoding level */
#define TIFFTAG_BLACKLEVELDELTAH		50715	/**< &zero light encoding level differences (columns) */
#define TIFFTAG_BLACKLEVELDELTAV		50716	/**< &zero light encoding level differences (rows) */
#define TIFFTAG_WHITELEVEL				50717	/**< &fully saturated encoding level */
#define TIFFTAG_DEFAULTSCALE			50718	/**< &default scale factors */
#define TIFFTAG_DEFAULTCROPORIGIN		50719	/**< &origin of the final image area */
#define TIFFTAG_DEFAULTCROPSIZE			50720	/**< &size of the final image area */
#define TIFFTAG_COLORMATRIX1			50721	/**< &XYZ->reference color space transformation matrix 1 */
#define TIFFTAG_COLORMATRIX2			50722	/**< &XYZ->reference color space transformation matrix 2 */
#define TIFFTAG_CAMERACALIBRATION1		50723	/**< &calibration matrix 1 */
#define TIFFTAG_CAMERACALIBRATION2		50724	/**< &calibration matrix 2 */
#define TIFFTAG_REDUCTIONMATRIX1		50725	/**< &dimensionality reduction matrix 1 */
#define TIFFTAG_REDUCTIONMATRIX2		50726	/**< &dimensionality reduction matrix 2 */
#define TIFFTAG_ANALOGBALANCE			50727	/**< &gain applied the stored raw values*/
#define TIFFTAG_ASSHOTNEUTRAL			50728	/**< &selected white balance in linear reference space */
#define TIFFTAG_ASSHOTWHITEXY			50729	/**< &selected white balance in x-y chromaticity coordinates */
#define TIFFTAG_BASELINEEXPOSURE		50730	/**< &how much to move the zero point */
#define TIFFTAG_BASELINENOISE			50731	/**< &relative noise level */
#define TIFFTAG_BASELINESHARPNESS		50732	/**< &relative amount of sharpening */
#define TIFFTAG_BAYERGREENSPLIT			50733	/**< &how closely the values of the green pixels in the
						   blue/green rows track the values of the green pixels in the red/green rows */
#define TIFFTAG_LINEARRESPONSELIMIT		50734	/**< &non-linear encoding range */
#define TIFFTAG_CAMERASERIALNUMBER		50735	/**< &camera's serial number */
#define TIFFTAG_LENSINFO				50736	/**< info about the lens */
#define TIFFTAG_CHROMABLURRADIUS		50737	/**< &chroma blur radius */
#define TIFFTAG_ANTIALIASSTRENGTH		50738	/**< &relative strength of the  camera's anti-alias filter */
#define TIFFTAG_SHADOWSCALE				50739	/**< &used by Adobe Camera Raw */
#define TIFFTAG_DNGPRIVATEDATA			50740	/**< &manufacturer's private data */
#define TIFFTAG_MAKERNOTESAFETY			50741	/**< &whether the EXIF MakerNote tag is safe to preserve
						   along with the rest of the EXIF data */
#define	TIFFTAG_CALIBRATIONILLUMINANT1	50778	/**< &illuminant 1 */
#define TIFFTAG_CALIBRATIONILLUMINANT2	50779	/**< &illuminant 2 */
#define TIFFTAG_BESTQUALITYSCALE		50780	/**< &best quality multiplier */
#define TIFFTAG_RAWDATAUNIQUEID			50781	/**< &unique identifier for the raw image data */
#define TIFFTAG_ORIGINALRAWFILENAME		50827	/**< &file name of the original raw file */
#define TIFFTAG_ORIGINALRAWFILEDATA		50828	/**< &contents of the original raw file */
#define TIFFTAG_ACTIVEAREA				50829	/**< &active (non-masked) pixels of the sensor */
#define TIFFTAG_MASKEDAREAS				50830	/**< &list of coordinates of fully masked pixels */
#define TIFFTAG_ASSHOTICCPROFILE		50831	/**< &these two tags used to */
#define TIFFTAG_ASSHOTPREPROFILEMATRIX	50832	/**< map cameras's color space into ICC profile space */
#define TIFFTAG_CURRENTICCPROFILE		50833	/**< & */
#define TIFFTAG_CURRENTPREPROFILEMATRIX	50834	/**< & */
#define TIFFTAG_DCSHUESHIFTVALUES		65535   /**< hue shift correction data */

/**<
 * The following are ``pseudo tags'' that can be used to control
 * codec-specific functionality.  These tags are not written to file.
 * Note that these values start at 0xffff+1 so that they'll never
 * collide with Aldus-assigned tags.
 *
 * If you want your private pseudo tags ``registered'' (i.e. added to
 * this file), please post a bug report via the tracking system at
 * http://www.remotesensing.org/libtiff/bugs.html with the appropriate
 * C definitions to add.
 */
#define	TIFFTAG_FAXMODE					65536		/**< Group 3/4 format control */
#define	    FAXMODE_CLASSIC					0x0000	/**< default, include RTC */
#define	    FAXMODE_NORTC					0x0001	/**< no RTC at end of data */
#define	    FAXMODE_NOEOL					0x0002	/**< no EOL code at end of row */
#define	    FAXMODE_BYTEALIGN				0x0004	/**< byte align row */
#define	    FAXMODE_WORDALIGN				0x0008	/**< word align row */
#define	    FAXMODE_CLASSF	FAXMODE_NORTC			/**< TIFF Class F */
#define	TIFFTAG_JPEGQUALITY				65537		/**< Compression quality level */
#define	TIFFTAG_JPEGCOLORMODE			65538		/**< Auto RGB<=>YCbCr convert? */
#define	    JPEGCOLORMODE_RAW				0x0000	/**< no conversion (default) */
#define	    JPEGCOLORMODE_RGB				0x0001	/**< do auto conversion */
#define	TIFFTAG_JPEGTABLESMODE			65539		/**< What to put in JPEGTables */
#define	    JPEGTABLESMODE_QUANT			0x0001	/**< include quantization tbls */
#define	    JPEGTABLESMODE_HUFF				0x0002	/**< include Huffman tbls */
#define	TIFFTAG_FAXFILLFUNC				65540		/**< G3/G4 fill function */
#define	TIFFTAG_PIXARLOGDATAFMT			65549		/**< PixarLogCodec I/O data sz */
#define	    PIXARLOGDATAFMT_8BIT			0		/**< regular u_char samples */
#define	    PIXARLOGDATAFMT_8BITABGR		1		/**< ABGR-order u_chars */
#define	    PIXARLOGDATAFMT_11BITLOG		2		/**< 11-bit log-encoded (raw) */
#define	    PIXARLOGDATAFMT_12BITPICIO		3		/**< as per PICIO (1.0==2048) */
#define	    PIXARLOGDATAFMT_16BIT			4		/**< signed short samples */
#define	    PIXARLOGDATAFMT_FLOAT			5		/**< IEEE float samples */
#define TIFFTAG_DCSIMAGERTYPE           65550	   /**< imager model & filter */
#define     DCSIMAGERMODEL_M3				0       /**< M3 chip (1280 x 1024) */
#define     DCSIMAGERMODEL_M5				1       /**< M5 chip (1536 x 1024) */
#define     DCSIMAGERMODEL_M6				2       /**< M6 chip (3072 x 2048) */
#define     DCSIMAGERFILTER_IR				0       /**< infrared filter */
#define     DCSIMAGERFILTER_MONO			1       /**< monochrome filter */
#define     DCSIMAGERFILTER_CFA				2       /**< color filter array */
#define     DCSIMAGERFILTER_OTHER			3       /**< other filter */
#define TIFFTAG_DCSINTERPMODE           65551		/**< interpolation mode */
#define     DCSINTERPMODE_NORMAL			0x0		/**< whole image, default */
#define     DCSINTERPMODE_PREVIEW			0x1		/**< preview of image (384x256) */
#define TIFFTAG_DCSBALANCEARRAY         65552		/**< color balance values */
#define TIFFTAG_DCSCORRECTMATRIX        65553		/**< color correction values */
#define TIFFTAG_DCSGAMMA                65554		/**< gamma value */
#define TIFFTAG_DCSTOESHOULDERPTS       65555		/**< toe & shoulder points */
#define TIFFTAG_DCSCALIBRATIONFD        65556		/**< calibration file desc */
/**< Note: quality level is on the ZLIB 1-9 scale. Default value is -1 */
#define	TIFFTAG_ZIPQUALITY				65557		/**< compression quality level */
#define	TIFFTAG_PIXARLOGQUALITY			65558		/**< PixarLog uses same scale */
#define TIFFTAG_DCSCLIPRECTANGLE		65559		/**< area of image to acquire */
#define TIFFTAG_SGILOGDATAFMT			65560		/**< SGILog user data format */
#define     SGILOGDATAFMT_FLOAT				0		/**< IEEE float samples */
#define     SGILOGDATAFMT_16BIT				1		/**< 16-bit samples */
#define     SGILOGDATAFMT_RAW				2		/**< uninterpreted data */
#define     SGILOGDATAFMT_8BIT				3		/**< 8-bit RGB monitor values */
#define TIFFTAG_SGILOGENCODE			65561		/**< SGILog data encoding control*/
#define     SGILOGENCODE_NODITHER			0		/**< do not dither encoded values*/
#define     SGILOGENCODE_RANDITHER			1		/**< randomly dither encd values */

/**<
 * EXIF tags
 */
#define EXIFTAG_EXPOSURETIME			33434		/**< Exposure time */
#define EXIFTAG_FNUMBER					33437		/**< F number */
#define EXIFTAG_EXPOSUREPROGRAM			34850		/**< Exposure program */
#define EXIFTAG_SPECTRALSENSITIVITY		34852		/**< Spectral sensitivity */
#define EXIFTAG_ISOSPEEDRATINGS			34855		/**< ISO speed rating */
#define EXIFTAG_OECF					34856		/**< Optoelectric conversion  factor */
#define EXIFTAG_EXIFVERSION				36864		/**< Exif version */
#define EXIFTAG_DATETIMEORIGINAL		36867		/**< Date and time of original data generation */
#define EXIFTAG_DATETIMEDIGITIZED		36868		/**< Date and time of digital data generation */
#define EXIFTAG_COMPONENTSCONFIGURATION	37121		/**< Meaning of each component */
#define EXIFTAG_COMPRESSEDBITSPERPIXEL	37122		/**< Image compression mode */
#define EXIFTAG_SHUTTERSPEEDVALUE		37377		/**< Shutter speed */
#define EXIFTAG_APERTUREVALUE			37378		/**< Aperture */
#define EXIFTAG_BRIGHTNESSVALUE			37379		/**< Brightness */
#define EXIFTAG_EXPOSUREBIASVALUE		37380		/**< Exposure bias */
#define EXIFTAG_MAXAPERTUREVALUE		37381		/**< Maximum lens aperture */
#define EXIFTAG_SUBJECTDISTANCE			37382		/**< Subject distance */
#define EXIFTAG_METERINGMODE			37383		/**< Metering mode */
#define EXIFTAG_LIGHTSOURCE				37384		/**< Light source */
#define EXIFTAG_FLASH					37385		/**< Flash */
#define EXIFTAG_FOCALLENGTH				37386		/**< Lens focal length */
#define EXIFTAG_SUBJECTAREA				37396		/**< Subject area */
#define EXIFTAG_MAKERNOTE				37500		/**< Manufacturer notes */
#define EXIFTAG_USERCOMMENT				37510		/**< User comments */
#define EXIFTAG_SUBSECTIME				37520		/**< DateTime subseconds */
#define EXIFTAG_SUBSECTIMEORIGINAL		37521		/**< DateTimeOriginal subseconds */
#define EXIFTAG_SUBSECTIMEDIGITIZED		37522		/**< DateTimeDigitized subseconds */
#define EXIFTAG_FLASHPIXVERSION			40960		/**< Supported Flashpix version */
#define EXIFTAG_COLORSPACE				40961		/**< Color space information */
#define EXIFTAG_PIXELXDIMENSION			40962		/**< Valid image width */
#define EXIFTAG_PIXELYDIMENSION			40963		/**< Valid image height */
#define EXIFTAG_RELATEDSOUNDFILE		40964		/**< Related audio file */
#define EXIFTAG_FLASHENERGY				41483		/**< Flash energy */
#define EXIFTAG_SPATIALFREQUENCYRESPONSE 41484		/**< Spatial frequency response */
#define EXIFTAG_FOCALPLANEXRESOLUTION	41486		/**< Focal plane X resolution */
#define EXIFTAG_FOCALPLANEYRESOLUTION	41487		/**< Focal plane Y resolution */
#define EXIFTAG_FOCALPLANARESOLUTIONUNIT 41488		/**< Focal plane resolution unit */
#define EXIFTAG_SUBJECTLOCATION			41492		/**< Subject location */
#define EXIFTAG_EXPOSUREINDEX			41493		/**< Exposure index */
#define EXIFTAG_SENSINGMETHOD			41495		/**< Sensing method */
#define EXIFTAG_FILESOURCE				41728		/**< File source */
#define EXIFTAG_SCENETYPE				41729		/**< Scene type */
#define EXIFTAG_CFAPATTERN				41730		/**< CFA pattern */
#define EXIFTAG_CUSTOMRENDERED			41985		/**< Custom image processing */
#define EXIFTAG_EXPOSUREMODE			41986		/**< Exposure mode */
#define EXIFTAG_WHITEBALANCE			41987		/**< White balance */
#define EXIFTAG_DIGITALZOOMRATIO		41988		/**< Digital zoom ratio */
#define EXIFTAG_FOCALLENGTHIN35MMFILM	41989		/**< Focal length in 35 mm film */
#define EXIFTAG_SCENECAPTURETYPE		41990		/**< Scene capture type */
#define EXIFTAG_GAINCONTROL				41991		/**< Gain control */
#define EXIFTAG_CONTRAST				41992		/**< Contrast */
#define EXIFTAG_SATURATION				41993		/**< Saturation */
#define EXIFTAG_SHARPNESS				41994		/**< Sharpness */
#define EXIFTAG_DEVICESETTINGDESCRIPTION 41995		/**< Device settings description */
#define EXIFTAG_SUBJECTDISTANCERANGE	41996		/**< Subject distance range */
#define EXIFTAG_GAINCONTROL				41991		/**< Gain control */
#define EXIFTAG_GAINCONTROL				41991		/**< Gain control */
#define EXIFTAG_IMAGEUNIQUEID			42016		/**< Unique image ID */


#define GPSTAG_VERSIONIF				0		
#define GPSTAG_LATITUDEREF				1
#define GPSTAG_LATITUDE					2
#define GPSTAG_LONGITUDEREF				3
#define GPSTAG_LONGITUDE				4
#define GPSTAG_ALITUDEREF				5
#define GPSTAG_ALITUDE					6
#define GPSTAG_TIMESTAMP				7
#define GPSTAG_SATELLITES				8
#define GPSTAG_STATUS					9
#define		MEASURE_IN_PROCESSION			'A'
#define		MEASURE_INTERO					'V'
#define GPSTAG_MEASUREMODE				10
#define		DIM_2							'2'
#define		DIM_3							'3'
#define GPSTAG_DOP						11
#define GPSTAG_SPEEDREF					12
#define		KILOM_PH						'K'
#define		MILES_PH						'M'
#define		KNOTES							'N'	
#define GPSTAG_SPEED					13
#define GPSTAG_TRACKREF					14
#define		TRUE_DIRE						'T'
#define		MAGN_DIRE						'M'
#define GPSTAG_TRACK					15
#define GPSTAG_IMGDIRECTIONREF			16
#define GPSTAG_IMGDIRECTION				17	
#define GPSTAG_MAPDATUM					18
#define GPSTAG_DESTLATITUDEREF			19		
#define GPSTAG_DESTLATITUDE				20
#define GPSTAG_DESTLONGITUDEREF			21
#define GPSTAG_DESTLONGITUDE			22
#define GPSTAG_DESTBEARINGREF			23
#define GPSTAG_DESTBEARING				24
#define GPSTAG_DESTDISTANCEREF			25
#define GPSTAG_DESTDISTANCE				26
#define	GPSTAG_PROCESSINGMETHOD			27
#define	GPSTAG_AVREAINFORMATION			28
#define GPSTAG_DATESTAMP				29
#define GPSTAG_DIFFERENTIAL				30


typedef	struct {
	UINT16		idf_tag;		/**< see top */
	UINT16		idf_type;		/**< data type; see IDFDataType */
	UINT32		idf_count;		/**< number of items; length in spec */
	UINT32		idf_offset;		/**< byte offset to field data */
} IDFEntry;

typedef	struct {
	UINT8	CameraMake[40];				/*	0x00000001	*/
    UINT8	CameraModel[40];			/*	0x00000002	*/
    UINT8	DateTime[20];				/*	0x00000004	*/
	UINT8	Software[20];				/*	0x00000008	*/
	UINT32	Orientation;				/*	0x00000010	*/
	UINT32	XResolution[2];				/*	0x00000020	*/
	UINT32	YResolution[2];				/*	0x00000040	*/
	UINT32	ResolutionUnit;				/*	0x00000080	*/
	UINT32	YCbCrPositioning;			/*	0x00000100	*/
	UINT32	exifIFDPointer;				/*	0x00000200	*/
	UINT32	GPSIFDPointer;				/*	0x00000400	*/	
} TIFFDirectory;


typedef	struct {
	UINT32	ExpoureTime[2];				/*	0x00000001	*/
	UINT32	FNumber[2];					/*	0x00000002	*/
	UINT32	ExifVersion;				/*	0x00000004	*/
	UINT8	DataTimeOriginal[20];		/*	0x00000008	*/
	UINT8	DataTimeDigitized[20];		/*	0x00000010	*/	
	UINT32	ComponentsConfiguration;	/*	0x00000020	*/
	UINT32	CompressedBitsPerPixel[2];	/*	0x00000040	*/
	UINT32	ShutterSpeedValue[2];		/*	0x00000080	*/
	UINT32	ApertureValue[2];			/*	0x00000100	*/
	UINT32	BrightnessValue[2];			/*	0x00000200	*/
	UINT32	ExposureBiasValue[2];		/*	0x00000400	*/
	UINT32	MaxApertureRationValue[2];	/*	0x00000800	*/
	UINT32	SubjectDistance;			/*	0x00001000	*/
	UINT32	MeteringMode;				/*	0x00002000	*/
	UINT32	LightSource;				/*	0x00004000	*/
	UINT32	Flash;						/*	0x00008000	*/
	UINT32	FlocalLength;				/*	0x00010000	*/
	UINT32	SubSecTime;					/*	0x00020000	*/
	UINT32	SubSecTimeOriginal;			/*	0x00040000	*/
	UINT32	SubSecTimeDigitized;		/*	0x00080000	*/
	UINT32	FlashpixVersion;			/*	0x00100000	*/
	UINT32	Colorspace;					/*	0x00200000	*/
	UINT32	PixelXDimension;			/*	0x00400000	*/
	UINT32	PixelYDimension;			/*	0x00800000	*/
	UINT8*	UserComments;				/*	0x01000000	*/
} EXIFDirectory;

typedef	struct {
	UINT8	GPSVersionIF[4];			/*	0x00000001	*/
	UINT32	GPSLatitudeRef;				/*	0x00000002	*/
	UINT32	GPSLatitude[6];				/*	0x00000004	*/
	UINT32	GPSLongtiudeRef;			/*	0x00000008	*/
	UINT32	GPSLongtiude[6];			/*	0x00000010	*/
	UINT32	GPSAltitudeRef;				/*	0x00000020	*/
	UINT32	GPSAltitude[2];				/*	0x00000040	*/
	UINT32	GPSTimeStamp;				/*	0x00000080	*/
	UINT8	GPSSatellites[40];			/*	0x00000100	*/
	UINT32	GPSStatus;					/*	0x00000200	*/	
	UINT32	GPSMeasureMode;				/*	0x00000400	*/		
	UINT32	GPSDOP;						/*	0x00000800	*/
	UINT32  GPSSpeedRef;				/*	0x00001000	*/
	UINT32	GPSSpeed[2];				/*	0x00002000	*/
	UINT32	GPSTrackRef;				/*	0x00004000	*/
	UINT32	GPSTrack[2];				/*	0x00008000	*/
	UINT32	GPSImgDirectionRef;			/*	0x00010000	*/
	UINT32	GPSImgDirection[2];			/*	0x00020000	*/
	UINT8	GPSMapDatum[20];			/*	0x00040000	*/
	UINT8	GPSDatastemp[12];			/*	0x00080000	*/
} GPSDirectory;

typedef	struct {
	UINT32	Compression;				/*	0x00000001	*/
	UINT32	XResolution[2];				/*	0x00000002	*/
	UINT32	YResolution[2];				/*	0x00000004	*/
	UINT32	ResolutionUnit;				/*	0x00000008	*/
	UINT32	JPEGOffSet;					/*	0x00000010	*/
	UINT32	JPEGByteCount;				/*	0x00000020	*/
	UINT32	YCbCrPositioning;			/*	0x00000040	*/
} ThumbnailEXIFDirectory;

typedef	struct {
	TIFFDirectory	Tiffdir;
	EXIFDirectory	Exifdir;
	GPSDirectory	Gpsdir;	
	ThumbnailEXIFDirectory thumbdir;
	UINT32	nextIDFP; 
	UINT32	exiflength;
	UINT32	enTIFFD;
	UINT32	enEXIFDir;
	UINT32	enGPSDir;
	UINT32	enThumbEXIFDir;
	UINT32	NumberTIFFD;
	UINT32	NumberEXIFDir;
	UINT32	NumberGPSDir;
	UINT32	NumberThumbEXIFDir;
	UINT32	thumWidth;
	UINT32	thumHeight;
} EXIFLibrary;


extern const UINT32 BytesPerFormat[];

#endif






