//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
    //  tables for decoding.
     extern U8_WMV sm_rgRunAtIndx_HghMt [168];
     extern I8_WMV sm_rgLevelAtIndx_HghMt [168];
     extern U8_WMV sm_rgRunAtIndxOfIntraY_HghMt [185];
     extern I8_WMV sm_rgLevelAtIndxOfIntraY_HghMt [185];

     extern U8_WMV sm_rgRunAtIndx_Talking [148];
     extern I8_WMV sm_rgLevelAtIndx_Talking [148];
     extern U8_WMV sm_rgRunAtIndxOfIntraY_Talking [132];
     extern I8_WMV sm_rgLevelAtIndxOfIntraY_Talking [132];
     extern tPackHuffmanCode_WMV sm_HufMVTable_Talking [1101];
     extern U8_WMV sm_uXMvFromIndex_Talking [1099];
     extern U8_WMV sm_uYMvFromIndex_Talking [1099];
     extern tPackHuffmanCode_WMV sm_HufMVTable_HghMt [1101];
     extern U8_WMV sm_uXMvFromIndex_HghMt [1099];
     extern U8_WMV sm_uYMvFromIndex_HghMt [1099];
    // Header
     extern tPackHuffmanCode_WMV sm_HufPCBPCYTable [129];
     extern tPackHuffmanCode_WMV sm_HufICBPCYTable [65];
    
    //#ifdef NEW_PCBPCY_TABLE
     extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_HighRate [129];
     extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_MidRate [129];
     extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_LowRate [129];
    //#endif
    
    //_DCTDC
     extern tPackHuffmanCode_WMV sm_HufDCTDCyTable_Talking[121];
     extern tPackHuffmanCode_WMV sm_HufDCTDCcTable_Talking[121];
     extern tPackHuffmanCode_WMV sm_HufDCTDCyTable_HghMt[121];
     extern tPackHuffmanCode_WMV sm_HufDCTDCcTable_HghMt[121];
     extern tPackHuffmanCode_WMV sm_HufDCTACInterTable_HghMt[170];
     extern tPackHuffmanCode_WMV sm_HufDCTACIntraTable_HghMt[187];
     extern tPackHuffmanCode_WMV sm_HufDCTACInterTable_Talking[150];
     extern tPackHuffmanCode_WMV sm_HufDCTACIntraTable_Talking[134];
     extern tPackHuffmanCode_WMV sm_HufDCTACInterTable_MPEG4 [104];
     extern tPackHuffmanCode_WMV sm_HufDCTACIntraTable_MPEG4 [104];
     extern tPackHuffmanCode_WMV sm_HufDCTACInterTable_HghRate [176];
     extern tPackHuffmanCode_WMV sm_HufDCTACIntraTable_HghRate[164];

     extern U8_WMV sm_rgRunAtIndx_MPEG4 [102];
     extern I8_WMV sm_rgLevelAtIndx_MPEG4 [102];
     extern U8_WMV sm_rgRunAtIndxOfIntraY_MPEG4 [102];
     extern I8_WMV sm_rgLevelAtIndxOfIntraY_MPEG4 [102];
     extern U8_WMV sm_rgRunAtIndx_HghRate [174];
     extern I8_WMV sm_rgLevelAtIndx_HghRate [174];
     extern U8_WMV sm_rgRunAtIndxOfIntraY_HghRate [162];
     extern I8_WMV  sm_rgLevelAtIndxOfIntraY_HghRate [162];

     // ---------------------------------------------------

    /*extern HuffDecInfo m_hufMVDec_TalkingDecInfo[1806]; 
    extern TableInfo   m_hufMVDec_TalkingTableInfo[90];

    extern HuffDecInfo m_hufMVDec_HghMtDecInfo[1894]; 
    extern TableInfo   m_hufMVDec_HghMtTableInfo[151];
    
    extern HuffDecInfo m_hufICBPCYDecDecInfo[162]; 
    extern TableInfo   m_hufICBPCYDecTableInfo[15];
    
    extern HuffDecInfo m_hufICBPCYDecDecInfo[162]; 
    extern TableInfo   m_hufICBPCYDecTableInfo[15];
    
    extern HuffDecInfo m_hufPCBPCYDecDecInfo[756]; 
    extern TableInfo   m_hufPCBPCYDecTableInfo[13];
    
    extern HuffDecInfo m_hufPCBPCYDec_HighRateDecInfo[1112];
    extern TableInfo   m_hufPCBPCYDec_HighRateTableInfo[19];
    
    extern HuffDecInfo m_hufPCBPCYDec_MidRateDecInfo[650];
    extern TableInfo   m_hufPCBPCYDec_MidRateTableInfo[17];
    
    extern HuffDecInfo m_hufPCBPCYDec_LowRateDecInfo[408];
    extern TableInfo   m_hufPCBPCYDec_LowRateTableInfo[21];
    
    extern HuffDecInfo m_hufDCTDCyDec_TalkingDecInfo[838]; 
    extern TableInfo   m_hufDCTDCyDec_TalkingTableInfo[6];
    
    extern HuffDecInfo m_hufDCTDCcDec_TalkingDecInfo[668]; 
    extern TableInfo   m_hufDCTDCcDec_TalkingTableInfo[7];
    
    extern HuffDecInfo m_hufDCTDCyDec_HghMtDecInfo[1476]; 
    extern TableInfo   m_hufDCTDCyDec_HghMtTableInfo[7];
    
    extern HuffDecInfo m_hufDCTDCcDec_HghMtDecInfo[1088]; 
    extern TableInfo   m_hufDCTDCcDec_HghMtTableInfo[5];
    
    extern HuffDecInfo m_hufDCTACInterDec_HghMtDecInfo[464]; 
    extern TableInfo   m_hufDCTACInterDec_HghMtTableInfo[30];
    
    extern HuffDecInfo m_hufDCTACIntraDec_HghMtDecInfo[504]; 
    extern TableInfo   m_hufDCTACIntraDec_HghMtTableInfo[42];
    
    extern HuffDecInfo m_hufDCTACInterDec_TalkingDecInfo[404]; 
    extern TableInfo   m_hufDCTACInterDec_TalkingTableInfo[32];
    
    extern HuffDecInfo m_hufDCTACIntraDec_TalkingDecInfo[286];
    extern TableInfo   m_hufDCTACIntraDec_TalkingTableInfo[28];
    
    extern HuffDecInfo m_hufDCTACInterDec_MPEG4DecInfo[144]; 
    extern TableInfo   m_hufDCTACInterDec_MPEG4TableInfo[18];
    
    extern HuffDecInfo m_hufDCTACIntraDec_MPEG4DecInfo[144]; 
    extern TableInfo   m_hufDCTACIntraDec_MPEG4TableInfo[18];
*/

    extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_V9_1[65];
    extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_V9_2[65];
    extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_V9_3[65];
    extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_V9_4[65];

    extern tPackHuffmanCode_WMV sm_HufMVTable_V9_MV1[74];
    extern tPackHuffmanCode_WMV sm_HufMVTable_V9_MV2[74];
    extern tPackHuffmanCode_WMV sm_HufMVTable_V9_MV3[74];
    extern tPackHuffmanCode_WMV sm_HufMVTable_V9_MV4[74];


    extern tPackHuffmanCode_WMV sm_Huf4x4PatternLowQPTable[16];
    extern tPackHuffmanCode_WMV sm_Huf4x4PatternMidQPTable[16];
    extern tPackHuffmanCode_WMV sm_Huf4x4PatternHighQPTable[16];
    extern tPackHuffmanCode_WMV sm_HufMBXformTypeLowQPTable[17];
    extern tPackHuffmanCode_WMV sm_HufMBXformTypeMidQPTable[17];
    extern tPackHuffmanCode_WMV sm_HufMBXformTypeHighQPTable[17];
    extern tPackHuffmanCode_WMV sm_HufBlkXformTypeLowQPTable[9];
    extern tPackHuffmanCode_WMV sm_HufBlkXformTypeMidQPTable[9];
    extern tPackHuffmanCode_WMV sm_HufBlkXformTypeHighQPTable[9];

//#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)
//#ifndef PPCWMP
//extern const U8_WMV g_rgDitherMap_WMV [4][4][3][256];
//#endif
//#endif


extern tPackHuffmanCode_WMV  sm_HufFieldPictureMVBP_1[17] ;
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMVBP_2[17] ;
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMVBP_3[17] ;
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMVBP_4[17] ;
extern tPackHuffmanCode_WMV  sm_HufFieldPicture1MVMBModeTable_1[7] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPicture1MVMBModeTable_2[7] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPicture1MVMBModeTable_3[7] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPicture1MVMBModeTable_4[7] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPicture1MVMBModeTable_5[7] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPicture1MVMBModeTable_6[7] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPicture1MVMBModeTable_7[7] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPicture1MVMBModeTable_8[7] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMixedMVMBModeTable_1[9] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMixedMVMBModeTable_2[9] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMixedMVMBModeTable_3[9] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMixedMVMBModeTable_4[9] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMixedMVMBModeTable_5[9] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMixedMVMBModeTable_6[9] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMixedMVMBModeTable_7[9] ; 
extern tPackHuffmanCode_WMV  sm_HufFieldPictureMixedMVMBModeTable_8[9] ; 
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame2MVBP_1[5] ; 
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame2MVBP_2[5] ; 
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame2MVBP_3[5] ; 
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame2MVBP_4[5] ; 
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame4MvMBModeTable0 [16] ;
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame4MvMBModeTable1 [16] ;
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame4MvMBModeTable2 [16] ;
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame4MvMBModeTable3 [16] ;
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame1MvMBModeTable0 [10] ;
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame1MvMBModeTable1 [10] ;
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame1MvMBModeTable2 [10] ;
extern tPackHuffmanCode_WMV  sm_HufInterlaceFrame1MvMBModeTable3 [10] ;
extern tPackHuffmanCode_WMV  sm_HufCBPCYTable_Interlace_1[64] ; 
extern tPackHuffmanCode_WMV  sm_HufCBPCYTable_Interlace_2[64] ; 
extern tPackHuffmanCode_WMV  sm_HufCBPCYTable_Interlace_3[64] ; 
extern tPackHuffmanCode_WMV  sm_HufCBPCYTable_Interlace_4[64] ; 
extern tPackHuffmanCode_WMV  sm_HufCBPCYTable_Interlace_5[64] ; 
extern tPackHuffmanCode_WMV  sm_HufCBPCYTable_Interlace_6[64] ; 
extern tPackHuffmanCode_WMV  sm_HufCBPCYTable_Interlace_7[64] ; 
extern tPackHuffmanCode_WMV  sm_HufCBPCYTable_Interlace_8[64] ; 
extern tPackHuffmanCode_WMV  sm_HufMVTable_Interlace_MV1[127] ; 
extern tPackHuffmanCode_WMV  sm_HufMVTable_Interlace_MV2[127] ; 
extern tPackHuffmanCode_WMV  sm_HufMVTable_Interlace_MV3[127] ; 
extern tPackHuffmanCode_WMV  sm_HufMVTable_Interlace_MV4[127] ; 
extern tPackHuffmanCode_WMV  sm_HufMVTable_Interlace_MV5[127] ; 
extern tPackHuffmanCode_WMV  sm_HufMVTable_Interlace_MV6[127] ; 
extern tPackHuffmanCode_WMV  sm_HufMVTable_Interlace_MV7[127] ; 
extern tPackHuffmanCode_WMV  sm_HufMVTable_Interlace_MV8[127] ; 

    extern tPackHuffmanCode_WMV sm_HufMVTable_Progressive_MV1[73];
    extern tPackHuffmanCode_WMV sm_HufMVTable_Progressive_MV2[73];
    extern tPackHuffmanCode_WMV sm_HufMVTable_Progressive_MV3[73];
    extern tPackHuffmanCode_WMV sm_HufMVTable_Progressive_MV4[73];


//extern I32_WMV  sm_HufArray_TrailingOnesForNotAllOnes_8x8[84] ; 
extern I32_WMV  sm_HufArray_NumNonOneLevels_8x8[610] ; 
extern I32_WMV  sm_HufArray_RunOfOnes_8x8[519] ; 
extern I32_WMV  sm_HufArray_IntraTrailingOnesForNotAllOnes_Y[84] ; 
//extern I32_WMV  sm_HufArray_IntraNumNonOneLevels_Y[332] ; 
extern I32_WMV  sm_HufArray_IntraRunOfOnes_Y[173] ; 


 
//I32_WMV* sm_pHufArray_NumNonOneLevels[5] ; 
//I32_WMV* sm_pHufArray_RunOfOnes[5] ; 

//I32_WMV* sm_pHufArray_IntraRunOfOnes[2] ; 
