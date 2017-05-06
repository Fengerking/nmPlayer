/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		AsfFileGuid.h

Contains:	Internal GUID Of ASF File

Written by:	East

Reference:	ASF Specification

Change History (most recent first):
2006-08-22		East			Create file

*******************************************************************************/
#ifndef __ASF_File_GUID_H
#define __ASF_File_GUID_H

#include "voType.h"
#include "fMacros.h"

//////////////////////////////////////////////////////////////////////////
//Top-level ASF Object
//////////////////////////////////////////////////////////////////////////
// {75b22630-668e-11cf-a6d9-00aa0062ce6c}
VO_DEFINE_GUID(ASF_Header_Object, 
			   0x75b22630, 0x668e, 0x11cf, 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c);

// {75b22636-668e-11cf-a6d9-00aa0062ce6c}
VO_DEFINE_GUID(ASF_Data_Object, 
			   0x75b22636, 0x668e, 0x11cf, 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c);

// {33000890-e5b1-11cf-89f4-00a0c90349cb}
VO_DEFINE_GUID(ASF_Simple_Index_Object, 
			   0x33000890, 0xe5b1, 0x11cf, 0x89, 0xf4, 0x00, 0xa0, 0xc9, 0x03, 0x49, 0xcb);

// {d6e229d3-35da-11d1-9034-00a0c90349be}
VO_DEFINE_GUID(ASF_Index_Object, 
			   0xd6e229d3, 0x35da, 0x11d1, 0x90, 0x34, 0x00, 0xa0, 0xc9, 0x03, 0x49, 0xbe);

// {feb103f8-12ad-4c64-840f-2a1d2f7ad48c}
VO_DEFINE_GUID(ASF_Media_Object_Index_Object, 
			   0xfeb103f8, 0x12ad, 0x4c64, 0x84, 0x0f, 0x2a, 0x1d, 0x2f, 0x7a, 0xd4, 0x8c);

// {3cb73fd0-0c4a-4803-953d-edf7b6228f0c}
VO_DEFINE_GUID(ASF_Timecode_Index_Object, 
			   0x3cb73fd0, 0x0c4a, 0x4803, 0x95, 0x3d, 0xed, 0xf7, 0xb6, 0x22, 0x8f, 0x0c);

//////////////////////////////////////////////////////////////////////////
//Header Object
//////////////////////////////////////////////////////////////////////////
// {8cabdca1-a947-11cf-8ee4-00c00c205365}
VO_DEFINE_GUID(ASF_File_Properties_Object, 
			   0x8cabdca1, 0xa947, 0x11cf, 0x8e, 0xe4, 0x00, 0xc0, 0x0c, 0x20, 0x53, 0x65);

// {b7dc0791-a9b7-11cf-8ee6-00c00c205365}
VO_DEFINE_GUID(ASF_Stream_Properties_Object, 
			   0xb7dc0791, 0xa9b7, 0x11cf, 0x8e, 0xe6, 0x00, 0xc0, 0x0c, 0x20, 0x53, 0x65);

// {5fbf03b5-a92e-11cf-8ee3-00c00c205365}
VO_DEFINE_GUID(ASF_Header_Extension_Object, 
			   0x5fbf03b5, 0xa92e, 0x11cf, 0x8e, 0xe3, 0x00, 0xc0, 0x0c, 0x20, 0x53, 0x65);

// {86d15240-311d-11d0-a3a4-00a0c90348f6}
VO_DEFINE_GUID(ASF_Codec_List_Object, 
			   0x86d15240, 0x311d, 0x11d0, 0xa3, 0xa4, 0x00, 0xa0, 0xc9, 0x03, 0x48, 0xf6);

// {1efb1a30-0b62-11d0-a39b-00a0c90348f6}
VO_DEFINE_GUID(ASF_Script_Command_Object, 
			   0x1efb1a30, 0x0b62, 0x11d0, 0xa3, 0x9b, 0x00, 0xa0, 0xc9, 0x03, 0x48, 0xf6);

// {f487cd01-a951-11cf-8ee6-00c00c205365}
VO_DEFINE_GUID(ASF_Marker_Object, 
			   0xf487cd01, 0xa951, 0x11cf, 0x8e, 0xe6, 0x00, 0xc0, 0x0c, 0x20, 0x53, 0x65);

// {d6e229dc-35da-11d1-9034-00a0c90349be}
VO_DEFINE_GUID(ASF_Bitrate_Mutual_Exclusion_Object, 
			   0xd6e229dc, 0x35da, 0x11d1, 0x90, 0x34, 0x00, 0xa0, 0xc9, 0x03, 0x49, 0xbe);

// {75b22635-668e-11cf-a6d9-00aa0062ce6c}
VO_DEFINE_GUID(ASF_Error_Correction_Object, 
			   0x75b22635, 0x668e, 0x11cf, 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c);

// {75b22633-668e-11cf-a6d9-00aa0062ce6c}
VO_DEFINE_GUID(ASF_Content_Description_Object, 
			   0x75b22633, 0x668e, 0x11cf, 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c);

// {d2d0a440-e307-11d2-97f0-00a0c95ea850}
VO_DEFINE_GUID(ASF_Extended_Content_Description_Object, 
			   0xd2d0a440, 0xe307, 0x11d2, 0x97, 0xf0, 0x00, 0xa0, 0xc9, 0x5e, 0xa8, 0x50);

// {2211b3fa-bd23-11d2-b4b7-00a0c955fc6e}
VO_DEFINE_GUID(ASF_Content_Branding_Object, 
			   0x2211b3fa, 0xbd23, 0x11d2, 0xb4, 0xb7, 0x00, 0xa0, 0xc9, 0x55, 0xfc, 0x6e);

// {7bf875ce-468d-11d1-8d82-006097c9a2b2}
VO_DEFINE_GUID(ASF_Stream_Bitrate_Properties_Object, 
			   0x7bf875ce, 0x468d, 0x11d1, 0x8d, 0x82, 0x00, 0x60, 0x97, 0xc9, 0xa2, 0xb2);

// {2211b3fb-bd23-11d2-b4b7-00a0c955fc6e}
VO_DEFINE_GUID(ASF_Content_Encryption_Object, 
			   0x2211b3fb, 0xbd23, 0x11d2, 0xb4, 0xb7, 0x00, 0xa0, 0xc9, 0x55, 0xfc, 0x6e);

// {298ae614-2622-4c17-b935-dae07ee9289c}
VO_DEFINE_GUID(ASF_Extended_Content_Encryption_Object, 
			   0x298ae614, 0x2622, 0x4c17, 0xb9, 0x35, 0xda, 0xe0, 0x7e, 0xe9, 0x28, 0x9c);

// {9A04F079-9840-4286-AB92E65BE0885F95}
VO_DEFINE_GUID(ASF_Protection_System_Identifier_Object, 
			   0x9A04F079, 0x9840, 0x4286, 0xab, 0x92, 0xe6, 0x5b, 0xe0, 0x88, 0x5f, 0x95);

// {2211b3fc-bd23-11d2-b4b7-00a0c955fc6e}
VO_DEFINE_GUID(ASF_Digital_Signature_Object, 
			   0x2211b3fc, 0xbd23, 0x11d2, 0xb4, 0xb7, 0x00, 0xa0, 0xc9, 0x55, 0xfc, 0x6e);

// {1806d474-cadf-4509-a4ba-9aabcb96aae8}
VO_DEFINE_GUID(ASF_Padding_Object, 
			   0x1806d474, 0xcadf, 0x4509, 0xa4, 0xba, 0x9a, 0xab, 0xcb, 0x96, 0xaa, 0xe8);

// {544091F3-7AFB-441E-A7F4-8FE217BEBA2D}
VO_DEFINE_GUID(ASF_Header_Object_Unknown_1, 
			   0x544091f3, 0x7afb, 0x441e, 0xa7, 0xf4, 0x8f, 0xe2, 0x17, 0xbe, 0xba, 0x2d);

//////////////////////////////////////////////////////////////////////////
//Header Extension Object
//////////////////////////////////////////////////////////////////////////
// {14e6a5cb-c672-4332-8399-a96952065b5a}
VO_DEFINE_GUID(ASF_Extended_Stream_Properties_Object, 
			   0x14e6a5cb, 0xc672, 0x4332, 0x83, 0x99, 0xa9, 0x69, 0x52, 0x06, 0x5b, 0x5a);

// {a08649cf-4775-4670-8a16-6e35357566cd}
VO_DEFINE_GUID(ASF_Advanced_Mutual_Exclusion_Object, 
			   0xa08649cf, 0x4775, 0x4670, 0x8a, 0x16, 0x6e, 0x35, 0x35, 0x75, 0x66, 0xcd);

// {d1465a40-5a79-4338-b71b-e36b8fd6c249}
VO_DEFINE_GUID(ASF_Group_Mutual_Exclusion_Object, 
			   0xd1465a40, 0x5a79, 0x4338, 0xb7, 0x1b, 0xe3, 0x6b, 0x8f, 0xd6, 0xc2, 0x49);

// {d4fed15b-88d3-454f-81f0-ed5c45999e24}
VO_DEFINE_GUID(ASF_Stream_Prioritization_Object, 
			   0xd4fed15b, 0x88d3, 0x454f, 0x81, 0xf0, 0xed, 0x5c, 0x45, 0x99, 0x9e, 0x24);

// {a69609e6-517b-11d2-b6af-00c04fd908e9}
VO_DEFINE_GUID(ASF_Bandwidth_Sharing_Object, 
			   0xa69609e6, 0x517b, 0x11d2, 0xb6, 0xaf, 0x00, 0xc0, 0x4f, 0xd9, 0x08, 0xe9);

// {7c4346a9-efe0-4bfc-b229-393ede415c85}
VO_DEFINE_GUID(ASF_Language_List_Object, 
			   0x7c4346a9, 0xefe0, 0x4bfc, 0xb2, 0x29, 0x39, 0x3e, 0xde, 0x41, 0x5c, 0x85);

// {c5f8cbea-5baf-4877-8467-aa8c44fa4cca}
VO_DEFINE_GUID(ASF_Metadata_Object, 
			   0xc5f8cbea, 0x5baf, 0x4877, 0x84, 0x67, 0xaa, 0x8c, 0x44, 0xfa, 0x4c, 0xca);

// {44231c94-9498-49d1-a141-1d134e457054}
VO_DEFINE_GUID(ASF_Metadata_Library_Object, 
			   0x44231c94, 0x9498, 0x49d1, 0xa1, 0x41, 0x1d, 0x13, 0x4e, 0x45, 0x70, 0x54);

// {d6e229df-35da-11d1-9034-00a0c90349be}
VO_DEFINE_GUID(ASF_Index_Parameters_Object, 
			   0xd6e229df, 0x35da, 0x11d1, 0x90, 0x34, 0x00, 0xa0, 0xc9, 0x03, 0x49, 0xbe);

// {6b203bad-3f11-48e4-aca8-d7613de2cfa7}
VO_DEFINE_GUID(ASF_Media_Object_Index_Parameters_Object, 
			   0x6b203bad, 0x3f11, 0x48e4, 0xac, 0xa8, 0xd7, 0x61, 0x3d, 0xe2, 0xcf, 0xa7);

// {f55e496d-9797-4b5d-8c8b-604dfe9bfb24}
VO_DEFINE_GUID(ASF_Timecode_Index_Parameters_Object, 
			   0xf55e496d, 0x9797, 0x4b5d, 0x8c, 0x8b, 0x60, 0x4d, 0xfe, 0x9b, 0xfb, 0x24);

//the guid is different from document, pls note when use!
// {26f18b5d-4584-47ec-9f5f-0e651f0452c9}
VO_DEFINE_GUID(ASF_Compatibility_Object, 
			   0x26f18b5d, 0x4584, 0x47ec, 0x9f, 0x5f, 0x0e, 0x65, 0x1f, 0x04, 0x52, 0xc9);

// {43058533-6981-49e6-9b74-ad12cb86d58c}
VO_DEFINE_GUID(ASF_Advanced_Content_Encryption_Object, 
			   0x43058533, 0x6981, 0x49e6, 0x9b, 0x74, 0xad, 0x12, 0xcb, 0x86, 0xd5, 0x8c);

//////////////////////////////////////////////////////////////////////////
//Stream Properties Object Stream Type
//////////////////////////////////////////////////////////////////////////
// {f8699e40-5b4d-11cf-a8fd-00805f5c442b}
VO_DEFINE_GUID(ASF_Audio_Media, 
			   0xf8699e40, 0x5b4d, 0x11cf, 0xa8, 0xfd, 0x00, 0x80, 0x5f, 0x5c, 0x44, 0x2b);

// {bc19efc0-5b4d-11cf-a8fd-00805f5c442b}
VO_DEFINE_GUID(ASF_Video_Media, 
			   0xbc19efc0, 0x5b4d, 0x11cf, 0xa8, 0xfd, 0x00, 0x80, 0x5f, 0x5c, 0x44, 0x2b);

// {59dacfc0-59e6-11d0-a3ac-00a0c90348f6}
VO_DEFINE_GUID(ASF_Command_Media, 
			   0x59dacfc0, 0x59e6, 0x11d0, 0xa3, 0xac, 0x00, 0xa0, 0xc9, 0x03, 0x48, 0xf6);

// {b61be100-5b4e-11cf-a8fd-00805f5c442b}
VO_DEFINE_GUID(ASF_JFIF_Media, 
			   0xb61be100, 0x5b4e, 0x11cf, 0xa8, 0xfd, 0x00, 0x80, 0x5f, 0x5c, 0x44, 0x2b);

// {35907de0-e415-11cf-a917-00805f5c442b}
VO_DEFINE_GUID(ASF_Degradable_JPEG_Media, 
			   0x35907de0, 0xe415, 0x11cf, 0xa9, 0x17, 0x00, 0x80, 0x5f, 0x5c, 0x44, 0x2b);

// {91bd222c-f21c-497a-8b6d-5aa86bfc0185}
VO_DEFINE_GUID(ASF_File_Transfer_Media, 
			   0x91bd222c, 0xf21c, 0x497a, 0x8b, 0x6d, 0x5a, 0xa8, 0x6b, 0xfc, 0x01, 0x85);

// {3afb65e2-47ef-40f2-ac2c-70a90d71d343}
VO_DEFINE_GUID(ASF_Binary_Media, 
			   0x3afb65e2, 0x47ef, 0x40f2, 0xac, 0x2c, 0x70, 0xa9, 0x0d, 0x71, 0xd3, 0x43);

//////////////////////////////////////////////////////////////////////////
//Web Stream Type-Specific Data
//////////////////////////////////////////////////////////////////////////
// {776257d4-c627-41cb-8f81-7ac7ff1c40cc}
VO_DEFINE_GUID(ASF_Web_Stream_Media_Subtype, 
			   0x776257d4, 0xc627, 0x41cb, 0x8f, 0x81, 0x7a, 0xc7, 0xff, 0x1c, 0x40, 0xcc);

// {da1e6b13-8359-4050-b398-388e965bf00c}
VO_DEFINE_GUID(ASF_Web_Stream_Format, 
			   0xda1e6b13, 0x8359, 0x4050, 0xb3, 0x98, 0x38, 0x8e, 0x96, 0x5b, 0xf0, 0x0c);

//////////////////////////////////////////////////////////////////////////
//Stream Properties Object Error Correction Type
//////////////////////////////////////////////////////////////////////////
// {20fb5700-5b55-11cf-a8fd-00805f5c442b}
VO_DEFINE_GUID(ASF_No_Error_Correction, 
			   0x20fb5700, 0x5b55, 0x11cf, 0xa8, 0xfd, 0x00, 0x80, 0x5f, 0x5c, 0x44, 0x2b);

// {bfc3cd50-618f-11cf-8bb2-00aa00b4e220}
VO_DEFINE_GUID(ASF_Audio_Spread, 
			   0xbfc3cd50, 0x618f, 0x11cf, 0x8b, 0xb2, 0x00, 0xaa, 0x00, 0xb4, 0xe2, 0x20);

//////////////////////////////////////////////////////////////////////////
//Reserved 1 Field Of The Header Extension Object
//////////////////////////////////////////////////////////////////////////
// {abd3d211-a9ba-11cf-8ee6-00c00c205365}
VO_DEFINE_GUID(ASF_Reserved_1, 
			   0xabd3d211, 0xa9ba, 0x11cf, 0x8e, 0xe6, 0x00, 0xc0, 0x0c, 0x20, 0x53, 0x65);

//////////////////////////////////////////////////////////////////////////
//System ID Filed Of The Advanced Content Encryption Object
//////////////////////////////////////////////////////////////////////////
// {7a079bb6-daa4-4e12-a5ca-91d38dc11a8d}
VO_DEFINE_GUID(ASF_Content_Encryption_System_Windows_Media_DRM_Network_Devices, 
			   0x7a079bb6, 0xdaa4, 0x4e12, 0xa5, 0xca, 0x91, 0xd3, 0x8d, 0xc1, 0x1a, 0x8d);

// {7a079bb6-daa4-4e12-a5ca-91d38dc11a8d}
VO_DEFINE_GUID(ASF_Content_Protection_System_Microsoft_PlayReady, 
			   0xf4637010, 0x03c3, 0x42cd, 0xb9, 0x32, 0xb4, 0x8a, 0xdf, 0x3a, 0x6a, 0x54);

//////////////////////////////////////////////////////////////////////////
//Reserved 2 Field Of The Codec List Object
//////////////////////////////////////////////////////////////////////////
// {86d15241-311d-11d0-a3a4-00a0c90348f6}
VO_DEFINE_GUID(ASF_Reserved_2, 
			   0x86d15241, 0x311d, 0x11d0, 0xa3, 0xa4, 0x00, 0xa0, 0xc9, 0x03, 0x48, 0xf6);

//////////////////////////////////////////////////////////////////////////
//Reserved 3 Field Of The Script Command Object
//////////////////////////////////////////////////////////////////////////
// {4b1acbe3-100b-11d0-a39b-00a0c90348f6}
VO_DEFINE_GUID(ASF_Reserved_3, 
			   0x4b1acbe3, 0x100b, 0x11d0, 0xa3, 0x9b, 0x00, 0xa0, 0xc9, 0x03, 0x48, 0xf6);

//////////////////////////////////////////////////////////////////////////
//Reserved 4 Field Of The Marker Object
//////////////////////////////////////////////////////////////////////////
// {4cfedb20-75f6-11cf-9c0f-00a0c90349cb}
VO_DEFINE_GUID(ASF_Reserved_4, 
			   0x4cfedb20, 0x75f6, 0x11cf, 0x9c, 0x0f, 0x00, 0xa0, 0xc9, 0x03, 0x49, 0xcb);

//////////////////////////////////////////////////////////////////////////
//Mutual Exclusion Object Exclusion Type
//////////////////////////////////////////////////////////////////////////
// {d6e22a00-35da-11d1-9034-00a0c90349be}
VO_DEFINE_GUID(ASF_Mutex_Language, 
			   0xd6e22a00, 0x35da, 0x11d1, 0x90, 0x34, 0x00, 0xa0, 0xc9, 0x03, 0x49, 0xbe);

// {d6e22a01-35da-11d1-9034-00a0c90349be}
VO_DEFINE_GUID(ASF_Mutex_Bitrate, 
			   0xd6e22a01, 0x35da, 0x11d1, 0x90, 0x34, 0x00, 0xa0, 0xc9, 0x03, 0x49, 0xbe);

// {d6e22a02-35da-11d1-9034-00a0c90349be}
VO_DEFINE_GUID(ASF_Mutex_Unknown, 
			   0xd6e22a02, 0x35da, 0x11d1, 0x90, 0x34, 0x00, 0xa0, 0xc9, 0x03, 0x49, 0xbe);

//////////////////////////////////////////////////////////////////////////
//Bandwidth Sharing Object
//////////////////////////////////////////////////////////////////////////
// {af6060aa-5197-11d2-b6af-00c04fd908e9}
VO_DEFINE_GUID(ASF_Bandwidth_Sharing_Exclusive, 
			   0xaf6060aa, 0x5197, 0x11d2, 0xb6, 0xaf, 0x00, 0xc0, 0x4f, 0xd9, 0x08, 0xe9);

// {af6060ab-5197-11d2-b6af-00c04fd908e9}
VO_DEFINE_GUID(ASF_Bandwidth_Sharing_Partial, 
			   0xaf6060ab, 0x5197, 0x11d2, 0xb6, 0xaf, 0x00, 0xc0, 0x4f, 0xd9, 0x08, 0xe9);

//////////////////////////////////////////////////////////////////////////
//Standard Payload Extension System
//////////////////////////////////////////////////////////////////////////
// {399595ec-8667-4e2d-8fdb-98814ce76c1e}
VO_DEFINE_GUID(ASF_Payload_Extension_System_Timecode, 
			   0x399595ec, 0x8667, 0x4e2d, 0x8f, 0xdb, 0x98, 0x81, 0x4c, 0xe7, 0x6c, 0x1e);

// {e165ec0e-19ed-45d7-b4a7-25cbd1e28e9b}
VO_DEFINE_GUID(ASF_Payload_Extension_System_File_Name, 
			   0xe165ec0e, 0x19ed, 0x45d7, 0xb4, 0xa7, 0x25, 0xcb, 0xd1, 0xe2, 0x8e, 0x9b);

// {d590dc20-07bc-436c-9cf7-f3bbfbf1a4dc}
VO_DEFINE_GUID(ASF_Payload_Extension_System_Content_Type, 
			   0xd590dc20, 0x07bc, 0x436c, 0x9c, 0xf7, 0xf3, 0xbb, 0xfb, 0xf1, 0xa4, 0xdc);

// {1b1ee554-f9ea-4bc8-821a-376b74e4c4b8}
VO_DEFINE_GUID(ASF_Payload_Extension_System_Pixel_Aspect_Ratio, 
			   0x1b1ee554, 0xf9ea, 0x4bc8, 0x82, 0x1a, 0x37, 0x6b, 0x74, 0xe4, 0xc4, 0xb8);

// {c6bd9450-867f-4907-83a3-c77921b733ad}
VO_DEFINE_GUID(ASF_Payload_Extension_System_Sample_Duration, 
			   0xc6bd9450, 0x867f, 0x4907, 0x83, 0xa3, 0xc7, 0x79, 0x21, 0xb7, 0x33, 0xad);

// {6698b84e-0afa-4330-aeb2-1c0a98d7a44d}
VO_DEFINE_GUID(ASF_Payload_Extension_System_Encryption_Sample_ID, 
			   0x6698b84e, 0x0afa, 0x4330, 0xae, 0xb2, 0x1c, 0x0a, 0x98, 0xd7, 0xa4, 0x4d);

// {00E1AF06-7BEC-11D1-A582-00C04FC29CFB}
VO_DEFINE_GUID(ASF_Payload_Extension_System_Degradable_JPEG, 
			   0x00e1af06, 0x7bec, 0x11d1, 0xa5, 0x82, 0x00, 0xc0, 0x4f, 0xc2, 0x9c, 0xfb);

#endif	//__ASF_File_GUID_H
