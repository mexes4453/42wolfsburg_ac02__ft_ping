#include "../inc/xapp.h"

int main(int argc, char *argv[])
{
    int                idxArg;
    int                retCode;
    XAPP_t             appVar;
    XPROTO_ICMP_t      icmpHdr;

    /* Initialise the variables */
    idxArg = 1;
    retCode = 0;
    XAPP__Ctor(&appVar, XAPP__ADDR_DST);
    XPROTO_ICMP__Ctor(&icmpHdr);

    retCode = XPROTO_ICMP__SendEchoRequest(&icmpHdr, appVar.pAddrInfo);
    printf("\nretcode(%d);\n", retCode);







#if 1
    int opt = 1;
    int optind =1, optopt=1;
    if (argc > 1)
    {
        //ipDst = argv[idxArg];
        while ( (opt = getopt(argc, argv, ":v:x")) != -1) 
        {
            printf("%c - %s %s", opt, argv[optind], argv[optopt]);
            printf("%s\n", argv[idxArg]);
            ++idxArg;
        }
    }
#endif


    freeaddrinfo(appVar.pAddrInfo);
    icmpHdr.Destroy(&icmpHdr);
    return (retCode);
}
