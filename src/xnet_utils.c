#include "../inc/xnet_utils.h"

/*>
 * https://shorturl.at/yxOUj
 * Page: 171
 * */
unsigned short 
XNET_UTILS__CalcCheckSum16(void *b, int unsigned len,
                                    XNET_UTILS__endianType_t endian)
{

    unsigned short *buf = b;
    unsigned int    sum = 0;
    unsigned short  result=0;
    int counter = 0;
    for (sum = 0; len > 1; len -= 2)
    {
        counter++;
#ifdef XNET_UTILS__DEBUG
        printf("\ndata %d: 0x%04x", counter, *buf);
#endif
        if (endian)
        {
            sum += ntohs(*(buf));
        }
        else
        {
            sum += (*(buf));
        }
#ifdef XNET_UTILS__DEBUG
        printf("\nsum %d: 0x%04x", counter, sum);
#endif
        buf++;

        if (sum & (XNET_UTILS__CHKSUM_CARRY_BITMASK))
        {
            sum = (sum - XNET_UTILS__CHKSUM_CARRY_BITMASK + 1);
#ifdef XNET_UTILS__DEBUG
            printf("\nsum %d: 0x%04x", counter, sum);
#endif
        }

    }

    if (len == 1)
    {
        sum += *(unsigned char *)buf;
        counter++;
#ifdef XNET_UTILS__DEBUG
        printf("\ndata %d: 0x%04x", counter, *(unsigned char *)buf);
        printf("\nsum %d: 0x%04x", counter, sum);
#endif

    }

    if (sum & (XNET_UTILS__CHKSUM_CARRY_BITMASK))
    {
        sum = (sum - XNET_UTILS__CHKSUM_CARRY_BITMASK + 1);
#ifdef XNET_UTILS__DEBUG
        printf("\ndata %d: 0x%04x", counter, sum);
#endif
    }

#ifdef XNET_UTILS__DEBUG
    printf("\nsum %d: 0x%04x", counter, sum);
#endif
    result = ~sum;
#ifdef XNET_UTILS__DEBUG
    printf("\ndata %d: 0x%04x", counter, result);
#endif
    return result;
}




void XNET_UTILS__ShowAddrIpv4(int unsigned addr)
{
    printf("\naddr: 0x%x( %d ) -> %03d.%03d.%03d.%03d", 
              /* hex */           addr, 
              /* dec */           addr, 
              /* byte 3 */        (((addr&(0xff << 24)) >> 24) & 0xff),
              /* byte 2 */        (((addr&(0xff << 16)) >> 16) & 0xff),
              /* byte 1 */        (((addr&(0xff <<  8)) >>  8) & 0xff),
              /* byte 0 */        addr&(0xff));
}




void XNET_UTILS__ShowPacketHex(char unsigned *buf, ssize_t bufSz)
{
    int idxRow = 0; 
    if (!buf) return;

    printf("\nPacket => size( %ld )", bufSz);
    for (ssize_t x=0; x<bufSz; x++)
    {   
        /* round of 8 split */
        if ( (x - ((idxRow-1)*16)) == 8)
        { 
            printf("   ");
        } 
        

        /* newline */
        if (!(x%16))
        { 
            printf("\n%03d: => ", idxRow);
            ++idxRow;
        }

        /* print data (byte) */
        printf("%02x ", buf[x]);
    }
    printf("\n");
}





void XNET_UTILS__Destroy(void **p)
{
    if (!(p && (*p)))
    {
        return ;
    }
    free(*p);
    *p = NULL;
}