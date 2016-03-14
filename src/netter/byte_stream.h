/*
 * byte_stream.h
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#ifndef __NETTER_BYTE_STREAM_H__
#define __NETTER_BYTE_STREAM_H__

#include "ostype.h"
#include "util.h"
#include "simple_buffer.h"

class CByteStream
{
public:
	CByteStream(uchar_t* buf, uint32_t len);
	CByteStream(SimpleBuffer* pSimpBuf, uint32_t pos);
	~CByteStream() {}
    
	unsigned char* GetBuf() { return m_pSimpBuf ? m_pSimpBuf->GetBuffer() : m_pBuf; }
	uint32_t GetPos() { return m_pos; }
	uint32_t GetLen() { return m_len; }
	void Skip(uint32_t len) {
		m_pos += len;
		if (m_pos > m_len) {
			throw PktException(ERROR_CODE_PARSE_FAILED, "parse packet failed!");
		}
	}
    
	static int16_t ReadInt16(uchar_t* buf);
	static uint16_t ReadUint16(uchar_t* buf);
	static int32_t ReadInt32(uchar_t* buf);
	static uint32_t ReadUint32(uchar_t* buf);
    static int64_t ReadInt64(uchar_t* buf);
    static uint64_t ReadUint64(uchar_t* buf);
	
    static void WriteInt16(uchar_t* buf, int16_t data);
	static void WriteUint16(uchar_t* buf, uint16_t data);
	static void WriteInt32(uchar_t* buf, int32_t data);
	static void WriteUint32(uchar_t* buf, uint32_t data);
    static void WriteInt64(uchar_t* buf, int64_t data);
    static void WriteUint64(uchar_t* buf, uint64_t data);
    
    void operator << (int8_t data);
    void operator << (uint8_t data);
    void operator << (int16_t data);
    void operator << (uint16_t data);
    void operator << (int32_t data);
    void operator << (uint32_t data);
    void operator << (int64_t data);
    void operator << (uint64_t data);
    
    void operator >> (int8_t& data);
    void operator >> (uint8_t& data);
    void operator >> (int16_t& data);
    void operator >> (uint16_t& data);
    void operator >> (int32_t& data);
	void operator >> (uint32_t& data);
    void operator >> (int64_t& data);
    void operator >> (uint64_t& data);
    
	void WriteString(const char* str);
	void WriteString(const char* str, uint32_t len);
	char* ReadString(uint32_t& len);
    
    void WriteString(const string& str_obj);
    void ReadString(string& str_obj);
    
	void WriteData(uchar_t* data, uint32_t len);
	uchar_t* ReadData(uint32_t& len);
private:
	void _WriteByte(void* buf, uint32_t len);
	void _ReadByte(void* buf, uint32_t len);
private:
	SimpleBuffer*	m_pSimpBuf;
	uchar_t*		m_pBuf;
	uint32_t		m_len;
	uint32_t		m_pos;
};

#endif
