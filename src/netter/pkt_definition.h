/*
 * pkt_definition.h
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#ifndef __NETTER_PKT_DEFINITION_H__
#define __NETTER_PKT_DEFINITION_H__

#include "pkt_base.h"

class PktHeartBeat : public PktBase
{
public:
    PktHeartBeat();
    PktHeartBeat(uchar_t* buf, uint32_t len);
    virtual ~PktHeartBeat() {}
};


#endif /* __PKT_DEFINITION_H__ */
