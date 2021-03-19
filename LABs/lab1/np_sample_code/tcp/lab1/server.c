/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // define sockaddr_in
#define SIZE 1024
char *filename = "send.txt";

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void sendFile(FILE *fp, int sockfd){
    int n;
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp) != NULL) {
        printf("File Transferring...\n");
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.\n");
            exit(1);
        }
        bzero(data, SIZE);
    }
    printf("[+] Sent File:%s to Client Successfully!\n", filename);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    // 執行時參數需要>=2
    // $ ./<執行檔檔名> <port number>
    // 例如：./server 5566
    if (argc < 2) {
        fprintf(stderr,"[-]ERROR, no port provided\n");
        exit(1);
    }
    // int socket(int domain, int type, int protocol);
    // 建立名為sockfd的socket()物件
    // - domain：	     AF_INET，表示此Socket目的是Hosts之間的連線，且是採用IPv4。
    // - type：		SOCK_STREAM，表示此Socket提供的是Connection-Oriented Byte-Stream，屬於TCP
    // - protocol：	0，讓kernel選擇type對應的默認協議。
    // - Return Value:  sockfd, socket的file discriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Socket物件若建立失敗會回傳-1，表示INVALID_SOCKET
    if (sockfd < 0)
        error("[-]ERROR opening socket\n");
    else
        printf("[+]Server socket created successfully.\n");
    // 利用bzero初始化，將struct sockaddr_in serv_addr涵蓋的bits設為0
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // 獲取傳進來的port number參數
    portno = atoi(argv[1]);
    // serv_addr為IPv4結構
    serv_addr.sin_family = AF_INET;
    // 當process想要建立連線或接收新封包，process需要透過bind()來綁定local interface的address
    // 對於Server而言，若不希望只是綁定localhost，則設此值為INADDR_ANY，如此Socket就會bind這台Host上所有ip
    // 懶人包：我不在乎loacl IP是什麼，讓kernel替我決定就好。
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // htons()將Endian調整成network endian(little endian)
    // serv_addr的port為傳進來的port number
    serv_addr.sin_port = htons(portno);
    // bind address to socket
    // 透過bind()將剛剛的ip address綁到socket上
    // 不論當Clinet或當Server，都需要給Socket一份address
    // 若bind失敗會回傳-1，若成功則回傳0
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("[-]ERROR on binding\n");
    else
        printf("[+]Binding successfull.\n");
    // 開啟Server的監聽，並設置最多5個Client連線
    listen(sockfd,5);
    printf("[+]Listening....\n");
    clilen = sizeof(cli_addr);
    // Server接收連線請求
    // - cli_addr:     用於儲存接收到的Client端相關資訊，比如port、IP address。
    // - clilen:       cli_addr的大小，也會於function return自動調整大小。
    // - Return Value: 0表成功, -1表失敗。
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
    if (newsockfd < 0)
        error("[-]ERROR on accept\n");
        
    //  // 將buffer歸零
    //  bzero(buffer,256);
    //  // 從client讀取資料
    //  n = read(newsockfd,buffer,255);
    //  if (n < 0) error("ERROR reading from socket");
    //  // 把訊息寫進buffer
    //  printf("Here is the message: %s\n",buffer);
    //  // 訊息透過socket傳給client
    //  n = write(newsockfd,"I got your message",18);
    //  if (n < 0) error("ERROR writing to socket");
    
    //TODO: File Transfer
    n = write(newsockfd,"Establist Connection",20);
    if (n < 0) error("[-]ERROR on write msg\n");
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        error("[-]Error in reading file.\n");
    }
    sendFile(fp, newsockfd)
    printf("[+]Closing the connection.\n");
    // 關閉client的socket
    close(newsockfd);
    // 關閉server的socket
    close(sockfd);
    return 0;
}
