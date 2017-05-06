#include "dma.h"

void ResetDMA()
{

	DMGCR = 0x0000;

	//reset Tx DMA ch
	DMCSDP(4)  = 0;		// src and dst parameters reg
	DMCICR(4)  = 0;		// interrupt control reg

	DMCSSAL(4) = 0;		// src start address reg (lower part)
	DMCSSAU(4) = 0;		// src start address reg (upper part)
	DMCDSAL(4) = 0;		// dst start address reg (lower part)
	DMCDSAU(4) = 0;		// dst start address reg (upper part)
	
	DMCEN(4)   = 0;		// element number reg
	DMCFN(4)   = 0;		// frame number reg
	DMCEI(4)   = 0;		// element index reg
	DMCFI(4)   = 0;		// frame index reg
	
}
/*
void setup_dma()
{
	avdInt32 addr;
	DMGCR = 0x0000;		// global control register

	// Transmit DMA setup
	// bits 12-9 of DMCSDP are dst selection bits. Here DST selects Peripherals as dst.
	// bits 5-2 of DMCSDP are src selection bits. Here SRC selects DARAM as src.
	// bits 1-0 of DMCSDP are data type bit. Here the DMAC makes 32-bit accesses at src & dst.
//	DMCSDP(4)  = (DM_RHEA << DM_DST) | (DM_DARAM << DM_SRC) | (DM_DTYPE_32 << DM_DATA_TYPE);

	DMCSDP(4)  = (DM_EMIF << DM_DST) | (DM_SARAM << DM_SRC) | (DM_DTYPE_32 << DM_DATA_TYPE);


	// send channel interrupt to CPU when all of current frame has been 
	// transferred from src port to dst port.
	DMCICR(4)  = (1 << DM_FRAME);

	// set src address
	addr = ((avdInt32)pt)<<1;		// change word address to form a byte address
	DMCSSAL(4) = (avdInt16)addr;		// load 16 LSBs of byte address
	DMCSSAU(4) = (avdInt16)((addr & 0xff0000)>>16);		// load 8 MSBs of byte address

	// set dst address
	// DXR2 is a McBSP data transmit reg 2
//    addr = ((u32)DXR2_ADDR(2))<<1;		// change word address to form a byte address
	addr = ((avdInt32)destBuf)<<1;
	DMCDSAL(4) = (avdInt16)addr;			// load 16 LSBs of byte address
	DMCDSAU(4) = (avdInt16)((addr & 0xff0000)>>16);		// load 8 MSBs of byte address
	                      
	//Divide by 2 for double word transfers
	DMCEN(4)   = 5;		// element number reg  
	DMCFN(4)   = 1;					// frame number reg
	DMCEI(4)   = 0;					// 4-byte element index reg
	DMCFI(4)   = 0;					// frame index reg

	// set the channel control reg
	DMCCR(4)  = (DM_POST_INCR     << DM_DST_AMODE)	|	// constant address mode for dst
				(DM_POST_INCR << DM_SRC_AMODE)	|	// automatic post increment at src port
				(1 << DM_END_PROG)				|	// end of programmation
				(1 << DM_AUTOINIT)				|	// DAMC reinitializes channel
				(1 << DM_PRIO)					|	// high priority channel
				(0 << DM_SYNC);			// McBSP 1 transmit event

	DMCCR(4) |=  (1 << DM_EN);		// channel is enabled

}      


void TestDMA()
{
	avdNativeInt i;
	destBuf = (avdInt32*)(0x120000);
    //setup data to transfer 
    pt = txBuf;                  //point to SARAM
	for (i=0; i<5; i++)
	{
		pt[i] = i;
	}

  // memcpy(destBuf, txBuf, 8);  
  	 memset(destBuf, 0, 16);

	setup_dma();
	
	//now start McBsp and DMA transfers
    DMA_ENABLE(4);
} 
*/

void memcpy_DMA(avdInt16* Dest, avdInt16* Source, avdInt16 len, avdInt16 channel, int DestType, int SourceType)
{

	avdInt32 addr;
	DMGCR = 0x0000;		// global control register

	// Transmit DMA setup
	// bits 12-9 of DMCSDP are dst selection bits. 
	// bits 5-2 of DMCSDP are src selection bits.
	// bits 1-0 of DMCSDP are data type bit for src & dst.
	DMCSDP(channel)  = (DestType<< DM_DST) | (SourceType << DM_SRC) | (DM_DTYPE_16 << DM_DATA_TYPE);

	// send channel interrupt to CPU when all of current frame has been 
	// transferred from src port to dst port.
	DMCICR(channel)  = (1 << DM_FRAME);

	// set src address
	addr = ((avdInt32)Source)<<1;		// change word address to form a byte address
	DMCSSAL(channel) = (avdInt16)addr;		// load 16 LSBs of byte address
	DMCSSAU(channel) = (avdInt16)((addr & 0xff0000)>>16);		// load 8 MSBs of byte address

	// set dst address
	addr = ((avdInt32)Dest)<<1;
	DMCDSAL(channel) = (avdInt16)addr;			// load 16 LSBs of byte address
	DMCDSAU(channel) = (avdInt16)((addr & 0xff0000)>>16);		// load 8 MSBs of byte address
	                      
	//Divide by 2 for double word transfers
	DMCEN(channel)   = len;		// element number reg  
	DMCFN(channel)   = 1;		// frame number reg
	DMCEI(channel)   = 0;		// 4-byte element index reg
	DMCFI(channel)   = 0;		// frame index reg

	// set the channel control reg
	DMCCR(channel)  = (DM_POST_INCR << DM_DST_AMODE)	|	// constant address mode for dst
				(DM_POST_INCR << DM_SRC_AMODE)	|	// automatic post increment at src port
				(1 << DM_END_PROG)				|	// end of programmation
				(1 << DM_AUTOINIT)				|	// DAMC reinitializes channel
				(1 << DM_PRIO)					|	// high priority channel
				(1<<DM_EN)                                       |
				(0 << DM_SYNC);			// McBSP  transmit event

//	DMCCR(4) |=  (1 << DM_EN);		// channel is enabled
	
	//now start  DMA transfers
//	DMA_ENABLE(channel);
} 

