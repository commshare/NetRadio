
// TestVideoCaptureDlg.h : ͷ�ļ�
//

#pragma once
#include "ICaptureEvent.h"
#include "IVideoCapture.h"
#include "IVideoPlayer.h"
#include "IVideoCodec.h"

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

private:
	IVideoCapture* CreateVideoCapture(void);
	void DestroyVideoCapture(IVideoCapture* pVideoCapture);

	IVideoPlayer* CreateVideoPlayer(void);
	void DestroyVideoPlayer(IVideoPlayer* pVideoPlayer);

	IVideoEncoder* CreateVideoEncoder(void);
	void DestroyVideoEncoder(IVideoEncoder* pVideoEncoder);

	IVideoDecoder* CreateVideoDecoder(void);
	void DestroyVideoDecoder(IVideoDecoder* pVideoDecoder);

protected:
	virtual void OnCaptureEvent(ENUM_EVENT_TYPE enType, 
		const char* szEventData, uint32_t nDataSize, uint64_t nTimeStamp);

// ʵ��
protected:
	HICON m_hIcon;

	HMODULE m_hHandleCapture;
	HMODULE m_hHandlePlayer;
	HMODULE m_hHandleCodec;

	IVideoCapture* m_pVideoCapture;
	IVideoPlayer* m_pVideoPlayer;

	IVideoEncoder* m_pVideoEncoder;
	IVideoDecoder* m_pVideoDecoder;

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
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnDestroy();
};
