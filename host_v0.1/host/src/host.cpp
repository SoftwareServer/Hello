#include<iostream>
#include<iomanip>
#include<boost/thread/thread.hpp>
#include<boost/bind.hpp>
#include"embedded.cpp"
using namespace std;
void process_run(int interval){
    int a=10;
    float b=5.6;
    double c=7.9;
    string str="dg952";
    while(true){
	process(a++);
	process(b++);
	process(c++);
	process(str);
	sleep(interval);
    }
}
void host_show(){
    if(!host_data.empty()){
	lock_s.lock();
	cout<<"No\t"<<"time"<<setw(13)<<"\t"<<"ip"<<setw(7)<<"\t"<<"pid\t"<<"type\t"<<"value"<<endl;
	for(int i=0;i<host_data.size();i++){
	    cout<<host_data[i].id<<"\t"<<host_data[i].time<<"\t"<<host_data[i].ip<<"\t"<<host_data[i].pid<<"\t"<<host_data[i].type<<"\t"<<host_data[i].value<<endl;
	}
	lock_s.unlock();
    }
}
void show_run(int interval){
    while(true){
	host_show();
	cout<<endl;
	sleep(interval);
    }
}
int main(){
    boost::thread_group thrd_grp;
    int show_interval=0.02;
    thrd_grp.create_thread(boost::bind(*process_run,show_interval));
    thrd_grp.create_thread(boost::bind(*show_run,show_interval));
    thrd_grp.join_all();
    return 0;
}
