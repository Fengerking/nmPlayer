/**
 * Constants (from spec)
 */

#ifndef _TS_CONSTANT_H_
#define _TS_CONSTANT_H_

#include "vobstype.h"


const uint8 TransportPacketSyncByte = 0x47;
const int TransportPacketSize = 188;
const uint32 PESPacketStartCodePrefix = 0x000001;

const int MAX_PAT_SECTION_LENGTH = 1021;
const int MAX_PMT_SECTION_LENGTH = 1021;
const int MAX_SDT_SECTION_LENGTH = 1021;
const int MAX_NIT_SECTION_LENGTH = 1021;
const int MAX_EIT_SECTION_LENGTH = 4093;
const int MAX_MGT_SECTION_LENGTH = 4093;


// PID
const uint32 PID_PAT = 0x00;
const uint32 PID_CAT = 0x01;
const uint32 PID_TSDT = 0x02;
const uint32 PID_NIT = 0x10;
const uint32 PID_BAT = 0x11;
const uint32 PID_SDT = 0x11;
const uint32 PID_EIT = 0x12;
const uint32 PID_RST = 0x13;
const uint32 PID_TDT = 0x14;
const uint32 PID_ATSC_PSIP = 0x1FFB;

const uint32 PID_NULL = 0x1fff;

// Stream Type
// see 13818-1-2000.pdf, Table 2-29, p.66 (maybe 13818-1.DOC Table 2-36 p85)
const uint8 ST_Reserved = 0x00;
const uint8 ST_111722Video = 0x01;
const uint8 ST_H262Video = 0x02; //13818-2, 11172-2
const uint8 ST_111723Audio = 0x03;
const uint8 ST_138183Audio = 0x04;
const uint8 ST_H2220_138181_PrivateSection = 0x05;
const uint8 ST_H2220_138181_PESPackets = 0x06;
const uint8 ST_13522MHEG = 0x07;
const uint8 ST_DSMCC = 0x08;
const uint8 ST_H2221 = 0x09;
const uint8 ST_138186TypeA = 0x0A;
const uint8 ST_138186TypeB = 0x0B;
const uint8 ST_138186TypeC = 0x0C;
const uint8 ST_138186TypeD = 0x0D;
const uint8 ST_138181Auxiliary = 0x0E;
const uint8 ST_138187AudioWithADTS = 0x0F;
const uint8 ST_144962Visual = 0x10;
const uint8 ST_144963AudioWithLATM = 0x11;
const uint8 ST_144961PES = 0x12;
const uint8 ST_14496Section = 0x13;
const uint8 ST_META_DATA_IN_PES = 0x15;
const uint8 ST_H264 = 0x1b;

//spec N13656 (ISO_IEC 13818-1-2013_FDAM3-final)-r1.doc, HEVC video stream or an HEVC temporal video sub-bitstream
const uint8 ST_HEVC = 0x24;

//spec N13656 (ISO_IEC 13818-1-2013_FDAM3-final)-r1.doc,HEVC temporal video subset of an HEVC video stream conforming to one or more profiles defined in Annex A of ITU-T Rec. H.265 | ISO/IEC 23008-2
const uint8 ST_HEVC_IEC_23008_2 = 0x25;


const uint8 ST_H2220ReservedMax = 0x7F;
const uint8 ST_UserPrivateMin = 0x80;
const uint8 ST_UserPrivateAC3 = 0x81;
const uint8 ST_HDMV_DTS_Audio = 0x82;
const uint8 ST_LPCM_Audio = 0x83;
const uint8 ST_AUDIO_DTS_HD = 0x86;
const uint8 ST_UserPrivateEAC3 = 0x87;
const uint8 ST_UserPrivateMax = 0xFF;


//Stream ID
const uint32 SID_METADATA_PES = 0x0d;
const uint32 SID_ProgramStreamMap = 0xbc;
const uint32 SID_PrivateStream1 = 0xbd;
const uint32 SID_PaddingStream = 0xbe;
const uint32 SID_PrivateStream2 = 0xbf;
const uint32 SID_AudioMin = 0xc0;
const uint32 SID_AudioMax = 0xdf;
const uint32 SID_VideoMin = 0xe0;
const uint32 SID_VideoMax = 0xef;
const uint32 SID_ECMStream = 0xf0;
const uint32 SID_EMMStream = 0xf1;
const uint32 SID_DSMCCStream = 0xf2;
const uint32 SID_13522Stream = 0xf3;
const uint32 SID_H2221A = 0xf4;
const uint32 SID_H2221B = 0xf5;
const uint32 SID_H2221C = 0xf6;
const uint32 SID_H2221D = 0xf7;
const uint32 SID_H2221E = 0xf8;
const uint32 SID_AncillaryStream = 0xf9;
const uint32 SID_ReservedDataStreamMin = 0xfa;
const uint32 SID_ReservedDataStreamMax = 0xfe;
const uint32 SID_ProgramStreamDirectory = 0xff;

// Table ID


const uint32 Master_Guide_Table_ID = 0xC7;
/*
Value Description
0x00 program_association_section
0x01 conditional_access_section
0x02 program_map_section
0x03 transport_stream_description_section
0x04 to 0x3F reserved
0x40 network_information_section - actual_network
0x41 network_information_section - other_network
0x42 service_description_section - actual_transport_stream
0x43 to 0x45 reserved for future use
0x46 service_description_section - other_transport_stream
0x47 to 0x49 reserved for future use
0x4A bouquet_association_section
0x4B to 0x4D reserved for future use
0x4E event_information_section - actual_transport_stream, present/following
0x4F event_information_section - other_transport_stream, present/following
0x50 to 0x5F event_information_section - actual_transport_stream, schedule
0x60 to 0x6F event_information_section - other_transport_stream, schedule
0x70 time_date_section
0x71 running_status_section
0x72 stuffing_section
0x73 time_offset_section
0x74 application information section (TS 102 812 [16])
0x75 container section (TS 102 323 [14])
0x76 related content section (TS 102 323 [14])
0x77 content identifier section (TS 102 323 [14])
0x78 MPE-FEC section (EN 301 192 [4])
0x79 resolution notification section (TS 102 323 [14])
0x79 to 0x7D reserved for future use
0x7E discontinuity_information_section
0x7F selection_information_section
0x80 to 0xFE user defined
0xFF reserved
*/



#endif //_TS_CONSTANT_H_
