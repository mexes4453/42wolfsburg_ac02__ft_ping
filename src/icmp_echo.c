# include "../inc/icmp_echo.h"



void ICMP_ECHO__Ctor(ICMP_ECHO_t * const me)
{
    memset((void *)me, 0, sizeof(ICMP_ECHO_t));
    //me->ParseFrom = ICMP_ECHO__ParseFrom;
    //me->ShowDetails = ICMP_ECHO__ShowDetails;
    //me->Destroy = ICMP_ECHO__Destroy;
    //me->CreatePacket = ICMP_ECHO__CreatePacket;
    //me->CalcCheckSum = ICMP_ECHO__CalcCheckSum;
}


#if 0
void ICMP_ECHO__ParseFrom( ICMP_ECHO_t *me, char unsigned *buf,
                                                ssize_t bufSz )
{
    int lenData;

    if (!(buf && me && (bufSz >= ICMP_ECHO__HDR_MIN_LEN))) return;

    /*>
     * The first 8 bytes are static. Therefore they can be copied
     * directly to the ip header object */
    memcpy((void *)me, buf, ICMP_ECHO__HDR_MIN_LEN);

    /*>
     * Parse the data 
     * The remaining data in buffer is for data */
    lenData = bufSz - ICMP_ECHO__HDR_MIN_LEN;
    me->pData = (char unsigned *)malloc(lenData * sizeof(char unsigned));
    if (!(me->pData))
    {
        return ;
    }
    memcpy((void *)(me->pData), &(buf[ ICMP_ECHO__HDR_MIN_LEN ]), lenData);
    me->dataLen = lenData;
    me->totalPacketLen = bufSz;
}
#endif







void ICMP_ECHO__Destroy(ICMP_ECHO_t * const me)
{
    if (me == NULL) return;
    if (me->pData)
    {
        XNET_UTILS__Destroy((void **)&(me->pData));
    }
    if (me->pPktChkSum)
    {
        XNET_UTILS__Destroy((void **)&(me->pPktChkSum));
    }
}




/* consider dynamic payload length */
int  ICMP_ECHO__CreatePacket(ICMP_ECHO_t * const me, 
                               ssize_t datalen,
                               short unsigned id,
                               short unsigned seqNbr)
{
    int retCode = ICMP_ECHO__enRetCode_CreatePacket_Failed;

    /* create the message to send */
    me->icmp.type = XPROTO_ICMP__enType_Echo;
    me->icmp.code = XPROTO_ICMP__enCode_Echo;
    me->icmp.checksum = 0;
    me->identifier = id;
    me->seqnbr = seqNbr;
    me->dataLen = datalen ? datalen : ICMP_ECHO__MIN_DATA_SZ;
    me->hdrLen = ICMP_ECHO__STATIC_HDR_SZ;
    me->totalPacketLen = me->hdrLen + me->dataLen;
    me->pData = (char unsigned *)malloc(me->dataLen);
    XNET_UTILS__ASSERT_UPD_REDIRECT(
                             me->pData, 
                             &retCode,
                             ICMP_ECHO__enRetCode_CreatePacket_MallocDataFailed,
                             labelExit);

     /* fill up data with value based on msg type*/
    memset((void *)me->pData, 'a', me->dataLen);


    /* Calculate and update the checksum */
    ICMP_ECHO__CalcCheckSum(me);

    retCode = EXIT_SUCCESS;
labelExit:
#ifdef XNET__DEBUG
    printf("%s - retcode(%d)\n", ICMP_ECHO_D_CREATE_PACKET, retCode);
    printf("%s - icmp datalen(%ld)\n", ICMP_ECHO_D_CREATE_PACKET, me->totalPacketLen);
    XNET_UTILS__ShowPacketHex(me->pPktSerial, me->totalPacketLen);
    ICMP_ECHO__ShowDetails(me);
#endif
    return (retCode);
}




#if 1
void ICMP_ECHO__ShowDetails(ICMP_ECHO_t * const me)
{
    if (!me) return;

    printf(ICMP_ECHO__MSG_FMT_HDR_ECHO_REPLY, me->icmp.type, 
                                 me->icmp.code, me->icmp.code,
                                 me->icmp.checksum,     /* hex */
                                 me->identifier,    /* hex */
                                 me->identifier,    /* dec */ 
                                 me->seqnbr,       /* hex */
                                 me->seqnbr,        /* dec */
                                 me->dataLen
                                );
    printf("\n");

}

#endif


int ICMP_ECHO__CalcCheckSum(ICMP_ECHO_t * const me)
{
    short unsigned result = 0;
    char unsigned *pCheckSumData;
    int retCode = ICMP_ECHO__enRetCode_CalcCheckSum_Failed;
    me->totalPacketLen = ICMP_ECHO__STATIC_HDR_SZ + me->dataLen;
    
    /* Discard data if it exists */
    if ( !XNET_UTILS__IsNullPtr((void **)&(me->pPktChkSum)))
    {
        XNET_UTILS__Destroy((void **)&(me->pPktChkSum));
    }

    /* remove existing checksum value */

    me->pPktChkSum = (char unsigned *)malloc(me->totalPacketLen);
    XNET_UTILS__ASSERT_UPD_REDIRECT(
                             me->pPktChkSum, 
                             &retCode,
                             ICMP_ECHO__enRetCode_CalcCheckSum_MallocDataFailed,
                             labelExit);
    pCheckSumData = me->pPktChkSum;
    memset((void *)pCheckSumData, 0, me->totalPacketLen);

    /* Copy header */
    memcpy((void *)pCheckSumData, (void *)me, ICMP_ECHO__STATIC_HDR_SZ);
    memcpy((void *)(pCheckSumData + ICMP_ECHO__STATIC_HDR_SZ), (void *)me->pData, me->dataLen);

    /* clear existing checksum - starts at index 2*/
    *((uint16_t *)(pCheckSumData + 2)) = 0;


    result = XNET_UTILS__CalcCheckSum16((void *)pCheckSumData,
                                            me->totalPacketLen,
                                            XNET_UTILS__enEndianType_Host);
    me->icmp.checksum = result;
    *((uint16_t *)(pCheckSumData + 2)) = result;

    retCode = EXIT_SUCCESS;
labelExit:
    return (result);
}




int ICMP_ECHO__ValidateRxPkt(ICMP_ECHO_t * const me, char unsigned *buf, ssize_t bufSz)
{
    int retCode = ICMP_ECHO__enRetCode_ValidateRxPkt_Failed;

    XNET_UTILS__ASSERT_UPD_REDIRECT((me && buf), &retCode,
        ICMP_ECHO__enRetCode_ValidateRxPkt_NullPtrArgs,
        labelExit);
    
    retCode = ICMP_ECHO__ParseFromNet(me, buf, bufSz);
    XNET_UTILS__ASSERT_UPD_REDIRECT((me && buf), &retCode,
        ICMP_ECHO__enRetCode_ValidateRxPkt_ParseFromNetFailed,
        labelExit);

    XNET_UTILS__ASSERT_UPD_REDIRECT( 
        (XPROTO_ICMP__GetType((XPROTO_ICMP_t *)me) == XPROTO_ICMP__enType_EchoReply),
        &retCode,
        ICMP_ECHO__enRetCode_ValidateRxPkt_InvalidPktType,
        labelExit);


    retCode = ICMP_ECHO__IsCheckSumValid(me);

#ifdef ICMP_ECHO__DEBUG
    printf("%s_S\n", ICMP_ECHO__D_VALIDATE_RX_PKT);
    XNET_UTILS__ShowPacketHex(buf, bufSz);
    ICMP_ECHO__ShowDetails(me);
    printf("\nretcode( %d )", retCode);
    printf("%s_E\n", ICMP_ECHO__D_VALIDATE_RX_PKT);
#endif
labelExit:
    return(retCode);
}



int ICMP_ECHO__ParseFromNet(ICMP_ECHO_t * const me, char unsigned *buf, ssize_t bufSz)
{
    int retCode = ICMP_ECHO__enRetCode_ParseFromNet_Init;
    me->icmp.type = buf[0];
    me->icmp.code = buf[1];
    me->icmp.checksum = ntohs(*((uint16_t *)(buf + 2)));
    me->identifier = ntohs(*((uint16_t *)(buf + 4)));
    me->seqnbr = ntohs(*((uint16_t *)(buf + 6)));
    me->dataLen = bufSz - ICMP_ECHO__STATIC_HDR_SZ;

    if ( !XNET_UTILS__IsNullPtr((void **)&(me->pData)) )
    {
        XNET_UTILS__Destroy((void **)&(me->pData));
    }
    /* create new data from packet */
    me->pData = (char unsigned *)malloc(me->dataLen);
    XNET_UTILS__ASSERT_UPD_REDIRECT( me->pData, &retCode,
        ICMP_ECHO__enRetCode_ParseFromNet_PdataMallocFailed,
        labelExit);
    
    memset((void *)me->pData, 0, me->dataLen);
    memcpy((void *)me->pData, (buf + ICMP_ECHO__STATIC_HDR_SZ), me->dataLen);

    retCode = EXIT_SUCCESS;
labelExit:
    return (retCode);
}




int ICMP_ECHO__IsCheckSumValid(ICMP_ECHO_t * const me)
{
    int retCode = ICMP_ECHO__enRetCode_IsCheckSumValid_Init;
    short unsigned checkSumReceived = me->icmp.checksum;
    short unsigned checkSumCalculated = ICMP_ECHO__CalcCheckSum(me);

    if (checkSumCalculated == checkSumReceived)
    {
        retCode = EXIT_SUCCESS;
    }
#ifdef ICMP_ECHO__DEBUG
    printf("%s__S", ICMP_ECHO__D_IS_CHECKSUM_VALID);
    printf("\ncalc: %04x => default: 0x%04x", checkSumCalculated, checkSumReceived);
    printf("%s__E", ICMP_ECHO__D_IS_CHECKSUM_VALID);
#endif
    return (retCode);

}






