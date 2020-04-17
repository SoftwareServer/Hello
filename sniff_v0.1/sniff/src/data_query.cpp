#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<vector>
#include<pthread.h>
#include<thread>
#include<boost/thread/thread.hpp>
#include<boost/bind.hpp>
#include"sniff.cpp"

//#define UNICAST
//#define MULTICAST
#define LINK

using namespace std;
void recv_run(){
    recv_data();
}
void unicast_run(int interval){
    while(true){
	unicast_show();
	sleep(interval);
    }
}
void multicast_run(int interval){
    while(true){
	multicast_show();
	sleep(interval);
    }
}
void link_run(int interval){
    while(true){
	link_show();
	sleep(interval);
    }
}
int main(){
    boost::thread_group thrd_grp;
    start_cmd();
    int show_interval=2;
    thrd_grp.create_thread(boost::bind(*recv_run)); //recv board's data, the premise of query data
#ifdef UNICAST
    thrd_grp.create_thread(boost::bind(*unicast_run,show_interval));
#endif
#ifdef MULTICAST
    thrd_grp.create_thread(boost::bind(*multicast_run,show_interval));
#endif
#ifdef LINK
    thrd_grp.create_thread(boost::bind(*link_run,show_interval));
#endif
    thrd_grp.join_all();
    return 0;
}
