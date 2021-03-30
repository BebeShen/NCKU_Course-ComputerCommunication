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
    int fp = open("output.txt", O_CREAT | O_WRONLY, 0644);
    // read file from socket
    while( (n = read(sockfd,buffer,sizeof(buffer))) > 0){
        write(fp, buffer, n);
        // recv_size += n;
        // if(recv_size >= (file_size/4)*_25){
        //     printf("[Info] Data received:%ld/%ld\n",recv_size,file_size);
        //     time(&cur_time);
        //     strftime(timebuf, 80, "%Y/%m/%d %X", localtime(&cur_time));
        //     printf("[Info]:%d%% %s\n\n", _25*25, timebuf);
        //     _25++;
        // }
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
    
    // FILE *fp;
    // fp = fopen(filename, "r");
    // if (fp == NULL)
    //     error("[-] No such File.\n");
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
    // char* file_content = malloc(stat_buf.st_size);
    // fread(file_content, stat_buf.st_size, 1, fp);
    // send(sockfd, file_content, stat_buf.st_size, 0);
    // free(file_content);
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
    printf("\n[+] File Name Received: %s\n", buffer);
    // get file size
    int fd = open(buffer, O_RDONLY);
    struct stat stat_buf;
    fstat(fd, &stat_buf);
    file_size = stat_buf.st_size;
    printf("[+] File size:%ld\n",file_size);
    // send file size
    sendto(sockfd, &file_size, sizeof(file_size), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    FILE* fp = fopen(buffer, "r");
    if (fp == NULL)
        error("[-] File open failed!\n");
    while (1) {
        /* ACK */
        // ret = recvfrom(sockfd, ack, 4, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
        // ack[ret] = '\0';
        
        /* sleep */
        // usleep(1);
        memset(buffer, 0, sizeof(buffer));
        // process file, if fp == EOF break
        if (sendFile(fp, buffer, buffer_SIZE)) {
            sendto(sockfd, buffer, buffer_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
            break;
         }
        // send
        sendto(sockfd, buffer, buffer_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    }
    
    if (fp != NULL)
        fclose(fp);
}

void checkProcess(long recv_size, long file_size, int* _25){
    char timebuf[80];
    time_t cur_time;
    if(recv_size >= (file_size/4)*(*_25)){
        printf("[Info] Data received:%ld/%ld\n",recv_size,file_size);
        time(&cur_time);
        strftime(timebuf, 80, "%Y/%m/%d %X", localtime(&cur_time));
        printf("[Info]:%d%% %s\n\n", (*_25)*25, timebuf);
        (*_25)++;
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

void udpGetFile(int sockfd, struct sockaddr_in addr_con, int addrlen){
    long file_size, recv_size = 0;
    char buffer[SIZE];
    int n, _25 = 1;;
    sendto(sockfd, filename, sizeof(filename), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    recvfrom(sockfd, &file_size, sizeof(file_size), sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
    printf("[+] file size:%ld\n", file_size);
    // start clock()
    clock_t start_time = clock();
    while (1) {
        // receive
        // sendto(sockfd, UDP_ACK, 4, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
        memset(buffer, 0, sizeof(buffer));
        n = recvfrom(sockfd, buffer, buffer_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
        // process File
        if (recvFile(buffer, buffer_SIZE, &recv_size)) {
            break;
        }
        checkProcess(n, file_size, &_25);
    }
    // end clock()
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time);
    printf("\n[+] Total data received:%ld\n[+] File size:%ld\n", recv_size, file_size);
    printf("[+] Packet Loss Rate(loss data size/total size):%lf\n", (((double)(file_size-recv_size))/(double)file_size));
    printf("[+] Total tran time: %f ms\n",time_spent);
    printf("[+] Total tran time: %f s\n", (time_spent / CLOCKS_PER_SEC));
    printf("[+] FIle Size:%ld MB\n", (file_size/1024)/1024);
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
        // 建立TCP socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            error("[-] ERROR opening socket\n");
        // 利用bzero初始化，將struct sockaddr_in serv_addr涵蓋的bits設為0
        bzero((char *) &serv_addr, sizeof(serv_addr));
        if(strcmp(role,"send")== 0){
            filename = argv[5];
            /* TCP Sender */
            // serv_addr為IPv4結構
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(portno);
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
        // socket()
        sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
        if (sockfd < 0)
            error("[-] file descriptor not received!!\n");
        if(strcmp(role,"send")== 0){
            // server socket ip addr settin
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(portno);
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            // bind()
            if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0)
                error("[+] Successfully binded!\n");
            udpSendFile(sockfd, serv_addr, addrlen);
        }
        else if(strcmp(role,"recv")== 0){
            // server socket ip addr setting
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(portno);
            serv_addr.sin_addr.s_addr = inet_addr(LOCAL_ADDRESS);
            udpGetFile(sockfd, serv_addr, addrlen);
        }
    }
    return 0;
}