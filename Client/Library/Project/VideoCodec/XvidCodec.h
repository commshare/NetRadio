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
/// \file    : XvidCodec.h
/// \brief   : XVID�������
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-20
///============================================================================
#ifndef __XVID_CODEC_H__
#define __XVID_CODEC_H__

#include "TypeDefine.h"
#include "IVideoCodec.h"
#include "xvid\\src\\xvid.h"

#ifdef _DEBUG
#pragma comment(lib, "libxvidcoreD.lib")
#pragma message("LINK libxvidcoreD.lib")
#else
#pragma comment(lib, "libxvidcore.lib")
#pragma message("LINK libxvidcore.lib")
#endif

//=============================================================================

//=============================================================================
// class CXvidEncoder
class CXvidEncoder
{
public:
	CXvidEncoder(void);
	~CXvidEncoder(void);

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
	void* m_HandleEncode;					///< �����������
	uint16_t m_nVideoWidth;					///< ��Ƶ���
	uint16_t m_nVideoHeight;				///< ��Ƶ�߶�
	uint16_t m_nVideoQuant;					///< ��Ƶ����
	uint16_t m_nFrameRate;					///< ��Ƶ֡��
};

//=============================================================================
// class CXvidDecoder
class CXvidDecoder
{
public:
	CXvidDecoder(void);
	~CXvidDecoder(void);

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
	int DecodeFrame(char *istream, int istream_size, char *ostream,
		  xvid_dec_stats_t *xvid_dec_stats);

private:
	void* m_HandleDecode;					///< ���������
	uint16_t m_nVideoWidth;					///< ��Ƶ���
	uint16_t m_nVideoHeight;				///< ��Ƶ�߶�
};

#endif //__XVID_CODEC_H__
