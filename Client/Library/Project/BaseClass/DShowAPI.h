/******************************************************************************
* Copyright (c) 2012, Letion
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of California, Berkeley nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
///============================================================================
/// \file    : DShowAPI.h
/// \brief   : dshow��װ����ͷ�ļ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-17
///============================================================================
#ifndef __DSHOW_API_H__
#define __DSHOW_API_H__

#include <dshow.h>
#include <initguid.h>
#include "dshow\\streams.h"
#include "TypeDefine.h"
#include "DeviceDefine.h"

//=============================================================================
/// pin���Ƴ���
#define MAX_PIN_NAME_SIZE		256

/// pin��Ϣ
typedef struct _pin_info
{
	WCHAR	 m_szPinName[MAX_PIN_NAME_SIZE];		///< ����
	uint32_t m_nPinProperty;						///< ����
}pin_info_t;

// �ļ����
//=============================================================================
/// \brief	Filter�Ƿ���ע��
/// \param	inFilterId Filter CLSID
/// \return �ɹ�-TRUE ʧ��-FALSE
BOOL IsFilterRegistered(CLSID FilterID);

/// \brief	ע��Filter
/// \param	szFileName Filter�ļ�·��������
/// \return �ɹ�-TRUE ʧ��-FALSE
BOOL RegisterFilter(const WCHAR* szFileName);

/// \brief	ע��Filter
/// \param	szFileName Filter�ļ�·��������
/// \return �ɹ�-TRUE ʧ��-FALSE
BOOL UnRegisterFilter(const WCHAR* szFileName);

// GraphBuilder���
//=============================================================================
/// \brief	����Graph���ļ�
/// \param  pGraphBuilder IGraphBuilderָ��
/// \param  szPathName �ļ���
/// \return HRESULT
HRESULT SaveGraphFile(IGraphBuilder* pGraphBuilder, const WCHAR* szPathName);

/// \brief	����Graph���ļ�
/// \param  pGraphBuilder IGraphBuilderָ��
/// \param  szPathName �ļ���
/// \return HRESULT
HRESULT LoadGraphFile(IGraphBuilder* pGraphBuilder, const WCHAR* szPathName);

/// \brief	��GraphBuilder�в���ָ��Filter
/// \param  pGraphBuilder GraphBuilderָ��
/// \param  MediaTypeID Filter����
/// \param[OUT]  ppFilter Filterָ��
/// \return HRESULT
HRESULT FindRenderer(IGraphBuilder* pGraphBuilder, const GUID* MediaTypeID, 
	IBaseFilter **ppFilter);

/// \brief	��GraphBuilder�в�����Ƶ����Filter
/// \param  pGraphBuilder GraphBuilderָ��
/// \param[OUT]  ppFilter Filterָ��
/// \return HRESULT
HRESULT FindAudioRenderer(IGraphBuilder* pGraphBuilder, 
	IBaseFilter** ppFilter);

/// \brief	��GraphBuilder�в�����Ƶ����Filter
/// \param  pGraphBuilder GraphBuilderָ��
/// \param[OUT]  ppFilter Filterָ��
/// \return HRESULT
HRESULT FindVideoRenderer(IGraphBuilder* pGraphBuilder, 
	IBaseFilter **ppFilter);

/// \brief	��GraphBuilder�в���FILTER
/// \param	pGraphBuilder GraphBuilderָ��
/// \param	inClsid FilterID
/// \return �ɹ�-IBaseFilterָ�� ʧ��-NULL
IBaseFilter* FindFilterByCLSID(IGraphBuilder* pGraphBuilder, 
	REFCLSID inClsid);

/// \brief	��GraphBuilder��ɾ��ָ��Filter֮�������Filter
/// \param	pGraphBuilder GraphBuilderָ��
/// \param	pFilter Filterָ��
/// \return ��
void NukeDownstream(IGraphBuilder* pGraphBuilder, IBaseFilter* pFilter);

/// \brief	��GraphBuilder��ɾ��ָ��Filter֮ǰ������Filter
/// \param	pGraphBuilder GraphBuilderָ��
/// \param	pFilter Filterָ��
/// \return ��
void NukeUpstream(IGraphBuilder* pGraphBuilder, IBaseFilter* pFilter);

// �豸���
//=============================================================================
/// \brief	�õ������豸�б�
/// \param[IN]	CategoryID �豸����GUID
/// \param[OUT]	pInfoArr �豸��Ϣ����ָ��
/// \param		nInfoCount �豸��Ϣ��������
/// \return �豸����
uint16_t QueryDeviceCategory(GUID CategoryID, device_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	�õ���Ƶ�豸�б�
/// \param[OUT]	pInfoArr �豸��Ϣ����ָ��
/// \param		nInfoCount �豸��Ϣ��������
/// \return �豸����
uint16_t QueryVideoCaptureDevices(device_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	�õ���Ƶ�豸�б�
/// \param[OUT]	pInfoArr �豸��Ϣ����ָ��
/// \param		nInfoCount �豸��Ϣ��������
/// \return �豸����
uint16_t QueryAudioCaptureDevices(device_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	�õ�Filter����
/// \param	inFilter Filterָ��
/// \return �����豸���� 
ENUM_DEVICE_TYPE DecideDeviceType(IBaseFilter* inFilter);

// DV: "Microsoft DV Camera and VCR"
/// \brief	������Ƶ�豸Filter
/// \param	szFriendlyName Filter����
/// \return �ɹ�-IBaseFilterָ�� ʧ��-NULL
IBaseFilter* CreateVideoDeviceFilter(const WCHAR* szFriendlyName);

/// \brief	������Ƶ�豸Filter
/// \param	szFriendlyName Filter����
/// \return �ɹ�-IBaseFilterָ�� ʧ��-NULL
IBaseFilter* CreateAudioDeviceFilter(const WCHAR* szFriendlyName);

// Create device according to the display name As a bonus, return the friendly name too.
/// \brief	������Ƶ�豸Filter
/// \param	szDisplayName Filter��ʾ����
/// \param[OUT]	szFriendlyName Filter����
/// \param	nFriendlyNameSize Filter���Ƴ���
/// \return �ɹ�-IBaseFilterָ�� ʧ��-NULL
IBaseFilter* CreateVideoDeviceFilter(const WCHAR* szDisplayName, 
	WCHAR* szFriendlyName, uint16_t nFriendlyNameSize);

/// \brief	������Ƶ�豸Filter
/// \param	szDisplayName Filter��ʾ����
/// \param[OUT]	szFriendlyName Filter����
/// \param	nFriendlyNameSize Filter���Ƴ���
/// \return �ɹ�-IBaseFilterָ�� ʧ��-NULL
IBaseFilter* CreateAudioDeviceFilter(const WCHAR* szDisplayName, 
	WCHAR* szFriendlyName, uint16_t nFriendlyNameSize);

/// \brief	������Ƶ������Filter
/// \param	bIsVideo �Ƿ�����Ƶ�豸
/// \param	szFilterName Filter����
/// \return �ɹ�-IBaseFilterָ�� ʧ��-NULL
IBaseFilter* CreateVideoCompressorFilter(const WCHAR* szFilterName);

/// \brief	������Ƶ������Filter
/// \param	bIsVideo �Ƿ�����Ƶ�豸
/// \param	szFilterName Filter����
/// \return �ɹ�-IBaseFilterָ�� ʧ��-NULL
IBaseFilter* CreateAudioCompressorFilter(const WCHAR* szFilterName);

/// \brief	����Ӳ��Filter
/// \param	CategoryID CategoryID
/// \param	szFriendlyName Filter����
/// \return �ɹ�-IBaseFilterָ�� ʧ��-NULL
IBaseFilter* CreateHardwareFilter(GUID CategoryID, 
	const WCHAR* szFriendlyName);

/// \brief	�����豸Filter
/// \param	inCategory ����ID
/// \param	szDisplayName Filter��ʾ����
/// \param[OUT]	szFriendlyName Filter����
/// \param	nFriendlyNameSize Filter���Ƴ���
/// \return �ɹ�-IBaseFilterָ�� ʧ��-NULL
IBaseFilter* CreateHardwareFilter(GUID CategoryID, const WCHAR* szDisplayName, 
	WCHAR* szFriendlyName, uint16_t nFriendlyNameSize);

/// \brief	�����豸�Ƿ���ͬ
/// \param	inDeviceName1 �豸1����
/// \param	inDeviceName2 �豸2����
/// \return ��ͬ-TRUE ��ͬ-FALSE
BOOL IsSameDevice(const WCHAR* szDeviceName1, const WCHAR* szDeviceName2);

// Filter���
//=============================================================================
/// \brief	Filter�Ƿ�֧������ҳ
/// \param  pFilter Filterָ��
/// \return ֧��-TRUE ��֧��-FALSE
BOOL IsSupportPropertyPage(IBaseFilter* pFilter);

/// \brief	��ʾFilter����ҳ
/// \param  pFilter Filterָ��
/// \param  hwndParent �����ھ��
/// \return HRESULT
HRESULT ShowFilterPropertyPage(IBaseFilter* pFilter, HWND hwndParent);

/// \brief	Filter�Ƿ��������
/// \param	pFilter Filterָ��
/// \return ��-TRUE ��-FALSE
BOOL IsCompletelyConnected(IBaseFilter* pFilter);

/// \brief	�õ���һ��/��һ��Filter
/// \param	pFilter ������ʼFilter
/// \param	dir ��������
/// \param[OUT]	ppNext ���ڵ�Filter
/// \return HRESULT
HRESULT GetNextFilter(IBaseFilter* pFilter, PIN_DIRECTION enPinType, 
	IBaseFilter** ppNext);

// pin���
//=============================================================================
/// \brief	�õ�Filter��Pin������
/// \param	pFilter Filterָ��
/// \param[OUT]	outInputPin ����Pin����
/// \param[OUT]	outOutputPin ���Pin����
/// \return �ɹ�-TRUE ʧ��-FALSE
BOOL GetPinCount(IBaseFilter* pFilter, uint16_t* pInPinCount, 
	uint16_t* pOutPinCount);

/// \brief	�õ�Filter������Pin��Ϣ�б�
/// \param	pFilter Filterָ��
/// \param[OUT]	pInfoArr Pin��Ϣ����ָ��
/// \param	nInfoCount Pin��Ϣ��������
/// \return pin����
uint16_t GetInputPinInfo(IBaseFilter* pFilter, pin_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	�õ�Filter�����Pin��Ϣ�б�
/// \param	pFilter Filterָ��
/// \param[OUT]	pInfoArr Pin��Ϣ����ָ��
/// \param	nInfoCount Pin��Ϣ��������
/// \return pin����
uint16_t GetOutputPinInfo(IBaseFilter* pFilter, pin_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	�õ�Filter������Pin
/// \param	pFilter Filterָ��
/// \param	szPinName Pin���ƣ�Ĭ��NULL��
/// \return �ɹ�-IPinָ�� ʧ��-NULL
IPin* GetInputPin(IBaseFilter* pFilter, const WCHAR* szPinName);

/// \brief	�õ�Filter�е�����ӿ�ָ��
/// \param  pFilter Filterָ��
/// \param  iNum �ӿ����
/// \return �ӿ�ָ�� 
IPin* GetInputPin(IBaseFilter* pFilter, uint16_t nIndex);

/// \brief	�õ�Filter�����Pin
/// \param	pFilter Filterָ��
/// \param	szPinName Pin���ƣ�Ĭ��NULL��
/// \return �ɹ�-IPinָ�� ʧ��-NULL
IPin* GetOutputPin(IBaseFilter* pFilter, const WCHAR* szPinName);

/// \brief	�õ�Filter�е�����ӿ�ָ��
/// \param  pFilter Filterָ��
/// \param  iNum �ӿ����
/// \return �ӿ�ָ�� 
IPin* GetOutputPin(IBaseFilter* pFilter, uint16_t nIndex);

/// \brief	�õ�Filterδ���ӵ�����Pin
/// \param	pFilter Filterָ��
/// \return �ɹ�-IPinָ�� ʧ��-NULL
IPin* GetUnconnectInputPin(IBaseFilter* pFilter);

/// \brief	�õ�Filterδ���ӵ����Pin
/// \param	pFilter Filterָ��
/// \return �ɹ�-IPinָ�� ʧ��-NULL
IPin* GetUnconnectOutputPin(IBaseFilter* pFilter);

/// \brief	�õ���Ƶ�豸����Pin����
/// \param	inLineType 
///				line in    -> MIXERLINE_COMPONENTTYPE_SRC_LINE
///             microphone -> MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
///             CD Player  -> MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC
/// \param[OUT]	szPinName Pin����
/// \param		nPinNameSize Pin���Ƴ���
/// \return �ɹ�-TRUE ʧ��-FALSE
BOOL GetAudioInputPinName(uint16_t nLineType, 
	WCHAR* szPinName, uint16_t nPinNameSize);

//��������
//=============================================================================
/// \brief	ɾ��ý������
/// \param  pMediaType ý������
/// \return ��
void UtilDeleteMediaType(AM_MEDIA_TYPE* pMediaType);

/// \brief	�ͷ�ý������
/// \param  MediaType ý������
/// \return ��
void UtilFreeMediaType(AM_MEDIA_TYPE& MediaType);

/// \brief	�õ�ý�岥����Ϣ
/// \param  pMediaSeek IMediaSeekingָ��
/// \param[OUT]  szDuration ��Ϣ
/// \param	nDurationSize ��Ϣ����
/// \return HRESULT
HRESULT GetDurationString(IMediaSeeking* pMediaSeek, 
	WCHAR* szDuration, uint16_t nDurationSize);

#endif //__DSHOW_API_H__

