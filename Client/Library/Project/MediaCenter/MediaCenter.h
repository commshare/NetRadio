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
/// \file    : MediaCenter.h
/// \brief   : ý������
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-15
///============================================================================
#ifndef __MEDIA_CENTER_H__
#define __MEDIA_CENTER_H__

#include "IMediaCenter.h"
#include "types\\stdint.h"
#include "CriticalSection.h"

//=============================================================================
/// ý��ģ������
#define MAX_MEDIA_MODULE_COUNT		64

/// ý��ģ��ӿ�����
#define MAX_MEDIA_INTERFACE_COUNT	64

/// ģ�����Ƴ���
#define MAX_MODULE_NAME_SIZE		256

//=============================================================================
/// ģ����Ϣ
typedef struct _module_info
{
	TCHAR m_szModuleName[MAX_MODULE_NAME_SIZE];	///< ģ������
	HANDLE m_hModuleHandle;						///< ģ����
	uint32_t nRefCount;							///< ���ü���
}module_info_t;

/// �ӿ���Ϣ
typedef struct _interface_info
{
	GUID  m_InterfaceID;						///< �ӿڱ�ʶ
	void* m_pInterface;							///< �ӿ�ָ��

	TCHAR m_szModuleName[MAX_MODULE_NAME_SIZE];	///< ģ������
}interface_info_t;

//=============================================================================
class CMediaCenter : public IMediaCenter
{
public:
	CMediaCenter(void);
	~CMediaCenter(void);

public:
	/// ��ѯ�ӿ�
	virtual IRESULT QueryInterface(const CLSID& oInterfaceID, 
		void** ppInterface);

	/// �ͷŽӿ�
	virtual IRESULT ReleaseInterface(const CLSID& oInterfaceID, 
		void* pInterface);

private:
	/// �����ӿ�
	IRESULT CreateInterface(const TCHAR* pszModuleName, 
		const CLSID& oInterfaceID, void** ppInterface);

	/// �ͷŽӿ�
	IRESULT DestroyInteface(const TCHAR* pszModuleName, 
		const CLSID& oInterfaceID, void* ppInterface);

	/// �жϽӿ��Ƿ�֧��
	BOOL IsSupportInterface(const CLSID& oInterfaceID) const;
	/// ��ýӿ�������ģ������
	TCHAR* GetModuleName(const CLSID& oInterfaceID) const;

private:
	/// ģ����Ϣ����
	static module_info_t m_arrModuleInfo[MAX_MEDIA_MODULE_COUNT];
	/// �ӿ���Ϣ����
	static interface_info_t m_arrInterfaceInfo[MAX_MEDIA_INTERFACE_COUNT];
	/// �ٽ�
	mutable CCriticalSection m_ModuleLock;
};

#endif //__MEDIA_CENTER_H__
