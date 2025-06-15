
#include "../inc/xapp.h"

static XAPP_t XAPP__var;

XAPP_t *XAPP__GetInstance(void)
{
    return (&XAPP__var);
}

int XAPP__Ctor(XAPP_t * const me, char const * const strIpAddr)
{
    int retCode = 0;

    memset( (void *)me, 0, sizeof(XAPP_t));
    me->dstAddrLen = sizeof(struct sockaddr_in);

    /* Initialise hints attribute */
    XNET__InitAddrInfo(&(me->hints), AF_INET, SOCK_RAW, IPPROTO_ICMP, AI_CANONNAME);

    /* setup and fetch addr info */
    retCode = getaddrinfo(strIpAddr, NULL, &(me->hints), &(me->pAddrInfo));
    if (retCode != 0)
    {
        fprintf(stderr, "%s\n", gai_strerror(retCode));
    }
    else
    {
#ifdef XAPP_DEBUG
        XNET__ShowAddrInfo(me->pAddrInfo);
#endif
        printf(XAPP__INFO_PING_START, me->pAddrInfo->ai_canonname,
                                      me->pAddrInfo->ai_canonname,
                                      XAPP__DEF_ICMP_DATA_SIZE);
    }

    /* Set the process id to server as all packet id */
    me->pid = getpid() & 0xFFFF;

    
    /* create socket */
    me->sockfd = XNET__CreateSocket(me->pAddrInfo);
    XNET_UTILS__ASSERT_UPD_REDIRECT((me->sockfd), 
                                    &retCode,
                                    XAPP__enRetCode_Ctor_CreateSockFailed,
                                    labelExit);

    /*>
     * Set the number of file descriptors  
     * This application uses only one file descriptor, which is the only socket
     * that will be required. */
    me->nfds = XAPP__SOCKFD_MAX_NBR;


    /*> Initialise the file descriptor poll array  */
    me->fds->fd = me->sockfd;
    me->fds->events = POLLIN;






labelExit:
    return (retCode);
}




int  XAPP__CreateIcmpHeader(XAPP_t * const me)
{
    int retCode = XAPP__enRetCode_CreateIcmpHdr_Failed;

    me->pIcmpHdr = malloc(sizeof(XPROTO_ICMP_t));
    XNET_UTILS__ASSERT_UPD_REDIRECT((me->pIcmpHdr), 
                                    &retCode,
                                    XAPP__enRetCode_CreateIcmpHdr_MallocFailed,
                                    labelExit);
    XPROTO_ICMP__Ctor(me->pIcmpHdr);
    retCode = EXIT_SUCCESS;
labelExit:
    return (retCode);
}




int  XAPP__CreateIcmpPayload(XAPP_t * const me)
{
    int retCode = XAPP__enRetCode_CreateIcmpPayload_Failed;

    me->payloadLen = XAPP__DEF_ICMP_DATA_SIZE;
    me->pucPayload = malloc(me->payloadLen);
    XNET_UTILS__ASSERT_UPD_REDIRECT((me->pucPayload), 
                                    &retCode,
                                    XAPP__enRetCode_CreateIcmpHdr_MallocFailed,
                                    labelExit);

    /*> Initialise payload array by setting all bytes to zero */
    memset((void *)(me->pucPayload), 0, me->payloadLen);

    retCode = EXIT_SUCCESS;
labelExit:
    return (retCode);

}









/* consider dynamic payload length */
int XAPP__CreateIcmpPacket(XAPP_t * const me, XPROTO_ICMP__eMsgType_t msgType)
{
    int retCode;

    /* create a icmp header */
    retCode = XAPP__CreateIcmpHeader(me);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == EXIT_SUCCESS), 
                                    &retCode,
                                    retCode, /* retransmit the same error */
                                    labelExit);

    /* create icmp payload base on parsed argument */
    retCode = XAPP__CreateIcmpPayload(me);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == EXIT_SUCCESS), 
                                    &retCode,
                                    retCode, /* retransmit the same error */
                                    labelExit);


    /* Create icmp packet for transmission */
    retCode = XPROTO_ICMP__CreatePacket(me->pIcmpHdr, msgType,
                                            me->pucPayload,
                                            me->payloadLen,
                                            me->pid,
                                            me->seqNbr);
    retCode = EXIT_SUCCESS;
labelExit:
    return (retCode);
}





void XAPP__TxPacket(XAPP_t * const me) 
{
    /* send data to dest address */
    me->datalenTx = sendto(me->sockfd, (void *)(me->pIcmpHdr->pPktSerial), 
                                       me->pIcmpHdr->totalPacketLen,
                                       0,
                                       me->pAddrInfo->ai_addr,
                                       me->pAddrInfo->ai_addrlen);
    XNET_UTILS__ShowPacketHex((me->pIcmpHdr->pPktSerial), me->pIcmpHdr->totalPacketLen );  
    XPROTO_ICMP__ShowDetails(me->pIcmpHdr);               

    if (me->datalenTx <= 0)
    {
        perror("send error\n");
    }
    else
    {
#ifdef XNET__DEBUG
        printf("success: sent (%ld)\n", me->datalenTx);
        printf("pid: identifier (%x)\n", me->pid);
        printf("pid: checksum (%x)\n", me->pIcmpHdr->checksum);
        printf("sent message type: %d\n", me->pIcmpHdr->type);
#endif  
    }

}
    
void  XAPP__RxPacket(XAPP_t * const me) 
{
    /* receive the response back */
    me->datalenRx  = recvfrom(me->sockfd, me->recvBuf, 
                                      XAPP__RX_BUFSZ, 
                                      0, 
                                      (struct sockaddr *)(&(me->dstAddr)),
                                      &(me->dstAddrLen) );
    if (me->datalenRx  <= 0)
    {
        printf("recv error\n");
    }
    else
    {
#ifdef XNET__DEBUG
        printf("success: recv (%ld)\n", me->datalenRx );
        XNET_UTILS__ShowPacketHex(me->recvBuf, me->datalenRx );
#endif
    }
}



void XAPP__Destroy(XAPP_t * const me)
{
    XNET_UTILS__Destroy((void *)&(me->pucPayload));
    if (me->pIcmpHdr)
    {
       /* Release resource created within the object first */
       me->pIcmpHdr->Destroy(me->pIcmpHdr);

       /* Release the memory for the object */
       XNET_UTILS__Destroy((void *)&(me->pIcmpHdr));
    }

    /* Close the file descriptor for the socket */
    if (me->sockfd)
    {
       close(me->sockfd);
    }

    /* Release the memory for the address information */
    freeaddrinfo(me->pAddrInfo);
}




/*>
 * ----------------------------------------------------------------------------
 * INTERRUPT - SIGNAL HANDLER
 * ----------------------------------------------------------------------------
 * */
void XAPP__SigIntHandler(int sig)
{
    XAPP_t *pAppVar = XAPP__GetInstance();

    if (sig == SIGINT)
    {
        XAPP__Destroy(pAppVar);
        printf("Exiting...\n");
        exit(EXIT_SUCCESS);
    }
}