#if !(defined(LINUX) || defined(_IOS))
#include <tchar.h>
#endif
#include	"voPNG.h"
#include "png.h"
#include "voPNGMemmory.h"

#ifndef WINCE
typedef struct tagRGBQUAD {
        VO_BYTE    rgbBlue;
        VO_BYTE    rgbGreen;
        VO_BYTE    rgbRed;
        VO_BYTE    rgbReserved;
} RGBQUAD, FAR *LPRGBQUAD;
#endif
typedef struct {
    VO_U32 origWidth;
    VO_U32 origHeight;
    VO_U32 component;
    VO_U32 bit_depth;
    VO_U32 color_type;
    VO_U32 interlace_type;
    VO_U32 hasAlpha;
    VO_U32 number_passes;
}PNGDECINFO;

static void sk_error_fn(png_structp png_ptr, png_const_charp msg)
{
    longjmp(png_jmpbuf(png_ptr), 1);
}
static int PNGAPI vo_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	FILE* fp = (FILE*)png_get_io_ptr(png_ptr);
	if (fp == NULL || fread(data,1,length,fp) != length) 
		png_error(png_ptr, "Read Error");
    return 0;
}

VO_U32 VO_API voPNGDecInit(VO_HANDLE *phCodec, VO_IMAGE_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
    VOPNGDECRETURNCODE ret = VO_ERR_NONE;
    PNGDEC *pPNGDec = NULL;

	if(VO_IMAGE_CodingPNG != vType){
		return VO_ERR_PNGDEC_DECODE_HEADER_ERR;
	}

    if(!pUserData)
    {
        pPNGDec = (PNGDEC*)voMpegMalloc (NULL,sizeof(PNGDEC),CACHE_LINE);
    }
    else if((pUserData->memflag & 0x0F) == VO_IMF_USERMEMOPERATOR)
    {
        VO_U8 *tmp, *mem_ptr;
        VO_MEM_OPERATOR* pMemOp = (VO_MEM_OPERATOR*)pUserData->memData;
        voMemAlloc(tmp, pMemOp, vType, sizeof(PNGDEC)+CACHE_LINE);
        if(tmp == NULL) {
            return(VO_ERR_OUTOF_MEMORY);
        }
        mem_ptr = (VO_U8 *)( (VO_U32) (tmp + CACHE_LINE - 1) & ( ~(VO_U32) (CACHE_LINE - 1) ) );
        if (mem_ptr == tmp)
            mem_ptr += CACHE_LINE;
        *(mem_ptr - 1) = (VO_U8) (mem_ptr - tmp);
        *phCodec = (VO_VOID *)mem_ptr;
        memset(mem_ptr, 0, sizeof(PNGDEC));
        ((PNGDEC *)(*phCodec))->memOperater = pMemOp;
    }

    memset(pPNGDec,0,sizeof(PNGDEC)); 

    pPNGDec->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, sk_error_fn, NULL);
    if ( pPNGDec->png_ptr == NULL) {
        return VO_ERR_OUTOF_MEMORY;
    }
    
    pPNGDec->info_ptr = png_create_info_struct(pPNGDec->png_ptr);
    if ( pPNGDec->info_ptr == NULL) {
        png_destroy_read_struct(&pPNGDec->png_ptr, png_infopp_NULL, png_infopp_NULL);
        return VO_ERR_OUTOF_MEMORY;
    }

    *phCodec = (VO_HANDLE)pPNGDec;

	return ret;
}
VO_U32 VO_API voPNGDecUninit(VO_HANDLE hCodec)
{
    VO_U32 ret = VO_ERR_NONE;
    PNGDEC * pPNGDec = (PNGDEC*)hCodec;    

     if(pPNGDec->image_buffer)
     {
        free(pPNGDec->image_buffer);
        pPNGDec->image_buffer = NULL;
     }
     if(pPNGDec->row_buffer)
     {
         free(pPNGDec->row_buffer);
         pPNGDec->row_buffer = NULL;
     }
    if(pPNGDec->png_ptr->io_ptr)
    {
        fclose(pPNGDec->png_ptr->io_ptr);
    }
	png_destroy_read_struct(&pPNGDec->png_ptr, &pPNGDec->info_ptr, png_infopp_NULL);

    return ret;
}

VO_U32 VO_API voPNGDecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	VO_S32 ret = VO_ERR_NONE;
    PNGDEC * pPNGDec = (PNGDEC*)hCodec;
    VO_U32 origWidth, origHeight; // component=1;
    VO_U32 bit_depth, color_type, interlace_type ; // hasAlpha = 0, number_passes;

    switch(nID){
        case VO_PID_PNG_INPUTFILE:
        {

#if (defined(LINUX) || defined(_IOS))
            char* infilename = ((char*)plValue);
            FILE *fp =fopen(infilename,"rb");
            if(fp == NULL)
                return -2;
#else
            char* infilename = ((char*)plValue);
         //   FILE *fp =_wfopen((TCHAR*)infilename,L"rb");
			FILE *fp = fopen(infilename,"rb");
            if(fp == NULL)
                return -2;
#endif
            pPNGDec->input_mode= 0; 
            png_set_read_fn(pPNGDec->png_ptr, (void *)fp, vo_read_data); 
            break;
        }
    default:
        return VO_ERR_WRONG_PARAM_ID;
    }

    png_set_keep_unknown_chunks(pPNGDec->png_ptr, PNG_HANDLE_CHUNK_ALWAYS, (png_byte*)"", 0);
    ret = png_read_info(pPNGDec->png_ptr, pPNGDec->info_ptr);
    if(ret != 0)
        goto END;
    ret = png_get_IHDR(pPNGDec->png_ptr, pPNGDec->info_ptr, &origWidth, &origHeight, (int*)&bit_depth, (int*)&color_type,
        (int*)&interlace_type, int_p_NULL, int_p_NULL);
    if(ret != 0)
        goto END;
    pPNGDec->origWidth  = origWidth;
    pPNGDec->origHeight = origHeight;
    pPNGDec->bit_depth  = bit_depth;
    pPNGDec->color_type = color_type;
    pPNGDec->interlace_type = interlace_type;
    pPNGDec->component = pPNGDec->info_ptr->channels ;
END:
    return ret;
}

VO_U32 VO_API voPNGDecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
    VO_U32 ret = VO_ERR_NONE;
    PNGDEC * pPNGDec = (PNGDEC*)hCodec;
    PNGDECINFO *pPNGInfo = (PNGDECINFO*)plValue;
    switch(nID)
    {
    case VO_PID_PNG_INFO:
        {
            pPNGInfo->origWidth  = pPNGDec->origWidth;
            pPNGInfo->origHeight = pPNGDec->origHeight;
            pPNGInfo->bit_depth  = pPNGDec->bit_depth;
            pPNGInfo->color_type = pPNGDec->color_type;
            pPNGInfo->interlace_type = pPNGDec->interlace_type;
        }
    default:
        return VO_ERR_WRONG_PARAM_ID;
    }
    return ret;
}

VO_U32 VO_API voPNGDecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
    VO_U32 ret = VO_ERR_NONE;
    PNGDEC * pPNGDec = (PNGDEC*)hCodec;
    VO_U32 origWidth, origHeight,component=1;
    VO_U32 bit_depth, color_type, interlace_type , hasAlpha = 0, number_passes;
     png_colorp palette;
     int num_palette=0;
     png_bytep trans;
    int num_trans,colorCount,i/*,j*/,y;

    origWidth = pPNGDec->origWidth;
    origHeight = pPNGDec->origHeight;
    bit_depth = pPNGDec->bit_depth;
    color_type = pPNGDec->color_type;
    interlace_type = pPNGDec->interlace_type;

    if (bit_depth == 16) {
        png_set_strip_16(pPNGDec->png_ptr);
    }

    if (bit_depth < 8) {
        png_set_packing(pPNGDec->png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_gray_1_2_4_to_8(pPNGDec->png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(pPNGDec->png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
		;
    }
	else 
    {
        png_color_16p   transpColor = NULL;
        int numTransp = 0,valid;
        
        png_get_tRNS(pPNGDec->png_ptr, pPNGDec->info_ptr, NULL, &numTransp, &transpColor);
        
        valid = png_get_valid(pPNGDec->png_ptr, pPNGDec->info_ptr, PNG_INFO_tRNS);
        
        if (valid && numTransp == 1 && transpColor != NULL) {
            if (color_type & PNG_COLOR_MASK_COLOR) {
                if (16 == bit_depth) {
                    ;
                } else {
                    ;
                }
            } else {    // gray
                if (16 == bit_depth) {
                    ;
                } else {
                    ;
                }
            }
        }

        if (valid ||
                PNG_COLOR_TYPE_RGB_ALPHA == color_type ||
                PNG_COLOR_TYPE_GRAY_ALPHA == color_type) {
            hasAlpha = 1;
            //config = SkBitmap::kARGB_8888_Config;
        } else {    // we get to choose the config
			;
        }
    }

	if (color_type == PNG_COLOR_TYPE_PALETTE) 
    {
        png_get_PLTE(pPNGDec->png_ptr, pPNGDec->info_ptr, &palette, &num_palette);
        colorCount  = num_palette + (num_palette < 256);
        //colorTable = SkNEW_ARGS(SkColorTable, (colorCount));
        //SkPMColor* colorPtr = colorTable->lockColors();
        if (png_get_valid(pPNGDec->png_ptr, pPNGDec->info_ptr, PNG_INFO_tRNS)) {
            png_get_tRNS(pPNGDec->png_ptr, pPNGDec->info_ptr, &trans, &num_trans, NULL);
            hasAlpha = (num_trans > 0);
        } else {
            num_trans = 0;
            //colorTable->setFlags(colorTable->getFlags() | SkColorTable::kColorsAreOpaque_Flag);
        }        
        // check for bad images that might make us crash
        if (num_trans > num_palette) {
            num_trans = num_palette;
        }   

#ifdef VO_CE
		for(i=0;i<num_palette;i++)
		{
			 unsigned char tmp = palette[i].red;
			 palette[i].red = palette[i].blue;
			 palette[i].blue =tmp;			
		}
#endif
    }

    if (color_type & PNG_COLOR_MASK_COLOR)
		png_set_bgr(pPNGDec->png_ptr);

	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY) {
        png_set_filler(pPNGDec->png_ptr, 0xff, PNG_FILLER_AFTER);
    }

	number_passes = interlace_type != PNG_INTERLACE_NONE ? 
                        png_set_interlace_handling(pPNGDec->png_ptr) : 1;

	png_read_update_info(pPNGDec->png_ptr, pPNGDec->info_ptr);

    component = pPNGDec->info_ptr->channels;
    pPNGDec->png_ptr->total_time = 0;

    //printf("number_passes = %d \n",number_passes);

    if(!pPNGDec->image_buffer)
    {
        if(component>=3)
        {
            pPNGDec->image_buffer = (VO_BYTE*)malloc(((pPNGDec->origWidth&~7)+8)*component*((origHeight&~7) +8)*sizeof(VO_BYTE));
            for (i = 0; i < number_passes; i++) 
            {
                for (y = 0; y < origHeight; y++) 
                {
                    unsigned char* bmRow = pPNGDec->image_buffer+(((pPNGDec->origWidth&~7)+8)*component)*y;
                    png_read_rows(pPNGDec->png_ptr, &bmRow, png_bytepp_NULL, 1);
                }
            }
        }
        else
        {
            component = 3;
            pPNGDec->image_buffer = (VO_BYTE*)malloc(((pPNGDec->origWidth&~7)+8)*component*((origHeight&~7) +8)*sizeof(VO_BYTE));
            pPNGDec->row_buffer    = (VO_BYTE*)malloc(((pPNGDec->origWidth&~7)+8)*sizeof(VO_BYTE));
            for (i = 0; i < number_passes; i++) 
            {
                for (y = 0; y < origHeight; y++) 
                {
                    VO_U32 j=0;
                    unsigned char* dst =  pPNGDec->image_buffer+((pPNGDec->origWidth&~7)+8)*component*y;
                    unsigned char* bmRow = pPNGDec->row_buffer;
                    png_read_rows(pPNGDec->png_ptr, &bmRow, png_bytepp_NULL, 1);
                    for(j=0;j<pPNGDec->origWidth;j++)
                    {
                        VO_BYTE pixel = bmRow[j];
                        *dst++ = palette[pixel].red;
                        *dst++ = palette[pixel].green;
                        *dst++ = palette[pixel].blue;
                    }
                }
            }
        }
    }


    pOutFormat->Format.Height = pPNGDec->origHeight;
    pOutFormat->Format.Width = pPNGDec->origWidth;
    pOutFormat->Format.Type = VO_VIDEO_FRAME_I;

    pOutData->Buffer[0] = pPNGDec->image_buffer;
    pOutData->Buffer[1] = NULL;
    pOutData->Buffer[2] = NULL;
    pOutData->Stride[0] = ((pPNGDec->origWidth&~7)+8)*component;
    pOutData->Stride[1] = 0;
    pOutData->Stride[2] = 0;
    if(component == 4 )
        pOutData->ColorType = VO_COLOR_RGB32_PACKED;
    else if(component == 3)
        pOutData->ColorType = VO_COLOR_RGB888_PACKED;


    ret = png_read_end(pPNGDec->png_ptr, pPNGDec->info_ptr);
    if(!ret == 0)
        return ret;

    return ret;
}

 VO_S32 VO_API voGetPNGDecAPI (VO_IMAGE_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VO_IMAGE_DECAPI *pPNGDec = pDecHandle;

	if(!pPNGDec)
		return VO_ERR_INVALID_ARG;

	pPNGDec->Init   = voPNGDecInit;
	pPNGDec->Uninit = voPNGDecUninit;
	pPNGDec->SetParam = voPNGDecSetParameter;
	pPNGDec->GetParam = voPNGDecGetParameter;
	pPNGDec->Process = voPNGDecProcess;

	return VO_ERR_NONE;

}