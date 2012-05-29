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
/// \file    : EncryptInfo.h
/// \brief   : ������Ϣ��
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-28
///============================================================================
#ifndef __ENCRYPT_INFO_H__
#define __ENCRYPT_INFO_H__

#include "ITcpNetServer.h"
#include "IEncrypt.h"

//=============================================================================
// class CEncryptInfo
class CEncryptInfo
{
public:
	CEncryptInfo(void);
	~CEncryptInfo(void);

public:
	/// ���ü�������
	void SetEncryptType(ENUM_ENCRYPT_TYPE enType);
	/// ��ȡ�������� 
	ENUM_ENCRYPT_TYPE GetEncryptType(void) const;

	/// ���ü�����Կ
	BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize);
	/// ��ȡ������Կ
	BOOL GetEncryptKey(char* szEncryptKey, uint16_t& nKeySize);
	/// ��ü�����Կ
	const char* GetEncryptKey(void) const;
	/// ��ü�����Կ����
	uint16_t GetEncryptKeySize(void) const;

private:
	ENUM_ENCRYPT_TYPE m_enEncryptType;				///< ��������
	char m_szEncryptKey[TCP_SVR_ENCRYPT_KEY_SIZE];	///< ������Կ
	uint16_t m_enEncryptKeySize;					///< ������Կ����
};

#endif //__ENCRYPT_INFO_H__
