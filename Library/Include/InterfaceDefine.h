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
/// \file    : InterfaceDefine.h
/// \brief   : �ӿ�ͨ�ö����ļ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-17
///============================================================================

#ifndef __I_INTERFACE_DEFINES_H__
#define __I_INTERFACE_DEFINES_H__

//=============================================================================
#include <string.h>			// for memcmp

#ifdef _WIN32
#include <InitGuid.h>
#include <Guiddef.h>
#else

/// GUID����
typedef struct _GUID {
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t  Data4[ 8 ];
}GUID, CLSID;

/// GUID��ʼ����
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	extern "C" const GUID name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

/// �ж�GUID�Ƿ���ͬ
int IsEqualCLSID(GUID rguid1, GUID rguid2)
{
	return !memcmp(&rguid1, &rguid2, sizeof(GUID));
}

#endif	//WIN32

//=============================================================================
// �ӿڷ���ֵ���Ͷ���
typedef long			IRESULT;

// �ӿڵ��óɹ����Ժ�
#define ISUCCEEDED(Status)	((IRESULT)(Status) >= 0)

// �ӿڵ���ʧ�ܲ��Ժ�
#define IFAILED(Status)		((IRESULT)(Status)<0)

// �ӿڷ���ֵ����
#define I_OK			IRESULT(0x00000000L)
#define I_SUCCEED		IRESULT(0x00000001L)

// �ӿڷ���ֵ������
#define I_NOERROR       I_OK

// �����ʧ��
#define I_UNEXPECTED	IRESULT(0x8000FFFFL)

// δʵ��
#define I_NOTIMPL       IRESULT(0x80004001L)

// δ�ܷ���������ڴ�
#define I_OUTOFMEMORY   IRESULT(0x8007000EL)

// һ������������Ч
#define I_INVALIDARG    IRESULT(0x80070057L)

// ��֧�ִ˽ӿ�
#define I_NOINTERFACE   IRESULT(0x80004002L)

// ��Чָ��
#define I_POINTER       IRESULT(0x80004003L)

// ��Ч���
#define I_HANDLE        IRESULT(0x80070006L)

// ��������ֹ
#define I_ABORT         IRESULT(0x80004004L)

// δָ����ʧ��
#define I_FAIL          IRESULT(0x80004005L)

// һ��ķ��ʱ��ܾ�����
#define I_ACCESSDENIED  IRESULT(0x80070005L)

#endif //__I_INTERFACE_DEFINES_H__
