#include "Frame.h"
#include "bit.h"

/**< 16kbit/s 2bits per sample */
/**< 24kbit/s 3bits per sample */
/**< 32kbit/s 4bits per sample */
/**< 40kbit/s 5bits per sample */
static int nTable16_IQ[] ={ 
	0x74, 
	0x16D, 
	0x16D, 
	0x74
};
static int nTable24_IQ[] ={ 
	INT_MIN, 
	135, 
	273, 
	373, 
	373, 
	273, 
	135, 
	INT_MIN 
};
static int nTable32_IQ[] ={ 
	INT_MIN,   4, 
	135, 213, 
	273, 323, 
	373, 425,               
	425, 373, 
	323, 273, 
	213, 135,   
	4, INT_MIN 
};

static int nTable40_IQ[] ={ 
	INT_MIN, -66,  28, 104, 
	169, 224, 274, 318,
	358, 395, 429, 459, 
	488, 514, 539, 566,
	566, 539, 514, 488, 
	459, 429, 395, 358,
	318, 274, 224, 169, 
	104,  28, -66, INT_MIN 
};

static int nTable16_W[] ={ 
	-22, 
	439, 
	439, 
	-22 
};
static int nTable24_W[] ={ 
	-4,  
	30, 
	137, 
	582, 
	582, 
	137,  
	30, 
	-4 
};

static int nTable32_W[] ={ 
	-12,  18,  41,  64, 
	112, 198, 355, 1122,
	1122, 355, 198, 112,  
	64,  41,  18, -12
};

static int nTable40_W[] ={   
	14,  14,  24,  39,  
	40,  41,   58,  100,
	141, 179, 219, 280, 
	358, 440,  529,  696,
	696, 529, 440, 358, 
	280, 219,  179,  141,
	100,  58,  41,  40,  
	39,  24,   14,   14 
};


static int nTable16_F[] ={ 
	0, 
	7, 
	7, 
	0 
};

static int nTable24_F[] ={ 
	0, 
	1, 
	2, 
	7, 
	7, 
	2, 
	1, 
	0 
};

static int nTable32_F[] ={ 
	0, 0, 0, 1, 
	1, 1, 3, 7, 
	7, 3, 1, 1, 
	1, 0, 0, 0 
};

static int nTable40_F[] ={ 
	0, 0, 0, 0, 
	0, 1, 1, 1, 
	1, 1, 2, 3, 
	4, 5, 6, 6,
    6, 6, 5, 4, 
	3, 2, 1, 1, 
	1, 1, 1, 0, 
	0, 0, 0, 0 
};

ITU_G726Info *voADPCMDecInitITUG726Info(VO_MEM_OPERATOR *pMemOP, int avgbytespersec)
{
	int bitrate, i;
	ITU_G726Info *G726Info; 
	G726Info = (ITU_G726Info *)voADPCMDecmem_malloc(pMemOP, sizeof(ITU_G726Info), 32);
	if(G726Info == NULL)
		return NULL;

	bitrate = avgbytespersec*8;

	switch(bitrate)
	{
	case 16000:
		G726Info->iquant_tab = nTable16_IQ;
		G726Info->W_tab = nTable16_W;
		G726Info->F_tab = nTable16_F;
		break;
	case 24000:
		G726Info->iquant_tab = nTable24_IQ;
		G726Info->W_tab = nTable24_W;
		G726Info->F_tab = nTable24_F;
		break;
	case 32000:
		G726Info->iquant_tab = nTable32_IQ;
		G726Info->W_tab = nTable32_W;
		G726Info->F_tab = nTable32_F;
		break;
	case 40000:
		G726Info->iquant_tab = nTable40_IQ;
		G726Info->W_tab = nTable40_W;
		G726Info->F_tab = nTable40_F;
		break;
	}

	for (i=0; i<2; i++) {
        G726Info->sr[i] = 0;
        G726Info->a[i] = 0;
        G726Info->pk[i] = 1;
    }

    for (i=0; i<6; i++) {
        G726Info->dq[i] = 0;
        G726Info->b[i] = 0;
    }

    G726Info->ap = 0;
    G726Info->dms = 0;
    G726Info->dml = 0;
    G726Info->yu = 544;
    G726Info->yl = 34816;
    G726Info->td = 0;

	return G726Info;
}

static int step_size(ITU_G726Info *g726)
{
	int		y;
	int		dif;
	int		al;

	if (g726->ap >= 256)
		return (g726->yu);
	else {
		y = g726->yl >> 6;
		dif = g726->yu - y;
		al = g726->ap >> 2;
		if (dif > 0)
			y += (dif * al) >> 6;
		else if (dif < 0)
			y += (dif * al + 0x3F) >> 6;
		return (y);
	}
}

static __inline int gabs(int value)
{
	if(value < 0)
		return -value;
	else
		return value;
}

static int reconstruct(
	int		sign,	
	int		dqln,	
	int		y)	
{
	int		dql;	
	int		dex;	
	int		dqt;
	int		dq;	

	dql = dqln + (y >> 2);	

	if (dql < 0) {
		return (sign) ? -1 : 1;
	} else {		
		dex = (dql >> 7) & 15;
		dqt = 128 + (dql & 127);
		dq = ((dqt << 19) >> (14 - dex));
		return (sign) ? -dq : dq;
	}
}

static void update(
	int		code_size,	
	int		y,		
	int		wi,		
	int		fi,		
	int		dq,		
	int		sr,		
	int		dqsez,	
	ITU_G726Info *g726)	
{
	int		cnt;
	int		mag;		
	int		a2p=0;		
	int		a1ul;		
	int		pks1;		
	int		fa1;
	int		tr;			
	int		ylint, thr2, dqthr;
	int		ylfrac, thr1;
	int		pk0;

	pk0 = (dqsez < 0) ? 1 : 0;	

	mag = (dq / 0x1000);
	if(mag < 0)
		mag = -mag;

	ylint = g726->yl >> 15;	
	ylfrac = (g726->yl >> 10) & 0x1F;	
	thr1 = (32 + ylfrac) << ylint;		
	thr2 = (ylint > 9) ? 31 << 10 : thr1;	
	dqthr = (thr2 + (thr2 >> 1)) >> 1;
	if (g726->td == 0)		
		tr = 0;
	else if (mag <= dqthr)		
		tr = 0;			
	else				
		tr = 1;

	g726->yu = y + ((wi - y) >> 5);

	if (g726->yu < 544)	
		g726->yu = 544;
	else if (g726->yu > 5120)
		g726->yu = 5120;

	g726->yl += g726->yu + ((-g726->yl) >> 6);

	if (tr == 1) {			
		g726->a[0] = 0;
		g726->a[1] = 0;
		g726->b[0] = 0;
		g726->b[1] = 0;
		g726->b[2] = 0;
		g726->b[3] = 0;
		g726->b[4] = 0;
		g726->b[5] = 0;
	} else {			
		pks1 = pk0 ^ g726->pk[0];		

		a2p = g726->a[1] - (g726->a[1] >> 7);
		if (dqsez != 0) {
			fa1 = (pks1) ? g726->a[0] : -g726->a[0];
			if (fa1 < -8191)	
				a2p -= 0x100;
			else if (fa1 > 8191)
				a2p += 0xFF;
			else
				a2p += fa1 >> 5;

			if (pk0 ^ g726->pk[1])
				/* LIMC */
				if (a2p <= -12160)
					a2p = -12288;
				else if (a2p >= 12416)
					a2p = 12288;
				else
					a2p -= 0x80;
			else if (a2p <= -12416)
				a2p = -12288;
			else if (a2p >= 12160)
				a2p = 12288;
			else
				a2p += 0x80;
		}

		/* TRIGB & DELAY */
		g726->a[1] = a2p;

		/* UPA1 */
		/* update predictor pole a[0] */
		g726->a[0] -= g726->a[0] >> 8;
		if (dqsez != 0) {
			if (pks1 == 0)
				g726->a[0] += 192;
			else
				g726->a[0] -= 192;
		}
		/* LIMD */
		a1ul = 15360 - a2p;
		if (g726->a[0] < -a1ul)
			g726->a[0] = -a1ul;
		else if (g726->a[0] > a1ul)
			g726->a[0] = a1ul;

		/* UPB : update predictor zeros b[6] */
		for (cnt = 0; cnt < 6; cnt++) {
			if (code_size == 5)		/* for 40Kbps G.723 */
				g726->b[cnt] -= g726->b[cnt] >> 9;
			else			/* for G.721 and 24Kbps G.723 */
				g726->b[cnt] -= g726->b[cnt] >> 8;
			if (mag)
			{	/* XOR */
				if ((dq ^ g726->dq[cnt]) >= 0)
					g726->b[cnt] += 128;
				else
					g726->b[cnt] -= 128;
			}
		}
	}

	for (cnt = 5; cnt > 0; cnt--)
		g726->dq[cnt] = g726->dq[cnt-1];

	g726->dq[0] = dq;

	g726->sr[1] = g726->sr[0];
	g726->sr[0] = sr;

	/* DELAY A */
	g726->pk[1] = g726->pk[0];
	g726->pk[0] = pk0;

	/* TONE */
	if (tr == 1)		/* this sample has been treated as data */
		g726->td = 0;	/* next one will be treated as voice */
	else if (a2p < -11776)	/* small sample-to-sample correlation */
		g726->td = 1;	/* signal may be data */
	else				/* signal is voice */
		g726->td = 0;

	/*
	 * Adaptation speed control.
	 */
	g726->dms += (fi - g726->dms) >> 5;		/* FILTA */
	g726->dml += (((fi << 2) - g726->dml) >> 7);	/* FILTB */

	if (tr == 1)
		g726->ap = 256;
	else if (y < 1536)					/* SUBTC */
		g726->ap += (0x200 - g726->ap) >> 4;
	else if (g726->td == 1)
		g726->ap += (0x200 - g726->ap) >> 4;
	else if (gabs((g726->dms << 2) - g726->dml) >=
	    (g726->dml >> 3))
		g726->ap += (0x200 - g726->ap) >> 4;
	else
		g726->ap += (-g726->ap) >> 4;
}



static short G726Dec(ITU_G726Info *g726, int code)
{
	int64	i64;
	int		sezi, sez, se;	
	int		y;			
	int		sr;			
	int		dq;
	int		dqsez;
	int		i;

	i64 = 0;
	for(i = 0; i < 6; i++)
	{
		i64 += (int64)g726->b[i] * g726->dq[i];
	}

	sezi = (int)(i64 >> 14);
	sez = sezi;

	i64 = ((int64)g726->a[1] * g726->sr[1] +
	              (int64)g726->a[0] * g726->sr[0]); 
	se = sezi + (int)(i64 >> 14);
	
	y = step_size(g726);

	dq = reconstruct(code >> (g726->code_size - 1), g726->iquant_tab[code], y); 

	sr = se + dq;	
	dqsez = dq + sez;

	update(g726->code_size, y, g726->W_tab[code] << 5, g726->F_tab[code] << 9, dq, sr, dqsez, g726);

	return (sr >> 10);	
}

int voADPCMDecITUG726ADPCMDec(ADPCMDecInfo *decoder, short* samples)
{
	int code, nbits;
	int s, nsample;
	ITU_G726Info *G726Info; 
	Bitstream bitrp;
	unsigned char *buf;

	G726Info = (ITU_G726Info *)decoder->adpcm;

	buf = decoder->stream->this_frame;
	nbits = decoder->BitsPerSample;

	voADPCMDecInitBits(&bitrp, buf, decoder->stream->length);

	s = 0;
	if(G726Info->bits_left)
	{
		s =  nbits - G726Info->bits_left;
		code = (G726Info->bit_buffer << s) | voADPCMDecGetBits(&bitrp, s);
		*samples++ = G726Dec(G726Info, code);
	}

	nsample = ((decoder->stream->length) * 8 - s) / nbits;

	decoder->framesperblock = nsample;
	if(s) decoder->framesperblock++;

	while(nsample)
	{
		nsample--;
		*samples++ = G726Dec(G726Info, voADPCMDecGetBits(&bitrp, nbits));
	}

	G726Info->bits_left = bitrp.bitsleft;
	if(G726Info->bits_left)
		G726Info->bit_buffer = bitrp.cache >> (32 - G726Info->bits_left);
	else
		G726Info->bit_buffer = 0;	

	return 0;
}

