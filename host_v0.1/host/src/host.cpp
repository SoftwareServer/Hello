/* user main(), need "embedded.cpp", using namespace "dataStoreAndQuery"
* API : dataSave(int/float,string,string), save data to the vector, the first param is data, the second param is attribute, the third param is note and optional, default ""
* API : host_show(), query data interactivily
* API : dataQuery(string,string), find lastest value acording to the nodeId and attribute
* APT : dataStatics(struct timeval,struct timeval,string,string), two param of time, the third param is attribute, the fourth param is method, 'average','max','min','accumulate'
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
    float err=0;
    float sig=0;
    struct timeval firstTus;
    gettimeofday(&firstTus,NULL);
    while(true){
	//API to store data
	dataStoreAndQuery::dataSave(a++,"robot num");
	dataStoreAndQuery::dataSave(rand()%100,"packets","tx and rx");
	dataStoreAndQuery::dataSave(rand()%1000,"data length");
	dataStoreAndQuery::dataSave(b++,"speed");
	dataStoreAndQuery::dataSave((float)(rand()%100+0.1),"signal");
	dataStoreAndQuery::dataSave((float)(rand()%100+0.3),"error rate","percents");
	err=dataStoreAndQuery::dataQuery("10.3.116.52","error rate");
	cout<<"err : "<<err<<endl;
	struct timeval nowTus;
	gettimeofday(&nowTus,NULL);
	if(err>20){
	    cout<<"error rate more than 20%"<<endl;
	    sig=dataStoreAndQuery::dataStatics(firstTus,nowTus,"signal","average");
	    cout<<"sig : "<<sig<<endl;
	}
	sleep(interval);
    }
}
int main(){
    boost::thread_group thrd_grp;
    int show_interval=2;
    thrd_grp.create_thread(boost::bind(*process_run,show_interval));
    //dataStoreAndQuery::host_show(); //interact in terminal
    thrd_grp.join_all();
    return 0;
}
