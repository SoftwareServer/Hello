/* user main(), need "embedded.cpp"
* API : init_dic(), int-string(key-value) to describe the defination of data
* API : process(int/float,int), save data to the vector, the first param is data, the second param is the dictionary key standing for data comments
* API : host_show(), query data interactivily
*/
#include<iostream>
#include<iomanip>
#include<boost/thread/thread.hpp>
#include<boost/bind.hpp>
#include"embedded.cpp"
using namespace std;

void process_run(int interval){  //thread for simulating user data
    int a=10;
    float b=5.6;
    //double c=7.9;
    //string str="dg952";
    while(true){
	//API to store data, according to the dictionary
	process(a++,0);                     //0: robot_num(int)
	process(rand()%100,1);              //1: packets(int)
	process(rand()%1000,2);             //2: data_lenth(int)
	process(b++,3);                     //3: speed(float)
	process((float)(rand()%100+0.1),4); //4: signal(float)
	process((float)(rand()%100+0.3),5); //5: error_rate(float)
	//process(c++);
	//process(str);
	sleep(interval);
    }
}
int main(){
    init_dic();  //init map<int,string> dic_comments, extendable, for example, 0:"robot_num", 1:"packets", 2:"data_lenth", 3:"speed", 4:"signal", 5:"error_rate"
    boost::thread_group thrd_grp;
    int show_interval=2;
    thrd_grp.create_thread(boost::bind(*process_run,show_interval));
    host_show(); //interact in terminal
    thrd_grp.join_all();
    return 0;
}
