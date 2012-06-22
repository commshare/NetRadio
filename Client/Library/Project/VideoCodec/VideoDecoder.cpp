#include "StdAfx.h"
#include "VideoDecoder.h"


CVideoDecoder::CVideoDecoder(void)
	: m_enCodecType(ENUM_VIDEO_CODEC_H264)
	, m_nVideoWidth(0)
	, m_nVideoHeight(0)
{
}


CVideoDecoder::~CVideoDecoder(void)
{
}

/// ����������
BOOL CVideoDecoder::Create(ENUM_VIDEO_CODEC_TYPE enCodecType)
{
	BOOL bResult = FALSE;
	switch(enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		m_XvidDecoder.SetFrameInfo(m_nVideoWidth, m_nVideoHeight);
		bResult = m_XvidDecoder.Create();
		break;
	case ENUM_VIDEO_CODEC_H264:
		m_H264Decoder.SetFrameInfo(m_nVideoWidth, m_nVideoHeight);
		bResult = m_H264Decoder.Create();
		break;
	default:
		break;
	}
	m_enCodecType = enCodecType;
	return bResult;
}

/// ���ٱ�����
void CVideoDecoder::Destroy(void)
{
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		m_XvidDecoder.Destroy();
		break;
	case ENUM_VIDEO_CODEC_H264:
		m_H264Decoder.Destroy();
		break;
	default:
		break;
	}	
}

/// ������Ƶ֡��Ϣ
BOOL CVideoDecoder::SetFrameInfo(uint16_t nVideoWidth, 
	uint16_t nVideoHeight)
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		bResult = m_XvidDecoder.SetFrameInfo(nVideoWidth, nVideoHeight);
		break;
	case ENUM_VIDEO_CODEC_H264:
		bResult = m_H264Decoder.SetFrameInfo(nVideoWidth, nVideoHeight);
		break;
	default:
		break;
	}

	m_nVideoWidth = nVideoWidth;
	m_nVideoHeight = nVideoHeight;

	return bResult;
}

/// �����Ƶ֡��Ϣ
BOOL CVideoDecoder::GetFrameInfo(uint16_t& nVideoWidth,
	uint16_t& nVideoHeight) const
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		bResult = m_XvidDecoder.GetFrameInfo(nVideoWidth, nVideoHeight);
		break;
	case ENUM_VIDEO_CODEC_H264:
		bResult = m_H264Decoder.GetFrameInfo(nVideoWidth, nVideoHeight);
		break;
	default:
		break;
	}
	return bResult;
}

/// ����
int32_t CVideoDecoder::Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	uint32_t nResult = 0;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		nResult = m_XvidDecoder.Decodec(pSrcBuffer, nSrcBuffSize, 
			pDestBuffer, nDestBufferSize);
		break;
	case ENUM_VIDEO_CODEC_H264:
		nResult = m_H264Decoder.Decodec(pSrcBuffer, nSrcBuffSize, 
			pDestBuffer, nDestBufferSize);
		break;
	default:
		break;
	}
	return nResult;
}

