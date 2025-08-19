
#ifndef XPROTO_ICMP_H
# define XPROTO_ICMP_H
# include <sys/types.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "../inc/xnet.h"

#define XPROTO_ICMP__CODE             (0)
#define XPROTO_ICMP__HDR_MIN_LEN      (8)
#define XPROTO_ICMP__PAYLOAD_SIZE     (56)
#ifdef XNET__DEBUG
# define XPROTO_ICMP_D_CREATE_PACKET "\n[XPROTO_ICMP::CreatePacket]"
#endif
typedef enum XPROTO_ICMP__retCode_e
{
    XPROTO_ICMP__enRetCode_CreatePacket_InvPtr = -155,
    XPROTO_ICMP__enRetCode_CreatePktMallocPtrData,
    XPROTO_ICMP__enRetCode_CreatePacket_Failed,
    XPROTO_ICMP__enRetCode_CreateSerialPacket_Failed,
    XPROTO_ICMP__enRetCode_CreateSerialPacket_InvPtr,
    XPROTO_ICMP__enRetCode_Failure = -1
}   XPROTO_ICMP__retCode_t;




typedef enum XPROTO_ICMP__Type_e
{
    XPROTO_ICMP__enType_EchoReply = 0,
    XPROTO_ICMP__enType_Echo = 8,
    XPROTO_ICMP__enType_Timestamp = 13,
    XPROTO_ICMP__enType_TimestampReply,
    XPROTO_ICMP__enType_InfoRequest,
    XPROTO_ICMP__enType_InfoReply,
} XPROTO_ICMP__eType_t;




typedef enum XPROTO_ICMP__Code_e
{
    XPROTO_ICMP__enCode_EchoReply = 0,
    XPROTO_ICMP__enCode_Echo = 0,
    XPROTO_ICMP__enCode_Timestamp = 13,
    XPROTO_ICMP__enCode_TimestampReply,
    XPROTO_ICMP__enCode_InfoRequest,
    XPROTO_ICMP__enCode_InfoReply,
} XPROTO_ICMP__eCode_t;



typedef struct XPROTO_ICMP_s XPROTO_ICMP_t;



/**
 * @brief ICMP struct RFC 792
 * 
 */
struct XPROTO_ICMP_s
{
    /* Static section */
    char unsigned      type;
    char unsigned      code;
    short unsigned     checksum;
};

char unsigned XPROTO_ICMP__GetType(XPROTO_ICMP_t * const me);
#if 0
void XPROTO_ICMP__Ctor(XPROTO_ICMP_t * const me);
void XPROTO_ICMP__ParseFrom( XPROTO_ICMP_t *me, char unsigned *buf,
                                                ssize_t bufSz );
void XPROTO_ICMP__ShowDetails(XPROTO_ICMP_t *me);
void XPROTO_ICMP__Destroy(XPROTO_ICMP_t * const me);
int  XPROTO_ICMP__CreatePacket(XPROTO_ICMP_t * const me, 
                               XPROTO_ICMP__eMsgType_t msgType,
                               char unsigned *buf,
                               ssize_t datalen,
                               short unsigned id,
                               short unsigned seqNbr);
short unsigned XPROTO_ICMP__CalcCheckSum(XPROTO_ICMP_t * const me);
int  XPROTO_ICMP__CreateSerialPacket(XPROTO_ICMP_t * const me, char unsigned **ppPkt);
#endif

#endif /* XPROTO_ICMP_H*/