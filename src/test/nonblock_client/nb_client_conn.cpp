/*
 * nb_client_conn.cpp
 *
 *  Created on: 2016-3-15
 *      Author: ziteng
 */

#include "nb_client_conn.h"
#include "pkt_definition.h"

void NbClientConn::OnConfirm()
{
    BaseConn::OnConfirm();
    
    PktHeartBeat hb_pkt;
    SendPkt(&hb_pkt);
}

void NbClientConn::HandlePkt(PktBase* pPkt)
{
    // continuous send packet to server when receive a packet
    PktHeartBeat hb_pkt;
    SendPkt(&hb_pkt);
}