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
/// \file    : IVolumeControl.h
/// \brief   : �������ƽӿ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-01
///============================================================================
#ifndef __I_VOLUME_CONTROL_H__
#define __I_VOLUME_CONTROL_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"

//=============================================================================
// �����豸
enum ENUM_VOLUME_DEVICE
{
	ENUM_VOLUME_NONE				= 0,			///< ռλ������
	ENUM_VOLUME_CD					= 1,			///< CD�豸
	ENUM_VOLUME_WAVE				= 2,			///< WAVE�豸
	ENUM_VOLUME_MIDI				= 3,			///< MIDI�豸
	ENUM_VOLUME_LINEIN				= 4,			///< LineIn
	ENUM_VOLUME_SPEAKER				= 5,			///< Speaker
	ENUM_VOLUME_MICROPHONE			= 6,			///< Microphone

	ENUM_VOLUME_WAVAIN_RECORD		= 20,
	ENUM_VOLUME_LINEIN_RECORD		= 21,
	ENUM_VOLUME_MICROPHONE_RECORD	= 22,
};

//=============================================================================
// {7AF58DC8-7B60-44ED-A7D5-BF5CA2F7C0F7}
DEFINE_GUID(CLSID_IVolumeControl, 
	0x7af58dc8, 0x7b60, 0x44ed, 0xa7, 0xd5, 0xbf, 0x5c, 0xa2, 0xf7, 0xc0, 0xf7);

//=============================================================================
// class IVolumeControl
class IVolumeControl
{
public:
	/// ����������
	virtual BOOL Open(ENUM_VOLUME_DEVICE enVolumeDevice) = 0;
	/// �ر���������
	virtual void Close(void) = 0;

	/// �ж��Ƿ���
	virtual BOOL IsMute(void) const = 0;
	/// ���þ���
	virtual BOOL SetMutex(BOOL bIsMutex) = 0;

	/// ��������
	virtual BOOL SetVolume(uint32_t nLValue, uint32_t nRValue) = 0;
	/// �������
	virtual BOOL GetVolume(uint32_t* pLValue, uint32_t* pRValue) const = 0;
};

#endif //__I_VOLUME_CONTROL_H__
