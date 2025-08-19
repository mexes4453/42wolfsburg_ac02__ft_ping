#ifndef XTIMER_H
# define XTIMER_H

# include <time.h>
# include <stdio.h>


typedef struct timespec XTIMER__timespec_t;


#define XTIMER__SECOND_TO_NS 1000000000L
#define XTIMER__SECOND_TO_MS 1000L

XTIMER__timespec_t *XTIMER__Max(XTIMER__timespec_t *pTs1, XTIMER__timespec_t *pTs2);
XTIMER__timespec_t *XTIMER__Min(XTIMER__timespec_t *pTs1, XTIMER__timespec_t *pTs2);
void                XTIMER__Diff(XTIMER__timespec_t *pTs1,
                                 XTIMER__timespec_t *pTs2,
                                 XTIMER__timespec_t *pRes);
void                XTIMER__Sum(XTIMER__timespec_t *pTs1,
                                XTIMER__timespec_t *pTs2,
                                XTIMER__timespec_t *pRes);

void    XTIMER__ShowTimeSpec(XTIMER__timespec_t *pTs);
double  XTIMER__ConvertTsToSec(XTIMER__timespec_t *pTs);
#endif
