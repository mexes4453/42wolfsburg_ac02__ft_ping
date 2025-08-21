
#include "../inc/xproto_ip.h"


void XPROTO_IP__Ctor(XPROTO_IP_t * const me)
{
    ft_memset((void *)me, 0, sizeof(XPROTO_IP_t));
    me->ShowDetails = XPROTO_IP__ShowDetails;
    me->ParseFrom = XPROTO_IP__ParseFrom;
    me->Destroy = XPROTO_IP__Destroy;
    me->CalcCheckSum = XPROTO_IP__CalcCheckSum;
    me->IsCheckSumValid = XPROTO_IP__IsCheckSumValid;
}



int XPROTO_IP__ParseFrom(XPROTO_IP_t * const me, char unsigned *buf,
                                                ssize_t bufSz)
{
    int lenOption;
    int lenData;
    int lenHdr;
    int retCode = EXIT_FAILURE;

    XNET_UTILS__ASSERT_UPD_REDIRECT( 
        (buf && me && (bufSz >= XPROTO_IP__HDR_MIN_LEN)),
        &retCode, 
        XPROTO_IP__enRetCode_ParseFrom_InvFrameLen,
        labelExit);

    /*>
     * The first 20 bytes are static. Therefore they can be copied
     * directly to the ip header object */
    ft_memcpy((void *)me, buf, XPROTO_IP__HDR_MIN_LEN);
    lenHdr = (me->hdrLen * 4);
    
    /*>
     * Parse the option.
     * Note that hdrLen = 5 means there is no option.  */
    lenOption =  lenHdr - XPROTO_IP__HDR_MIN_LEN;
    if (lenOption)
    {
        me->pOption = (char unsigned *)malloc(lenOption * sizeof(char unsigned));
        XNET_UTILS__ASSERT_UPD_REDIRECT( me->pOption,
            &retCode, 
            XPROTO_IP__enRetCode_ParseFrom_OptionMallocFailed,
            labelExit);
        /* copying continues from the 20th byte from the buf */
        ft_memcpy((void *)(me->pOption), 
               &(buf[XPROTO_IP__HDR_MIN_LEN]),
               lenOption);
    }

    /*>
     * Parse the data 
     * The remaining data in buffer is for data */
    lenData = bufSz - lenHdr;
    me->pData = (char unsigned *)malloc(lenData * sizeof(char unsigned));
    XNET_UTILS__ASSERT_UPD_REDIRECT( me->pData,
            &retCode, 
            XPROTO_IP__enRetCode_ParseFrom_DataMallocFailed,
            labelExit);
    ft_memcpy((void *)(me->pData), &(buf[ lenHdr ]), lenData);
    me->dataLen = lenData;
    retCode = EXIT_SUCCESS;
labelExit:
    return(retCode);
}




void XPROTO_IP__ShowDetails(XPROTO_IP_t * const me)
{
    if (!me) return;

    printf(XPROTO_IP__MSG_FMT_HDR, me->ver_hdrLen, 
                                 me->ver,
                                 me->hdrLen,
                                 me->tos, me->tos,
                                 htons(me->totalLength), /* hex */
                                 htons(me->totalLength), /* dec */
                                 htons(me->id),          /* hex */
                                 htons(me->id),          /* dec */ 
                                 htons(me->flag_fragOffset),
                                 me->flag,
                                 me->fragOffset,
                                 me->ttl, me->ttl,
                                 me->protocol,
                                 htons(me->hdrChkSum),
                                 htonl(me->addrSrc),
                                 htonl(me->addrDst)
                                );
    XNET_UTILS__ShowAddrIpv4(htonl(me->addrSrc));
    XNET_UTILS__ShowAddrIpv4(htonl(me->addrDst));
    printf("\n");
}




void XPROTO_IP__Destroy(XPROTO_IP_t * const me)
{
    if (me == NULL) return ;

    if (me->pData != NULL)
    {
        XNET_UTILS__Destroy((void **)&(me->pData));
    } 

    if (me->pOption != NULL) 
    {
        XNET_UTILS__Destroy((void **)&(me->pOption));
    } 

    if (me->pOption != NULL) 
    {
        XNET_UTILS__Destroy((void **)&(me->pPktChkSum));
    } 
}




short unsigned XPROTO_IP__CalcCheckSum(XPROTO_IP_t * const me)
{
    ssize_t     bufSz = 0;
    ssize_t     lenOption = 0;         
    short unsigned res = 0;
    
    /*>
     * Copy the original checksum value for temorary storage */
    res = me->hdrChkSum;

    /*>
     * Clear the current checksum in the header object.
     * This will be used to create the data for checksum computation.
     * This assumes that the checksum value is unknown  */
    me->hdrChkSum = 0;

    /*>
     * Determine the amount of buffer to create
     * ---
     * mininum header size (static:20) + option size (varies) */
    bufSz = me->hdrLen * 4;
    lenOption = bufSz - XPROTO_IP__HDR_MIN_LEN;

    /* Create the buffer */
    me->pPktChkSum = (char unsigned *)malloc(bufSz * sizeof(char unsigned));
    if (me->pPktChkSum) 
    {
        /* Copy the static header into the buffer */
        ft_memcpy((void *)me->pPktChkSum, (void *)me, XPROTO_IP__HDR_MIN_LEN);
#ifdef XPROTO_IP__DEBUG
        for (int x=0; x<bufSz; x++)
        {
            printf("\nIP_raw_data: (%d) -> 0x%02x", x, me->pPktChkSum[x]);
        }
#endif

        /* Copy the options if available */
        if (lenOption && me->pOption)
        {
            ft_memcpy((void *)(me->pPktChkSum + XPROTO_IP__HDR_MIN_LEN),
                   (void *)(me->pOption),
                   lenOption);
        }

        /*>
         * Restore original checksum before computation as the data required
         * has been created */
        me->hdrChkSum = res;

        res = XNET_UTILS__CalcCheckSum16((void *)me->pPktChkSum, bufSz, XNET_UTILS__enEndianType_Network);
#ifdef XPROTO_IP__DEBUG
        printf("\nThe ip checksum: 0x%04x\n", res);
#endif
        free(me->pPktChkSum); /*  Release resource */
    }
    else
    {
        res = 0xFFFF;
    }

    return (res);
}



XPROTO_IP__retCode_t XPROTO_IP__IsCheckSumValid(XPROTO_IP_t * const me)
{
    XPROTO_IP__retCode_t retCode = XPROTO_IP__enRetCode_InvalidCheckSum;
    short unsigned checkSumCalculated = XPROTO_IP__CalcCheckSum(me);
    short unsigned checkSumReceived = ntohs(me->hdrChkSum);

    if (checkSumCalculated == checkSumReceived)
    {
        retCode = XPROTO_IP__enRetCode_Success;
    }
#ifdef XPROTO_IP__DEBUG
    printf("\ncalc: %04x => default: 0x%04x", checkSumCalculated, checkSumReceived);
#endif
    return retCode;
}




