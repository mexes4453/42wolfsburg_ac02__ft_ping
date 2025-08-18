
#include "../inc/xapp.h"

static XAPP_t XAPP__var;

XAPP_t *XAPP__GetInstance(void)
{
    return (&XAPP__var);
}




int XAPP__HandleUserInput( XAPP_t *me, int argc, char *argv[])
{
    int  argIdx;
    int  retCode = 0;
    char *str = NULL;


    printf("Prog: %s\n", argv[0]);
    if (argc > 1)
    {
        argIdx = 1;
        while (argIdx < argc)
        {
            /* access and retrieve string at argv idx */
            // chr = argv[ argIdx ];

            /* stripwhitespace around string - leading and trailing */
            XPARSER__StripWhiteSpace( argv[ argIdx ], &str);

            printf("args(%s)\n", str);

            /* Check type of argument (option or ip addres ) */
            if ( str[0] == '-')
            {
                printf("==> opt: \n");
                retCode = XAPP__HandleOpt(str, argv, &argIdx);
            }
            else
            {
                /* process ip address */
                printf("==> adr: \n");
            }

            /* Evaluate return Code */
            if (retCode != 0)
            {
                printf("Error: %d\n", retCode);
                goto labelExit;
            }
            argIdx++;
        }
    }
labelExit:
    retCode = -1; /* for testing purpose only */
    return (retCode);
}



#if 0 /* NOT_USED */
int     XAPP__GetOpt(XAPP_t * const me, int argc, char *argv[])
{
    int retCode = XAPP__enRetCode_GetOpt_Init;
    int idxArgc = 1;                /* program name is idx 0 */

    while ( idxArgc < argc )
    {
        if ( strcmp(argv[ idxArgc ], "-v") == 0)
        {
            me->option.optVerbose = 1;
            printf("verbose( %d )\n", me->option.optVerbose);
        }
        else if ( strcmp(argv[ idxArgc ], "-?") == 0)
        {
            me->option.optUsage = 1;
            printf("usage( %d )\n", me->option.optUsage);
        }
        else if ( strcmp(argv[ idxArgc ], "-c") == 0)
        {
            idxArgc += 1;
            me->option.optPktCnt = atoi(argv[idxArgc]);
            printf("count( %ld )\n", me->option.optPktCnt);
        }
        else if ( strcmp(argv[ idxArgc ], "-ttl") == 0)
        {
            idxArgc += 1;
            me->option.optTimeToLive = atoi(argv[idxArgc]);
            printf("ttl( %ld )\n", me->option.optTimeToLive);
        }
        else
        {
            me->option.pOptHostAddr = argv[idxArgc];
            printf("addr( %s )\n", me->option.pOptHostAddr);
        }
        ++idxArgc;
    }
    retCode = EXIT_SUCCESS;
    return (retCode);
}
#endif 




int XAPP__ProcessOptionChar( char *pChr, char *argv[], int *pArgIdx)
{
    int  retCode = 0;
    char chr = *pChr;
    int  cntValue = 0;

    switch (chr)
    {
        case 'v':
            {
                printf("setting verbose flag\n");
                retCode = 0;
                break ;
            }
        case '?':
            {
                printf("show the program usage here\n");
                retCode = -501;
                break ;
            }
        case 'c':
            {
                if ( *(pChr + 1) != '\0' )
                {
                    printf("Invalid option format for (c)\n");
                    retCode = -502;
                    goto labelExit;
                }
                /* increment argument index to where count value is expected */ 
                *pArgIdx += 1;
                if ( argv[ (*pArgIdx) ] == NULL )
                {
                    printf("Missing value for option (c)\n");
                    retCode = -503;
                    goto labelExit;
                }
                printf("Retrieve count value string from next argv[ idx ] and conv to int\n");
                cntValue = atoi( argv[ *pArgIdx ] );

                printf("Count value: (%d)\n", cntValue);
                if (cntValue == 0)
                {
                    printf("Invalid count value for option (c)\n");
                    retCode = -504;
                    goto labelExit;
                }

                printf("update the count value variable in app instance\n");
                retCode = 0;
                break ;
            }
        default:
        {
            printf("Unknown option\n");
            retCode = 506;
            break ;
        }
    }
labelExit:
    return (retCode);
}




int XAPP__HandleOpt(char *strOpt, char *argv[], int *pArgIdx)
{
    int retCode = 0;

    /* check for any whitespace in option str */
    if (XPARSER__IsWhiteSpaceInStr(strOpt) || (strlen(strOpt) <= 1)) 
    {
        retCode = -500; /* invalid option format */
        goto labelExit;
    }

    do
    {
        /* Note that the first char in string is '-'. Hence, its skipped. */
        strOpt++;
        
        if ( *strOpt == '\0' )
        {
            break ;
        }
        retCode = XAPP__ProcessOptionChar( strOpt, argv, pArgIdx);
    } while ( retCode == 0);

labelExit:
    return (retCode);
}














int     XAPP__Ctor(XAPP_t * const me, char const * const strIpAddr, int argc, char *argv[])
{
    int retCode = 0;
    memset( (void *)me, 0, sizeof(XAPP_t));

    /* Initialise the parser */
    retCode = XAPP__HandleUserInput(me, argc, argv);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode ==  EXIT_SUCCESS), 
                                    &retCode,
                                    XAPP__enRetCode_Ctor_GetOptFailed,
                                    labelExit);

    /* Initialise the sig action and event */
    /* Establish handler for signal (SIGINT & SIGALRM) */
    me->timerEvt.sigev_signo = SIGALRM;
    me->timerEvt.sigev_notify = SIGEV_SIGNAL;
    me->timerEvt.sigev_value.sival_ptr= &(me->timerId);
    
    me->sa.sa_flags = SA_SIGINFO;
    me->sa.sa_sigaction = XAPP__SigHandler;
    sigemptyset(&(me->sa.sa_mask));
    sigaction(SIGALRM, &(me->sa), NULL);
    sigaction(SIGINT, &(me->sa), NULL);


    me->dstAddrLen = sizeof(struct sockaddr_in);

    /* Initialise hints attribute */
    XNET__InitAddrInfo(&(me->hints), AF_INET, SOCK_RAW, IPPROTO_ICMP, AI_CANONNAME);

    /* setup and fetch addr info */
    retCode = getaddrinfo(strIpAddr, NULL, &(me->hints), &(me->pAddrInfo));
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
    me->stats.tPoll.tv_sec = XAPP__POLL_BLOCK_DURATION; 
    

    /* Create timer */
    me->timerVal.it_value.tv_sec = XAPP__POLL_BLOCK_DURATION; /* 1 Second */
    me->timerVal.it_value.tv_nsec = 0; /* 1 Second */
    me->timerVal.it_interval.tv_sec = 0; /* 1 Second */
    me->timerVal.it_interval.tv_nsec = 0; /* 1 Second */
    timer_create(CLOCK_MONOTONIC, &(me->timerEvt), &(me->timerId));

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




void    XAPP__ShowStartMsg(XAPP_t * const me)
{
    printf(XAPP__INFO_PING_START, me->pAddrInfo->ai_canonname,
                                  me->pAddrInfo->ai_canonname,
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
        /* reuse existing packet header */
    }





    /* send data to dest address */
    XAPP__GetTimeOfStart(me);
    alarm(1);
    //retCode = timer_settime(me->timerId, 0, &(me->timerVal), NULL);
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
    timer_delete(me->timerId);

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
    pRttRec->sec = XTIMER__ConvertTsToSec(&(me->stats.tDuration));
    XAPP__StatsUpdate(me);
    XAPP__StatsShowRtt(me);
}




void    XAPP__StatsUpdate(XAPP_t * const me)
{
    XTIMER__timespec_t *pTs;

    /*> compute min rtt : compare with the most recent computed rtt */
    if (me->stats.tRttMin.tv_nsec + me->stats.tRttMin.tv_sec == 0)
    {
        memcpy((void *)&(me->stats.tRttMin), 
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
    txtlen = strlen(str); 
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
                               me->pAddrInfo->ai_canonname);

    snprintf(me->strText, XAPP__BUFSZ_TXTSTR,
                          XAPP__MSG_FMT_RTT2,
                          me->seqNbr,
                          me->pIpHdr->ttl,
                          me->stats.pRttRec->sec * 1000);
    /* print information stored in string variable to terminal */
    /* replace all '.' to comma ',' */
    XAPP__StrFindReplace(me->strText, '.', ',');
    printf("%s", me->strText);
}




int     XAPP__ValidateRxPkt(XAPP_t * const me)
{
    int retCode = EXIT_FAILURE;

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

    memset((void *)&duration, 0, sizeof(duration));
    memset((void *)&timeCurr, 0, sizeof(timeCurr));

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
        memset((void *)&(me->stats.tStart), 0, sizeof(XTIMER__timespec_t));
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