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
#define PORT_NO 15050
#define NET_BUF_SIZE 512
#define sendrecvflag 0
#define nofile "File Not Found!"
  
// function sending file
int sendFile(FILE* fp, char* buf, int s)
{
    int i, len;
    if (fp == NULL) {
        strcpy(buf, nofile);
        len = strlen(nofile);
        buf[len] = EOF;
        return 1;
    }
  
    char ch;
    for (i = 0; i < s; i++) {
        ch = fgetc(fp);
        buf[i] = ch;
        if (ch == EOF)
            return 1;
    }
    return 0;
}
  
// driver code
int main(int argc, char *argv[])
{
    int sockfd, nBytes, ret;
    long file_size;
    char net_buf[NET_BUF_SIZE];
    char ack[4] = {0};
    FILE* fp;
    
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    char* ip = argv[3];
    int portno = atoi(argv[4]);
    // server socket ip addr settin
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(portno);
    addr_con.sin_addr.s_addr = INADDR_ANY;
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
  
    if (sockfd < 0)
        printf("[-] file descriptor not received!!\n");
    else
        printf("[+] file descriptor %d received\n", sockfd);
  
    // bind()
    if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0)
        printf("[+] Successfully binded!\n");
    else
        printf("[-] Binding Failed!\n");

    // receive file name
    nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
    printf("\n[+] File Name Received: %s\n", net_buf);
    // get file size
    int fd = open(net_buf, O_RDONLY);
    struct stat stat_buf;
    fstat(fd, &stat_buf);
    file_size = stat_buf.st_size;
    printf("[+] File size:%ld\n",file_size);
    // send file size
    sendto(sockfd, &file_size, sizeof(file_size), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    fp = fopen(net_buf, "r");
    if (fp == NULL)
        printf("[-] File open failed!\n");
    
    while (1) {
        // ret = recvfrom(sockfd, ack, 4, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
        // ack[ret] = '\0';
        
        // usleep(1);
        // printf("[+] info: %s\n",ack);
        memset(net_buf, 0, sizeof(net_buf));
        // process file, if fp == EOF break
        if (sendFile(fp, net_buf, NET_BUF_SIZE)) {
            sendto(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
            break;
         }
        // send
        sendto(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
        // clearBuf(net_buf);
        //  memset(net_buf, 0, sizeof(net_buf));
    }
    
    
    if (fp != NULL)
        fclose(fp);
    close(sockfd);
    return 0;
}
