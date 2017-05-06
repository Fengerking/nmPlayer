/******************************************************************/
/* regs55xx.h (Extension for regs.h)                              */
/* Copyright (c) Texas Instruments , Incorporated  1998           */
/* Author: partly Stefan Haas                                     */
/******************************************************************/

/******************************************************************/
/* Check to see if mmregs.h has been previously included by       */
/* another header, if so, skip this and go on                     */
/******************************************************************/
#ifndef _REGS55X_H
#define _REGS55X_H

#define MASK_TARGET_WORD	0xffff
#define TARGET_WRD_SZ	CHAR_BIT	
#define BYTES_PER_WORD  TARGET_WRD_SZ/8
#define WORD_SIZE	(CHAR_BIT * sizeof(unsigned int))

/*----------------------------------------------------------------------------*/
/* MACRO FUNCTIONS                                                            */
/*----------------------------------------------------------------------------*/
#define CONTENTS_OF(addr) \
        (*((volatile unsigned int*)(addr)))

#define LENGTH_TO_BITS(length) \
        (~(0xffffffff << (length)))

/* MACROS to SET, CLEAR and RETURN bits and bitfields in Memory Mapped        */
/* locations using the address of the specified register.                     */

#define REG_READ(addr) \
        (CONTENTS_OF(addr))

#define REG_WRITE(addr,val) \
        (CONTENTS_OF(addr) = (val))

#define MASK_BIT(bit) \
        (1 << (bit))

#define RESET_BIT(addr,bit) \
        (CONTENTS_OF(addr) &= (~MASK_BIT(bit)))

#define GET_BIT(addr,bit) \
        (CONTENTS_OF(addr) & (MASK_BIT(bit)) ? 1 : 0)

#define SET_BIT(addr,bit) \
        (CONTENTS_OF(addr) = (CONTENTS_OF(addr)) | (MASK_BIT(bit)))

#define ASSIGN_BIT_VAL(addr,bit,val) \
        ( (val) ? SET_BIT(addr,bit) : RESET_BIT(addr,bit) )

#define CREATE_FIELD(bit,length) \
        (LENGTH_TO_BITS(length) << (bit))

#define RESET_FIELD(addr,bit,length) \
        ( CONTENTS_OF(addr) &= (~CREATE_FIELD(bit,length)))

#define TRUNCATE(val,bit,length) \
        (((unsigned int)(val) << (bit)) & (CREATE_FIELD(bit, length)))

#define MASK_FIELD(bit,val,length)\
        TRUNCATE(val, bit, length)

#define GET_FIELD(addr,bit,length) \
       ((CONTENTS_OF(addr) & CREATE_FIELD(bit,length)) >> bit)

#define LOAD_FIELD(addr,val,bit,length) \
        (CONTENTS_OF(addr) &= (~CREATE_FIELD(bit,length)) | TRUNCATE(val, bit, length))  


/******************************************************************************/
/* Memory-mapped Byte Manipulation Macros                                     */
/******************************************************************************/
#define CSET_BIT(reg,bit) \
((*((volatile unsigned char *)(reg))) |= (MASK_BIT(bit)))

#define CGET_BIT(reg,bit) \
((*((volatile unsigned char *)(reg))) & (MASK_BIT(bit)) ? 1 : 0)

#define CCLR_BIT(reg,bit) \
((*((volatile unsigned char *)(reg))) &= (~MASK_BIT(bit)))

#define CGET_FIELD(reg,bit,length) \
((*((volatile unsigned char *)(reg)) & (MASK_FIELD(bit,length))) >> bit)

#define CLOAD_FIELD(reg,bit,length,val) \
   ((*((volatile unsigned char *)(reg))) = \
((*((volatile unsigned char *)(reg)) & (~MASK_FIELD(bit,length)))) | (val<<bit))

#define CREG_READ(addr) \
(*((unsigned char *)(addr)))

#define CREG_WRITE(addr,val) \
(*((unsigned char *)(addr)) = (val))

/* MACROS to SET, CLEAR and RETURN bits and bitfields in Memory Mapped        */
/* and Non-Memory Mapped using register names.                                */

#define GET_REG(reg) \
        (reg)

#define SET_REG(reg,val) \
        ((reg)= (val))

#define GET_REG_BIT(reg,bit) \
        ((reg) & MASK_BIT(bit) ? 1 : 0)

#define SET_REG_BIT(reg,bit) \
        ((reg) |= MASK_BIT(bit))

#define RESET_REG_BIT(reg,bit) \
        ((reg) &= (~MASK_BIT(bit)))

#define GET_REG_FIELD(reg,bit,length) \
        (reg & CREATE_FIELD(bit,length)) >> bit)

#define LOAD_REG_FIELD(reg,val,bit,length) \
        (reg &= (~CREATE_FIELD(bit,length)) | (val<<bit))
           
/*********************************************************************/
/* Define data structures for some memory mapped registers           */
/*********************************************************************/

/*********************************************************************/
/* Data bitfields for Clock Mode Register (CLKMD)   				 */
/*********************************************************************/
#define IAI				14
#define IAI_SZ		 	1

#define IOB				13
#define IOB_SZ	 		1

#define PLL_MULT		7
#define PLL_MULT_SZ	 	5

#define PLL_MULT		7
#define PLL_MULT_SZ	 	5

#define PLL_DIV			5
#define PLL_DIV_SZ	 	2

#define PLL_ENABLE	 	4
#define PLL_ENABLE_SZ	1

#define BYPASS_DIV	 	2
#define BYPASS_DIV_SZ 	2

#define BREAKLN 		1
#define BREAKLN_SZ		1

#define LOCK	 		0
#define LOCK_SZ 		1

/*********************************************************************/
/* Define bit fields for Clock Mode Register (CLKMD)                 */
/*********************************************************************/
#define PLL_DIV_1		0x00 //CLKOUT=INPUT_CLK
#define PLL_DIV_2		0x01 //CLKOUT=INPUT_CLK/2
#define PLL_DIV_4		0x02 //CLKOUT=INPUT_CLK/4
#define PLL_DIV_8		0x03 //CLKOUT=INPUT_CLK/8

#define BYPASS_DIV_1	0x00 //CLKOUT=INPUT_CLK
#define BYPASS_DIV_2	0x01 //CLKOUT=INPUT_CLK/2
#define BYPASS_DIV_4	0x02 //CLKOUT=INPUT_CLK/4
#define BYPASS_DIV_8	0x03 //CLKOUT=INPUT_CLK/8

/*********************************************************************/
/* CLOCK MODE REGISTER ADDRESS                                       */
/* Defined for C55XX				                                 */
/*********************************************************************/
#define CLKMD_ADDR	0x1c00
#define CLKMD 	*(ioport volatile unsigned int *)CLKMD_ADDR

/*********************************************************************/
/* CLOCK_RESET(ctrl) - resets clock mode register					 */
/*    ctrl - mask to set control register							 */
/*********************************************************************/
#define CLOCK_RESET(clkmd)\
	CLKMD = clkmd

/******************************************************************************/
/* clock_init() -  Initialize clock frequency to specified value              */
/*                                                                            */
/* if pllmult > 1                                                             */
/*		outclk = (pllmult / (plldiv + 1)) * inclk                             */
/*                                                                            */
/* if pllmult < 1                                                             */
/*		outclk = (1 / (plldiv + 1)) * inclk                                   */
/*                                                                            */
/******************************************************************************/
void clock_init(unsigned int inclk, unsigned int outclk, unsigned int plldiv);

#endif //_REGS55X_H
