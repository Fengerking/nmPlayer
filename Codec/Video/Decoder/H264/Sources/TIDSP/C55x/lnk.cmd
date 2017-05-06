
/**********************************************************/
/*                                                        */
/*         LINKER command file for memory map             */
/*                                                        */
/**********************************************************/

-lrts55x.lib
-stack 0x1000
-sysstack 0x0800
-heap 0x8000
 
MEMORY {
PAGE 0:

   DARAM:      origin = 0x200,         len = 0x0fe00
  /* SARAM:      origin = 0x10010,       len = 0xfff0*/
   SARAM1:     origin = 0x10010,       len = 0x17df0
   VECT:       origin = 0x27e00,       len = 0x200
   
 /*  
   APIBOOT:    origin = 0x20000,       len = 0x10
   SPAMAP:     origin = 0x100000,      len = 0x10
   
   SHMSEG0:    origin = 0x120000,      len = 0x10000
   SHMSEG0_GPP: origin = 0x130000,     len = 0x10000
   DYNEXTMEM:  origin = 0x140000,      len = 0x40000
   DYNSRAMMEM: origin = 0xd0000,       len = 0x30000 
   EXTMEM:     origin = 0x180000,      len = 0x80000
   SHMMEM:     origin = 0x200000,      len = 0x1fff0
 */
   FIFO(RWXI) : o = 0x04bf60, l = 0x0000a0	/* reserve 0xa0 bytes for FIFO*/
   PDROM:      origin = 0xff8000,      len = 0x8000
   
PAGE 1:  
   IOPORT (RWI)   : origin = 0x000000, len = 0x020000
   
}
 
 
 
SECTIONS
{
      	vectors  : {} > VECT   PAGE 0     
        .cinit   : {} fill = 0x00> SARAM1 PAGE 0
        .text    : {} fill = 0x00> SARAM1 PAGE 0
           
        .sysinit : {} fill = 0x00> DARAM PAGE 0 
        .stack   : {} fill = 0x00> DARAM PAGE 0
        .sysstack: {} fill = 0x00> DARAM PAGE 0
        .sysmem  : {} fill = 0x00> DARAM PAGE 0
        .data    : {} fill = 0x00> DARAM PAGE 0
        
        .bss     : {} fill = 0x00> DARAM PAGE 0
        .const   : {} fill = 0x00> DARAM PAGE 0 
      
        .switch  : {} > SARAM1 PAGE 0
        .cio     : {} > SARAM1 PAGE 0
		.fifo     > FIFO   PAGE 0  /* C I/O buffers     */

   		.ioport   > IOPORT PAGE 1  /* Global & static IO vars    */
}

