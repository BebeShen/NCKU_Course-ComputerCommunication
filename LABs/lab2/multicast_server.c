/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#define nofile "File Not Found!"
#define NET_BUF_SIZE 512
struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[1024] = "Multicast test message lol!";
int datalen = sizeof(databuf);
 
int sendFile(FILE* fp, char* buf, int s){
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

int main (int argc, char *argv[ ])
{
    char* filename;
    if(argc < 2){
        printf("[-] Insufficient Argument\n");
        exit(1);
    }
    filename = argv[1];
    printf("[+] Filename:%s\n",filename);
    long file_size;
    int fd = open(filename, O_RDONLY);
    struct stat stat_buf;
    fstat(fd, &stat_buf);
    file_size = stat_buf.st_size;
    printf("[+] File size: %ld bytes\n",file_size);
	/* Create a datagram socket on which to send. */
	// int socket(int domain, int type, int protocol);
	// 建立名為sd的socket()物件
	// - domain：	AF_INET，表示此Socket目的是Hosts之間的連線，且是採用IPv4。
	// - type：		SOCK_DGRM，表示此Socket的傳輸方式是Datagram，屬於UDP。
	// - protocol：	0，讓kernel選擇type對應的默認協議。
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	// Socket物件若建立失敗會回傳-1，表示INVALID_SOCKET
	if(sd < 0)
	{
	  perror("[-] Opening datagram socket error");
	  exit(1);
	}
	else	// Socket建立成功
	  printf("[+] Opening the datagram socket...OK.\n");
	 
	/* Initialize the group sockaddr structure with a */
	/* group address of 225.1.1.1 and port 4321. */
	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
    // serv_addr的port為4321
	groupSock.sin_port = htons(4321);
	
	
	// Disable loopback so you do not receive your own datagrams.
	
	// char loopch = 0;
	// if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
	// {
	// 	perror("Setting IP_MULTICAST_LOOP error");
	// 	close(sd);
	// 	exit(1);
	// }
	// else
	// 	printf("Disabling the loopback...OK.\n");
	
	
	 
	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	// localInterface.s_addr = inet_addr("192.168.32.143");
	localInterface.s_addr = inet_addr("10.0.2.15");
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0){
	// if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&localInterface, sizeof(localInterface)) < 0){
	  perror("Setting local interface error");
	  exit(1);
	}
	else
	  printf("[+] Setting the local interface...OK\n");

	/* Send a message to the multicast group specified by the*/
	/* groupSock sockaddr structure. */
	/*int datalen = 1024;*/
    // Send file name
	if(sendto(sd, filename, sizeof(filename), 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0){
        perror("[-] Sending filename message error");
    }
    else
        printf("[+] Sending filename \"%s\"\n",filename);

    // Send file size
    sendto(sd, &file_size, sizeof(file_size), 0, (struct sockaddr*)&groupSock, sizeof(groupSock));

    // Send file
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
        printf("[-] File open failed!\n");
    while (1){
        memset(databuf, 0, sizeof(databuf));
		// usleep(1);
        if (sendFile(fp, databuf, NET_BUF_SIZE)) {
            sendto(sd, databuf, NET_BUF_SIZE, 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
            break;
        }
        sendto(sd, databuf, NET_BUF_SIZE, 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
    }
    
    close(sd);
	printf("[+] Close Socket\n");
	return 0;
}