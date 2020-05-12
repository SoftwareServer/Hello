/* user main(), need "embedded.cpp", using namespace "dataStoreAndQuery"
* API : dataSave(int/float,string), save data to the vector, the first param is data, the second param is data comments
* API : host_show(), query data interactivily
*/
#include<iostream>
#include<boost/thread/thread.hpp>
#include<boost/bind.hpp>
#include"embedded.cpp"
using namespace std;
//using namespace dataStoreAndQuery;
void process_run(int interval){  //thread for simulating user data
    int a=10;
    float b=5.6;
    while(true){
	//API to store data
	dataStoreAndQuery::dataSave(a++,"robot num");
	dataStoreAndQuery::dataSave(rand()%100,"packets");
	dataStoreAndQuery::dataSave(rand()%1000,"data length");
	dataStoreAndQuery::dataSave(b++,"speed");
	dataStoreAndQuery::dataSave((float)(rand()%100+0.1),"signal");
	dataStoreAndQuery::dataSave((float)(rand()%100+0.3),"error rate");
	sleep(interval);
    }
}
int main(){
    boost::thread_group thrd_grp;
    int show_interval=2;
    thrd_grp.create_thread(boost::bind(*process_run,show_interval));
    dataStoreAndQuery::host_show(); //interact in terminal
    thrd_grp.join_all();
    return 0;
}
