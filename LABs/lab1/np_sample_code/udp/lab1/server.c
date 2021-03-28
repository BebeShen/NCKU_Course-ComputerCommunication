/*************************************************************************
    > File Name: echoser_udp.c
    > Author: Simba
    > Mail: dameng34@163.com
    > Created Time: Sun 03 Mar 2013 06:13:55 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#define SIZE 512
char *filename = "data.txt";

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)

int get_file_size(char* file_name){
    struct stat statbuf;
    stat(file_name, &statbuf);
    return statbuf.st_size;
}

void echo_ser(int sock)
{
    char recvbuf[1024] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;
    int sendBytes = 0;

    while (1){
        peerlen = sizeof(peeraddr);
        memset(recvbuf, 0, sizeof(recvbuf));
        // recvfrom():從client接收訊息
        n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&peeraddr, &peerlen);
        if (n == -1){
            if (errno == EINTR)
                continue;
            ERR_EXIT("recvfrom error");
        }
        else if(n > 0){
            // 將讀取的buffer輸出到畫面
            sendBytes += n;
            fputs(recvbuf, stdout);
            // sendto()
            sendto(sock, recvbuf, n, 0, (struct sockaddr *)&peeraddr, peerlen);
        }
    }
    printf("Total bytes send:%d\n", sendBytes);
    close(sock);
}

void sendFile(FILE *fp, int sockfd){
    char recvbuf[SIZE] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;
    int sendBytes = 0;

    int fd = open(filename, O_RDONLY);
    struct stat stat_buf;
    fstat(fd, &stat_buf);
    file_size = stat_buf.st_size;
    printf("[+] File size:%ld\n",file_size);
    send(sockfd, &file_size, sizeof(file_size), 0);

    char* file_content = malloc(stat_buf.st_size);
    fread(file_content, stat_buf.st_size, 1, fp);
    // printf("[+] File content:\n%s\n",file_content);
    send(sockfd, file_content, stat_buf.st_size, 0);
    free(file_content);
}

int main(void)
{
    int sock;
    // 建立IPv4、UDP的Socket物件
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket error");
    // servaddr用來儲存socket server的資訊
    struct sockaddr_in servaddr;
    // 清空
    memset(&servaddr, 0, sizeof(servaddr));
    // AF_INET:IPv4, port:5188, INADDR_ANY:kernel替我決定loacl IP
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 將Socket綁到這個socket server
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");
    FILE *fp;
    fp = fopen(filename, "r");
    // echo_ser(sock);
    sendFile(fp,sock)
    return 0;
}
