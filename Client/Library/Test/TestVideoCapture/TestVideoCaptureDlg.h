
// TestVideoCaptureDlg.h : ͷ�ļ�
//

#pragma once
#include "ICaptureEvent.h"
#include "IVideoCapture.h"

// CTestVideoCaptureDlg �Ի���
class CTestVideoCaptureDlg : public CDialogEx, ICaptureEvent
{
// ����
public:
	CTestVideoCaptureDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTVIDEOCAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

protected:
	virtual void OnCaptureEvent(ENUM_EVENT_TYPE enType, 
		const char* szEventData, uint16_t nDataSize, uint64_t nTimeStamp);

// ʵ��
protected:
	HICON m_hIcon;
	HANDLE m_hHandle;
	IVideoCapture* m_pVideoCapture;


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
};
