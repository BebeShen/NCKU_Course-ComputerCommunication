#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

void echo_cli(int sock)
{
    long file_size, recv_size = 0;
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

    int ret;
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    // hello server
    sendto(sockfd, net_buf, NET_BUF_SIZE,
               sendrecvflag, (struct sockaddr*)&addr_con,
               addrlen);
    // 獲取檔案大小
    // ret = recv(sock, &file_size, sizeof(file_size),0);
    ret = recvfrom(sock, &file_size, sizeof(file_size), 0, NULL, NULL);
    if (ret == -1)
    {
        if (errno == EINTR)
            continue;
        ERR_EXIT("recvfrom");
    }
    // 將寫入buffer的內容輸出到畫面
    fputs(recvbuf, stdout);
    file_size = 
    memset(sendbuf, 0, sizeof(sendbuf));
    memset(recvbuf, 0, sizeof(recvbuf));    

    while (1){
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
        // receive data
        ret = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
        if (ret == -1){
            if (errno == EINTR)
                continue;
            ERR_EXIT("recvfrom");
        }
        // 將寫入buffer的內容輸出到畫面
        fputs(recvbuf, stdout);
        
    }

    close(sock);


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

    echo_cli(sock);

    return 0;
}