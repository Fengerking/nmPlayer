/******************************************************************************/
/*  DMA55XX.H - DMA55xx routines header file.                                 */
/*                                                                            */
/*     This module provides the devlib implementation for the DMAC            */
/*     on the TMS320C55XX DSP.                                                */
/*                                                                            */
/*  MACRO FUNCTIONS:                                                          */
/*     DMA_ENABLE 		- enable selected DMA ch                              */                      
/*     DMA_DISABLE 		- disable selected DMA ch                             */
/*     DMA_AUTO_ENABLE 		- enable selected DMA ch with auto-init           */
/*     DMA_FREE_RUNNING 	- enable DMA free running                         */
/*     DMA_NO_FREE_RUNNING 	- disable DMA free running                        */
/*     DMA_FRAMECOUNT 		- set number of frames in multi-frame transfer    */
/*     DMA_INTMASK_ENABLE 	- enable DMA interrupt                            */
/*     DMA_INTMASK_DISABLE 	- disable DMA interrupt                           */
/*     DMA_DMS_SELECT 		- set Source Space Select                         */
/*     DMA_DMD_SELECT 		- set Destination Space Select                    */
/*                                                                            */
/*  FUNCTIONS:                                                                */
/*     dma_init 		 -  Initialize ch specific control registers          */
/*     dma_global_init() - Initialize global control registers.               */
/*     dma_reset_all()       - Resets indicated DMA ch                        */
/*                                                                            */
/*                                                                            */
/*  AUTHOR:                                                                   */
/*     Stefan Haas                                                            */
/*                                                                            */
/*  REVISION HISTORY:                                                         */
/*                                                                            */
/*    DATE       AUTHOR                       DESCRIPTION                     */
/*   -------   -------------      ------------------------------------------  */
/*   13OCT98   St Haas            Original.                                   */
/*                                                                            */
/******************************************************************************/
#ifndef _DMA55X_H_
#define _DMA55X_H_

#include "regs55x.h"

/******************************************************************************/
/* Register Definition for 55x DMA      									  */
/******************************************************************************/
#define DMGCR_ADDR			0x0e00
#define DMGCR				*(ioport volatile unsigned int *)DMGCR_ADDR

#define DMCSDP_ADDR(port)	(0x0c00+(port * 0x20))
#define DMCSDP(port)    	*(ioport volatile unsigned int *)DMCSDP_ADDR(port)

#define DMCCR_ADDR(port)    (DMCSDP_ADDR(port)+1)
#define DMCCR(port)    		*(ioport volatile unsigned int *)DMCCR_ADDR(port)

#define DMCICR_ADDR(port)   (DMCCR_ADDR(port)+1)
#define DMCICR(port)    	*(ioport volatile unsigned int *)DMCICR_ADDR(port)

#define DMCSR_ADDR(port)    (DMCSDP_ADDR(port)+3)
#define DMCSR(port)    		*(ioport volatile unsigned int *)DMCSR_ADDR(port)

#define DMCSSAL_ADDR(port)  (DMCSDP_ADDR(port)+4)
#define DMCSSAL(port)    	*(ioport volatile unsigned int *)DMCSSAL_ADDR(port)

#define DMCSSAU_ADDR(port)  (DMCSDP_ADDR(port)+5)
#define DMCSSAU(port)    	*(ioport volatile unsigned int *)DMCSSAU_ADDR(port)

#define DMCDSAL_ADDR(port)  (DMCSDP_ADDR(port)+6)
#define DMCDSAL(port)    	*(ioport volatile unsigned int *)DMCDSAL_ADDR(port)

#define DMCDSAU_ADDR(port)  (DMCSDP_ADDR(port)+7)
#define DMCDSAU(port)    	*(ioport volatile unsigned int *)DMCDSAU_ADDR(port)

#define DMCEN_ADDR(port)    (DMCSDP_ADDR(port)+8)
#define DMCEN(port)    		*(ioport volatile unsigned int *)DMCEN_ADDR(port)

#define DMCFN_ADDR(port)    (DMCSDP_ADDR(port)+9)
#define DMCFN(port)    		*(ioport volatile unsigned int *)DMCFN_ADDR(port)

#define DMCFI_ADDR(port)    (DMCSDP_ADDR(port)+0xa)
#define DMCFI(port)    		*(ioport volatile unsigned int *)DMCFI_ADDR(port)

#define DMCEI_ADDR(port)    (DMCSDP_ADDR(port)+0xb)
#define DMCEI(port)    		*(ioport volatile unsigned int *)DMCEI_ADDR(port)

/******************************************************************************/
/*  DMA   Registers, Bits, Bitfields										  */
/******************************************************************************/
//DMGCR
#define DM_EHPI_PRIO		0
#define DM_EHPI_PRIO_SZ		1

#define DM_EHPI_EXCL		1
#define DM_EHPI_EXCL_SZ		1

#define DM_FREE				2
#define DM_FREE_SZ			1

//DMCSDP
#define DM_DATA_TYPE		0
#define DM_DATA_TYPE_SZ		2

#define DM_SRC				2
#define DM_SRC_SZ			4

#define DM_SRC_PACK			6
#define DM_SRC_PACK_SZ		1

#define DM_SRC_BEN			7
#define DM_SRC_BEN_SZ		2

#define DM_DST				9
#define DM_DST_SZ			4

#define DM_DST_PACK			13
#define DM_DST_PACK_SZ		1

#define DM_DST_BEN			14
#define DM_DST_BEN_SZ		2

//DMCCR
#define DM_SYNC				0
#define DM_SYNC_SZ			5

#define DM_FS				5
#define DM_FS_SZ			1

#define DM_PRIO				6
#define DM_PRIO_SZ			1

#define DM_EN				7
#define DM_EN_SZ			1

#define DM_AUTOINIT			8
#define DM_AUTOINIT_SZ		1

#define DM_REPEAT			9
#define DM_REPEAT_SZ		1

#define DM_FIFO_FLUSH		10
#define DM_FIFO_FLUSH_SZ	1

#define DM_END_PROG			11
#define DM_END_PROG_SZ		1

#define DM_SRC_AMODE		12
#define DM_SRC_AMODE_SZ		2

#define DM_DST_AMODE		14
#define DM_DST_AMODE_SZ		2

//DMCICR/DMCSR
#define DM_TIMEOUT			0
#define DM_TIMEOUT_SZ		1

#define DM_DROP				1
#define DM_DROP_SZ			1

#define DM_1STHALF			2
#define DM_1STHALF_SZ		1

#define DM_FRAME			3
#define DM_FRAME_SZ			1

#define DM_LAST				4
#define DM_LAST_SZ			1

#define DM_BLOCK			5
#define DM_BLOCK_SZ			1

/******************************************************************************/
/* CONFIGURATION REGISTER BIT and BITFIELD values                   		  */
/******************************************************************************/
//DMGCR
#define DM_EHPI_PRIO_LOW    0x00     //Lowest priority for EHPI DMA ch
#define DM_EHPI_PRIO_HIGH  	0x01     //Highest priority for EHPI DMA ch

#define DM_EXCL_ON			0x00     //EHPI has exclusive on-chip RAM access
#define DM_EXCL_OFF	  		0x01     //EHPI does not have exclusive access

#define DM_FREE_OFF			0x00     //DMA stops after the DMA xfer complete
#define DM_FREE_ON			0x01     //DMA runs free, even if device halted

//DMCSDP
#define DM_DTYPE_8          0x00     //8-bit data
#define DM_DTYPE_16         0x01     //16-bit data
#define DM_DTYPE_32         0x02     //32-bit data

#define DM_SARAM			0x0000   //src/dst port is SARAM
#define DM_DARAM			0x0001   //src/dst port is DARAM
#define DM_EMIF				0x0002   //src/dst port is EMIF
#define DM_RHEA				0x0003   //src/dst port is RHEA

#define DM_PACK_ON			0x01	 //pack data 
#define DM_PACK_OFF			0x00	 //do not pack data 

#define DM_NO_BURST			0x00	 //Single access (no burst)
#define DM_BURST4			0x02	 //burst of 4

//DMCCR
#define DMSYNC_OFF			0x00	//No event synchronized

#define DMSYNC_REVT0        0x01   //McBsp 0 Rx Event
#define DMSYNC_XEVT0        0x02   //McBsp 0 Tx Event
#define DMSYNC_REVTA0       0x03   //McBsp 0 Rx EventA
#define DMSYNC_XEVTA0       0x04   //McBsp 0 Tx EventA

#define DMSYNC_REVT1        0x05   //McBsp 1 Rx Event
#define DMSYNC_XEVT1        0x06   //McBsp 1 Tx Event
#define DMSYNC_REVTA1       0x07   //McBsp 1 Rx EventA
#define DMSYNC_XEVTA1       0x08   //McBsp 1 Tx EventA

#define DMSYNC_REVT2        0x09   //McBsp 2 Rx Event
#define DMSYNC_XEVT2        0x0a   //McBsp 2 Tx Event
#define DMSYNC_REVTA2       0x0b   //McBsp 2 Rx EventA
#define DMSYNC_XEVTA2       0x0c   //McBsp 2 Tx EvenA

#define DMSYNC_TIMER1       0x0d   //Timer 1 Event
#define DMSYNC_TIMER2       0x0e   //Timer 2 Event

#define DMSYNC_EXT0         0x0f   //External Ineterrupt 0
#define DMSYNC_EXT1         0x10   //External Ineterrupt 0
#define DMSYNC_EXT2         0x11   //External Ineterrupt 0
#define DMSYNC_EXT3         0x12   //External Ineterrupt 0
#define DMSYNC_EXT4         0x13   //External Ineterrupt 0
#define DMSYNC_EXT5         0x14   //External Ineterrupt 0

#define DM_FS_ON			0x00   //Double word transfer mode is disabled
#define DM_FS_OFF			0x01   //Double word transfer mode is enabled
                            
#define DM_PRIO_HIGH		0x01   //Channel has high priority
#define DM_PRIO_LOW			0x00   //Channel has low priority
                            
#define DM_ENABLE   		0x01   //Start transfer
#define DM_DISABLE			0x00   //Disable transfer
                            
#define DM_AUTOINIT_ON 		0x01   //Auto-initialization enabled
#define DM_AUTOINIT_OFF 	0x00   //Auto-initialization enabled disabled
                            
#define DM_REPEAT_ON 		0x01   //Auto-initialization regardless of DM_END_PROG
#define DM_REPEAT_OFF 		0x00   //Auto-initialization only if DM_END_PROG=1

#define DM_FIFO_FLUSH_ON 	0x01   //FIFO is flushed
#define DM_FIFO_FLUSH_OFF	0x00   //No flushing occurs

#define DM_END_PROG_ON 		0x01   //Allows the ch to re-init itself if DM_AUTOINIT=1
#define DM_END_PROG_OFF		0x00   //Delays the ch re-init if DM_AUTOINIT=1

#define DM_NOMOD 			0x00   //Constant address
#define DM_POST_INCR 		0x01   //Increment address
#define DM_INDX_INCR		0x02   //Increment address by element index
#define DM_FRM_INDX_INCR 	0x03   //Increment address by element and frame index

                            
//DMCICR
#define DMINTR_TIMEOUT 		(1<<DM_TIMEOUT)	//Interrupt if timeout
#define DMINTR_DROP  		(1<<DM_DROP)	//Interrupt if timeout
#define DMINTR_1STHALF 		(1<<DM_1STHALF)	//Interrupt if timeout
#define DMINTR_FRAME  		(1<<DM_FRAME)	//Interrupt if timeout
#define DMINTR_LAST  		(1<<DM_LAST)	//Interrupt if timeout
#define DMINTR_BLOCK  		(1<<DM_BLOCK)	//Interrupt if timeout

//DMA channels
#define DMCH0				0x00	// DMA Channel 0                         
#define DMCH1				0x01    // DMA Channel 1                         
#define DMCH2				0x02    // DMA Channel 2                         
#define DMCH3				0x03    // DMA Channel 3                         
#define DMCH4				0x04    // DMA Channel 4                         
#define DMCH5				0x05    // DMA Channel 5                           

/****************************************************************************/
/* MACRO DEFINITIONS                                                        */
/****************************************************************************/
                     
/****************************************************************************/
/* DMA_ENABLE - enable selected DMA ch								 	    */
/****************************************************************************/
#define DMA_ENABLE(ch) \
		DMCCR(ch) = DMCCR(ch) | (1<<DM_EN)


/****************************************************************************/
/* DMA_DISABLE - disable selected DMA ch								    */
/****************************************************************************/
#define DMA_DISABLE(ch) \
		DMCCR(ch) &= ~(1<<DM_EN)


/****************************************************************************/
/* DMA_AUTO_ENABLE - enable selected DMA ch with auto-init				    */
/****************************************************************************/
#define DMA_AUTO_ENABLE(ch) \
	   {DMCCR_ADDR(ch) |= ((1<<DM_AUTOINIT) | (1<<DM_END_PROG));\
		DMA_ENABLE(ch);}
		
/****************************************************************************/
/* DMA_FREE_RUNNING - enable DMA free running								*/
/****************************************************************************/
#define DMA_FREE_RUNNING \
		DMCCR(ch) |= (1<<DM_FREE)


/****************************************************************************/
/* DMA_NO_FREE_RUNNING - disable DMA free running							*/
/****************************************************************************/
#define DMA_NO_FREE_RUNNING \
		DMCCR(ch) &= ~(1<<DM_FREE)


/****************************************************************************/
/* DMA_FRAMECOUNT - set number of frames in multi-frame transfer			*/
/****************************************************************************/
#define DMA_FRAME_COUNT(ch, value) \
        DMCFN(ch) = value
	
/****************************************************************************/
/* DMA_FRAMECOUNT - set number of frames in multi-frame transfer			*/
/****************************************************************************/
#define DMA_ELEMENT_COUNT(ch, value) \
        DMCEN(ch) = value

/****************************************************************************/
/* DMA_FRAME_INDEX - set frame index in multi-frame transfer	 		    */
/****************************************************************************/
#define DMA_FRAME_INDEX(ch, value) \
        DMCFI(ch) = value
	
/****************************************************************************/
/* DMA_ELEMENT_INDEX - set element index            						*/
/****************************************************************************/
#define DMA_ELEMENT_INDEX(ch, value) \
        DMCEI_ADDR(ch) = value
	
/******************************************************************/
/* Structure definitions                                          */
/******************************************************************/
typedef struct _Dma55xConfig
{
	int ch;						//DMA channel
    unsigned int  csdp;			//DMCSDP
    unsigned int  ccr;			//DMCCR
    unsigned int  cicr;			//CICR
    unsigned int  srcl;			//src address (LSW)
    unsigned int  srch;			//src address (MSW)
    unsigned int  dstl;			//dst address (LSW)
    unsigned int  dsth;			//src address (MSW)
    unsigned int  elem;			//element count
    unsigned int  frame;		//frame count
    unsigned int  framei;		//frame index in multi-frame transfer
    unsigned int  elemi;		//element index
} Dma55xConfig, *PDma55xConfig;

/****************************************************************************/
/* FUNCTION DEFINITIONS                                                     */
/****************************************************************************/

/****************************************************************************/
/* dma_init -  Initialize ch specific control registers.					*/
/*                                                                          */
/*     This function is responsible for setting the DMA control registers,  */
/*     source address, destination address and the corresponding pages      */
/*     transfer count for the specified DMA ch.								*/
/*                                                                          */
/****************************************************************************/
void dma_init(PDma55xConfig pDma);

/****************************************************************************/
/* dma_global_init -  Initialize global control registers.                  */
/*                                                                          */
/*     This function is responsible for setting the DMA global control      */
/*     register                                                             */
/****************************************************************************/
void dma_global_init(unsigned int gcr);

/*****************************************************************************/
/* dma_reset -  Reset DMA ch.                                                */
/*                                                                           */
/*     This function resets the specified DMA ch by initializing             */
/*     ch control registers to their default values                          */
/*                                                                           */
/*****************************************************************************/
void dma_reset(int ch);

/*****************************************************************************/
/* dma_reset_all -  Reset all DMA channels.                                  */
/*                                                                           */
/*     This function resets all DMA channels by initializing                 */
/*     ch control registers to their default values                          */
/*                                                                           */
/*****************************************************************************/
void dma_reset_all(void);

#endif //_DMA55X_H_
