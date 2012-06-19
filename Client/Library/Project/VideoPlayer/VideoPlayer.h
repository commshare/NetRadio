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
/// \file    : VideoPlayer.h
/// \brief   : ��Ƶ������ͷ�ļ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-16
///============================================================================
#ifndef __VIDEO_PLAYER_H__
#define __VIDEO_PLAYER_H__

#include "dshow\\streams.h"
#include "IVideoPlayer.h"
#include "GraphBuilder.h"
#include "VideoCapture.h"

//=============================================================================
class CVideoPlayer : public IVideoPlayer, CGraphBuilder
{
public:
	CVideoPlayer(void);
	~CVideoPlayer(void);

public:
	/// ��
	virtual BOOL Open(HWND hWndVideo);
	/// �ر�
	virtual void Close(void);

	/// �Ƿ��
	virtual BOOL IsOpened(void) const;

	/// ��ʼ����
	virtual BOOL StartPlay(void);
	/// ��ͣ����
	virtual BOOL PausePlay(void);
	/// ֹͣ����
	virtual BOOL StopPlay(void);

	/// ������Ƶ����ʾ����
	virtual BOOL SetDisplayWindow(HWND hWndVideo);
	/// ����ȫ��
	virtual BOOL SetFullScreen(BOOL bIsFullScreen);
	/// �Ƿ�ȫ��
	virtual BOOL IsFullScreen(void) const;

	/// ������Ƶ��Ϣ
	virtual BOOL SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
		uint16_t nFrameRate);
	/// �����Ƶ��Ϣ
	virtual BOOL GetVideoFormat(uint16_t* pVideoWidth, uint16_t* pVideoHeight, 
		uint16_t* pFrameRate) const;

	/// ������Ƶ����
	virtual void OnVideoData(const char* pVideoData, uint32_t nDataSize, 
		uint64_t nTimeStamp);

protected:
	virtual void OnNotify(int nNotifyCode);

private:
	CVideoCapture* m_pVideoCapture;			///< ��Ƶ�ɼ�Filter
	IBaseFilter*  m_pVideoRender;           ///< ��Ƶ����Filter
	IBasicVideo*  m_pBasicVideo;            ///< IBasicVideo
	IVideoWindow* m_pVideoWindow;           ///< IVideoWindow

	uint16_t m_nVideoWidth;					///< ��Ƶ���
	uint16_t m_nVideoHeight;				///< ��Ƶ�߶�
	uint16_t m_nFrameRate;					///< ֡��
};

#endif //__I_VIDEO_CAPTURE_H__
