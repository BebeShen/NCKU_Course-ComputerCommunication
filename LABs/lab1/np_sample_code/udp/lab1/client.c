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
    while (1) {
        // receive
        // clearBuf(net_buf);

        // sendto(sockfd, UDP_ACK, 4, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
        memset(net_buf, 0, sizeof(net_buf));
        nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
        // recv_bytes += nBytes;
        printf("[+] Data received:%ld\n[+] Current Data size:%ld\n", nBytes,recv_bytes);
        // process
        if (recvFile(net_buf, NET_BUF_SIZE, &recv_bytes)) {
            break;
        }
    }
    sendto(sockfd, "OK", sizeof("OK"), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    printf("\n[+] Total data received:%ld\n[+] File size:%ld\n", recv_bytes, file_size);
    return 0;
}
