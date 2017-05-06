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
        if ( ip[0*8] | ip[1*8] | ip[2*8] | ip[3*8] | ip[4*8] | ip[5*8] | ip[6*8] | ip[7*8] ) {
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

            G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8]);
            H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8]);

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            /*  Final sequence of operations over-write original inputs. */
            ip[0*8] = Gd + Cd ;
            ip[7*8] = Gd - Cd ;

            ip[1*8] = Add + Hd;
            ip[2*8] = Add - Hd;

            ip[3*8] = Ed + Dd ;
            ip[4*8] = Ed - Dd ;

            ip[5*8] = Fd + Bdd;
            ip[6*8] = Fd - Bdd;
        }

        ip += 1;            /* next row */
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
        if ( ip[1] | ip[2] | ip[3] | ip[4] | ip[5] | ip[6] | ip[7] ) {
            A = M(xC1S7, ip[1]) + M(xC7S1, ip[7]);
            B = M(xC7S1, ip[1]) - M(xC1S7, ip[7]);
            C = M(xC3S5, ip[3]) + M(xC5S3, ip[5]);
            D = M(xC3S5, ip[5]) - M(xC5S3, ip[3]);

            Ad = M(xC4S4, (A - C));
            Bd = M(xC4S4, (B - D));

            Cd = A + C;
            Dd = B + D;

            E = M(xC4S4, (ip[0] + ip[4])) + 8;
            F = M(xC4S4, (ip[0] - ip[4])) + 8;

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

            G = M(xC2S6, ip[2]) + M(xC6S2, ip[6]);
            H = M(xC6S2, ip[2]) - M(xC2S6, ip[6]);

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

   	if(!src){
                dst[0] = SAT(((Gd + Cd )  >> 4));
                dst[7] = SAT(((Gd - Cd )  >> 4));

                dst[1] = SAT(((Add + Hd ) >> 4));
                dst[2] = SAT(((Add - Hd ) >> 4));

                dst[3] = SAT(((Ed + Dd )  >> 4));
                dst[4] = SAT(((Ed - Dd )  >> 4));

                dst[5] = SAT(((Fd + Bdd ) >> 4));
                dst[6] = SAT(((Fd - Bdd ) >> 4));
            }else{
                dst[0] = SAT((src[0] + ((Gd + Cd )  >> 4)));
                dst[7] = SAT((src[7] + ((Gd - Cd )  >> 4)));

                dst[1] = SAT((src[1] + ((Add + Hd ) >> 4)));
                dst[2] = SAT((src[2] + ((Add - Hd ) >> 4)));

                dst[3] = SAT((src[3] + ((Ed + Dd )  >> 4)));
                dst[4] = SAT((src[4] + ((Ed - Dd )  >> 4)));

                dst[5] = SAT((src[5] + ((Fd + Bdd ) >> 4)));
                dst[6] = SAT((src[6] + ((Fd - Bdd ) >> 4)));
				src += stride;
            }

        } else {
            if(!src){
				v = ((xC4S4 * ip[0] + (IdctAdjustBeforeShift<<16))>>20);
                dst[0]=
                dst[1]=
                dst[2]=
                dst[3]=
                dst[4]=
                dst[5]=
                dst[6]=
                dst[7]= SAT((128 + v));
            }else{
				
                if(ip[0])
		{
                    v= ((xC4S4 * ip[0] + (IdctAdjustBeforeShift<<16))>>20);
                    dst[0] = SAT((src[0] + v));
                    dst[1] = SAT((src[1] + v));
                    dst[2] = SAT((src[2] + v));
                    dst[3] = SAT((src[3] + v));
                    dst[4] = SAT((src[4] + v));
                    dst[5] = SAT((src[5] + v));
                    dst[6] = SAT((src[6] + v));
                    dst[7] = SAT((src[7] + v));
		}
		else
		{
                    dst[0] = src[0];
                    dst[1] = src[1];
                    dst[2] = src[2];
                    dst[3] = src[3];
                    dst[4] = src[4];
                    dst[5] = src[5];
                    dst[6] = src[6];
                    dst[7] = src[7];
		}
		src += stride;
            }
        }

        ip += 8;            /* next column */
        dst += DestStride;
    }
	memset(input, 0, 64*sizeof(Q_LIST_ENTRY));
}
