
#include "../inc/xapp.h"


void XAPP__Ctor(XAPP_t * const me, char const * const strIpAddr)
{
    int retCode = 0;

    memset( (void *)me, 0, sizeof(XAPP_t));
    
    /* Initialise hints attribute */
    XNET__InitAddrInfo(&(me->hints), AF_INET, SOCK_RAW, IPPROTO_ICMP, AI_CANONNAME);

    /* setup and fetch addr info */
    retCode = getaddrinfo(strIpAddr, NULL, &(me->hints), &(me->pAddrInfo));
    if (retCode != 0)
    {
        fprintf(stderr, "%s\n", gai_strerror(retCode));
    }
    else
    {
        XNET__ShowAddrInfo(me->pAddrInfo);
    }
}