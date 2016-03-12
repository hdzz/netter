//
//  PktDefinition.h
//  mogucache
//
//  Created by jianqing.du on 15-7-22.
//  Copyright (c) 2015年 mgj. All rights reserved.
//

#ifndef __PKT_DEFINITION_H__
#define __PKT_DEFINITION_H__

#include "pkt_base.h"

class PktHeartBeat : public PktBase
{
public:
    PktHeartBeat();
    PktHeartBeat(uchar_t* buf, uint32_t len);
    virtual ~PktHeartBeat() {}
};


#endif /* __PKT_DEFINITION_H__ */
