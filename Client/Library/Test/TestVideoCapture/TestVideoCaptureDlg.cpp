
// TestVideoCaptureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestVideoCapture.h"
#include "TestVideoCaptureDlg.h"
#include "afxdialogex.h"
#include "SetupDialog.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestVideoCaptureDlg �Ի���
CTestVideoCaptureDlg::CTestVideoCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestVideoCaptureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hHandleCapture = NULL;
	m_hHandlePlayer = NULL;
	m_hHandleCodec = NULL;
	m_hHandleResize = NULL;

	m_pVideoCapture = NULL;
	m_pVideoPlayer = NULL;

	m_pVideoEncoder = NULL;
	m_pVideoDecoder = NULL;

	m_pVideoResize = NULL;
}

void CTestVideoCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestVideoCaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestVideoCaptureDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestVideoCaptureDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestVideoCaptureDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON7, &CTestVideoCaptureDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON4, &CTestVideoCaptureDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CTestVideoCaptureDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CTestVideoCaptureDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON8, &CTestVideoCaptureDlg::OnBnClickedButton8)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTestVideoCaptureDlg ��Ϣ�������

BOOL CTestVideoCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_hHandleCapture = LoadLibrary(L"VideoCapture.dll");
	m_hHandlePlayer = LoadLibrary(L"VideoPlayer.dll");
	m_hHandleCodec = LoadLibrary(L"VideoCodec.dll");
	m_hHandleResize = LoadLibrary(L"VideoResize.dll");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


void CTestVideoCaptureDlg::OnDestroy()
{
	__super::OnDestroy();

	if(NULL != m_hHandleCapture)
	{
		FreeLibrary(m_hHandleCapture);
		m_hHandleCapture = NULL;
	}

	if(NULL != m_hHandlePlayer)
	{
		FreeLibrary(m_hHandlePlayer);
		m_hHandlePlayer = NULL;
	}

	if(NULL != m_hHandleCodec)
	{
		FreeLibrary(m_hHandleCodec);
		m_hHandleCodec = NULL;
	}

	if(NULL != m_hHandleResize)
	{
		FreeLibrary(m_hHandleResize);
		m_hHandleResize = NULL; 
	}
}


void CTestVideoCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestVideoCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTestVideoCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestVideoCaptureDlg::OnBnClickedButton1()
{
	if(NULL == m_pVideoCapture)
	{
		m_pVideoCapture = CreateVideoCapture();
		if(NULL == m_pVideoCapture)
		{
			AfxMessageBox(L"CreateVideoCapture ʧ��!");
			return;
		}
	}

	if(NULL == m_pVideoEncoder)
	{
		m_pVideoEncoder = CreateVideoEncoder();
		if(NULL == m_pVideoEncoder)
		{
			AfxMessageBox(L"CreateVideoEncoder ʧ��!");
			return;
		}

		m_pVideoEncoder->SetFrameInfo(DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT);
		m_pVideoEncoder->SetFrameRate(15);
		m_pVideoEncoder->SetVideoQuant(100);
		m_pVideoEncoder->Create(ENUM_VIDEO_CODEC_H264);
	}

	if(NULL == m_pVideoResize)
	{
		m_pVideoResize = CreateVideoResize();
		if(NULL == m_pVideoResize)
		{
			AfxMessageBox(L"CreateVideoEncoder ʧ��!");
			return;
		}

		m_pVideoResize->Create(ENUM_RESIZE_BILINEAER, ENUM_RESIZE_CSP_RGB24, 
			320, 240, 176, 144);
	}

	if(NULL != m_pVideoCapture)
	{
		m_pVideoCapture->Open(this);
		m_pVideoCapture->StartCapture();
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton2()
{
	if(NULL != m_pVideoCapture)
	{
		m_pVideoCapture->StopCapture();
		m_pVideoCapture->Close();

		DestroyVideoCapture(m_pVideoCapture);
		m_pVideoCapture = NULL;
	}

	if(NULL != m_pVideoResize)
	{
		m_pVideoResize->Destroy();
		DestroyVideoResize(m_pVideoResize);
		m_pVideoResize = NULL;
	}

	if(NULL != m_pVideoEncoder)
	{
		DestroyVideoEncoder(m_pVideoEncoder);
		m_pVideoEncoder = NULL;
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton8()
{
	CSetupDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(NULL != m_pVideoCapture)
		{
			m_pVideoCapture->SetVideoFormat(dlg.m_nVideoWidth, dlg.m_nVideoHeight, 
				dlg.m_nFrameRate);
		}
	}
}


void CTestVideoCaptureDlg::OnBnClickedButton3()
{

}

void CTestVideoCaptureDlg::OnCaptureEvent(ENUM_EVENT_TYPE enType, 
	const char* szEventData, uint32_t nDataSize, uint64_t nTimeStamp)
{
	if(enType == ENUM_EVENT_VIDEO)
	{
		if(NULL != m_pVideoPlayer)
		{
			if(NULL != m_pVideoEncoder && NULL != m_pVideoDecoder)
			{
				char* pBuffer = new char[nDataSize];
				char* pEncode = new char[nDataSize];
				int32_t nEncodeSize = 0;
				int32_t nDecodeSize = 0;

				memcpy(pBuffer, szEventData, nDataSize);
				//memset(pBuffer, 255, nDataSize);

				nEncodeSize = m_pVideoEncoder->Encodec(pBuffer, nDataSize, 
					pEncode, nDataSize);
				nDecodeSize = m_pVideoDecoder->Decodec(pEncode, nEncodeSize, pBuffer, nDataSize);

				if(nDecodeSize > 0)
				{
					// ��Ƶ�ߴ�ת��
					if(NULL != m_pVideoResize)
					{
						uint32_t nResizeSize = 176*144*3;
						char szResizeBuffer[176*144*3] = {0};

						nResizeSize = m_pVideoResize->Resize(pBuffer, nDecodeSize, szResizeBuffer, nResizeSize);
						if(nResizeSize > 0)
						{
							m_pVideoPlayer->OnVideoData(szResizeBuffer, nResizeSize, nTimeStamp);
						}

						//m_pVideoPlayer->OnVideoData(pBuffer, nDecodeSize, nTimeStamp);
					}
					else
					{
						m_pVideoPlayer->OnVideoData(pBuffer, nDecodeSize, nTimeStamp);
					}

				}

				if(NULL != pBuffer)
				{
					delete[] pBuffer;
					pBuffer = NULL;
				}

				if(NULL != pEncode)
				{
					delete[] pEncode;
					pEncode = NULL;
				}
			}
			else
			{
				m_pVideoPlayer->OnVideoData(szEventData, nDataSize, nTimeStamp);
			}
		}
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton4()
{
	if(NULL == m_pVideoPlayer)
	{
		m_pVideoPlayer = CreateVideoPlayer();
		if(NULL == m_pVideoPlayer)
		{
			AfxMessageBox(L"CreateVideoPlayer ʧ��!");
			return;
		}
	}

	if(NULL == m_pVideoDecoder)
	{
		m_pVideoDecoder = CreateVideoDecoder();
		if(NULL == m_pVideoDecoder)
		{
			AfxMessageBox(L"CreateVideoEncoder ʧ��!");
			return;
		}

		m_pVideoDecoder->SetFrameInfo(DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT);
		m_pVideoDecoder->Create(ENUM_VIDEO_CODEC_H264);
	}

	if(NULL != m_pVideoPlayer)
	{
		CWnd* pWnd = GetDlgItem(IDC_STATIC_VIDEO);
		if(NULL != pWnd)
		{
			m_pVideoPlayer->SetVideoFormat(176, 144, 20);
			m_pVideoPlayer->Open(pWnd->GetSafeHwnd());
			m_pVideoPlayer->StartPlay();
		}
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton5()
{
	if(NULL != m_pVideoPlayer)
	{
		m_pVideoPlayer->StopPlay();
		m_pVideoPlayer->Close();

		DestroyVideoPlayer(m_pVideoPlayer);
		m_pVideoPlayer = NULL;
	}

	if(NULL != m_pVideoDecoder)
	{
		DestroyVideoDecoder(m_pVideoDecoder);
		m_pVideoDecoder = NULL;
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton7()
{
	CSetupDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(NULL != m_pVideoPlayer)
		{
			m_pVideoPlayer->SetVideoFormat(dlg.m_nVideoWidth, dlg.m_nVideoHeight, 
				dlg.m_nFrameRate);
		}
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton6()
{
}

//=============================================================================
IVideoCapture* CTestVideoCaptureDlg::CreateVideoCapture(void)
{
	IVideoCapture* pInterface = NULL;
	if(NULL != m_hHandleCapture)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandleCapture, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IVideoCaputre, (void**)&pInterface);
		}
	}
	return pInterface;
}

void CTestVideoCaptureDlg::DestroyVideoCapture(IVideoCapture* pVideoCapture)
{
	if(NULL != m_hHandleCapture && NULL != pVideoCapture)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			m_hHandleCapture, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			DestroyInterface(CLSID_IVideoCaputre, (void*)pVideoCapture);
			pVideoCapture = NULL;
		}
	}
}

IVideoPlayer* CTestVideoCaptureDlg::CreateVideoPlayer(void)
{
	IVideoPlayer* pInterface = NULL;
	if(NULL != m_hHandlePlayer)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandlePlayer, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IVideoPlayer, (void**)&pInterface);
		}
	}
	return pInterface;

}

void CTestVideoCaptureDlg::DestroyVideoPlayer(IVideoPlayer* pVideoPlayer)
{
	if(NULL != m_hHandlePlayer && NULL != pVideoPlayer)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			m_hHandlePlayer, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			DestroyInterface(CLSID_IVideoPlayer, (void*)pVideoPlayer);
			pVideoPlayer = NULL;
		}
	}
}

IVideoEncoder* CTestVideoCaptureDlg::CreateVideoEncoder(void)
{
	IVideoEncoder* pInterface = NULL;
	if(NULL != m_hHandleCodec)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandleCodec, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IVideoEncoder, (void**)&pInterface);
		}
	}
	return pInterface;
}

void CTestVideoCaptureDlg::DestroyVideoEncoder(IVideoEncoder* pVideoEncoder)
{
	if(NULL != m_hHandleCodec && NULL != pVideoEncoder)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			m_hHandleCodec, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			DestroyInterface(CLSID_IVideoEncoder, (void*)pVideoEncoder);
			pVideoEncoder = NULL;
		}
	}
}

IVideoDecoder* CTestVideoCaptureDlg::CreateVideoDecoder(void)
{
	IVideoDecoder* pInterface = NULL;
	if(NULL != m_hHandleCodec)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandleCodec, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IVideoDecoder, (void**)&pInterface);
		}
	}
	return pInterface;
}

void CTestVideoCaptureDlg::DestroyVideoDecoder(IVideoDecoder* pVideoDecoder)
{
	if(NULL != m_hHandleCodec && NULL != pVideoDecoder)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			m_hHandleCodec, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			DestroyInterface(CLSID_IVideoDecoder, (void*)pVideoDecoder);
			pVideoDecoder = NULL;
		}
	}
}

IVideoResize* CTestVideoCaptureDlg::CreateVideoResize(void)
{
	IVideoResize* pInterface = NULL;
	if(NULL != m_hHandleResize)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandleResize, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IVideoResize, (void**)&pInterface);
		}
	}
	return pInterface;
}

void CTestVideoCaptureDlg::DestroyVideoResize(IVideoResize* pVideoResize)
{
	if(NULL != m_hHandleResize && NULL != pVideoResize)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			m_hHandleResize, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			DestroyInterface(CLSID_IVideoResize, (void*)pVideoResize);
			pVideoResize = NULL;
		}
	}
}