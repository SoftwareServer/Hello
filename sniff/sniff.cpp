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
#include<time.h>
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
FILE *logfile;
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
    unsigned int tx_retries;
    unsigned int tx_fail;
    signed int tx_bitrate;
    signed char signal;

    unsigned char bc_peerID[6];
    unsigned int bc_packets;
    unsigned int mc_packets;
    unsigned int nb_packets;

    unsigned char state_peerID[6];
    unsigned int state;
}__attribute__((packed))ORG_DATA_STU;
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
int main(int argc, char** argv){
    int interval=1;
    if(2!=argc){
	PRINTF("Outputing interval is 1 second(default).\n");
    }else{
	interval=atoi(argv[1]);
	if(interval<=0){
	    PRINTF("Input is invalid ! Outputing interval is 1 second(default).\n");
	    interval=1;
	}
	else{
	    PRINTF("Outputing interval is %d second.\n",interval);
	}
    }

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
    dic["\x16\x16\x03\x57\x8e"]="ground 03";
    dic["\x16\x16\x03\x57\xde"]="ground 04";
    cout<<"The number of other Channel boards in the dictionary are : "<<dic.size()<<endl;

    start_cmd();

    time_t nowT;    
    nowT = time(0);    
    // time(&nowT);
    // nowT = time(NULL);
    char strT[32];
    /* use strftime formats the time（"YYYY-MM-DD hh:mm:ss")*/
    strftime(strT, sizeof(strT), "%Y-%m-%d-%H-%M-%S", localtime(&nowT));

    logfile=fopen(strT,"w");
    if(logfile==NULL) 
    {
	PRINTF("Unable to create file.");
    }

    while(true){

	int sockListen;
	if((sockListen = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
	    PRINTF("socket fail\n");
	    return -1;
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
		__attribute__((packed))ORG_DATA_STU stu;
		PRINTF("\n\n\nsize = %lu\n\n\n",msg_data.size());
		unsigned int len;
		unsigned char mac[6];
		unsigned int tx_num;
		len=getINT16(&msg_data[1/*43*/]);PRINTF("%02x len = %d\n",len,len);
		fprintf(logfile,"peer_len = %d\n",len);
		mac[0]=msg_data[3/*45*/];PRINTF("mac : %02x ",mac[0]);
		mac[1]=msg_data[4/*46*/];PRINTF("%02x ",mac[1]);
		mac[2]=msg_data[5/*47*/];PRINTF("%02x ",mac[2]);
		mac[3]=msg_data[6/*48*/];PRINTF("%02x ",mac[3]);
		mac[4]=msg_data[7/*49*/];PRINTF("%02x ",mac[4]);
		mac[5]=msg_data[8/*50*/];PRINTF("%02x\n",mac[5]);PRINTF("ip = 10.%d.%d.%d\n",mac[3],mac[4],mac[5]);
		fprintf(logfile,"peer_mac : %02x ",mac[0]);
		fprintf(logfile,"%02x ",mac[1]);
		fprintf(logfile,"%02x ",mac[2]);
		fprintf(logfile,"%02x ",mac[3]);
		fprintf(logfile,"%02x ",mac[4]);
		fprintf(logfile,"%02x\n",mac[5]);
		tx_num=getINT16(&msg_data[9/*51*/]);PRINTF("%02x tx_num = %d\n",tx_num,tx_num);
		fprintf(logfile,"peer_tx_num = %d\n",tx_num);
		int count=0;
		for(int i=45/*87*/;i<msg_data.size();i+=35){
		    fprintf(logfile,"No : %d\n",++count);
		    stu.peerID[0]=msg_data[i-34];PRINTF("another mac : %02x ",stu.peerID[0]);
		    stu.peerID[1]=msg_data[i-33];PRINTF("%02x ",stu.peerID[1]);
		    stu.peerID[2]=msg_data[i-32];PRINTF("%02x ",stu.peerID[2]);
		    stu.peerID[3]=msg_data[i-31];PRINTF("%02x ",stu.peerID[3]);
		    stu.peerID[4]=msg_data[i-30];PRINTF("%02x ",stu.peerID[4]);
		    stu.peerID[5]=msg_data[i-29];PRINTF("%02x\n",stu.peerID[5]);
		    fprintf(logfile,"peer_another_mac : %02x ",stu.peerID[0]);
		    fprintf(logfile,"%02x ",stu.peerID[1]);
		    fprintf(logfile,"%02x ",stu.peerID[2]);
		    fprintf(logfile,"%02x ",stu.peerID[3]);
		    fprintf(logfile,"%02x ",stu.peerID[4]);
		    fprintf(logfile,"%02x\n",stu.peerID[5]);
		    PRINTF("ip = 10.%d.%d.%d\n",stu.peerID[3],stu.peerID[4],stu.peerID[5]);
		    stu.ts.tv_sec = getINT32(&msg_data[i-28]);
		    stu.ts.tv_usec = getINT32(&msg_data[i-24]);PRINTF("time = %ld.%ld\n",stu.ts.tv_sec,stu.ts.tv_usec);
		    fprintf(logfile,"peer_time : %ld.%ld\n",stu.ts.tv_sec,stu.ts.tv_usec);
		    stu.tx_packets = getINT32(&msg_data[i-20]);PRINTF("%02x %02x %02x %02x   %02x tx_packets = %d\n",msg_data[i-20],msg_data[i-19],msg_data[i-18],msg_data[i-17],stu.tx_packets,stu.tx_packets);
		    fprintf(logfile,"peer_tx_packets = %d\n",stu.tx_packets);
		    stu.tx_bytes = getINT32(&msg_data[i-16]);PRINTF("%02x %02x %02x %02x   %02x tx_bytes = %d\n",msg_data[i-16],msg_data[i-15],msg_data[i-14],msg_data[i-13],stu.tx_bytes,stu.tx_bytes);
		    fprintf(logfile,"peer_tx_bytes = %d\n",stu.tx_bytes);
		    stu.tx_fail = getINT32(&msg_data[i-12]);PRINTF("%02x %02x %02x %02x   %02x tx_fail = %d\n",msg_data[i-12],msg_data[i-11],msg_data[i-10],msg_data[i-9],stu.tx_fail,stu.tx_fail);
		    fprintf(logfile,"peer_tx_fail = %d\n",stu.tx_fail);
		    stu.tx_retries = getINT32(&msg_data[i-8]);PRINTF("%02x %02x %02x %02x   %02x tx_retries = %d\n",msg_data[i-8],msg_data[i-7],msg_data[i-6],msg_data[i-5],stu.tx_retries,stu.tx_retries);
		    fprintf(logfile,"peer_tx_retries = %d\n",stu.tx_retries);
		    stu.tx_bitrate = getINT32(&msg_data[i-4]);PRINTF("%02x %02x %02x %02x   %02x tx_bitrate = %d\n",msg_data[i-4],msg_data[i-3],msg_data[i-2],msg_data[i-1],stu.tx_bitrate,stu.tx_bitrate);
		    fprintf(logfile,"peer_tx_bitrate = %d\n",stu.tx_bitrate);
		    stu.signal = msg_data[i];PRINTF("%02x signal = %d\n",stu.signal,stu.signal);
		    fprintf(logfile,"peer_signal = %d\n",stu.signal);fprintf(logfile,"\n########################\n");
		}
	    }
	    else if(0x04==msg_data[0]){
		__attribute__((packed))ORG_DATA_STU stu_bc;
		PRINTF("\n\n\nsize = %lu\n\n\n",msg_data.size());
		unsigned int len;
		unsigned int node_num;
		len=getINT16(&msg_data[1/*43*/]);PRINTF("%02x len = %d\n",len,len);
		fprintf(logfile,"bc_len = %d\n",len);
		node_num=msg_data[3/*45*/];PRINTF("%02x node_num = %d\n",node_num,node_num);
		fprintf(logfile,"bc_node_num = %d\n",node_num);
		int bc_count=0;
		for(int i=21/*63*/;i<msg_data.size();i+=18){
		    fprintf(logfile,"No : %d\n",++bc_count);
		    stu_bc.bc_peerID[0]=msg_data[i-17];PRINTF("node mac : %02x ",stu_bc.bc_peerID[0]);
		    stu_bc.bc_peerID[1]=msg_data[i-16];PRINTF("%02x ",stu_bc.bc_peerID[1]);
		    stu_bc.bc_peerID[2]=msg_data[i-15];PRINTF("%02x ",stu_bc.bc_peerID[2]);
		    stu_bc.bc_peerID[3]=msg_data[i-14];PRINTF("%02x ",stu_bc.bc_peerID[3]);
		    stu_bc.bc_peerID[4]=msg_data[i-13];PRINTF("%02x ",stu_bc.bc_peerID[4]);
		    stu_bc.bc_peerID[5]=msg_data[i-12];PRINTF("%02x\n",stu_bc.bc_peerID[5]);
		    fprintf(logfile,"bc_node_mac : %02x ",stu_bc.bc_peerID[0]);
		    fprintf(logfile,"%02x ",stu_bc.bc_peerID[1]);
		    fprintf(logfile,"%02x ",stu_bc.bc_peerID[2]);
		    fprintf(logfile,"%02x ",stu_bc.bc_peerID[3]);
		    fprintf(logfile,"%02x ",stu_bc.bc_peerID[4]);
		    fprintf(logfile,"%02x\n",stu_bc.bc_peerID[5]);
		    stu_bc.bc_packets = getINT32(&msg_data[i-11]);PRINTF("%02x %02x %02x %02x   %02x broadcast packets num = %d\n",msg_data[i-11],msg_data[i-10],msg_data[i-9],msg_data[i-8],stu_bc.bc_packets,stu_bc.bc_packets);
		    fprintf(logfile,"broadcast_packets_num = %d\n",stu_bc.bc_packets);
		    stu_bc.mc_packets = getINT32(&msg_data[i-7]);PRINTF("%02x %02x %02x %02x   %02x multicast packets num = %d\n",msg_data[i-7],msg_data[i-6],msg_data[i-5],msg_data[i-4],stu_bc.mc_packets,stu_bc.mc_packets);
		    fprintf(logfile,"multicast_packets_num = %d\n",stu_bc.mc_packets);
		    stu_bc.nb_packets = getINT32(&msg_data[i-3]);PRINTF("%02x %02x %02x %02x   %02x neighbourhood packets num = %d\n",msg_data[i-3],msg_data[i-2],msg_data[i-1],msg_data[i],stu_bc.nb_packets,stu_bc.nb_packets);
		    fprintf(logfile,"neighbourhood_packets_num = %d\n",stu_bc.nb_packets);fprintf(logfile,"\n########################\n");
		}
	    }
	    else if(0x06==msg_data[0]){
		__attribute__((packed))ORG_DATA_STU stu_st;
		PRINTF("\n\n\nsize = %lu\n\n\n",msg_data.size());
		unsigned int len;
		unsigned int node_num_st;
		len=getINT16(&msg_data[1/*43*/]);PRINTF("%02x len = %d\n",len,len);
		fprintf(logfile,"state_len = %d\n",len);
		node_num_st=msg_data[3/*45*/];PRINTF("%02x node_num_st = %d\n",node_num_st,node_num_st);
		fprintf(logfile,"node_st_num = %d\n",node_num_st);
		int st_count=0;
		unsigned char maclist[5];
		for(int i=11/*53*/;i<msg_data.size();i+=8){
		    fprintf(logfile,"No : %d\n",++st_count);
		    stu_st.state_peerID[0]=msg_data[i-7];PRINTF("node mac : %02x ",stu_st.state_peerID[0]);
		    stu_st.state_peerID[1]=msg_data[i-6];PRINTF("%02x ",stu_st.state_peerID[1]);
		    stu_st.state_peerID[2]=msg_data[i-5];PRINTF("%02x ",stu_st.state_peerID[2]);
		    stu_st.state_peerID[3]=msg_data[i-4];PRINTF("%02x ",stu_st.state_peerID[3]);
		    stu_st.state_peerID[4]=msg_data[i-3];PRINTF("%02x ",stu_st.state_peerID[4]);
		    stu_st.state_peerID[5]=msg_data[i-2];PRINTF("%02x\n",stu_st.state_peerID[5]);
		    fprintf(logfile,"state_node_mac : %02x ",stu_st.state_peerID[0]);
		    fprintf(logfile,"%02x ",stu_st.state_peerID[1]);
		    fprintf(logfile,"%02x ",stu_st.state_peerID[2]);
		    fprintf(logfile,"%02x ",stu_st.state_peerID[3]);
		    fprintf(logfile,"%02x ",stu_st.state_peerID[4]);
		    fprintf(logfile,"%02x\n",stu_st.state_peerID[5]);
		    PRINTF("ip = 10.%d.%d.%d\n",stu_st.state_peerID[3],stu_st.state_peerID[4],stu_st.state_peerID[5]);
		    stu_st.state = getINT16(&msg_data[i-1]);PRINTF("%02x %02x   %02x state = %d  ",msg_data[i-1],msg_data[i],stu_st.state,stu_st.state);
		    fprintf(logfile,"state = %d\n",stu_st.state);fprintf(logfile,"\n########################\n");
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
		    cout<<"No : "<<st_count<<" ";
		    string str_temp((char*)maclist);
		    it = dic.find(str_temp.substr(0,5));
		    if(it!=dic.end()){
			cout<<it->second<<" ";
			//dic.erase(it);
		    }else{
			cout<<"Don't find ! "<<endl;
		    }
		    if(1000>=stu_st.state){
			PRINTF("link state is good!\n");
			cout<<"link state is good!"<<endl;
		    }
		    else if(1000<stu_st.state&&1900>=stu_st.state){
			PRINTF("link state is normal!\n");
			cout<<"link state is normal!"<<endl;
		    }
		    else if(1900<stu_st.state){
			PRINTF("link state is bad!\n");
			cout<<"link state is bad!"<<endl;
		    }
		}
		cout<<endl;
		PRINTF("\n");
	    }else{
		PRINTF("other UDP pkt\n");
	    }
	} else {
	    PRINTF("recvfrom fail\n");
	}
	//sleep(interval);
	close(sockListen);
    }
    return 0;
}
