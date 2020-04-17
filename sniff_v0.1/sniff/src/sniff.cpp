#include<stdio.h>
#include<stdlib.h> // for exit()
#include<string.h> //for memset
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h> // for inet_ntoa()
#include<errno.h>
#include<unistd.h>
#include<iostream>
#include<iomanip> //for setprecision()
#include<time.h>
#include<mutex>
#include<vector>
#include<map>
#define RECVBUFF 1000

//#define _PRINTF_
#ifdef _PRINTF_
#define PRINTF(format, ...) printf(format,##__VA_ARGS__)
#else
#define PRINTF(format, ...) ((void)0)
#endif

using namespace std;
int getINT16(char *dataptr)
{
    int tmp = 0;
    tmp = (int)((int)(dataptr[0] <<  8) & 0xFF00);
    tmp |= (int)((int) dataptr[1] & 0xFF);
    return tmp;	
}
int getINT32(char *dataptr)
{
    int tmp = 0;
    tmp  = (int)((int)(dataptr[0] << 24) & 0xFF000000);
    tmp |= (int)((int)(dataptr[1] << 16) & 0xFF0000);
    tmp |= (int)((int)(dataptr[2] <<  8) & 0xFF00);
    tmp |= (int)((int) dataptr[3] & 0xFF);
    return tmp;	
}
typedef struct data_stu{
    unsigned char peerID[6];
    struct timeval ts;
    unsigned int tx_packets;
    unsigned int tx_bytes;
    unsigned int tx_fail;
    unsigned int tx_retries;
    signed int tx_bitrate;
    signed char signal;

    unsigned char bc_peerID[6];
    unsigned int bc_packets;
    unsigned int mc_packets;
    unsigned int nb_packets;

    unsigned char state_peerID[6];
    unsigned int state;
}__attribute__((packed))ORG_DATA_STU;
struct Unicast{
    string str_dgnum;
    unsigned char peerID[6];
    string str_dgnum_oppoend;
    struct timeval ts;
    unsigned int tx_packets;
    unsigned int tx_bytes;
    unsigned int tx_fail;
    unsigned int tx_retries;
    signed int tx_bitrate;
    signed char signal;
};
struct Multicast{
    string str_dgnum;
    unsigned char bc_peerID[6];
    unsigned int bc_packets;
    unsigned int mc_packets;
    unsigned int nb_packets;
};
struct Link{
    string str_dgnum_link;
    unsigned int state_link;
};
mutex lock_s;
vector<Unicast> unicast_data;
vector<Multicast> multicast_data;
vector<Link> link_state;
vector<vector<Unicast> > unicast_data_show;
vector<vector<Multicast> > multicast_data_show;
vector<vector<Link> > link_state_show;
void start_cmd(){
    char msg[128];
    //01 00 17 77 6C 61 6E 31 5F 6D 77 64 73 00 00 00 00 00 00 00 00 00 00 00 00 01
    msg[0] = 0x01; 
    msg[1] = 0x00;
    msg[2] = 0x17;
    msg[3] = 0x77;
    msg[4] = 0x6C;
    msg[5] = 0x61;
    msg[6] = 0x6E;
    msg[7] = 0x31;
    msg[8] = 0x5F;
    msg[9] = 0x6D;
    msg[10] = 0x77;
    msg[11] = 0x64;
    msg[12] = 0x73;
    msg[13] = 0x00;
    msg[14] = 0x00;
    msg[15] = 0x00;
    msg[16] = 0x00;
    msg[17] = 0x00;
    msg[18] = 0x00;
    msg[19] = 0x00;
    msg[20] = 0x00;
    msg[21] = 0x00;
    msg[22] = 0x00;
    msg[23] = 0x00;  //0x01  stop
    msg[24] = 0x00;
    msg[25] = 0x01;
    int brdcFd;
    if((brdcFd = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
	PRINTF("socket fail\n");
	exit(-1);//return -1;
    }

    struct sockaddr_in local;
    memset(&local, 0, sizeof(struct sockaddr_in));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(10008);//UDP 广播包 本地端口
    socklen_t local_len = sizeof(struct sockaddr);
    if(bind(brdcFd, (struct sockaddr *)&local, sizeof(local)))//绑定端口
    {
        PRINTF("####L(%d) client bind port failed!\n", __LINE__);
        close(brdcFd);//关闭socket
        exit(-1);
    }

    int optval = 1;//这个值一定要设置，否则可能导致sendto()失败
    setsockopt(brdcFd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &optval, sizeof(int));
    struct sockaddr_in theirAddr;
    memset(&theirAddr, 0, sizeof(struct sockaddr_in));
    theirAddr.sin_family = AF_INET;
    theirAddr.sin_addr.s_addr = inet_addr("10.0.112.94"); //网线直连通信板ip
    theirAddr.sin_port = htons(10008);
    int sendBytes;
    if((sendBytes = sendto(brdcFd, msg, 26, 0,
	(struct sockaddr *)&theirAddr, sizeof(struct sockaddr))) == -1){
	PRINTF("sendto fail, errno=%d\n", errno);
	exit(-1);//return -1;
    }
    PRINTF("msg=%s, sendBytes=%d\n", msg, sendBytes);
    close(brdcFd);
}
void recv_data(){
    map<string, string> dic_unicast;
    map<string, string>::iterator it_unicast;
    dic_unicast["\x16\x16\x03\x74\x84"]="dg915";
    dic_unicast["\x16\x16\x03\x75\x34"]="dg916";
    dic_unicast["\x16\x16\x03\x75\x64"]="dg918";
    dic_unicast["\x16\x16\x03\x75\x54"]="dg921";
    dic_unicast["\x16\x16\x03\x75\x74"]="dg922";
    dic_unicast["\x16\x16\x03\x74\x54"]="dg926";
    dic_unicast["\x16\x16\x03\x74\xc4"]="dg927";
    dic_unicast["\x16\x16\x03\x74\xa4"]="dg928";
    dic_unicast["\x16\x16\x03\x74\x94"]="dg930";
    dic_unicast["\x16\x16\x03\x75\x94"]="dg932";
    dic_unicast["\x16\x16\x03\x74\xe4"]="dg933";
    dic_unicast["\x16\x16\x03\x6c\xf4"]="dg936";
    dic_unicast["\x16\x16\x03\x6c\xc4"]="dg937";
    dic_unicast["\x16\x16\x03\x6d\x34"]="dg938";
    dic_unicast["\x16\x16\x03\x75\x24"]="dg939";
    dic_unicast["\x16\x16\x03\x75\x14"]="dg940";
    dic_unicast["\x16\x16\x03\x6d\x54"]="dg942";
    dic_unicast["\x16\x16\x03\x74\xf4"]="dg943";
    dic_unicast["\x16\x16\x03\x74\xb4"]="dg945";
    dic_unicast["\x16\x16\x03\x74\x74"]="dg946";
    dic_unicast["\x16\x16\x03\x6d\x24"]="dg947";
    dic_unicast["\x16\x16\x03\x75\xc4"]="dg948";
    dic_unicast["\x8e\xdf\xee\x4c\x3b"]="dg950";
    dic_unicast["\x16\x16\x03\x74\x34"]="dg951";
    dic_unicast["\x8e\xdf\xee\x70\x5e"]="dg952";
    dic_unicast["\x8e\xdf\xee\x70\x2b"]="dg953";
    dic_unicast["\x16\x16\x23\xee\x14"]="dg955";
    dic_unicast["\x16\x16\x03\x6c\xd4"]="dg965";
    dic_unicast["\x8e\xdf\xee\x70\x78"]="dg967";
    dic_unicast["\x16\x16\x03\x6c\xe4"]="dg968";
    dic_unicast["\x16\x16\x03\x75\x44"]="dg969";
    dic_unicast["\x16\x16\x03\x74\x44"]="dg970";
    dic_unicast["\x16\x16\x03\x57\x84"]="g03";
    dic_unicast["\x16\x16\x03\x57\xd4"]="g04";cout<<dic_unicast.size()<<endl;

    map<string, string> dic_multicast;
    map<string, string>::iterator it_multicast;
    dic_multicast["\x04\x4b\xdd\xc8\xaa"]="dg915";
    dic_multicast["\x04\x4b\xdd\xca\xe1"]="dg916";
    dic_multicast["\x04\x4b\xdd\x9e\x89"]="dg918";
    //dic_multicast["\x04\x4b\xdd\x9d\x3e"]="dg921";
    dic_multicast["\x04\x4b\xdd\x9b\x3e"]="dg922";
    dic_multicast["\x04\x4b\xdd\xcb\x02"]="dg926";
    dic_multicast["\x04\x4b\xdd\xca\x7e"]="dg927";
    dic_multicast["\x04\x4b\xdd\xca\x7b"]="dg928";
    dic_multicast["\x04\x4b\xdd\xca\x12"]="dg930";
    dic_multicast["\x04\x4b\xdd\x82\x81"]="dg932";
    dic_multicast["\x04\x4b\xdd\xca\xe7"]="dg933";
    dic_multicast["\x04\x4b\xdd\xc9\x13"]="dg936";
    dic_multicast["\x04\x4b\xdd\x9e\xdd"]="dg937";
    dic_multicast["\x04\x4b\xdd\xca\x39"]="dg938";
    dic_multicast["\x04\x4b\xdd\xca\x81"]="dg939";
    dic_multicast["\x04\x4b\xdd\xca\xf9"]="dg940";
    dic_multicast["\x04\x4b\xdd\xca\x24"]="dg942";
    //dic_multicast["\x04\x4b\xdd\x74\xf4"]="dg943";
    dic_multicast["\x04\x4b\xdd\xca\x27"]="dg945";
    dic_multicast["\x04\x4b\xdd\xca\x0f"]="dg946";
    dic_multicast["\x04\x4b\xdd\x82\x7b"]="dg947";
    dic_multicast["\x04\x4b\xdd\xc8\x8c"]="dg948";
    dic_multicast["\x04\x4b\xdd\xca\x36"]="dg950";
    dic_multicast["\x04\x4b\xde\x59\x4a"]="dg951";
    dic_multicast["\x04\x4b\xdd\xca\x4e"]="dg952";
    dic_multicast["\x04\x4b\xdd\xc8\xb3"]="dg953";
    dic_multicast["\x04\x4b\xdd\xca\xf3"]="dg955";
    dic_multicast["\x04\x4b\xde\xce\xfb"]="dg965";
    dic_multicast["\x04\x4b\xdf\x4f\x1f"]="dg967";
    dic_multicast["\x04\x4b\xdf\x90\x65"]="dg968";
    dic_multicast["\x04\x4b\xdf\x8b\xa0"]="dg969";
    dic_multicast["\x04\x4b\xde\xf2\x39"]="dg970";
    dic_multicast["\xee\xc0\xa8\x01\xfe"]="P0";
    dic_multicast["\xf2\xd2\x8b\x06\x66"]="g03";
    dic_multicast["\xf1\xd2\xbf\x06\x66"]="g04";
    dic_multicast["\x02\x89\x13\x57\xf2"]="gw201";
    //dic_multicast["\xf2\xd2\x8b\x06\x66"]="g03";cout<<dic_multicast.size()<<endl;

    map<string, string> dic;
    map<string, string>::iterator it;
    dic["\x16\x16\x03\x74\x8e"]="dg915";
    dic["\x16\x16\x03\x75\x3e"]="dg916";
    dic["\x16\x16\x03\x75\x6e"]="dg918";
    dic["\x16\x16\x03\x75\x5e"]="dg921";
    dic["\x16\x16\x03\x75\x7e"]="dg922";
    dic["\x16\x16\x03\x74\x5e"]="dg926";
    dic["\x16\x16\x03\x74\xce"]="dg927";
    dic["\x16\x16\x03\x74\xae"]="dg928";
    dic["\x16\x16\x03\x74\x9e"]="dg930";
    dic["\x16\x16\x03\x75\x9e"]="dg932";
    dic["\x16\x16\x03\x74\xee"]="dg933";
    dic["\x16\x16\x03\x6c\xfe"]="dg936";
    dic["\x16\x16\x03\x6c\xce"]="dg937";
    dic["\x16\x16\x03\x6d\x3e"]="dg938";
    dic["\x16\x16\x03\x75\x2e"]="dg939";
    dic["\x16\x16\x03\x75\x1e"]="dg940";
    dic["\x16\x16\x03\x6d\x5e"]="dg942";
    dic["\x16\x16\x03\x74\xfe"]="dg943";
    dic["\x16\x16\x03\x74\xbe"]="dg945";
    dic["\x16\x16\x03\x74\x7e"]="dg946";
    dic["\x16\x16\x03\x6d\x2e"]="dg947";
    dic["\x16\x16\x03\x75\xce"]="dg948";
    dic["\x8e\xdf\xee\x4c\x3e"]="dg950";
    dic["\x16\x16\x03\x74\x3e"]="dg951";
    dic["\x8e\xdf\xee\x70\x5e"]="dg952";
    dic["\x8e\xdf\xee\x70\x2e"]="dg953";
    dic["\x16\x16\x23\xee\x1e"]="dg955";
    dic["\x16\x16\x03\x6c\xde"]="dg965";
    dic["\x8e\xdf\xee\x70\x7e"]="dg967";
    dic["\x16\x16\x03\x6c\xee"]="dg968";
    dic["\x16\x16\x03\x75\x4e"]="dg969";
    dic["\x16\x16\x03\x74\x4e"]="dg970";
    dic["\x16\x16\x03\x57\x8e"]="g03";
    dic["\x16\x16\x03\x57\xde"]="g04";
    cout<<"The number of other Channel boards in the dictionary are : "<<dic.size()<<endl;
    while(true){

	int sockListen;
	if((sockListen = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
	    PRINTF("socket fail\n");
	    exit(-1);
	}
	int set = 1;
	setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
	struct sockaddr_in recvAddr;
	memset(&recvAddr, 0, sizeof(struct sockaddr_in));
	recvAddr.sin_family = AF_INET;
	recvAddr.sin_addr.s_addr = INADDR_ANY;//inet_addr("10.3.116.238"); //网线直连通信板ip
	recvAddr.sin_port = htons(10008);
	if(bind(sockListen, (struct sockaddr *)&recvAddr, sizeof(struct sockaddr))==-1){
	    PRINTF("bind port failed!\n");
	    close(sockListen);//关闭socket
	    exit(-1);
	}
	int recvBytes;
	char recvbuf[RECVBUFF];
	int addrLen=sizeof(struct sockaddr_in);

	if((recvBytes = recvfrom(sockListen, recvbuf, 64000, 0,
		(struct sockaddr *)&recvAddr, (socklen_t*)&addrLen)) != -1){
	    recvbuf[recvBytes]='\0';
	    int len=recvBytes;//sizeof(recvbuf);
	    PRINTF("length of received msg : %d\n",len);
	    if(len>RECVBUFF){
		PRINTF("Warning : length of received msg is bigger than defined receiving buff size '%d', please modify the buff size to comfortable the programm !\n",RECVBUFF);
	    }
	    std::vector<char> msg_data(recvbuf,recvbuf+recvBytes);//std::cout<<"msg_data : "<<msg_data.size()<<" "<<sizeof(recvbuf)<<std::endl;
	    if(0x02==msg_data[0]){
		PRINTF("Received data from the Communication board successfully\n");
	    }
	    for(int i=0;i<10;i++){
		PRINTF("%02x ",msg_data[i]);
	    }
	    PRINTF("\n");
	    //TODO 0x04 mac address to dg_number
	    if(0x03==msg_data[0]){
		ORG_DATA_STU stu;
		PRINTF("\n\n\nsize = %lu\n\n\n",msg_data.size());
		unsigned int len;
		unsigned char mac[6];
		unsigned int tx_num;
		len=getINT16(&msg_data[1/*43*/]);PRINTF("%02x len = %d\n",len,len);
		mac[0]=msg_data[3/*45*/];PRINTF("mac : %02x ",mac[0]);
		mac[1]=msg_data[4/*46*/];PRINTF("%02x ",mac[1]);
		mac[2]=msg_data[5/*47*/];PRINTF("%02x ",mac[2]);
		mac[3]=msg_data[6/*48*/];PRINTF("%02x ",mac[3]);
		mac[4]=msg_data[7/*49*/];PRINTF("%02x ",mac[4]);
		mac[5]=msg_data[8/*50*/];PRINTF("%02x\n",mac[5]);PRINTF("ip = 10.%d.%d.%d\n",mac[3],mac[4],mac[5]);
		tx_num=getINT16(&msg_data[9/*51*/]);PRINTF("%02x tx_num = %d\n",tx_num,tx_num);
		int count=0;
		unsigned char maclist[5];
		for(int i=45/*87*/;i<msg_data.size();i+=35){
		    stu.peerID[0]=msg_data[i-34];PRINTF("another mac : %02x ",stu.peerID[0]);
		    stu.peerID[1]=msg_data[i-33];PRINTF("%02x ",stu.peerID[1]);
		    stu.peerID[2]=msg_data[i-32];PRINTF("%02x ",stu.peerID[2]);
		    stu.peerID[3]=msg_data[i-31];PRINTF("%02x ",stu.peerID[3]);
		    stu.peerID[4]=msg_data[i-30];PRINTF("%02x ",stu.peerID[4]);
		    stu.peerID[5]=msg_data[i-29];PRINTF("%02x\n",stu.peerID[5]);
		    PRINTF("ip = 10.%d.%d.%d\n",stu.peerID[3],stu.peerID[4],stu.peerID[5]);
		    stu.ts.tv_sec = getINT32(&msg_data[i-28]);
		    stu.ts.tv_usec = getINT32(&msg_data[i-24]);PRINTF("time = %ld.%ld\n",stu.ts.tv_sec,stu.ts.tv_usec);
		    stu.tx_packets = getINT32(&msg_data[i-20]);PRINTF("%02x %02x %02x %02x   %02x tx_packets = %d\n",msg_data[i-20],msg_data[i-19],msg_data[i-18],msg_data[i-17],stu.tx_packets,stu.tx_packets);
		    stu.tx_bytes = getINT32(&msg_data[i-16]);PRINTF("%02x %02x %02x %02x   %02x tx_bytes = %d\n",msg_data[i-16],msg_data[i-15],msg_data[i-14],msg_data[i-13],stu.tx_bytes,stu.tx_bytes);
		    stu.tx_fail = getINT32(&msg_data[i-12]);PRINTF("%02x %02x %02x %02x   %02x tx_fail = %d\n",msg_data[i-12],msg_data[i-11],msg_data[i-10],msg_data[i-9],stu.tx_fail,stu.tx_fail);
		    stu.tx_retries = getINT32(&msg_data[i-8]);PRINTF("%02x %02x %02x %02x   %02x tx_retries = %d\n",msg_data[i-8],msg_data[i-7],msg_data[i-6],msg_data[i-5],stu.tx_retries,stu.tx_retries);
		    stu.tx_bitrate = getINT32(&msg_data[i-4]);PRINTF("%02x %02x %02x %02x   %02x tx_bitrate = %d\n",msg_data[i-4],msg_data[i-3],msg_data[i-2],msg_data[i-1],stu.tx_bitrate,stu.tx_bitrate);
		    stu.signal = msg_data[i];PRINTF("%02x signal = %d\n",stu.signal,stu.signal);
		    Unicast unicast_data_temp;
		    maclist[0]=stu.peerID[1];
		    maclist[1]=stu.peerID[2];
		    maclist[2]=stu.peerID[3];
		    maclist[3]=stu.peerID[4];
		    maclist[4]=stu.peerID[5];
		    for(int i=0;i<5;i++){
			if(0x00==maclist[i]){
			     maclist[i]=0xee;
			}
		    }
		    string str_temp((char*)maclist);
		    it_unicast = dic_unicast.find(str_temp.substr(0,5));
		    if(it_unicast!=dic_unicast.end()){
			//cout<<it_unicast->second<<" ";
			//dic.erase(it);
		    }else{
			cout<<"Don't find ! (unicast)"<<endl;
		    }
		    //cout<<dic_unicast[str_temp.substr(0,5)]<<endl;
		    unicast_data_temp.str_dgnum=dic_unicast[str_temp.substr(0,5)];
		    unicast_data_temp.ts=stu.ts;
		    unicast_data_temp.tx_packets=stu.tx_packets;
		    unicast_data_temp.tx_bytes=stu.tx_bytes;
		    unicast_data_temp.tx_fail=stu.tx_fail;
		    unicast_data_temp.tx_retries=stu.tx_retries;
		    unicast_data_temp.tx_bitrate=stu.tx_bitrate;
		    unicast_data_temp.signal=stu.signal;
		    lock_s.lock();
		    unicast_data.push_back(unicast_data_temp);
		    lock_s.unlock();
		}
		lock_s.lock();
		unicast_data_show.push_back(unicast_data);
		unicast_data.clear();
		lock_s.unlock();
	    }
	    else if(0x04==msg_data[0]){
		ORG_DATA_STU stu_bc;
		PRINTF("\n\n\nsize = %lu\n\n\n",msg_data.size());
		unsigned int len;
		unsigned int node_num;
		len=getINT16(&msg_data[1/*43*/]);PRINTF("%02x len = %d\n",len,len);
		node_num=msg_data[3/*45*/];PRINTF("%02x node_num = %d\n",node_num,node_num);
		int bc_count=0;
		unsigned char maclist[5];
		for(int i=21/*63*/;i<msg_data.size();i+=18){
		    //fprintf(logfile,"No : %d\n",++bc_count);
		    stu_bc.bc_peerID[0]=msg_data[i-17];PRINTF("node mac : %02x ",stu_bc.bc_peerID[0]);
		    stu_bc.bc_peerID[1]=msg_data[i-16];PRINTF("%02x ",stu_bc.bc_peerID[1]);
		    stu_bc.bc_peerID[2]=msg_data[i-15];PRINTF("%02x ",stu_bc.bc_peerID[2]);
		    stu_bc.bc_peerID[3]=msg_data[i-14];PRINTF("%02x ",stu_bc.bc_peerID[3]);
		    stu_bc.bc_peerID[4]=msg_data[i-13];PRINTF("%02x ",stu_bc.bc_peerID[4]);
		    stu_bc.bc_peerID[5]=msg_data[i-12];PRINTF("%02x\n",stu_bc.bc_peerID[5]);
		    stu_bc.bc_packets = getINT32(&msg_data[i-11]);PRINTF("%02x %02x %02x %02x   %02x broadcast packets num = %d\n",msg_data[i-11],msg_data[i-10],msg_data[i-9],msg_data[i-8],stu_bc.bc_packets,stu_bc.bc_packets);
		    stu_bc.mc_packets = getINT32(&msg_data[i-7]);PRINTF("%02x %02x %02x %02x   %02x multicast packets num = %d\n",msg_data[i-7],msg_data[i-6],msg_data[i-5],msg_data[i-4],stu_bc.mc_packets,stu_bc.mc_packets);
		    stu_bc.nb_packets = getINT32(&msg_data[i-3]);PRINTF("%02x %02x %02x %02x   %02x neighbourhood packets num = %d\n",msg_data[i-3],msg_data[i-2],msg_data[i-1],msg_data[i],stu_bc.nb_packets,stu_bc.nb_packets);
		    Multicast multicast_data_temp;
		    maclist[0]=stu_bc.bc_peerID[1];
		    maclist[1]=stu_bc.bc_peerID[2];
		    maclist[2]=stu_bc.bc_peerID[3];
		    maclist[3]=stu_bc.bc_peerID[4];
		    maclist[4]=stu_bc.bc_peerID[5];
		    /*for(int i=0;i<5;i++){
			if(0x00==maclist[i]){
			     maclist[i]=0xee;
			}
		    }*/
		    if(0==bc_count){
			for(int i=0;i<5;i++){
			    if(0x00==maclist[i]){
				maclist[i]=0xee;
			    }
			}
		    }
		    if(0x00==maclist[0]){
			maclist[0]=0xee;
		    }
		    string str_temp((char*)maclist);
		    it_multicast = dic_multicast.find(str_temp.substr(0,5));

		    if(0!=bc_count){
			if(it_multicast!=dic_multicast.end()){
			    //cout<<it_unicast->second<<" ";
			    //dic.erase(it);
			}else{
			    cout<<"Don't find ! (multicast)"<<endl;
			}
		    }

		    if(0==bc_count){
			//cout<<"No. "<<++bc_count<<" "<<dic[str_temp.substr(0,5)]<<"\t";
			++bc_count;
			multicast_data_temp.str_dgnum=dic[str_temp.substr(0,5)];
		    }
		    else{
			//cout<<"No. "<<++bc_count<<" "<<dic_multicast[str_temp.substr(0,5)]<<"\t";
			++bc_count;
			multicast_data_temp.str_dgnum=dic_multicast[str_temp.substr(0,5)];
		    }

		    //multicast_data_temp.str_dgnum=dic_multicast[str_temp.substr(0,5)];
		    multicast_data_temp.bc_packets=stu_bc.bc_packets;
		    multicast_data_temp.mc_packets=stu_bc.mc_packets;
		    multicast_data_temp.nb_packets=stu_bc.nb_packets;
		    lock_s.lock();
		    multicast_data.push_back(multicast_data_temp);
		    lock_s.unlock();
		}
		lock_s.lock();
		multicast_data_show.push_back(multicast_data);
		multicast_data.clear();
		lock_s.unlock();
	    }
	    else if(0x06==msg_data[0]){
		ORG_DATA_STU stu_st;
		PRINTF("\n\n\nsize = %lu\n\n\n",msg_data.size());
		unsigned int len;
		unsigned int node_num_st;
		len=getINT16(&msg_data[1/*43*/]);PRINTF("%02x len = %d\n",len,len);
		node_num_st=msg_data[3/*45*/];PRINTF("%02x node_num_st = %d\n",node_num_st,node_num_st);
		int st_count=0;
		unsigned char maclist[5];
		for(int i=11/*53*/;i<msg_data.size();i+=8){
		    stu_st.state_peerID[0]=msg_data[i-7];PRINTF("node mac : %02x ",stu_st.state_peerID[0]);
		    stu_st.state_peerID[1]=msg_data[i-6];PRINTF("%02x ",stu_st.state_peerID[1]);
		    stu_st.state_peerID[2]=msg_data[i-5];PRINTF("%02x ",stu_st.state_peerID[2]);
		    stu_st.state_peerID[3]=msg_data[i-4];PRINTF("%02x ",stu_st.state_peerID[3]);
		    stu_st.state_peerID[4]=msg_data[i-3];PRINTF("%02x ",stu_st.state_peerID[4]);
		    stu_st.state_peerID[5]=msg_data[i-2];PRINTF("%02x\n",stu_st.state_peerID[5]);
		    PRINTF("ip = 10.%d.%d.%d\n",stu_st.state_peerID[3],stu_st.state_peerID[4],stu_st.state_peerID[5]);
		    stu_st.state = getINT16(&msg_data[i-1]);PRINTF("%02x %02x   %02x state = %d  ",msg_data[i-1],msg_data[i],stu_st.state,stu_st.state);
		    maclist[0]=stu_st.state_peerID[1];
		    maclist[1]=stu_st.state_peerID[2];
		    maclist[2]=stu_st.state_peerID[3];
		    maclist[3]=stu_st.state_peerID[4];
		    maclist[4]=stu_st.state_peerID[5];
		    for(int i=0;i<5;i++){
			if(0x00==maclist[i]){
			     maclist[i]=0xee;
			}
		    }
		    //cout<<"No : "<<++st_count<<" ";
		    string str_temp((char*)maclist);
		    it = dic.find(str_temp.substr(0,5));
		    if(it!=dic.end()){
			//cout<<it->second<<" ";
			//dic.erase(it);
		    }else{
			cout<<"Don't find ! (link_state)"<<endl;
		    }
		    if(1000>=stu_st.state){//cout<<"  "<<dic[str_temp.substr(0,5)]<<" ";
			PRINTF("link state is good!\n");
			//cout<<"link state is good!"<<endl;
		    }
		    else if(1000<stu_st.state&&1900>=stu_st.state){
			PRINTF("link state is normal!\n");
			//cout<<"link state is normal!"<<endl;
		    }
		    else if(1900<stu_st.state){
			PRINTF("link state is bad!\n");
			//cout<<"link state is bad!"<<endl;
		    }
		    Link link_temp;
		    link_temp.str_dgnum_link=dic[str_temp.substr(0,5)];
		    link_temp.state_link=stu_st.state;
		    lock_s.lock();
		    link_state.push_back(link_temp);
		    lock_s.unlock();
		    //cout<<"link size : "<<link_state.size()<<endl;
		}
		lock_s.lock();
		link_state_show.push_back(link_state);
		link_state.clear();
		lock_s.unlock();
		PRINTF("\n");
	    }else{
		PRINTF("other UDP pkt\n");
	    }
	} else {
	    PRINTF("recvfrom fail\n");
	}
	close(sockListen);
    }
}
void timeToDate(struct timeval tv){
    struct tm tv_temp;
    localtime_r(&tv.tv_sec,&tv_temp);
    char strT[32];
    strftime(strT,sizeof(strT),"%Y-%m-%d %H:%M:%S",&tv_temp);
    printf("%s\t",strT);
}
void unicast_show(){
    if(!unicast_data_show.empty()){
	lock_s.lock();
	cout<<"\nUNICAST_STATISTIC :"<<endl<<"\nNo\t"<<"dgnum\t"<<"time\t\t\t\t"<<"packets\t"<<"bytes\t"<<"fail\t"<<"retries\t"<<"bitrate\t"<<"signal\t"<<"lossrate"<<endl;
	for(int i=0;i<unicast_data_show.size();i++){
	    for(int j=0;j<unicast_data_show[i].size();j++){
		cout<<(j+1)<<"\t"<<unicast_data_show[i][j].str_dgnum<<"\t";
		//timeToDate(unicast_data_show[i][j].ts);
		struct timeval nowTus;
		gettimeofday(&nowTus,NULL);
		timeToDate(nowTus);
		cout<<"\t"<<unicast_data_show[i][j].tx_packets;
		cout<<"\t"<<unicast_data_show[i][j].tx_bytes;
		cout<<"\t"<<unicast_data_show[i][j].tx_fail;
		cout<<"\t"<<unicast_data_show[i][j].tx_retries;
		cout<<"\t"<<unicast_data_show[i][j].tx_bitrate;
		cout<<"\t"<<(int)unicast_data_show[i][j].signal;
		cout<<"\t"<<setprecision(4)<<((float)unicast_data_show[i][j].tx_fail/unicast_data_show[i][j].tx_packets)*100<<"%"<<dec<<endl;
	    }
	}
	lock_s.unlock();
    }
    lock_s.lock();
    unicast_data_show.clear();
    lock_s.unlock();
}
void multicast_show(){
    if(!multicast_data_show.empty()){
	lock_s.lock();
	cout<<"\nMULTICAST_STATISTIC :"<<endl<<"\nNo\t"<<"dgnum\t"<<"time\t\t\t\t"<<"bc_pkt\t"<<"mc_pkt\t"<<"nb_pkt"<<endl;
	for(int i=0;i<multicast_data_show.size();i++){
	    for(int j=0;j<multicast_data_show[i].size();j++){
		cout<<(j+1)<<"\t"<<multicast_data_show[i][j].str_dgnum<<"\t";
		struct timeval nowTus;
		gettimeofday(&nowTus,NULL);
		timeToDate(nowTus);
		cout<<"\t"<<multicast_data_show[i][j].bc_packets;
		cout<<"\t"<<multicast_data_show[i][j].mc_packets;
		cout<<"\t"<<multicast_data_show[i][j].nb_packets<<endl;
	    }
	}
	lock_s.unlock();
    }
    lock_s.lock();
    multicast_data_show.clear();
    lock_s.unlock();
}
void link_show(){
    if(!link_state_show.empty()){
	cout<<"\nLINK_STATE :"<<endl<<"\nNo\t"<<"dgnum\t"<<"state\t"<<"description(good/normal/bad)"<<endl;
	lock_s.lock();
	for(int i=0;i<link_state_show.size();i++){
	    for(int j=0;j<link_state_show[i].size();j++){
		cout<<(i+1)<<"."<<(j+1)<<"\t"<<link_state_show[i][j].str_dgnum_link<<"\t"<<link_state_show[i][j].state_link<<"\t";
		if(1000>=link_state_show[i][j].state_link){//cout<<"  "<<dic[str_temp.substr(0,5)]<<" ";
		    cout<<"link state is good!"<<endl;
		}
		else if(1000<link_state_show[i][j].state_link&&1900>=link_state_show[i][j].state_link){
		    cout<<"link state is normal!"<<endl;
		}
		else if(1900<link_state_show[i][j].state_link){
		    cout<<"link state is bad!"<<endl;
		}
	    }
	}
	lock_s.unlock();
    }
    lock_s.lock();
    link_state_show.clear();
    lock_s.unlock();
}
