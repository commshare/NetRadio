#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include "ConfigFileIni.h"

//=============================================================================
CConfigFileIni::CConfigFileIni(void)
	: m_pFileHandle(NULL)
{
}


CConfigFileIni::~CConfigFileIni(void)
{
	CloseFile();
}

/// ��
BOOL CConfigFileIni::OpenFile(const char* szPathName)
{
	// �������
	if(NULL == szPathName)
		return FALSE;

	// �ظ��������
	if(NULL != m_pFileHandle)
		return FALSE;

	BOOL bResult = FALSE;
	
	do 
	{
		m_pFileHandle = fopen(szPathName, "rb");
		if(NULL == m_pFileHandle)
			break;

		if(m_SimpleIni.LoadFile(m_pFileHandle) >= 0)
		{
			return TRUE;
		}
	} while (FALSE);


	return FALSE;
}

/// �ر�
BOOL CConfigFileIni::CloseFile(void)
{
	if(NULL != m_pFileHandle)
	{
		m_SimpleIni.Reset();
		fclose(m_pFileHandle);
		m_pFileHandle = NULL;
		return TRUE;
	}
	return FALSE;
}

/// ����
BOOL CConfigFileIni::SaveFile(void)
{
	if(NULL != m_pFileHandle)
	{
		if(m_SimpleIni.SaveFile(m_pFileHandle) >= 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/// �������
const char* CConfigFileIni::GetValueString(const char* szSection, 
	const char* szKey, const char* szDefaultValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return szDefaultValue;

	if(NULL == m_pFileHandle)
		return szDefaultValue;

	return m_SimpleIni.GetValue(szSection, szKey, szDefaultValue);
}

/// ��������
BOOL CConfigFileIni::SetValueString(const char* szSection, 
	const char* szKey, const char* szValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pFileHandle)
		return FALSE;

	return (m_SimpleIni.SetValue(szSection, szKey, szValue) >= 0);
}

/// �������
int32_t CConfigFileIni::GetValueInt(const char* szSection, 
	const char* szKey, int32_t nDefaultValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return nDefaultValue;

	if(NULL == m_pFileHandle)
		return nDefaultValue;

	return m_SimpleIni.GetLongValue(szSection, szKey, nDefaultValue);
}

/// ��������
BOOL CConfigFileIni::SetValueInt(const char* szSection, 
	const char* szKey, int32_t nValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pFileHandle)
		return FALSE;

	return (m_SimpleIni.SetLongValue(szSection, szKey, nValue) >= 0);
}

/// �������
double CConfigFileIni::GetValueDouble(const char* szSection, 
	const char* szKey, double nDefaultValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return nDefaultValue;

	if(NULL == m_pFileHandle)
		return nDefaultValue;

	return m_SimpleIni.GetDoubleValue(szSection, szKey, nDefaultValue);
}

/// ��������
BOOL CConfigFileIni::SetValueDouble(const char* szSection, 
	const char* szKey, double nValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pFileHandle)
		return FALSE;

	return (m_SimpleIni.SetDoubleValue(szSection, szKey, nValue) >= 0);
}