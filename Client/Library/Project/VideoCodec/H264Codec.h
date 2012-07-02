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
/// \file    : H264Codec.h
/// \brief   : H264�������
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-20
///============================================================================
#ifndef __H264_CODEC_H__
#define __H264_CODEC_H__

#include "TypeDefine.h"
#include "IVideoCodec.h"
#include "ColorSpaceLib.h"
#include "h264\\Encoder\\x264.h"
#include "h264\\Decoder\\h264.h"

#ifdef _DEBUG
#pragma comment(lib, "libH264EncodeD.lib")
#pragma comment(lib, "libH264DecodeD.lib")
#pragma message("LINK libH264EncodeD.lib and libH264DecodeD.lib")
#else
#pragma comment(lib, "libH264Encode.lib")
#pragma comment(lib, "libH264Decode.lib")
#pragma message("LINK libH264Encode.lib and libH264Decode.lib")
#endif

//=============================================================================
// class CH264Encoder
class CH264Encoder
{
public:
	CH264Encoder(void);
	~CH264Encoder(void);

public:
	/// ����
	BOOL Create(void);
	/// ����
	void Destroy(void);

	/// ������Ƶ֡��Ϣ
	BOOL SetFrameInfo(uint16_t nVideoWidth, uint16_t nVideoHeight);
	/// �����Ƶ֡��Ϣ
	BOOL GetFrameInfo(uint16_t& nVideoWidth, uint16_t& nVideoHeight) const;

	/// ������Ƶ����
	BOOL SetVideoQuant(uint16_t nQuant = 85);
	/// �����Ƶ����
	uint16_t GetVideoQuant(void) const;

	/// ����֡��
	BOOL SetFrameRate(uint16_t nFrameRate);
	/// ���֡��
	uint16_t GetFrameRate(void) const;

	/// ����
	int32_t Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize);

private:
	x264_t* m_pHandleEncoder;
	x264_param_t m_EncoderParam;

	uint16_t m_nVideoWidth;					///< ��Ƶ���
	uint16_t m_nVideoHeight;				///< ��Ƶ�߶�
	uint16_t m_nVideoQuant;					///< ��Ƶ����
	uint16_t m_nFrameRate;					///< ��Ƶ֡��

	x264_picture_t m_inPicture;
	x264_picture_t m_outPicture;
};

//=============================================================================
// class CH264Decoder
class CH264Decoder
{
public:
	CH264Decoder(void);
	~CH264Decoder(void);

public:
	/// ����
	BOOL Create(void);
	/// ����
	void Destroy(void);

	/// ������Ƶ֡��Ϣ
	BOOL SetFrameInfo(uint16_t nVideoWidth, uint16_t nVideoHeight);
	/// �����Ƶ֡��Ϣ
	BOOL GetFrameInfo(uint16_t& nVideoWidth, uint16_t& nVideoHeight) const;

	/// ����
	int32_t Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize);

private:
	AVCodec* m_hHandleDecoder;;	
	AVCodecContext* m_pH264Context;
	AVFrame* m_pVideoFrame;

	uint16_t m_nVideoWidth;					///< ��Ƶ���
	uint16_t m_nVideoHeight;				///< ��Ƶ�߶�
};

#endif //__H264_CODEC_H__
