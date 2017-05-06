/*
 *  CCGRender.cpp
 *  vompEngn
 *
 *  Created by Lin Jun on 1/27/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CCGRender.h"
#import <Foundation/Foundation.h>

CCGRender::CCGRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
:CBaseVideoRender(hInst, hView, pMemOP)
,m_pVideoBuf(NULL)
{
}

CCGRender::~CCGRender(void)
{
	if(m_pVideoBuf)
	{
		delete m_pVideoBuf;
		m_pVideoBuf = NULL;
	}
}

VO_U32 	CCGRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	voCAutoLock lock (&m_csDraw);
	
	/*
	CGContextRef c = (CGContextRef)m_hView;
	int width	= m_nDrawWidth;
	int height	= m_nDrawHeight;
	char* rgba = (char*)malloc(width*height*4);
	for(int i=0; i < width*height; ++i) {
		rgba[4*i] = 0;
		rgba[4*i+1] = rand()%250;
		rgba[4*i+2] = rand()%250;
		rgba[4*i+3] = rand()%250;
	}
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	CGContextRef bitmapContext = CGBitmapContextCreate(
													   rgba,
													   width,
													   height,
													   8, // bitsPerComponent
													   4*width, // bytesPerRow
													   colorSpace,
													   kCGImageAlphaNoneSkipLast);
	
	CFRelease(colorSpace);
	
	CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
	
	CGRect imageRect = {{m_nDrawLeft, m_nDrawTop}, {width, height}};
	
	CGContextAddRect(c, CGRectMake(0, 0, width, height));
	CGContextDrawImage(c, imageRect, cgImage);
	CGContextFlush(c);
	free(rgba);
	CGImageRelease(cgImage);

	return 0;
	 */
	
#ifdef _DUMP_YUV
	
#define _YUV_INTERLACE_OUTPUT_ 1
	
	if(m_hYUV)
	{
		unsigned char* out_src = pVideoBuffer->Buffer[0];
		
		int i = 0;
		int width = m_nVideoWidth;
		int height= m_nVideoHeight;
		
		/* output decoded YUV420 frame */
		/* Y */
		out_src = pVideoBuffer->Buffer[0];
		for(i = 0;i < height; i++){
			fwrite(out_src, width, 1, m_hYUV);
			out_src += pVideoBuffer->Stride[0];
		}
		
#if _YUV_INTERLACE_OUTPUT_
		/* U */
		out_src = pVideoBuffer->Buffer[1];
		for(i = 0; i< height/2; i++){
			fwrite(out_src, width/2, 1, m_hYUV);
			out_src += pVideoBuffer->Stride[1];
		}
		
		/* V */
		out_src = pVideoBuffer->Buffer[2];
		for(i = 0; i < height/2; i++){
			fwrite(out_src, width/2, 1, m_hYUV);
			out_src += pVideoBuffer->Stride[2];
		}
#else
		/* U */
		out_src = pVideoBuffer->Buffer[1];
		fwrite(out_src, width*height/4, 1, m_hYUV);
		
		/* V */
		out_src = pVideoBuffer->Buffer[2];
		fwrite(out_src, width*height/4, 1, m_hYUV);
		
#endif
		
	}
#endif
	
	if (m_fCallBack != NULL)
		return CBaseVideoRender::Render (pVideoBuffer, nStart, bWait);
	
	VO_VIDEO_BUFFER buf;
	
	{
		buf.Buffer[0] = m_pVideoBuf;
		//VOLOGI("cc buf ptr = %x", buf.Buffer[0]);
	}
	
	
	if (m_nVideoColor == VO_COLOR_RGB565_PACKED) 
	{
		buf.Stride[0] = m_nDrawWidth*2;
	}
	else if(m_nVideoColor == VO_COLOR_ARGB32_PACKED)
		buf.Stride[0] = m_nDrawWidth*4;
	
	buf.Stride[1] = 0;
	buf.Stride[2] = 0;
	
	if (!ConvertData (pVideoBuffer, &buf, nStart, bWait))
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
	
#ifdef _DUMP_RGB
	if(m_hRGB)
	{
		fwrite(buf.Buffer[0], 1, buf.Stride[0]*m_nDrawHeight, m_hRGB);
	}
#endif	

	
	// save the cgcontext gstate
	CGContextRef context = (CGContextRef)m_hView;
	CGContextSaveGState(context);
	
	
	//CGContextTranslateCTM(context, 0.0, m_nScreenHeight);
	//CGContextScaleCTM(context, 1.0, -1.0);
	
	int width	= m_nDrawWidth;
	int height	= m_nDrawHeight;

	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	CGContextRef bitmapContext = CGBitmapContextCreate(
													   m_pVideoBuf,
													   width,
													   height,
													   8, // bitsPerComponent
													   m_nVideoColor == VO_COLOR_ARGB32_PACKED ? 4*width:2*width, // bytesPerRow
													   colorSpace,
													   kCGImageAlphaNoneSkipLast);
	
	CFRelease(colorSpace);
	
	CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
	
	CGRect imageRect = {{m_nDrawLeft, m_nDrawTop}, {m_nDrawWidth, m_nDrawHeight}};
	
	CGContextAddRect(context, CGRectMake(m_nDrawLeft, m_nDrawTop, m_nScreenWidth, m_nDrawHeight));
	CGContextDrawImage(context, imageRect, cgImage);
	CGContextFlush(context);
	CGImageRelease(cgImage);
	
	// restore the cgcontext gstate
	CGContextRestoreGState(context);
	
	return 0;
}

VO_U32 	CCGRender::SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	return CBaseVideoRender::SetVideoInfo(nWidth, nHeight, nColor);;
}

VO_U32 	CCGRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect)
{
	//voCAutoLock lock (&m_csDraw);
	return CBaseVideoRender::SetDispRect(hView, pDispRect);
}

VO_U32 CCGRender::UpdateSize (void)
{
	int nRet = CBaseVideoRender::UpdateSize();
	
	if(m_pVideoBuf)
		delete m_pVideoBuf;
	
	if(m_nVideoColor == VO_COLOR_ARGB32_PACKED)
		m_pVideoBuf = new VO_BYTE[m_nDrawWidth*m_nDrawHeight*4];
	else
		m_pVideoBuf = new VO_BYTE[m_nDrawWidth*m_nDrawHeight*2];
	
	/*
	CGContextRef context = (CGContextRef)m_hView;
	if(context)
	{
		CGContextTranslateCTM(context, 0.0, m_nScreenHeight);
		CGContextScaleCTM(context, 1.0, -1.0);
	}
*/
	
	return nRet;
}

