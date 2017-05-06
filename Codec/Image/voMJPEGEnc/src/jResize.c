#include "jencoder.h"


INT32	YUV444pResize(VO_VIDEO_BUFFER* pindata, 
					  VO_VIDEO_BUFFER* poutdata, 
					  UINT32 oldWidth,
					  UINT32 oldHeight,
					  UINT32 newWidth,
					  UINT32 newHeight)
{
	register UINT32 i, j;
	UINT8 *Yout, *Uout, *Vout;
	UINT8 *Yin, *Uin, *Vin;	
	UINT32 XResution;
	UINT32 YResution;

	XResution = oldWidth  / newWidth;
	YResution = oldHeight / newHeight;

	Yout = poutdata->Buffer[0];
	Uout = poutdata->Buffer[1];
	Vout = poutdata->Buffer[2];

	Yin  = pindata->Buffer[0];
	Uin  = pindata->Buffer[1];
	Vin  = pindata->Buffer[2];

	for(i = 0; i < newHeight; i++)
	{
		for(j = 0; j < newWidth; j++)
		{
			Yout[j] = Yin[j * XResution];
			Uout[j] = Uin[j * XResution];
			Vout[j] = Vin[j * XResution];
		}
		Yout += poutdata->Stride[0];
		Uout += poutdata->Stride[1];
		Vout += poutdata->Stride[2];

		Yin += pindata->Stride[0] * YResution;
		Uin += pindata->Stride[1] * YResution;
		Vin += pindata->Stride[2] * YResution;
	}
	
	return 0;	
}


INT32	YUV444iResize(VO_VIDEO_BUFFER* pindata, 
					  VO_VIDEO_BUFFER* poutdata, 
					  UINT32 oldWidth,
					  UINT32 oldHeight,
					  UINT32 newWidth,
					  UINT32 newHeight)
{
	register UINT32 i, j, k;
	UINT8  *inBuffer;
	UINT8  *outBuffer;	
	UINT32 XResution;
	UINT32 YResution;

	XResution = oldWidth  / newWidth;
	YResution = oldHeight / newHeight;

	inBuffer = pindata->Buffer[0];
	outBuffer= poutdata->Buffer[0];

	for(i = 0; i < newHeight; i++)
	{
		for(j = 0; j < newWidth; j++)
		{
			k = j * 3;
			outBuffer[k    ] = inBuffer[k * XResution    ];
			outBuffer[k + 1] = inBuffer[k * XResution + 1];
			outBuffer[k + 2] = inBuffer[k * XResution + 2];
		}
		outBuffer += poutdata->Stride[0];
		inBuffer  += pindata->Stride[0] * YResution;
	}
	
	return 0;
}

//VO_VIDEO_BUFFER --> VO_VIDEO_BUFFER
//VOCODECDATABUFFER  --> VO_CODECBUFFER
INT32	YUV422pResize(VO_VIDEO_BUFFER* pindata, 
					  VO_VIDEO_BUFFER* poutdata, 
					  UINT32 oldWidth,
					  UINT32 oldHeight,
					  UINT32 newWidth,
					  UINT32 newHeight)
{
	register UINT32 i, j, k;
	UINT8 *Yout, *Uout, *Vout;
	UINT8 *Yin, *Uin, *Vin;
	UINT32 XResution;
	UINT32 YResution;

	XResution = oldWidth  / newWidth;
	YResution = oldHeight / newHeight;
	Yout = poutdata->Buffer[0];
	Uout = poutdata->Buffer[1];
	Vout = poutdata->Buffer[2];

	Yin  = pindata->Buffer[0];
	Uin  = pindata->Buffer[1];
	Vin  = pindata->Buffer[2];

	for(i = 0; i < newHeight; i++)
	{
		for(j = 0; j < newWidth >> 1; j++)
		{
			k = j << 1;
			Yout[k    ] = Yin[k * XResution     ];
			Yout[k + 1] = Yin[(k + 1)* XResution];

			Uout[j] = Uin[j * XResution];
			Vout[j] = Vin[j * XResution];
		}
		Yout += poutdata->Stride[0];
		Uout += poutdata->Stride[1];
		Vout += poutdata->Stride[2];
		
		Yin  += pindata->Stride[0] * YResution;
		Uin  += pindata->Stride[1] * YResution;
		Vin  += pindata->Stride[2] * YResution;
	}
	
	return 0;	
}

INT32	YUYV422iResize(VO_VIDEO_BUFFER* pindata, 
					   VO_VIDEO_BUFFER* poutdata, 
					   UINT32 oldWidth,
					   UINT32 oldHeight,
					   UINT32 newWidth,
					   UINT32 newHeight)
{
	register UINT32 i, j, k;
	UINT8  *inBuffer;
	UINT8  *outBuffer;	
	UINT32 XResution;
	UINT32 YResution;

	XResution = oldWidth  / newWidth;
	YResution = oldHeight / newHeight;
	inBuffer = pindata->Buffer[0];
	outBuffer= poutdata->Buffer[0];

	for(i = 0; i < newHeight; i++)
	{
		for(j = 0; j < newWidth >> 1; j++)
		{
			k = j << 2;
			outBuffer[k    ] = inBuffer[k * XResution    ];
			outBuffer[k + 1] = inBuffer[k * XResution + 1];
			outBuffer[k + 2] = inBuffer[(k + 2)*XResution];
			outBuffer[k + 3] = inBuffer[k * XResution + 3];
		}
		outBuffer += poutdata->Stride[0];
		inBuffer  += pindata->Stride[0] * YResution;
	}
	
	return 0;	
}

INT32	UYVY422iResize(VO_VIDEO_BUFFER* pindata, 
					   VO_VIDEO_BUFFER* poutdata, 
					   UINT32 oldWidth,
					   UINT32 oldHeight,
					   UINT32 newWidth,
					   UINT32 newHeight)
{
	register UINT32 i, j, k;
	UINT8  *inBuffer;
	UINT8  *outBuffer;	
	UINT32 XResution;
	UINT32 YResution;

	XResution = oldWidth  / newWidth;
	YResution = oldHeight / newHeight;

	inBuffer = pindata->Buffer[0];
	outBuffer= poutdata->Buffer[0];

	for(i = 0; i < newHeight; i++)
	{
		for(j = 0; j < newWidth >> 1; j++)
		{
			k = j << 2;
			outBuffer[k    ] = inBuffer[k * XResution    ];
			outBuffer[k + 1] = inBuffer[k * XResution + 1];
			outBuffer[k + 2] = inBuffer[k * XResution + 2];
			outBuffer[k + 3] = inBuffer[(k + 2)*XResution + 1];
		}
		outBuffer += poutdata->Stride[0];
		inBuffer  += pindata->Stride[0] * YResution;
	}
	
	return 0;		
}

INT32	YUV420pResize(VO_VIDEO_BUFFER* pindata, 
					  VO_VIDEO_BUFFER* poutdata, 
					  UINT32 oldWidth,
					  UINT32 oldHeight,
					  UINT32 newWidth,
					  UINT32 newHeight)
{
	register UINT32 i, j, k;
	UINT32 XResution;
	UINT32 YResution;
	UINT8  *Yin1, *Yin2;
	UINT8  *Uin, *Vin;
	UINT8  *Yout1, *Yout2;
	UINT8  *Uout, *Vout;


	XResution = oldWidth  / newWidth;
	YResution = oldHeight / newHeight;

	Yout1 = poutdata->Buffer[0];
	Yout2 = Yout1 + poutdata->Stride[0];
	Yin1 = pindata->Buffer[0];
	Yin2 = Yin1 + pindata->Stride[0] * YResution;
	Uin  = pindata->Buffer[1];
	Vin	 = pindata->Buffer[2];
	Uout = poutdata->Buffer[1];
	Vout = poutdata->Buffer[2];

	for(i = 0; i < newHeight >> 1; i++) //318
	{
		for(j = 0; j < newWidth >> 1; j++)
		{
			k = j << 1;
			Yout1[k    ] = Yin1[k * XResution     ];
			Yout1[k + 1] = Yin1[(k + 1)* XResution];
			Yout2[k    ] = Yin2[k * XResution     ];
			Yout2[k + 1] = Yin2[(k + 1)* XResution];

			Uout[j] = Uin[j * XResution];
			Vout[j] = Vin[j * XResution];
		}
		
		Yout1 += poutdata->Stride[0] << 1;
		Yout2 += poutdata->Stride[0] << 1;
		Uout += poutdata->Stride[1];
		Vout += poutdata->Stride[2];
		
		Yin1 += pindata->Stride[0] * YResution << 1;
		Yin2 += pindata->Stride[0] * YResution << 1;
		Uin  += pindata->Stride[1] * YResution;
		Vin  += pindata->Stride[2] * YResution;
	}
	
	return 0;		
}

INT32	YUV420iResize(VO_VIDEO_BUFFER* pindata, 
					  VO_VIDEO_BUFFER* poutdata, 
					  UINT32 oldWidth,
					  UINT32 oldHeight,
					  UINT32 newWidth,
					  UINT32 newHeight)
{
	register UINT32 i, j, k;
	UINT32 XResution;
	UINT32 YResution;
	UINT8  *Yin1, *Yin2;
	UINT8  *UVin;
	UINT8  *Yout1, *Yout2;
	UINT8  *UVout;


	XResution = oldWidth  / newWidth;
	YResution = oldHeight / newHeight;

	Yout1 = poutdata->Buffer[0];
	Yout2 = Yout1 + poutdata->Stride[0];
	Yin1 = pindata->Buffer[0];
	Yin2 = Yin1 + pindata->Stride[0] * YResution;
	UVin  = pindata->Buffer[1];
	UVout = poutdata->Buffer[1];

	for(i = 0; i < newHeight >> 1; i++)
	{
		for(j = 0; j < newWidth >> 1; j++)
		{
			k = j << 1;
			Yout1[k    ] = Yin1[k * XResution     ];
			Yout1[k + 1] = Yin1[(k + 1)* XResution];
			Yout2[k    ] = Yin2[k * XResution     ];
			Yout2[k + 1] = Yin2[(k + 1)* XResution];

			UVout[k] = UVin[k * XResution];
			UVout[k + 1] = UVin[(k + 1) * XResution];
		}
		
		Yout1 += poutdata->Stride[0] << 1;
		Yout2 += poutdata->Stride[0] << 1;
		UVout += poutdata->Stride[1];
		
		Yin1 += pindata->Stride[0] * YResution << 1;
		Yin2 += pindata->Stride[0] * YResution << 1;
		UVin += pindata->Stride[1] * YResution;
	}
	
	return 0;		
}

INT32	RGB565iResize(VO_VIDEO_BUFFER* pindata, 
					  VO_VIDEO_BUFFER* poutdata, 
					  UINT32 oldWidth,
					  UINT32 oldHeight,
					  UINT32 newWidth,
					  UINT32 newHeight)
{
	register UINT32 i, j, k;
	UINT8  *RGBIn, *RGBOut;
	UINT32 XResution;
	UINT32 YResution;

	XResution = oldWidth / newWidth;
	YResution = oldHeight/ newHeight;
	
	RGBIn  = pindata->Buffer[0];
	RGBOut = poutdata->Buffer[0];

	for(i = 0; i < newHeight; i++)
	{
		for(j = 0; j < newWidth; j++)
		{
			k = j << 1;
			RGBOut[k    ] = RGBIn[k * XResution    ];
			RGBOut[k + 1] = RGBIn[k * XResution + 1];
		}
		RGBOut += poutdata->Stride[0];
		RGBIn  += pindata->Stride[0] * YResution;
	}
	
	return 0;			
}