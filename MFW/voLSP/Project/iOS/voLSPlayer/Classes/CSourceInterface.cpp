/*
 *  CSourceInterface.cpp
 *
 *  Created by Lin Jun on 4/28/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CSourceInterface.h"

CSourceInterface::CSourceInterface(void * pUserData, SENDDATACB fSendData)
:m_pUserData(pUserData)
,m_fSendData(fSendData)
{
}

CSourceInterface::~CSourceInterface(void)
{
}

