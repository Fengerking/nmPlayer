#include "pbdll.h"
#include "voVP6back.h"

YV12_BUFFER_CONFIG*  buf_seq_ctl(FIFOTYPE *priv, YV12_BUFFER_CONFIG* img , const UINT32 flag)
{
    YV12_BUFFER_CONFIG *temp = NULL;

    if (flag == FIFO_WRITE){ /*write data*/

        /* Save it to buffer */
        if (((priv->w_idx + 1) % FIFO_NUM) == priv->r_idx) {
            /* Adjust read index since buffer is full */
            /* Keep the latest one and drop the oldest one */
            priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
        }
        priv->img_seq[priv->w_idx] = img;
        priv->w_idx = (priv->w_idx + 1) % FIFO_NUM;

    }else{/*read data*/
		if ((priv->r_idx == priv->w_idx)){
            return NULL;/*there is no free buffer*/
		}else{
            temp = priv->img_seq[priv->r_idx];
            priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
            }
        }
     return temp;
}

/* After flush*/
void  reset_buf_seq(PB_INSTANCE *dec)
{
	FIFOTYPE *priv = &dec->frame_buf_fifo; 

	UINT32 i, w_idx, r_idx;

	for(i = 0; i < dec->frame_buf_num; i++){
		w_idx = priv->w_idx;
		r_idx = priv->r_idx;

		do{
			if(priv->img_seq[r_idx] == &dec->FrameYV12Config[i])
				break;
			r_idx = (r_idx + 1) % FIFO_NUM;
		}while(r_idx != w_idx);

		if(r_idx == w_idx)
			buf_seq_ctl(priv, &dec->FrameYV12Config[i], FIFO_WRITE);
	}
}