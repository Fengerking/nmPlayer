#include <time.h>
#include "jexif.h"
#include "jcommon.h"
#include "jdecoder.h"
#include "mem_align.h"

const UINT32 BytesPerFormat[] = {0,1,1,2,4,8,1,1,2,4,8,4,8,4};

static INT32 GetIDFValue(JPEG_STREAM*	bitstream, IDFEntry* idfen, INT32 bigend)
{
	UINT32		byte;
	
	switch(idfen->idf_type)
	{
	case IDF_BYTE:
	case IDF_SBYTE:
		getbyte(bitstream, &idfen->idf_offset, bigend);
		getbyte(bitstream, &byte, bigend);
		get2byte(bitstream, &byte, bigend);
		break;
	case IDF_ASCII:
	case IDF_UNDEFINED:
		get4byte(bitstream, &idfen->idf_offset, bigend);
		break;
	case IDF_SHORT:
	case IDF_SSHORT:
		get2byte(bitstream, &idfen->idf_offset, bigend);
		get2byte(bitstream, &byte, bigend);
		break;
	case IDF_LONG:
	case IDF_SLONG:
	case IDF_IFD:
		get4byte(bitstream, &idfen->idf_offset, bigend);	
		break;
	default:
		get4byte(bitstream, &idfen->idf_offset, bigend);	
		break;
	}

	return 0;
}

static UINT32 Get32s(UINT8* ValuePtr, UINT32 bigend)
{
    if (bigend)
	{
        return  ((( INT8 *)ValuePtr)[0] << 24) | (((UINT8 *)ValuePtr)[1] << 16)
              | (((UINT8 *)ValuePtr)[2] << 8 ) | (((UINT8 *)ValuePtr)[3] << 0 );
    }
	else
	{
        return  ((( INT8 *)ValuePtr)[3] << 24) | (((UINT8 *)ValuePtr)[2] << 16)
              | (((UINT8 *)ValuePtr)[1] << 8 ) | (((UINT8 *)ValuePtr)[0] << 0 );
    }
}

static INT32 processExif(JPEG_EXIF	*jexif, UINT32 *startoff)
{
	JPEG_STREAM*	bitstream;
	EXIFLibrary*	exiflib;
	EXIFDirectory*	Exifdir;
	IDFEntry		idfen;
	UINT32			numidf;
	UINT32			byte;
	UINT32			ByteCount;
	UINT8*			ValuePtr;
	UINT32			index;
	UINT32			bigend;

	bitstream = &jexif->exifbitstream;
	exiflib   = &jexif->exiflib;
	Exifdir	  = &exiflib->Exifdir;
	bigend	  = jexif->bigend;

	*startoff += 2;
	get2byte(bitstream, &numidf, bigend);
	
	for(index = 0; index < numidf; index++)
	{
       	*startoff += 12;
		get2byte(bitstream, &byte, bigend);
		idfen.idf_tag = (UINT16)byte;
		get2byte(bitstream, &byte, bigend);
		idfen.idf_type = (UINT16)byte;
		get4byte(bitstream, &idfen.idf_count, bigend);

		if((idfen.idf_type-1) >= 13) 
			return -1;
        		
        if((UINT32)idfen.idf_count > 0x10000)
			return -1;

		ByteCount = idfen.idf_count * BytesPerFormat[idfen.idf_type];

        if(ByteCount > 4)
		{
			get4byte(bitstream, &idfen.idf_offset, bigend);
			ValuePtr = bitstream->next_output_byte + idfen.idf_offset - *startoff;
        }
		else
		{
			ValuePtr = bitstream->next_output_byte;
			GetIDFValue(bitstream, &idfen, bigend);
		}

        switch(idfen.idf_tag)
		{
		case EXIFTAG_DATETIMEORIGINAL:
			exiflib->enEXIFDir |= 0x00000008;
			strncpy(Exifdir->DataTimeOriginal, ValuePtr, ByteCount < 19 ? ByteCount : 19);
			break;			
		case EXIFTAG_DATETIMEDIGITIZED:
			exiflib->enEXIFDir |= 0x00000010;
			strncpy(Exifdir->DataTimeDigitized , ValuePtr, ByteCount < 19 ? ByteCount : 19);
			break;			
		case EXIFTAG_PIXELXDIMENSION:
			exiflib->enEXIFDir |= 0x00400000;
			Exifdir->PixelXDimension = idfen.idf_offset;
			break;
		case EXIFTAG_PIXELYDIMENSION:
			exiflib->enEXIFDir |= 0x00800000;
			Exifdir->PixelYDimension = idfen.idf_offset;
			break;
		}		
	}
	
	*startoff += 4;
	get4byte(bitstream, &byte, bigend);

	return 0;
}

static INT32 processGPS(JPEG_EXIF	*jexif, UINT32 *startoff)
{
	JPEG_STREAM*	bitstream;
	EXIFLibrary*	exiflib;
	GPSDirectory*	GPSdir;
	IDFEntry		idfen;
	UINT32			numidf;
	UINT32			byte;
	UINT32			ByteCount;
	UINT8*			ValuePtr;
	UINT32			index;
	UINT32			bigend;
	UINT32			strlength;

	bitstream = &jexif->exifbitstream;
	exiflib   = &jexif->exiflib;
	GPSdir	  = &exiflib->Gpsdir;
	bigend	  = jexif->bigend;

	*startoff += 2;
	get2byte(bitstream, &numidf, bigend);
	
	for(index = 0; index < numidf; index++)
	{
       	*startoff += 12;
		get2byte(bitstream, &byte, bigend);
		idfen.idf_tag = (UINT16)byte;
		get2byte(bitstream, &byte, bigend);
		idfen.idf_type = (UINT16)byte;
		get4byte(bitstream, &idfen.idf_count, bigend);

		if((idfen.idf_type-1) >= 13) 
			return -1;
        		
        if((UINT32)idfen.idf_count > 0x10000)
			return -1;

		ByteCount = idfen.idf_count * BytesPerFormat[idfen.idf_type];

        if(ByteCount > 4)
		{
			get4byte(bitstream, &idfen.idf_offset, bigend);
			ValuePtr = bitstream->next_output_byte + idfen.idf_offset - *startoff;
        }
		else
		{
			ValuePtr = bitstream->next_output_byte;
			GetIDFValue(bitstream, &idfen, bigend);
		}

		switch(idfen.idf_tag)
		{
		case GPSTAG_LATITUDEREF:
			exiflib->enGPSDir |= 0x00000002;
			GPSdir->GPSLatitudeRef = idfen.idf_offset;
			break;
		case GPSTAG_LATITUDE:
			exiflib->enGPSDir |= 0x00000004;
			GPSdir->GPSLatitude[0] = Get32s(ValuePtr, bigend);
			GPSdir->GPSLatitude[1] = Get32s(ValuePtr + 4, bigend);
			GPSdir->GPSLatitude[2] = Get32s(ValuePtr + 8, bigend);
			GPSdir->GPSLatitude[3] = Get32s(ValuePtr + 12, bigend);
			GPSdir->GPSLatitude[4] = Get32s(ValuePtr + 16, bigend);
			GPSdir->GPSLatitude[5] = Get32s(ValuePtr + 20, bigend);
			break;
		case GPSTAG_LONGITUDEREF:
			exiflib->enGPSDir |= 0x00000008;
			GPSdir->GPSLongtiudeRef = idfen.idf_offset;
			break;
		case GPSTAG_LONGITUDE:
			exiflib->enGPSDir |= 0x00000010;
			GPSdir->GPSLongtiude[0] = Get32s(ValuePtr, bigend);
			GPSdir->GPSLongtiude[1] = Get32s(ValuePtr + 4, bigend);
			GPSdir->GPSLongtiude[2] = Get32s(ValuePtr + 8, bigend);
			GPSdir->GPSLongtiude[3] = Get32s(ValuePtr + 12, bigend);
			GPSdir->GPSLongtiude[4] = Get32s(ValuePtr + 16, bigend);
			GPSdir->GPSLongtiude[5] = Get32s(ValuePtr + 20, bigend);
			break;
		case GPSTAG_ALITUDEREF:
			exiflib->enGPSDir |= 0x00000020;
			GPSdir->GPSAltitudeRef = idfen.idf_offset;
			break;
		case GPSTAG_ALITUDE:
			exiflib->enGPSDir |= 0x00000040;
			GPSdir->GPSAltitude[0] = Get32s(ValuePtr, bigend);
			GPSdir->GPSAltitude[1] = Get32s(ValuePtr + 4, bigend);
			break;
		case GPSTAG_MAPDATUM:
			exiflib->enGPSDir |= 0x00040000;
			strlength = (idfen.idf_count < 20) ? idfen.idf_count : 20;
			strncpy(GPSdir->GPSMapDatum , ValuePtr, strlength);
			break;
		case GPSTAG_DATESTAMP:
			exiflib->enGPSDir |= 0x00080000;
			strncpy(GPSdir->GPSDatastemp, ValuePtr, 11);
			break;
		}		
	}
	
	*startoff += 4;
	get4byte(bitstream, &byte, bigend);

	return 0;
}


static INT32 processTiff(JPEG_EXIF	*jexif, UINT32 *startoff)
{
	JPEG_STREAM*	bitstream;
	EXIFLibrary*	exiflib;
	TIFFDirectory*	Tiffdir;
	IDFEntry		idfen;
	UINT32			numidf;
	UINT32			byte;
	UINT32			tiffoff;
	UINT32			ByteCount;
	UINT8*			ValuePtr;
	UINT32			index;
	UINT32			bigend;

	bitstream = &jexif->exifbitstream;
	exiflib   = &jexif->exiflib;
	Tiffdir	  = &exiflib->Tiffdir;
	bigend	  = jexif->bigend;

	*startoff += 2;
	get2byte(bitstream, &numidf, bigend);
	
	for(index = 0; index < numidf; index++)
	{
       	*startoff += 12;
		get2byte(bitstream, &byte, bigend);
		idfen.idf_tag = (UINT16)byte;
		get2byte(bitstream, &byte, bigend);
		idfen.idf_type = (UINT16)byte;
		get4byte(bitstream, &idfen.idf_count, bigend);

		if((idfen.idf_type-1) >= 13) 
			return -1;
        		
        if((UINT32)idfen.idf_count > 0x10000)
			return -1;

		ByteCount = idfen.idf_count * BytesPerFormat[idfen.idf_type];

        if(ByteCount > 4)
		{
			get4byte(bitstream, &idfen.idf_offset, bigend);
			ValuePtr = bitstream->next_output_byte + idfen.idf_offset - *startoff;
        }
		else
		{			
			ValuePtr = bitstream->next_output_byte;
			GetIDFValue(bitstream, &idfen, bigend);
		}

        switch(idfen.idf_tag)
		{
		case TIFFTAG_MAKE:
			exiflib->enTIFFD |= 0x00000001;
			strncpy(Tiffdir->CameraMake, ValuePtr, ByteCount < 39 ? ByteCount : 39);
			break;			
		case TIFFTAG_MODEL:
			exiflib->enTIFFD |= 0x00000002;
			strncpy(Tiffdir->CameraModel, ValuePtr, ByteCount < 39 ? ByteCount : 39);
			break;			
		case TIFFTAG_DATETIME:
			exiflib->enTIFFD |= 0x00000004;
			strncpy(Tiffdir->DateTime, ValuePtr, 19);
			break;
		case TIFFTAG_YCBCRPOSITIONING:
			exiflib->enTIFFD |= 0x00000100;
			Tiffdir->YCbCrPositioning = idfen.idf_offset;
			break;
		case TIFFTAG_ORIENTATION:
			exiflib->enTIFFD |= 0x00000010;
			Tiffdir->Orientation = idfen.idf_offset;
			break;
		case TIFFTAG_RESOLUTIONUNIT:
			exiflib->enTIFFD |= 0x00000080;
			Tiffdir->ResolutionUnit = idfen.idf_offset;
			break;
		case TIFFTAG_XRESOLUTION:
			exiflib->enTIFFD |= 0x00000020;
			Tiffdir->XResolution[0] = Get32s(ValuePtr, bigend);
			Tiffdir->XResolution[1] = Get32s(ValuePtr + 4, bigend);
			break;
		case TIFFTAG_YRESOLUTION:
			exiflib->enTIFFD |= 0x00000040;
			Tiffdir->YResolution[0] = Get32s(ValuePtr, bigend);
			Tiffdir->YResolution[1] = Get32s(ValuePtr + 4, bigend);
			break;
		case TIFFTAG_EXIFIFD:
			exiflib->enTIFFD |= 0x00000200;
			Tiffdir->exifIFDPointer = idfen.idf_offset;
			break;
		case TIFFTAG_GPSIFD:
			exiflib->enTIFFD |= 0x00000400;
			Tiffdir->GPSIFDPointer = idfen.idf_offset;
			break;
		}		
	}
	
	*startoff += 4;
	get4byte(bitstream, &tiffoff, bigend);
	jexif->offset1 = tiffoff;
	//if(jexif->offset1 == 0)
	//	return -1;

	if(Tiffdir->exifIFDPointer)
	{
		tiffoff = Tiffdir->exifIFDPointer - *startoff;
		bitstream->next_output_byte += tiffoff;
		bitstream->free_in_buffer   -= tiffoff;
		*startoff = Tiffdir->exifIFDPointer;	
		
		processExif(jexif, startoff);
	}

	if(Tiffdir->GPSIFDPointer)
	{
		tiffoff = Tiffdir->GPSIFDPointer - *startoff;
		bitstream->next_output_byte += tiffoff;
		bitstream->free_in_buffer   -= tiffoff;
		*startoff = Tiffdir->GPSIFDPointer;	
		
		processGPS(jexif, startoff);
	}

	tiffoff = jexif->offset1 - *startoff;
	bitstream->next_output_byte += tiffoff;
	bitstream->free_in_buffer   -= tiffoff;		

	*startoff = jexif->offset1;
		
	return 0;
}

static INT32 processThumb(JPEG_EXIF	*jexif, UINT32 *startoff)
{
	JPEG_STREAM*	bitstream;
	EXIFLibrary*	exiflib;
	ThumbnailEXIFDirectory* thumbdir;
	IDFEntry		idfen;
	UINT32			numidf;
	UINT32			byte;
	UINT32			ByteCount;
	UINT8*			ValuePtr;
	UINT32			index;
	UINT32			bigend;


	bitstream = &jexif->exifbitstream;
	exiflib   = &jexif->exiflib;
	thumbdir  = &exiflib->thumbdir;
	bigend	  = jexif->bigend;

	*startoff += 2;
	get2byte(bitstream, &numidf, bigend);
	
	for(index = 0; index < numidf; index++)
	{
        *startoff += 12;
		get2byte(bitstream, &byte, bigend);
		idfen.idf_tag = (UINT16)byte;
		get2byte(bitstream, &byte, bigend);
		idfen.idf_type = (UINT16)byte;
		get4byte(bitstream, &idfen.idf_count, bigend);
		if((idfen.idf_type-1) >= 13) 
			return -1;
		
        if((UINT32)idfen.idf_count > 0x10000)
			return -1;
		
		ByteCount = idfen.idf_count * BytesPerFormat[idfen.idf_type];		
        if(ByteCount > 4)
		{
			get4byte(bitstream, &idfen.idf_offset, bigend);
			ValuePtr = bitstream->next_output_byte + idfen.idf_offset - *startoff;
        }
		else
		{
			ValuePtr = bitstream->next_output_byte;
			GetIDFValue(bitstream, &idfen, bigend);
		}
		
        switch(idfen.idf_tag)
		{
		case TIFFTAG_COMPRESSION:
			thumbdir->Compression = idfen.idf_offset;
			if(idfen.idf_offset != 6)
				return -1;
			break;			
		case TIFFTAG_JPEGIFOFFSET:
			thumbdir->JPEGOffSet = idfen.idf_offset;
			break;			
		case TIFFTAG_JPEGIFBYTECOUNT:
			thumbdir->JPEGByteCount = idfen.idf_offset;			
			break;
		case TIFFTAG_RESOLUTIONUNIT:
			thumbdir->ResolutionUnit = idfen.idf_offset;
			break;
		case TIFFTAG_XRESOLUTION:
			thumbdir->XResolution[0] = Get32s(ValuePtr, bigend);
			thumbdir->XResolution[1] = Get32s(ValuePtr + 4, bigend);
			break;
		case TIFFTAG_YRESOLUTION:
			thumbdir->YResolution[0] = Get32s(ValuePtr, bigend);
			thumbdir->YResolution[1] = Get32s(ValuePtr + 4, bigend);
			break;
		}		
	}
		
	return 0;
}


INT32 init_exif(Jpeg_DecOBJ *decoder, JPEG_EXIF	*jexif)
{	
	if(decoder->memoryOperator.Set)
	{
		decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, &jexif->exiflib, 0, sizeof(EXIFLibrary));
	} 
	else
	{
		memset(&jexif->exiflib, 0, sizeof(EXIFLibrary));
	}
	
	jexif->bigend = 0;
	jexif->offset0 = 0;
	jexif->offset1 = 0;
	
	return 0;
}

INT32	decoder_exif(JPEG_EXIF	*jexif)
{
	JPEG_STREAM*	bitstream;
	EXIFLibrary*	exiflib;
	UINT32			offset;
	UINT32			byte;
	UINT32			index;	

	bitstream = &jexif->exifbitstream;
	exiflib   = &jexif->exiflib;

	get2byte(bitstream, &byte, 0);

	if(byte == 0x4949)
		jexif->bigend = 0;
	else if(byte == 0x4D4D)
		jexif->bigend = 1;
	else
		return -1;

	 get2byte(bitstream, &byte, jexif->bigend);
	 if(byte != 0x2A)
		 return -1;

	 get4byte(bitstream, &offset, jexif->bigend);
	 if(offset < 8 || offset > 16)
		 return -1;
	
	 for(index = 8; index < offset; index++)
		 getbyte(bitstream, &byte, jexif->bigend);

	 jexif->offset0 = offset;

	 if(processTiff(jexif, &offset) < 0)
		 return -1;

	 if(jexif->offset1)
	 {
		 if(processThumb(jexif, &offset) < 0)
			 return -2;		
	 }
	 else
	 {
		return -2;
	 }

	 return 0;
}
