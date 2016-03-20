###netter网络库介绍

#### 1. 多线程Reactor模型
具体可以参考Doug Lea的 [Scalable IO in Java](http://gee.cs.oswego.edu/dl/cpjslides/nio.pdf)

* 主线程是一个EventLoop，用于监听客户端的连接。
* 一个IO线程池，每个线程是一个EventLoop，所有连接的IO操作都在IO线程里面进行，两种情况会把连接注册到IO线程的Eventloop: 1) 主线程的Eventloop接受一个客户端连接，2)在任一一个线程，客户端通过Connect()接口连接服务端后。注册之后，对一个连接来说，所有的事件触发都在一个IO线程里面完成, 但不同连接可能在不同的IO线程，所以如果要修改一些全局的数据结构，需要加线程锁。
* 一个Work线程池，对于耗时的处理请求，应该生成一个Task，放到其中的一个Work线程执行， 执行完后，把回复包放回IO线程发送给客户端。

#### 2. 接口使用说明
#####1) event_loop.h
```
void init_thread_event_loops(int io_thread_num);

EventLoop* get_main_event_loop();
EventLoop* get_io_event_loop(net_handle_t handle);

// class EventLoop
void AddTimer(callback_t callback, void* user_data, 
			uint64_t interval);
void Start(uint32_t wait_timeout = 100);    
void Stop()；
```
说明：
init_thread_event_loops()用来初始化主线程和IO线程的Eventloop
io_thread_num = 0表示没有独立的IO线程，所有IO都在主线程处理
否则，启动io_thread_num个线程的线程池来处理IO事件

get_main_event_loop()用来获取主线程的Eventloop
get_io_event_loop()用来获取handle所在的IO线程的Eventloop，一般在base库内使用

每个连接对象的OnTimer()都是在各自的IO线程里面触发，应用不需要关心。
如果要注册其他的Timer，可以在主线程的Eventloop里面用AddTimer接口
注册定时器，定时器的精度是Start()接口的wait_timeout，默认是100ms

Start() 启动Eventloop，应用只需要启动主线程的Eventloop，IO线程的Eventloop由IO线程池自己启动，应用一般在main函数的最后调用一下
get_main_event_loop()->Start();

Stop() 停止某个IO线程的Eventloop，测试时用一下，没卵用

#####2) base_listener.h
```
template <class T>
int start_listen(const string& server_ip, uint16_t port)
```
说明: 
启动监听到server_ip:port的服务，生成客户端连接的对象用模板T表示
比如用 start_listen< HttpConn >("127.0.0.1", 8888); 
启动一个监听到127.0.0.1:8888的http服务，用HttpConn来生成客户端连接对象

#####3) base_conn.h
```
void init_thread_base_conn(int io_thread_num);

// class BaseConn
virtual net_handle_t Connect(
			const string& server_ip, 
			uint16_t server_port);
virtual void Close();
    
virtual void OnConnect(BaseSocket* base_socket);
virtual void OnConfirm();
virtual void OnRead();
virtual void OnWrite();
virtual void OnClose();
virtual void OnTimer(uint64_t curr_tick);

virtual void HandlePkt(CPktBase* pPkt) {}
	
int SendPkt(CPktBase* pkt);
int Send(void* data, int len);

static int SendPkt(net_handle_t handle, CPktBase* pkt);
static int CloseHandle(net_handle_t handle); 
```
说明:
init_thread_base_conn()用来初始化BaseConn的IO线程池管理，
每一个IO线程有一个handle->BaseConn的映射表，还有每个IO线程需要发送的数据包队列

应用的连接对象需要继承BaseConn

OnConnect()表示客户端刚连接进来，如果应用override OnConnect()方法，
那么在OnConnect()里面需要调用BaseConn::OnConnect()，用来处理BaseConn里面的一些成员初始化和连接对象添加工作

OnConfirm()表示成功连接服务端，如果应用override OnConfirm()方法，
那么在OnConform()里面需要调用BaseConn::OnConform()，用来处理BaseConn里面的一些成员初始化和连接对象添加工作

OnRead()表示可读事件，如果处理CPktBase类型的数据包，应用不用关心，
应用只需要override HandlePkt()方法

OnWrite()表示可写事件，应用不用关心

OnClose()表示连接关闭(对端关闭连接或连接服务器失败), 如果应用override OnClose()方法，那么在OnClose()里面需要调用BaseConn::OnClose()，用来处理BaseConn里面的一些连接对象清除工作

OnTimer()表示定时器事件，默认1秒触发一次

Connect()连接服务器的方法，如果要override该方法，应用需要先调用
BaseConn::Connect()来做一些初始化

Close() 关闭连接，删除连接对象, 在OnClose()和OnTimer()里面会调用。 如果应用override该方法，需要在方法里面先调用BaseConn::Close()，用来处理BaseConn里面的一些清除工作

SendPkt(CPktBase* pkt)用于BaseConn所在的IO线程发送数据包，应用需要自己删除pkt，因为pkt可能是栈上分配的数据包

int SendPkt(net_handle_t handle, CPktBase* pkt)用于非BaseConn所在的IO线程发送数据包，比如Work线程发送数据包。pkt必须是在堆上分配的数据包，由网络IO线程发送完数据包后，再进行删除，应用不能删除pkt，不然会产生多次delete而造成crash。

CloseHandle()用于其他线程关闭某个handle的连接，用handle处理多线程中的连接对象，可以避免因为多线程操作同一个对象指针而导致的crash问题。

#####4)  http_conn.h
```
void init_thread_http_conn(int io_thread_num);
```

#####5) http_handler_map.h
```
// class HttpHandlerMap
void AddHandler(const string& url, http_handler_t handler);
```
说明:
 使用HTTP时，只要在初始化时调用init_thread_http_conn()，
然后在HttpHandlerMap单实例里面通过AddHandler()方法添加处理url的
handler就可以了

#####6）simple_log.h
```
void init_simple_log(LogLevel level, string path);
log_message(level, fmt, ...)
```
说明：不解释

#####7) thread_pool.h
```
//class Task;
virtual void run() = 0;

//class ThreadPool;
int Init(uint32_t thread_num);
void AddTask(Task* pTask);
```

说明：
用Init()初始化一个固定数量的线程池
定义Task的子类，override Task的run()方法执行具体任务
然后用AddTask()方法把具体的Task扔到工作线程池执行就可以了

#### 3. Demo
src/test目录下有一个服务器，阻塞式客户端，非阻塞式客户端3个例子

#### 4. C++语言标准
[C++11](https://en.wikipedia.org/wiki/C%2B%2B11)

* 用C++11的标准库unordered_map来替换原来非标准的hash_map
* int2string()用std::to_string()替换
* atomic原子操作
* auto类型推导简化迭代器的初始化
* initializer_list
* ...

#### 5. 代码命名规范
参考:  [google c++ style guide](https://google.github.io/styleguide/cppguide.html#Variable_Names])

