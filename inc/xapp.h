
#ifndef XAPP_H
# define XAPP_H
# include "../inc/xnet.h"

#define XAPP__ADDR_DST  "142.250.181.238"   ///< Google.com ip addr
typedef struct XAPP_s
{
    struct addrinfo    hints;
    struct addrinfo   *pAddrInfo;

}    XAPP_t;


#endif /* XAPP_H */


void XAPP__Ctor(XAPP_t * const me, char const * const strIpAddr);