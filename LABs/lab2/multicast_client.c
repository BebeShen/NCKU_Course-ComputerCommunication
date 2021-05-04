/* Receiver/client multicast Datagram example. */
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

#define NET_BUF_SIZE 512
struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1024];
int _25 = 1;

void checkProcess(long recv_size, long file_size){
    char timebuf[80];
    time_t cur_time;
    if(recv_size >= (file_size/4)*_25){
        printf("[Info] Data received:%ld/%ld\n",recv_size,file_size);
        time(&cur_time);
        strftime(timebuf, 80, "%Y/%m/%d %X", localtime(&cur_time));
        printf("[Info]:%d%% %s\n", _25*25, timebuf);
        _25++;
    }
}
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
            // printf("%c", ch);
            (*size)++;
        }

    }
    return 0;
}

int main(int argc, char *argv[])
{
    /* Create a datagram socket on which to receive. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("[-] Opening datagram socket error");
		exit(1);
	}
	else
	printf("[+] Opening datagram socket....OK.\n");
		 
	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	int reuse = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("[-] Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
		printf("[+] Setting SO_REUSEADDR...OK.\n");
	 
	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
    // localsock_addr的port為4321
	localSock.sin_port = htons(4321);
	localSock.sin_addr.s_addr = INADDR_ANY;
	int localSocklen = sizeof(localSock);
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
	{
		perror("[-] Binding datagram socket error");
		close(sd);
		exit(1);
	}
	else
		printf("[+] Binding datagram socket...OK.\n");
	 
	/* Join the multicast group 226.1.1.1 on the local 10.0.2.15 */
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	// group.imr_interface.s_addr = inet_addr("192.168.32.143");
	group.imr_interface.s_addr = inet_addr("10.0.2.15");
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
		perror("[-] Adding multicast group error");
		close(sd);
		exit(1);
	}
	else
		printf("[+] Adding multicast group...OK.\n");
	printf("------------------------------------------------------\n");
	/* Read filename from the socket. */
	datalen = sizeof(databuf);
	if(read(sd, databuf, datalen) < 0)
	{
		perror("[-] Reading filename message error");
		close(sd);
		exit(1);
	}
	else
	{
		printf("[Info]:Filename is: \"%s\"\n", databuf);
	}
    
    long nBytes, recv_bytes = 0, file_size;
    if(read(sd, &file_size, sizeof(file_size)) < 0)
        printf("[-] Reading filesize fail\n");
    else
        printf("[Info]:file size:%ld\n", file_size);

    // start clock()
    clock_t start_time = clock();
    while (1) {
        // receive
        // clearBuf(net_buf);

        // sendto(sockfd, UDP_ACK, 4, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
        memset(databuf, 0, sizeof(databuf));
        nBytes = read(sd, databuf, datalen);
        // nBytes = recvfrom(sd, databuf, NET_BUF_SIZE, 0, (struct sockaddr*)&localSock, &localSocklen);
		if(nBytes <= 0){
			printf("[-] Connection Interrupt!\n");
			break;
		}
        // recv_bytes += nBytes;
        // printf("[+] Data received:%ld\n[+] Current Data size:%ld\n", nBytes,recv_bytes);
        
        // process File
        if (recvFile(databuf, NET_BUF_SIZE, &recv_bytes)) {
        	checkProcess(recv_bytes, file_size);
            break;
        }
        checkProcess(recv_bytes, file_size);
    }
    // end clock()
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time);

	printf("------------------------------------------------------\n");
    printf("[+] FIle Size:%ld KB\n", file_size/1024);
    printf("[+] Total data received:%ld\n", recv_bytes);
    // printf("[+] Total data received:%ld\n[+] File size:%ld\n", recv_bytes, file_size);
    // printf("[+] Packet Loss Rate(loss data size/total size):%lf\n", (((double)(file_size-recv_bytes))/(double)file_size));
    // printf("[+] Total tran time: %f ms\n",time_spent);
    printf("[+] Total transition time: %f s\n", (time_spent / CLOCKS_PER_SEC));
    // /* Read file from the socket. */
    // memset(databuf, 0, sizeof(databuf));
	// if(read(sd, databuf, datalen) < 0)
	// {
	// 	perror("[-] Reading datagram message error");
	// 	close(sd);
	// 	exit(1);
	// }
	// else
	// {
	// 	printf("[+] Reading datagram message...OK.\n");
	// 	printf("\n[Info] The message from multicast server is: \"%s\"\n", databuf);
	// }
	close(sd);
	return 0;
}
