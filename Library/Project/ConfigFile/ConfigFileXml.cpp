#include "ConfigFileXml.h"

CConfigFileXml::CConfigFileXml(void)
	: m_pXmlDocument(NULL)
{
}


CConfigFileXml::~CConfigFileXml(void)
{
	CloseFile();
}

/// ��
BOOL CConfigFileXml::OpenFile(const char* szPathName)
{
	// �������
	if(NULL == szPathName)
		return FALSE;

	if(NULL != m_pXmlDocument)
		return FALSE;

	m_pXmlDocument = new TiXmlDocument;
	if(NULL == m_pXmlDocument)
		return FALSE;
	
	return m_pXmlDocument->LoadFile(szPathName, TIXML_ENCODING_UTF8);
}

/// �ر�
BOOL CConfigFileXml::CloseFile(void)
{
	if(NULL != m_pXmlDocument)
	{
		delete m_pXmlDocument;
		m_pXmlDocument = NULL;
		
		return TRUE;
	}
	return FALSE;
}

/// ����
BOOL CConfigFileXml::SaveFile(void)
{
	if(NULL != m_pXmlDocument)
	{
		return m_pXmlDocument->SaveFile();
	}
	return TRUE;
}

/// �������
const char* CConfigFileXml::GetValueString(const char* szSection, 
	const char* szKey, const char* szDefaultValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return szDefaultValue;

	if(NULL == m_pXmlDocument)
		return szDefaultValue;

	// ��ø��ڵ�
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// ��öνڵ�
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// ���key�ڵ�
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				return pElement->Attribute(szKey);
			}
		}
	}
	return szDefaultValue;
}

/// ��������
BOOL CConfigFileXml::SetValueString(const char* szSection, 
	const char* szKey, const char* szValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pXmlDocument)
		return FALSE;

	// ��ø��ڵ�
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// ��öνڵ�
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// ���key�ڵ�
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->SetAttribute(szKey, szValue);
				return TRUE;
			}
		}
	}
	return FALSE;
}

/// �������
int32_t CConfigFileXml::GetValueInt(const char* szSection, 
	const char* szKey, int32_t nDefaultValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return nDefaultValue;

	if(NULL == m_pXmlDocument)
		return nDefaultValue;

	int32_t nValue = nDefaultValue;

	// ��ø��ڵ�
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// ��öνڵ�
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// ���key�ڵ�
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->Attribute(szKey, (int*)&nValue);
			}
		}
	}
	return nValue;
}

/// ��������
BOOL CConfigFileXml::SetValueInt(const char* szSection, 
	const char* szKey, int32_t nValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pXmlDocument)
		return FALSE;

	// ��ø��ڵ�
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// ��öνڵ�
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// ���key�ڵ�
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->SetAttribute(szKey, nValue);
				return TRUE;
			}
		}
	}
	return FALSE;
}

/// �������
double CConfigFileXml::GetValueDouble(const char* szSection, 
	const char* szKey, double nDefaultValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return nDefaultValue;

	if(NULL == m_pXmlDocument)
		return nDefaultValue;

	double nValue = nDefaultValue;

	// ��ø��ڵ�
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// ��öνڵ�
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// ���key�ڵ�
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->Attribute(szKey, (double*)&nValue);
			}
		}
	}
	return nValue;
}

/// ��������
BOOL CConfigFileXml::SetValueDouble(const char* szSection, 
	const char* szKey, double nValue)
{
	// �������
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pXmlDocument)
		return FALSE;

	// ��ø��ڵ�
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// ��öνڵ�
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// ���key�ڵ�
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->SetAttribute(szKey, nValue);
				return TRUE;
			}
		}
	}
	return FALSE;
}