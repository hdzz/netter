/*
 * pkt_definition.cpp
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#include "pkt_definition.h"

PktHeartBeat::PktHeartBeat()
{
    SETUP_PKT_HEADER(PKT_ID_HEARTBEAT)
    
    WriteHeader();
}

PktHeartBeat::PktHeartBeat(uchar_t* buf, uint32_t len)
{
    READ_PKT_HEADER
    
    PARSE_PACKET_ASSERT
}



