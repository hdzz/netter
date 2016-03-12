//
//  PktDefinition.cpp
//  mogucache
//
//  Created by jianqing.du on 15-7-22.
//  Copyright (c) 2015年 mgj. All rights reserved.
//

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



