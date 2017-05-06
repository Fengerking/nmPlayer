
/* pngtest.c - a simple test program to test libpng
 *
 * Last changed in libpng 1.2.37 [June 4, 2009]
 * Copyright (c) 1998-2009 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * This code is released under the libpng license.
 * For conditions of distribution and use, see the disclaimer
 * and license in png.h
 *
 * This program reads in a PNG image, writes it out again, and then
 * compares the two files.  If the files are identical, this shows that
 * the basic chunk handling, filtering, and (de)compression code is working
 * properly.  It does not currently test all of the transforms, although
 * it probably should.
 *
 * The program will report "FAIL" in certain legitimate cases:
 * 1) when the compression level or filter selection method is changed.
 * 2) when the maximum IDAT size (PNG_ZBUF_SIZE in pngconf.h) is not 8192.
 * 3) unknown unsafe-to-copy ancillary chunks or unknown critical chunks
 *    exist in the input file.
 * 4) others not listed here...
 * In these cases, it is best to check with another tool such as "pngcheck"
 * to see what the differences between the two files are.
 *
 * If a filename is given on the command-line, then this file is used
 * for the input, rather than the default "pngtest.png".  This allows
 * testing a wide variety of files easily.  You can also test a number
 * of files at once by typing "pngtest -m file1.png file2.png ..."
 */
#include "voPNG.h"
#include "png.h"
#ifndef LINUX
#include <tchar.h>
#endif

static void sk_error_fn(png_structp png_ptr, png_const_charp msg)
{
    longjmp(png_jmpbuf(png_ptr), 1);
}
static int PNGAPI vo_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	FILE* fp = (FILE*)png_get_io_ptr(png_ptr);
	if (fp == NULL || fread(data,1,length,fp) != length) 
		png_error(png_ptr, "Read Error");
}

//int DecodePNG(char *infilename,unsigned long* total1,unsigned long* total2)
//{
//	png_structp png_ptr;
//	png_infop   info_ptr;
//	png_uint_32 origWidth, origHeight,component=1;
//    int bit_depth, color_type, interlace_type , hasAlpha = 0,number_passes;
//	int num_palette=0;
//    png_colorp palette;
//	png_bytep trans;
//    int num_trans,colorCount;
//	int i,y;
//	FILE *fp =fopen(infilename,"rb");
//	char* filename[250];
//	FILE* fout;
//
//	long t1,t2,toal=0;
//	//bit_depth = fread(dat,1,8,fp);
//
//	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, sk_error_fn, NULL);
//    if (png_ptr == NULL) {
//        return -1;
//    }
//	info_ptr = png_create_info_struct(png_ptr);
//    if (info_ptr == NULL) {
//        png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
//        return -1;
//    }
//	png_set_read_fn(png_ptr, (void *)fp, vo_read_data);
//
//	png_set_keep_unknown_chunks(png_ptr, PNG_HANDLE_CHUNK_ALWAYS, (png_byte*)"", 0);
//    //if (this->getPeeker()) {
//    //    png_set_read_user_chunk_fn(png_ptr, (png_voidp)this->getPeeker(), sk_read_user_chunk);
//    //}
//
//	//bit_depth = fread(dat,1,8,fp);
//	png_read_info(png_ptr, info_ptr);
//
//    png_get_IHDR(png_ptr, info_ptr, &origWidth, &origHeight, &bit_depth, &color_type,
//        &interlace_type, int_p_NULL, int_p_NULL);
//
//	if (bit_depth == 16) {
//        png_set_strip_16(png_ptr);
//    }
//    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
//     * byte into separate bytes (useful for paletted and grayscale images). */
//    if (bit_depth < 8) {
//        png_set_packing(png_ptr);
//    }
//    /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
//    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
//        png_set_gray_1_2_4_to_8(png_ptr);
//    }    
//    /* Make a grayscale image into RGB. */
//    if (color_type == PNG_COLOR_TYPE_GRAY ||
//        color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
//        png_set_gray_to_rgb(png_ptr);
//    }
//
//
//
//	if (color_type == PNG_COLOR_TYPE_PALETTE) {
//		;
//    }
//	else 
//    {
//        png_color_16p   transpColor = NULL;
//        int             numTransp = 0,valid;
//        
//        png_get_tRNS(png_ptr, info_ptr, NULL, &numTransp, &transpColor);
//        
//        valid = png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS);
//        
//        if (valid && numTransp == 1 && transpColor != NULL) {
//            if (color_type & PNG_COLOR_MASK_COLOR) {
//                if (16 == bit_depth) {
//                    ;
//                } else {
//                    ;
//                }
//            } else {    // gray
//                if (16 == bit_depth) {
//                    ;
//                } else {
//                    ;
//                }
//            }
//        }
//
//        if (valid ||
//                PNG_COLOR_TYPE_RGB_ALPHA == color_type ||
//                PNG_COLOR_TYPE_GRAY_ALPHA == color_type) {
//            hasAlpha = 1;
//            //config = SkBitmap::kARGB_8888_Config;
//        } else {    // we get to choose the config
//			;
//        }
//    }
//
//	if (color_type == PNG_COLOR_TYPE_PALETTE) 
//    {
//        png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
//        colorCount  = num_palette + (num_palette < 256);
//        //colorTable = SkNEW_ARGS(SkColorTable, (colorCount));
//        //SkPMColor* colorPtr = colorTable->lockColors();
//        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
//            png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL);
//            hasAlpha = (num_trans > 0);
//        } else {
//            num_trans = 0;
//            //colorTable->setFlags(colorTable->getFlags() | SkColorTable::kColorsAreOpaque_Flag);
//        }        
//        // check for bad images that might make us crash
//        if (num_trans > num_palette) {
//            num_trans = num_palette;
//        }   
//
//#ifdef VO_CE
//		for(i=0;i<num_palette;i++)
//		{
//			 unsigned char tmp = palette[i].red;
//			 palette[i].red = palette[i].blue;
//			 palette[i].blue =tmp;			
//		}
//#endif
//    }
//
//	//png_set_invert_alpha(png_ptr);
//	if (color_type & PNG_COLOR_MASK_COLOR)
//		png_set_bgr(png_ptr);
//
//	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY) {
//        png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
//    }
//
//	number_passes = interlace_type != PNG_INTERLACE_NONE ? 
//                        png_set_interlace_handling(png_ptr) : 1;
//
//	png_read_update_info(png_ptr, info_ptr);
//
//	component = info_ptr->channels;
//
//	png_ptr->total_time = 0;
//#define WRITE_BMP 1
//#ifdef WRITE_BMP
//#ifdef _WIN32_WCE
//	sprintf(filename, "/sdmmc/result_0.bmp");
//#else
//	sprintf(filename, "%s.bmp",infilename);
//#endif
//	fout = writebmphead(filename,origWidth,origHeight,component,palette,num_palette);
//#endif
//    for (i = 0; i < number_passes; i++) 
//	{
//		unsigned char* bmRow;
//		bmRow = (unsigned char*)malloc((info_ptr->rowbytes + 8)*sizeof(unsigned char));
//#ifdef _WIN32_WCE
//		t1 = GetTickCount();
//#endif
//        for (y = 0; y < origHeight; y++) 
//		{ 
//            png_read_rows(png_ptr, &bmRow, png_bytepp_NULL, 1);
//#ifdef WRITE_BMP
//			if(i==number_passes-1)
//				writebmpdata(fout,bmRow,origWidth,1,origWidth*component,component);
//#endif
//        }
//		free(bmRow);
//
//#ifdef _WIN32_WCE
//		t2 = GetTickCount();
//		toal += (t2-t1);
//#endif
//    }
//
//#ifdef WRITE_BMP
//		writebmptail(fout);
//#endif
//
//	*total1 += toal;
//	*total2 += png_ptr->total_time;
//
//	png_read_end(png_ptr, info_ptr);
//	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
//	fclose(fp);
//
//	return 0;
//}


//#ifdef _WIN32_WCE
////int WinMain()
//int _tmain(int argc, TCHAR **argv) 
//#else // _WIN32_WCE
//int main(int argc, char **argv)
//#endif//_WIN32_WCE
//{
//	int i=0,totaltime1=0,totaltime2=0;
//#ifdef _WIN32_WCE
//	if(1)
//	{
//
//		FILE *fp = fopen("/sdmmc/PNG_Speed.txt","a");
//		fprintf(fp,"start: \n");
//		fclose(fp);
//	}
//#endif
//	for(i=0;i<1;i++)
//#ifdef _WIN32_WCE
//		DecodePNG("/sdmmc/logo.png",&totaltime1,&totaltime2);
//#else
//		DecodePNG("E:/MyResource/Image/clips/PNGIMGE/492_PNG_LZ77_1016x1024'8b.png",&totaltime1,&totaltime2);
//#endif
//
//#ifdef _WIN32_WCE
//	if(1)
//	{
//		FILE *fp = fopen("/sdmmc/PNG_Speed.txt","a");
//		fprintf(fp,"Cost= %f ms %f ms\n",totaltime1*1.0/(i),totaltime2*1.0/(i));
//		fclose(fp);
//	}
//#endif
//	return 1;
//}

//int i,j;
//        char argbfile[250];
//        FILE* fp;
//        sprintf(argbfile,"%s_A.bmp",infilename);
//        fp =fopen(argbfile,"wb");
//       
//
//        printf("%d %d %d",component,outInfo.Format.Width&~7,(outInfo.Format.Height&~7)+8);   
//        for(i =0; i< (outInfo.Format.Height&~7)+8;i++)
//        {
//            unsigned char* row = outdata.Buffer[0] + outdata.Stride[0]*((outInfo.Format.Height&~7)+8-i-1);
//            fwrite(row, (outInfo.Format.Width&~7)*component,1,fp);
//        }
//        fclose(fp);


#ifdef _WIN32_WCE
//int WinMain()
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
    VO_S32 ret =0;
    VO_HANDLE phCodec;
	VO_CODECBUFFER	pInData;
	VO_VIDEO_BUFFER	outdata;
	VO_VIDEO_FORMAT	outformat;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_IMAGE_DECAPI	voPNGDec;
    VO_U32 component = 0;
    //char infilename[250];
    //char* infilename = "E:/MyResource/Image/PNG/609x773_296k.png";
#ifndef LINUX
    TCHAR* infilename = _T("E:/MyResource/Image/PNG/609x773_296k.png");
#else
    char* infilename = "609x773_296k.png";
#endif

   /* if(argc == 1)
	{       
		printf("\nUsage: voPNGDec.exe intputFile");
		return;
	}
	else
	{
		strcpy(infilename, argv[1]);
	}  */ 

    voGetPNGDecAPI(&voPNGDec);

    ret = voPNGDec.Init(&phCodec, VO_IMAGE_CodingPNG, NULL);
    if(ret != 0)
        goto END;
    ret = voPNGDec.SetParam(phCodec,VO_PID_PNG_INPUTFILE,infilename);
    if(ret != 0)
        goto END;
    ret = voPNGDec.Process(phCodec,&pInData,&outdata,&outInfo);
    if(ret != 0)
        goto END;

    if(outdata.ColorType == VO_COLOR_RGB32_PACKED)
        component = 4;
    else if(outdata.ColorType == VO_COLOR_RGB888_PACKED)
        component = 3;

    if(component ==3){
        int i;
        char bmpfile[250];
        FILE *fout;
        sprintf(bmpfile,"%s.bmp",infilename);
        fout = writebmphead(bmpfile,outInfo.Format.Width,outInfo.Format.Height,3,NULL,0);
   
        for(i =0; i< (outInfo.Format.Height&~7)+8;i++)
        {
            unsigned char* row = outdata.Buffer[0] + outdata.Stride[0]*i;
            writebmpdata(fout,row,outInfo.Format.Width,1,outInfo.Format.Width*component,component);
        }
        writebmptail(fout);
    }
    else if(component == 4)
    {
        int i,j;
        char argbfile[250];
        FILE* fp;
        unsigned char* rgb_buffer;
        sprintf(argbfile,"%s_A.bmp",infilename);
        fp = writebmphead(argbfile,outInfo.Format.Width,outInfo.Format.Height,3,NULL,0);

        rgb_buffer =  (unsigned char*)malloc((outInfo.Format.Height)*(outInfo.Format.Width)*3);
          
        for(i =0; i< outInfo.Format.Height;i++)
        {
            unsigned char* row_src = outdata.Buffer[0] + outdata.Stride[0]*i;
            unsigned char* row_dst = rgb_buffer+(outInfo.Format.Width)*3*i;
            for(j=0;j<(outInfo.Format.Width*4);j+=4)
            {               
                unsigned char r = row_src[j];
                unsigned char g = row_src[j+1];
                unsigned char b = row_src[j+2];
                unsigned char a = row_src[j+3];
                unsigned char a1=(unsigned char)~a;

                *row_dst++ =  (unsigned char)(unsigned char)((255 * a1 + a * r)  >> 8 ); 
                *row_dst++ =  (unsigned char)(unsigned char)((255 * a1 + a * g)  >> 8 );
                *row_dst++ =  (unsigned char)(unsigned char)((255 * a1 + a * b)  >> 8 );                
            }
            writebmpdata(fp,
                rgb_buffer+(outInfo.Format.Width)*3*i,
                outInfo.Format.Width,
                1,
                outInfo.Format.Width*3,
                3);
            //fwrite(rgb_buffer+(outInfo.Format.Width)*3*i, outInfo.Format.Width*3,1,fp);
        }

        free(rgb_buffer);
         writebmptail(fp);
    }
    else
    {
        printf("\a");
    }
    
END:
    voPNGDec.Uninit(phCodec);

	return 0;
}
