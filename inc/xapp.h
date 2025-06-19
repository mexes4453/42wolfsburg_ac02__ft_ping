
#ifndef XAPP_H
# define XAPP_H
# define _GNU_SOURCE /* allows the use of ppoll */
# include <unistd.h>
# include <signal.h>
# include "../inc/xnet.h"
# include "../inc/xtimer.h"
#include "../inc/xproto_ip.h"
#include "../inc/xproto_icmp.h"
#include "../inc/icmp_echo.h"

#define XAPP__ADDR_DST  "142.250.181.238"   ///< Google.com ip addr
#define XAPP__DEF_REQNBR          (4)
#define XAPP__SOCKFD_MAX_NBR      (1)
#define XAPP__RX_BUFSZ            (1024)
#define XAPP__DEF_ICMP_DATA_SIZE  (56)
#define XAPP__TRUE                (1)
#define XAPP__POLL_BLOCK_DURATION (1000)
#define XAPP__INFO_PING_START   "PING %s (%s): %d data bytes\n"
#define XAPP__MSG_FMT_RTT       "%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n"
#define XAPP__MSG_FMT_STATS     "--- %s ping statistics ---\
                                 \n %ld packets transmitted, %ld packets received, %ld packet loss \
                                 \n round-trip min/avg/max/stddev = %3.3f/%3.3f/%3.3f/%3.3f ms \n"

#ifdef XAPP__DEBUG
# define XAPP_D_VALIDATE_RX_PKT "\n[ XAPP::VALIDATE_RX_PKT ]"
#endif
typedef enum XAPP__retCode_e
{
    XAPP__enRetCode_Ctor_CreateSockFailed = -355,
    XAPP__enRetCode_CreateIcmpHdr_Failed,
    XAPP__enRetCode_CreateIcmpHdr_MallocFailed,
    XAPP__enRetCode_CreateIcmpPayload_Failed,
    XAPP__enRetCode_CreateIcmpPayload_MallocFailed,
    XAPP__enRetCode_ValidateRxPkt_MallocFailed,
    XAPP__enRetCode_ValidateRxPkt_InvalidIpFrame,
    XAPP__enRetCode_ValidateRxPkt_WrongIpFrameLen,
    XAPP__enRetCode_RxPacket_Init,
    XAPP__enRetCode_RxPacket_Failed,
    XAPP__enRetCode_TxPacket_Init,
}   XAPP__retCode_t;




typedef struct XAPP__statsRttRecord_s XAPP__statsRttRecord_t;
struct XAPP__statsRttRecord_s
{
    double                  sec;
    XAPP__statsRttRecord_t *next;
}; 




typedef struct XAPP__stats_e
{
    XTIMER__timespec_t     tRttMin;
    XTIMER__timespec_t     tRttMax;
    XTIMER__timespec_t     tRttAvg;
    XTIMER__timespec_t     tRttStdDev;
    XTIMER__timespec_t     tStart;
    XTIMER__timespec_t     tEnd;
    XTIMER__timespec_t     tPoll;
    XTIMER__timespec_t     tDuration;
    XAPP__statsRttRecord_t *pRttRec;
}   XAPP__stats_t;




typedef struct XAPP_s
{
    struct addrinfo    hints;
    struct addrinfo   *pAddrInfo;
    /* parser to parse user input */
    /* ttl */
    ssize_t             pktCntTx;
    ssize_t             pktCntRx;
    ssize_t             pktMaxNbr;
    ICMP_ECHO_t        *pIcmpHdrTx;
    ICMP_ECHO_t        *pIcmpHdrRx;
    XPROTO_IP_t        *pIpHdr;
    int                 sockfd;
    short unsigned      pid;
    nfds_t              nfds;
    struct pollfd       fds[XAPP__SOCKFD_MAX_NBR];
    int                 ready;
    ssize_t             payloadLen;
    short unsigned      seqNbr;
    ssize_t             datalenTx;      ///< ip header + icmp header + payload size
    ssize_t             datalenRx;      ///< ip header + icmp header + payload size
    struct sockaddr_in  dstAddr;
    socklen_t           dstAddrLen;
    char unsigned       recvBuf[XAPP__RX_BUFSZ];
    XAPP__stats_t       stats;
}   XAPP_t;




XAPP_t *XAPP__GetInstance(void);
int     XAPP__Ctor(XAPP_t * const me, char const * const strIpAddr);
int     XAPP__CreateIcmpHeader(XAPP_t * const me);
int     XAPP__CreateIcmpPacket(XAPP_t * const me, XPROTO_ICMP__eType_t msgType);
int     XAPP__TxPacket(XAPP_t * const me); 
int     XAPP__RxPacket(XAPP_t * const me);
void    XAPP__Destroy(XAPP_t * const me);
int     XAPP__ValidateRxPkt(XAPP_t * const me);
void    XAPP__GetTimeOfStart(XAPP_t * const me);
void    XAPP__GetTimeOfEnd(XAPP_t * const me);
void    XAPP__StatsComputeRtt(XAPP_t * const me);
void    XAPP__StatsUpdate(XAPP_t * const me);
void    XAPP__StatsShowRtt(XAPP_t * const me);
void    XAPP__StatsShowSummary(XAPP_t * const me);
void    XAPP__SigIntHandler(int sig);
void    XAPP__Wait(XAPP_t * const me);

#endif /* XAPP_H */
/*>
 * parse user input
 * verify the address on rxPacket
 * verify the sequence on rxPacket
 * verify the message type
 * create method app_StatsComputeSummary
 * create method app_StatsShowSummary
 * create method app_StatsShowRtt
 * manage error 
 * set root privilege
 * set the proper load data according to iputeils ref.
 * classify all the classes 
 * ttl update
 * [ x ] - handle signal ctrl + c 
 * [ x ] - add timer to time sent packet
 * [ x ] - make socket non-blocking
 * [ x ] - create method app_StatsUpdate
 * [ x ] - verify the checksum
 * == info ==
 * /usr/local/bin/ping -v -c 5 8.8.8.8
 * sudo setcap cap_net_raw+ep ./my_ping_program
 * Reversible (via sudo setcap -r ./my_ping_program)
 * */
