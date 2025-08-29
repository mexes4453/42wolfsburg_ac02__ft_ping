
#ifndef XERR_H
# define XERR_H

# include "xapp.h"
# include <errno.h>
# define XERR__NIL ""

typedef struct XERR_s
{
    int   code;
    char *strMsg;
}   XERR_t;

void XERR__HandleError(int retCode, char *progTitle);

#endif /* XERR_H */