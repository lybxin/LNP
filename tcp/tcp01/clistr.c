#include<stdio.h>
#include<string.h>
#include<unistd.h>
//#include<readline/readline.h>
#include<stdlib.h>


#define MAXLINE 1024

ssize_t readline(int fd,void *vptr, size_t maxlen);

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE],recvline[MAXLINE];
    int sendlen, readlen;
    int i=0;

    while (fgets(sendline, MAXLINE, fp) != NULL){
        sendlen = strlen(sendline);
        
        if (sendlen == 1){
            close(sockfd);
            exit(0);
        }
        
        //printf("before send %s sendlen=%d\n",sendline,sendlen);
/*        for(i = 0; i < sendlen; i++){
            putchar(sendline[i]);
            printf(":%d\n",(int)sendline[i]);
        }
*/
        write(sockfd,sendline,sendlen);
        printf("send %d bytes\n",sendlen);
        if(readline(sockfd,recvline, MAXLINE) ==0){
            readlen = strlen(recvline);
            printf("read %d bytes\n",readlen);
            //printf("%s",recvline);
        }

        fputs(recvline,stdout);
        printf("\n");
    }
}
























