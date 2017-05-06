#define IdctAdjustBeforeShift 8

#define xC1S7 64277
#define xC2S6 60547
#define xC3S5 54491
#define xC4S4 46341
#define xC5S3 36410
#define xC6S2 25080
#define xC7S1 12785
#define M(a,b) (((a) * (b))>>16)
#define SAT(Value) Value < 0 ? 0: (Value > 255 ? 255: Value);
void IDCTvp6_Block8x8_c(INT16 *input, UINT8 *dst, INT32 DestStride, UINT8 *src, INT32 stride)
{
    INT16 *ip = input;

    INT32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
    INT32 Ed, Gd, Add, Bdd, Fd, Hd;

    INT32 i, v;

    /* Inverse DCT on the rows now */
    for (i = 0; i < 8; i++) {
        /* Check for non-zero values */
        if ( ip[0] | ip[1] | ip[2] | ip[3] | ip[4] | ip[5] | ip[6] | ip[7] ) {
            A = M(xC1S7, ip[1]) + M(xC7S1, ip[7]);
            B = M(xC7S1, ip[1]) - M(xC1S7, ip[7]);
            C = M(xC3S5, ip[3]) + M(xC5S3, ip[5]);
            D = M(xC3S5, ip[5]) - M(xC5S3, ip[3]);

            Ad = M(xC4S4, (A - C));
            Bd = M(xC4S4, (B - D));

            Cd = A + C;
            Dd = B + D;

            E = M(xC4S4, (ip[0] + ip[4]));
            F = M(xC4S4, (ip[0] - ip[4]));

            G = M(xC2S6, ip[2]) + M(xC6S2, ip[6]);
            H = M(xC6S2, ip[2]) - M(xC2S6, ip[6]);

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            /*  Final sequence of operations over-write original inputs. */
            ip[0] = Gd + Cd ;
            ip[7] = Gd - Cd ;

            ip[1] = Add + Hd;
            ip[2] = Add - Hd;

            ip[3] = Ed + Dd ;
            ip[4] = Ed - Dd ;

            ip[5] = Fd + Bdd;
            ip[6] = Fd - Bdd;
        }

        ip += 8;            /* next row */
    }




////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////


////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////




////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////





    ip = input;

    for ( i = 0; i < 8; i++) {
        /* Check for non-zero values (bitwise or faster than ||) */
        if ( ip[1 * 8] | ip[2 * 8] | ip[3 * 8] |
             ip[4 * 8] | ip[5 * 8] | ip[6 * 8] | ip[7 * 8] ) {

            A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8]);
            B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8]);
            C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8]);
            D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8]);

            Ad = M(xC4S4, (A - C));
            Bd = M(xC4S4, (B - D));

            Cd = A + C;
            Dd = B + D;

            E = M(xC4S4, (ip[0*8] + ip[4*8]));
            F = M(xC4S4, (ip[0*8] - ip[4*8]));

            if(!src)
			{  //HACK
                E += (16*128 + 8);
                F += (16*128 + 8);
            }
			else
			{  //HACK
                E += 8;
                F += 8;
            }

            G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8]);
            H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8]);

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

   	if(!src){
                dst[0*DestStride] = SAT(((Gd + Cd )  >> 4));
                dst[7*DestStride] = SAT(((Gd - Cd )  >> 4));

                dst[1*DestStride] = SAT(((Add + Hd ) >> 4));
                dst[2*DestStride] = SAT(((Add - Hd ) >> 4));

                dst[3*DestStride] = SAT(((Ed + Dd )  >> 4));
                dst[4*DestStride] = SAT(((Ed - Dd )  >> 4));

                dst[5*DestStride] = SAT(((Fd + Bdd ) >> 4));
                dst[6*DestStride] = SAT(((Fd - Bdd ) >> 4));
            }else{
                dst[0*DestStride] = SAT((src[0*stride] + ((Gd + Cd )  >> 4)));
                dst[7*DestStride] = SAT((src[7*stride] + ((Gd - Cd )  >> 4)));

                dst[1*DestStride] = SAT((src[1*stride] + ((Add + Hd ) >> 4)));
                dst[2*DestStride] = SAT((src[2*stride] + ((Add - Hd ) >> 4)));

                dst[3*DestStride] = SAT((src[3*stride] + ((Ed + Dd )  >> 4)));
                dst[4*DestStride] = SAT((src[4*stride] + ((Ed - Dd )  >> 4)));

                dst[5*DestStride] = SAT((src[5*stride] + ((Fd + Bdd ) >> 4)));
                dst[6*DestStride] = SAT((src[6*stride] + ((Fd - Bdd ) >> 4)));
				src++;
            }

        } else {
            if(!src){
				v = ((xC4S4 * ip[0*8] + (IdctAdjustBeforeShift<<16))>>20);
                dst[0*DestStride]=
                dst[1*DestStride]=
                dst[2*DestStride]=
                dst[3*DestStride]=
                dst[4*DestStride]=
                dst[5*DestStride]=
                dst[6*DestStride]=
                dst[7*DestStride]= SAT((128 + v));
            }else{
				if(ip[0*8])
				{
					v = ((xC4S4 * ip[0*8] + (IdctAdjustBeforeShift<<16))>>20);
                    dst[0*DestStride] = SAT((src[0*stride] + v));
                    dst[1*DestStride] = SAT((src[1*stride] + v));
                    dst[2*DestStride] = SAT((src[2*stride] + v));
                    dst[3*DestStride] = SAT((src[3*stride] + v));
                    dst[4*DestStride] = SAT((src[4*stride] + v));
                    dst[5*DestStride] = SAT((src[5*stride] + v));
                    dst[6*DestStride] = SAT((src[6*stride] + v));
                    dst[7*DestStride] = SAT((src[7*stride] + v));
				}
				else
				{
                    dst[0*DestStride] = src[0*stride];
                    dst[1*DestStride] = src[1*stride];
                    dst[2*DestStride] = src[2*stride];
                    dst[3*DestStride] = src[3*stride];
                    dst[4*DestStride] = src[4*stride];
                    dst[5*DestStride] = src[5*stride];
                    dst[6*DestStride] = src[6*stride];
                    dst[7*DestStride] = src[7*stride];
				}
				src++;
            }
        }

        ip++;            /* next column */
        dst++;
    }
	memset(input, 0, 64*sizeof(Q_LIST_ENTRY));
}
