
#ifndef XERR_H
#define XERR_H

#include "xapp.h"
#include <errno.h>

typedef struct XERR_s
{
    int   id;
    int   code;
    char *strMsg;
} XERR_t;

void XERR__HandleError(int retCode, char *progTitle);

#endif /* XERR_H */