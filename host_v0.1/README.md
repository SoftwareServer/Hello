“宿主”模拟程序v0.3.2  
一、目录结构  
host  
    │  CMakeLists.txt  
    │  
    └─src  
            embedded.cpp  
            host.cpp  
二、embedded.cpp  
增加命名空间 dataStoreAndQuery  
struct host{}：模拟数据库  
字段	类型	描述  
id	string	标识  
nodeId	string	节点编号（ip地址）  
timetag	string	时间戳   
attribute	string	字段含义  
value	string	值  
notes	string	备注  
  
set<string> dicIp：ip地址集合（ip作为Node ID）  
set<string> dic：float类型数据类别集合  
set<string> dicInt：int类型数据类别集合  
vector<host> host_data：保存结构体host，模拟数据库  

  
int get_local_ip(const char *,char *)：根据网卡名获取本机ip地址。  
void timeToDate(struct timeval,char *)：格式化处理时间戳变量为字符串。  
void dataSave(int,string)：处理int类型数据，将标识、时间、ip地址、进程编号、数据类型（int），值和字段含义存入结构体向量。第一个参数为待处理数据，第二个参数为将加入动态字典的数据含义。  
void dataSave(float,string)：处理float类型数据，将标识、时间、ip地址、进程编号、数据类型（float），值和字段含义存入结构体向量。第一个参数为待处理数据，第二个参数为将加入动态字典的数据含义。  
float dataQuery(string,string)：根据ip地址和属性查询最近的符合条件的值  
float dataStatics(struct timeval,struct timeval,string,string)：前两个参数为时间戳，第三个参数为属性，第四个参数为统计方法（4种，平均值、最大值、最小值、累加值），查询某个时间段内某个属性的统计值  
void cinString(string&)：读取回车前的输入内容。  
bool cinInt(int&)：判断输入内容是否为数字。  
void host_show()：用于交互式查询数据，1查询所有数据，2分类查询，3清空历史数据，0退出；分类查询中，1该类数据的平均值，2该类数据的最大值，3该类数据的最小值，4该类数据的累加值  
  
三、host.cpp  
void process_run()：模拟用户数据线程，模拟各种类型的数据，根据参数类型和字段含义标识调用dataSave()处理，调用dataQuery()查询，调用dataStatics()统计。  
  
四、编译  
$ cd host  
$ mkdir build  
$ cd build  
$ cmake ..  
$ make  
生成可执行文件 host
