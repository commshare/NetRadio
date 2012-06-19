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
/// \file    : IVideoPlayer.h
/// \brief   : ��Ƶ���Žӿ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-16
///============================================================================
#ifndef __I_VIDEO_PLAYER_H__
#define __I_VIDEO_PLAYER_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"
#include "DeviceDefine.h"

//=============================================================================
// {278865DF-0E4A-4E77-ACE4-3BED25587F5B}
DEFINE_GUID(CLSID_IVideoPlayer, 
0x278865df, 0xe4a, 0x4e77, 0xac, 0xe4, 0x3b, 0xed, 0x25, 0x58, 0x7f, 0x5b);

//=============================================================================
class IVideoPlayer
{
public:
	/// ��
	virtual BOOL Open(HWND hWndVideo) = 0;
	/// �ر�
	virtual void Close(void) = 0;

	/// �Ƿ��
	virtual BOOL IsOpened(void) const = 0;

	/// ��ʼ����
	virtual BOOL StartPlay(void) = 0;
	/// ��ͣ����
	virtual BOOL PausePlay(void) = 0;
	/// ֹͣ����
	virtual BOOL StopPlay(void) = 0;

	/// ������Ƶ����ʾ����
	virtual BOOL SetDisplayWindow(HWND hWndVideo) = 0;

	/// ����ȫ��
	virtual BOOL SetFullScreen(BOOL bIsFullScreen) = 0;
	/// �Ƿ�ȫ��
	virtual BOOL IsFullScreen(void) const = 0;

	/// ������Ƶ��Ϣ
	virtual BOOL SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
		uint16_t nFrameRate) = 0;
	/// �����Ƶ��Ϣ
	virtual BOOL GetVideoFormat(uint16_t* pVideoWidth, uint16_t* pVideoHeight, 
		uint16_t* pFrameRate) const = 0;

	/// ������Ƶ����
	virtual void OnVideoData(const char* pVideoData, uint32_t nDataSize, 
		uint64_t nTimeStamp) =0;
};

#endif //__I_VIDEO_CAPTURE_H__
