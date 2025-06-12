
#ifndef XPROTO_ICMP_H
# define XPROTO_ICMP_H
# include <sys/types.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "../inc/xnet.h"

#define XPROTO_ICMP__TYPE_ECHO        (8)
#define XPROTO_ICMP__TYPE_ECHO_REPLY  (0)
#define XPROTO_ICMP__CODE             (0)
#define XPROTO_ICMP__HDR_MIN_LEN      (8)
#define XPROTO_ICMP__PAYLOAD_SIZE     (56)
#define XPROTO_ICMP__RX_BUFSZ         (1024)
#define XPROTO_ICMP__MSG_FMT_HDR_ECHO_REPLY "\ntype: %x \
                                           \ncode: 0x%x( %d )\
                                           \nchecksum: 0x%04x \
                                           \nid: 0x%x( %d ) \
                                           \nseqnbr: 0x%x( %d ) \
                                           \ndataLen: %ld \
                                           \n"

typedef enum XPROTO_ICMP__retCode_e
{
    XPROTO_ICMP__enRetCode_CreatePktInvPtr = -255,
    XPROTO_ICMP__enRetCode_CreatePktMallocPtrData,
    XPROTO_ICMP__enRetCode_SendReqEcho_CreateSockFailed,
    XPROTO_ICMP__enRetCode_SendReqEcho_CreatePacketFailed,
    XPROTO_ICMP__enRetCode_Failure = -1
}   XPROTO_ICMP__retCode_t;

typedef struct XPROTO_ICMP_s XPROTO_ICMP_t;
/**
 * @brief ICMP struct RFC 792
 * 
 */
struct XPROTO_ICMP_s
{
    char unsigned      type;
    char unsigned      code;
    short unsigned     checksum;
    short unsigned     identifier;
    short unsigned     seqnbr;
    /* Dynamic section */
    char unsigned     *pData;       /* Payload      */
    ssize_t            dataLen;     /* Payload size */
    ssize_t            totalPacketLen; /* header + payload size */
    ssize_t            datalenTx;      /* ip header + header + payload size */
    ssize_t            datalenRx;      /* ip header + header + payload size */
    char unsigned      recvBuf[XPROTO_ICMP__RX_BUFSZ];
    struct sockaddr_in dstAddr;
    socklen_t          dstAddrLen;
    int                sockfd;




    /* Methods */
    void (*ParseFrom)(XPROTO_ICMP_t * const, char unsigned *, ssize_t);
    void (*ShowDetails)(XPROTO_ICMP_t * const);
    void (*Destroy)(XPROTO_ICMP_t * const);
    int  (*CreatePacket)(XPROTO_ICMP_t * const, char unsigned **,
                                                ssize_t,
                                                short unsigned,
                                                short unsigned);
    short unsigned (*CalcCheckSum)(XPROTO_ICMP_t * const);

};

void XPROTO_ICMP__Ctor(XPROTO_ICMP_t * const me);
void XPROTO_ICMP__ParseFrom( XPROTO_ICMP_t *me, char unsigned *buf,
                                                ssize_t bufSz );
void XPROTO_ICMP__ShowDetails(XPROTO_ICMP_t *me);
void XPROTO_ICMP__Destroy(XPROTO_ICMP_t * const me);
int  XPROTO_ICMP__CreatePacket(XPROTO_ICMP_t * const me, char unsigned **buf,
                                                         ssize_t datalen,
                                                         short unsigned id,
                                                         short unsigned seqNbr);
short unsigned XPROTO_ICMP__CalcCheckSum(XPROTO_ICMP_t * const me);
int  XPROTO_ICMP__SendEchoRequest(XPROTO_ICMP_t * const me, 
                                  struct addrinfo *pAddrInfo);

#endif /* XPROTO_ICMP_H*/