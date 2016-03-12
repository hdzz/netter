/*
 * PktBase.h
 *
 *  Created on: 2013-8-27
 *      Author: ziteng@mogujie.com
 */

#ifndef __PKT_BASE_H__
#define __PKT_BASE_H__

#include "simple_buffer.h"
#include "byte_stream.h"

#define PKT_VERSION     1
#define PKT_HEADER_LEN  20

#define SETUP_PKT_HEADER(PKT_ID) \
    m_pkt_header.pkt_id = PKT_ID; \
    CByteStream os(&m_buf, PKT_HEADER_LEN); \
    m_buf.Write(NULL, PKT_HEADER_LEN);

#define READ_PKT_HEADER \
    ReadPktHeader(buf, PKT_HEADER_LEN, &m_pkt_header); \
    CByteStream is(buf + PKT_HEADER_LEN, len - PKT_HEADER_LEN);

#define PARSE_PACKET_ASSERT \
    if (is.GetPos() != (len - PKT_HEADER_LEN)) { \
        throw PktException(ERROR_CODE_PARSE_FAILED, "parse pkt failed"); \
    }

enum {
    PKT_ID_HEARTBEAT  = 1,
};

//////////////////////////////
typedef  struct {
    uint32_t    length;     // 整个数据包长度
    uint16_t    version;    // 数据包版本号，用于版本兼容
    uint16_t    flag;       // 预留的标志，以后可以用于压缩，加密
    uint32_t    pkt_id;     // 数据包类型ID
    uint32_t    req_time;   // 请求的毫秒时间戳，用于统计请求消耗时间
    uint32_t    seq_no;     // 请求序列号
} pkt_header_t;

class PktBase
{
public:
	PktBase();
	virtual ~PktBase() {}

	uchar_t* GetBuffer();
	uint32_t GetLength();

	uint16_t GetVersion() { return m_pkt_header.version; }
	uint16_t GetFlag() { return m_pkt_header.flag; }
	uint32_t GetPktId() { return m_pkt_header.pkt_id; }
	uint16_t GetReqTime() { return m_pkt_header.req_time; }
    uint32_t GetSeqNo() { return m_pkt_header.seq_no; }

	void SetVersion(uint16_t version);
	void SetFlag(uint16_t flag);
    void SetReqTime(uint32_t req_time);
    void SetSeqNo(uint32_t seq_no);

    void SetPktHeader(PktBase* pPkt);
   
	void WriteHeader();

	static bool IsPktAvailable(uchar_t* buf, uint32_t len, uint32_t& pkt_len);
	static int ReadPktHeader(uchar_t* buf, uint32_t len, pkt_header_t* header);
	static PktBase* ReadPacket(uchar_t* buf, uint32_t len);
private:
	void _SetIncomingLen(uint32_t len) { m_incoming_len = len; }
	void _SetIncomingBuf(uchar_t* buf) { m_incoming_buf = buf; }

protected:
	SimpleBuffer	m_buf;
	uchar_t*		m_incoming_buf;
	uint32_t		m_incoming_len;
	pkt_header_t    m_pkt_header;
};


#endif /* __PKT_BASE_H__ */
