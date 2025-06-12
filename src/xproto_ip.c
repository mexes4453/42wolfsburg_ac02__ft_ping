
#include "../inc/xproto_ip.h"


void XPROTO_IP__Ctor(XPROTO_IP_t * const me)
{
    memset((void *)me, 0, sizeof(XPROTO_IP_t));
    me->ShowDetails = XPROTO_IP__ShowDetails;
    me->ParseFrom = XPROTO_IP__ParseFrom;
    me->Destroy = XPROTO_IP__Destroy;
    me->CalcCheckSum = XPROTO_IP__CalcCheckSum;
    me->IsCheckSumValid = XPROTO_IP__IsCheckSumValid;
}



void XPROTO_IP__ParseFrom(XPROTO_IP_t * const me, char unsigned *buf,
                                                ssize_t bufSz)
{
    int lenOption;
    int lenData;
    int lenHdr;

    if (!(buf && me && (bufSz >= XPROTO_IP__HDR_MIN_LEN))) return;

    /*>
     * The first 20 bytes are static. Therefore they can be copied
     * directly to the ip header object */
    memcpy((void *)me, buf, XPROTO_IP__HDR_MIN_LEN);
    lenHdr = (me->hdrLen * 4);
    
    /*>
     * Parse the option.
     * Note that hdrLen = 5 means there is no option.  */
    lenOption =  lenHdr - XPROTO_IP__HDR_MIN_LEN;
    if (lenOption)
    {
        me->pOption = malloc(lenOption * sizeof(char unsigned));
        if (!(me->pOption))
        {
            return ;
        }
        /* copying continues from the 20th byte from the buf */
        memcpy((void *)(me->pOption), 
               &(buf[XPROTO_IP__HDR_MIN_LEN]),
               lenOption);
    }

    /*>
     * Parse the data 
     * The remaining data in buffer is for data */
    lenData = bufSz - lenHdr;
    me->pData = malloc(lenData * sizeof(char unsigned));
    if (!(me->pData))
    {
        return ;
    }
    memcpy((void *)(me->pData), &(buf[ lenHdr ]), lenData);
    me->dataLen = lenData;
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
                                 me->tol, me->tol,
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
    if (me->pData != NULL)
    {
        free(me->pData);
        me->pData = NULL;
    } 

    if (me->pOption != NULL) 
    {
        free(me->pOption);
        me->pOption = NULL;
    } 
}



short unsigned XPROTO_IP__CalcCheckSum(XPROTO_IP_t * const me)
{
    ssize_t     bufSz = 0;
    ssize_t     lenOption = 0;         
    char unsigned *buf = NULL;    
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
    buf = malloc(bufSz * sizeof(char unsigned));
    if (buf) 
    {
        /* Copy the static header into the buffer */
        memcpy((void *)buf, (void *)me, XPROTO_IP__HDR_MIN_LEN);
        for (int x=0; x<bufSz; x++)
        {
            printf("\nIP_raw_data: (%d) -> 0x%02x", x, buf[x]);
        }

        /* Copy the options if available */
        if (lenOption && me->pOption)
        {
            memcpy((void *)(buf + XPROTO_IP__HDR_MIN_LEN),
                   (void *)(me->pOption),
                   lenOption);
        }

        /*>
         * Restore original checksum before computation as the data required
         * has been created */
        me->hdrChkSum = res;

        res = XNET_UTILS__CalcCheckSum16((void *)buf, bufSz, XNET_UTILS__enEndianType_Network);
        printf("\nThe ip checksum: 0x%04x\n", res);
        free(buf); /*  Release resource */
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
    printf("\ncalc: %04x => default: 0x%04x", checkSumCalculated, checkSumReceived);
    return retCode;
}