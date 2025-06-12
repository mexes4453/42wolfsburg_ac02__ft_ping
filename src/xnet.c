
#include "../inc/xnet.h"

void XNET__InitAddrInfo(struct addrinfo *pAi, int family,
                                              int socktyp,
                                              int protocol,
                                              int flags)
{
    memset(pAi, 0, sizeof(struct addrinfo));
    pAi->ai_canonname = NULL;
    pAi->ai_addr = NULL;
    pAi->ai_next = NULL;
    pAi->ai_family = family;
    pAi->ai_socktype = socktyp;
    pAi->ai_flags = flags;
    pAi->ai_protocol = protocol;

}

void XNET__ShowAddrInfo(struct addrinfo *pAi)
{
    struct addrinfo *pNodeAi;

    if (!pAi) return;
    pNodeAi = pAi;

    while ( pNodeAi != NULL )
    {
        printf(XNET__MSG_FMT_ADDR_INFO, 
               pNodeAi->ai_canonname,
               ((struct sockaddr_in *)pNodeAi->ai_addr)->sin_family,
               ntohs(((struct sockaddr_in *)pNodeAi->ai_addr)->sin_port),
               /*ntohl(*/((struct sockaddr_in *)pNodeAi->ai_addr)->sin_addr.s_addr); //);

        /* Next node to addr info in linked list */
        pNodeAi = pNodeAi->ai_next;
    }
}




int  XNET__CreateSocket(struct addrinfo *pAi)
{
    struct addrinfo *pNodeAi;
    int              retCode;

    retCode = XNET__ERR_CREATSOCK;
    if (!pAi) goto escape;
    pNodeAi = pAi;

    while ( pNodeAi != NULL )
    {
        retCode = socket(pNodeAi->ai_family, pNodeAi->ai_socktype,
                                             pNodeAi->ai_protocol);
        
        if (retCode == XNET__FAILURE) 
        {
            perror("[XNET::CreateSocket] - Error Failed to create sock for addr_info node");
        }
        else
        {
            printf("[XNET::CreateSocket] - success %d",retCode);
            break ;
        }
        /* Next node to addr info in linked list */
        pNodeAi = pNodeAi->ai_next;
    }
    if (!pNodeAi) /* equal NULL - No socket was created */
    {
        fprintf(stderr, XNET__ERR_MSG_NO_SOCK);
    }
escape:
    return (retCode);
}








void XNET__ShowRecvdMsg(char unsigned *buf, ssize_t bufSz)
{
    int idxRow = 0; 
    //XPROTO_IP_t   procIp;
    //XPROTO_ICMP_t procIcmp;
    //XPROTO_IP__Ctor(&procIp);
    //XPROTO_ICMP__Ctor(&procIcmp);

    if (!buf) return;

    printf("\nMessage\n");
    for (ssize_t x=0; x<bufSz; x++)
    {   
        /* round of 8 split */
        if ( (x - ((idxRow-1)*16)) == 8)
        { 
            printf("   ");
        } 
        

        /* newline */
        if (!(x%16))
        { 
            printf("\n%03d: => ", idxRow);
            ++idxRow;
        }

        /* print data (byte) */
        printf("%02x ", buf[x]);
    }
    printf("\n");
    //procIp.ParseFrom(&procIp, buf, bufSz);
    //procIp.ShowDetails(&procIp);
    //if ( procIp.IsCheckSumValid(&procIp) == XPROTO_IP__enRetCode_Success)
    //{
    //    procIcmp.ParseFrom(&procIcmp, procIp.pData, procIp.dataLen);
    //    procIcmp.ShowDetails(&procIcmp);
        /* verify checksum */
    //}
    //else
    //{
    //    printf("\nfail checksum check");
    //}

    //XNET__hdrIcmp_t protoIcmp;
    //memcpy((void *)&protoIcmp, (void *)&procIcmp, sizeof(procIcmp));
    //protoIcmp.checksum = 0;
    //printf("\nChecksum icmp: %x\n", XNET__CalcCheckSum16((void *)&procIcmp, XNET__HDR_MIN_LEN_ICMP));

    /* release resources */
    //procIp.Destroy(&procIp);
    //procIcmp.Destroy(&procIcmp);
}










