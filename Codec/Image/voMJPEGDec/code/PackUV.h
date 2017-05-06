

#define SAT(Value) (Value) < 0 ? 0: ((Value) > 255 ? 255: (Value));
void cc_yuv420torgb24_c_16bit(unsigned char *psrc_y, unsigned char *psrc_u, unsigned char *psrc_v, const signed long in_stridey, const signed long in_strideu, const signed long in_stridev, 
				        unsigned char *out_buf, const signed long out_stride, signed long width, signed long height);
