#include "../inc/xproto_icmp.h"



void XPROTO_ICMP__Ctor(XPROTO_ICMP_t * const me)
{
    memset((void *)me, 0, sizeof(XPROTO_ICMP_t));
    me->ParseFrom = XPROTO_ICMP__ParseFrom;
    me->ShowDetails = XPROTO_ICMP__ShowDetails;
    me->Destroy = XPROTO_ICMP__Destroy;
    me->CreatePacket = XPROTO_ICMP__CreatePacket;
    me->CalcCheckSum = XPROTO_ICMP__CalcCheckSum;

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
labelOnExit:
    /* clean up */
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