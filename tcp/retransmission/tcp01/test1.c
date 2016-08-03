/**
 * @file raw_print_pkt.c
 * @brief 
 * @author Airead Fan <fgh1987168@gmail.com>
 * @date 2012/08/22 12:35:22
 */

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

int main(int argc, char *argv[])
{
    int s;
    ssize_t rn;                 /* receive number */
    struct sockaddr_in saddr;
    char packet[4096];
    int count;

    if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
        perror("error:");
        exit(EXIT_FAILURE);
    }

    memset(packet, 0, sizeof(packet));
    socklen_t *len = (socklen_t *)sizeof(saddr);
    int fromlen = sizeof(saddr);
    int opt = 0;

    count = 0;
    while(1) {
        printf("start recv from\n");
        if ((rn = recvfrom(s, (char *)&packet, sizeof(packet), 0,
                           (struct sockaddr *)&saddr, &fromlen)) < 0)
            perror("packet receive error:");
        if (rn == 0) {
            printf("the peer has performed an orderly shutdown\n");
            break;
        }

        printf("[%d] rn = %lu \n", count++, rn);

        if (count == 16) {
            if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) < 0) {
                perror("setsocketopt failed");
            } else {
                fprintf(stdout, "setsocketopt successful\n");
            }
            // int shutdown(int sockfd, int how);
            /* if (shutdown(s, SHUT_RD) < 0) {
             *     perror("shutdown failed");
             * } */
        }
    }

    return 0;
}
