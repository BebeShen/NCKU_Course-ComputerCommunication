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
  
// function to clear buffer
void clearBuf(char* b)
{
    int i;
    for (i = 0; i < NET_BUF_SIZE; i++)
        b[i] = '\0';
}
  
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
int main()
{
    int sockfd, nBytes, ret = -1;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT_NO);
    addr_con.sin_addr.s_addr = INADDR_ANY;
    char net_buf[NET_BUF_SIZE];
    FILE* fp;
  
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
  
    if (sockfd < 0)
        printf("\nfile descriptor not received!!\n");
    else
        printf("\nfile descriptor %d received\n", sockfd);
  
    // bind()
    if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0)
        printf("\nSuccessfully binded!\n");
    else
        printf("\nBinding Failed!\n");
  
    printf("\nWaiting for file name...\n");

    // clearBuf(net_buf);

    long file_size;

    // file_size = get_file_size(filename);
    
    // receive file name
    nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
    printf("\n[+] File Name Received: %s\n", net_buf);
    int fd = open(net_buf, O_RDONLY);
    struct stat stat_buf;
    fstat(fd, &stat_buf);
    file_size = stat_buf.st_size;
    printf("[+] File size:%ld\n",file_size);
    sendto(sockfd, &file_size, sizeof(file_size), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    fp = fopen(net_buf, "r");
    if (fp == NULL)
        printf("\n[-] File open failed!\n");
    else
        printf("\n[+] File Successfully opened!\n");
    
    while (1) {
    memset(net_buf, 0, sizeof(net_buf));
    // process
    if (sendFile(fp, net_buf, NET_BUF_SIZE)) {
        sendto(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
        break;
    }

    // send
    sendto(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    // clearBuf(net_buf);
    //  memset(net_buf, 0, sizeof(net_buf));
    }
    while (ret < 0){
        ret = recvfrom(sockfd, net_buf, NET_BUF_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
    }
    
    
    if (fp != NULL)
        fclose(fp);
    return 0;
}