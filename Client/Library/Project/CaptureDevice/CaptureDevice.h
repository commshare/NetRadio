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
/// \file    : CaptureDevice.h
/// \brief   : �ɼ��豸��
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-14
///============================================================================
#ifndef __CAPTURE_DEVICE_H__
#define __CAPTURE_DEVICE_H__

#include "ICaptureDevice.h"

//=============================================================================
// class CCaptureDevice
class CCaptureDevice : public ICaptureDevice
{
public:
	CCaptureDevice(void);
	~CCaptureDevice(void);

public:
	/// ����豸����
	virtual uint16_t GetDeviceCount(ENUM_DEVICE_TYPE enDeviceType) const;

	/// ����豸�б� 
	virtual uint16_t GetDeviceList(ENUM_DEVICE_TYPE enDeviceType, 
		capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const;

	/// ���ݸ�����Ƶ�豸���Ƶõ��õ����豸��Filter
	virtual IBaseFilter* GetDeviceFilter(ENUM_DEVICE_TYPE enDeviceType, 
		TCHAR* pszDeviceName, uint16_t nNameSize) const;

	/// �õ�ѡ��Filter���豸����
	virtual ENUM_VIDEO_DEVICE_TYPE GetDeviceType(IBaseFilter* apFilter) const;	

private:
	/// �����Ƶ�����豸�б�
	uint16_t GetAudioInputList(capture_device_info_t* pArrDeviceInfo, 
		uint16_t nArrCount) const;
	/// �����Ƶ����豸�б�
	uint16_t GetAudioOutputList(capture_device_info_t* pArrDeviceInfo, 
		uint16_t nArrCount) const;
	/// �����Ƶ�����豸�б�
	uint16_t GetVideoInputList(capture_device_info_t* pArrDeviceInfo, 
		uint16_t nArrCount) const;

	/// �����Ƶ�����豸Filter
	IBaseFilter* GetAudioInputFilter(TCHAR* pszDeviceName, uint16_t nNameSize) const;
	/// �����Ƶ����豸Filter
	IBaseFilter* GetAudioOutputFilter(TCHAR* pszDeviceName, uint16_t nNameSize) const;
	/// �����Ƶ�����豸Filter
	IBaseFilter* GetVideoInputFilter(TCHAR* pszDeviceName, uint16_t nNameSize) const;
};

#endif //__CAPTURE_DEVICE_H__
