//
//  ServConn.cpp
//  EventLoop
//
//  Created by ziteng on 16-3-3.
//  Copyright (c) 2016年 mgj. All rights reserved.
//

#include "serv_conn.h"
#include "pkt_definition.h"
#include "mutex.h"
#include "thread_pool.h"

Mutex   g_qps_mutex;
int     g_qps = 0;
int     g_task_id = 0;
uint64_t    g_last_tick = get_tick_count();
ThreadPool g_worker_threads;


class WorkTask : public Task
{
public:
    WorkTask(net_handle_t handle, int task_id) {
        conn_handle_ = handle;
        task_id_ = task_id;
    }
    
    virtual ~WorkTask() {
        //printf("~WorkTask, task_id=%d\n", task_id_);
    }
    
    void run() {
        PktHeartBeat* pkt = new PktHeartBeat();
        BaseConn::SendPkt(conn_handle_, pkt);
    }
            
private:
    net_handle_t    conn_handle_;
    int             task_id_;
};

void init_server_thread_pool(int worker_thread_num)
{
    g_worker_threads.Init(worker_thread_num);
}

ServConn::ServConn()
{
    printf("ServConn::ServConn\n");
}

ServConn::~ServConn()
{
    printf("ServConn::~ServConn, handle=%d\n", m_handle);
}

void ServConn::OnConnect(BaseSocket* base_socket)
{
    BaseConn::OnConnect(base_socket);
    printf("connect from Client %s:%d, handle=%d, thread=%ld\n",
           m_peer_ip.c_str(), m_peer_port, m_handle, (long)pthread_self());
}

void ServConn::HandlePkt(PktBase* pkt)
{
    //printf("receive packet id: %d\n", pkt->GetPktId());
    {
        uint64_t cur_tick = get_tick_count();
        MutexGuard mg(g_qps_mutex);
        ++g_qps;
        ++g_task_id;
        if (cur_tick > g_last_tick + 1000) {
            printf("qps=%d\n", g_qps);
            g_last_tick = cur_tick;
            g_qps = 0;
        }
    }
    PktHeartBeat pkt_resp;
    SendPkt(&pkt_resp);
    
    //WorkTask* task = new WorkTask(m_handle, g_task_id);
    //g_worker_threads.AddTask(task);
}
