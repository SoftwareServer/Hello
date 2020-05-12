#include"table.h"
#include"get_ip.cpp"

Table::Table(QWidget *parent): QTableView(parent){
    //init model and header
    model=new QStandardItemModel(this);
    model->setHorizontalHeaderItem( 0, new QStandardItem( "id" ) );
    model->setHorizontalHeaderItem( 1, new QStandardItem( "time" ) );
    model->setHorizontalHeaderItem( 2, new QStandardItem( "ip" ) );
    model->setHorizontalHeaderItem( 3, new QStandardItem( "pid" ) );
    model->setHorizontalHeaderItem( 4, new QStandardItem( "type" ) );
    model->setHorizontalHeaderItem( 5, new QStandardItem( "value" ) );
    model->setHorizontalHeaderItem( 6, new QStandardItem( "comments" ) );

    connect(&timer, SIGNAL(timeout()), this, SLOT(result()));
    timer.start(2 * 1000);
    a=0;
    b=1.1;
    c=2.2;
    d="dg952";
    this->setModel(model);
}
Table::~Table(){
    this->close();
    if(NULL!=model){
	delete model;
	model=NULL;
    }
}
void Table::update(){
    dataSave(++a,"num");
    dataSave(++b,"speed");
    dataSave(++c,"high");
    dataSave(d,"note");
}
void Table::result(){
    if(!host_data.empty()){
	lock_s.lock();
	for(int i=0;i<host_data.size();i++){
	    model->setItem(i,0,new QStandardItem(host_data[i].id.c_str()));
	    model->setItem(i,1,new QStandardItem(host_data[i].time.c_str()));
	    model->setItem(i,2,new QStandardItem(host_data[i].ip.c_str()));
	    model->setItem(i,3,new QStandardItem(host_data[i].pid.c_str()));
	    model->setItem(i,4,new QStandardItem(host_data[i].type.c_str()));
	    model->setItem(i,5,new QStandardItem(host_data[i].value.c_str()));
	    model->setItem(i,6,new QStandardItem(host_data[i].comments.c_str()));
	}
	lock_s.unlock();
    }
    this->setModel(model);
}
void Table::timeToDate(struct timeval tv,char *time){
    struct tm tv_temp;
    localtime_r(&tv.tv_sec,&tv_temp);
    snprintf(time,25,"%04d-%02d-%02d %02d:%02d:%02d.%03ld",tv_temp.tm_year+1900,tv_temp.tm_mon+1,tv_temp.tm_mday,tv_temp.tm_hour,tv_temp.tm_min,tv_temp.tm_sec,tv.tv_usec/1000);
}
void Table::dataSave(int n,std::string comm){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);
    struct timeval nowTus;
    gettimeofday(&nowTus,NULL);
    char time[25]="";
    timeToDate(nowTus,time);
    host temp;
    temp.id=to_string(host_data.size()+1);
    temp.time=time;
    temp.ip=ip_num;
    temp.pid=to_string(getpid());
    temp.type="int";
    temp.value=to_string(n);
    temp.comments=comm;
    lock_s.lock();
    host_data.push_back(temp);       //save data to "database", vector<host>
    lock_s.unlock();
}
void Table::dataSave(float n,std::string comm){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);
    struct timeval nowTus;
    gettimeofday(&nowTus,NULL);
    char time[25]="";
    timeToDate(nowTus,time);
    host temp;
    temp.id=to_string(host_data.size()+1);
    temp.time=time;
    temp.ip=ip_num;
    temp.pid=to_string(getpid());
    temp.type="float";
    temp.value=to_string(n);
    temp.comments=comm;
    lock_s.lock();
    host_data.push_back(temp);
    lock_s.unlock();
}
void Table::dataSave(double n,std::string comm){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);
    struct timeval nowTus;
    gettimeofday(&nowTus,NULL);
    char time[25]="";
    timeToDate(nowTus,time);
    host temp;
    temp.id=to_string(host_data.size()+1);
    temp.time=time;
    temp.ip=ip_num;
    temp.pid=to_string(getpid());
    temp.type="double";
    temp.value=to_string(n);
    temp.comments=comm;
    lock_s.lock();
    host_data.push_back(temp);
    lock_s.unlock();
}
void Table::dataSave(string str,std::string comm){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);
    struct timeval nowTus;
    gettimeofday(&nowTus,NULL);
    char time[25]="";
    timeToDate(nowTus,time);
    host temp;
    temp.id=to_string(host_data.size()+1);
    temp.time=time;
    temp.ip=ip_num;
    temp.pid=to_string(getpid());
    temp.type="string";
    temp.value=str;
    temp.comments=comm;
    lock_s.lock();
    host_data.push_back(temp);
    lock_s.unlock();
}
void Table::getData(int interval){
    connect(&timerUpdate, SIGNAL(timeout()), this, SLOT(update()));
    timerUpdate.start(interval * 1000);
}
