#ifndef ICMP_ECHO_H
# define ICMP_ECHO_H

# include "../inc/xproto_icmp.h"
# include "../inc/xnet_utils.h"

# define ICMP_ECHO__STATIC_HDR_SZ (8)
# define ICMP_ECHO__MIN_DATA_SZ   (56)

#ifdef ICMP_ECHO__DEBUG
# define ICMP_ECHO__D_VALIDATE_RX_PKT "\n[ ICMP_ECHO::VALIDATE_RX_PKT ]"
# define ICMP_ECHO__D_IS_CHECKSUM_VALID "\n[ ICMP_ECHO::IS_CHECKSUM_VALID ]"
#endif

#define ICMP_ECHO__MSG_FMT_HDR_ECHO_REPLY "\ntype: %x \
                                           \ncode: 0x%x( %d )\
                                           \nchecksum: 0x%04x \
                                           \nid: 0x%x( %d ) \
                                           \nseqnbr: 0x%x( %d ) \
                                           \ndataLen: %ld \
                                           \n"

typedef enum ICMP_ECHO__retCode_e
{
    ICMP_ECHO__enRetCode_CreatePacket_InvPtr = -155,
    ICMP_ECHO__enRetCode_CreatePacket_MallocDataFailed,
    ICMP_ECHO__enRetCode_CreatePacket_Failed,
    ICMP_ECHO__enRetCode_CreateSerialPacket_Failed,
    ICMP_ECHO__enRetCode_CreateSerialPacketToNet_MallocFailed,
    ICMP_ECHO__enRetCode_CalcCheckSum_Failed,
    ICMP_ECHO__enRetCode_CalcCheckSum_MallocDataFailed,
    ICMP_ECHO__enRetCode_ValidateRxPkt_Failed,
    ICMP_ECHO__enRetCode_ValidateRxPkt_NullPtrArgs,
    ICMP_ECHO__enRetCode_ParseFromNet_Init,
    ICMP_ECHO__enRetCode_ParseFromNet_PdataMallocFailed,
    ICMP_ECHO__enRetCode_IsCheckSumValid_Init,
    ICMP_ECHO__enRetCode_ValidateRxPkt_ParseFromNetFailed,
    ICMP_ECHO__enRetCode_ValidateRxPkt_InvalidPktType,
    ICMP_ECHO__enRetCode_Failure = -1
}   ICMP_ECHO__retCode_t;


typedef struct ICMP_ECHO_s
{
    XPROTO_ICMP_t      icmp;
    short unsigned     identifier;
    short unsigned     seqnbr;
    /* Dynamic section */
    char unsigned     *pData;       /* Payload      */
    ssize_t            dataLen;     /* Payload size */
    ssize_t            hdrLen;      /* header size */
    ssize_t            totalPacketLen; /* header + payload size */
    char unsigned     *pPktChkSum;




#if 0
    void (*ParseFrom)(ICMP_ECHO_t * const, char unsigned *, ssize_t);
    void (*ShowDetails)(ICMP_ECHO_t * const);
    void (*Destroy)(ICMP_ECHO_t * const);
    int  (*CreatePacket)(ICMP_ECHO_t * const, ICMP_ECHO__eMsgType_t ,
                                                char unsigned *,
                                                ssize_t ,
                                                short unsigned ,
                                                short unsigned );
    short unsigned (*CalcCheckSum)(ICMP_ECHO_t * const);
#endif 
}   ICMP_ECHO_t;

/* Methods */
void ICMP_ECHO__Ctor(ICMP_ECHO_t * const me);
void ICMP_ECHO__ShowDetails(ICMP_ECHO_t *me);
void ICMP_ECHO__Destroy(ICMP_ECHO_t * const me);
int  ICMP_ECHO__CreatePacket(ICMP_ECHO_t * const me, 
                               ssize_t datalen,
                               short unsigned id,
                               short unsigned seqNbr);
int ICMP_ECHO__ValidateRxPkt(ICMP_ECHO_t * const me, char unsigned *buf, ssize_t bufSz);
int  ICMP_ECHO__ParseFromNet( ICMP_ECHO_t *me, char unsigned *buf,
                                                ssize_t bufSz );
int ICMP_ECHO__CalcCheckSum(ICMP_ECHO_t * const me);
int ICMP_ECHO__IsCheckSumValid(ICMP_ECHO_t * const me);


#endif /* ICMP_ECHO_H */