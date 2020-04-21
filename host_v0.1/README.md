“宿主”模拟程序  
一、目录结构  
host  
    │  CMakeLists.txt  
    │  
    └─src  
            embedded.cpp  
            host.cpp  
二、embedded.cpp  
struct host{}：模拟数据库  
字段	类型	描述  
id	string	标识  
time	string	时间戳  
ip	string	机器ip地址  
pid	string	进程编号  
type	string	数据类型  
value	string	值  
  
int get_local_ip(const char *,char *)：根据网卡名获取本机ip地址。  
void timeToDate(struct timeval,char *)：格式化处理时间戳变量为字符串。  
void process(int)：处理int类型数据，将标识、时间、ip地址、进程编号、数据类型（int）和值存入结构体向量。  
void process(float)：处理float类型数据，将标识、时间、ip地址、进程编号、数据类型（float）和值存入结构体向量。  
void process(double)：处理double类型数据，将标识、时间、ip地址、进程编号、数据类型（double）和值存入结构体向量。  
void process(string)：处理string类型数据，将标识、时间、ip地址、进程编号、数据类型（string）和值存入结构体向量。  
void show()：用于查询数据  
三、host.cpp  
void process_run()：处理数据线程，模拟各种类型的数据，根据参数类型调用process()处理。  
void host_show()：用于查询数据。  
void show_run()：数据查询线程。  
四、编译  
$ cd host  
$ mkdir build  
$ cd build  
$ cmake ..  
$ make  
生成可执行文件 host
