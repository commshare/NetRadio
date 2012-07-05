#include "ConfigFile.h"

CConfigFile::CConfigFile(void)
	: m_pConfigFile(NULL)
{
}


CConfigFile::~CConfigFile(void)
{
	CloseFile();
}

/// ��
BOOL CConfigFile::OpenFile(const char* szPathName, 
	ENUM_CONFIG_FILE_TYPE enFileType)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	// �������
	if(NULL == szPathName)
		return FALSE;

	// �ظ��������
	if(NULL != m_pConfigFile)
		return FALSE;

	switch(enFileType)
	{
	case CONFIG_FILE_INI:
		m_pConfigFile = new CConfigFileIni;
		break;
	case CONFIG_FILE_XML:
		m_pConfigFile = new CConfigFileXml;
		break;
	default:
		break;
	}

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->OpenFile(szPathName);
	}
	return FALSE;
}

// �ر�
BOOL CConfigFile::CloseFile(void)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->CloseFile();
	}
	return FALSE;
}

/// ����
BOOL CConfigFile::SaveFile(void)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->SaveFile();
	}
	return FALSE;
}

/// �������
const char* CConfigFile::GetValueString(const char* szSection, 
	const char* szKey, const char* szDefaultValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->GetValueString(szSection, szKey, szDefaultValue);
	}

	return szDefaultValue;
}

/// ��������
BOOL CConfigFile::SetValueString(const char* szSection, 
	const char* szKey, const char* szValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->SetValueString(szSection, szKey, szValue);
	}
	return FALSE;
}

/// �������
int32_t CConfigFile::GetValueInt(const char* szSection, 
	const char* szKey, int32_t nDefaultValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->GetValueInt(szSection, szKey, nDefaultValue);
	}
	return nDefaultValue;
}

/// ��������
BOOL CConfigFile::SetValueInt(const char* szSection, 
	const char* szKey, int32_t nValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->SetValueInt(szSection, szKey, nValue);
	}
	return FALSE;
}

/// �������
double CConfigFile::GetValueDouble(const char* szSection, 
	const char* szKey, double nDefaultValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->GetValueDouble(szSection, szKey, nDefaultValue);
	}
	return nDefaultValue;
}

/// ��������
BOOL CConfigFile::SetValueDouble(const char* szSection, 
	const char* szKey, double nValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->SetValueDouble(szSection, szKey, nValue);
	}
	return FALSE;
}