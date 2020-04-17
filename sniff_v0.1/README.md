数据采集+分类查询实现  
一、目录结构  
sniff  
    │  CMakeLists.txt  
    │    
    └─src  
            data_query.cpp  
                sniff.cpp  
二、sniff.cpp  
	int getINT16(char *)：取得收到的占2字节的数据  
	int getINT32(char *)：取得收到的占4字节的数据  
	typedef struct data_stu{}__attribute__((packed))ORG_DATA_STU：此结构体用于解析udp接收到的通信板数据  
	struct Unicast{}：此结构体用于将解析出来的单播数据保存到用于查询方法的该类型向量中  
	struct Multicast{}：此结构体用于将解析出来的广播数据保存到用于查询方法的该类型向量中  
	struct Link{}：此结构体用于将解析出来的链路状态数据保存到用于查询方法的该类型向量中  
	void start_cmd()：向指定ip和端口发送udp指令，开启数据采集功能  
	void recv_data()：根据协议接收解析udp端口收到的通信板的单播/广播和链路状态数据，保存至用于各类查询方法的各类向量中。    
	void timeToDate(struct timeval)：将时间戳转换为“年-月-日 时：分：秒”显示  
	void unicast_show()：用于宿主代码调用查询显示实时单播数据  
	void multicast_show()：用于宿主代码调用查询显示实时广播数据  
	void link_show()：用于宿主代码调用查询显示实时链路状态信息  


三、data_query.cpp（模拟宿主代码）  
	void recv_run()：开启数据采集线程，调用sniff.cpp中的recv_data()方法  
	void unicast_run()：开启单播数据查询线程，调用sniff.cpp中的unicast_show()方法  
	void multicast_run()：开启广播播数据查询线程，调用sniff.cpp中的multicast_show()方法  
	void link_run()：开启链路状态查询线程，调用sniff.cpp中的link_show()方法  


四、编译  
$ cd sniff  
$ mkdir build  
$ cd build  
$ cmake ..  
$ make  
