#include "stdio.h"
#include "gif_lib.h"
#include "voGifDec.h"
#include "voGif.h"

//#define LICENSEFILE 1
#ifdef LICENSEFILE 
#include "voCheck.h"
VO_HANDLE g_hGifDecInst = NULL;
#endif
//#ifdef VO_ARMV77
//{
//unsigned char *src,*dst;
//for(y=0;y<height/16;y++)
//{
//	int h;
//	src = pGifDec->bkgrdimage+y*width*48;
//	dst = inbuf+y*width*48;
//	for(x=0;x<(width*3)/16;x++)
//	{
//		copy_mem16x16_neon(src,width*3,dst,width*3);
//		src+=16;dst+=16;
//	}
//	for(h=0;h<16;h++)
//		memcpy(dst+h*width*3,src+h*width*3,(width*3)%16);
//}
//for(y=0;y<height%16;y++)
//{
//	memcpy(dst,src,width*3);
//	dst +=width*3;
//	src +=width*3;
//}
//}
//#else

#define NEW_RGB
#ifdef NEW_RGB
int get_tiff_data(GifDecOBj *pGifDec,unsigned char* inbuf)
{
	GifByteType *extData  = pGifDec->extData;
	GifFileType* gif      = pGifDec->gif;
	unsigned long width= pGifDec->gif->SWidth;
	unsigned long height= pGifDec->gif->SHeight;
	SavedImage* image;
	unsigned long colorCount=0,x=0,y=0;
	unsigned char/** gifframes=NULL,*/*scanline=NULL;

	long t1,t2;
	gif_color my_palette[256];
	do 
	{
		if (DGifGetRecordType(gif, &pGifDec->recType) == GIF_ERROR)
			return -1;
        
        switch (pGifDec->recType) 
		{
        case IMAGE_DESC_RECORD_TYPE: 
			{
            if (DGifGetImageDesc(gif) == GIF_ERROR) {
                return -1;
            }            
            if (gif->ImageCount < 1) {    // sanity check
                return -1;
            }

            image = &gif->SavedImages[gif->ImageCount-1];
            pGifDec->desc = image->ImageDesc;
            
            // check for valid descriptor
            if (   (pGifDec->desc.Top | pGifDec->desc.Left) < 0 ||
                    pGifDec->desc.Left + pGifDec->desc.Width > width ||
                    pGifDec->desc.Top + pGifDec->desc.Height > height) {
                return -1;
            }
            
            // now we decode the colortable
            colorCount = 0;
            { 
				int index;
				const ColorMapObject* cmap = find_colormap(gif);
                if (NULL == cmap) {
                    return -1;
                }

                colorCount = cmap->ColorCount;
                for (index = 0; index < colorCount; index++)
				{
					my_palette[index].blue = cmap->Colors[index].Red;
					my_palette[index].green = cmap->Colors[index].Green;
					my_palette[index].red = cmap->Colors[index].Blue;
					my_palette[index].resevered = 255;					
				}
                pGifDec->transpIndex = find_transpIndex(pGifDec->temp_save, colorCount);
                if (pGifDec->transpIndex < 0)
					pGifDec->transpIndex = colorCount-1;
                else
					;
            }  

            // time to decode the scanlines
			if(pGifDec->bkgrdimage==NULL)
				pGifDec->bkgrdimage = (unsigned char*)malloc(sizeof(unsigned char)*width*height*3);

			if(pGifDec->bkgrdimage==NULL)
				return -1;


			scanline = inbuf;//gifframes;
			pGifDec->fill = pGifDec->transpIndex;
            // are we only a subset of the total bounds?
            if ((pGifDec->desc.Top | pGifDec->desc.Left) > 0 ||
                 pGifDec->desc.Width < width || pGifDec->desc.Height < height)
            {     
                if (pGifDec->transpIndex >= 0) {
                    pGifDec->fill = pGifDec->transpIndex;
                } else {
                    pGifDec->fill = gif->SBackGroundColor;
                }
				if(pGifDec->fill>colorCount)
					pGifDec->fill = 0;
                scanline = inbuf + pGifDec->desc.Top * width + pGifDec->desc.Left;
            }

			//printf("%d %d %d\n",pGifDec->prevdispmeth,pGifDec->desc.Width,pGifDec->desc.Height);
			if(gif->ImageCount-1!=0)
			{
				unsigned char r,g,b;
				unsigned char* tmp = pGifDec->bkgrdimage;

				if(pGifDec->prevdispmeth==2)
				{
					for(x=0;x<width*3;x++)
					{
						*tmp++ = pGifDec->r;
						*tmp++ = pGifDec->g;
						*tmp++ = pGifDec->b;
					}
					for(y=1;y<height;y++)
						memcpy(pGifDec->bkgrdimage+y*width*3,pGifDec->bkgrdimage,width*3);		
				}
				else if(pGifDec->prevdispmeth==3)
					;
				else
					;
			} 
			else
			{
				unsigned char r,g,b;
				unsigned char* tmp = pGifDec->bkgrdimage;
				pGifDec->first_transpIndex = (pGifDec->gif_gce.flags & 0x1) ? pGifDec->gif_gce.transpcolindex : -1;				
				pGifDec->r = (pGifDec->devclr>>16)&0xff;
				pGifDec->g = (pGifDec->devclr>>8)&0xff;
				pGifDec->b = (pGifDec->devclr)&0xff;
				for(x=0;x<width*3;x++)
				{
					*tmp++ = pGifDec->r;
					*tmp++ = pGifDec->g;
					*tmp++ = pGifDec->b;
				}
				for(y=1;y<height;y++)
					memcpy(pGifDec->bkgrdimage+y*width*3,pGifDec->bkgrdimage,width*3);
			}          

            // add-----
            // pGifDec->gif_gce.transpcolindex 应该和pGifDec->transpIndex是相等的
            pGifDec->first_transpIndex = (pGifDec->gif_gce.flags & 0x1) ? pGifDec->gif_gce.transpcolindex  : -1;		

            // now decode each scanline
            if (gif->Image.Interlace) 
            {
				int  fDeltaY,fCurrY;
				unsigned char* fStartYPtr = gStartingIterlaceYValue;
				unsigned char* fDeltaYPtr = gDeltaIterlaceYValue;
				fCurrY  = *fStartYPtr++;
				fDeltaY = *fDeltaYPtr++;
                for (y = 0; y < pGifDec->desc.Height; y++)
                {
					unsigned char* row = scanline + fCurrY * width;
					if (DGifGetLine(gif, row, pGifDec->desc.Width) == GIF_ERROR)
                        return -1;
					{
						int yy = fCurrY + fDeltaY;
						while(yy >= pGifDec->desc.Height)
						{
							if (gStartingIterlaceYValue + 4 == fStartYPtr)
							{
								yy = 0;
							} else {
								yy = *fStartYPtr++;
								fDeltaY = *fDeltaYPtr++;
							}
						}
						fCurrY = yy;
					}
                }
				gifframemix_new  (pGifDec->bkgrdimage,inbuf,pGifDec->desc,width,height,pGifDec->first_transpIndex,my_palette);
                
				memcpy(inbuf,pGifDec->bkgrdimage,width*height*3);
	          }
            else
            {
                // easy, non-interlace case
				for (y = 0; y < pGifDec->desc.Height; y++) 
				{
                    if (DGifGetLine(gif, scanline, pGifDec->desc.Width) == GIF_ERROR) 
                        return -1;
					scanline += width;
                }

#ifdef TIME
				pGifDec->timeused0 += gif->timeused0;
				pGifDec->timeused1 += gif->timeused1;
				pGifDec->timeused2 += gif->timeused2;
				gif->timeused0 =0;
				gif->timeused1 =0;
				gif->timeused2 =0;
#endif
				gifframemix_new  (pGifDec->bkgrdimage,inbuf,pGifDec->desc,width,height,pGifDec->first_transpIndex,my_palette);
                
				memcpy(inbuf,pGifDec->bkgrdimage,width*height*3);
            }
			pGifDec->prevdispmeth=((pGifDec->gif_gce.flags >> 2) & 0x7);
            } 
			pGifDec->recType = UNDEFINED_RECORD_TYPE;
			break;            
        case EXTENSION_RECORD_TYPE:
            if (DGifGetExtension(gif, &pGifDec->temp_save.Function,&extData) == GIF_ERROR)
                return -1;
			if(pGifDec->temp_save.Function==0xf9)
			{
				pGifDec->gif_gce.flags = extData[1];
				pGifDec->gif_gce.delaytime = extData[2]+(extData[3]<<8);
				pGifDec->gif_gce.transpcolindex = extData[4];
			}
            while (extData != NULL) 
			{
                if (AddExtensionBlock(&pGifDec->temp_save, extData[0], &extData[1]) == GIF_ERROR)
                    return -1;       
                if (DGifGetExtensionNext(gif, &extData) == GIF_ERROR)
                    return -1;
                pGifDec->temp_save.Function = 0;
            }
            break;            
        case TERMINATE_RECORD_TYPE:
			pGifDec->recType = TERMINATE_RECORD_TYPE;
            break;
            
        default:	/* Should be trapped by DGifGetRecordType */
            break;
        }
    } while (pGifDec->recType!=UNDEFINED_RECORD_TYPE&&pGifDec->recType != TERMINATE_RECORD_TYPE);

	if(pGifDec->recType==UNDEFINED_RECORD_TYPE)
		return VO_ERR_GIFDEC_DECODE_UNFINISHED;
	else
		return 0;

}
#else
int get_tiff_data(GifDecOBj *pGifDec,unsigned char* inbuf)
{
	GifByteType *extData  = pGifDec->extData;
	GifFileType* gif      = pGifDec->gif;
	unsigned long width= pGifDec->gif->SWidth;
	unsigned long height= pGifDec->gif->SHeight;
	SavedImage* image;
	unsigned long colorCount=0,y=0;
	unsigned char/** gifframes=NULL,*/*scanline=NULL;
	gif_color my_palette[256];
	do 
	{
		if (DGifGetRecordType(gif, &pGifDec->recType) == GIF_ERROR)
			return -1;
        
        switch (pGifDec->recType) 
		{
        case IMAGE_DESC_RECORD_TYPE: 
			{
            if (DGifGetImageDesc(gif) == GIF_ERROR) {
                return -1;
            }            
            if (gif->ImageCount < 1) {    // sanity check
                return -1;
            }

            image = &gif->SavedImages[gif->ImageCount-1];
            pGifDec->desc = image->ImageDesc;
            
            // check for valid descriptor
            if (   (pGifDec->desc.Top | pGifDec->desc.Left) < 0 ||
                    pGifDec->desc.Left + pGifDec->desc.Width > width ||
                    pGifDec->desc.Top + pGifDec->desc.Height > height) {
                return -1;
            }
            
            // now we decode the colortable
            colorCount = 0;
            { 
				int index;
				const ColorMapObject* cmap = find_colormap(gif);
                if (NULL == cmap) {
                    return -1;
                }

                colorCount = cmap->ColorCount;
                for (index = 0; index < colorCount; index++)
				{
					my_palette[index].blue = cmap->Colors[index].Red;
					my_palette[index].green = cmap->Colors[index].Green;
					my_palette[index].red = cmap->Colors[index].Blue;
					my_palette[index].resevered = 255;					
				}
                pGifDec->transpIndex = find_transpIndex(pGifDec->temp_save, colorCount);
                if (pGifDec->transpIndex < 0)
					pGifDec->transpIndex = colorCount-1;
                else
					;
            }  

            // time to decode the scanlines
			if(pGifDec->bkgrdimage==NULL)
				pGifDec->bkgrdimage = (unsigned char*)malloc(sizeof(unsigned char)*width*height*3);


			scanline = inbuf;//gifframes;
			pGifDec->fill = pGifDec->transpIndex;
            // are we only a subset of the total bounds?
            if ((pGifDec->desc.Top | pGifDec->desc.Left) > 0 ||
                 pGifDec->desc.Width < width || pGifDec->desc.Height < height)
            {     
                if (pGifDec->transpIndex >= 0) {
                    pGifDec->fill = pGifDec->transpIndex;
                } else {
                    pGifDec->fill = gif->SBackGroundColor;
                }
				if(pGifDec->fill>colorCount)
					pGifDec->fill = 0;
                scanline = inbuf + pGifDec->desc.Top * width + pGifDec->desc.Left;
            }

			printf("**%d \n",pGifDec->prevdispmeth);
			if(gif->ImageCount-1!=0)
			{
				if(pGifDec->prevdispmeth==2)
				{
					memset(pGifDec->bkgrdimage,pGifDec->first_transpIndex,width*height);
					//memset(inbuf,pGifDec->first_transpIndex,width*height);
				}
				//else if(pGifDec->prevdispmeth==3)
				//	;//memset(inbuf,0,width*height);
				//else
				//	;//memset(inbuf,pGifDec->fill,width*height);
			}
			else
			{
				pGifDec->first_transpIndex = (pGifDec->gif_gce.flags & 0x1) ? pGifDec->gif_gce.transpcolindex : -1;
				memset(pGifDec->bkgrdimage,pGifDec->gif_gce.transpcolindex,width*height);
				//memset(inbuf,pGifDec->gif_gce.transpcolindex,width*height);
			}
            
            // now decode each scanline
            if (gif->Image.Interlace) 
            {
				int  fDeltaY,fCurrY;
				unsigned char* fStartYPtr = gStartingIterlaceYValue;
				unsigned char* fDeltaYPtr = gDeltaIterlaceYValue;
                //GifInterlaceIter iter(innerHeight);
				fCurrY  = *fStartYPtr++;
				fDeltaY = *fDeltaYPtr++;
                for (y = 0; y < pGifDec->desc.Height; y++)
                {
					unsigned char* row = scanline + fCurrY * width;
					if (DGifGetLine(gif, row, pGifDec->desc.Width) == GIF_ERROR)
                        return -1;
					{
						int yy = fCurrY + fDeltaY;
						while(yy >= pGifDec->desc.Height)
						{
							if (gStartingIterlaceYValue + 4 == fStartYPtr)
							{
								yy = 0;
							} else {
								yy = *fStartYPtr++;
								fDeltaY = *fDeltaYPtr++;
							}
						}
						fCurrY = yy;
					}
                }
				gifframemix  (pGifDec->bkgrdimage,inbuf,pGifDec->desc,width,height,pGifDec->first_transpIndex);
				gifconvertrgb(pGifDec->bkgrdimage,inbuf,width,height,my_palette,pGifDec->first_transpIndex);
            }
            else
            {
                // easy, non-interlace case
				for (y = 0; y < pGifDec->desc.Height; y++) 
				{
                    if (DGifGetLine(gif, scanline, pGifDec->desc.Width) == GIF_ERROR) 
                        return -1;
					scanline += width;
                }
				gifframemix  (pGifDec->bkgrdimage,inbuf,pGifDec->desc,width,height,pGifDec->first_transpIndex);
				gifconvertrgb(pGifDec->bkgrdimage,inbuf,width,height,my_palette,pGifDec->first_transpIndex);
            }
			pGifDec->prevdispmeth=((pGifDec->gif_gce.flags >> 2) & 0x7);
            } 
			pGifDec->recType = UNDEFINED_RECORD_TYPE;
			break;            
        case EXTENSION_RECORD_TYPE:
            if (DGifGetExtension(gif, &pGifDec->temp_save.Function,&extData) == GIF_ERROR)
                return -1;
			if(pGifDec->temp_save.Function==0xf9)
			{
				pGifDec->gif_gce.flags = extData[1];
				pGifDec->gif_gce.delaytime = extData[2]+(extData[3]<<8);
				pGifDec->gif_gce.transpcolindex = extData[4];
			}
            while (extData != NULL) 
			{
                if (AddExtensionBlock(&pGifDec->temp_save, extData[0], &extData[1]) == GIF_ERROR)
                    return -1;       
                if (DGifGetExtensionNext(gif, &extData) == GIF_ERROR)
                    return -1;
                pGifDec->temp_save.Function = 0;
            }
            break;            
        case TERMINATE_RECORD_TYPE:
			pGifDec->recType = TERMINATE_RECORD_TYPE;
            break;
            
        default:	/* Should be trapped by DGifGetRecordType */
            break;
        }
    } while (pGifDec->recType!=UNDEFINED_RECORD_TYPE&&pGifDec->recType != TERMINATE_RECORD_TYPE);

	if(pGifDec->recType==UNDEFINED_RECORD_TYPE)
		return VO_ERR_GIFDEC_DECODE_UNFINISHED;
	else
		return 0;
}
#endif
VO_U32 VO_API voGifDecInit(VO_HANDLE *phCodec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VOGIFDECRETURNCODE ret = VO_ERR_NONE;
	GifDecOBj *pGifDec;
#ifdef LICENSEFILE 
	VO_PTR pLicHandle;
	VO_U32 err;
	VO_LIB_OPERATOR *pLibOp = NULL;
#endif
	if(VO_IMAGE_CodingGIF != vType){
		return VO_ERR_GIFDEC_DECODE_HEADER_ERR;
	}
#ifdef LICENSEFILE 
	if(pUserData) {
		if(pUserData->memflag & 0xF0)
			pLibOp = pUserData->libOperator;
	}
	err = voCheckLibInit(&pLicHandle, VO_INDEX_DEC_GIF, pUserData?pUserData->memflag:0, g_hGifDecInst, pLibOp);
	if(err != VO_ERR_NONE)
	{
		*phCodec = NULL;
		if(pLicHandle)
		{
			voCheckLibUninit(pLicHandle);
			pLicHandle = NULL;
		}
		return err;
	}
#endif
	if(!pUserData)
	{
		pGifDec = (GifDecOBj *)malloc(sizeof(GifDecOBj));
		memset(pGifDec,0,sizeof(GifDecOBj));
	}
	else
	{
		pGifDec = (GifDecOBj *)malloc(sizeof(GifDecOBj));
		memset(pGifDec,0,sizeof(GifDecOBj));
	}

	pGifDec->recType = UNDEFINED_RECORD_TYPE;
	pGifDec->gif=NULL;
	pGifDec->extData = NULL;
	pGifDec->first_transpIndex=-1;
	pGifDec->bkgrdimage = NULL;
	pGifDec->devclr = 0xffffffff;
#ifdef TIME
	pGifDec->timeused0 =0;
	pGifDec->timeused1 =0;
	pGifDec->timeused2 =0;
#endif

	if(pGifDec==NULL)
		ret = VO_ERR_OUTOF_MEMORY;

#ifdef LICENSEFILE
	pGifDec->phCheck = pLicHandle;
#endif

	*phCodec = (VO_HANDLE)pGifDec;
	return ret;
}

VO_U32 VO_API voGifDecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	VOGIFDECRETURNCODE ret;
	return ret;
}

VO_U32 VO_API voGifDecUninit(VO_HANDLE hCodec)
{
	VOGIFDECRETURNCODE ret = VO_ERR_NONE;
	GifDecOBj *pGifDec = (GifDecOBj*)hCodec;
#ifdef TIME
	{
		FILE* fp=fopen("/Temp/Gif/gif_speed.txt","a");
		fprintf(fp,"<%d %d %d ms>\n",pGifDec->timeused0,pGifDec->timeused1,pGifDec->timeused2);
		fclose(fp);
	}
#endif
	if(pGifDec)
	{
#ifdef LICENSEFILE
	voCheckLibUninit(pGifDec->phCheck);
#endif
		if(pGifDec->bkgrdimage)
		{
			free(pGifDec->bkgrdimage);
			pGifDec->bkgrdimage =NULL;
		}
		if(pGifDec->fp)
			fclose(pGifDec->fp);
		free(pGifDec);
		pGifDec =NULL;
	}
	return ret;
}

VO_U32 VO_API voGifDecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	GifDecOBj *pGifDec;
	pGifDec = (GifDecOBj*)hCodec;
	
	switch(nID)
	{
	case VO_PID_GIF_FILEPATH:
		{
			pGifDec->fp = fopen((char*)plValue,"rb");
			pGifDec->gif = DGifOpen((void*)pGifDec->fp, DecodeCallBackProc);
#ifdef TIME
			pGifDec->gif->timeused0=0;
			pGifDec->gif->timeused1=0;
			pGifDec->gif->timeused2=0;
#endif
        }
		break;
	case VO_PID_GIF_DEVCLOR:
		pGifDec->devclr = *(VO_U32*)plValue;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	if(pGifDec->gif==NULL)
		return VO_ERR_GIFDEC_OPENFILE_ERROR;

	return VO_ERR_NONE;
}

VO_U32 VO_API voGifDecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	int ret = VO_ERR_NONE;
	GifDecOBj *pGifDec;
	VO_VIDEO_BUFFER *pOutData;
	unsigned char* inbuf;
	pGifDec = (GifDecOBj*)hCodec;
	
	switch(nID)
	{
	case VO_PID_GIF_WIDTH:
		*(VO_U32*)plValue = pGifDec->gif->SWidth;
		break;
	case VO_PID_GIF_HEIGHT:
		*(VO_U32*)plValue = pGifDec->gif->SHeight;
		break;
	case VO_PID_GIF_SAMPLESPERPIXEL:
		*(VO_U32*)plValue = 0;//pGifDec->samplesperpixel;
		break;
	case VO_PID_GIF_BITSPERSAMPLE:
		*(VO_U32*)plValue = 0;//pGifDec->bitspersample;
		break;
	case VO_PID_GIF_OUTPUTDATA:
		if(!plValue)
			return VO_ERR_INVALID_ARG;
		pOutData = (VO_VIDEO_BUFFER*)plValue;
		ret = get_tiff_data(pGifDec,pOutData->Buffer[0]);
		if(ret==-1)
			return VO_ERR_OUTOF_MEMORY;

		pOutData->Stride[0]= pGifDec->gif->SWidth;
		pOutData->ColorType = VO_COLOR_RGB888_PACKED;
		pOutData->Time = pGifDec->gif_gce.delaytime*10;//(pOutData.Time*0.01 s)
#ifdef LICENSEFILE
		{
			VO_VIDEO_FORMAT		Format;	
			Format.Height = pGifDec->gif->SWidth;
			Format.Width  = pGifDec->gif->SHeight;
			Format.Type   = VO_VIDEO_FRAME_I;
			voCheckLibCheckImage(pGifDec->phCheck, pOutData, &Format);
		}
#endif
		break;
	case VO_PID_GIF_COLORTYPE:
		*(VO_IV_COLORTYPE*)plValue = VO_COLOR_RGB888_PACKED;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return ret;//VO_ERR_NONE;
}

VO_S32 VO_API voGetGIFDecAPI (VO_IMAGE_DECAPI * pDecHandle)
{
	VO_IMAGE_DECAPI *pGifDec = pDecHandle;

	if(!pGifDec)
		return VO_ERR_INVALID_ARG;

	pGifDec->Init   = voGifDecInit;
	pGifDec->Uninit = voGifDecUninit;
	pGifDec->SetParam = voGifDecSetParameter;
	pGifDec->GetParam = voGifDecGetParameter;
	pGifDec->Process = voGifDecProcess;

	return VO_ERR_NONE;

}