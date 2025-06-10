#include "../inc/xnet.h"

int main(int argc, char *argv[])
{
    int                idxArg;
    int                sockfd;
    int                retCode;
    char               *ipDst __attribute__((unused));
    struct addrinfo    hints;
    struct addrinfo   *pResult;
    char unsigned      *ptrPacket;
    char unsigned      recvBuf[1024];
    XPROTO_ICMP_t      icmpHdr;
    ssize_t            dataSize;
    struct sockaddr_in addr_target;
    socklen_t          addrlen_target;

    /* Initialise the variables */
    sockfd = 0;
    ipDst = NULL;
    idxArg = 1;
    pResult = NULL;
    dataSize = 0;
    ptrPacket = NULL;
    XPROTO_ICMP__Ctor(&icmpHdr);
    memset(recvBuf, 0, 1024); /* clear packet buffer */

    /* setup and fetch addr info */
    XNET__InitAddrInfo(&hints, AF_INET, SOCK_RAW, IPPROTO_ICMP, AI_CANONNAME);
    retCode = getaddrinfo(XNET__ADDR_DST, NULL, &hints, &pResult);
    if (retCode != 0)
    {
        fprintf(stderr, "%s\n", gai_strerror(retCode));
        goto escape;
    }
    XNET__ShowAddrInfo(pResult);

    /* create socket */
    sockfd = XNET__CreateSocket(pResult);
    if (!sockfd) { goto escape; }

    /* Create icmp packet for transmission */
    XPROTO_ICMP__CreatePacket(&icmpHdr, &ptrPacket, 0, 0x4321, 1);

    /* send data to dest address */
    if (pResult) {printf("\nSocket exist - (%d)\n", sockfd); }
    printf("\npacket addr %p", ptrPacket);
    dataSize = sendto(sockfd, (void *)ptrPacket, 
                              icmpHdr.totalPacketLen,
                              0,
                              pResult->ai_addr,
                              pResult->ai_addrlen);

    if (dataSize <= 0)
    {
        perror("send error\n");
    }
    else
    {
        printf("success: sent (%ld)\n", dataSize);
        printf("pid: identifier (%x)\n", icmpHdr.identifier);
        printf("pid: checksum (%x)\n", icmpHdr.checksum);
        printf("sent message type: %d\n", icmpHdr.type);
    }


    /* receive the response back */
    dataSize = 0;
    addrlen_target = sizeof(struct sockaddr_in);
    dataSize = recvfrom(sockfd, recvBuf, 1024, 0, 
                                (struct sockaddr *)&addr_target,
                                &addrlen_target);
    if (dataSize <= 0)
    {
        printf("recv error\n");
    }
    else
    {
        printf("success: recv (%ld)\n", dataSize);
        XNET__ShowRecvdMsg(recvBuf, dataSize);
    }




#if 1
    int opt = 1;
    int optind =1, optopt=1;
    if (argc > 1)
    {
        //ipDst = argv[idxArg];
        while ( (opt = getopt(argc, argv, ":v:x")) != -1) 
        {
            printf("%c - %s %s", opt, argv[optind], argv[optopt]);
            printf("%s\n", argv[idxArg]);
            ++idxArg;
        }
    }
#endif
escape:
    close(sockfd);


    freeaddrinfo(pResult);
    icmpHdr.Destroy(&icmpHdr);
    if (ptrPacket) free(ptrPacket);
    return (retCode);
}
