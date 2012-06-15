#include <atlbase.h>
#include "CaptureDevice.h"


CCaptureDevice::CCaptureDevice(void)
{
}


CCaptureDevice::~CCaptureDevice(void)
{
}

//=============================================================================
/// ����豸����
uint16_t CCaptureDevice::GetDeviceCount(ENUM_DEVICE_TYPE enDeviceType) const
{
	capture_device_info_t arrDeviceInfo[256];
	uint16_t nDeviceCount = GetDeviceList(enDeviceType, arrDeviceInfo, 256);
	return nDeviceCount;
}

/// ����豸�б� 
uint16_t CCaptureDevice::GetDeviceList(ENUM_DEVICE_TYPE enDeviceType, 
	capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const
{
	uint16_t nDeviceCount = 0;
	switch(enDeviceType)
	{
	case ENUM_DEVICE_UNKNOWN:
		break;
	case ENUM_DEVICE_AUDIO_INPUT:
		nDeviceCount = GetAudioInputList(pArrDeviceInfo, nArrCount);
		break;
	case ENUM_DEVICE_AUDIO_OUTPUT:
		nDeviceCount = GetAudioOutputList(pArrDeviceInfo, nArrCount);
		break;
	case ENUM_DEVIDE_VIDEO_INPUT:
		nDeviceCount = GetVideoInputList(pArrDeviceInfo, nArrCount);
		break;
	default:
		break;
	}
	return nDeviceCount;
}

/// ���ݸ�����Ƶ�豸���Ƶõ��õ����豸��Filter
IBaseFilter* CCaptureDevice::GetDeviceFilter(ENUM_DEVICE_TYPE enDeviceType, 
	TCHAR* pszDeviceName, uint16_t nNameSize) const
{
	IBaseFilter* pFilter = NULL;
	switch(enDeviceType)
	{
	case ENUM_DEVICE_UNKNOWN:
		break;
	case ENUM_DEVICE_AUDIO_INPUT:
		pFilter = GetAudioInputFilter(pszDeviceName, nNameSize);
		break;
	case ENUM_DEVICE_AUDIO_OUTPUT:
		pFilter = GetAudioOutputFilter(pszDeviceName, nNameSize);
		break;
	case ENUM_DEVIDE_VIDEO_INPUT:
		pFilter = GetVideoInputFilter(pszDeviceName, nNameSize);
		break;
	default:
		break;
	}
	return pFilter;
}

/// �õ�ѡ��Filter���豸����
ENUM_VIDEO_DEVICE_TYPE CCaptureDevice::GetDeviceType(IBaseFilter* apFilter) const
{
	HRESULT hr = S_OK;
	IEnumPins* lpEnumPins = NULL ;
	IPin* lpPin = NULL; 
	IEnumMediaTypes* lpEnumMediaTypes = NULL;
	PIN_DIRECTION lPinDir;
	AM_MEDIA_TYPE* lpMediaType = NULL;
	ULONG lulFetched;

	hr = apFilter->EnumPins(&lpEnumPins);  
	if (hr != S_OK)
	{
		return ENUM_VIDEO_DEVICE_UNKNOWN;
	}

	lpEnumPins->Reset();
	hr = lpEnumPins->Next(1, &lpPin, &lulFetched);
	if (hr != S_OK)
	{
		lpEnumPins->Release();						//ȡpin��Ϣʧ��Ҫ�ͷ�lpEnumPins����
		return ENUM_VIDEO_DEVICE_VFW;
	}
	else
	{
		lpPin->Release();							//ȡpin��Ϣ�ɹ�Ҫ�ͷ�lpPin����
	}

	lpEnumPins->Reset();
	while(S_OK == lpEnumPins->Next(1, &lpPin, &lulFetched))
	{
		//ֻ����������ͣ��������������
		hr = lpPin->QueryDirection(&lPinDir); 
		if (hr != S_OK || PINDIR_INPUT == lPinDir)
		{
			lpPin->Release(); 
			continue;
		}

		hr = lpPin->EnumMediaTypes(&lpEnumMediaTypes);
		if (hr == S_OK)
		{
			while (S_OK == lpEnumMediaTypes->Next(1, &lpMediaType, &lulFetched))   
			{
				if ((MEDIASUBTYPE_DVSD == lpMediaType->subtype) ||
					(MEDIASUBTYPE_dvsd == lpMediaType->subtype)) 
				{
					DeleteMediaType(lpMediaType);
					return ENUM_VIDEO_DEVICE_DV;
				}
				DeleteMediaType(lpMediaType);
			}
			lpEnumMediaTypes->Release();  
		}
		lpPin->Release(); 
	}
	lpEnumPins->Release();  
	return ENUM_VIDEO_DEVICE_WDM;
}

//=============================================================================
/// �����Ƶ�����豸�б�
uint16_t CCaptureDevice::GetAudioInputList(
	capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//����ĳ���豸ö����

	if(nArrCount <= 0)
	{
		return 0;
	}

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return 0;	
	}

	//Ϊָ���豸���ʹ����豸ö����
	IEnumMoniker* lpEnumMoniker = NULL;			//ö�����ƽӿ�
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return 0;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IMoniker* lpMoniker = NULL;					//���ƽӿ�
	uint16_t nIndex = 0;

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		// �����豸����
		pArrDeviceInfo[nIndex].m_nDeviceProperty = 0;

		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				_tcscpy(pArrDeviceInfo[nIndex].m_szDeviceName, W2T(var.bstrVal));
				
				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
			nIndex++;
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return nIndex;
}

/// �����Ƶ����豸�б�
uint16_t CCaptureDevice::GetAudioOutputList(
	capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//����ĳ���豸ö����

	if(nArrCount <= 0)
	{
		return 0;
	}

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return 0;	
	}

	//Ϊָ���豸���ʹ����豸ö����
	IEnumMoniker* lpEnumMoniker = NULL;			//ö�����ƽӿ�
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_AudioRendererCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return 0;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IMoniker* lpMoniker = NULL;					//���ƽӿ�
	uint16_t nIndex = 0;

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		// �����豸����
		pArrDeviceInfo[nIndex].m_nDeviceProperty = 0;

		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				_tcscpy(pArrDeviceInfo[nIndex].m_szDeviceName, W2T(var.bstrVal));

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
			nIndex++;
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return nIndex;
}

/// �����Ƶ�����豸�б�
uint16_t CCaptureDevice::GetVideoInputList(
	capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//����ĳ���豸ö����

	if(nArrCount <= 0)
	{
		return 0;
	}

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return 0;	
	}

	//Ϊָ���豸���ʹ����豸ö����
	IEnumMoniker* lpEnumMoniker = NULL;			//ö�����ƽӿ�
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return 0;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IMoniker* lpMoniker = NULL;					//���ƽӿ�
	uint16_t nIndex = 0;

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		// �����豸����
		pArrDeviceInfo[nIndex].m_nDeviceProperty = 0;

		// �ж��Ƿ�����USB�豸
		//WCHAR* wzDisplayName = NULL;
		//hr = lpMoniker->GetDisplayName(NULL,NULL,&wzDisplayName);
		//if(SUCCEEDED(hr))
		//{
		//	if(wcsstr(wzDisplayName, L"@device:pnp:\\\\?\\usb") != NULL)
		//	{
		//		pArrDeviceInfo[nIndex].m_bIsUsbDevice = TRUE;
		//	}
		//	CoTaskMemFree(wzDisplayName);
		//}

		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				_tcscpy(pArrDeviceInfo[nIndex].m_szDeviceName, W2T(var.bstrVal));

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
			nIndex++;
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return nIndex;
}

/// �����Ƶ�����豸Filter
IBaseFilter* CCaptureDevice::GetAudioInputFilter(
	TCHAR* pszDeviceName, uint16_t nNameSize) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//����ĳ���豸ö����

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return NULL;	
	}

	//Ϊָ���豸���ʹ����豸ö����
	IEnumMoniker* lpEnumMoniker = NULL;			//ö�����ƽӿ�
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return NULL;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IBaseFilter* pFilter = NULL;
	IMoniker* lpMoniker = NULL;					//���ƽӿ�

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				if(_tcsncmp(pszDeviceName, W2T(var.bstrVal), nNameSize) == 0)
				{
					lpMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter); 
				}

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return pFilter;
}

/// �����Ƶ����豸Filter
IBaseFilter*  CCaptureDevice::GetAudioOutputFilter(
	TCHAR* pszDeviceName, uint16_t nNameSize) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//����ĳ���豸ö����

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return NULL;	
	}

	//Ϊָ���豸���ʹ����豸ö����
	IEnumMoniker* lpEnumMoniker = NULL;			//ö�����ƽӿ�
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_AudioRendererCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return NULL;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IBaseFilter* pFilter = NULL;
	IMoniker* lpMoniker = NULL;					//���ƽӿ�

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				if(_tcsncmp(pszDeviceName, W2T(var.bstrVal), nNameSize) == 0)
				{
					lpMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter); 
				}

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return pFilter;
}

/// �����Ƶ�����豸Filter
IBaseFilter*  CCaptureDevice::GetVideoInputFilter(
	TCHAR* pszDeviceName, uint16_t nNameSize) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//����ĳ���豸ö����

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return NULL;	
	}

	//Ϊָ���豸���ʹ����豸ö����
	IEnumMoniker* lpEnumMoniker = NULL;			//ö�����ƽӿ�
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return NULL;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IBaseFilter* pFilter = NULL;
	IMoniker* lpMoniker = NULL;					//���ƽӿ�

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				if(_tcsncmp(pszDeviceName, W2T(var.bstrVal), nNameSize) == 0)
				{
					lpMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter); 
				}

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return pFilter;
}
