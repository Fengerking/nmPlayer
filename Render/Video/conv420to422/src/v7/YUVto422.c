void YUV420_YUYV422_C(unsigned char *  Dst, unsigned char *  Y, unsigned char *  U, unsigned char *  V, int YStride, int UVStride, int w, int h, int OutStride)
{
  unsigned char *Des, *Des1, *Ybuf, *Ybuf1, *Ubuf, *Vbuf;
  int i, j;
  for (i=0; i<h; i+=2) 
  {
	Des = Dst;	Des1 = Dst + (OutStride<<1);
	Ybuf = Y;	Ybuf1 = Y + YStride;     
	Ubuf = U;                              
	Vbuf = V;                                      
	for (j = 0; j < w; j+=2) 
	{
	  *Des++ = *Ybuf++;
	  *Des++ = *Ubuf;
	  *Des++ = *Ybuf++;
	  *Des++ = *Vbuf;
	
	  *Des1++ = *Ybuf1++;
	  *Des1++ = *Ubuf++;
	  *Des1++ = *Ybuf1++;
	  *Des1++ = *Vbuf++;
	}
	Dst += (OutStride<<2);
	Y += (YStride<<1);
	U += UVStride;
	V += UVStride;
  }
}
 
void YUV420_UYVY422_C(unsigned char *  Dst, unsigned char *  Y, unsigned char *  U, unsigned char *  V, int YStride, int UVStride, int w, int h, int OutStride)
{
  unsigned char *Des, *Des1, *Ybuf, *Ybuf1, *Ubuf, *Vbuf;
  int i, j;
  for (i=0; i<h; i+=2) 
  {
	Des = Dst;	Des1 = Dst + (OutStride<<1);
	Ybuf = Y;	Ybuf1 = Y + YStride;     
	Ubuf = U;                              
	Vbuf = V;                                      
	for (j = 0; j < w; j+=2) 
	{
        *Des++ = *Ubuf;
        *Des++ = *Ybuf++;
        *Des++ = *Vbuf;
        *Des++ = *Ybuf++;

        *Des1++ = *Ubuf++;
        *Des1++ = *Ybuf1++;
        *Des1++ = *Vbuf++;
        *Des1++ = *Ybuf1++;
	}
	Dst += (OutStride<<2);
	Y += (YStride<<1);
	U += UVStride;
	V += UVStride;
  }
} 

void YUV420_YUYV422(unsigned char *  Dst, unsigned char *  Y, unsigned char *  U, unsigned char *  V, int YStride, int UVStride, int w, int h, int OutStride)
{

  if(!(w%32))
  {
  	YUV420_YUYV422_32(Dst, Y, U, V, YStride, UVStride, w, h, OutStride);
  }
  else
  {
	int w_offset;
	w_offset = w%16;
	w = w - w_offset;
  	YUV420_YUYV422_16(Dst, Y, U, V, YStride, UVStride, w, h, OutStride);
  	if(w_offset)
  	{
  	 Dst = Dst + (w<<1);
  	 Y = Y + w;
  	 U = U + (w>>1);
  	 V = V + (w>>1);  	   	 	
  	 YUV420_YUYV422_C(Dst, Y, U, V, YStride, UVStride, w_offset, h, OutStride);
  	} 	 	
  		
  }
}
void YUV420_UYVY422(unsigned char *  Dst, unsigned char *  Y, unsigned char *  U, unsigned char *  V, int YStride, int UVStride, int w, int h, int OutStride)
{

  if(!(w%32))
  {
  	YUV420_UYVY422_32(Dst, Y, U, V, YStride, UVStride, w, h, OutStride);
  }
  else
  {
	int w_offset;
	w_offset = w%16;
	w = w - w_offset;
  	YUV420_UYVY422_16(Dst, Y, U, V, YStride, UVStride, w, h, OutStride);
  	if(w_offset)
  	{
  	 Dst = Dst + (w<<1);
  	 Y = Y + w;
  	 U = U + (w>>1);
  	 V = V + (w>>1);  	   	 	
  	 YUV420_UYVY422_C(Dst, Y, U, V, YStride, UVStride, w_offset, h, OutStride);
  	} 	 	
  		
  }
}