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

    retCode = XAPP__CreateIcmpPacket(pAppVar, XPROTO_ICMP__enMsgType_Echo);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == EXIT_SUCCESS), 
                                    &retCode,
                                    retCode, /* retransmit the same error */
                                    labelExit);
    while (XAPP__TRUE)
    {
        /*>
         * Blocks indefinitely until something happens in any of the 
         * file descriptors */
        pAppVar->ready = poll(pAppVar->fds, pAppVar->nfds, 1000);
                             
        if (pAppVar->ready)
        {
            if (pAppVar->fds[0].revents & POLLIN)
            {
                XAPP__RxPacket(pAppVar);
                pAppVar->pktCntRx++;
            }
        }
        else
        {
            printf("\nNothing happened !\n");
        }
        

        /* send tx packet  */
        //if (pAppVar->pktCntTx <= XAPP__DEF_REQNBR)
        if (pAppVar->pktCntTx >= 2)
        {
            break;
        }
        else
        {
            XAPP__TxPacket(pAppVar);
            pAppVar->pktCntTx++;
        }
    }


labelExit:
    /* clean up */
    XAPP__Destroy(pAppVar);
    return (retCode);
}
