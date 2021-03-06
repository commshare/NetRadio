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
/// \brief   : 数据包队列类头文件
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
	char		m_szPackHeadTag[4];				///< 包头标示
	uint8_t		m_nPackVersion;					///< 版本号
	uint16_t	m_nPackTimeStamp;				///< 时间戳

	uint8_t		m_nEncryptType;					///< 加密类型
	uint16_t	m_nEncryptSize;					///< 加密后数据长度
	uint16_t	m_nExtFillSize;					///< 数据填充长度

public:
	/// 序列化
	BOOL Serialize(CNetSerialize & aoNetSerialize);
	/// 判断是否有效
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
	/// 创建
	BOOL Create(ENUM_ENCRYPT_TYPE enEncryptType = ENUM_ENCRYPT_NONE,
			const char* szEncryKey = NULL, uint16_t nKeySize = 0);
	/// 释放
	void Destroy(void);
	/// 释放创建
	BOOL IsCreated(void);

	/// 设置加密类型
	BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enEncryptType);
	/// 设置密钥
	BOOL SetEncryptKey(const char* szEncryKey, uint16_t nKeySize);

public:
	/// 打包
	uint32_t Pack(const char* szInBuffer, uint16_t nInBufferSize,
		char* szOutBuffer, uint16_t nOutBufferSize);

	/// 解包
	uint32_t UnPack(const char* szInBuffer, uint16_t nInBufferSize,
		char* szOutBuffer, uint16_t& nOutBufferSize, uint16_t& nTimeStamp);

private:
	uint16_t m_nTimeStamp;						///< 时间戳
	IEncrypt* m_pEncrypt;						///< 加解密接口
	ENUM_ENCRYPT_TYPE m_enEncryptType;			///< 加密类型

	char m_szBuffer[MAX_PACK_BUFFER_SIZE*2];	///< 缓存区
	uint32_t m_nDataSize;						///< 缓冲区中的数据长度
};

#endif //__TCP_PACK_BUFFER_h__
