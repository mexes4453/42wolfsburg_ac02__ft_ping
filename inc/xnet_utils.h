
#ifndef XNET_UTILS_H
# define XNET_UTILS_H
//# define NDEBUG
# include <stdio.h>
# include <arpa/inet.h>
# include <assert.h>

# define XNET_UTILS__CHKSUM_CARRY_BITMASK (0x10000)
# define XNET_UTILS__LITTLE_ENDIAN

typedef enum XNET_UTILS__endianType_e
{
    XNET_UTILS__enEndianType_Host = 0,
    XNET_UTILS__enEndianType_Network
}   XNET_UTILS__endianType_t;

unsigned short XNET_UTILS__CalcCheckSum16(void *b, 
                                          int unsigned len, 
                                          XNET_UTILS__endianType_t endian);
void           XNET_UTILS__ShowAddrIpv4(int unsigned addr);
void           XNET_UTILS__Destroy(void *p);
void           XNET_UTILS__ShowPacketHex(char unsigned *buf, ssize_t bufSz);

#endif /* XNET_UTILS_H */
