#include "../inc/xapp.h"
#include "../inc/xerr.h"


int main(int argc, char *argv[])
{
    int     retCode;
    XAPP_t *pAppVar;

    /* Initialise the variables */
    pAppVar = XAPP__GetInstance();
    retCode = XAPP__Ctor(pAppVar, NULL, argc, argv); /* clean up : remove full from function and prototype */
    //XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == 0), &retCode, retCode, labelExit);
    XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == 0), &retCode, retCode, labelCleanup);


    XAPP__ShowStartMsg(pAppVar);

    while (XAPP__TRUE)
    {
        /*>
         * Blocks indefinitely until something happens in any of the 
         * file descriptors */
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
                        XAPP__StatsShowRtt(pAppVar);
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
        /*> 
         * Put process to sleep and Wait for timer set prior to
         * transmit to expire before sending the next packet */
        XAPP__Wait(pAppVar);

        /* send tx packet  */
        if (pAppVar->pktCntTx >= XAPP__DEF_REQNBR)
        {
            break;
        }
        else
        {
            retCode = XAPP__TxPacket(pAppVar);
            XNET_UTILS__ASSERT_UPD_REDIRECT((retCode == 0), &retCode, retCode, labelExit);
        }
    }



labelExit:
    if (retCode != EXIT_SUCCESS)
    {
        XERR__HandleError(retCode, argv[0]);
    }
    else
    {
        XAPP__StatsShowSummary(pAppVar);
    }
labelCleanup: /* clean up */
    XAPP__Destroy(pAppVar);
    return (retCode);
}
