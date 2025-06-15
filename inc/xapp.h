
#ifndef XAPP_H
# define XAPP_H
# define _GNU_SOURCE /* allows the use of ppoll */
# include <poll.h>
# include <unistd.h>
# include <signal.h>
# include "../inc/xnet.h"

#define XAPP__ADDR_DST  "142.250.181.238"   ///< Google.com ip addr
#define XAPP__DEF_REQNBR         (4)
#define XAPP__SOCKFD_MAX_NBR     (1)
#define XAPP__RX_BUFSZ         (1024)
#define XAPP__DEF_ICMP_DATA_SIZE (56)
#define XAPP__TRUE               (1)
#define XAPP__INFO_PING_START   "PING %s (%s): %d data bytes\n"
#define XAPP__INFO_PING_SENT    "PING %s (%s): %d data bytes\n"

typedef enum XAPP__retCode_e
{
    XAPP__enRetCode_Ctor_CreateSockFailed = -355,
    XAPP__enRetCode_CreateIcmpHdr_Failed,
    XAPP__enRetCode_CreateIcmpHdr_MallocFailed,
    XAPP__enRetCode_CreateIcmpPayload_Failed,
    XAPP__enRetCode_CreateIcmpPayload_MallocFailed,
}   XAPP__retCode_t;




typedef struct XAPP_s
{
    struct addrinfo    hints;
    struct addrinfo   *pAddrInfo;
    /* parser to parse user input */
    /* id */
    /* seqNbr */
    /* ttl */
    ssize_t             pktCntTx;
    ssize_t             pktCntRx;
    ssize_t             pktMaxNbr;
    XPROTO_ICMP_t      *pIcmpHdr;
    int                 sockfd;
    short unsigned      pid;
    nfds_t              nfds;
    struct pollfd       fds[XAPP__SOCKFD_MAX_NBR];
    int                 ready;
    ssize_t             payloadLen;
    char unsigned      *pucPayload;
    short unsigned      seqNbr;
    ssize_t            datalenTx;      /* ip header + header + payload size */
    ssize_t            datalenRx;      /* ip header + header + payload size */
    struct sockaddr_in dstAddr;
    socklen_t          dstAddrLen;
    char unsigned      recvBuf[XAPP__RX_BUFSZ];
    /* timer */
}   XAPP_t;


#endif /* XAPP_H */


XAPP_t *XAPP__GetInstance(void);
int     XAPP__Ctor(XAPP_t * const me, char const * const strIpAddr);
int     XAPP__CreateIcmpHeader(XAPP_t * const me);
int     XAPP__CreateIcmpPayload(XAPP_t * const me);
int     XAPP__CreateIcmpPacket(XAPP_t * const me, XPROTO_ICMP__eMsgType_t msgType);
void    XAPP__TxPacket(XAPP_t * const me); 
void    XAPP__RxPacket(XAPP_t * const me);
void    XAPP__Destroy(XAPP_t * const me);
void    XAPP__SigIntHandler(int sig);
/*>
 * /usr/local/bin/ping -v -c 5 8.8.8.8
 * handle signal timer alarm 
 * parse user input
 * add timer to time sent packet
 * make socket non-blocking
 * create method app_createStats
 * create method app_showStats
 * manage error 
 * set root privilege
 * set the proper load data according to iputeils ref.
 * classify all the classes 
 * [x] handle signal ctrl + c 
 * */
