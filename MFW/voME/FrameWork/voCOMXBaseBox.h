	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseBox.h

	Contains:	voCOMXBaseBox header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXBaseBox_H__
#define __voCOMXBaseBox_H__

#include "voCOMXBaseHole.h"
#include "voCOMXBaseObject.h"

typedef enum vomeBOX_TYPE
{
	vomeBOX_TYPE_UNKNOWN	= 0x00,
	vomeBOX_TYPE_SOURCE,
	vomeBOX_TYPE_FILTER,
	vomeBOX_TYPE_RENDER
} vomeBOX_TYPE;

typedef enum{
	VOBOX_CMD_DisablePort			= 0,			/*!<Disable the ports */
	VOBOX_CMD_AllocBuffer			= 1,			/*!<Alloc the buffer */
	VOBOX_CMD_FreeBuffer			= 2,			/*!<Free the buffer */
	VOBOX_CMD_StartBuffer			= 3,			/*!<Start the buffer */
	VOBOX_CMD_Flush					= 4,			/*!<Flush component */
	VOBOX_CMD_StateLoaded			= 9,			/*!<Set the component state to loaded */
	VOBOX_CMD_StateIdle				= 10,			/*!<Set the component state to Idle */
	VOBOX_CMD_StatePause			= 11,			/*!<Set the component state to Pause */
	VOBOX_CMD_StateExecute			= 12,			/*!<Set the component state to Execute */
	VOBOX_CMD_MAX					= 0X7FFFFFFF,
}VOBOX_Command;

typedef enum{
	VOBOX_STATUS_Invalid			= 0,			/*!<The status is Invalid */
	VOBOX_STATUS_Loaded				= 1,			/*!<The status is Loaded */
	VOBOX_STATUS_Idle				= 2,			/*!<The status is Idle */
	VOBOX_STATUS_Executing			= 3,			/*!<The status is Executing */
	VOBOX_STATUS_Pause				= 4,			/*!<The status is Pause */
	VOBOX_STATUS_WaitForResource	= 5,			/*!<The status is Wait for resource */
	VOBOX_STATUS_Flushing			= 6,			/*!<The status is Flushing */
	VOBOX_STATUS_MAX				= 0X7FFFFFFF,
}VOBOX_Statue;

#define VO_IS_HW_DECODER(nHardwareComponent)	((nHardwareComponent != 0) && (nHardwareComponent & 0x1))
#define VO_IS_HW_ENCODER(nHardwareComponent)	((nHardwareComponent != 0) && !(nHardwareComponent & 0x1))

#define VOBOX_HARDWARECOMPONENT_QCOM_DECODER			1
#define VOBOX_HARDWARECOMPONENT_QCOM_ENCODER			2

#define VOBOX_HARDWARECOMPONENT_TI_DECODER				3
#define VOBOX_HARDWARECOMPONENT_TI_ENCODER				4

#define VOBOX_HARDWARECOMPONENT_NVIDIA_DECODER			5
#define VOBOX_HARDWARECOMPONENT_NVIDIA_ENCODER			6

#define VOBOX_HARDWARECOMPONENT_SAMSUNG_DECODER			7
#define VOBOX_HARDWARECOMPONENT_SAMSUNG_ENCODER			8

#define VOBOX_HARDWARECOMPONENT_HUAWEI_DECODER			9
#define VOBOX_HARDWARECOMPONENT_HUAWEI_ENCODER			10

#define VOBOX_HARDWARECOMPONENT_MARVELL_DECODER			11
#define VOBOX_HARDWARECOMPONENT_MARVELL_ENCODER			12

#define VOBOX_HARDWARECOMPONENT_STE_DECODER				13
#define VOBOX_HARDWARECOMPONENT_STE_ENCODER				14

#define VOBOX_HARDWARECOMPONENT_SEU_DECODER				15


#define VOBOX_HARDWARECOMPONENT_FSL_DECODER				17
#define VOBOX_HARDWARECOMPONENT_FSL_ENCODER				18

class voCOMXCompBaseChain;
// wrapper for whatever critical section we have
class voCOMXBaseBox : public voCOMXBaseObject
{
public:
    voCOMXBaseBox(voCOMXCompBaseChain * pChain);
    virtual ~voCOMXBaseBox(void);

	virtual void				SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData);

	virtual OMX_ERRORTYPE		SetComponent (OMX_COMPONENTTYPE * pComponent, OMX_STRING pName);
	virtual OMX_COMPONENTTYPE *	GetComponent (void);
	virtual OMX_STRING			GetComponentName (void);

	virtual OMX_ERRORTYPE		SetHeadder (OMX_PTR pHeader, OMX_U32 nSize);

	virtual OMX_ERRORTYPE		CreateHoles (void);
	virtual OMX_U32				GetHoleCount (void);
	virtual voCOMXBaseHole *	GetHole (OMX_U32 nIndex);

	virtual OMX_S32				GetDuration (void);
	virtual OMX_S32				GetPos (void);
	virtual OMX_S32				SetPos (OMX_S32 nPos);

	virtual OMX_ERRORTYPE		SendCommand (OMX_COMMANDTYPE nCmd, OMX_U32 nParam, OMX_PTR pData);
	virtual OMX_ERRORTYPE		GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	virtual OMX_ERRORTYPE		SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	virtual OMX_ERRORTYPE		GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);
	virtual OMX_ERRORTYPE		SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);

	virtual OMX_ERRORTYPE		DisablePorts (void);

	virtual OMX_ERRORTYPE		AllocBuffer (void);
	virtual OMX_ERRORTYPE		FreeBuffer (void);

	virtual OMX_ERRORTYPE		StartBuffer (void);

	virtual OMX_ERRORTYPE		Flush (void);
	virtual OMX_BOOL			IsFlush (void);

	virtual OMX_STATETYPE		GetCompState (void);
	virtual OMX_TRANS_STATE		GetTransState (void);
	virtual vomeBOX_TYPE		GetCompType (void);
	virtual voCOMXCompBaseChain* GetChain (void) {return m_pChain;}
	virtual voCOMXThreadMutex *	GetCompMutex (void) {return &m_tmComp;}

	virtual OMX_BOOL			GetOMXError() {return m_bOMXError;}

	virtual void				SetPortSettingsChanged(OMX_BOOL bPortSettingsChanged) {m_bPortSettingsChanged = bPortSettingsChanged;}
	virtual OMX_BOOL			IsPortSettingsChanged() {return m_bPortSettingsChanged;}
	virtual void				SetPortSettingsChanging(OMX_BOOL bPortSettingsChanging) {m_bPortSettingsChanging = bPortSettingsChanging;}
	virtual OMX_BOOL			IsPortSettingsChanging() {return m_bPortSettingsChanging;}
	virtual OMX_U32				GetHardwareComponentType() {return m_nHardwareComponent;}

	virtual OMX_U32				GetVideoFourCC(){return mnVideoFourCC;}
	virtual OMX_U32				GetAudioFourCC(){return mnAudioFourCC;}

public:
	virtual OMX_ERRORTYPE		EventHandler (OMX_EVENTTYPE eEvent,OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	virtual OMX_ERRORTYPE		EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE		FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);

	static OMX_U32				SetStateIdleProc(OMX_PTR pParam);
	OMX_U32						SetStateIdleProcB();

	static OMX_U32				FlushProc(OMX_PTR pParam);
	OMX_U32						FlushProcB();

	bool                        isHoneyComb();

	virtual OMX_BOOL			NativeWindow_IsUsable();

protected:
	virtual void				ReleaseHoles (void);

protected:
	voCOMXCompBaseChain *		m_pChain;
	OMX_COMPONENTTYPE *			m_pComponent;
	OMX_S8						m_szCompName[128];

	OMX_S8						m_szName[128];
	OMX_VERSIONTYPE				m_verMain;
	OMX_VERSIONTYPE				m_verSpec;
	OMX_UUIDTYPE				m_uuidType;

	vomeBOX_TYPE				m_uType;
	voCOMXThreadMutex			m_tmComp;

	OMX_STATETYPE				m_sState;
	OMX_TRANS_STATE				m_sTrans;
	OMX_BOOL					m_bFlush;

	OMX_U32						m_nHoles;
	voCOMXBaseHole **			m_ppHole;

	VOMECallBack				m_pCallBack;
	OMX_PTR						m_pUserData;
	char						m_szLog[512];
	
	OMX_BOOL					m_bOMXError;

	OMX_BOOL					m_bPortSettingsChanged;
	OMX_BOOL					m_bPortSettingsChanging;

	OMX_U32						m_nHardwareComponent;
	OMX_BOOL					m_bPretendPause;
	OMX_U32						mnVideoFourCC;
	OMX_U32						mnAudioFourCC;
	bool                        mIsHComb;
};

#endif //__voCOMXBaseBox_H__
