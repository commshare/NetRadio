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
/// \file    : AudioCaputre.h
/// \brief   : ��Ƶ�ɼ���ͷ�ļ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-17
///============================================================================
#ifndef __AUDIO_CAPTURE_H__
#define __AUDIO_CAPTURE_H__

#include "dshow\\streams.h"
#include "IAudioCapture.h"
#include "GraphBuilder.h"
#include "AudioRenderer.h"

//=============================================================================
class CAudioCapture : public IAudioCapture, CGraphBuilder
{
public:
	CAudioCapture(void);
	~CAudioCapture(void);

public:
	/// ����Ƶ�ɼ��豸
	virtual BOOL Open(ICaptureEvent* pCaptureEvent, 
		const TCHAR* szDeviceName = NULL);
	/// �ر���Ƶ�ɼ��豸
	virtual void Close(void);

	/// �豸�ɼ��豸�Ƿ��
	virtual BOOL IsOpened(void) const;

	/// ��ʼ��Ƶ�ɼ�
	virtual BOOL StartCapture(void);
	/// ��ͣ��Ƶ�ɼ�
	virtual BOOL PauseCapture(void);
	/// ֹͣ��Ƶ�ɼ�
	virtual BOOL StopCapture(void);

	/// �����Ƶ����
	virtual BOOL GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
		ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const;
	/// ������Ƶ����
	virtual BOOL SetAudioFormat
		(ENUM_FREQUENCY_TYPE enFrequency = ENUM_FREQUENCY_22KHZ, 
		ENUM_CHANNEL_TYPE enChannel = ENUM_CHANNEL_STEREO,
		ENUM_SAMPLE_TYPE enSample = ENUM_SAMPLE_16BIT);

	/// ����豸�б� 
	virtual uint16_t GetAudioDeviceInfo(
		device_info_t* pArrDeviceInfo, uint16_t nArrCount);

protected:
	/// �ײ��¼�֪ͨ
	virtual void OnNotify(int nNotifyCode);

private:
	/// �����ɼ�Filter
	IBaseFilter* CreateCaptureFiler(const TCHAR* szDeviceName);

private:
	ICaptureGraphBuilder2* m_pCGBuilder;	///< ICaptureGraphBuilder2
	IBaseFilter* m_pCaptureFilter;			///< �����豸Filter
	CAudioRenderer* m_pAudioReander;		///< Audio Render Filter

	ENUM_FREQUENCY_TYPE m_enFrequency;		///< ����Ƶ��
	ENUM_CHANNEL_TYPE m_enChannel;			///< ������
	ENUM_SAMPLE_TYPE m_enSample;			///< ����λ��
};

#endif //__AUDIO_CAPTURE_H__
