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
/// \file    : AudioResampleAPI.cpp
/// \brief   : 音频冲采样模块函数
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-04
///============================================================================
#include "stdafx.h"
#include "IAudioResample.h"
#include "AudioResample.h"

//=============================================================================
/// 创建接口
IRESULT CreateInterface(const CLSID& oInterfaceID, void** ppInterface)
{
	IRESULT liResult = I_FAIL;
	if(IsEqualCLSID(CLSID_IAudioResample, oInterfaceID))
	{
		*ppInterface = (IAudioResample*)new CAudioResample;
		liResult = I_SUCCEED;
	}
	else
	{
		liResult = I_NOINTERFACE;
	}
	return liResult;
}

/// 释放接口
IRESULT DestroyInterface(const CLSID& oInterfaceID, void* pInterface)
{
	if(NULL == pInterface)
		return I_INVALIDARG;

	IRESULT liResult = I_FAIL;
	if(IsEqualCLSID(CLSID_IAudioResample, oInterfaceID))
	{
		CAudioResample* pAudioResample = (CAudioResample*)pInterface;
		delete pAudioResample;
		pAudioResample = NULL;
		liResult = I_SUCCEED;
	}
	else
	{
		liResult = I_NOINTERFACE;
	}

	return liResult;
}
