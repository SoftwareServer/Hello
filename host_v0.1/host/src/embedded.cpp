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
#include<map>
using namespace std;
struct host{
    string id;
    string time;
    string ip;
    string pid;
    string type;
    string value;
    int comm_id;
    string comments;
};
map<int,string> dic_comments;
void init_dic(){
    dic_comments[0]="robot_num";
    dic_comments[1]="packets";
    dic_comments[2]="data_lenth";
    dic_comments[3]="speed";
    dic_comments[4]="signal";
    dic_comments[5]="error_rate";
}
vector<host> host_data;  //similar to database
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
void process(int n,int comm){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);   //network card name
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
    temp.comm_id=comm;
    temp.comments=dic_comments[comm];
    lock_s.lock();
    host_data.push_back(temp);    //insert to the struct
    lock_s.unlock();
}
void process(float n,int comm){
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
    temp.comm_id=comm;
    temp.comments=dic_comments[comm];
    lock_s.lock();
    host_data.push_back(temp);
    lock_s.unlock();
}
/*void process(double n){
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
}*/
void cinString(string& data){
    data.clear();
    cin>>data;
    std::cin.clear();
    std::cin.ignore(INT_MAX, '\n');
}

bool cinInt(int& data){  //judge the input str whether it is a digit
    std::string tmp = "";
    cinString(tmp);

    int i = 0;
    while(i < tmp.size() && tmp[i] != '\0' && isdigit(tmp[i])){
         i++;
    }

    if (i<tmp.size()) {
        cout<<"invalid input!"<<endl;
        return false;
    } else {
        data = atoi(tmp.c_str());
        return true;
    }
}
void host_show(){  //query API
    int sel = -1;
    while(sel != 0){
        cout<<"#########################################################"<<endl;
        cout<<"# 1. query all data \t\t 2. query of robot_num "<<endl;
        cout<<"# 3. query of packets \t\t 4. query of data_lenth "<<endl;
        cout<<"# 5. query of speed \t\t 6. query of signal "<<endl;
        cout<<"# 7. query of error_rate\t 8. clear the history "<<endl;
        cout<<"# 0. exit "<<endl;
        cout<<"#########################################################"<<endl;
        cout<<"choose your operation:";
        if(!cinInt(sel) || sel < 0 || sel > 8){
            cout<<"invalid input, please choose in [0-8]!"<<endl;
            continue;
        }

        switch(sel){
            case 1:{
                if(!host_data.empty()){
		    lock_s.lock();
		    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
		    for(int i=0;i<host_data.size();i++){
			cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
			cout<<host_data[i].value.substr(0,6);//printf("%.*s",6,host_data[i].value.c_str());
			cout<<"\t"<<host_data[i].comments<<"["<<host_data[i].comm_id<<"]"<<endl;
		    }
		    lock_s.unlock();
		} else {
		    cout<<"No data !"<<endl;
		}
                break;
            }
            case 2:{
                if(!host_data.empty()){
		    lock_s.lock();
		    vector<int> temp;
		    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
		    for(int i=0;i<host_data.size();i++){
			if(0==host_data[i].comm_id){
			    temp.push_back(atoi(host_data[i].value.c_str()));
			    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
			    cout<<host_data[i].value.substr(0,6);
			    cout<<"\t"<<host_data[i].comments<<"["<<host_data[i].comm_id<<"]"<<endl;
			}
		    }
		    if(!temp.empty()){
			float sum=accumulate(begin(temp),end(temp),0);
			float mean=sum/temp.size();
			cout<<"accumulate all the elements : "<<sum<<endl;
			cout<<"mean of all the elements : "<<mean<<endl;
			cout<<"max of all the elements : "<<*max_element(temp.begin(),temp.end())<<endl;
			cout<<"min of all the elements : "<<*min_element(temp.begin(),temp.end())<<endl;
		    } else {
			cout<<"No such type data !"<<endl;
		    }
		    lock_s.unlock();
		} else {
		    cout<<"No data !"<<endl;
		}
                break;
            }
            case 3:{
                if(!host_data.empty()){
		    lock_s.lock();
		    vector<int> temp;
		    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
		    for(int i=0;i<host_data.size();i++){
			if(1==host_data[i].comm_id){
			    temp.push_back(atoi(host_data[i].value.c_str()));
			    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
			    cout<<host_data[i].value.substr(0,6);
			    cout<<"\t"<<host_data[i].comments<<"["<<host_data[i].comm_id<<"]"<<endl;
			}
		    }
		    if(!temp.empty()){
			float sum=accumulate(begin(temp),end(temp),0);
			float mean=sum/temp.size();
			cout<<"accumulate all the elements : "<<sum<<endl;
			cout<<"mean of all the elements : "<<mean<<endl;
			cout<<"max of all the elements : "<<*max_element(temp.begin(),temp.end())<<endl;
			cout<<"min of all the elements : "<<*min_element(temp.begin(),temp.end())<<endl;
		    } else {
			cout<<"No such type data !"<<endl;
		    }
		    lock_s.unlock();
		} else {
		    cout<<"No data !"<<endl;
		}
                break;
            }
            case 4:{
                if(!host_data.empty()){
		    lock_s.lock();
		    vector<int> temp;
		    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
		    for(int i=0;i<host_data.size();i++){
			if(2==host_data[i].comm_id){
			    temp.push_back(atoi(host_data[i].value.c_str()));
			    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
			    cout<<host_data[i].value.substr(0,6);
			    cout<<"\t"<<host_data[i].comments<<"["<<host_data[i].comm_id<<"]"<<endl;
			}
		    }
		    if(!temp.empty()){
			float sum=accumulate(begin(temp),end(temp),0);
			float mean=sum/temp.size();
			cout<<"accumulate all the elements : "<<sum<<endl;
			cout<<"mean of all the elements : "<<mean<<endl;
			cout<<"max of all the elements : "<<*max_element(temp.begin(),temp.end())<<endl;
			cout<<"min of all the elements : "<<*min_element(temp.begin(),temp.end())<<endl;
		    } else {
			cout<<"No such type data !"<<endl;
		    }
		    lock_s.unlock();
		} else {
		    cout<<"No data !"<<endl;
		}
                break;
            }
            case 5:{
                if(!host_data.empty()){
		    lock_s.lock();
		    vector<float> temp;
		    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
		    for(int i=0;i<host_data.size();i++){
			if(3==host_data[i].comm_id){
			    temp.push_back(atof(host_data[i].value.c_str()));
			    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
			    cout<<host_data[i].value.substr(0,6);
			    cout<<"\t"<<host_data[i].comments<<"["<<host_data[i].comm_id<<"]"<<endl;
			}
		    }
		    if(!temp.empty()){
			float sum=accumulate(begin(temp),end(temp),0.0);
			float mean=sum/temp.size();
			cout<<"accumulate all the elements : "<<sum<<endl;
			cout<<"mean of all the elements : "<<mean<<endl;
			cout<<"max of all the elements : "<<*max_element(temp.begin(),temp.end())<<endl;
			cout<<"min of all the elements : "<<*min_element(temp.begin(),temp.end())<<endl;
		    } else {
			cout<<"No such type data !"<<endl;
		    }
		    lock_s.unlock();
		} else {
		    cout<<"No data !"<<endl;
		}
                break;
            }
            case 6:{
                if(!host_data.empty()){
		    lock_s.lock();
		    vector<float> temp;
		    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
		    for(int i=0;i<host_data.size();i++){
			if(4==host_data[i].comm_id){
			    temp.push_back(atof(host_data[i].value.c_str()));
			    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
			    cout<<host_data[i].value.substr(0,6);
			    cout<<"\t"<<host_data[i].comments<<"["<<host_data[i].comm_id<<"]"<<endl;
			}
		    }
		    if(!temp.empty()){
			float sum=accumulate(begin(temp),end(temp),0.0);
			float mean=sum/temp.size();
			cout<<"accumulate all the elements : "<<sum<<endl;
			cout<<"mean of all the elements : "<<mean<<endl;
			cout<<"max of all the elements : "<<*max_element(temp.begin(),temp.end())<<endl;
			cout<<"min of all the elements : "<<*min_element(temp.begin(),temp.end())<<endl;
		    } else {
			cout<<"No such type data !"<<endl;
		    }
		    lock_s.unlock();
		} else {
		    cout<<"No data !"<<endl;
		}
                break;
            }
            case 7:{
                if(!host_data.empty()){
		    lock_s.lock();
		    vector<float> temp;
		    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
		    for(int i=0;i<host_data.size();i++){
			if(5==host_data[i].comm_id){
			    temp.push_back(atof(host_data[i].value.c_str()));
			    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
			    cout<<host_data[i].value.substr(0,6);
			    cout<<"\t"<<host_data[i].comments<<"["<<host_data[i].comm_id<<"]"<<endl;
			}
		    }
		    if(!temp.empty()){
			float sum=accumulate(begin(temp),end(temp),0.0);
			float mean=sum/temp.size();
			cout<<"accumulate all the elements : "<<sum<<endl;
			cout<<"mean of all the elements : "<<mean<<endl;
			cout<<"max of all the elements : "<<*max_element(temp.begin(),temp.end())<<endl;
			cout<<"min of all the elements : "<<*min_element(temp.begin(),temp.end())<<endl;
		    } else {
			cout<<"No such type data !"<<endl;
		    }
		    lock_s.unlock();
		} else {
		    cout<<"No data !"<<endl;
		}
                break;
            }
            case 8:{
                lock_s.lock();
		host_data.clear();
                lock_s.unlock();
                break;
            }
            default: break;
        }
        sleep(1);
    }
    exit(0);
}
