#include "../inc/xapp.h"


int main(int argc, char *argv[])
{
    int     idxArg;
    int     retCode;
    XAPP_t *pAppVar;

    /* Establish handler for signal (SIGINT) */
    signal(SIGINT, XAPP__SigIntHandler);

    /* parse argument */
#if 1
    int opt = 1;
    int optind =1, optopt=1;
    if (argc > 1)
    {
        //ipDst = argv[idxArg];
        while ( (opt = getopt(argc, argv, ":v:x")) != -1) 
        {
            printf("%c - %s %s", opt, argv[optind], argv[optopt]);
            printf("%s\n", argv[idxArg]);
            ++idxArg;
        }
    }
#endif

    /* Initialise the variables */
    idxArg = 1;
    pAppVar = XAPP__GetInstance();
    retCode = XAPP__Ctor(pAppVar, XAPP__ADDR_DST);

  
    while (XAPP__TRUE)
    {
        /*>
         * Blocks indefinitely until something happens in any of the 
         * file descriptors */
        //pAppVar->ready = poll(pAppVar->fds, pAppVar->nfds, XAPP__POLL_BLOCK_DURATION);
        pAppVar->ready = ppoll(pAppVar->fds, pAppVar->nfds, &(pAppVar->stats.tPoll), NULL);
                             
        if (pAppVar->ready)
        {
            if (pAppVar->fds[0].revents & POLLIN)
            {
                XAPP__GetTimeOfEnd(pAppVar);
                if (XAPP__RxPacket(pAppVar) == EXIT_SUCCESS)
                {
                    retCode = XAPP__ValidateRxPkt(pAppVar);
                    if (retCode == EXIT_SUCCESS)
                    {
                        pAppVar->pktCntRx++;
                        XAPP__StatsComputeRtt(pAppVar);
                    }
                }
            }
        }
        else
        {
#ifdef XNET__DEBUG
            printf("\nNothing to read !\n");
#endif
        }
        XAPP__Wait(pAppVar);
        /* send tx packet  */
        //if (pAppVar->pktCntTx <= XAPP__DEF_REQNBR)
        if (pAppVar->pktCntTx >= 5)
        {
            break;
        }
        else
        {
            XAPP__TxPacket(pAppVar);
        }
    }


//labelExit:
    /* clean up */
    XAPP__Destroy(pAppVar);
    return (retCode);
}
