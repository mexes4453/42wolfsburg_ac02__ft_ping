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
    //XNET_UTILS__Destroy((void *)&(me->pData));
    XNET_UTILS__Destroy((void *)&(me->pPktChkSum));
    XNET_UTILS__Destroy((void *)&(me->pPktSerial));
}




/* consider dynamic payload length */
int  XPROTO_ICMP__CreatePacket(XPROTO_ICMP_t * const me, 
                               XPROTO_ICMP__eMsgType_t msgType,
                               char unsigned *buf,
                               ssize_t datalen,
                               short unsigned id,
                               short unsigned seqNbr)
{
    int retCode = XPROTO_ICMP__enRetCode_CreatePacket_Failed;
    XNET_UTILS__ASSERT_UPD_REDIRECT((buf != NULL), 
                                    &retCode,
                                    XPROTO_ICMP__enRetCode_CreatePacket_InvPtr,
                                    labelExit);

    /* create the message to send */
    me->type = msgType;
    me->code = XPROTO_ICMP__CODE;
    me->checksum = 0;
    me->identifier = id;
    me->seqnbr = seqNbr;
    me->pData = buf;
    me->dataLen = datalen;
    me->hdrLen = XPROTO_ICMP__HDR_MIN_LEN;
    me->totalPacketLen = me->hdrLen + me->dataLen;

    /* fill up payload with data based on msg type*/
    memset((void *)me->pData, 'a', datalen);

    /* Calculate and update the checksum */
    me->CalcCheckSum(me);
    XPROTO_ICMP__CreateSerialPacket(me, &(me->pPktSerial));
    retCode = EXIT_SUCCESS;

labelExit:
#ifdef XNET__DEBUG
    printf("%s - retcode(%d)\n", XPROTO_ICMP_D_CREATE_PACKET, retCode);
    printf("%s - icmp datalen(%ld)\n", XPROTO_ICMP_D_CREATE_PACKET, me->totalPacketLen);
    XNET_UTILS__ShowPacketHex(me->pPktSerial, me->totalPacketLen);
    XPROTO_ICMP__ShowDetails(me);
#endif
    return (retCode);
}





short unsigned XPROTO_ICMP__CalcCheckSum(XPROTO_ICMP_t * const me)
{
    short unsigned result = 0;
    
    if (me->pPktSerial)
    {
        /* do nothing */
    }
    else
    {
        me->pPktSerial = malloc(me->totalPacketLen * sizeof(char unsigned));
    }
    if (me->pPktSerial)
    {
        XPROTO_ICMP__CreateSerialPacket(me, &(me->pPktChkSum));
    
        result = XNET_UTILS__CalcCheckSum16((void *)me->pPktChkSum,
                                            me->totalPacketLen,
                                            XNET_UTILS__enEndianType_Host);
        me->checksum = result;
    }
    return (result);
}




int  XPROTO_ICMP__CreateSerialPacket(XPROTO_ICMP_t * const me, char unsigned **ppPkt)
{
    int retCode = XPROTO_ICMP__enRetCode_CreateSerialPacket_Failed;
    XNET_UTILS__ASSERT_UPD_REDIRECT((me->pData != NULL), 
                                    &retCode,
                                    XPROTO_ICMP__enRetCode_CreateSerialPacket_InvPtr,
                                    labelExit);
    if (*ppPkt)
    {
        /* Data has been serialised */
        free(*ppPkt);
        *ppPkt = NULL;
    }

    *ppPkt = malloc(me->totalPacketLen);
    if (*ppPkt)
    {
        /* copy header */
        memcpy((void *)(*ppPkt), (void *)me, me->hdrLen);

        /* copy data */
        memcpy((void *)(*(ppPkt) + me->hdrLen),
               (void *)me->pData,
               me->dataLen);
    }

    retCode = EXIT_SUCCESS;
labelExit:
    return (retCode);
}



