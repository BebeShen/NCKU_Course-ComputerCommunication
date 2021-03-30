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
#include <arpa/inet.h>
#define IP_PROTOCOL 0
#define buffer_SIZE 512
#define sendrecvflag 0
#define nofile "File Not Found!"
#define LOCAL_ADDRESS "127.0.0.1" // localhost
#define SIZE 512
char* filename;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void tcpGetFile(int sockfd){
    long file_size, recv_size = 0;
    int n, _25 = 1;
    char buffer[SIZE];
    // get file size from sender
    n = recv(sockfd, &file_size, sizeof(file_size),0);
    bzero(buffer, SIZE);
    // 寫入檔案
    int fp = open("output.txt", O_CREAT | O_WRONLY, 0644);
    // read file from socket
    while( (n = read(sockfd,buffer,sizeof(buffer))) > 0){
        write(fp, buffer, n);
    }
}

void tcpSendFile(int sockfd){
    int n, _25 = 1;
    char buffer[SIZE];
    /* time function */
    time_t cur_time;
    char timebuf[80];
    // get file size
    long file_size, send_size = 0;
    int fd = open(filename, O_RDONLY);
    struct stat stat_buf;
    fstat(fd, &stat_buf);
    file_size = stat_buf.st_size;
    printf("[+] File size:%ld\n",file_size);
    // send file size
    send(sockfd, &file_size, sizeof(file_size), 0);
    
    int fp = open(filename, O_RDONLY);
    // start clock()
    clock_t start_time = clock();
    // read file to socket
    while( (n = read(fp,buffer,sizeof(buffer))) > 0){
        write(sockfd, buffer, n);
        send_size += n;
        if(send_size >= (file_size/4)*_25){
            printf("[Info] Data send:%ld/%ld\n",send_size,file_size);
            time(&cur_time);
            strftime(timebuf, 80, "%Y/%m/%d %X", localtime(&cur_time));
            printf("[Info]:%d%% %s\n\n", _25*25, timebuf);
            _25++;
        }
    }
    // end clock()
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time);
    printf("[+] Total trans time: %f ms\n", (time_spent));
    printf("[+] Total trans time: %f s\n", (time_spent/CLOCKS_PER_SEC));
    printf("[+] file Size:%ldMB\n", (file_size/1024)/1024);
}

// function sending file
int sendFile(FILE* fp, char* buf, int s, long* size)
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
        (*size)++;
    }
    return 0;
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

void udpSendFile(int sockfd, struct sockaddr_in addr_con, int addrlen){
    int n, _25 = 1;
    char buffer[SIZE];
    /* time function */
    time_t cur_time;
    char timebuf[80];
    // get file size
    long file_size, send_size = 0;
    // receive file name
    n = recvfrom(sockfd, buffer, SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
    printf("\n[+] File Name Received: %s\n", filename);
    // get file size
    int fd = open(filename, O_RDONLY);
    struct stat stat_buf;
    fstat(fd, &stat_buf);
    file_size = stat_buf.st_size;
    printf("[+] File size:%ld\n",file_size);
    // send file size
    sendto(sockfd, &file_size, sizeof(file_size), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
        error("[-] File open failed!\n");
    // start clock()
    clock_t start_time = clock();
    while (1) {
        /* ACK */
        // ret = recvfrom(sockfd, ack, 4, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
        // ack[ret] = '\0';
        
        /* sleep */
        // usleep(1);
        
        memset(buffer, 0, sizeof(buffer));
        // process file, if fp == EOF break
        if (sendFile(fp, buffer, buffer_SIZE, &send_size)) {
            sendto(sockfd, buffer, buffer_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
            
            time(&cur_time);
            strftime(timebuf, 80, "%Y/%m/%d %X", localtime(&cur_time));
            printf("[Info]:%d%% %s\n\n", 100, timebuf);
            break;
         }
        // send
        sendto(sockfd, buffer, buffer_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
        if(send_size >= (file_size/4)*(_25)){
            printf("[Info] Data received:%ld/%ld\n",send_size,file_size);
            time(&cur_time);
            strftime(timebuf, 80, "%Y/%m/%d %X", localtime(&cur_time));
            printf("[Info]:%d%% %s\n\n", (_25)*25, timebuf);
            (_25)++;
        }
    }
    // end clock()
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time);
    printf("[+] Total tran time: %f ms\n",time_spent);
    printf("[+] Total tran time: %f s\n", (time_spent / CLOCKS_PER_SEC));
    printf("[+] FIle Size:%ld MB\n", (file_size/1024)/1024);
    if (fp != NULL)
        fclose(fp);
}

void udpGetFile(int sockfd, struct sockaddr_in addr_con, int addrlen){
    long file_size, recv_size = 0;
    char buffer[SIZE];
    int n;
    sendto(sockfd, "ACK", sizeof("ACK"), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    recvfrom(sockfd, &file_size, sizeof(file_size), sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
    printf("[+] file size:%ld\n", file_size);
    while (1) {
        // receive
        // sendto(sockfd, UDP_ACK, 4, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
        memset(buffer, 0, sizeof(buffer));
        n = recvfrom(sockfd, buffer, buffer_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
        // process File
        if (recvFile(buffer, buffer_SIZE, &recv_size)) {
            break;
        }
    }
    printf("\n[+] Total data received:%ld\n[+] File size:%ld\n", recv_size, file_size);
    printf("[+] Packet Loss Rate(loss data size/total size):%lf\n", (((double)(file_size-recv_size))/(double)file_size));
    sendto(sockfd, "OK", sizeof("OK"), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
}

int main(int argc, char *argv[]){
    if (argc < 2) {
        error("[-] ERROR, protocol OR port not provided\n");
    }
    char* protocol = argv[1];   // TCP or UDP
    char* role = argv[2];       // send or recv
    char* ip = argv[3];          // ip
    int portno = atoi(argv[4]);   // port
    int sockfd, newsockfd;        // Socket 
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    // TCP client usage
    struct hostent *server;
    if(strcmp(protocol,"tcp")== 0){
        /*  
            建立TCP socket
            *   AF_INET，表示此Socket目的是Hosts之間的連線，且是採用IPv4。
            *   SOCK_STREAM，表示此Socket提供的是Connection-Oriented Byte-Stream，屬於TCP
            *   protocol：	0，讓kernel選擇type對應的默認協議。
            *   Socket物件若建立失敗會回傳-1，表示INVALID_SOCKET 
        */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            error("[-] ERROR opening socket\n");
        // 利用bzero初始化，將struct sockaddr_in serv_addr涵蓋的bits設為0
        bzero((char *) &serv_addr, sizeof(serv_addr));
        if(strcmp(role,"send")== 0){
            /* TCP Sender */
            filename = argv[5];
            /*  
                設定Server socket
                *   AF_INET:serv_addr為IPv4結構
                *   當process想要建立連線或接收新封包，process需要透過bind()來綁定local interface的address
                    對於Server而言，若不希望只是綁定localhost，則設此值為INADDR_ANY，如此Socket就會bind這台Host上所有ip
                    懶人包：我不在乎local IP是什麼，讓kernel替我決定就好。
                *   htons()將Endian調整成network endian(little endian)
                *   serv_addr的port為傳進來的port number
            */
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(portno);
            /*
                將剛剛設定的server bind到socket上
                *   透過bind()將剛剛的ip address綁到socket上
                *   若bind失敗會回傳-1，若成功則回傳0
            */
            if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                error("[-] ERROR on binding\n");
            listen(sockfd,5);
            clilen = sizeof(cli_addr);
            // get client socket File Descriptor
            newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
            if (newsockfd < 0)
                error("[-] ERROR on accept\n");

            tcpSendFile(newsockfd);

            /* close Client File Descriptor*/
            close(newsockfd);
        }
        else if(strcmp(role,"recv")== 0){
            /* TCP Receiver */
            server = gethostbyname(ip);
            if(server == NULL)
                error("[-] ERROR, no such host\n");
            /*  
                設定Server socket
                *   AF_INET:serv_addr為IPv4結構
                *   利用bcopy()將執行參數的ip轉為socket的ip可讀型態
                *   htons()將Endian調整成network endian(little endian)
                *   serv_addr的port為執行參數傳進來的port number
            */
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
            serv_addr.sin_port = htons(portno);
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
                error("[-] ERROR connecting");

            tcpGetFile(sockfd);
        }
        /* close File Descriptor*/
        close(sockfd);
    }
    else if(strcmp(protocol,"udp")== 0){
        int addrlen = sizeof(serv_addr);
        /*  
            建立TCP socket
            *   AF_INET，表示此Socket目的是Hosts之間的連線，且是採用IPv4。
            *   SOCK_STREAM，表示此Socket提供的是Connection-Oriented Byte-Stream，屬於TCP
            *   protocol：	0，讓kernel選擇type對應的默認協議。
            *   Socket物件若建立失敗會回傳-1，表示INVALID_SOCKET 
        */
        sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
        if (sockfd < 0)
            error("[-] file descriptor not received!!\n");
        if(strcmp(role,"send")== 0){
            /* UDP Sender */
            filename = argv[5];
            /*  
                設定Server socket
                *   AF_INET:serv_addr為IPv4結構
                *   INADDR_ANY:讓kernel決定local IP。
                *   htons()將Endian調整成network endian(little endian)
                *   serv_addr的port為傳進來的port number
            */
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(portno);
            /*
                將剛剛設定的server bind到socket上
                *   透過bind()將剛剛的ip address綁到socket上
                *   若bind失敗會回傳-1，若成功則回傳0
            */
            if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
                error("[-] ERROR on binding\n");
            udpSendFile(sockfd, serv_addr, addrlen);
        }
        else if(strcmp(role,"recv")== 0){
            /* UDP Receiver */
            /*  
                設定Server socket
                *   AF_INET:serv_addr為IPv4結構
                *   INADDR_ANY:讓kernel決定local IP。
                *   htons()將Endian調整成network endian(little endian)
                *   serv_addr的port為傳進來的port number
            */
            serv_addr.sin_family = AF_INET;
            // serv_addr.sin_addr.s_addr = inet_addr(LOCAL_ADDRESS);
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(portno);
            udpGetFile(sockfd, serv_addr, addrlen);
        }
        close(sockfd);
    }
    return 0;
}