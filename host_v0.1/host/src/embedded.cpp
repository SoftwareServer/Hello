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
#include<set>
using namespace std;
namespace dataStoreAndQuery{

struct host{
    string id;
    string time;
    string ip;
    string pid;
    string type;
    string value;
    string comments;
};
set<string> dic;
set<string> dicInt;
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
void dataSave(int n,string comm){
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
    temp.comments=comm;
    lock_s.lock();
    dicInt.insert(comm);
    host_data.push_back(temp);    //insert to the struct
    lock_s.unlock();
}
void dataSave(float n,string comm){
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
    dic.insert(comm);
    host_data.push_back(temp);
    lock_s.unlock();
}
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
        cout<<"# 1. query all data \t\t 2. classification query "<<endl;
        cout<<"# 3. clear the history "<<endl;
        cout<<"# 0. exit "<<endl;
        cout<<"#########################################################"<<endl;
        cout<<"choose your operation:";
        if(!cinInt(sel) || sel < 0 || sel > 3){
            cout<<"invalid input, please choose in [0-3]!"<<endl;
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
			cout<<"\t"<<host_data[i].comments<<endl;
		    }
		    lock_s.unlock();
		} else {
		    cout<<"No data !"<<endl;
		}
                break;
            }
	    case 2:{
		map<int,string> queryDic;
		if(!dic.empty()){
		    cout<<"The dictionary of data(float) : "<<dic.size()<<endl;
		    set<string>::iterator it=dic.begin();
		    while(it!=dic.end()){
			cout<<*it<<endl;
			queryDic[queryDic.size()]=*it;
			it++;
		    }
		    cout<<endl;
		}
		else{
		    cout<<"The dictionary of data(float) has no member temporary"<<endl;
		}
		if(!dicInt.empty()){
		    cout<<"The dictionary of data(int) : "<<dicInt.size()<<endl;
		    set<string>::iterator it=dicInt.begin();
		    while(it!=dicInt.end()){
			cout<<*it<<endl;
			queryDic[queryDic.size()]=*it;
			it++;
		    }
		    cout<<endl;
		}
		else{
		    cout<<"The dictionary of data(int) has no member temporary"<<endl;
		}
		int query=-1;
		//cout<<"map size : "<<queryDic.size()<<endl;
		while(0!=query){
		    cout<<"========================================================="<<endl;
		    if(!queryDic.empty()){
			for(int i=0;i<queryDic.size();i++){
			    if(0==i%2){
				cout<<"* "<<(i+1)<<". query of "<<queryDic.at(i);
				if(10<=queryDic.at(i).length()){
				    cout<<"\t";
				}else{
				    cout<<"\t\t";
				}
			    }else{
				cout<<(i+1)<<". query of "<<queryDic.at(i)<<endl;
			    }
			}
		    }
		    if(1==queryDic.size()%2){
			cout<<endl;
		    }
		    cout<<"* 0. return main menu "<<endl;
		    cout<<"========================================================="<<endl;
		    cout<<"choose your operation:";
		    if(!cinInt(query) || query < 0 || query > queryDic.size()){
			cout<<"invalid input, please choose in [0-"<<queryDic.size()<<"]!"<<endl;
			continue;
		    }
		    if(query){
			cout<<"========================================================="<<endl;
			cout<<"1. mean of "<<queryDic.at(query-1)<<endl;
			cout<<"2. max of "<<queryDic.at(query-1)<<endl;
			cout<<"3. min of "<<queryDic.at(query-1)<<endl;
			cout<<"4. accumulate of "<<queryDic.at(query-1)<<endl;
			cout<<"0. return last menu "<<endl;
			cout<<"========================================================="<<endl;
			cout<<"choose your operation:";
			int classify=-1;
			if(!cinInt(classify) || classify < 0 || classify > 4){
			    cout<<"invalid input, please choose in [0-3]!"<<endl;
			    continue;
			}
			switch(classify){
			    case 1:{
				if(!host_data.empty()){
				    lock_s.lock();
				    vector<float> temp;
				    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
				    for(int i=0;i<host_data.size();i++){
					if(host_data[i].comments==queryDic.at(query-1)){
					    temp.push_back(atof(host_data[i].value.c_str()));
					    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
					    cout<<host_data[i].value.substr(0,6);
					    cout<<"\t"<<host_data[i].comments<<endl;
					}
				    }
				    if(!temp.empty()){
					float sum=accumulate(begin(temp),end(temp),0.0);
					float mean=sum/temp.size();
					cout<<"mean of all the elements : "<<mean<<endl;
				    } else {
					cout<<"No such type data !"<<endl;
				    }
				    lock_s.unlock();
				}else {
				    cout<<"No data !"<<endl;
				}
				break;
			    }
			    case 2:{
				if(!host_data.empty()){
				    lock_s.lock();
				    vector<float> temp;
				    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
				    for(int i=0;i<host_data.size();i++){
					if(host_data[i].comments==queryDic.at(query-1)){
					    temp.push_back(atof(host_data[i].value.c_str()));
					    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
					    cout<<host_data[i].value.substr(0,6);
					    cout<<"\t"<<host_data[i].comments<<endl;
					}
				    }
				    if(!temp.empty()){
					cout<<"max of all the elements : "<<*max_element(temp.begin(),temp.end())<<endl;
				    } else {
					cout<<"No such type data !"<<endl;
				    }
				    lock_s.unlock();
				}else {
				    cout<<"No data !"<<endl;
				}
				break;
			    }
			    case 3:{
				if(!host_data.empty()){
				    lock_s.lock();
				    vector<float> temp;
				    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
				    for(int i=0;i<host_data.size();i++){
					if(host_data[i].comments==queryDic.at(query-1)){
					    temp.push_back(atof(host_data[i].value.c_str()));
					    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
					    cout<<host_data[i].value.substr(0,6);
					    cout<<"\t"<<host_data[i].comments<<endl;
					}
				    }
				    if(!temp.empty()){
					cout<<"min of all the elements : "<<*min_element(temp.begin(),temp.end())<<endl;
				    } else {
					cout<<"No such type data !"<<endl;
				    }
				    lock_s.unlock();
				}else {
				    cout<<"No data !"<<endl;
				}
				break;
			    }
			    case 4:{
				if(!host_data.empty()){
				    lock_s.lock();
				    vector<float> temp;
				    cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value\t"<<"comments"<<endl;
				    for(int i=0;i<host_data.size();i++){
					if(host_data[i].comments==queryDic.at(query-1)){
					    temp.push_back(atof(host_data[i].value.c_str()));
					    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t";
					    cout<<host_data[i].value.substr(0,6);
					    cout<<"\t"<<host_data[i].comments<<endl;
					}
				    }
				    if(!temp.empty()){
					float sum=accumulate(begin(temp),end(temp),0.0);
					cout<<"accumulate all the elements : "<<sum<<endl;
				    } else {
					cout<<"No such type data !"<<endl;
				    }
				    lock_s.unlock();
				}else {
				    cout<<"No data !"<<endl;
				}
				break;
			    }
			    default: break;
			}
		    }
		}
		break;
	    }
            case 3:{
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

} /* namespace dataStoreAndQuery */
