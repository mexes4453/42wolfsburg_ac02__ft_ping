
#ifndef XAPP_H
# define XAPP_H
# define _GNU_SOURCE /* allows the use of ppoll */
# include <unistd.h>
# include <signal.h>
# include <math.h>
# include "../inc/xnet.h"
# include "../inc/xtimer.h"
# include "../inc/xproto_ip.h"
# include "../inc/xproto_icmp.h"
# include "../inc/icmp_echo.h"

#define XAPP__ADDR_DST  "142.250.181.238"   ///< Google.com ip addr
#define XAPP__ADDR_NAME  "www.google.com"   ///< Google.com ip addr
#define XAPP__DEF_REQNBR          (4)
#define XAPP__SOCKFD_MAX_NBR      (1)
#define XAPP__RX_BUFSZ            (1024)
#define XAPP__BUFSZ_TXTSTR        (512)
#define XAPP__DEF_ICMP_DATA_SIZE  (56)
#define XAPP__TRUE                (1)
#define XAPP__POLL_BLOCK_DURATION (1)    ///< 1 second
#define XAPP__INFO_PING_START     "PING %s (%s): %d data bytes"
#define XAPP__MSG_FMT_RTT1         "%ld bytes from %s: "
#define XAPP__MSG_FMT_RTT2         "icmp_seq=%d ttl=%d time=%.3f ms\n"
#define XAPP__MSG_FMT_STATS_TITLE "--- %s ping statistics ---"
#define XAPP__MSG_FMT_STATS       "\n %ld packets transmitted, %ld packets received, %d%c packet loss \
                                   \n round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms \n"

#ifdef XAPP__DEBUG
# define XAPP_D_VALIDATE_RX_PKT "\n[ XAPP::VALIDATE_RX_PKT ]"
#endif
typedef enum XAPP__retCode_e
{
    XAPP__enRetCode_Ctor_CreateSockFailed = -355,
    XAPP__enRetCode_GetOpt_Init,
    XAPP__enRetCode_Ctor_GetOptFailed,
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
    XAPP__enRetCode_TxPacket_SendToFailed,
}   XAPP__retCode_t;




typedef struct XAPP__statsRttRecord_s XAPP__statsRttRecord_t;
struct XAPP__statsRttRecord_s
{
    double                  sec;
    XAPP__statsRttRecord_t *next;
}; 




typedef struct XAPP__stats_e
{
    XTIMER__timespec_t      tRttMin;
    XTIMER__timespec_t      tRttMax;
    XTIMER__timespec_t      tRttAvg;
    XTIMER__timespec_t      tRttStdDev;
    XTIMER__timespec_t      tStart;
    XTIMER__timespec_t      tEnd;
    XTIMER__timespec_t      tPoll;
    XTIMER__timespec_t      tDuration;
    XAPP__statsRttRecord_t *pRttRec;
}   XAPP__stats_t;


typedef struct XAPP__opt_s
{

    uint8_t optUsage:1; 
    uint8_t optVerbose:1; 
    uint8_t optReserved:6;
    size_t  optPktCnt;
    size_t  optTimeToLive;
    char    *pOptHostAddr;
}   XAPP__opt_t;



typedef struct XAPP_s
{
    struct addrinfo    hints;
    struct addrinfo   *pAddrInfo;
    char              *strIpAddr;
    XAPP__opt_t        option;          ; /* parser to parse user input */
    /* ttl */
    ssize_t             pktCntTx;
    ssize_t             pktCntRx;
    ssize_t             pktMaxNbr;
    ICMP_ECHO_t        *pIcmpHdrTx;
    ICMP_ECHO_t        *pIcmpHdrRx;
    XPROTO_IP_t        *pIpHdr;
    int                 sockfd;
    short unsigned      pid;
    timer_t             timerId;
    struct sigevent     timerEvt;
    struct itimerspec   timerVal;
    struct itimerspec   timerValRem;
    struct sigaction    sa;
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
    char                strText[XAPP__BUFSZ_TXTSTR];
    XAPP__stats_t       stats;
}   XAPP_t;




XAPP_t *XAPP__GetInstance(void);
int     XAPP__Ctor( XAPP_t * const me, char const * const strIpAddr, int argc, char *argv[]);
int     XAPP__HandleOpt( char *strOpt, char *argv[], int *pArgIdx);
int     XAPP__ProcessOptionChar( char *pChr, char *argv[], int *pArgIdx);
int     XAPP__HandleUserInput( XAPP_t * const me, int argc, char *argv[]);
int     XAPP__CreateIcmpHeader( XAPP_t * const me);
int     XAPP__CreateIcmpPacket( XAPP_t * const me, XPROTO_ICMP__eType_t msgType);
int     XAPP__TxPacket( XAPP_t * const me); 
int     XAPP__RxPacket( XAPP_t * const me);
void    XAPP__Destroy( XAPP_t * const me);
int     XAPP__ValidateRxPkt( XAPP_t * const me);
void    XAPP__GetTimeOfStart( XAPP_t * const me);
void    XAPP__GetTimeOfEnd( XAPP_t * const me);
void    XAPP__StatsComputeRtt( XAPP_t * const me);
void    XAPP__StatsUpdate( XAPP_t * const me);
void    XAPP__StatsShowRtt( XAPP_t * const me);
void    XAPP__StatsShowSummary( XAPP_t * const me);
void    XAPP__StatsComputeSummary( XAPP_t * const me);
void    XAPP__StatsComputeRttAvg( XAPP_t * const me);
void    XAPP__StatsComputeRttStDev( XAPP_t * const me);
void    XAPP__Wait( XAPP_t * const me);
void    XAPP__SigHandler( int sig, siginfo_t *si, void *uc);
void    XAPP__ShowStartMsg( XAPP_t * const me);

#endif /* XAPP_H */
/*>
 * verify the address on rxPacket
 * verify the sequence on rxPacket
 * verify the message type
 * manage error state
 * set root privilege
 * set the proper load data according to iputeils ref.
 + --- update packet being sent with sequence number
 + --- seq should start from 0
 *
 * [ x ] - handle verbose flag and usage flag
 * [ x ] - parse user input to option.
 * [ x ] - add timer to time sent packet; wake up thread with sig alarm 
 * [ x ] - manage dynamic memory
 * [ x ] - create method app_StatsComputeSummary
 * [ x ] - create method app_StatsShowSummary
 * [ x ] - create method app_StatsShowRtt
 * [ x ] - ttl update
 * [ x ] - handle signal ctrl + c 
 * [ x ] - make socket non-blocking
 * [ x ] - create method app_StatsUpdate
 * [ x ] - verify the checksum
 * ==========
 *    INFO 
 * ==========
 * /usr/local/bin/ping -v -c 5 8.8.8.8
 * sudo setcap cap_net_raw+ep ./my_ping_program
 * Reversible (via sudo setcap -r ./my_ping_program)
 * */
