“宿主”模拟程序v0.2  
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
comm_id	int	字段含义标识  
comments	string	字段含义 
  
map<int, string> dic_comments ：字段含义字典  
值	内容	描述  
0	robot_num	机器数量  
1	packets	包的数量  
2	data_lenth	数据长度  
3	speed	速度  
4	signal	信号  
5	error_rate	误码率  
  
void init_dic()：初始化字典  
int get_local_ip(const char *,char *)：根据网卡名获取本机ip地址。  
void timeToDate(struct timeval,char *)：格式化处理时间戳变量为字符串。  
void process(int,int)：处理int类型数据，将标识、时间、ip地址、进程编号、数据类型（int），值和字段含义存入结构体向量。第一个参数为待处理数据，第二个参数为数据含义标识。  
void process(float,int)：处理float类型数据，将标识、时间、ip地址、进程编号、数据类型（float），值和字段含义存入结构体向量。第一个参数为待处理数据，第二个参数为数据含义标识。  
void cinString(string&)：读取回车前的输入内容。  
bool cinInt(int&)：判断输入内容是否为数字。  
void host_show()：用于交互式查询数据，1查询所有数据，2-7分类查询结果和累加值、最大值、最小值和平均值，8清空历史数据，0退出  
  
三、host.cpp  
void process_run()：模拟用户数据线程，模拟各种类型的数据，根据参数类型和字段含义标识调用process()处理。  
  
四、编译  
$ cd host  
$ mkdir build  
$ cd build  
$ cmake ..  
$ make  
生成可执行文件 host
