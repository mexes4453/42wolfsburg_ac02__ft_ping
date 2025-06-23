#ifndef XPROTO_IP_H
# define XPROTO_IP_H
# include <sys/types.h>
# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include <arpa/inet.h>
# include "../inc/xnet_utils.h"

#define XPROTO_IP__HDR_MIN_LEN    (20)
#define XPROTO_IP__MSG_FMT_HDR "\nver_ihl: %x \
                              \nver: %d \
                              \nihl: %d \
                              \ntos: 0x%x( %d )\
                              \ntotal_len: 0x%x( %d )\
                              \nid: 0x%x( %d ) \
                              \nflag_fragOffset: %x \
                              \nflag: %x \
                              \nfragOffset: %x \
                              \nttl: %x( %d ) \
                              \nprotocol: %x \
                              \nheader checksum: %x \
                              \naddrSrc: %x \
                              \naddrDst: %x \
                              \n"

typedef enum XPROTO_IP__retCode_s
{
    XPROTO_IP__enRetCode_InvalidCheckSum = -255,
    XPROTO_IP__enRetCode_ParseFrom_InvFrameLen, 
    XPROTO_IP__enRetCode_ParseFrom_OptionMallocFailed,
    XPROTO_IP__enRetCode_ParseFrom_DataMallocFailed,
    XAPP__enRetCode_ValidateRxPkt_ParseFailed,
    XAPP__enRetCode_ValidateRxPkt_PtrIpHdrMallocFailed,
    XPROTO_IP__enRetCode_Success = 0,
}   XPROTO_IP__retCode_t;



typedef struct XPROTO_IP_s XPROTO_IP_t;
/**
 * @brief Internet Header struct RFC 791
 * 
 */
struct XPROTO_IP_s
{
    /* Version & Header length */
    union
    {
        unsigned char ver_hdrLen;
        struct 
        {
#if defined (XNET_UTILS__LITTLE_ENDIAN)
            unsigned char hdrLen:4; /* bits 3-0 : low */
            unsigned char ver:4;    /* bits 7-4 : high */
#else  /* XNET__BIG_ENDIAN */
            unsigned char ver:4;    /* bits 7-4 : high */
            unsigned char hdrLen:4; /* bits 3-0 : low */
#endif /* XNET__LITTLE_ENDIAN */
        };
    };

    char unsigned  tos;         /* 1 byte: Type of service */
    short unsigned totalLength; /* 2 byte: Total Length    */
    short unsigned id;          /* 2 byte: Identification  */

    /* flag & fragment offset */
    union
    {
        short unsigned flag_fragOffset;
        struct 
        {
#if defined (XNET_UTILS__LITTLE_ENDIAN)
            short unsigned fragOffset: 13; /* bits 12-0  : low  */
            short unsigned flag: 3;        /* bits 15-13 : high */
#else  /* XNET__BIG_ENDIAN */
            short unsigned flag: 3;        /* bits 15-13 : high */
            short unsigned fragOffset: 13; /* bits 12-0  : low  */
#endif /* XNET__LITTLE_ENDIAN */
        };
    };
    
    char unsigned  ttl;          /* 1 byte: Time to live        */
    char unsigned  protocol;     /* 1 byte: Protocol            */
    short unsigned hdrChkSum;    /* 2 byte: Header checksum     */
    int unsigned   addrSrc;      /* 4 byte: Address source      */      
    int unsigned   addrDst;      /* 4 byte: Address destination */
    /* Dynamic section */
    char unsigned *pOption;      /* Variable: Option            */
    char unsigned *pData;        /* Variable: Data              */
    ssize_t        dataLen;      /* Variable: Data              */
    char unsigned *pPktChkSum;        /* Variable: Data              */
    /* Methods */
    void                 (*ShowDetails)(XPROTO_IP_t * const);
    int                  (*ParseFrom)(XPROTO_IP_t * const, char unsigned *,
                                                           ssize_t);
    void                 (*Destroy)(XPROTO_IP_t * const);
    short unsigned       (*CalcCheckSum)(XPROTO_IP_t * const);
    XPROTO_IP__retCode_t (*IsCheckSumValid)(XPROTO_IP_t * const);
};



void                 XPROTO_IP__Ctor(XPROTO_IP_t * const me);
void                 XPROTO_IP__ShowDetails(XPROTO_IP_t * const me);
int                  XPROTO_IP__ParseFrom(XPROTO_IP_t * const me, 
                                          char unsigned      *buf,
                                          ssize_t             bufSz);
void                 XPROTO_IP__Destroy(XPROTO_IP_t * const me);
short unsigned       XPROTO_IP__CalcCheckSum(XPROTO_IP_t * const me);
XPROTO_IP__retCode_t XPROTO_IP__IsCheckSumValid(XPROTO_IP_t * const me);

                                           

#endif /* XPROTO_IP_H*/
