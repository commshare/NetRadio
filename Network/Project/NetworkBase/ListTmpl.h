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
/// \file    : ListTmpl.h
/// \brief   : Listģ����
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-20
///============================================================================
#ifndef __LIST_TMPL_H__
#define __LIST_TMPL_H__

#include "TypeDefine.h"

//=============================================================================
// abstract iteration position
struct __POSITION {};
typedef __POSITION* POSITION;

// �ڴ��ڵ�
typedef struct _block_node_t
{
	struct _block_node_t* m_pNext;
	char* m_pBuffer;
}block_node_t;

//=============================================================================
template<typename TYPE, typename ARG_TYPE = const TYPE&>
class CListTmpl
{
protected:
	/// ����ڵ�
	typedef struct _list_node_t
	{
		_list_node_t* m_pNext;
		_list_node_t* m_pPrev;
		TYPE m_Data;
	}list_node_t;

public:
	CListTmpl(uint32_t nBlockSize = 16);
	~CListTmpl(void);

public:
	// �����е�Ԫ������
	uint32_t GetCount(void) const;
	uint32_t GetSize(void) const;
	BOOL IsEmpty(void) const;

	// ���Ԫ�ص�ͷ�ڵ��β�ڵ�
	TYPE& GetHead(void);
	const TYPE& GetHead(void) const;
	TYPE& GetTail(void);
	const TYPE& GetTail(void) const;

	// ��ò�ɾ��ͷ�ڵ��β�ڵ㣨��Ҫ������β�յ�ʱ����ã�
	TYPE RemoveHead(void);
	TYPE RemoveTail(void);

	// ���ͷ�ڵ��β�ڵ�
	POSITION AddHead(ARG_TYPE newNode);
	POSITION AddTail(ARG_TYPE newNode);

	// �����������ͷ�ڵ����β�ڵ�
	void AddHead(CListTmpl* pNewList);
	void AddTail(CListTmpl* pNewList);

	// ɾ�����нڵ�
	void RemoveAll(void);

	// λ�ò�������
	POSITION GetHeadPosition(void) const;
	POSITION GetTailPosition(void) const;
	TYPE& GetNext(POSITION& rPosition);				// return *Position++
	const TYPE& GetNext(POSITION& rPosition) const; // return *Position++
	TYPE& GetPrev(POSITION& rPosition);				// return *Position--
	const TYPE& GetPrev(POSITION& rPosition) const; // return *Position--

	// ͨ��λ����Ϣ��ú��޸�Ԫ��
	TYPE& GetAt(POSITION position);
	const TYPE& GetAt(POSITION position) const;
	void SetAt(POSITION pos, ARG_TYPE newNode);
	void RemoveAt(POSITION position);

	// �����½ڵ�
	POSITION InsertBefore(POSITION position, ARG_TYPE newNode);
	POSITION InsertAfter(POSITION position, ARG_TYPE newNode);

	// ���ҽڵ�(note: O(n) speed) δ�ҵ�����NULL
	POSITION Find(ARG_TYPE searchValue, POSITION startAfter = NULL) const;
	POSITION FindIndex(uint32_t nIndex) const;

protected:
	/// �����ڵ�
	list_node_t* NewNode(list_node_t* pPrevNode, list_node_t* pNextNode);
	/// �ͷŽڵ�
	void FreeNode(list_node_t*);

protected:
	list_node_t* m_pNodeHead;		///< ͷ�ڵ�
	list_node_t* m_pNodeTail;		///< β�ڵ�
	uint32_t m_nNodeCount;			///< �ڵ�����

	list_node_t* m_pNodeFree;		///< ���ɽڵ�����

	block_node_t* m_pBlockCache;	///< �黺��
	uint32_t m_nBlockSize;			///< �黺��ߴ�
};

#include "ListTmpl.inl"

#endif //__LIST_TMPL_H__
