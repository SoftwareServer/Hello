#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<iomanip>
#include<arpa/inet.h>
#include<string.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<errno.h>
#include<net/if.h>
#include<mutex>
#include<vector>
using namespace std;
struct host{
    string id;
    string time;
    string ip;
    string pid;
    string type;
    string value;
};
vector<host> host_data;
mutex lock_s;
int get_local_ip(const char *eth_inf,char *ip){
    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd){
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    // if error: No such device
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0){
        printf("ioctl error: %s\n", strerror(errno));
        close(sd);
        return -1;
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    snprintf(ip, 16, "%s", inet_ntoa(sin.sin_addr));

    close(sd);
    return 0;
}
void timeToDate(struct timeval tv,char *time){
    struct tm tv_temp;
    localtime_r(&tv.tv_sec,&tv_temp);
    snprintf(time,25,"%04d-%02d-%02d %02d:%02d:%02d.%03ld",tv_temp.tm_year+1900,tv_temp.tm_mon+1,tv_temp.tm_mday,tv_temp.tm_hour,tv_temp.tm_min,tv_temp.tm_sec,tv.tv_usec/1000);
}
void process(int n){
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
    lock_s.lock();
    host_data.push_back(temp);
    lock_s.unlock();
}
void process(float n){
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
    lock_s.lock();
    host_data.push_back(temp);
    lock_s.unlock();
}
void process(double n){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);
    struct timeval nowTus;
    gettimeofday(&nowTus,NULL);
    //timeToDate(nowTus);
    char time[25]="";
    timeToDate(nowTus,time);
    host temp;
    temp.id=to_string(host_data.size()+1);
    temp.time=time;
    temp.ip=ip_num;
    temp.pid=to_string(getpid());
    temp.type="double";
    temp.value=to_string(n);
    lock_s.lock();
    host_data.push_back(temp);
    lock_s.unlock();
}
void process(string str){
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
    lock_s.lock();
    host_data.push_back(temp);
    lock_s.unlock();
}
void show(){
    if(!host_data.empty()){
	lock_s.lock();
	cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value"<<endl;
	for(int i=0;i<host_data.size();i++){
	    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t"<<host_data[i].value<<endl;
	}
	lock_s.unlock();
    }
}
