#ifndef _DS_DISPIDS_H
#define _DS_DISPIDS_H

// Passing flags to MIDL on CE doesn't seem to work...
#if defined(WMPCE) || !defined(WMPCE)
#define helpstring(s) 
#endif

#include <idispids.h>

///////////////////////////////////////////////////////////////////////////////
//                              PROPERTIES
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ActiveMovie poperties
///////////////////////////////////////////////////////////////////////////////
#define DISPID_AM_IMAGESOURCEWIDTH              4       // Duplicate prop
#define DISPID_AM_IMAGESOURCEHEIGHT             5       // Duplicate prop
#define DISPID_AM_AUTHOR                        6       // Duplicate prop
#define DISPID_AM_TITLE                         7       // Duplicate prop
#define DISPID_AM_COPYRIGHT                     8       // Duplicate prop
#define DISPID_AM_DESCRIPTION                   9       // Duplicate prop
#define DISPID_AM_RATING                       10       // Duplicate prop
#define DISPID_AM_FILENAME                     11       // Duplicate prop
#define DISPID_AM_DURATION                     12       // Duplicate prop
#define DISPID_AM_CURRENTPOSITION              13       // Duplicate prop
#define DISPID_AM_PLAYCOUNT                    14       // Duplicate prop
#define DISPID_SELECTION_START                 15
#define DISPID_SELECTION_END                   16
#define DISPID_AM_CURRENTSTATE                 17       // Duplicate prop
#define DISPID_AM_RATE                         18       // Duplicate prop
#define DISPID_VOLUME                          19
#define DISPID_BALANCE                         20
#define DISPID_AM_ENABLECONTEXTMENU            21       // Duplicate prop
#define DISPID_SHOW_DISPLAY                    22
#define DISPID_SHOW_CONTROLS                   23
#define DISPID_SHOW_POSITION_CONTROLS          24
#define DISPID_SHOW_SELECTION_CONTROLS         25
#define DISPID_SHOW_TRACKER                    26
#define DISPID_ENABLE_POSITION_CONTROLS        27
#define DISPID_ENABLE_SELECTION_CONTROLS       28
#define DISPID_ENABLE_TRACKER                  29
#define DISPID_ALLOW_HIDE_DISPLAY              30
#define DISPID_ALLOW_HIDE_CONTROLS             31
#define DISPID_DISPLAY_MODE                    32
#define DISPID_ALLOW_CHANGE_DISPLAY_MODE       33
#define DISPID_FILTERGRAPH                     34
#define DISPID_FILTER_GRAPH_DISPATCH           35
#define DISPID_DISPLAY_FORE_COLOR              36
#define DISPID_DISPLAY_BACK_COLOR              37
#define DISPID_MOVIE_WINDOW_SIZE               38
#define DISPID_FULL_SCREEN_MODE                39
#define DISPID_AM_AUTOSTART                    40        // Duplicate prop
#define DISPID_AM_AUTOREWIND                   41        // Duplicate prop
#define DISPID_BORDER_STYLE                    42
#define DISPID_ISSOUNDCARDENABLED              53
#define DISPID_INFO                            0x6002004d   // hidden

///////////////////////////////////////////////////////////////////////////////
// NSPlay control properties
///////////////////////////////////////////////////////////////////////////////
#define DISPID_IMAGESOURCEWIDTH                 1001
#define DISPID_IMAGESOURCEHEIGHT                1002
#define DISPID_DURATION                         1003
#define DISPID_AUTHOR                           1004
#define DISPID_COPYRIGHT                        1005
#define DISPID_DESCRIPTION                      1006
#define DISPID_RATING                           1007
#define DISPID_TITLE                            1008
#define DISPID_SOURCELINK                       1009
#define DISPID_MARKERCOUNT                      1010
#define DISPID_CANSCAN                          1011
#define DISPID_CANSEEK                          1012
#define DISPID_SENDKEYBOARDEVENTS               1013
#define DISPID_SENDMOUSECLICKEVENTS             1014
#define DISPID_SENDMOUSEMOVEEVENTS              1015
#define DISPID_SENDSTATECHANGEEVENTS            1016
#define DISPID_AUTOSTART                        1017
#define DISPID_AUTOREWIND                       1018
#define DISPID_ALLOWCHANGECONTROLTYPE           1019
#define DISPID_INVOKEURLS                       1020
#define DISPID_ENABLECONTEXTMENU                1021
#define DISPID_TRANSPARENTATSTART               1022
#define DISPID_TRANSPARENTONSTOP                1023
#define DISPID_CLICKTOPLAY                      1025
#define DISPID_FILENAME                         1026
#define DISPID_CURRENTPOSITION                  1027
#define DISPID_RATE                             1028
#define DISPID_CURRENTMARKER                    1029
#define DISPID_PLAYCOUNT                        1030
#define DISPID_CURRENTSTATE                     1031
#define DISPID_DISPLAYSIZE                      1032
#define DISPID_MAINWINDOW                       1033
#define DISPID_CONTROLTYPE                      1034
#define DISPID_ALLOWSCAN                        1035
#define DISPID_CREATIONDATE                     1036
#define DISPID_BANDWIDTH                        1037
#define DISPID_ERRORCORRECTION                  1038
#define DISPID_RECEIVEDPACKETS                  1039
#define DISPID_RECOVEREDPACKETS                 1040
#define DISPID_LOSTPACKETS                      1041
#define DISPID_RECEPTIONQUALITY                 1042
#define DISPID_BUFFERINGCOUNT                   1043
#define DISPID_CURSORTYPE                       1044
#define DISPID_ANIMATIONATSTART                 1045
#define DISPID_ANIMATIONONSTOP                  1046
#define DISPID_CANSEEKTOMARKERS                 1047
#define DISPID_SESSIONSTART                     1048
#define DISPID_SESSIONEND                       1049
#define DISPID_CHANNELNAME                      1050
#define DISPID_CHANNELDESCRIPTION               1051
#define DISPID_CHANNELURL                       1052
#define DISPID_CONTACTADDRESS                   1053
#define DISPID_CONTACTPHONE                     1054
#define DISPID_CONTACTEMAIL                     1055
#define DISPID_ALLOWCHANGEDISPLAYSIZE           1056
#define DISPID_CODECCOUNT                       1057
#define DISPID_ISBROADCAST                      1058
#define DISPID_ISDURATIONVALID                  1059
#define DISPID_SOURCEPROTOCOL                   1060
#define DISPID_OPENSTATE                        1061
#define DISPID_SENDOPENSTATECHANGEEVENTS        1062
#define DISPID_SENDWARNINGEVENTS                1063
#define DISPID_SENDERROREVENTS                  1064
#define DISPID_HASERROR                         1065
#define DISPID_ERRORDESCRIPTION                 1066
#define DISPID_ERRORCODE                        1067 
#define DISPID_PLAYSTATE                        1068
#define DISPID_SENDPLAYSTATECHANGEEVENTS        1069
#define DISPID_BUFFERINGTIME                    1070
#define DISPID_USEFIXEDUDPPORT                  1071
#define DISPID_FIXEDUDPPORT                     1072
#define DISPID_USEHTTPPROXY                     1073
#define DISPID_HTTPPROXYHOST                    1074
#define DISPID_HTTPPROXYPORT                    1075
#define DISPID_ENABLEMULTICAST                  1076
#define DISPID_ENABLEUDP                        1077
#define DISPID_ENABLETCP                        1078
#define DISPID_ENABLEHTTP                       1079
#define DISPID_BUFFERINGPROGRESS                1080
#define DISPID_ENABLEAUTOPROXY                  1081
#define DISPID_BASEURL                          1082
#define DISPID_DEFAULTFRAME                     1083

///////////////////////////////////////////////////////////////////////////////
// DirectShow properties
///////////////////////////////////////////////////////////////////////////////
#define DISPID_SHOWCAPTIONING	                1084
#define DISPID_CAPTIONING_ID	                1085
#define DISPID_SHOWSTATUSBAR                    1086
#define DISPID_SHOWTOOLBAR                      1087
#define DISPID_SHOWGOTOBAR                      1088
#define DISPID_MUTE                             1089
#define DISPID_FULLSCREENCONTROLS               1090
#define DISPID_PREVIEWMODE			1091
#define DISPID_DIALOGAVAILABILITY               1092
#define DISPID_CANPREVIEW                       1093
#define DISPID_HASMULTIPLEITEMS                 1094
#define DISPID_LANGUAGE				1095
#define DISPID_AUDIOSTREAM			1096
#define DISPID_SAMISTYLE			1097
#define DISPID_SAMILANG				1098
#define DISPID_SAMIFILENAME			1099
#define DISPID_STREAMCOUNT                      1100
#define DISPID_VIDEOBORDER_WIDTH                1101
#define DISPID_VIDEOBORDER_COLOR                1102
#define DISPID_VIDEOBORDER_3D                   1103

#define DISPID_CLIENTID                         1106
#define DISPID_SHOWAUDIOCONTROLS                1107
#define DISPID_ENABLEFULLSCREENCONTROLS         1108
#define DISPID_ACTIVEMOVIE                      1109
#define DISPID_NSPLAY                           1110
#define DISPID_MEDIAPLAYER                      1111
#define DISPID_WINDOWLESSVIDEO                  1112
#define DISPID_CONNECTIONSPEED                  1113


#define DISPID_MPDVD                            1500
#define DISPID_MPDVDCONTROL                     1501
#define DISPID_MPDVDINFO                        1502
#define DISPID_MPDVDCONFIG                      1503
#define DISPID_DVDNOTIFY                        1505

#define DISPID_DOEVENT                          1506

#define DISPID_DVD_TITLEPLAY                    1507
#define DISPID_DVD_CHAPTERPLAY                  1508
#define DISPID_DVD_TIMEPLAY                     1509
#define DISPID_DVD_STOPRESUME                   1510
#define DISPID_DVD_GOUP                         1511
#define DISPID_DVD_TIMESEARCH                   1512
#define DISPID_DVD_CHAPTERSEARCH                1513
#define DISPID_DVD_PREVPGSEARCH                 1514
#define DISPID_DVD_TOPPGSEARCH                  1515    
#define DISPID_DVD_NEXTPGSEARCH                 1516
#define DISPID_DVD_FORWARDSCAN                  1517
#define DISPID_DVD_BACKWARDSCAN                 1518
#define DISPID_DVD_MENUCALL                     1519    
#define DISPID_DVD_RESUME                       1520
#define DISPID_DVD_UPPERBUTTONSELECT            1521
#define DISPID_DVD_LOWERBUTTONSELECT            1522
#define DISPID_DVD_LEFTBUTTONSELECT             1523
#define DISPID_DVD_RIGHTBUTTONSELECT            1524
#define DISPID_DVD_BUTTONACTIVATE               1525
#define DISPID_DVD_BUTTONSELECTANDACTIVATE      1526
#define DISPID_DVD_STILLOFF                     1527
#define DISPID_DVD_PAUSEON                      1528
#define DISPID_DVD_PAUSEOFF                     1529
#define DISPID_DVD_MENULANGSEL                  1530
#define DISPID_DVD_AUDIOSTREAMCHANGE            1531
#define DISPID_DVD_SUBPICTURESTREAMCHANGE       1532    
#define DISPID_DVD_ANGLECHANGE                  1533
#define DISPID_DVD_PARENTALLEVELSELECT          1534
#define DISPID_DVD_PARENTALCONTRYSELECT         1535
#define DISPID_DVD_KARAOKEMODE                  1536
#define DISPID_DVD_VIDEOMODEPREF                1537
#define DISPID_DVD_ROOT                         1538
#define DISPID_DVD_MOUSEACTIVATE                1539
#define DISPID_DVD_MOUSESELECT                  1540
#define DISPID_DVD_CHAPTERPLAYAUTOSTOP          1541
#define DISPID_DVD_DOMAIN                       1542
#define DISPID_DVD_AUDIOSTREAMSAVAIL            1543
#define DISPID_DVD_CURRENTAUDIOSTREAM           1544
#define DISPID_DVD_CURRENTSUBPICTURESTREAM      1545
#define DISPID_DVD_SUBPICTURESTREAMSAVAIL       1546
#define DISPID_DVD_SUBPICTUREON                 1547
#define DISPID_DVD_CURRENTANGLE                 1548
#define DISPID_DVD_ANGLESAVAILABLE              1549

#define DISPID_DVD_NUMBEROFCHAPTERS             1550
#define DISPID_DVD_GETAUDIOLANGUAGE             1551
#define DISPID_DVD_GETBUTTON                    1552
#define DISPID_DVD_GETDOMAIN                    1553
#define DISPID_DVD_GETLOCATION                  1554
#define DISPID_DVD_GETSUBPICTURELANG            1555
#define DISPID_DVD_PARENTALLEVEL                1556

#define DISPID_DVD_GETUOPS                      1558
#define DISPID_DVD_GETALLSPRMS                  1559
#define DISPID_DVD_GETALLGPRMS                  1560
#define DISPID_DVD_GETVIDEOATTRIBUTES           1561
#define DISPID_DVD_GETAUDIOATTRIBUTES           1562
#define DISPID_DVD_GETSUBPICTUREATTRIBUTES      1563
#define DISPID_DVD_GETTITLEATTRIBUTES           1564
#define DISPID_DVD_GETVMGATTRIBUTES             1565
#define DISPID_DVD_GETDVDTEXTINFO               1566
#define DISPID_DVD_GETTITLENUM                  1567
#define DISPID_DVD_GETCHAPTERNUM                1568
#define DISPID_DVD_CURRENTTIME                  1569

#define DISPID_DVD_CURRENTBUTTON                1570
#define DISPID_DVD_BUTTONSAVAIL                 1571

#define DISPID_DVD_PARENTALCOUNTRY              1572
#define DISPID_DVD_FRAMESPERSECOND              1573
#define DISPID_DVD_CURRENTDOMAIN                1574
#define DISPID_DVD_TITLESAVAILABLE              1575
#define DISPID_DVD_VOLUMESAVAILABLE             1576
#define DISPID_DVD_CURRENTVOLUME                1577
#define DISPID_DVD_CURRENTDISCSIDE              1578
#define DISPID_DVD_CURRENTUOPS                  1579
#define DISPID_DVD_CURRENTCCSERVICE             1580
#define DISPID_DVD_CCACTIVE                     1581
#define DISPID_DVD_GETTOTALTITLETIME            1582
#define DISPID_DVD_GETTITLEPARENTALLEVEL        1583
#define DISPID_DVD_UNIQUEID                     1584
#define DISPID_DVD_COLORKEY                     1585


///////////////////////////////////////////////////////////////////////////////
//                          METHODS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ActiveMovie methods 
///////////////////////////////////////////////////////////////////////////////
#define DISPID_RUN                     0x60020001
#define DISPID_AM_PAUSE                0x60020002    // duplicate method
#define DISPID_AM_STOP                 0x60020003    // duplicate method

///////////////////////////////////////////////////////////////////////////////
// NSPlay control methods - 10 of our own currently + 1 "stock" AboutBox property
///////////////////////////////////////////////////////////////////////////////
#define DISPID_PLAY                     2001
#define DISPID_PAUSE                    2002    
#define DISPID_STOP                     2003    
#define DISPID_GETMARKERTIME            2004
#define DISPID_GETMARKERNAME            2005
#define DISPID_CANCEL                   2006
#define DISPID_GETCODECINSTALLED        2007
#define DISPID_GETCODECDESCRIPTION      2008
#define DISPID_GETCODECURL              2009
#define DISPID_OPEN                     2010                                

///////////////////////////////////////////////////////////////////////////////
// DirectShow methods
///////////////////////////////////////////////////////////////////////////////
#define DISPID_GETMOREINFOURL           2011
#define DISPID_FILEPROPERTIESDIALOG	    2012
#define DISPID_STATISTICSDIALOG	        2013
#define DISPID_GOTODIALOG	            2014
#define DISPID_STREAMSELECT             2015
#define DISPID_GETMEDIAINFOSTRING       2016
#define DISPID_DOWNLOADIMAGE            2017
#define DISPID_NAVIGATEBROWSERTOURL     2018
#define DISPID_GETSTREAMNAME            2019
#define DISPID_GETSTREAMGROUP           2020
#define DISPID_GETSTREAMSELECTED        2021
#define DISPID_PREVIOUS                 2022
#define DISPID_NEXT                     2023
#define DISPID_FASTFORWARD              2024
#define DISPID_FASTREVERSE              2025
#define DISPID_PREVIEW                  2026
#define DISPID_WAIT                     2027
#define DISPID_GETMEDIAPARAMETER        2028
#define DISPID_GETMEDIAPARAMETERNAME    2029
#define DISPID_ENTRYCOUNT               2030
#define DISPID_GETCURRENTENTRY          2031
#define DISPID_SETCURRENTENTRY          2032
#define DISPID_SHOWDIALOG               2033

///////////////////////////////////////////////////////////////////////////////
//                                  EVENTS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//                  ActiveMovie events
///////////////////////////////////////////////////////////////////////////////
#define DISPID_AM_STATECHANGE                   1   // duplicate event
#define DISPID_POSITIONCHANGE                   2
#define DISPID_TIMER                            3
#define DISPID_OPENCOMPLETE                    50
#define DISPID_DISPLAYMODECHANGE               51
#define DISPID_AM_SCRIPTCOMMAND	               52   // duplicate event
#define DISPID_AM_ERROR                        999  // duplicate event

///////////////////////////////////////////////////////////////////////////////
// NSPlay control events - 12 of our own currently + 8 "stock" keyboard + mouse events
///////////////////////////////////////////////////////////////////////////////
#define DISPID_SCRIPTCOMMAND            3001
#define DISPID_ENDOFSTREAM              3002
#define DISPID_BUFFERING                3003
#define DISPID_DISCONNECT               3004
#define DISPID_STATECHANGE              3005
#define DISPID_MARKERHIT                3006
#define DISPID_AUTOSTARTFAILURE         3007
#define DISPID_NEWSTREAM                3008
#define DISPID_WARNING                  3009
#define DISPID_ERROR                    3010
#define DISPID_OPENSTATECHANGE          3011
#define DISPID_PLAYSTATECHANGE          3012

///////////////////////////////////////////////////////////////////////////////
// DirectShow events 
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//                          INTERNAL PROPERTIES
///////////////////////////////////////////////////////////////////////////////

// Toolbar properties
#define DISPID_AMTFLAT			6000
#define DISPID_AMTDOWNLOADED		6001
#define DISPID_AMTRESET			6002
#define DISPID_AMTHIGHLIGHTCOLOR	6003
#define DISPID_AMTSHADOWCOLOR		6004
#define DISPID_AMTSHOWBORDER		6005
#define DISPID_AMTSTATICCOLOR		6006
#define DISPID_AMTDOWNLOADCOLOR		6007
#define DISPID_AMTVIEWEDCOLOR		6008
#define DISPID_AMTVIEWED		6009
#define DISPID_AMTSTATUSTEXT            6010
#define DISPID_AMTSHOWAUDIOCONTROLS     6011
#define DISPID_AMTFULLSCREEN            6012
#define DISPID_SHOWCANCEL               6013
#define DISPID_AMTMINWIDTH              6014
#define DISPID_AMTRESTARTPLAYLIST       6015
#define DISPID_REOPENURL                6016
#define DISPID_AMTALLOWPAUSE            6017

// Ad Banner control properties
#define DISPID_IMAGEURL                 6100
#define DISPID_IMAGEJUSTIFYWIDTH        6101
#define DISPID_DESTINATIONURL           6102
#define DISPID_LAUNCHBROWSER            6103
#ifndef DISPID_CLEAR
#define DISPID_CLEAR 			6104
#endif

// More toolbar id's

// common control properties
#define DISPID_CONTAINERTIMER           6200
#define DISPID_VISIBLE                  6201
// common control methods

// More toolbar id's
#define DISPID_SKIPFORWARD              6301
#define DISPID_SKIPREVERSE              6302

///////////////////////////////////////////////////////////////////////////////
//                             INTERNAL EVENTS
///////////////////////////////////////////////////////////////////////////////

// Toolbar events	
#define DISPID_AMTSTATECHANGE           8001
#define DISPID_AMTCANCEL                8002
#define DISPID_AMTDOWNLOADCOMPLETE      8003

// Player launch methods
#define DISPID_LOADMONIKER              8004
#define DISPID_LOADMONIKER2             8005
#endif // _DS_DISPIDS_H
