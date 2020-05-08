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
map<int,string> dic_comments;
void init_dic(){
    dic_comments[0]="robot_num";
    dic_comments[1]="packets";
    dic_comments[2]="data_length";
    dic_comments[3]="speed";
    dic_comments[4]="signal";
    dic_comments[5]="error_rate";
}
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
        printf("!!ioctl error: %s\n", strerror(errno));
        close(sd);
        return -1;
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    snprintf(ip, 16, "%s", inet_ntoa(sin.sin_addr));

    close(sd);
    return 0;
}
