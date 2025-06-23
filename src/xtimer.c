# include "../inc/xtimer.h"


XTIMER__timespec_t *XTIMER__Max(XTIMER__timespec_t *pTs1, XTIMER__timespec_t *pTs2)
{
    XTIMER__timespec_t *pTs;
    long int diff = (int long)pTs1->tv_sec - (int long)pTs2->tv_sec;
    if (diff > 0)
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
        diff = (int long) pTs1->tv_nsec - (int long)pTs2->tv_nsec;
        if (diff > 0)
        {
            pTs = pTs1;
#ifdef XTIMER__DEBUG
            printf("\n max check %ld\n", diff);
#endif
        }
        else if (diff < 0)
        {
            pTs = pTs2;
        }
        else
        {
            pTs = pTs2;
        }
    }
#ifdef XTIMER__DEBUG
    XTIMER__ShowTimeSpec(pTs1);
    XTIMER__ShowTimeSpec(pTs2);
    XTIMER__ShowTimeSpec(pTs);
#endif
    return (pTs);
}




XTIMER__timespec_t *XTIMER__Min(XTIMER__timespec_t *pTs1, XTIMER__timespec_t *pTs2)
{
    XTIMER__timespec_t *pTs;
    long int diff = (int long)(pTs1->tv_sec) - (int long)(pTs2->tv_sec);
    if (diff > 0)
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
        diff = (int long)(pTs1->tv_nsec) - (int long)(pTs2->tv_nsec);
        if (diff > 0)
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
#ifdef XTIMER__DEBUG
    XTIMER__ShowTimeSpec(pTs1);
    XTIMER__ShowTimeSpec(pTs2);
    XTIMER__ShowTimeSpec(pTs);
#endif
    return (pTs);
}




void XTIMER__Diff(XTIMER__timespec_t *pTs1, XTIMER__timespec_t *pTs2,
                                            XTIMER__timespec_t *pRes)
{
    XTIMER__timespec_t *pTsMax = XTIMER__Max(pTs1, pTs2);
    XTIMER__timespec_t *pTsMin = XTIMER__Min(pTs1, pTs2);
    long int diff = (int long)pTsMax->tv_nsec - (int long)pTsMin->tv_nsec;

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
    printf("Timespec: S( %ld ), NS ( %ld ) - (%p)\n", pTs->tv_sec, pTs->tv_nsec, pTs);
}




double  XTIMER__ConvertTsToSec(XTIMER__timespec_t *pTs)
{
    double sec = 0.0;
    
    sec = pTs->tv_nsec/1000000000.0;
    sec += pTs->tv_sec;
    return (sec);
}
