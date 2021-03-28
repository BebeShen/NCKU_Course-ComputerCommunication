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
#define SIZE 1024
char filename[50] = "data.txt";

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void getFile(int sockfd){
    long file_size, recv_size = 0;
    int n;
    int _25 = 1;
    char buffer[SIZE];

    // get file size from sender
    n = recv(sockfd, &file_size, sizeof(file_size),0);
    bzero(buffer, SIZE);
    int fp = open(filename, O_CREAT | O_WRONLY, 0644);
    /* time function */
    time_t cur_time;
    char timebuf[80];
    // start clock()
    clock_t start_time = clock();
    // read file from socket
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
    // end clock()
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("[+] Total tran time: %d ms\n", (int)(time_spent*1000));
    printf("[+] FIle Size:%ld MB\n", (file_size/1024)/1024);
}

void sendFile(int sockfd){
    // get file size
    long file_size;
    int fd = open(filename, O_RDONLY);
    struct stat stat_buf;
    fstat(fd, &stat_buf);
    file_size = stat_buf.st_size;
    printf("[+] File size:%ld\n",file_size);
    send(sockfd, &file_size, sizeof(file_size), 0);

    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL)
        error("[-] No such File.\n");
    char* file_content = malloc(stat_buf.st_size);
    fread(file_content, stat_buf.st_size, 1, fp);
    send(sockfd, file_content, stat_buf.st_size, 0);
    free(file_content);
}

void udp_send(){

}

void udp_recv(){

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
    // UDP client usage
    struct hostent *server;
    if(strcmp(protocol,"tcp")== 0){
        // 建立TCP socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            error("[-] ERROR opening socket\n");
        // 利用bzero初始化，將struct sockaddr_in serv_addr涵蓋的bits設為0
        bzero((char *) &serv_addr, sizeof(serv_addr));
        if(strcmp(role,"send")== 0){
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

            sendFile(newsockfd);

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

            getFile(sockfd);
        }
        /* close File Descriptor*/
        close(sockfd);
    }
    else if(strcmp(protocol,"udp")== 0){
        if(strcmp(role,"send")== 0){
            udp_send();
        }
        else if(strcmp(role,"recv")== 0){
            udp_recv();
        }
    }
    return 0;
}