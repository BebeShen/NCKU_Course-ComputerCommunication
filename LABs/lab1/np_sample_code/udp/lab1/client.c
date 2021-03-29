// client code for UDP socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
  
#define IP_PROTOCOL 0
#define LOCAL_ADDRESS "127.0.0.1" // localhost
#define PORT_NO 15050
#define NET_BUF_SIZE 512
#define sendrecvflag 0
#define UDP_ACK "ACK"
int _25 = 1;

void checkProcess(long recv_size, long file_size){
    char timebuf[80];
    time_t cur_time;
    if(recv_size >= (file_size/4)*_25){
        printf("[Info] Data received:%ld/%ld\n",recv_size,file_size);
        time(&cur_time);
        strftime(timebuf, 80, "%Y/%m/%d %X", localtime(&cur_time));
        printf("[Info]:%d%% %s\n\n", _25*25, timebuf);
        _25++;
    }
}
// function to receive file
int recvFile(char* buf, int s, long* size)
{
    int i;
    char ch;
    for (i = 0; i < s; i++) {
        ch = buf[i];
        // (*size)++;
        if (ch == EOF)
            return 1;
        else{
            //printf("%c", ch);
            (*size)++;
        }

    }
    return 0;
}
  
// driver code
int main(int argc, char *argv[])
{
    int sockfd;
    long nBytes, recv_bytes = 0, file_size;
    char net_buf[NET_BUF_SIZE];
    char filename[NET_BUF_SIZE] = "data.txt";
    FILE* fp;
  
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    char* ip = argv[3];
    int portno = atoi(argv[4]);
    // server socket ip addr setting
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(portno);
    addr_con.sin_addr.s_addr = inet_addr(LOCAL_ADDRESS);
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
  
    if (sockfd < 0)
        printf("[-] file descriptor not received!!\n");
    else
        printf("[+] file descriptor %d received\n", sockfd);
  
    sendto(sockfd, filename, sizeof(filename), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    recvfrom(sockfd, &file_size, sizeof(file_size), sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
    printf("[+] file size:%ld\n", file_size);
    
    // start clock()
    clock_t start_time = clock();
    while (1) {
        // receive
        // clearBuf(net_buf);

        // sendto(sockfd, UDP_ACK, 4, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
        memset(net_buf, 0, sizeof(net_buf));
        nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
        // recv_bytes += nBytes;
        // printf("[+] Data received:%ld\n[+] Current Data size:%ld\n", nBytes,recv_bytes);
        
        // process File
        if (recvFile(net_buf, NET_BUF_SIZE, &recv_bytes)) {
            break;
        }
        checkProcess(recv_bytes, file_size);
    }
    // end clock()
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\n[+] Total data received:%ld\n[+] File size:%ld\n", recv_bytes, file_size);
    printf("[+] Packet Loss Rate(loss data size/total size):%lf\n", (((double)(file_size-recv_bytes))/(double)file_size));
    printf("[+] Total tran time: %lf ms\n", (time_spent*1000));
    printf("[+] FIle Size:%ld MB\n", (file_size/1024)/1024);
    sendto(sockfd, "OK", sizeof("OK"), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    close(sockfd);
    return 0;
}
