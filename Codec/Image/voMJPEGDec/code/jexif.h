#ifndef __JEXIF_H__
#define __JEXIF_H__

#include "vojconfig.h"


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
#define	TIFFTAG_COMPRESSION			259		/**< data compression technique */
#define	TIFFTAG_MAKE				271		/**< scanner manufacturer name */
#define	TIFFTAG_MODEL				272		/**< scanner model name/number */
#define	TIFFTAG_ORIENTATION			274		/**< +image orientation */
#define	TIFFTAG_XRESOLUTION			282		/**< pixels/resolution in x */
#define	TIFFTAG_YRESOLUTION			283		/**< pixels/resolution in y */
#define	TIFFTAG_RESOLUTIONUNIT		296		/**< units of resolutions */
#define	TIFFTAG_DATETIME			306		/**< creation date and time */
#define	TIFFTAG_JPEGIFOFFSET		513		/**< !pointer to SOI marker */
#define	TIFFTAG_JPEGIFBYTECOUNT		514		/**< !JFIF stream length */
#define	TIFFTAG_YCBCRPOSITIONING	531		/**< !subsample positioning */
#define TIFFTAG_EXIFIFD					34665	/**< Pointer to EXIF private directory */
#define TIFFTAG_GPSIFD					34853	/**< Pointer to GPS private directory */
#define EXIFTAG_DATETIMEORIGINAL		36867		/**< Date and time of original data generation */
#define EXIFTAG_DATETIMEDIGITIZED		36868		/**< Date and time of digital data generation */
#define EXIFTAG_PIXELXDIMENSION			40962		/**< Valid image width */
#define EXIFTAG_PIXELYDIMENSION			40963		/**< Valid image height */


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






