/*
 * nb_client_conn.h
 *
 *  Created on: 2016-3-15
 *      Author: ziteng
 */

#ifndef __TEST_NB_CLIENT_CONN_H__
#define __TEST_NB_CLIENT_CONN_H__

#include "base_conn.h"

class NbClientConn : public BaseConn {
public:
    NbClientConn() {}
    virtual ~NbClientConn() {}
    
	virtual void HandlePkt(PktBase* pPkt);
};


#endif
