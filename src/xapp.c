
#include "../inc/xapp.h"

static XAPP_t XAPP__var;

XAPP_t *XAPP__GetInstance(void)
{
    return (&XAPP__var);
}




int XAPP__HandleUserInput( XAPP_t *me, int argc, char *argv[])
{
    int  argIdx;
    int  retCode = XAPP__enRetCode_HandleUserInput_Init;
    char *str = NULL;

    if (argc > 1)
    {
        argIdx = 1;
        me->option.progName = argv[0];
        while (argIdx < argc)
        {
            XPARSER__StripWhiteSpace( argv[ argIdx ], &str);

            /* Check type of argument (option or ip addres ) */
            if ( str[0] == '-')
            {
                retCode = XAPP__HandleOpt( me, str, argv, &argIdx);
            }
            else
            {
                /* process ip address */
                me->option.pOptHostAddr = str;
#ifdef XAPP__DEBUG_HANDLE_USER_INPUT /*============================= */
                printf("==> adr: %s\n", me->option.pOptHostAddr);
#endif /* XAPP__DEBUG_HANDLE_USER_INPUT ---------------------------- */
                retCode = EXIT_SUCCESS;
            }

            /* Evaluate return Code */
            if (retCode != 0)
            {
                goto labelExit;
            }
            argIdx++;
        }
    }
labelExit:
    return (retCode);
}







void  XAPP__ShowErrMsg(const char *text, ...)
{
    char buf[XAPP__RX_BUFSZ];
    ft_memset(buf, 0, XAPP__RX_BUFSZ);
    va_list args;
    va_start(args, text);
    vsprintf(buf, text, args);
    va_end(args);
    fprintf(stderr, "%s", buf);
}



char *XAPP__HandleChrOptionCount( XAPP_t * const me, char *pChr, char *argv[], int *pArgIdx, int *rc)
{
    int nbrVal = 0;

    if (*(pChr + 1) != '\0')
    {
        *rc = XAPP__enRetCode_ProcessOptionChar_InvalidOptFormatCount;
        goto labelExit;
    }
    /* increment argument index to where count value is expected */
    *pArgIdx += 1;
    if (argv[(*pArgIdx)] == NULL)
    {
        *rc = XAPP__enRetCode_ProcessOptionChar_NoOptVal;
        goto labelExit;
    }
    /*>
     * Retrieve count value string from next argv[ idx ] and conv to int */
    // check that all the nbr characters are digit before
    // converting from string to integer.
    nbrVal = atoi(argv[*pArgIdx]);
    // advance the argvIdx since it is now consumed.
    me->option.optPktCnt = nbrVal;

    printf("Count value: (%ld)\n", me->option.optPktCnt);
    if (nbrVal == 0)
    {
        *rc = XAPP__enRetCode_ProcessOptionChar_CountValInvalid;
        goto labelExit;
    }
    *rc = EXIT_SUCCESS;
labelExit:
    return ( pChr );
}





char *XAPP__HandleStrOptionTtl( XAPP_t * const me, char *currStrChr, char *nextUsrArg, int *pRetCode)
{
    int nbrVal = 0;
    *pRetCode = XAPP__enRetCode_HandleOptionTtl_Init;

    if ( *currStrChr != '=' )
    {
        if (*currStrChr == XPARSER__NULL_CHAR)
        {
            if (nextUsrArg)
            {
                XAPP__ShowErrMsg(XAPP__ERR_MSG_OPT_INVALID_VALUE, me->option.progName, nextUsrArg, nextUsrArg);
            }
            else
            {
                XAPP__ShowErrMsg(XAPP__ERR_MSG_OPT_ARG_REQ, me->option.progName, XAPP__OPT_STR_TTL);
            }
            *pRetCode = XAPP__enRetCode_HandleOptionTtl_NoEqualSign;
            goto labelExit;
        }
        else
        {
            XAPP__ShowErrMsg(XAPP__ERR_MSG_OPT_UNRECOGNISED, me->option.progName, currStrChr-ft_strlen(XAPP__OPT_STR_TTL)-2);
            *pRetCode = XAPP__enRetCode_ProcessOptionChar_OptUnknown;
            goto labelExit;
        }
    }

    if (!XPARSER__IsNbr((++currStrChr)))
    {
        XAPP__ShowErrMsg(XAPP__ERR_MSG_OPT_INVALID_VALUE, me->option.progName, currStrChr, currStrChr);
        *pRetCode = XAPP__enRetCode_HandleOptionTtl_InvalidValue;
        goto labelExit;
    }

    nbrVal = ft_atoi((currStrChr));
    if (nbrVal == 0)
    {
        XAPP__ShowErrMsg(XAPP__ERR_MSG_OPT_VAL_SMALL, me->option.progName, nbrVal);
        *pRetCode = XAPP__enRetCode_HandleOptionTtl_OptValueTooSmall;
        goto labelExit;
    }
    me->option.optTimeToLive = nbrVal;
    printf("The ttl value ( %d )\n", nbrVal);

    // advance the the pointer to the last character in string before '\0'
    currStrChr = ft_strchr(currStrChr, XPARSER__NULL_CHAR) - 1;

    *pRetCode = EXIT_SUCCESS;
labelExit:
    return ( currStrChr );

}




char   *XAPP__ProcStrOpt( XAPP_t * const me, char *pChr, char *argv[], int *pArgIdx, int *rc)
{
    if (ft_strncmp((pChr + 1), XAPP__OPT_STR_USAGE, ft_strlen((pChr + 1))) == 0)
    {
        XAPP__ShowErrMsg(XAPP__MSG_FMT_USAGE);
        *rc = XAPP__enRetCode_ProcessOptionChar_OptUsageHandled;
        goto labelExit;
    }
    else if (ft_strncmp((pChr + 1), XAPP__OPT_STR_HELP, ft_strlen((pChr + 1))) == 0)
    {
        XAPP__ShowErrMsg(XAPP__MSG_FMT_HELP);
        *rc = XAPP__enRetCode_ProcessOptionChar_OptHelpHandled;
        goto labelExit;
    }
    else if (ft_strncmp((pChr + 1), XAPP__OPT_STR_TTL, ft_strlen(XAPP__OPT_STR_TTL)) == 0)
    {
        pChr += (ft_strlen(XAPP__OPT_STR_TTL) + 1);
        pChr = XAPP__HandleStrOptionTtl(me, pChr, argv[((*pArgIdx) + 1)], rc);
        XNET_UTILS__ASSERT_UPD_REDIRECT((*rc == EXIT_SUCCESS),
                                        rc,
                                        *rc,
                                        labelExit);
    }
    else
    {
        XAPP__ShowErrMsg(XAPP__ERR_MSG_OPT_UNRECOGNISED, me->option.progName, (pChr - 1));
        *rc = XAPP__enRetCode_ProcessOptionChar_OptUnknown;
        goto labelExit;
    }
    *rc = EXIT_SUCCESS;
labelExit:
    return (pChr);
}




char   *XAPP__ProcChrOpt( XAPP_t * const me, char *pChr, char *argv[], int *pArgIdx, int *rc)
{
    char chr = *pChr;

    printf("==> opt: %s\n", (pChr-1));
    switch (chr)
    {
        case 'v':
            {
                me->option.optVerbose = 1;
                *rc = EXIT_SUCCESS;
                break ;
            }
        case '?':
            {
                XAPP__ShowErrMsg( XAPP__MSG_FMT_HELP ) ;
                *rc = XAPP__enRetCode_ProcessOptionChar_OptUsageHandled;
                break ;
            }
        case 'c':
            {
                XAPP__HandleChrOptionCount(me, pChr, argv, pArgIdx, rc);
                break ;
            }
        case '-':
        {
            pChr = XAPP__ProcStrOpt(me, pChr, argv, pArgIdx, rc);
            break ; 
        }
        default:
        {
            XAPP__ShowErrMsg( XAPP__ERR_MSG_OPT_INVALID ,  me->option.progName, *pChr);
            *rc = XAPP__enRetCode_ProcessOptionChar_InvalidOption;
            break ;
        }
    }
    return (pChr);
}




int XAPP__HandleOpt(XAPP_t * const me, char *strOpt, char *argv[], int *pArgIdx)
{
    int retCode = 0;
    char *currStrChr = strOpt;
    
    

    /* check for any whitespace in option str */
    if (XPARSER__IsWhiteSpaceInStr(strOpt) || (ft_strlen(strOpt) <= 1)) 
    {
        retCode = XAPP__enRetCode_HandleUserInput_InvalidOptionFormat;
        goto labelExit;
    }
    /* Note that the first char in string is '-'. Hence, its skipped. */
    currStrChr++;

    while (  *currStrChr != '\0' )
    {
        currStrChr = XAPP__ProcChrOpt( me, currStrChr, argv, pArgIdx, &retCode);
        XNET_UTILS__ASSERT_UPD_REDIRECT( (retCode == EXIT_SUCCESS),
                                         &retCode,
                                          retCode,
                                          labelExit);
        currStrChr++;
    }

labelExit:
    return (retCode);
}





void     XAPP__Ctor(XAPP_t * const me)
{
    ft_memset( (void *)me, 0, sizeof(XAPP_t));
}




void  XAPP__Init(XAPP_t * const me)
{
    /* Initialise the sig action and event */
    /* Establish handler for signal (SIGINT & SIGALRM) */
    me->timerEvt.sigev_signo = SIGALRM;
    me->timerEvt.sigev_notify = SIGEV_SIGNAL;
    //me->timerEvt.sigev_value.sival_ptr= &(me->timerId);
    
    me->sa.sa_flags = SA_SIGINFO;
    me->sa.sa_sigaction = XAPP__SigHandler;

    sigemptyset(&(me->sa.sa_mask));
    sigaction(SIGALRM, &(me->sa), NULL);
    sigaction(SIGINT, &(me->sa), NULL);


    /* Set the process id to server as all packet id */
    me->pid = getpid() & 0xFFFF;
    
    /* Set up the recv message component */
    me->msgRxIOvector->iov_base = me->recvBuf;
    me->msgRxIOvector->iov_len = sizeof( me->recvBuf);

    /* Setup the recv message header  */
    ft_memset(&(me->msgRx), 0, sizeof(me->msgRx));
    me->dstAddrLen = sizeof(me->dstAddr);
    me->msgRx.msg_name = &(me->dstAddr);
    me->msgRx.msg_namelen = me->dstAddrLen;
    me->msgRx.msg_iov = me->msgRxIOvector;
    me->msgRx.msg_iovlen = XAPP__NBR_IO_VECTORS;




    /* Create timer */
#if 0 /* clean up */
    me->timerVal.it_value.tv_sec = XAPP__POLL_BLOCK_DURATION; /* 1 Second */
    me->timerVal.it_value.tv_nsec = 0; /* 1 Second */
    me->timerVal.it_interval.tv_sec = 0; /* 1 Second */
    me->timerVal.it_interval.tv_nsec = 0; /* 1 Second */
    timer_create(CLOCK_MONOTONIC, &(me->timerEvt), &(me->timerId));
#endif /* clean up */
}





int     XAPP__Connect( XAPP_t * const me)
{
    int retCode = XAPP__enRetCode_Connect_Failed;
    me->dstAddrLen = sizeof(struct sockaddr_in);

    /* Initialise hints attribute */
    XNET__InitAddrInfo(&(me->hints), AF_INET, SOCK_RAW, IPPROTO_ICMP, AI_CANONNAME);

    /* setup and fetch addr info */
    retCode = getaddrinfo(me->option.pOptHostAddr, NULL, &(me->hints), &(me->pAddrInfo));
    if (retCode != 0)
    {
        fprintf(stderr, "%s\n", gai_strerror(retCode));
        goto labelExit;
    }
    else
    {
#ifdef XAPP_DEBUG
        XNET__ShowAddrInfo(me->pAddrInfo);
#endif
    }

    /* get address name info as numeric (234.232.123.31) */
    retCode = getnameinfo(me->pAddrInfo->ai_addr, me->pAddrInfo->ai_addrlen,
                    me->txAddrBuf, XAPP__BUFSZ_ADDR,
                    NULL, 0,
                    NI_NUMERICHOST);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == EXIT_SUCCESS), 
                                    &retCode,
                                    XAPP__enRetCode_Ctor_SysCallgetnameInfoFailed,
                                    labelExit);
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
    me->stats.tPoll.tv_sec = XAPP__POLL_BLOCK_DURATION; 

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




void    XAPP__ShowStartMsg(XAPP_t * const me)
{
    printf(XAPP__INFO_PING_START, me->pAddrInfo->ai_canonname,
                                  me->txAddrBuf,
                                  XAPP__DEF_ICMP_DATA_SIZE);
    if (me->option.optVerbose)
    {
        printf(", id 0x%x = %d", me->pid, me->pid);
    }
    printf("\n");
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
        /*>
         * reuse existing packet header 
         * - update the sequence number and recompute the checksum value */
        me->pIcmpHdrTx->seqnbr = me->seqNbr;
        ICMP_ECHO__CalcCheckSum(me->pIcmpHdrTx);
    }





    /* send data to dest address */
    XAPP__GetTimeOfStart(me);
    alarm(1);
    me->datalenTx = sendto(me->sockfd, (void *)(me->pIcmpHdrTx->pPktChkSum), 
                                       me->pIcmpHdrTx->totalPacketLen,
                                       0,
                                       me->pAddrInfo->ai_addr,
                                       me->pAddrInfo->ai_addrlen);
#ifdef XNET__DEBUG
    XNET_UTILS__ShowPacketHex((me->pIcmpHdr->pPktChkSum), me->pIcmpHdr->totalPacketLen );  
    XPROTO_ICMP__ShowDetails(me->pIcmpHdr);               
#endif

    XNET_UTILS__ASSERT_UPD_REDIRECT((me->datalenTx > 0),
        &retCode,
        XAPP__enRetCode_TxPacket_SendToFailed,
        labelExit);

#ifdef XAPP__DEBUG
    printf("success: sent (%ld)\n", me->datalenTx);
    printf("pid: identifier (%x)\n", me->pid);
    printf("pid: checksum (%x)\n", me->pIcmpHdrTx->icmp.checksum);
    printf("sent message type: %d\n", me->pIcmpHdrTx->icmp.type);
#endif  
    (me->pktCntTx)++;
    (me->seqNbr)++;
    retCode = EXIT_SUCCESS;

labelExit:
#ifdef XAPP__DEBUG
    printf("tx count : ( %ld ); rc( %d )\n", me->pktCntTx, retCode);
#endif  
    return (retCode);
}
    


int  XAPP__RxPacket(XAPP_t * const me) 
{
    int retCode = XAPP__enRetCode_RxPacket_Init;

    /* receive the response back */
#if 0 /* NOT_USED */
    me->datalenRx  = recvfrom(me->sockfd,
                              me->recvBuf, 
                              XAPP__RX_BUFSZ, 
                              0, 
                              (struct sockaddr *)(&(me->dstAddr)),
                              &(me->dstAddrLen) );
#endif /* NOT_USED */
    me->datalenRx  = recvmsg(me->sockfd, &(me->msgRx), 0);
    if (me->datalenRx  <= 0)
    {
        if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) )
        {
            /* No message in socket to read */
            retCode = XAPP__enRetCode_RxPacket_NoMsg;

        }
        printf("recv error\n");
        retCode = XAPP__enRetCode_RxPacket_RecvMsg_Failed;
    }
    else
    {
        // validate the received packet
#ifdef XNET__DEBUG /* ====================================================== */
        printf("success: recv (%ld)\n", me->datalenRx );
        XNET_UTILS__ShowPacketHex(me->recvBuf, me->datalenRx );
#endif /* XNET__DEBUG ------------------------------------------------------ */
        retCode = EXIT_SUCCESS;
    }
    return (retCode);
}




void XAPP__Destroy(XAPP_t * const me)
{
    XAPP__statsRttRecord_t *pRttRec  = NULL;


    

    if (me->pIpHdr)
    {
        XPROTO_IP__Destroy(me->pIpHdr);
        XNET_UTILS__Destroy((void **)&(me->pIpHdr));
    }


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

    /* Destroy timer */
    //timer_delete(me->timerId);

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


    /* transpose data */
    pRttRec->sec = XTIMER__ConvertTsToSec(&(me->stats.tDuration));
    XAPP__StatsUpdate(me);
}




void    XAPP__StatsUpdate(XAPP_t * const me)
{
    XTIMER__timespec_t *pTs;

    /*> compute min rtt : compare with the most recent computed rtt */
    if (me->stats.tRttMin.tv_nsec + me->stats.tRttMin.tv_sec == 0)
    {
        ft_memcpy((void *)&(me->stats.tRttMin), 
               (void *)&(me->stats.tDuration),
               sizeof(XTIMER__timespec_t));
    }
    else
    {
        pTs = XTIMER__Min(&(me->stats.tRttMin), &(me->stats.tDuration));
        me->stats.tRttMin.tv_nsec = pTs->tv_nsec;
        me->stats.tRttMin.tv_sec  = pTs->tv_sec;
    }
    
    /*> compute max rtt : compare with the most recent computed rtt */
    pTs = XTIMER__Max(&(me->stats.tRttMax), &(me->stats.tDuration));
    me->stats.tRttMax.tv_nsec = pTs->tv_nsec;
    me->stats.tRttMax.tv_sec  = pTs->tv_sec;
}




void    XAPP__StatsComputeRttAvg(XAPP_t * const me)
{
    double resAvg = 0.0;
    XAPP__statsRttRecord_t *pRttRec = NULL;

    if (me->pktCntRx)
    {
        pRttRec = me->stats.pRttRec;
        while (pRttRec != NULL)
        {
            resAvg += pRttRec->sec;
#ifdef XAPP__DEBUG
            printf( "\nsumAvg: curr( %.9f ) -> sum( %.9f )", pRttRec->sec, resAvg);
#endif
            /* Next item in list */
            pRttRec = pRttRec->next;
        }
        resAvg /= (double)(me->pktCntRx);
        me->stats.tRttAvg.tv_sec = ((resAvg * 1000000000) / 1000000000);
        me->stats.tRttAvg.tv_nsec = ((resAvg - me->stats.tRttAvg.tv_sec) * 1000000000);
#ifdef XAPP__DEBUG
        printf( "\nsumAvg: ( %.9f )", resAvg );
#endif
    }
}




void    XAPP__StatsComputeRttStDev(XAPP_t * const me)
{
    double resAvg = XTIMER__ConvertTsToSec(&(me->stats.tRttAvg));
    double result = 0.0;
    XAPP__statsRttRecord_t *pRttRec = NULL;

    if (me->pktCntRx)
    {
        pRttRec = me->stats.pRttRec;
        while (pRttRec != NULL)
        {
            result += ((resAvg - pRttRec->sec)*(resAvg - pRttRec->sec));

            /* Next item in list */
            pRttRec = pRttRec->next;
        }
        result /= (double)(me->pktCntRx);
        result = sqrt(result);
        me->stats.tRttStdDev.tv_sec = ((result* XTIMER__SECOND_TO_NS) / XTIMER__SECOND_TO_NS);
        me->stats.tRttStdDev.tv_nsec = ((result - me->stats.tRttStdDev.tv_sec) * XTIMER__SECOND_TO_NS);
#ifdef XAPP__DEBUG
        printf( "\nstDev: ( %.9f )", result );
#endif
    }
}




void    XAPP__StatsComputeSummary(XAPP_t * const me)
{
    /* compute avg rtt */
    XAPP__StatsComputeRttAvg(me);

    /* compute std rtt */
    XAPP__StatsComputeRttStDev(me);
}




static void XAPP__StrFindReplace(char *str, char s, char r )
{
    ssize_t txtlen = 0;
    ssize_t idx = 0;
    if ( !str )
    {
        return ;
    }
    txtlen = ft_strlen(str); 
    while ( idx < txtlen)
    {
        if ( str[idx] == s )
        {
            str[idx] = r;
        }
        idx++;
    }
}




static int XAPP__StatsComputePercentage(XAPP_t * const me)
{
    int res = 0;

    if ( me->pktCntTx)
    {
        res = ((me->pktCntTx - me->pktCntRx) * 100) / me->pktCntTx;
    }
    return (res);
}




void    XAPP__StatsShowSummary(XAPP_t * const me)
{
    XAPP__StatsComputeSummary(me);

    /*> 
     * ======================
     * show statistic summary
     * ======================
     * ^C--- 8.8.8.8 ping statistics ---
       4 packets transmitted, 4 packets received, 0% packet loss
       round-trip min/avg/max/stddev = 19,130/21,334/25,046/2,405 ms

       */
    /* Show the summary title */
    printf(XAPP__MSG_FMT_STATS_TITLE, me->pAddrInfo->ai_canonname);

    /* print information to variable */
    snprintf(me->strText,
                XAPP__BUFSZ_TXTSTR,
                XAPP__MSG_FMT_STATS,
                me->pktCntTx,
                me->pktCntRx,
                XAPP__StatsComputePercentage(me),
                '%',
                XTIMER__ConvertTsToSec(&(me->stats.tRttMin)) * 1000,
                XTIMER__ConvertTsToSec(&(me->stats.tRttAvg)) * 1000,
                XTIMER__ConvertTsToSec(&(me->stats.tRttMax)) * 1000,
                XTIMER__ConvertTsToSec(&(me->stats.tRttStdDev)) * 1000
            );
    
    /* print information stored in string variable to terminal */
    /* replace all '.' to comma ',' */
    XAPP__StrFindReplace(me->strText, '.', ',');
    printf("%s", me->strText);
}





void XAPP__StatsShowRtt(XAPP_t * const me)
{
    /*>
     * Note that the timespec has been convert to seconds only
     * using the typedef __statsRttRecord_t.
     * The new type stores the seconds value as a double.
     * ex. 1.0384234992 seconds
     * Therefore, simply multiply by 1000 to convert to ms */
    printf(XAPP__MSG_FMT_RTT1, me->pIcmpHdrRx->totalPacketLen, 
                               me->rxAddrBuf);

    snprintf(me->strText, XAPP__BUFSZ_TXTSTR,
                          XAPP__MSG_FMT_RTT2,
                          ntohs(me->pIcmpHdrRx->seqnbr),
                          me->pIpHdr->ttl,
                          me->stats.pRttRec->sec * XTIMER__SECOND_TO_MS);
    /* print information stored in string variable to terminal */
    /* replace all '.' to comma ',' */
    XAPP__StrFindReplace(me->strText, '.', ',');
    printf("%s", me->strText);
}





int  XAPP__IsRxAddrValid(XAPP_t * const me)
{
    int retCode = XAPP__enRetCode_IsRxAddrValid_Init;

    retCode = getnameinfo((struct sockaddr *)&(me->dstAddr), me->dstAddrLen,
                          me->rxAddrBuf, XAPP__BUFSZ_ADDR,
                          NULL, 0,
                          NI_NUMERICHOST);
    XNET_UTILS__ASSERT_UPD_REDIRECT( (retCode == EXIT_SUCCESS),
                                     &retCode,
                                     XAPP__enRetCode_IsRxAddrValid_AddrResFailed,
                                     labelExit);
                                     
    retCode = ft_strncmp(me->rxAddrBuf, me->txAddrBuf, ft_strlen(me->txAddrBuf));
    XNET_UTILS__ASSERT_UPD_REDIRECT( (retCode == EXIT_SUCCESS),
                                     &retCode,
                                     XAPP__enRetCode_IsRxAddrValid_InvalidRecvAddr,
                                     labelExit);
    retCode = EXIT_SUCCESS;
labelExit:
    return (retCode);
}




int     XAPP__ValidateRxPkt(XAPP_t * const me)
{
    int retCode = EXIT_FAILURE;

    /* Validate the ip address is same */
    retCode = XAPP__IsRxAddrValid(me);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == EXIT_SUCCESS), 
           &retCode, 
           XAPP__enRetCode_ValidateRxPkt_IsRxAddrValidFailed,
           labelExit);

    /*> First: validate that internet frame is valid - delete old frame */
    if (me->pIpHdr)
    {
        me->pIpHdr->Destroy(me->pIpHdr);
        XNET_UTILS__Destroy((void **)&(me->pIpHdr));
    }

    /*> First: validate that icmp frame is valid - delete old frame */
    if (me->pIcmpHdrRx)
    {
        ICMP_ECHO__Destroy(me->pIcmpHdrRx);
        XNET_UTILS__Destroy((void **)&(me->pIcmpHdrRx));
    }

    /* Create ipheader instance */
    me->pIpHdr = (XPROTO_IP_t *)malloc(sizeof(XPROTO_IP_t));
    XNET_UTILS__ASSERT_UPD_REDIRECT(me->pIpHdr, 
           &retCode, 
           XAPP__enRetCode_ValidateRxPkt_PtrIpHdrMallocFailed,
           labelExit);
    XPROTO_IP__Ctor(me->pIpHdr);

    /* Parse the recv packet to ip header instance */
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


    /* Create icmp header  instance */
    me->pIcmpHdrRx = (ICMP_ECHO_t *)malloc(sizeof(ICMP_ECHO_t));
    XNET_UTILS__ASSERT_UPD_REDIRECT(me->pIcmpHdrRx, &retCode, 
           XAPP__enRetCode_ValidateRxPkt_MallocFailed,
           labelExit);
    ICMP_ECHO__Ctor(me->pIcmpHdrRx);

    retCode = ICMP_ECHO__ValidateRxPkt(me->pIcmpHdrRx, me->pIpHdr->pData,
                                                       me->pIpHdr->dataLen);

labelExit:
    return(retCode);
}





void    XAPP__Wait(XAPP_t * const me)
{
    XTIMER__timespec_t timeCurr;
    XTIMER__timespec_t duration;

    ft_memset((void *)&duration, 0, sizeof(duration));
    ft_memset((void *)&timeCurr, 0, sizeof(timeCurr));

    /* Check to see that a time stamp was recorded prior to packet transmission */
    if ( me->stats.tStart.tv_nsec + me->stats.tStart.tv_sec != 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &timeCurr);
        XTIMER__Diff(&(timeCurr), &(me->stats.tStart), &(duration));
#ifdef XAPP__DEBUG
        XTIMER__ShowTimeSpec(&(me->stats.tStart));
        XTIMER__ShowTimeSpec(&(timeCurr));
        XTIMER__ShowTimeSpec(&(duration));
#endif
        if ( !(duration.tv_sec))
        {
            pause();
        }
        ft_memset((void *)&(me->stats.tStart), 0, sizeof(XTIMER__timespec_t));
    }
}








/*>
 * ----------------------------------------------------------------------------
 * INTERRUPT - SIGNAL HANDLER
 * ----------------------------------------------------------------------------
 * */

void XAPP__SigHandler(int sig, siginfo_t *si, void *uc)
{
    XAPP_t *pAppVar = XAPP__GetInstance();

#ifdef XAPP__DEBUG
    printf("Got signal: %d\n", sig);
    printf("handle timer signal ( %d )\n", si->si_signo);
#endif

    switch (sig)
    {
        case SIGALRM:
        {
            /* Do nothing - Only required to wake up the process from sleep/pause */
            break;
        }
        case SIGINT:
        {
            XAPP__Destroy(pAppVar);
            exit(EXIT_SUCCESS);
            break ;
        }
        default: break ;
    }
    if (uc && si){ /* avoid compiler warnining and error */ }
}
