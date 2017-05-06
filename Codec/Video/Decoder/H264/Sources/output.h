
/*!
 **************************************************************************************
 * \file
 *    output.h
 * \brief
 *    Picture writing routine headers
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details) 
 *      - Karsten Suehring        <suehring@hhi.de>
 ***************************************************************************************
 */

void write_stored_frame(ImageParameters *img,FrameStore *fs);
void direct_output(ImageParameters *img,StorablePicture *p);
void init_out_buffer(ImageParameters *img);
void uninit_out_buffer(ImageParameters *img);
