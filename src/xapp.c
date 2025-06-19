
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
    me->stats.tPoll.tv_sec = 1; 

labelExit:
    return (retCode);
}




int  XAPP__CreateIcmpHeader(XAPP_t * const me)
{
    int retCode = XAPP__enRetCode_CreateIcmpHdr_Failed;

    me->pIcmpHdrTx = (ICMP_ECHO_t*)malloc(sizeof(ICMP_ECHO_t));
    XNET_UTILS__ASSERT_UPD_REDIRECT((me->pIcmpHdrTx), 
                                    &retCode,
                                    XAPP__enRetCode_CreateIcmpHdr_MallocFailed,
                                    labelExit);
    ICMP_ECHO__Ctor(me->pIcmpHdrTx);
    retCode = EXIT_SUCCESS;
labelExit:
    return (retCode);
}



#if 0
int  XAPP__CreateIcmpPayload(XAPP_t * const me)
{
    int retCode = XAPP__enRetCode_CreateIcmpPayload_Failed;

    me->payloadLen = XAPP__DEF_ICMP_DATA_SIZE;
    me->pucPayload = (char unsigned *)malloc(me->payloadLen);
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

#endif








/* consider dynamic payload length */
int XAPP__CreateIcmpPacket(XAPP_t * const me, XPROTO_ICMP__eType_t msgType)
{
    int retCode;

    /* create a icmp header */
    retCode = XAPP__CreateIcmpHeader(me);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == EXIT_SUCCESS), 
                                    &retCode,
                                    retCode, /* retransmit the same error */
                                    labelExit);


    /* Create icmp packet for transmission */
    if (msgType) {}
    retCode = ICMP_ECHO__CreatePacket(me->pIcmpHdrTx,
                                            0,
                                            me->pid,
                                            me->seqNbr);
    retCode = EXIT_SUCCESS;
labelExit:
    return (retCode);
}





int XAPP__TxPacket(XAPP_t * const me) 
{
    int retCode = XAPP__enRetCode_TxPacket_Init;

    if (me->pktCntTx == 0)
    {
        retCode = XAPP__CreateIcmpPacket(me, XPROTO_ICMP__enType_Echo);
        XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == EXIT_SUCCESS),
                                        &retCode,
                                        retCode, /* retransmit the same error */
                                        labelExit);
    }
    else if (me->pktCntTx > 0)
    {
        /* reuse existing packet header */

    }

    /* send data to dest address */
    XAPP__GetTimeOfStart(me);
    me->datalenTx = sendto(me->sockfd, (void *)(me->pIcmpHdrTx->pPktChkSum), 
                                       me->pIcmpHdrTx->totalPacketLen,
                                       0,
                                       me->pAddrInfo->ai_addr,
                                       me->pAddrInfo->ai_addrlen);
#ifdef XNET__DEBUG
    XNET_UTILS__ShowPacketHex((me->pIcmpHdr->pPktChkSum), me->pIcmpHdr->totalPacketLen );  
    XPROTO_ICMP__ShowDetails(me->pIcmpHdr);               
#endif

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
        (me->pktCntTx)++;
        (me->seqNbr)++;
    }

labelExit:
    return (retCode);
}
    


int  XAPP__RxPacket(XAPP_t * const me) 
{
    int retCode = XAPP__enRetCode_RxPacket_Init;
    /* receive the response back */
    me->datalenRx  = recvfrom(me->sockfd, me->recvBuf, 
                                      XAPP__RX_BUFSZ, 
                                      0, 
                                      (struct sockaddr *)(&(me->dstAddr)),
                                      &(me->dstAddrLen) );
    if (me->datalenRx  <= 0)
    {
        printf("recv error\n");
        retCode = XAPP__enRetCode_RxPacket_Failed;
    }
    else
    {
        // validate the received packet
#ifdef XNET__DEBUG
        printf("success: recv (%ld)\n", me->datalenRx );
        XNET_UTILS__ShowPacketHex(me->recvBuf, me->datalenRx );
#endif
        retCode = EXIT_SUCCESS;
    }
    return (retCode);
}




void XAPP__Destroy(XAPP_t * const me)
{
    XAPP__statsRttRecord_t *pRttRec  = NULL;


    XAPP__StatsShowSummary(me);
    if (me->pIcmpHdrTx)
    {
       /* Release resource created within the object first */
       XNET_UTILS__Destroy((void **)&(me->pIcmpHdrTx->pData));
       XNET_UTILS__Destroy((void **)&(me->pIcmpHdrTx->pPktChkSum));

       /* Release the memory for the object */
       XNET_UTILS__Destroy((void **)&(me->pIcmpHdrTx));
    }

    if (me->pIcmpHdrRx)
    {
       /* Release resource created within the object first */
       XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx->pData));
       XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx->pPktChkSum));

       /* Release the memory for the object */
       XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx));
    }


    /* Release the time record */
    if ( !XNET_UTILS__IsNullPtr((void **)&(me->stats.pRttRec)))
    {
        while (me->stats.pRttRec != NULL) 
        {
            /* capture the head */
            pRttRec = me->stats.pRttRec;

            /* seperate the head from body */
            me->stats.pRttRec = pRttRec->next;

            /* destroy detached head */
            pRttRec->next = NULL;
            XNET_UTILS__Destroy((void **)&(pRttRec));
        }
    }





    /* Close the file descriptor for the socket */
    if (me->sockfd)
    {
       close(me->sockfd);
    }



    /* Release the memory for the address information */
    freeaddrinfo(me->pAddrInfo);
}




void    XAPP__GetTimeOfStart(XAPP_t * const me)
{
    clock_gettime(CLOCK_MONOTONIC, &(me->stats.tStart));
}




void    XAPP__GetTimeOfEnd(XAPP_t * const me)
{
    clock_gettime(CLOCK_MONOTONIC, &(me->stats.tEnd));
}




void    XAPP__StatsComputeRtt(XAPP_t * const me)
{
    XAPP__statsRttRecord_t *pRttRec= NULL;

    XTIMER__Diff(&(me->stats.tEnd), &(me->stats.tStart), &(me->stats.tDuration));
#ifdef XAPP__DEBUG
    XTIMER__ShowTimeSpec(&(me->stats.tDuration));
#endif


    /* transfer duration (rtt) to rtt record list (decimal format) */
    /* create a rec */
    if (me->stats.pRttRec == NULL)
    {
        me->stats.pRttRec = (XAPP__statsRttRecord_t *)malloc(sizeof(XAPP__statsRttRecord_t));
        pRttRec = me->stats.pRttRec;
        pRttRec->next = NULL;
    }
    else
    {
        /* create a new instance and add to the existing link */
        pRttRec = (XAPP__statsRttRecord_t *)malloc(sizeof(XAPP__statsRttRecord_t));
        pRttRec->next = me->stats.pRttRec; /* attached new instance to existing link top */
        me->stats.pRttRec = pRttRec; /* give back - add to link */
    }

    /* check for error */
    if (pRttRec != NULL)
    {

    } /* RETURN */

    /* */
    //memset((void *)pRttRec, 0, sizeof(XAPP__statsRttRecord_t));

    /* transpose data */
    pRttRec->sec = me->stats.tDuration.tv_nsec/1000000000.0;
    pRttRec->sec += (me->stats.tDuration.tv_sec);
    XAPP__StatsUpdate(me);
    XAPP__StatsShowRtt(me);
}




void    XAPP__StatsUpdate(XAPP_t * const me)
{
    XTIMER__timespec_t *pTs;

    /*> compute min rtt : compare with the most recent computed rtt */
    pTs = XTIMER__Min(&(me->stats.tRttMin), &(me->stats.tDuration));
    me->stats.tRttMin.tv_nsec = pTs->tv_nsec;
    me->stats.tRttMin.tv_sec  = pTs->tv_sec;
    
    /*> compute max rtt : compare with the most recent computed rtt */
    pTs = XTIMER__Max(&(me->stats.tRttMax), &(me->stats.tDuration));
    me->stats.tRttMax.tv_nsec = pTs->tv_nsec;
    me->stats.tRttMax.tv_sec  = pTs->tv_sec;

    /* compute avg rtt */
    if (me->pktCntRx)
    {
        XTIMER__Sum(&(me->stats.tRttAvg), &(me->stats.tDuration), &(me->stats.tRttAvg));
    }

    /* compute std rtt */
}


void    XAPP__StatsShowSummary(XAPP_t * const me)
{
    /*> 
     * ==============
     * show statistic
     * ==============
     * ^C--- 8.8.8.8 ping statistics ---
       4 packets transmitted, 4 packets received, 0% packet loss
       round-trip min/avg/max/stddev = 19,130/21,334/25,046/2,405 ms
     */
    printf(XAPP__MSG_FMT_STATS, me->pAddrInfo->ai_canonname,
                me->pktCntTx,
                me->pktCntRx,
                ((me->pktCntTx - me->pktCntRx) * 100) / me->pktCntTx,
                23.4, 23.4, 23.4, 23.4
            );

}


void XAPP__StatsShowRtt(XAPP_t * const me)
{
    printf(XAPP__MSG_FMT_RTT,
                           me->datalenRx - XPROTO_IP__HDR_MIN_LEN,
                           me->pAddrInfo->ai_canonname,
                           me->seqNbr,
                           111,
                           me->stats.pRttRec->sec * 1000);
}




int     XAPP__ValidateRxPkt(XAPP_t * const me)
{
    int retCode = EXIT_FAILURE;

    /*> First: validate that internet frame is valid */
    if (me->pIpHdr)
    {
        me->pIpHdr->Destroy(me->pIpHdr);
        XNET_UTILS__Destroy((void **)&(me->pIpHdr));
    }
    if (me->pIcmpHdrRx)
    {
        XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx->pData));
        XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx->pPktChkSum));
        XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx));
    }
    me->pIpHdr = NULL;
    me->pIcmpHdrRx = NULL;

    /* Create ipheader instance */
    me->pIpHdr = (XPROTO_IP_t *)malloc(sizeof(XPROTO_IP_t));
    XNET_UTILS__ASSERT_UPD_REDIRECT(me->pIpHdr, 
           &retCode, 
           XAPP__enRetCode_ValidateRxPkt_PtrIpHdrMallocFailed,
           labelExit);
    memset((void *)(me->pIpHdr), 0, sizeof(XPROTO_IP_t));

    /* Parse the recv packet to ip header instance*/
    retCode = XPROTO_IP__ParseFrom(me->pIpHdr, me->recvBuf, me->datalenRx);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == EXIT_SUCCESS), 
           &retCode, 
           XAPP__enRetCode_ValidateRxPkt_ParseFailed,
           labelExit);

#ifdef XAPP__DEBUG
        printf("%s => XPROTO_IP__ParseFrome: retcode(%d)\n", XAPP_D_VALIDATE_RX_PKT, retCode );
#endif

#ifdef XAPP_DEBUG
    XPROTO_IP__ShowDetails(me->pIpHdr);
#endif


    retCode = XPROTO_IP__IsCheckSumValid(me->pIpHdr);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == EXIT_SUCCESS), 
           &retCode, 
           XAPP__enRetCode_ValidateRxPkt_InvalidIpFrame,
           labelExit);


    me->pIcmpHdrRx = (ICMP_ECHO_t *)malloc(sizeof(ICMP_ECHO_t));
    XNET_UTILS__ASSERT_UPD_REDIRECT(me->pIcmpHdrRx, &retCode, 
           XAPP__enRetCode_ValidateRxPkt_MallocFailed,
           labelExit);
    ICMP_ECHO__Ctor(me->pIcmpHdrRx);

    retCode = ICMP_ECHO__ValidateRxPkt(me->pIcmpHdrRx, me->pIpHdr->pData,
                                                       me->pIpHdr->dataLen);

    /* release resource */
    //XNET_UTILS__Destroy((void **)&(me->pIpHdr->pPktChkSum));
    XNET_UTILS__Destroy((void **)&(me->pIpHdr->pData));
    XNET_UTILS__Destroy((void **)&(me->pIpHdr->pOption));
    XNET_UTILS__Destroy((void **)&(me->pIpHdr));
    /* */
    XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx->pData));
    XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx->pPktChkSum));
    XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx));
labelExit:
    return(retCode);
}





void    XAPP__Wait(XAPP_t * const me)
{
    XTIMER__timespec_t timeCurr;
    XTIMER__timespec_t duration;

    memset((void *)&duration, 0, sizeof(duration));

    while( !(duration.tv_sec) )
    {
        clock_gettime(CLOCK_MONOTONIC, &timeCurr);
        XTIMER__Diff(&(timeCurr), &(me->stats.tStart), &(duration));
    }

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
        exit(EXIT_SUCCESS);
    }
}