#include <time.h>
#include "jexif.h"
#include "mem_align.h"
#include "jcommon.h"
//#include "jpegenc.h"
#include "voJPEG.h"

#include "stdio.h"

VO_U32 VO_API voJPEGEncSetParameter2(VO_HANDLE hCodec, VO_S32 nID, VO_PTR pValue);
VO_U32 VO_API voJPEGEncProcess2(VO_HANDLE hCodec, VO_VIDEO_BUFFER *pInData, VO_CODECBUFFER *pOutData,VO_VIDEO_OUTPUTINFO *pOutFormat);

INT32 init_exif(EXIFLibrary* exiflib)
{
	exiflib->thumWidth  = DEFAULT_THUMWIDTH;
	exiflib->thumHeight = DEFAULT_THUMHEIGHT;
	exiflib->enTIFFD	= TIFFDIRENALBE;
	exiflib->enEXIFDir  = EXIFDIRENABLE;
	exiflib->enThumbEXIFDir  = THUMDIRENABLE;
	exiflib->Tiffdir.XResolution[0] = exiflib->Tiffdir.YResolution[0] = DEFAULT_RESOLUTION;
	exiflib->Tiffdir.XResolution[1] = exiflib->Tiffdir.YResolution[1] = 1;
	exiflib->Tiffdir.ResolutionUnit = RESUNIT_INCH;
	exiflib->Tiffdir.Orientation = ORIENTATION_TOPLEFT;
	exiflib->Tiffdir.YCbCrPositioning = YCBCRPOSITION_COSITED;
	exiflib->thumbdir.XResolution[0] = exiflib->thumbdir.YResolution[0] = DEFAULT_RESOLUTION;
	exiflib->thumbdir.XResolution[1] = exiflib->thumbdir.YResolution[1] = 1;
	exiflib->thumbdir.ResolutionUnit = RESUNIT_INCH;
	exiflib->thumbdir.Compression = COMPRESSION_OJPEG;
	exiflib->thumbdir.YCbCrPositioning = YCBCRPOSITION_COSITED;
	exiflib->Exifdir.ComponentsConfiguration = DEFAULT_COMCONFIG;
	exiflib->Exifdir.Colorspace = 1;
	exiflib->Exifdir.ExifVersion = EXIF_VERSION;
	exiflib->Exifdir.FlashpixVersion = FlashPIX_VERSION;	
	memcpy(exiflib->Tiffdir.Software, "VISUALON V3.0", 15);
	
	exiflib->Gpsdir.GPSVersionIF[0] = exiflib->Gpsdir.GPSVersionIF[1] = 2;
	exiflib->Gpsdir.GPSVersionIF[2] = exiflib->Gpsdir.GPSVersionIF[3] = 0;
	exiflib->Gpsdir.GPSLatitudeRef = VO_NORTH_LATITITUDE;//NORTH_LATITITUDE;
	exiflib->Gpsdir.GPSLongtiudeRef = VO_EAST_LONGITUDE;//EAST_LONGITUDE;
	exiflib->Gpsdir.GPSAltitudeRef = SEA_LEVEL;
	
	return 0;
}

static INT32 Init_ThumbBuffer(VO_VIDEO_BUFFER *pthumbInData, VO_CODECBUFFER  *pthumbOutData, UINT32 width, UINT32 Height)
{
	UINT32 Hstride;
	UINT32 dataSize;	

	//Hstride = (Height + 1) & ~1;
	Hstride = (Height + 31) & ~31; //bugfix zou 318
	dataSize = Hstride * ((width+31)&~31);
	if(pthumbOutData->Buffer)
		mem_free(pthumbOutData->Buffer);
	pthumbOutData->Buffer = (UINT8 *)mem_malloc(dataSize >> 1, DCACHE_ALIGN_NUM);
	if(pthumbOutData->Buffer == NULL)
		return -1;
	memset(pthumbOutData->Buffer,0,dataSize >> 1);
	pthumbOutData->Length = dataSize >> 1;
	
	switch(pthumbInData->ColorType)
	{
	case VO_COLOR_YUV_PLANAR444://VOYUV_PLANAR444:
	case VO_COLOR_RGB888_PLANAR://VORGB888_PLANAR:
		pthumbInData->Stride[0] = pthumbInData->Stride[1] = pthumbInData->Stride[2] = (width + 15) & ~15;
		dataSize = pthumbInData->Stride[0] * Hstride;
		if(pthumbInData->Buffer[0])
			mem_free(pthumbInData->Buffer[0]);
		pthumbInData->Buffer[0] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		if(pthumbInData->Buffer[0] == NULL)	
			return -1;
		if(pthumbInData->Buffer[1])
			mem_free(pthumbInData->Buffer[1]);
		pthumbInData->Buffer[1] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		if(pthumbInData->Buffer[1] == NULL)	
			return -1;
		if(pthumbInData->Buffer[2])
			mem_free(pthumbInData->Buffer[2]);
		pthumbInData->Buffer[2] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		if(pthumbInData->Buffer[2] == NULL)	
			return -1;
		break;
	case VO_COLOR_RGB888_PACKED://VORGB888_PACKED:
	case VO_COLOR_YUV444_PACKED://VOYUV444_PACKED:
		pthumbInData->Stride[0] = (width * 3 + 15) & ~15;
		dataSize = pthumbInData->Stride[0] * Hstride;
		if(pthumbInData->Buffer[0])
			mem_free(pthumbInData->Buffer[0]);
		pthumbInData->Buffer[0] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		if(pthumbInData->Buffer[0] == NULL)	
			return -1;		
		break;
	case VO_COLOR_YVU_PLANAR422_12://VOYUV_PLANAR422_12:
		pthumbInData->Stride[0] = (width + 15) & ~15;
		pthumbInData->Stride[1] = pthumbInData->Stride[2] = pthumbInData->Stride[0] >> 1;
		dataSize = pthumbInData->Stride[0] * Hstride;
		if(pthumbInData->Buffer[0])
			mem_free(pthumbInData->Buffer[0]);
		pthumbInData->Buffer[0] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		if(pthumbInData->Buffer[0] == NULL)	
			return -1;
		if(pthumbInData->Buffer[1])
			mem_free(pthumbInData->Buffer[1]);
		dataSize >>= 1;
		//dataSize = pthumbInData->Stride[1]*Hstride;
		pthumbInData->Buffer[1] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		if(pthumbInData->Buffer[1] == NULL)	
			return -1;
		if(pthumbInData->Buffer[2])
			mem_free(pthumbInData->Buffer[2]);
		pthumbInData->Buffer[2] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		if(pthumbInData->Buffer[2] == NULL)	
			return -1;
		break;
	case VO_COLOR_YUYV422_PACKED://VOYUYV422_PACKED:
	case VO_COLOR_YVYU422_PACKED://VOYVYU422_PACKED:
	case VO_COLOR_UYVY422_PACKED://VOUYVY422_PACKED:
	case VO_COLOR_VYUY422_PACKED://VOVYUY422_PACKED:
	case VO_COLOR_RGB565_PACKED://VORGB565_PACKED:
		pthumbInData->Stride[0] = ((width << 1) + 15) & ~15;
		dataSize = pthumbInData->Stride[0] * Hstride;
		if(pthumbInData->Buffer[0])
			mem_free(pthumbInData->Buffer[0]);
		pthumbInData->Buffer[0] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		if(pthumbInData->Buffer[0] == NULL)	
			return -1;
		break;	
	case VO_COLOR_YUV_PLANAR420://VOYUV_PLANAR420:
	case VO_COLOR_YVU_PLANAR420://VOYVU_PLANAR420:
		pthumbInData->Stride[0] = (width + 31) & ~31;
		pthumbInData->Stride[1] = pthumbInData->Stride[2] = pthumbInData->Stride[0] >> 1;
		dataSize = pthumbInData->Stride[0] * Hstride;
		if(pthumbInData->Buffer[0])
			mem_free(pthumbInData->Buffer[0]);
		pthumbInData->Buffer[0] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		memset(pthumbInData->Buffer[0],0,dataSize);
		if(pthumbInData->Buffer[0] == NULL)	
			return -1;
		dataSize >>= 2;
		if(pthumbInData->Buffer[1])
			mem_free(pthumbInData->Buffer[1]);
		pthumbInData->Buffer[1] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		memset(pthumbInData->Buffer[0],0,dataSize);
		if(pthumbInData->Buffer[1] == NULL)	
			return -1;
		if(pthumbInData->Buffer[2])
			mem_free(pthumbInData->Buffer[2]);
		pthumbInData->Buffer[2] = (UINT8 *)mem_malloc(dataSize, DCACHE_ALIGN_NUM);
		memset(pthumbInData->Buffer[0],0,dataSize);
		if(pthumbInData->Buffer[2] == NULL)	
			return -1;
		break;
	default:
	  	break;
	}
	
	return 0;
}

//VOCODECVIDEOBUFFER --> VO_VIDEO_BUFFER

//VOCODECDATABUFFER  --> VO_CODECBUFFER

INT32	jpeg_encthumbnail(JPEG_ENCOBJ* jpeg_obj, VO_VIDEO_BUFFER *pInData)
{
	EXIFLibrary *exiflib;
	PICTURE_ATT*		picture;
	VO_VIDEO_BUFFER*	pThumbInData;
	VO_CODECBUFFER*	pThumbOutData;
	VO_VIDEO_OUTPUTINFO  outinfo;

	exiflib = jpeg_obj->jExifLibary;
	picture = jpeg_obj->picture;
	pThumbInData  = &jpeg_obj->ThumbInData;
	pThumbOutData = &jpeg_obj->ThumbOutData;

	jpeg_obj->ThumbInData.ColorType = pInData->ColorType;
	if(Init_ThumbBuffer(pThumbInData, pThumbOutData, exiflib->thumWidth, exiflib->thumHeight ) < 0)
		return -1;

	(*jpeg_obj->jpeg_resize)(pInData, pThumbInData, picture->width, picture->heigth, 
		//(exiflib->thumWidth+31)&~31, (exiflib->thumHeight+31)&~31); //zou 319
		exiflib->thumWidth, exiflib->thumHeight);

	//memset(pThumbInData->Buffer[0]+pThumbInData->Stride[0]*60,0,8*pThumbInData->Stride[0]);
	//memset(pThumbInData->Buffer[1]+pThumbInData->Stride[1]*60,0,4*pThumbInData->Stride[1]);
	//memset(pThumbInData->Buffer[2]+pThumbInData->Stride[2]*60,0,4*pThumbInData->Stride[2]);
	{
		FILE* fp = fopen("E:\\420.yuv","wb");
		fwrite(pThumbInData->Buffer[0],1,pThumbInData->Stride[0]*128,fp);
		fwrite(pThumbInData->Buffer[1],1,pThumbInData->Stride[1]*64,fp);
		fwrite(pThumbInData->Buffer[2],1,pThumbInData->Stride[2]*64,fp);
		fclose(fp);
	}
	exiflib->thumWidth = 160;
	exiflib->thumHeight = 128;
	voJPEGEncSetParameter2(jpeg_obj->hThumbHand, VO_PID_JPEG_WIDTH, &exiflib->thumWidth);
	voJPEGEncSetParameter2(jpeg_obj->hThumbHand, VO_PID_JPEG_HEIGHT, &exiflib->thumHeight);

	if(voJPEGEncProcess2(jpeg_obj->hThumbHand, pThumbInData, pThumbOutData, &outinfo) != VO_ERR_NONE)
		return -1;

	return 0;
}

static INT32 write_rational(JPEG_STREAM* bitstream, INT32* rational, UINT32 offset)	
{
	UINT8*	nextbyte_write;

	nextbyte_write = bitstream->next_output_byte + offset;
	
	*nextbyte_write++ = (UINT8)(rational[0] & 0xFF);
	*nextbyte_write++ = (UINT8)((rational[0] >> 8 ) & 0xFF);
	*nextbyte_write++ = (UINT8)((rational[0] >> 16) & 0xFF);
	*nextbyte_write++ = (UINT8)((rational[0] >> 24) & 0xFF);
	
	*nextbyte_write++ = (UINT8)(rational[1] & 0xFF);
	*nextbyte_write++ = (UINT8)((rational[1] >> 8 ) & 0xFF);
	*nextbyte_write++ = (UINT8)((rational[1] >> 16) & 0xFF);
	*nextbyte_write++ = (UINT8)((rational[1] >> 24) & 0xFF);

	return 0;
}

static INT32 write_string(JPEG_STREAM* bitstream, UINT8* str, UINT32 length, UINT32 offset)	
{
	UINT32	i;
	UINT8*	nextbyte_write;

	nextbyte_write = bitstream->next_output_byte + offset;

	for(i = 0; i < length; i++)
	{
		*nextbyte_write++ = *str++;
	}

	return 0;
}

static INT32 write_idfentry(JPEG_STREAM* bitstream, IDFEntry* idfen)	
{
	put2byte(bitstream, idfen->idf_tag,   1);
	put2byte(bitstream, idfen->idf_type,  1);
	put4byte(bitstream, idfen->idf_count, 1);

	switch(idfen->idf_type)
	{
	case IDF_SHORT:
	case IDF_SSHORT:
		put2byte(bitstream, idfen->idf_offset,  1);
		put2byte(bitstream, 0,	1);
		break;
	default:
		put4byte(bitstream, idfen->idf_offset, 1);			
		break;
	}

	return 0;
}

static INT32 write_tiff_idf( EXIFLibrary*	exifLib, JPEG_STREAM*	bitstream, UINT32* length, UINT32 writeflag)
{
	IDFEntry	idfen;
	TIFFDirectory*	Tiffdir;
	UINT32	strCount;	
	UINT32	numTiff;
	UINT32	enTiff;
	UINT32	total;
	UINT32	offset;
	UINT32	M4len;
	UINT32	i;

	Tiffdir = &exifLib->Tiffdir;
	numTiff = exifLib->NumberTIFFD;
	enTiff	= exifLib->enTIFFD;	
	total = M4len = 0;
	offset = *length + 12 * numTiff + 6;

	*length += 2;
	if(writeflag)
	{
		put2byte(bitstream, numTiff, 1);
	}

	if(enTiff & 0x00000001)   // Camera make
	{
		*length += 12;
		strCount = strlen((INT8*)Tiffdir->CameraMake) + 1;
		if(strCount <= 4)
		{
			if(writeflag)
			{
				idfen.idf_tag   = TIFFTAG_MODEL;
				idfen.idf_type  = IDF_ASCII;
				idfen.idf_count = strCount;

				idfen.idf_offset = 0;
				for(i = 0; i < strCount; i++)
				{
					idfen.idf_offset |= (Tiffdir->CameraMake[i] << (i*8));
				}
			
				write_idfentry(bitstream, &idfen);
			}
		}
		else
		{
			if(writeflag)
			{
				idfen.idf_tag   = TIFFTAG_MAKE;
				idfen.idf_type  = IDF_ASCII;
				idfen.idf_count = strCount;
				idfen.idf_offset= offset - 8;
				
				write_idfentry(bitstream, &idfen);
				
				write_string(bitstream, Tiffdir->CameraMake, strCount, offset - (*length));			
			}
			
			offset += strCount;
			M4len  += strCount;		
		}
	}

	if(enTiff & 0x00000002)  // Camera modal
	{
		*length += 12;
		strCount = strlen((INT8*)Tiffdir->CameraModel) + 1;
		if(strCount <= 4)
		{
			if(writeflag)
			{
				idfen.idf_tag   = TIFFTAG_MODEL;
				idfen.idf_type  = IDF_ASCII;
				idfen.idf_count = strCount;
				
				idfen.idf_offset = 0;
				for(i = 0; i < strCount; i++)
				{
					idfen.idf_offset |= (Tiffdir->CameraModel[i] << (i*8));
				}

				write_idfentry(bitstream, &idfen);
			}
		}
		else
		{
			if(writeflag)
			{
				idfen.idf_tag   = TIFFTAG_MODEL;
				idfen.idf_type  = IDF_ASCII;
				idfen.idf_count = strCount;
				idfen.idf_offset= offset - 8;
				
				write_idfentry(bitstream, &idfen);
				
				write_string(bitstream, Tiffdir->CameraModel, strCount, offset - (*length));			
			}
			
			offset += strCount;
			M4len  += strCount;		
		}
	}

	if(enTiff & 0x00000004)  // photo date and time
	{
		*length += 12;
		strCount = 20;

		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_DATETIME;
			idfen.idf_type  = IDF_ASCII;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);

			write_string(bitstream, Tiffdir->DateTime, strCount, offset - (*length));			
		}

		offset += strCount;
		M4len  += strCount;		
	}

	if(enTiff & 0x00000008)  // Software
	{
		*length += 12;
		strCount = strlen((INT8*)Tiffdir->Software) + 1;

		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_SOFTWARE;
			idfen.idf_type  = IDF_ASCII;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);

			write_string(bitstream, Tiffdir->Software, strCount, offset - (*length));			
		}

		offset += strCount;
		M4len  += strCount;		
	}

	if(enTiff & 0x00000800)  //author
	{
		*length += 12;
		strCount = strlen((INT8*)Tiffdir->Author) + 1;
		if(strCount <= 4)
		{
			if(writeflag)
			{
				idfen.idf_tag   = TIFFTAG_WINAUTHOR;
				idfen.idf_type  = IDF_ASCII;
				idfen.idf_count = strCount;

				idfen.idf_offset = 0;
				for(i = 0; i < strCount; i++)
				{
					idfen.idf_offset |= (Tiffdir->Author[i] << (i*8));
				}
			
				write_idfentry(bitstream, &idfen);
			}
		}
		else
		{
			if(writeflag)
			{
				idfen.idf_tag   = TIFFTAG_WINAUTHOR;
				idfen.idf_type  = IDF_ASCII;
				idfen.idf_count = strCount;
				idfen.idf_offset= offset - 8;
				
				write_idfentry(bitstream, &idfen);
				
				write_string(bitstream, Tiffdir->Author, strCount, offset - (*length));			
			}
			
			offset += strCount;
			M4len  += strCount;		
		}
	}

	if(enTiff & 0x00000010)	//orientation
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_ORIENTATION;
			idfen.idf_type  = IDF_SHORT;
			idfen.idf_count = 1;
			idfen.idf_offset= Tiffdir->Orientation;
			
			write_idfentry(bitstream, &idfen);			
		}
	}

	if(enTiff & 0x00000020)
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_XRESOLUTION;
			idfen.idf_type  = IDF_RATIONAL;
			idfen.idf_count = 1;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);
			
			write_rational(bitstream, (INT32*)Tiffdir->XResolution, offset - (*length));			
		}

		offset += 8;
		M4len  += 8;	
	}
	
	if(enTiff & 0x00000040)
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_YRESOLUTION;
			idfen.idf_type  = IDF_RATIONAL;
			idfen.idf_count = 1;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);
			
			write_rational(bitstream, (INT32*)Tiffdir->YResolution, offset - (*length));			
		}

		offset += 8;
		M4len  += 8;	
	}
	
	if(enTiff & 0x00000080)
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_RESOLUTIONUNIT;
			idfen.idf_type  = IDF_SHORT;
			idfen.idf_count = 1;
			idfen.idf_offset= Tiffdir->ResolutionUnit;
			
			write_idfentry(bitstream, &idfen);			
		}
	}

	if(enTiff & 0x00000100)
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_YCBCRPOSITIONING;
			idfen.idf_type  = IDF_SHORT;
			idfen.idf_count = 1;
			idfen.idf_offset= Tiffdir->YCbCrPositioning;
			
			write_idfentry(bitstream, &idfen);				
		}
	}
	
	if(enTiff & 0x00000200)
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_EXIFIFD;
			idfen.idf_type  = IDF_LONG;
			idfen.idf_count = 1;
			idfen.idf_offset= Tiffdir->exifIFDPointer;
			
			write_idfentry(bitstream, &idfen);				
		}
	}

	if(enTiff & 0x00000400)
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_GPSIFD;
			idfen.idf_type  = IDF_LONG;
			idfen.idf_count = 1;
			idfen.idf_offset= Tiffdir->GPSIFDPointer;
			
			write_idfentry(bitstream, &idfen);				
		}
	}

	*length += 4;
	if(writeflag)
	{
		put4byte(bitstream, exifLib->nextIDFP, 1);
	}

	*length += M4len;
	if(writeflag)
	{
		bitstream->next_output_byte += M4len;
		bitstream->free_in_buffer   -= M4len;
	}

	return 0;
}

static INT32 write_exif_idf(EXIFLibrary* exifLib, JPEG_STREAM*	bitstream, UINT32* length, UINT32 writeflag)
{
	IDFEntry	idfen;
	EXIFDirectory*	Exifdir;
	UINT32	strCount;	
	UINT32	numExif;
	UINT32	enExif;
	UINT32	total;
	UINT32	offset;
	UINT32	M4len;

	Exifdir = &exifLib->Exifdir;
	numExif = exifLib->NumberEXIFDir;
	enExif	= exifLib->enEXIFDir;	
	total = M4len = 0;
	offset = *length + 12 * numExif + 6;

	*length += 2;
	if(writeflag)
	{
		put2byte(bitstream, numExif, 1);
	}

	if(enExif & 0x00000004)
	{
		*length += 12;
		strCount = 4;

		if(writeflag)
		{
			idfen.idf_tag  = EXIFTAG_EXIFVERSION;
			idfen.idf_type = IDF_UNDEFINED;
			idfen.idf_count = strCount;
			idfen.idf_offset = Exifdir->ExifVersion;

			write_idfentry(bitstream, &idfen);
		}
	}

	if(enExif & 0x00000020)
	{
		*length += 12;

		if(writeflag)
		{
			idfen.idf_tag  = EXIFTAG_COMPONENTSCONFIGURATION;
			idfen.idf_type = IDF_UNDEFINED;
			idfen.idf_count = 4;
			idfen.idf_offset = Exifdir->ComponentsConfiguration;
			
			write_idfentry(bitstream, &idfen);
		}		
	}

	if(enExif & 0x00100000)
	{
		*length += 12;

		if(writeflag)
		{
			idfen.idf_tag  = EXIFTAG_FLASHPIXVERSION;
			idfen.idf_type = IDF_UNDEFINED;
			idfen.idf_count = 4;
			idfen.idf_offset = Exifdir->FlashpixVersion;
			
			write_idfentry(bitstream, &idfen);
		}		
	}
	
	if(enExif & 0x00200000)
	{
		*length += 12;

		if(writeflag)
		{
			idfen.idf_tag  = EXIFTAG_COLORSPACE;
			idfen.idf_type = IDF_SHORT;
			idfen.idf_count = 1;
			idfen.idf_offset = Exifdir->Colorspace;
			
			write_idfentry(bitstream, &idfen);
		}		
	}
	
	if(enExif & 0x00000008)  // photo date and time
	{
		*length += 12;
		strCount = 20;

		if(writeflag)
		{
			idfen.idf_tag   = EXIFTAG_DATETIMEORIGINAL;
			idfen.idf_type  = IDF_ASCII;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);

			write_string(bitstream, Exifdir->DataTimeOriginal, strCount, offset - (*length));			
		}

		offset += strCount;
		M4len  += strCount;		
	}	

	if(enExif & 0x00000010)  // photo date and time
	{
		*length += 12;
		strCount = 20;

		if(writeflag)
		{
			idfen.idf_tag   = EXIFTAG_DATETIMEDIGITIZED;
			idfen.idf_type  = IDF_ASCII;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);

			write_string(bitstream, Exifdir->DataTimeDigitized, strCount, offset - (*length));			
		}

		offset += strCount;
		M4len  += strCount;		
	}	

	if(enExif & 0x00400000)
	{
		*length += 12;

		if(writeflag)
		{
			idfen.idf_tag  = EXIFTAG_PIXELXDIMENSION;
			idfen.idf_type = IDF_LONG;
			idfen.idf_count = 1;
			idfen.idf_offset = Exifdir->PixelXDimension;
						
			write_idfentry(bitstream, &idfen);
		}		
	}
	
	if(enExif & 0x00800000)
	{
		*length += 12;

		if(writeflag)
		{
			idfen.idf_tag  = EXIFTAG_PIXELYDIMENSION;
			idfen.idf_type = IDF_LONG;
			idfen.idf_count = 1;
			idfen.idf_offset = Exifdir->PixelYDimension;
			
			write_idfentry(bitstream, &idfen);
		}		
	}

	*length += 4;
	if(writeflag)
	{
		put4byte(bitstream, 0, 1);
	}

	*length += M4len;
	if(writeflag)
	{
		bitstream->next_output_byte += M4len;
		bitstream->free_in_buffer   -= M4len;
	}

	return 0;
}

static INT32 write_thumb_idf(EXIFLibrary* exifLib, JPEG_STREAM*	bitstream, UINT32* length,  UINT32 writeflag)
{
	IDFEntry	idfen;
	ThumbnailEXIFDirectory*	Thumbdir;
	UINT32	strCount;	
	UINT32	numThumb;
	UINT32	enThumb;
	UINT32	total;
	UINT32	offset;
	UINT32	M4len;

	Thumbdir = &exifLib->thumbdir;
	numThumb = exifLib->NumberThumbEXIFDir;
	enThumb	= exifLib->enThumbEXIFDir;	
	total = M4len = 0;
	offset = *length + 12 * numThumb + 6;
	
	*length += 2;
	if(writeflag)
	{
		put2byte(bitstream, numThumb, 1);
	}
	
	if(enThumb & 0x00000001)  
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_COMPRESSION;
			idfen.idf_type  = IDF_SHORT;
			idfen.idf_count = 1;
			idfen.idf_offset= Thumbdir->Compression;
			
			write_idfentry(bitstream, &idfen);
		}		
	}

	if(enThumb & 0x00000002)
	{
		*length += 12;
		strCount = 1;

		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_XRESOLUTION;
			idfen.idf_type  = IDF_RATIONAL;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);

			write_rational(bitstream, (INT32*)Thumbdir->XResolution, offset - (*length));			
		}

		offset += 8;
		M4len  += 8;		
	}

	if(enThumb & 0x00000004)
	{
		*length += 12;

		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_YRESOLUTION;
			idfen.idf_type  = IDF_RATIONAL;
			idfen.idf_count = 1;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);

			write_rational(bitstream, (INT32*)Thumbdir->YResolution, offset - (*length));			
		}

		offset += 8;
		M4len  += 8;		
	}

	if(enThumb & 0x00000008)  
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_RESOLUTIONUNIT;
			idfen.idf_type  = IDF_SHORT;
			idfen.idf_count = 1;
			idfen.idf_offset= Thumbdir->ResolutionUnit;
			
			write_idfentry(bitstream, &idfen);
		}		
	}

	if(enThumb & 0x00000010)  
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_JPEGIFOFFSET;
			idfen.idf_type  = IDF_LONG;
			idfen.idf_count = 1;
			idfen.idf_offset= Thumbdir->JPEGOffSet;
			
			write_idfentry(bitstream, &idfen);
		}		
	}

	if(enThumb & 0x00000020)  
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_JPEGIFBYTECOUNT;
			idfen.idf_type  = IDF_LONG;
			idfen.idf_count = 1;
			idfen.idf_offset= Thumbdir->JPEGByteCount;
			
			write_idfentry(bitstream, &idfen);
		}		
	}

	if(enThumb & 0x00000040)  
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = TIFFTAG_YCBCRPOSITIONING;
			idfen.idf_type  = IDF_SHORT;
			idfen.idf_count = 1;
			idfen.idf_offset= Thumbdir->YCbCrPositioning;
			
			write_idfentry(bitstream, &idfen);
		}		
	}

	*length += 4;
	if(writeflag)
	{
		put4byte(bitstream, 0, 1);
	}

	*length += M4len;
	if(writeflag)
	{
		bitstream->next_output_byte += M4len;
		bitstream->free_in_buffer   -= M4len;
	}	
	
	return 0;
}

static INT32 write_GPS_idf(EXIFLibrary* exifLib, JPEG_STREAM*	bitstream, UINT32* length,  UINT32 writeflag)
{
	IDFEntry	idfen;
	GPSDirectory*	GPSdir;
	UINT32	strCount;	
	UINT32	numGPS;
	UINT32	enGPS;
	UINT32	total;
	UINT32	offset;
	UINT32	M4len;

	GPSdir = &exifLib->Gpsdir;
	numGPS = exifLib->NumberGPSDir;
	enGPS	= exifLib->enGPSDir;	
	total = M4len = 0;
	offset = *length + 12 * numGPS + 6;

	*length += 2;
	if(writeflag)
	{
		put2byte(bitstream, numGPS, 1);
	}
	
	if(enGPS & 0x00000001)  
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = GPSTAG_VERSIONIF;
			idfen.idf_type  = IDF_BYTE;
			idfen.idf_count = 4;
			idfen.idf_offset = GPSdir->GPSVersionIF[0] | (GPSdir->GPSVersionIF[1] << 8)
							 | (GPSdir->GPSVersionIF[2] << 16) | (GPSdir->GPSVersionIF[3] << 24);
			
			write_idfentry(bitstream, &idfen);
		}		
	}

	if(enGPS & 0x00000002)
	{
		*length += 12;

		if(writeflag)
		{
			idfen.idf_tag   = GPSTAG_LATITUDEREF;
			idfen.idf_type  = IDF_ASCII;
			idfen.idf_count = 2;
			idfen.idf_offset= GPSdir->GPSLatitudeRef;
			
			write_idfentry(bitstream, &idfen);		
		}	
	}

	if(enGPS & 0x00000004)
	{
		*length += 12;
		strCount = 3;

		if(writeflag)
		{
			idfen.idf_tag   = GPSTAG_LATITUDE;
			idfen.idf_type  = IDF_RATIONAL;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);

			write_rational(bitstream, (INT32*)&GPSdir->GPSLatitude[0], offset - (*length));			
			write_rational(bitstream, (INT32*)&GPSdir->GPSLatitude[2], offset - (*length) + 8);			
			write_rational(bitstream, (INT32*)&GPSdir->GPSLatitude[4], offset - (*length) + 16);			
		}

		offset += 24;
		M4len  += 24;		
	}
	
	if(enGPS & 0x00000008)
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = GPSTAG_LONGITUDEREF;
			idfen.idf_type  = IDF_ASCII;
			idfen.idf_count = 2;
			idfen.idf_offset= GPSdir->GPSLongtiudeRef;
			
			write_idfentry(bitstream, &idfen);		
		}	
	}

	if(enGPS & 0x00000010)
	{
		*length += 12;
		strCount = 3;

		if(writeflag)
		{
			idfen.idf_tag   = GPSTAG_LONGITUDE;
			idfen.idf_type  = IDF_RATIONAL;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);

			write_rational(bitstream, (INT32*)&GPSdir->GPSLongtiude[0], offset - (*length));			
			write_rational(bitstream, (INT32*)&GPSdir->GPSLongtiude[2], offset - (*length) + 8);			
			write_rational(bitstream, (INT32*)&GPSdir->GPSLongtiude[4], offset - (*length) + 16);			
		}

		offset += 24;
		M4len  += 24;		
	}
	
	if(enGPS & 0x00000020)
	{
		*length += 12;
		if(writeflag)
		{
			idfen.idf_tag   = GPSTAG_ALITUDEREF;
			idfen.idf_type  = IDF_BYTE;
			idfen.idf_count = 1;
			idfen.idf_offset= GPSdir->GPSAltitudeRef;
			
			write_idfentry(bitstream, &idfen);		
		}	
	}

	if(enGPS & 0x00000040)
	{
		*length += 12;
		strCount = 1;

		if(writeflag)
		{
			idfen.idf_tag   = GPSTAG_ALITUDE;
			idfen.idf_type  = IDF_RATIONAL;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);
			write_rational(bitstream, (INT32*)&GPSdir->GPSAltitude[0], offset - (*length));			
		}

		offset += 8;
		M4len  += 8;		
	}

	if(enGPS & 0x00040000)
	{
		*length += 12;
		strCount = strlen((INT8*)GPSdir->GPSMapDatum) + 1;

		if(writeflag)
		{
			idfen.idf_tag   = GPSTAG_MAPDATUM;
			idfen.idf_type  = IDF_ASCII;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);

			write_string(bitstream, GPSdir->GPSMapDatum,strCount, offset - (*length));		
		}

		offset += strCount;
		M4len  += strCount;		
	}

	if(enGPS & 0x00080000)
	{
		*length += 12;
		strCount = 11;

		if(writeflag)
		{
			idfen.idf_tag   = GPSTAG_DATESTAMP;
			idfen.idf_type  = IDF_ASCII;
			idfen.idf_count = strCount;
			idfen.idf_offset= offset - 8;
			
			write_idfentry(bitstream, &idfen);
			write_string(bitstream, GPSdir->GPSDatastemp,strCount, offset - (*length));
		}

		offset += strCount;
		M4len  += strCount;			
	}
	
	*length += 4;
	if(writeflag)
	{
		put4byte(bitstream, 0, 1);
	}

	*length += M4len;
	if(writeflag)
	{
		bitstream->next_output_byte += M4len;
		bitstream->free_in_buffer   -= M4len;
	}	

	return 0;
}

static INT32 write_thumbnail(JPEG_ENCOBJ* jpeg_obj, UINT32* length, UINT32 writeflag)
{
	EXIFLibrary*	exifLib;
	JPEG_STREAM*	bitstream; 
	VO_CODECBUFFER* JpegBuffer;
	
	exifLib = jpeg_obj->jExifLibary;
	bitstream = jpeg_obj->J_stream;
	JpegBuffer= &jpeg_obj->ThumbOutData;
	
	exifLib->thumbdir.JPEGByteCount = JpegBuffer->Length;

	*length += JpegBuffer->Length;
	if(writeflag)
	{
		memcpy(bitstream->next_output_byte, JpegBuffer->Buffer, JpegBuffer->Length);
		bitstream->next_output_byte += JpegBuffer->Length;
		bitstream->free_in_buffer   -= JpegBuffer->Length;
	}	

	return 0;
}

static INT32 write_exif(JPEG_ENCOBJ* jpeg_obj, UINT32 writeflag)
{
	UINT32 len;
	JPEG_STREAM*	bitstream;
	EXIFLibrary*	exifLib;

	len = 0;
	bitstream = jpeg_obj->J_stream;
	exifLib	  = jpeg_obj->jExifLibary;
	
	len += 8;
	if(writeflag)
	{
		put2byte(bitstream, exifLib->exiflength, 0);
		putbyte(bitstream, 'E');
		putbyte(bitstream, 'x');
		putbyte(bitstream, 'i');
		putbyte(bitstream, 'f');
		put2byte(bitstream, 0, 0);
	}

	len += 8;
	if(writeflag)
	{
		put2byte(bitstream, 0x4949, 1);
		put2byte(bitstream, 0x002A, 1);
		put4byte(bitstream, 8,	1);
	}
	
	write_tiff_idf(exifLib, bitstream, &len, writeflag);
	
	exifLib->Tiffdir.exifIFDPointer = len - 8;	
	write_exif_idf(exifLib, bitstream, &len, writeflag);

	if(exifLib->enGPSDir)
	{
		exifLib->Tiffdir.GPSIFDPointer = len - 8;	
		write_GPS_idf(exifLib, bitstream, &len, writeflag);
	}
	
	if(jpeg_obj->thumb_support)
	{
		exifLib->nextIDFP = len - 8;
		write_thumb_idf(exifLib, bitstream, &len, writeflag);
		
		exifLib->thumbdir.JPEGOffSet = len - 8;
		write_thumbnail(jpeg_obj, &len, writeflag);
	}

	exifLib->exiflength = len + 2;

	return 0;
}



INT32 write_jpeg_exif(JPEG_ENCOBJ* jpeg_obj)
{
	UINT32 i;
	EXIFLibrary*	exifLib;
	JPEG_STREAM*	bitstream;
	
	bitstream = jpeg_obj->J_stream;
	exifLib	  = jpeg_obj->jExifLibary;
	
	exifLib->NumberTIFFD = 0;
	exifLib->NumberEXIFDir = 0;
	exifLib->NumberGPSDir  = 0;
	exifLib->NumberThumbEXIFDir = 0;
	for(i = 0; i < 32; i++)
	{
		exifLib->NumberTIFFD		+= (exifLib->enTIFFD >> i) & 1;
		exifLib->NumberEXIFDir		+= (exifLib->enEXIFDir >> i) & 1;
		exifLib->NumberGPSDir		+= (exifLib->enGPSDir >> i) & 1;
		exifLib->NumberThumbEXIFDir += (exifLib->enThumbEXIFDir >> i) & 1;
	}

	exifLib->Exifdir.PixelXDimension = jpeg_obj->picture->width;
	exifLib->Exifdir.PixelYDimension = jpeg_obj->picture->heigth;

	putmarker(bitstream, M_APP1);

	write_exif(jpeg_obj, 0);

	if(exifLib->exiflength < 0xFFFF)
	{
		write_exif(jpeg_obj, 1);
	}

	return 0;
}