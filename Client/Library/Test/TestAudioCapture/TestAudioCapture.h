
// TestAudioCapture.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestAudioCaptureApp:
// �йش����ʵ�֣������ TestAudioCapture.cpp
//

class CTestAudioCaptureApp : public CWinApp
{
public:
	CTestAudioCaptureApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestAudioCaptureApp theApp;