
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

	m_pAudioCapture = NULL;
	m_pAudioPlayer = NULL;
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

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
	m_hHandleCapture = LoadLibrary(L"AudioCapture.dll");
	if(NULL != m_hHandleCapture)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandleCapture, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IAudioCaputre, (void**)&m_pAudioCapture);
		}
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton2()
{
	if(NULL != m_hHandleCapture)
	{
		if(NULL != m_pAudioCapture)
		{
			m_pAudioCapture->StopCapture();
			m_pAudioCapture->Close();

			typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
			DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
				m_hHandleCapture, "DestroyInterface");
			if(NULL != DestroyInterface)
			{
				DestroyInterface(CLSID_IAudioCaputre, (void*)m_pAudioCapture);
				m_pAudioCapture = NULL;
			}
		}

		FreeLibrary(m_hHandleCapture);
		m_hHandleCapture = NULL;
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton3()
{
	if(NULL != m_pAudioCapture)
	{
		if(m_pAudioCapture->Open((ICaptureEvent*)this))
		{
			m_pAudioCapture->StartCapture();
		}
	}
}

void CTestAudioCaptureDlg::OnCaptureEvent(ENUM_EVENT_TYPE enType, 
	const char* szEventData, uint32_t nDataSize, uint64_t nTimeStamp)
{
	if(enType == ENUM_EVENT_AUDIO)
	{
		if(NULL != m_pAudioPlayer)
		{
			m_pAudioPlayer->OnAudioData(szEventData, nDataSize, nTimeStamp);
		}
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton4()
{
	m_hHandlePlayer = LoadLibrary(L"AudioPlayer.dll");
	if(NULL != m_hHandlePlayer)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandlePlayer, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IAudioPlayer, (void**)&m_pAudioPlayer);
		}
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton5()
{
	if(NULL != m_hHandlePlayer)
	{
		if(NULL != m_pAudioPlayer)
		{
			m_pAudioPlayer->StopPlay();
			m_pAudioPlayer->Close();

			typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
			DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
				m_hHandlePlayer, "DestroyInterface");
			if(NULL != DestroyInterface)
			{
				DestroyInterface(CLSID_IAudioPlayer, (void*)m_pAudioPlayer);
				m_pAudioPlayer = NULL;
			}
		}

		FreeLibrary(m_hHandlePlayer);
		m_hHandlePlayer = NULL;
	}
}


void CTestAudioCaptureDlg::OnBnClickedButton6()
{
	if(NULL != m_pAudioPlayer)
	{
		if(m_pAudioPlayer->Open())
		{
			m_pAudioPlayer->StartPlay();
		}
	}
}
