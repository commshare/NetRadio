
// TestAudioCaptureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestAudioCapture.h"
#include "TestAudioCaptureDlg.h"
#include "afxdialogex.h"

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


// CTestAudioCaptureDlg �Ի���
CTestAudioCaptureDlg::CTestAudioCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestAudioCaptureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hHandleCapture = NULL;
	m_hHandlePlayer = NULL;
	m_hHandleCodec = NULL;

	m_pAudioCapture = NULL;
	m_pAudioPlayer = NULL;

	m_pAudioEncoder = NULL;
	m_pAudioDecoder = NULL;
}

void CTestAudioCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestAudioCaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestAudioCaptureDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestAudioCaptureDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestAudioCaptureDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTestAudioCaptureDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CTestAudioCaptureDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CTestAudioCaptureDlg::OnBnClickedButton6)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTestAudioCaptureDlg ��Ϣ�������

BOOL CTestAudioCaptureDlg::OnInitDialog()
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

	m_hHandleCapture = LoadLibrary(L"AudioCapture.dll");
	m_hHandlePlayer = LoadLibrary(L"AudioPlayer.dll");
	m_hHandleCodec = LoadLibrary(L"AudioCodec.dll");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


void CTestAudioCaptureDlg::OnDestroy()
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
}

void CTestAudioCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestAudioCaptureDlg::OnPaint()
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
HCURSOR CTestAudioCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestAudioCaptureDlg::OnBnClickedButton1()
{
	if(NULL == m_pAudioCapture)
	{
		m_pAudioCapture = CreateAudioCapture();
		if(NULL == m_pAudioCapture)
		{
			AfxMessageBox(L"CreateAudioCapture ʧ��!");
			return;
		}
	}

	if(NULL == m_pAudioEncoder)
	{
		m_pAudioEncoder = CreateAudioEncoder();
		if(NULL == m_pAudioEncoder)
		{
			AfxMessageBox(L"CreateAudioEncoder ʧ��!");
			return;
		}

		m_pAudioEncoder->Create(ENUM_AUDIO_CODEC_AAC);
	}

	if(NULL != m_pAudioCapture)
	{
		m_pAudioCapture->SetAudioFormat(ENUM_FREQUENCY_44KHZ, ENUM_CHANNEL_STEREO, 
			ENUM_SAMPLE_16BIT);
		m_pAudioCapture->Open(this);
		m_pAudioCapture->StartCapture();
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton2()
{
	if(NULL != m_pAudioCapture)
	{
		m_pAudioCapture->StopCapture();
		m_pAudioCapture->Close();

		DestroyAudioCapture(m_pAudioCapture);
		m_pAudioCapture = NULL;
	}

	if(NULL != m_pAudioEncoder)
	{
		DestroyAudioEncoder(m_pAudioEncoder);
		m_pAudioEncoder = NULL;
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton3()
{
}

void CTestAudioCaptureDlg::OnCaptureEvent(ENUM_EVENT_TYPE enType, 
	const char* szEventData, uint32_t nDataSize, uint64_t nTimeStamp)
{
	if(enType == ENUM_EVENT_AUDIO)
	{
		if(NULL != m_pAudioPlayer)
		{
			if(NULL != m_pAudioEncoder && NULL != m_pAudioDecoder)
			{
				char* pBuffer = new char[nDataSize];
				memcpy(pBuffer, szEventData, nDataSize);

				char szEncodeBuffer[102400] = {0};
				char szDecodeBuffer[102400] = {0};
				int32_t nEncodeSize = 0;
				int32_t nDecodeSize = 0;

				nEncodeSize = m_pAudioEncoder->Encodec(pBuffer, nDataSize, 
					szEncodeBuffer, 102400);
				if(nEncodeSize > 0)
				{
					memset(szDecodeBuffer, 0, 102400);
					nDecodeSize = m_pAudioDecoder->Decodec(szEncodeBuffer, nEncodeSize, 
						szDecodeBuffer, 102400);

					m_pAudioPlayer->OnAudioData(szDecodeBuffer, nDecodeSize, nTimeStamp);
				}

				WCHAR szDebugInfo[1024] = {0};
				swprintf(szDebugInfo, L"OnCaptureEvent AUDIO Size=%d Time=%I64d EncodeSize=%d DecodeSize=%d\n", 
					nDataSize, nTimeStamp, nEncodeSize, nDecodeSize);
				OutputDebugString(szDebugInfo);

				if(NULL != pBuffer)
				{
					delete[] pBuffer;
					pBuffer = NULL;
				}

			}
			else
			{
				m_pAudioPlayer->OnAudioData(szEventData, nDataSize, nTimeStamp);
			}

		}
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton4()
{
	if(NULL == m_pAudioPlayer)
	{
		m_pAudioPlayer = CreateAudioPlayer();
		if(NULL == m_pAudioPlayer)
		{
			AfxMessageBox(L"CreateAudioPlayer ʧ��!");
			return;
		}
	}

	if(NULL == m_pAudioDecoder)
	{
		m_pAudioDecoder = CreateAudioDecoder();
		if(NULL == m_pAudioDecoder)
		{
			AfxMessageBox(L"CreateAudioEncoder ʧ��!");
			return;
		}

		m_pAudioDecoder->Create(ENUM_AUDIO_CODEC_AAC);
	}

	if(NULL != m_pAudioPlayer)
	{
		m_pAudioPlayer->SetAudioFormat(ENUM_FREQUENCY_44KHZ, ENUM_CHANNEL_STEREO, 
			ENUM_SAMPLE_16BIT);

		m_pAudioPlayer->Open();
		m_pAudioPlayer->StartPlay();
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton5()
{
	if(NULL != m_pAudioPlayer)
	{
		m_pAudioPlayer->StopPlay();
		m_pAudioPlayer->Close();

		DestroyAudioPlayer(m_pAudioPlayer);
		m_pAudioPlayer = NULL;
	}

	if(NULL != m_pAudioDecoder)
	{
		DestroyAudioDecoder(m_pAudioDecoder);
		m_pAudioDecoder = NULL;
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton6()
{
}


//=============================================================================
IAudioCapture* CTestAudioCaptureDlg::CreateAudioCapture(void)
{
	IAudioCapture* pInterface = NULL;
	if(NULL != m_hHandleCapture)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandleCapture, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IAudioCaputre, (void**)&pInterface);
		}
	}
	return pInterface;
}

void CTestAudioCaptureDlg::DestroyAudioCapture(IAudioCapture* pAudioCapture)
{
	if(NULL != m_hHandleCapture && NULL != pAudioCapture)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			m_hHandleCapture, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			DestroyInterface(CLSID_IAudioCaputre, (void*)pAudioCapture);
			pAudioCapture = NULL;
		}
	}
}

IAudioPlayer* CTestAudioCaptureDlg::CreateAudioPlayer(void)
{
	IAudioPlayer* pInterface = NULL;
	if(NULL != m_hHandlePlayer)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandlePlayer, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IAudioPlayer, (void**)&pInterface);
		}
	}
	return pInterface;

}

void CTestAudioCaptureDlg::DestroyAudioPlayer(IAudioPlayer* pAudioPlayer)
{
	if(NULL != m_hHandlePlayer && NULL != pAudioPlayer)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			m_hHandlePlayer, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			DestroyInterface(CLSID_IAudioPlayer, (void*)pAudioPlayer);
			pAudioPlayer = NULL;
		}
	}
}

IAudioEncoder* CTestAudioCaptureDlg::CreateAudioEncoder(void)
{
	IAudioEncoder* pInterface = NULL;
	if(NULL != m_hHandleCodec)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandleCodec, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IAudioEncoder, (void**)&pInterface);
		}
	}
	return pInterface;
}

void CTestAudioCaptureDlg::DestroyAudioEncoder(IAudioEncoder* pAudioEncoder)
{
	if(NULL != m_hHandleCodec && NULL != pAudioEncoder)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			m_hHandleCodec, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			DestroyInterface(CLSID_IAudioEncoder, (void*)pAudioEncoder);
			pAudioEncoder = NULL;
		}
	}
}

IAudioDecoder* CTestAudioCaptureDlg::CreateAudioDecoder(void)
{
	IAudioDecoder* pInterface = NULL;
	if(NULL != m_hHandleCodec)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandleCodec, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IAudioDecoder, (void**)&pInterface);
		}
	}
	return pInterface;
}

void CTestAudioCaptureDlg::DestroyAudioDecoder(IAudioDecoder* pAudioDecoder)
{
	if(NULL != m_hHandleCodec && NULL != pAudioDecoder)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			m_hHandleCodec, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			DestroyInterface(CLSID_IAudioDecoder, (void*)pAudioDecoder);
			pAudioDecoder = NULL;
		}
	}
}
