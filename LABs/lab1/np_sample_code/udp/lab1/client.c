/*************************************************************************
    > File Name: echocli_udp.c
    > Author: Simba
    > Mail: dameng34@163.com
    > Created Time: Sun 03 Mar 2013 06:13:55 PM CST
 ************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#define SIZE 512
char *filename = "recv.txt";
#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

void echo_cli(int sock)
{

    struct sockaddr_in servaddr;
    // 利用memset初始化，將struct sockaddr_in servaddr涵蓋的bits設為0
    memset(&servaddr, 0, sizeof(servaddr));
    // serv_addr為IPv4結構
    //     補充:AF_INET和PF_INET基本上等值，AF = Address Family, PF = Protocol Family
    //     所以，理論上建立socket時是指定協議，應該用PF_xxxx，設置地址時應該用AF_xxxx。
    servaddr.sin_family = AF_INET;
    // serv_addr的port為5188
    servaddr.sin_port = htons(5188);
    // servaddr綁127.0.0.1這個addr
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int ret, recvBytes = 0;
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        // sendto():將sendbuf傳給socket
        sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        // recvfrom():從server接收訊息
        ret = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
        if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("recvfrom");
        }
        recvBytes += ret;
        // 將寫入buffer的內容輸出到畫面
        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }
    printf("Total bytes received:%d\n", recvBytes);
    close(sock);


}

void getFile(int sockfd){
    int n;
    int fp;
    int _25 = 1;
    long file_size;
    time_t cur_time;
    char timebuf[80];
    char *filename = "recv.txt";
    char buffer[SIZE];
    long file_size, recv_size = 0;

    n = recv(sockfd, &file_size, sizeof(file_size),0);
    // file_size = atol(buffer);
    printf("[+] File size:%ld\n",file_size);
    bzero(buffer, SIZE);
    fp = open(filename, O_CREAT | O_WRONLY, 0644);
    clock_t start_time = clock();
    while( (n = read(sockfd,buffer,sizeof(buffer))) > 0){
        write(fp, buffer, n);
        recv_size += n;
        if(recv_size >= (file_size/4)*_25){
            printf("[Info] Data received:%ld/%ld\n",recv_size,file_size);
            time(&cur_time);
            strftime(timebuf, 80, "%Y/%m/%d %X", localtime(&cur_time));
            printf("[Info]:%d%% %s\n\n", _25*25, timebuf);
            _25++;
        }
    }
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("[+] Total tran time: %d ms\n", (int)(time_spent*1000));
    printf("[+] FIle Size:%ld MB\n", (file_size/1024)/1024);
}

int main(void)
{
    int sock;
    // int socket(int domain, int type, int protocol);
	// 建立名為sockfd的socket()物件
	// - domain：	PF_INET，表示此Socket目的是Hosts之間的連線，且是採用IPv4。
	// - type：		SOCK_DGRAM，表示此Socket提供的是datagram-based protocol，屬於UDP
	// - protocol：	0，讓kernel選擇type對應的默認協議。
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket");

    // echo_cli(sock);
    getFile(sock);
    return 0;
}