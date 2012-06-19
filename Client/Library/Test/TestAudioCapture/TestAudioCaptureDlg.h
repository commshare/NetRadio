
// TestAudioCaptureDlg.h : ͷ�ļ�
//

#pragma once

#include "ICaptureEvent.h"
#include "IAudioCapture.h"
#include "IAudioPlayer.h"


// CTestAudioCaptureDlg �Ի���
class CTestAudioCaptureDlg : public CDialogEx, ICaptureEvent
{
// ����
public:
	CTestAudioCaptureDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTAUDIOCAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

protected:
	void OnCaptureEvent(ENUM_EVENT_TYPE enType, 
		const char* szEventData, uint32_t nDataSize, uint64_t nTimeStamp);

// ʵ��
protected:
	HICON m_hIcon;

	HMODULE m_hHandleCapture;
	HMODULE m_hHandlePlayer;

	IAudioCapture* m_pAudioCapture;
	IAudioPlayer* m_pAudioPlayer;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
};
