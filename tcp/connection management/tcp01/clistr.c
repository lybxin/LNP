#include "../common/common.h"

#define MAXLINE 1024

ssize_t Readline(int fd,void *vptr, size_t maxlen);

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE],recvline[MAXLINE];
    int sendlen, Readlen;
    //int i=0;

    while (fgets(sendline, MAXLINE, fp) != NULL){
        sendlen = strlen(sendline);
        
        if (sendlen == 1){
            Close(sockfd);
            exit(0);
        }
        
        //printf("before send %s sendlen=%d\n",sendline,sendlen);
/*        for(i = 0; i < sendlen; i++){
            putchar(sendline[i]);
            printf(":%d\n",(int)sendline[i]);
        }
*/
        Write(sockfd,sendline,sendlen);
        printf("send %d bytes\n",sendlen);
        if(Readline(sockfd,recvline, MAXLINE) ==0){
            Readlen = strlen(recvline);
            printf("Read %d bytes\n",Readlen);
            //printf("%s",recvline);
        }

        fputs(recvline,stdout);
        printf("\n");
    }
}
























