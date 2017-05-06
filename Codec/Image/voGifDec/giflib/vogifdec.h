
#define MIN(x,y) (x)<=(y)?(x):(y)

typedef struct tag_gifgce{
  unsigned char flags; /*res:3|dispmeth:3|userinputflag:1|transpcolflag:1*/
  unsigned short delaytime;
  unsigned char transpcolindex;
} struct_gifgce;

typedef struct gif_color_struct
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
   unsigned char resevered;

} gif_color;

typedef struct {
	GifRecordType recType;
    GifByteType *extData;
	GifFileType* gif;
    GifImageDesc desc;
	SavedImage temp_save;
	struct_gifgce gif_gce;
	FILE* fp;
	unsigned char*  bkgrdimage;
	int frames;
	int fill;
	int	prevdispmeth;
	int first_transpIndex;
	int transpIndex;   // -1 means we don't have it (yet)
	unsigned int devclr;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	void*  phCheck;
#ifdef TIME
	unsigned long timeused0;
	unsigned long timeused1;
	unsigned long timeused2;
#endif
}GifDecOBj;

static int DecodeCallBackProc(GifFileType* fileType, GifByteType* out,
                              int size) 
{
    FILE* fp = (FILE*) fileType->UserData;
	if (fp == NULL || fread(out,1,size,fp) != size) 
		return -1;
    return size;
}
static const unsigned char gStartingIterlaceYValue[] = {
    0, 4, 2, 1
};
static const unsigned char gDeltaIterlaceYValue[] = {
    8, 8, 4, 2
};

static int find_transpIndex(const SavedImage image, int colorCount) 
{
    int i, transpIndex = -1;
    for (i = 0; i < image.ExtensionBlockCount; ++i) {
        const ExtensionBlock* eb = image.ExtensionBlocks + i;
        if (eb->Function == 0xF9 && eb->ByteCount == 4) {
            if (eb->Bytes[0] & 1) {
                transpIndex = (unsigned char)eb->Bytes[3];
                // check for valid transpIndex
                if (transpIndex >= colorCount) {
                    transpIndex = -1;
                }
                break;
            }
        }
    }
    return transpIndex;
}

static const ColorMapObject* find_colormap(const GifFileType* gif) 
{
    const ColorMapObject* cmap = gif->Image.ColorMap;
    if (NULL == cmap) {
        cmap = gif->SColorMap;
    }
    // some sanity checks
    if ((unsigned)cmap->ColorCount > 256 ||
            cmap->ColorCount != (1 << cmap->BitsPerPixel)) {
        cmap = NULL;
    }
    return cmap;
}
void gifframemix(unsigned char* backgr,unsigned char* newframe,GifImageDesc desc,
				 unsigned int width,unsigned int height,int transpIndex)
{
	long y,x;
	unsigned char i2;
	int ibg2;

	long ymin = desc.Top;
	long ymax = MIN(height,desc.Top + desc.Height);
	long xmin = desc.Left;
	long xmax = MIN(width, (desc.Left + desc.Width));

	ibg2 =transpIndex; 
	for(y = desc.Top; y < ymax; y++)
	{
		for(x = desc.Left; x < xmax; x++)
		{
			i2 = newframe[y*width+x];
			if( i2 != ibg2 ) 
			{
				backgr[y*width+x] = i2;
			}
		}
	}
}
void gifframemix_new(unsigned char* backgr,unsigned char* newframe,GifImageDesc desc,
				 unsigned int width,unsigned int height,int transpIndex,gif_color *palette)
{
	long y,x;
	unsigned char i2;
	int ibg2;

	long ymin = desc.Top;
	long ymax = MIN(height,desc.Top + desc.Height);
	long xmin = desc.Left;
	long xmax = MIN(width, (desc.Left + desc.Width));

	unsigned char* backptr = NULL;
	unsigned char* newptr  = newframe;

	ibg2 =transpIndex; 

	for(y = ymin; y < ymax; y++)
	{
		backptr = backgr+y*width*3+xmin*3;
		for(x = xmin; x < xmax; x++)
		{
			i2 = newframe[y*width+x];
			if( i2 != ibg2 ) 
			{
				//backgr[y*width+x] = i2;
				*backptr++ = palette[i2].red;
				*backptr++ = palette[i2].green;
				*backptr++ = palette[i2].blue;
				 
			}
			else
			{
				backptr++;
				backptr++;
				backptr++;
			}
		}
	}
}

void gifconvertrgb(unsigned char* inbuf,unsigned char* outbuf,
				 unsigned int width,unsigned int height,gif_color *palette,int transpIndex)
{
	int i,j;
	unsigned char* outptr = outbuf;
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			unsigned char index = inbuf[i*width+j];
			if(index == transpIndex)
			{
				*outptr++ = 255;
				*outptr++ = 255;
				*outptr++ = 255;
			}
			else
			{
				unsigned char r = palette[index].red;
				unsigned char g = palette[index].green;
				unsigned char b = palette[index].blue;

				*outptr++ = r;
				*outptr++ = g;
				*outptr++ = b;
			}
		}
	}
	return;
}