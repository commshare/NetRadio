// VideoCapture.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "VideoCapture.h"
#include "DShowAPI.h"

#ifdef NDEBUG
	#ifndef _UNICODE
		#pragma comment(lib,"strmbase.lib")
		#pragma message("LINK strmbase.lib")
	#else
		#pragma comment(lib,"strmbaseu.lib")
		#pragma message("LINK strmbaseu.lib")
	#endif
#else
	#ifndef _UNICODE
		#pragma comment(lib,"strmbasd.lib")
		#pragma message("LINK strmbasd.lib")
	#else
		#pragma comment(lib,"strmbasdu.lib")
		#pragma message("LINK strmbasdu.lib")
	#endif
#endif


//#pragma comment(lib,"Version.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"strmiids.lib")

//=============================================================================
// ��ƵĬ�Ͽ��
#define DEFAULT_VIDEO_WIDTH				320
// ��ƵĬ�ϸ߶�
#define DEFAULT_VIDEO_HEIGHT			240
// ��ƵĬ��ɫ��
#define DEFAULT_COLOR_BIT				12
// ��ƵĬ��֡��
#define DEFAULT_FRAME_RATE				15

//=============================================================================
CVideoCapture::CVideoCapture(void)
	: m_pCGBuilder(NULL)
	, m_pCaptureFilter(NULL)
	, m_nVideoWidth(DEFAULT_VIDEO_WIDTH)
	, m_nVideoHeight(DEFAULT_VIDEO_HEIGHT)
	, m_nColorBit(DEFAULT_COLOR_BIT)
	, m_nFrameRate(DEFAULT_FRAME_RATE)
	, m_pVideoReander(NULL)
{
}


CVideoCapture::~CVideoCapture(void)
{
	Close();
}

/// ����Ƶ�ɼ��豸
BOOL CVideoCapture::Open(ICaptureEvent* pCaptureEvent, 
	const TCHAR* szDeviceName)
{
	ASSERT(pCaptureEvent);

	// �������
	if(NULL == pCaptureEvent)
		return FALSE;

	BOOL bResult = FALSE;
    do
    {
		// ����GraphBuilder
        if(!CreateGraphBuilder())
            break;

		// ����CaptureGraphBuilder
        HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, 
            CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&m_pCGBuilder);
        if(FAILED(hr))
		{
			break;
		}

        hr = m_pCGBuilder->SetFiltergraph(m_pGraphBulider);
        if(FAILED(hr))
		{
			break;
		}

		// ������Ƶ�ɼ��豸
		m_pCaptureFilter = CreateCaptureFiler(szDeviceName);
		if(NULL == m_pCaptureFilter)
			break;

		if (FAILED(m_pGraphBulider->AddFilter(m_pCaptureFilter, L"Video Capture")))
			break;

		// ������Ƶ��Ϣ
        if(!SetVideoFormat(DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT, DEFAULT_COLOR_BIT))
            break;

		// ����֡��
		if(!SetFrameRate(DEFAULT_FRAME_RATE))
			break;

		// ����Read Filter
		m_pVideoReander = new CVideoRenderer(NULL, &hr);
		if(NULL == m_pVideoReander)
			break;
		m_pVideoReander->AddRef();
		m_pVideoReander->SetCaptureEvent(pCaptureEvent);

		if (FAILED(m_pGraphBulider->AddFilter(m_pVideoReander, L"Video Render")))
			break;

		// ����Filter
		IPin* pOutPin = GetOutputPin(m_pCaptureFilter, (uint16_t)0);
		IPin* pInPin  = GetInputPin(m_pVideoReander, (uint16_t)0);

		hr = m_pGraphBulider->Connect(pOutPin, pInPin);
		SAFE_RELEASE(pOutPin);
		SAFE_RELEASE(pInPin);

		if(FAILED(hr))
			break;

        bResult = TRUE;
    }while(FALSE);

    if(!bResult)
        Close();

    return bResult;
}

/// �ر���Ƶ�ɼ��豸
void CVideoCapture::Close(void)
{
    if(!IsStopped())
        Stop();

    SAFE_RELEASE(m_pCaptureFilter);
    SAFE_RELEASE(m_pVideoReander);
    SAFE_RELEASE(m_pCGBuilder);

    DestroyGraphBuilder();
}

/// �豸�ɼ��豸�Ƿ��
BOOL CVideoCapture::IsOpened(void) const
{
	return (NULL != m_pGraphBulider);
}

/// ��ʼ��Ƶ�ɼ�
BOOL CVideoCapture::StartCapture(void)
{
	return Run();
}

/// ��ͣ��Ƶ�ɼ�
BOOL CVideoCapture::PauseCapture(void)
{
	return Pause();
}

/// ֹͣ��Ƶ�ɼ�
BOOL CVideoCapture::StopCapture(void)
{
	return Stop();
}

/// ������Ƶ��Ϣ
BOOL CVideoCapture::SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
		uint16_t nColorBit)
{
    if(NULL == m_pCGBuilder)
        return FALSE;

    ASSERT(IsStopped());
    if(!IsStopped())
        return FALSE;

    IAMStreamConfig* iconfig = NULL; 
    BOOL bReturn = FALSE;

    do
    {
        HRESULT hr = NOERROR;   
        hr = m_pCGBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, 
            m_pCaptureFilter, IID_IAMStreamConfig, (void**)&iconfig); 
        if(hr != NOERROR)   
        { 
            hr = m_pCGBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                m_pCaptureFilter, IID_IAMStreamConfig, (void**)&iconfig); 
        } 

        if(FAILED(hr))
            break;

        AM_MEDIA_TYPE* pmt; 
        if(iconfig->GetFormat(&pmt) != S_OK) 
            break;

        if(pmt->formattype == FORMAT_VideoInfo) 
        { 
            VIDEOINFOHEADER* phead =(VIDEOINFOHEADER*)pmt->pbFormat; 
            phead->bmiHeader.biWidth = nVideoWidth; 
            phead->bmiHeader.biHeight = nVideoHeight;
            phead->bmiHeader.biBitCount = nColorBit; 
            phead->bmiHeader.biSizeImage = nVideoWidth * nVideoHeight * nColorBit / 8; 

            if(SUCCEEDED(iconfig->SetFormat(pmt)))
			{
				m_nVideoWidth = nVideoWidth;
				m_nVideoHeight = nVideoHeight;
				m_nColorBit = nColorBit;
                bReturn = TRUE;
			}
        } 
        DeleteMediaType(pmt); 
    }while(FALSE);

    SAFE_RELEASE(iconfig);

    return bReturn; 
}

/// �����Ƶ��Ϣ
BOOL CVideoCapture::GetVideoFormat(uint16_t* pVideoWidth, uint16_t* pVideoHeight, 
		uint16_t* pColorBit) const
{
	if(NULL != pVideoWidth)
		*pVideoWidth = m_nVideoWidth;

	if(NULL != pVideoHeight)
		*pVideoHeight = m_nVideoHeight;

	if(NULL != pColorBit)
		*pColorBit = m_nColorBit;

	return TRUE;
}

/// ����֡��
BOOL CVideoCapture::SetFrameRate(uint16_t nFrameRate)
{
    if(NULL == m_pCGBuilder)
        return FALSE;

    ASSERT(IsStopped());
    if(!IsStopped())
        return FALSE;

    IAMStreamConfig* iconfig = NULL; 
    BOOL bReturn = FALSE;

    do
    {
        HRESULT hr = NOERROR;   
        hr = m_pCGBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, 
            m_pCaptureFilter, IID_IAMStreamConfig, (void**)&iconfig); 
        if(hr != NOERROR)   
        { 
            hr = m_pCGBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                m_pCaptureFilter, IID_IAMStreamConfig, (void**)&iconfig); 
        } 

        if(FAILED(hr))
            break;

        AM_MEDIA_TYPE* pmt; 
        if(iconfig->GetFormat(&pmt) != S_OK) 
            break;

        if(pmt->formattype == FORMAT_VideoInfo) 
        { 
            VIDEOINFOHEADER* phead =(VIDEOINFOHEADER*)pmt->pbFormat; 
            phead->AvgTimePerFrame =  UNITS / nFrameRate; 

            if(SUCCEEDED(iconfig->SetFormat(pmt)))
			{
				m_nFrameRate = nFrameRate;
                bReturn = TRUE;
			}
        } 
        DeleteMediaType(pmt); 
    }while(FALSE);

    SAFE_RELEASE(iconfig);

    return bReturn; 
}

/// ���ָ��
uint16_t CVideoCapture::GetFrameRate(void) const
{
	return m_nFrameRate;
}

/// ����豸�б� 
uint16_t CVideoCapture::GetVideoDeviceInfo(
		device_info_t* pArrDeviceInfo, uint16_t nArrCount)
{
	return QueryVideoCaptureDevices(pArrDeviceInfo, nArrCount);
}

/// �ײ��¼�֪ͨ
void CVideoCapture::OnNotify(int nNotifyCode)
{

}

//=============================================================================
IBaseFilter* CVideoCapture::CreateCaptureFiler(const TCHAR* szDeviceName)
{
	IBaseFilter* pCaptureFilter = NULL;
	// ������Ƶ�ɼ��豸
	if(NULL != szDeviceName)
	{
		pCaptureFilter = CreateVideoDeviceFilter(szDeviceName);
	}
	else
	{
		device_info_t arrDeviceInfo[16];
		uint16_t nDeviceCount = QueryVideoCaptureDevices(arrDeviceInfo, 16);
		for(uint16_t nIndex=0; nIndex<nDeviceCount; ++nIndex)
		{
			pCaptureFilter = CreateVideoDeviceFilter(
				arrDeviceInfo[nIndex].m_szDeviceName);
			if(NULL != pCaptureFilter)
				break;
		}
	}
	return pCaptureFilter;
}
