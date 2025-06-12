#include "../inc/xproto_icmp.h"



void XPROTO_ICMP__Ctor(XPROTO_ICMP_t * const me)
{
    memset((void *)me, 0, sizeof(XPROTO_ICMP_t));
    me->ParseFrom = XPROTO_ICMP__ParseFrom;
    me->ShowDetails = XPROTO_ICMP__ShowDetails;
    me->Destroy = XPROTO_ICMP__Destroy;
    me->CreatePacket = XPROTO_ICMP__CreatePacket;
    me->CalcCheckSum = XPROTO_ICMP__CalcCheckSum;
    me->dstAddrLen = sizeof(struct sockaddr_in);
}



void XPROTO_ICMP__ParseFrom( XPROTO_ICMP_t *me, char unsigned *buf,
                                                ssize_t bufSz )
{
    int lenData;

    if (!(buf && me && (bufSz >= XPROTO_ICMP__HDR_MIN_LEN))) return;

    /*>
     * The first 8 bytes are static. Therefore they can be copied
     * directly to the ip header object */
    memcpy((void *)me, buf, XPROTO_ICMP__HDR_MIN_LEN);

    /*>
     * Parse the data 
     * The remaining data in buffer is for data */
    lenData = bufSz - XPROTO_ICMP__HDR_MIN_LEN;
    me->pData = malloc(lenData * sizeof(char unsigned));
    if (!(me->pData))
    {
        return ;
    }
    memcpy((void *)(me->pData), &(buf[ XPROTO_ICMP__HDR_MIN_LEN ]), lenData);
    me->dataLen = lenData;
    me->totalPacketLen = bufSz;
}




void XPROTO_ICMP__ShowDetails(XPROTO_ICMP_t * const me)
{
    if (!me) return;

    printf(XPROTO_ICMP__MSG_FMT_HDR_ECHO_REPLY, me->type, 
                                 me->code, me->code,
                                 me->checksum,     /* hex */
                                 me->identifier,    /* hex */
                                 me->identifier,    /* dec */ 
                                 me->seqnbr,       /* hex */
                                 me->seqnbr,        /* dec */
                                 me->dataLen
                                );
    printf("\n");

}




void XPROTO_ICMP__Destroy(XPROTO_ICMP_t * const me)
{
    if (me->pData != NULL)
    {
        free(me->pData);
        me->pData = NULL;
    }
}




/* consider dynamic payload length */
int XPROTO_ICMP__CreatePacket(XPROTO_ICMP_t * const me, char unsigned **buf,
                                                         ssize_t datalen,
                                                         short unsigned id,
                                                         short unsigned seqNbr)
{
    ssize_t payloadSize;
    char unsigned *newBuf = NULL;
    int retCode = 0;

    if (!(buf)) 
    {
        retCode = (int)XPROTO_ICMP__enRetCode_CreatePktInvPtr;
        goto labelOnExit;
    }
    payloadSize = (datalen) ? datalen : XPROTO_ICMP__PAYLOAD_SIZE;

    /* create the message to send */
    me->type = XPROTO_ICMP__TYPE_ECHO;
    me->code = XPROTO_ICMP__CODE;
    me->checksum = 0;
    me->identifier = id;
    me->seqnbr = seqNbr;
    me->pData = malloc(payloadSize * sizeof(char unsigned));
    if (!(me->pData))
    {
        retCode = (int)XPROTO_ICMP__enRetCode_CreatePktMallocPtrData;
        goto labelOnExit;
    }

    memset((void *)me->pData, 'a', payloadSize);
    me->dataLen = payloadSize;
    me->totalPacketLen = XPROTO_ICMP__HDR_MIN_LEN + me->dataLen;

    /* Calculate and update the checksum */
    me->checksum = me->CalcCheckSum(me);

    /* Parse to buffer */
    /* create a new buffer */
    newBuf = malloc(me->totalPacketLen * sizeof(char unsigned));
    if (newBuf)
    {
        /* copy header */
        memcpy((void *)newBuf, (void *)me, XPROTO_ICMP__HDR_MIN_LEN);

        /* copy data */
        memcpy((void *)(newBuf + XPROTO_ICMP__HDR_MIN_LEN),
               (void *)me->pData,
               me->dataLen);

        /* update pointer address */
        *buf = newBuf;
    }
    retCode = EXIT_SUCCESS;
labelOnExit:
    /* clean up */
    printf("\nretcode(%d); \n", retCode);
    return (retCode);
}


short unsigned XPROTO_ICMP__CalcCheckSum(XPROTO_ICMP_t * const me)
{
    unsigned char *buf = NULL;
    short unsigned result = 0;
    
    buf = malloc(me->totalPacketLen * sizeof(char unsigned));
    if (buf)
    {
        /* copy header */
        memcpy((void *)buf, (void *)me, XPROTO_ICMP__HDR_MIN_LEN);

        /* copy data */
        memcpy((void *)(buf + XPROTO_ICMP__HDR_MIN_LEN),
               (void *)me->pData,
               me->dataLen);
    
        result = XNET_UTILS__CalcCheckSum16((void *)buf,
                                            me->totalPacketLen,
                                            XNET_UTILS__enEndianType_Host);
        /* Release resources */
        free(buf);
        buf = NULL;
    }
    return (result);





}



#if 1

int  XPROTO_ICMP__SendEchoRequest(XPROTO_ICMP_t * const me, 
                                  struct addrinfo *pAddrInfo)
{
    int retCode = XPROTO_ICMP__enRetCode_Failure;
    char unsigned  *ptrPacket = NULL;
    

    /* create socket */
    me->sockfd = XNET__CreateSocket(pAddrInfo);
    if (!me->sockfd)
    {
        retCode =  XPROTO_ICMP__enRetCode_SendReqEcho_CreateSockFailed;
        goto labelEsc;
    }
    printf("\nSocket exist - (%d)\n", me->sockfd); 


    /* Create icmp packet for transmission */
    retCode = XPROTO_ICMP__CreatePacket(me, &ptrPacket, 0, 0x4321, 1);
    if (retCode != EXIT_SUCCESS)
    {
        retCode =  XPROTO_ICMP__enRetCode_SendReqEcho_CreatePacketFailed;
        goto labelEsc;
    }

    /* print packet content */
    printf("\nretcode(%d); icmp datalen:%ld\n", retCode, me->totalPacketLen);
    XNET_UTILS__ShowPacketHex(ptrPacket, me->totalPacketLen);
    

    /* send data to dest address */
    me->datalenTx = sendto(me->sockfd, (void *)ptrPacket, 
                              me->totalPacketLen,
                              0,
                              pAddrInfo->ai_addr,
                              pAddrInfo->ai_addrlen);

    if (me->datalenTx <= 0)
    {
        perror("send error\n");
    }
    else
    {
        printf("success: sent (%ld)\n", me->datalenTx);
        printf("pid: identifier (%x)\n", me->identifier);
        printf("pid: checksum (%x)\n", me->checksum);
        printf("sent message type: %d\n", me->type);
    }

    
    
    
    /* receive the response back */
    me->datalenRx  = recvfrom(me->sockfd, me->recvBuf, 
                                      XPROTO_ICMP__RX_BUFSZ, 
                                      0, 
                                      (struct sockaddr *)(&(me->dstAddr)),
                                      &(me->dstAddrLen) );
    if (me->datalenRx  <= 0)
    {
        printf("recv error\n");
    }
    else
    {
        printf("success: recv (%ld)\n", me->datalenRx );
        XNET_UTILS__ShowPacketHex(me->recvBuf, me->datalenRx );
    }

labelEsc:
    if (ptrPacket) free(ptrPacket);
    close(me->sockfd);
    return (retCode);
}
#endif