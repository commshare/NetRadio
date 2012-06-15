/******************************************************************************
* Copyright (c) 2012, Letion
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of California, Berkeley nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
///============================================================================
/// \file    : CaptureDeviceAPI.cpp
/// \brief   : �ɼ��豸APIʵ���ļ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-15
///============================================================================

#include "ICaptureDevice.h"
#include "CaptureDevice.h"

//=============================================================================
/// �����ɼ��豸�ӿ�
IRESULT CreateCaptureDevice(const CLSID& oInterfaceID, void** ppInterface)
{
	IRESULT liResult = I_FAIL;
	if(IsEqualCLSID(CLSID_ICaptureDevice, oInterfaceID))
	{
		*ppInterface = (ICaptureDevice*)new CCaptureDevice;
		liResult = I_SUCCEED;
	}
	else
	{
		liResult = I_NOINTERFACE;
	}
	return liResult;
}

/// �ͷŲɼ��豸�ӿ�
IRESULT DestroyCaptureDevice(const CLSID& oInterfaceID, void* pInterface)
{
	if(NULL == pInterface)
		return I_INVALIDARG;

	IRESULT liResult = I_FAIL;
	if(IsEqualCLSID(CLSID_ICaptureDevice, oInterfaceID))
	{
		CCaptureDevice* pCaptureDevice = (CCaptureDevice*)pInterface;
		delete pCaptureDevice;
		pCaptureDevice = NULL;
		liResult = I_SUCCEED;
	}
	else
	{
		liResult = I_NOINTERFACE;
	}

	return liResult;
}
