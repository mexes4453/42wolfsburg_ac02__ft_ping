# include "../inc/xtimer.h"



XTIMER__timespec_t *XTIMER__Max(XTIMER__timespec_t *pTs1, XTIMER__timespec_t *pTs2)
{
    XTIMER__timespec_t *pTs;
    long int diff = pTs1->tv_sec - pTs2->tv_sec;
    if (diff)
    {
        pTs = pTs1;
    }
    else if ( diff < 0) 
    {
        pTs = pTs2;
    }
    else /* the same value : diff=0 */
    {
        /* check nsec */
        diff = pTs1->tv_nsec - pTs2->tv_nsec;
        if (diff)
        {
            pTs = pTs1;
        }
        else if (diff < 0)
        {
            pTs = pTs2;
        }
        else
        {
            pTs = pTs1;
        }
    }
    return (pTs);
}


XTIMER__timespec_t *XTIMER__Min(XTIMER__timespec_t *pTs1, XTIMER__timespec_t *pTs2)
{
    XTIMER__timespec_t *pTs;
    long int diff = pTs1->tv_sec - pTs2->tv_sec;
    if (diff)
    {
        pTs = pTs2;
    }
    else if ( diff < 0) 
    {
        pTs = pTs1;
    }
    else /* the same value : diff=0 */
    {
        /* check nsec */
        diff = pTs1->tv_nsec - pTs2->tv_nsec;
        if (diff)
        {
            pTs = pTs2;
        }
        else if (diff < 0)
        {
            pTs = pTs1;
        }
        else
        {
            pTs = pTs1;
        }
    }
    return (pTs);
}



void XTIMER__Diff(XTIMER__timespec_t *pTs1, XTIMER__timespec_t *pTs2,
                                                XTIMER__timespec_t *pRes)
{
    XTIMER__timespec_t *pTsMax = XTIMER__Max(pTs1, pTs2);
    XTIMER__timespec_t *pTsMin = XTIMER__Min(pTs1, pTs2);
    long int diff = pTsMax->tv_nsec - pTsMin->tv_nsec;

    if (diff < 0)
    {
        pRes->tv_nsec = pTsMax->tv_nsec + (XTIMER__SECOND_TO_NS - pTsMin->tv_nsec);
        pRes->tv_sec  = pTsMax->tv_sec - 1 - pTsMin->tv_sec;
    }
    else
    {
        pRes->tv_nsec = pTsMax->tv_nsec - pTsMin->tv_nsec;
        pRes->tv_sec  = pTsMax->tv_sec - pTsMin->tv_sec;
    }
}




void XTIMER__Sum(XTIMER__timespec_t *pTs1, XTIMER__timespec_t *pTs2,
                                                XTIMER__timespec_t *pRes)
{
    long int sum = pTs1->tv_nsec - pTs2->tv_nsec;
    pRes->tv_nsec = sum % XTIMER__SECOND_TO_NS;
    pRes->tv_sec  = pTs1->tv_sec + pTs2->tv_sec + (sum / XTIMER__SECOND_TO_NS);
}




void  XTIMER__ShowTimeSpec(XTIMER__timespec_t *pTs)
{
    printf("Timespec: S( %ld ), NS ( %ld )\n", pTs->tv_sec, pTs->tv_nsec);
}