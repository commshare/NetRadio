
// TestVideoCapture.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestVideoCaptureApp:
// �йش����ʵ�֣������ TestVideoCapture.cpp
//

class CTestVideoCaptureApp : public CWinApp
{
public:
	CTestVideoCaptureApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestVideoCaptureApp theApp;