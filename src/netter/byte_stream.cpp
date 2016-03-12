//
//  byte_stream.cpp
//  netter
//
//  Created by ziteng on 16-3-12.
//  Copyright (c) 2016年 mgj. All rights reserved.
//

#include "byte_stream.h"

CByteStream::CByteStream(uchar_t* buf, uint32_t len)
{
	m_pBuf = buf;
	m_len = len;
	m_pSimpBuf = NULL;
	m_pos = 0;
}

CByteStream::CByteStream(SimpleBuffer* pSimpBuf, uint32_t pos)
{
	m_pSimpBuf = pSimpBuf;
	m_pos = pos;
	m_pBuf = NULL;
	m_len = 0;
}

int16_t CByteStream::ReadInt16(uchar_t *buf)
{
	int16_t data = (buf[0] << 8) | buf[1];
	return data;
}

uint16_t CByteStream::ReadUint16(uchar_t* buf)
{
	uint16_t data = (buf[0] << 8) | buf[1];
	return data;
}

int32_t CByteStream::ReadInt32(uchar_t *buf)
{
	int32_t data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	return data;
}

uint32_t CByteStream::ReadUint32(uchar_t *buf)
{
	uint32_t data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	return data;
}

int64_t CByteStream::ReadInt64(uchar_t *buf)
{
    int64_t data = ((int64_t)buf[0] << 56) | ((int64_t)buf[1] << 48) | ((int64_t)buf[2] << 40) | ((int64_t)buf[3] << 32) |
    ((int64_t)buf[4] << 24) | ((int64_t)buf[5] << 16) | ((int64_t)buf[6] << 8)  | (int64_t)buf[7];
    return data;
}

uint64_t CByteStream::ReadUint64(uchar_t *buf)
{
    uint64_t data = ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) | ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) |
    ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) | ((uint64_t)buf[6] << 8)  | (uint64_t)buf[7];
    return data;
}

void CByteStream::WriteInt16(uchar_t *buf, int16_t data)
{
	buf[0] = static_cast<uchar_t>(data >> 8);
	buf[1] = static_cast<uchar_t>(data & 0xFF);
}

void CByteStream::WriteUint16(uchar_t *buf, uint16_t data)
{
	buf[0] = static_cast<uchar_t>(data >> 8);
	buf[1] = static_cast<uchar_t>(data & 0xFF);
}

void CByteStream::WriteInt32(uchar_t *buf, int32_t data)
{
	buf[0] = static_cast<uchar_t>(data >> 24);
	buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
	buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
	buf[3] = static_cast<uchar_t>(data & 0xFF);
}

void CByteStream::WriteUint32(uchar_t *buf, uint32_t data)
{
	buf[0] = static_cast<uchar_t>(data >> 24);
	buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
	buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
	buf[3] = static_cast<uchar_t>(data & 0xFF);
}

void CByteStream::WriteInt64(uchar_t *buf, int64_t data)
{
    buf[0] = static_cast<uchar_t>(data >> 56);
    buf[1] = static_cast<uchar_t>((data >> 48) & 0xFF);
    buf[2] = static_cast<uchar_t>((data >> 40) & 0xFF);
    buf[3] = static_cast<uchar_t>((data >> 32) & 0xFF);
    buf[4] = static_cast<uchar_t>((data >> 24) & 0xFF);
    buf[5] = static_cast<uchar_t>((data >> 16) & 0xFF);
    buf[6] = static_cast<uchar_t>((data >> 8)  & 0xFF);
    buf[7] = static_cast<uchar_t>(data         & 0xFF);
}

void CByteStream::WriteUint64(uchar_t *buf, uint64_t data)
{
    buf[0] = static_cast<uchar_t>(data >> 56);
    buf[1] = static_cast<uchar_t>((data >> 48) & 0xFF);
    buf[2] = static_cast<uchar_t>((data >> 40) & 0xFF);
    buf[3] = static_cast<uchar_t>((data >> 32) & 0xFF);
    buf[4] = static_cast<uchar_t>((data >> 24) & 0xFF);
    buf[5] = static_cast<uchar_t>((data >> 16) & 0xFF);
    buf[6] = static_cast<uchar_t>((data >> 8)  & 0xFF);
    buf[7] = static_cast<uchar_t>(data         & 0xFF);
}


void CByteStream::operator << (int8_t data)
{
	_WriteByte(&data, 1);
}

void CByteStream::operator << (uint8_t data)
{
	_WriteByte(&data, 1);
}

void CByteStream::operator << (int16_t data)
{
	unsigned char buf[2];
	buf[0] = static_cast<uchar_t>(data >> 8);
	buf[1] = static_cast<uchar_t>(data & 0xFF);
	_WriteByte(buf, 2);
}

void CByteStream::operator << (uint16_t data)
{
	unsigned char buf[2];
	buf[0] = static_cast<uchar_t>(data >> 8);
	buf[1] = static_cast<uchar_t>(data & 0xFF);
	_WriteByte(buf, 2);
}

void CByteStream::operator << (int32_t data)
{
	unsigned char buf[4];
	buf[0] = static_cast<uchar_t>(data >> 24);
	buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
	buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
	buf[3] = static_cast<uchar_t>(data & 0xFF);
	_WriteByte(buf, 4);
}

void CByteStream::operator << (uint32_t data)
{
	unsigned char buf[4];
	buf[0] = static_cast<uchar_t>(data >> 24);
	buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
	buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
	buf[3] = static_cast<uchar_t>(data & 0xFF);
	_WriteByte(buf, 4);
}

void CByteStream::operator << (int64_t data)
{
    unsigned char buf[8];
    buf[0] = static_cast<uchar_t>(data >> 56);
    buf[1] = static_cast<uchar_t>((data >> 48) & 0xFF);
    buf[2] = static_cast<uchar_t>((data >> 40) & 0xFF);
    buf[3] = static_cast<uchar_t>((data >> 32) & 0xFF);
    buf[4] = static_cast<uchar_t>((data >> 24) & 0xFF);
    buf[5] = static_cast<uchar_t>((data >> 16) & 0xFF);
    buf[6] = static_cast<uchar_t>((data >> 8)  & 0xFF);
    buf[7] = static_cast<uchar_t>(data         & 0xFF);
    _WriteByte(buf, 8);
}

void CByteStream::operator << (uint64_t data)
{
    unsigned char buf[8];
    buf[0] = static_cast<uchar_t>(data >> 56);
    buf[1] = static_cast<uchar_t>((data >> 48) & 0xFF);
    buf[2] = static_cast<uchar_t>((data >> 40) & 0xFF);
    buf[3] = static_cast<uchar_t>((data >> 32) & 0xFF);
    buf[4] = static_cast<uchar_t>((data >> 24) & 0xFF);
    buf[5] = static_cast<uchar_t>((data >> 16) & 0xFF);
    buf[6] = static_cast<uchar_t>((data >> 8)  & 0xFF);
    buf[7] = static_cast<uchar_t>(data         & 0xFF);
    _WriteByte(buf, 8);
}

void CByteStream::operator >> (int8_t& data)
{
	_ReadByte(&data, 1);
}

void CByteStream::operator >> (uint8_t& data)
{
	_ReadByte(&data, 1);
}

void CByteStream::operator >> (int16_t& data)
{
	unsigned char buf[2];
    
	_ReadByte(buf, 2);
    
	data = (buf[0] << 8) | buf[1];
}

void CByteStream::operator >> (uint16_t& data)
{
	unsigned char buf[2];
    
	_ReadByte(buf, 2);
    
	data = (buf[0] << 8) | buf[1];
}

void CByteStream::operator >> (int32_t& data)
{
	unsigned char buf[4];
    
	_ReadByte(buf, 4);
    
	data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

void CByteStream::operator >> (uint32_t& data)
{
	unsigned char buf[4];
    
	_ReadByte(buf, 4);
    
	data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

void CByteStream::operator >> (int64_t& data)
{
    unsigned char buf[8];
    
    _ReadByte(buf, 8);
    
    data = ((int64_t)buf[0] << 56) | ((int64_t)buf[1] << 48) | ((int64_t)buf[2] << 40) | ((int64_t)buf[3] << 32) |
    ((int64_t)buf[4] << 24) | ((int64_t)buf[5] << 16) | ((int64_t)buf[6] << 8)  | (int64_t)buf[7];
}

void CByteStream::operator >> (uint64_t& data)
{
    unsigned char buf[8];
    
    _ReadByte(buf, 8);
    
    data = ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) | ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) |
    ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) | ((uint64_t)buf[6] << 8)  | (uint64_t)buf[7];
}

void CByteStream::WriteString(const char *str)
{
	uint32_t size = str ? (uint32_t)strlen(str) : 0;
    
	*this << size;
	_WriteByte((void*)str, size);
}

void CByteStream::WriteString(const char *str, uint32_t len)
{
	*this << len;
	_WriteByte((void*)str, len);
}

char* CByteStream::ReadString(uint32_t& len)
{
	*this >> len;
	char* pStr = (char*)GetBuf() + GetPos();
	Skip(len);
	return pStr;
}

void CByteStream::WriteString(const string& str_obj)
{
    WriteString(str_obj.c_str(), (uint32_t)str_obj.size());
}

void CByteStream::ReadString(string& str_obj)
{
    uint32_t len;
    *this >> len;
    char* pStr = (char*)GetBuf() + GetPos();
    Skip(len);
    
    str_obj.clear();
    str_obj.append(pStr, len);
}

void CByteStream::WriteData(uchar_t *data, uint32_t len)
{
	*this << len;
	_WriteByte(data, len);
}

uchar_t* CByteStream::ReadData(uint32_t &len)
{
	*this >> len;
	uchar_t* pData = (uchar_t*)GetBuf() + GetPos();
	Skip(len);
	return pData;
}

void CByteStream::_ReadByte(void* buf, uint32_t len)
{
	if (m_pos + len > m_len) {
		throw PktException(ERROR_CODE_PARSE_FAILED, "parase packet failed!");
	}
    
	if (m_pSimpBuf)
		m_pSimpBuf->Read((char*)buf, len);
	else
		memcpy(buf, m_pBuf + m_pos, len);
    
	m_pos += len;
}

void CByteStream::_WriteByte(void* buf, uint32_t len)
{
	if (m_pBuf && (m_pos + len > m_len))
		return;
    
	if (m_pSimpBuf)
		m_pSimpBuf->Write((char*)buf, len);
	else
		memcpy(m_pBuf + m_pos, buf, len);
    
	m_pos += len;
}