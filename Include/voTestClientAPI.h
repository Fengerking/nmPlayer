/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __VO_TEST_CLIENT_API_H_
#define __VO_TEST_CLIENT_API_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    
/**
 * Defination of return error code
 */
#define	VO_TC_ERR_None                      0X00000000	/*!<No Error */
#define	VO_TC_ERR_OutMemory                 0X80000001	/*!<Error is out of memory */
#define	VO_TC_ERR_Pointer                   0X80000002	/*!<The pointer is invoid */
#define	VO_TC_ERR_ParamID                   0X80000003	/*!<The param id was not supported */
#define	VO_TC_ERR_Status                    0X80000004	/*!<The status is wrong */
#define	VO_TC_ERR_Implement                 0X80000005	/*!<This function was not impement */
#define	VO_TC_ERR_Unknown                   0X80000006	/*!<Unknow error*/

/**
 * Defination call back ID
 */
#define	VO_TC_CB_CONNECT_SUCCESS            0X00000000	/*!<Connect success. */
#define	VO_TC_CB_CONNECT_FAIL               0X00000001	/*!<Start a job fail. */
#define	VO_TC_CB_NEW_JOB                    0X00000002	/*!<Dispatch a new job, param1:VO_TC_JOB *. */
#define	VO_TC_CB_SOURCE_DOWNLOAD_START      0X00000003	/*!<start download source of current job, param1:int *(The index of sub job which need resume). */
#define	VO_TC_CB_SOURCE_DOWNLOAD_PERCENT    0X00000004	/*!<download source percent of current job, param1:int *(The index of sub job which need resume). */
#define	VO_TC_CB_SOURCE_DOWNLOAD_FAIL       0X00000005	/*!<download source of current job fail, param1:int *(The index of sub job which need resume). */    
#define	VO_TC_CB_UPLOAD_RP_START            0X00000006	/*!<start upload report of current job, param1:int *(The index of sub job which need resume). */
#define	VO_TC_CB_UPLOAD_RP_FAIL             0X00000007	/*!<upload report of current job fail, param1:int *(The index of sub job which need resume). */
#define	VO_TC_CB_UPLOAD_RP_SUCCESS          0X00000008	/*!<upload report of current job success, param1:int *(The index of sub job which need resume). */
#define	VO_TC_CB_RESUME_JOB                 0X00000009	/*!<Resume current job, param1:int *(The index of sub job which need resume). */
#define	VO_TC_CB_STOP_CURRENT_JOB           0X0000000A	/*!<Stop current job */
#define	VO_TC_CB_CANCEL_ALL_JOB             0X0000000B	/*!<Cancel current job list */
#define VO_TC_CB_API_TEST                   0X0000000C  /*!<For test api */
/**
 * Call back function. Update the status from SDK
 * \param nID [in] the id of special event
 * \param pParam1 [in/out] the first parameter
 * \param pParam2 [in/out] the second parameter
 */
typedef int (* VOTCListener) (void * pUserData, int nID, void * pParam1, void * pParam2);

/*!
 * Definition of even callback
 */	
typedef struct 
{
    VOTCListener    pListenerFunc;
    void*			pUserData;
}VOTC_LISTENERINFO;


/**
 * Enumeration for sub job's status
 */
typedef enum
{
    TC_SUBJOB_STATUS_WAITING,         /*!<waiting for download and or play */
    TC_SUBJOB_STATUS_READY,           /*!<ready */
    TC_SUBJOB_STATUS_RUNNING,         /*!<running a job */
    TC_SUBJOB_STATUS_FINISHED,        /*!<succeeded to run a job */
    TC_SUBJOB_STATUS_FAILED,          /*!<failed to run a job (doesn't mean the test case failed). */
    TC_SUBJOB_STATUS_CANCELLED,       /*!<cancelled a job as user required */
}VO_TC_SUBJOB_STATUS;


/**
 * Enumeration for sub job's source status
 */
typedef enum
{
    TC_SOURCE_STATUS_NOT_READY,         /*!<The source is not ready */
    TC_SOURCE_STATUS_DOWNLOADING,       /*!<The source is downloading*/
    TC_SOURCE_STATUS_NOT_AVAILABLE,     /*!<The source is not available*/
    TC_SOURCE_STATUS_READY,             /*!<The source is ready */
}VO_TC_SOURCE_STATUS;

typedef enum
{
    TC_URL_TYPE_LOCAL,
    TC_URL_TYPE_DOWNLOAD,
    TC_URL_TYPE_LINK,
}VO_TC_URL_TYPE;

typedef enum
{
    TC_REPORT_STATUS_INIT,              /*!<The init status of report */
    TC_REPORT_STATUS_UPLOADING,         /*!<Report is uploading */
    TC_REPORT_STATUS_UPLOAD_FAIL,       /*!<Report is upload fail */
    TC_REPORT_STATUS_UPLOAD_SUCCESS,    /*!<Report is upload success */
}TC_REPORT_STATUS;

/**
 * job info
 */
typedef struct
{
    TC_REPORT_STATUS       nStatus;              /*!< the status of report. */
    char                   szStart[256];         /*!< the time of when start test. */
    int                    nInit;                /*!< return value of Init. */
    int                    nOpen;                /*!< return value of Open. */
    int                    nRun;                 /*!< return value of Run. */
    int                    nStop;                /*!< return value of Stop. */
    int                    nClose;               /*!< return value of Close. */
    int                    nUninit;              /*!< return value of Uninit. */
    char                   szStop[256];            /*!< the time of when stop test. */
}VO_TC_SUB_JOB_TP_REPORT;

/**
 * job info
 */
typedef struct
{
    char* pHost;
    int   nPort;
    char* pUser;
    char* pPass;
}VO_TC_SUB_JOB_FTP;

/**
 * job info
 */
typedef struct
{
    char*                   pSubJobId;                    /*!< The related job ID. */
    VO_TC_SOURCE_STATUS     nSourceStatus;         /*!< The status of this sub job's source. */
    VO_TC_SUBJOB_STATUS     nSubJobStatus;         /*!< The status of this sub job. */
    VO_TC_URL_TYPE          nUrlType;              /*!< The type of this type. */
    char*                   pUrlFromServer;        /*!< The url get from server*/
    char*                   pUrlForPlayback;       /*!< The url of this sub job for playback */
    VO_TC_SUB_JOB_FTP*      pSubFtp;
    void*                   pReport;               /*!< The test report VO_TC_SUB_JOB_TP_REPORT* */
}VO_TC_SUB_JOB;

typedef struct
{
    char*               pOutputFtpHost;
    char*               pOutputFtpUser;
    char*               pOutputFtpPass;
    char*               pOutputFtpPath;
    int                 nOutputFtpPort;
}VO_TC_OUTPUT;
    
/**
 * job info
 */
typedef struct
{
    int                 nJobId;                    /*!< The related job ID. */
//    char*               pTargetName;               /*!< Running at this target(e.p. application target)*/
//    char*               pTargetDownloadUrl;        /*!< The url to download target */
    int                 nTestDuration;             /*!< The duration to test for every sub job*/
//    int                 nPerformanceInterval;
    int                 nSubJobsCount;             /*!< The need test count of this job*/
    VO_TC_SUB_JOB**     pSubJobs;                  /*!< The need test list*/
    VO_TC_OUTPUT        cOutput;
}VO_TC_JOB;

typedef enum
{
    VO_TEST_API_PARAM_TYPE_LONG         = 0,
    VO_TEST_API_PARAM_TYPE_STRING       = 1,
    VO_TEST_API_PARAM_TYPE_CALLBACK     = 2,
    VO_TEST_API_PARAM_TYPE_COMMENT      = 3,
    VO_TEST_API_PARAM_TYPE_TIME         = 4,
    VO_TEST_API_PARAM_TYPE_FUNCTION     = 5,
    VO_TEST_API_PARAM_TYPE_MATRIX       = 0x00010000,
    VO_TEST_API_PARAM_TYPE_STRUCTURE    = 0x00020000, 
}VO_TEST_API_PARAM_TYPE;

typedef struct
{
    int nMonth;
    int nDay;
    int nHour;
    int nMin;
    int nSecond;
}VO_TEST_API_TIME;

typedef struct
{
    VO_TEST_API_PARAM_TYPE paramType;
    void* pValue;
    long nValueSize;
}VO_TEST_API_PARAM;

typedef struct
{
    int nArgCount;
    VO_TEST_API_PARAM* pParam;
}VO_TEST_API_PARAM_ARRAY;

typedef struct
{
    VO_TEST_API_PARAM_ARRAY inputParamArray;
    VO_TEST_API_PARAM_ARRAY* pOutputParamArray;
}VO_TEST_API;

typedef struct
{
    int nTestApiCount;
    VO_TEST_API* pApi;
}VO_TEST_API_CASE;


/**
* Test client function set
*/
typedef struct
{
	/**
     * Initialize test client instance.
     * This function should be called first for a session.
     * \param phHandle [out] Instance handle.
     * \retval VO_TC_ERR_None Succeeded 
     */
	int (* Init) (void** phHandle, VOTC_LISTENERINFO* pListener);

	
	/**
     * Destroy test client instance.
     * This function should be called last of the session.
     * \param pHandle [in] The instance handle. Created by Init().
     * \retval VO_TC_ERR_None Succeeded
     */
	int (* Uninit) (void* pHandle);


    /**
     * Set status of sub job.
     * \param pHandle [in] The instance handle. Created by Init().
     * \param nStatus [in] The staus to set.
     * \param nIndex  [in] The index of current sub job.
     * \retval VO_TC_ERR_None Succeeded
     */
    int (* SetSubJobStatus) (void* pHandle, VO_TC_SUBJOB_STATUS nStatus, int nIndex);
    
    
	/**
     * Get the special value from param ID
     * \param pHandle [in] The instance handle. Created by Init().
     * \param nParamID [in] The param ID
     * \param pValue [out] The return value depend on the param ID.
     * \retval VO_TC_ERR_None Succeeded
     */
	int (* GetParam) (void* pHandle, int nParamID, void* pValue);


	/**
     * Set the special value from param ID
     * \param pHandle [in] The instance handle. Created by Init().
     * \param nParamID [in] The param ID
     * \param pValue [in] The set value depend on the param ID.
     * \retval VO_TC_ERR_None Succeeded 
     */
	int (* SetParam) (void* pHandle, int nParamID, void* pValue);
    
    
	/**
     * Set the special value from param ID
     * \dwTestCaseID  RESERVED
     * \retval true if Succeeded. Otherwise false
     */
    bool (* SendReport)(void* pHandle, unsigned int dwTestCaseID);
    
}voTestClientAPI;


/**
* Get test client API interface
* \param pFuncSet [in/out] Return the API set.
* \retval VO_TC_ERR_None Succeeded.
*/
int voGetTestClientAPI(voTestClientAPI* pFuncSet);

typedef int (* VO_GET_TESTCLIENT_API)(voTestClientAPI* pFuncSet);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __VO_TEST_CLIENT_API_H_ */


