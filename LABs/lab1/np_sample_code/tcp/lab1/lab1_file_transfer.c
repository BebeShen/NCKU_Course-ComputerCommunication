#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void tcp_send(){

}

void tcp_recv(){

}

void udp_send(){

}

void udp_recv(){

}

int main(int argc, char *argv[]){
    char protocol[3] = argv[1];   // TCP or UDP
    char role[5] = argv[2];       // send or recv
    if(protocol == "tcp"){
        if(role == "send"){
            tcp_send();
        }
        else if(role == "recv"){
            tcp_recv();
        }
        else{
            printf("Param 3 is send/recvm please check");
        }
    }
    else if(protocol == "udp"){
        if(role == "send"){
            udp_send();
        }
        else if(role == "recv"){
            udp_recv();
        }
        else{
            printf("Param 3 is send/recvm please check");
        }
    }
    else{
        printf("Param 2 is tcp/udp, please check");
    }
    return 0;
}