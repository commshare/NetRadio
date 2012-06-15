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
/// \file    : ICaptureDevice.h
/// \brief   : �ɼ��豸�ӿ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-14
///============================================================================

#ifndef __I_CAPTURE_DEVICE_H__
#define __I_CAPTURE_DEVICE_H__

#include "dshow\\streams.h"
#include "types\\stdint.h"
#include "InterfaceDefine.h"

//=============================================================================
/// �豸����
enum ENUM_DEVICE_TYPE
{
	ENUM_DEVICE_UNKNOWN = 0,			///< δ֪�豸
	ENUM_DEVICE_AUDIO_INPUT	= 1,		///< ��Ƶ�����豸
	ENUM_DEVICE_AUDIO_OUTPUT = 2,		///< ��Ƶ����豸
	ENUM_DEVIDE_VIDEO_INPUT = 3,		///< ��Ƶ�����豸
};

/// ��Ƶ�豸����
enum ENUM_VIDEO_DEVICE_TYPE
{
	ENUM_VIDEO_DEVICE_UNKNOWN = 0,		///< δ֪���豸����
	ENUM_VIDEO_DEVICE_WDM	  = 1,		///< WDM��Ƶ�����豸
	ENUM_VIDEO_DEVICE_VFW	  = 2,		///< VFW��Ƶ�����豸
	ENUM_VIDEO_DEVICE_DV	  = 3,		///< DV�豸
};

/// �豸���Ƴ���
#define MAX_DEVICE_NAME_SIZE	1024

/// �豸��Ϣ
typedef struct _capture_device_info
{
	TCHAR m_szDeviceName[MAX_DEVICE_NAME_SIZE];	///< �豸����
	uint32_t m_nDeviceProperty;					///< �豸����
}capture_device_info_t;

//=============================================================================
// {B339FE10-4BCA-4129-B376-B897D43B2CC1}
DEFINE_GUID(CLSID_ICaptureDevice, 
	0xb339fe10, 0x4bca, 0x4129, 0xb3, 0x76, 0xb8, 0x97, 0xd4, 0x3b, 0x2c, 0xc1);

//=============================================================================
class ICaptureDevice
{
	/// ����豸����
	virtual uint16_t GetDeviceCount(ENUM_DEVICE_TYPE enDeviceType) const = 0;

	/// ����豸�б� 
	virtual uint16_t GetDeviceList(ENUM_DEVICE_TYPE enDeviceType, 
		capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const = 0;

	/// ���ݸ�����Ƶ�豸���Ƶõ��õ����豸��Filter
	virtual IBaseFilter* GetDeviceFilter(ENUM_DEVICE_TYPE enDeviceType, 
		TCHAR* pszDeviceName, uint16_t nNameSize) const = 0;

	/// �õ�ѡ��Filter���豸����
	virtual ENUM_VIDEO_DEVICE_TYPE GetDeviceType(IBaseFilter* apFilter) const = 0;	
};

//=============================================================================
/// �����ɼ��豸�ӿ�
IRESULT CreateCaptureDevice(const CLSID& oInterfaceID, void** ppInterface);

/// �ͷŲɼ��豸�ӿ�
IRESULT DestroyCaptureDevice(const CLSID& oInterfaceID, void* pInterface);

#endif //__I_CAPTURE_DEVICE_H__
