#include <stdio.h>
#include "../inc/protocol.h"
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


int main(int argc, char *argv[])
{
    int idxArg;
    int sockfd;
    char *ipDst;
    struct addrinfo hints;
    struct addrinfo *pResult;


    sockfd = 0;
    ipDst = NULL;
    idxArg = 1;
    memset(&hints, 0, sizeof(struct addrinfo));



    if (argc > 1)
    {
        ipDst = argv[idxArg];
        while (argv[idxArg] != NULL)
        {
            printf("%s\n", argv[idxArg]);
            ++idxArg;
        }
    }
    
    /* */
}
