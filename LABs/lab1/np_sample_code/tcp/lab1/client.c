#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#define SIZE 512

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void getFile(int sockfd){
    int n;
    int fp;
    time_t cur_time;
    char timebuf[80];
    char *filename = "recv.txt";
    char buffer[SIZE];
    long file_size,recv_size = 0;

    n = recv(sockfd, &file_size, sizeof(file_size),0);
    // file_size = atol(buffer);
    printf("[+] File size:%ld\n",file_size);
    bzero(buffer, SIZE);
    fp = open(filename, O_CREAT | O_WRONLY, 0644);
    // while (1) {
    //     n = read(sockfd, buffer, SIZE);
    //     if (n <= 0){
    //         break;
    //     }
    //     recv_size += n;
	//     printf("[+] Current received data size:%d\n",recv_size);
    //     fprintf(fp, "%s", buffer);
    //     bzero(buffer, SIZE);
    // }
    while( (n = read(sockfd,buffer,sizeof(buffer))) > 0){
        write(fp, buffer, n);
        recv_size += n;
    }
    printf("[+] FIle size:%ld\n",recv_size);
    time(&cur_time);
    strftime(timebuf, 80, "%Y/%m/%d %X", localtime(&cur_time));
    printf("[Info]:\n\n\t\t100%% %s\n",timebuf);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    // 執行時參數需要>=3
    // $ ./<執行檔檔名> <hostname> <port number> 
    // 例如：./client localhost 5566
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    // int socket(int domain, int type, int protocol);
	// 建立名為sockfd的socket()物件
	// - domain：	AF_INET，表示此Socket目的是Hosts之間的連線，且是採用IPv4。
	// - type：		SOCK_STREAM，表示此Socket提供的是Connection-Oriented Byte-Stream，屬於TCP
	// - protocol：	0，讓kernel選擇type對應的默認協議。
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Socket物件若建立失敗會回傳-1，表示INVALID_SOCKET
    if (sockfd < 0) 
        error("[-] ERROR opening socket");
    else
        printf("[+] Server socket created successfully.\n");
    // 透過name取得host address
    // struct hostent *gethostbyname(const char *name);
    //      gethostbyname的return值是hostent的struct的指標
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"[-] ERROR, no such host\n");
        exit(0);
    }
    // 利用bzero初始化，將struct sockaddr_in serv_addr涵蓋的bits設為0
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // serv_addr為IPv4結構
    serv_addr.sin_family = AF_INET;
    // void bcopy(const void *src, void *dest, size_t n);
    // 利用bcopy將server->h_addr複製 server->h_length的byte長度 給serv_addr.sin_addr.s_addr
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    // serv_addr的port為傳進來的port number
    serv_addr.sin_port = htons(portno);
    // 建立與Server的連線
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("[-] ERROR connecting");
    else
        printf("[+] Connected to Server.\n");
    // printf("Please enter the message: ");
    // bzero(buffer,256);
    // // 從stdin拿取要輸入的訊息，並寫入buffer
    // fgets(buffer,255,stdin);
    // // 將buffer的訊息寫入socket傳給server
    // n = write(sockfd,buffer,strlen(buffer));
    // if (n < 0) 
    //      error("ERROR writing to socket");
    // // 清空buffer
    // bzero(buffer,256);
    // // 從server讀取訊息
    // n = read(sockfd,buffer,255);
    // if (n < 0) 
    //      error("ERROR reading from socket");
    // printf("%s\n",buffer);
    
    // File Transfer
    // printf("%s\n",buffer);
    getFile(sockfd);
    printf("[+] Closing the connection.\n");
    // 關閉socket
    close(sockfd);
    return 0;
}
