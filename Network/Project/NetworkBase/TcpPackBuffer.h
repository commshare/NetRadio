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
/// \file    : PacketQueue.h
/// \brief   : ���ݰ�������ͷ�ļ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-18
///============================================================================
#ifndef __TCP_PACK_BUFFER_h__
#define __TCP_PACK_BUFFER_h__

#include "TypeDefine.h"
#include "IEncrypt.h"
#include "NetSerialize.h"

//=============================================================================
// struct tcp_pack_header
typedef struct _tcp_pack_header
{
	char		m_szPackHeadTag[4];				///< ��ͷ��ʾ
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
}tcp_pack_header;

//=============================================================================
// class CTcpPackBuffer
class CTcpPackBuffer
{
public:
	CTcpPackBuffer(void);
	~CTcpPackBuffer(void);

public:
	/// ����
	BOOL Create(ENUM_ENCRYPT_TYPE enEncryptType = ENUM_ENCRYPT_NONE,
			const char* szEncryKey = NULL, uint16_t nKeySize = 0);
	/// �ͷ�
	void Destroy(void);
	/// �ͷŴ���
	BOOL IsCreated(void);

	/// ���ü�������
	BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enEncryptType);
	/// ������Կ
	BOOL SetEncryptKey(const char* szEncryKey, uint16_t nKeySize);

public:
	/// ���
	uint32_t Pack(const char* szInBuffer, uint16_t nInBufferSize,
		char* szOutBuffer, uint16_t nOutBufferSize);

	/// ���
	uint32_t UnPack(const char* szInBuffer, uint16_t nInBufferSize,
		char* szOutBuffer, uint16_t& nOutBufferSize, uint16_t& nTimeStamp);

private:
	uint16_t m_nTimeStamp;						///< ʱ���
	IEncrypt* m_pEncrypt;						///< �ӽ��ܽӿ�
	ENUM_ENCRYPT_TYPE m_enEncryptType;			///< ��������

	char m_szBuffer[MAX_PACK_BUFFER_SIZE*2];	///< ������
	uint32_t m_nDataSize;						///< �������е����ݳ���
};

#endif //__TCP_PACK_BUFFER_h__
