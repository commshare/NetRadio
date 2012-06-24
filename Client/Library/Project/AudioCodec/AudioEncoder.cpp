#include "StdAfx.h"
#include "AudioEncoder.h"


CAudioEncoder::CAudioEncoder(void)
	: m_enCodecType(ENUM_AUDIO_CODEC_AAC)
{
}


CAudioEncoder::~CAudioEncoder(void)
{
}

/// ����������
BOOL CAudioEncoder::Create(ENUM_AUDIO_CODEC_TYPE enCodecType)
{
	BOOL bResult = FALSE;
	switch(enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	bResult = m_Mp3Encoder.Create();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		bResult = m_AacEncoder.Create();
		break;
	default:
		break;
	}
	m_enCodecType = enCodecType;
	return bResult;
}

/// ���ٱ�����
void CAudioEncoder::Destroy(void)
{
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	m_Mp3Encoder.Destroy();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		m_AacEncoder.Destroy();
		break;
	default:
		break;
	}
}

/// �����Ƶ����
BOOL CAudioEncoder::GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
	ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	m_Mp3Encoder.Destroy();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		bResult = m_AacEncoder.GetAudioFormat(enFrequency, enChannel, enSample);
		break;
	default:
		break;
	}
	return bResult;
}

/// ������Ƶ����
BOOL CAudioEncoder::SetAudioFormat(ENUM_FREQUENCY_TYPE enFrequency, 
	ENUM_CHANNEL_TYPE enChannel, ENUM_SAMPLE_TYPE enSample)
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	m_Mp3Encoder.Destroy();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		bResult = m_AacEncoder.SetAudioFormat(enFrequency, enChannel, enSample);
		break;
	default:
		break;
	}
	return bResult;
}

/// ������Ƶ����
BOOL CAudioEncoder::SetAudioQuant(uint16_t nQuant)
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	m_Mp3Encoder.Destroy();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		bResult = m_AacEncoder.SetAudioQuant(nQuant);
		break;
	default:
		break;
	}
	return bResult;
}

/// �����Ƶ����
uint16_t CAudioEncoder::GetAudioQuant(void) const
{
	uint16_t nQuant = 0;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	m_Mp3Encoder.Destroy();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		nQuant = m_AacEncoder.GetAudioQuant();
		break;
	default:
		break;
	}
	return nQuant;
}

/// ���ô���
BOOL CAudioEncoder::SetBandWidth(uint32_t nBandWidth)
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	m_Mp3Encoder.Destroy();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		bResult = m_AacEncoder.SetBandWidth(nBandWidth);
		break;
	default:
		break;
	}
	return bResult;
}

/// ��ô���
uint32_t CAudioEncoder::GetBandWidth(void) const
{
	uint32_t nBandWidth = 0;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	m_Mp3Encoder.Destroy();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		nBandWidth = m_AacEncoder.GetBandWidth();
		break;
	default:
		break;
	}
	return nBandWidth;
}

/// ����
int32_t CAudioEncoder::Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	int32_t nResult = 0;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	nResult = m_Mp3Encoder.Encodec(pSrcBuffer, nSrcBuffSize, pDestBuffer, nDestBufferSize);
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		nResult = m_AacEncoder.Encodec(pSrcBuffer, nSrcBuffSize, pDestBuffer, nDestBufferSize);
		break;
	default:
		break;
	}
	return nResult;
}