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
/// \file    : CUdpPackBuffer.h
/// \brief   : UDP���ݰ������
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-23
///============================================================================
#ifndef __UDP_PACK_BUFFER_h__
#define __UDP_PACK_BUFFER_h__

#include "TypeDefine.h"
#include "SafeQueue.h"
#include "IEncrypt.h"
#include "NetSerialize.h"

//=============================================================================
// struct udp_pack_header
typedef struct _udp_pack_header
{
	uint16_t	m_nPackHeadTag;					///< ��ͷ��ʾ
	uint8_t		m_nPackVersion;					///< �汾��
	uint16_t	m_nPackTimeStamp;				///< ʱ���

	uint8_t		m_nEncryptType;					///< ��������
	uint16_t	m_nEncryptSize;					///< ���ܺ����ݳ���
	uint16_t	m_nExtFillSize;					///< ������䳤��

public:
	/// ���л�
	BOOL Serialize(CNetSerialize & aoNetSerialize);
	/// �ж��Ƿ���Ч
	BOOL IsValid(void) const;
}udp_pack_header;

//=============================================================================
// class CUdpPackBuffer
class CUdpPackBuffer
{
public:
	CUdpPackBuffer(void);
	~CUdpPackBuffer(void);

public:
	/// ����
	BOOL Create(const char* szEncryKey, uint16_t nKeySize);
	/// �ͷ�
	void Destroy(void);
	/// �ͷŴ���
	BOOL IsCreated(void);
	/// ������Կ
	BOOL SetEncryptKey(const char* szEncryKey, uint16_t nKeySize);

public:
	/// ���
	uint32_t Pack(const char* szInBuffer, uint16_t nInBufferSize, 
		char* szOutBuffer, uint16_t nOutBufferSize, 
		ENUM_ENCRYPT_TYPE enEncryptType = ENUM_ENCRYPT_AES);

	/// ���
	uint32_t UnPack(const char* szInBuffer, uint16_t nInBufferSize, 
		char* szOutBuffer, uint16_t& nOutBufferSize, uint16_t& nTimeStamp);

private:
	uint16_t m_nTimeStamp;			///< ʱ���
	IEncrypt* m_pEncrypt;			///< �ӽ��ܽӿ�
};

#endif //__UDP_PACK_BUFFER_h__
